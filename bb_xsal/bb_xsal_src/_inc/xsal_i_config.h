/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_config.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: SAL library configuration.
 *
 * -------------------------------------------------------------------------*/

#ifndef XSAL_I_CONFIG_H
#define XSAL_I_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"


/** Identifier of SAL Application
 */
extern SAL_App_Id_T  SAL_I_App_Id;


/** Maximum number of SAL threads the application can have.
 *  This variable also defines maximum value for the SAL thread identifier.
 */
extern uint32_t  SAL_I_Max_Number_Of_Threads;


/** First SAL Thread ID which can be used during creation of the
 *  thread with "unknown" ID.
 */
extern SAL_Thread_Id_T SAL_First_Unknown_Thread_Id;


/** Defines maximum number of SAL timers per process
 */
extern uint32_t  SAL_I_Max_Number_Of_Timers;


/** Maximum number of buffer pools, application will have
 */
extern uint32_t  SAL_I_Max_Number_Of_Buffer_Pools;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_CONFIG_H */


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
