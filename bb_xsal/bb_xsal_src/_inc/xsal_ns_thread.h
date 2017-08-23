/* -------------------------------------------------------------------------
 *
 * File:        xsal_ns_thread.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Sepcification of the Name Server thread
 *
 * -------------------------------------------------------------------------*/

#ifndef XSAL_NS_THREAD_H
#define XSAL_NS_THREAD_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"


/**
 *  Initialize Name Server internal data.
 *
 *  @return false on failure
 */
bool_t SAL_Name_Server_Init(void);


/**
 *  Start Name Server Thread.
 *
 *  @param [in] thread_attr Attributes for name server thread
 *
 *  @return ID of thread or -1 on failure
 */
SAL_Thread_Id_T SAL_Start_Name_Server_Thread(SAL_Thread_Attr_T * thread_attr);

/**
 *  Destroy Name Server data.
 */
void SAL_Name_Server_Destroy(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_NS_THREAD_H */


/* -------------------------------------------------------------------------
 *
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *    Initial revision.
 *
 * -----------------------------------------------------------------------*/
