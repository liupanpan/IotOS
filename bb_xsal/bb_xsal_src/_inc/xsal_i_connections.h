/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_connections.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the internal interface of the SAL Send.
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_CONNECTIONS_H
#define XSAL_I_CONNECTIONS_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal.h"


/**
 *  List of connections to other SAL processes.
 *  This table keeps connections to router threads in other applications.
 *  When any thread sends a message to specified application (to its  
 *  Router Thread), then it makes connection to it (only once) and stores 
 *  connection ID in the table.
 *  Size of the table is SAL_MAX_APPS+1 because application IDs 
 *  starts from 1 (not 0).
 *  Access to table is synchronized by SAL_I_Connection_Id_Mutex mutex.
 */
extern SAL_Connection_T SAL_I_Connection_Id[SAL_MAX_APPS + 1]; 


/** Mutex for synchronization access to SAL_I_Connection_Id table
 */
extern SAL_Mutex_T SAL_I_Connection_Id_Mutex;


#if defined(XSAL)

bool_t SAL_I_Init_Connection_Module(void);
void SAL_I_Deinit_Connection_Module(void);
SAL_Connection_T* SAL_Get_Connection(SAL_App_Id_T app_id);

#else

#define SAL_I_Init_Connection_Module() true
#define SAL_I_Deinit_Connection_Module()

#endif /* XSAL */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_CONNECTIONS_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/

