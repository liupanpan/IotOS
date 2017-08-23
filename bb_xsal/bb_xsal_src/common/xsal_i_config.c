/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_config.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: XSAL global configuration parameters.
 *
 * -----------------------------------------------------------------------*/


#include "xsal_i_config.h"


/** Identifier of XSAL Application
 */
SAL_App_Id_T  SAL_I_App_Id;


/** Maximum number of XSAL threads the application can have.
 *  This variable also defines maximum value for the XSAL thread identifier.
 */
uint32_t  SAL_I_Max_Number_Of_Threads = 5;


/** First XSAL Thread ID which can be used during creation of the
 *  thread with "unknown" ID.
 */
SAL_Thread_Id_T SAL_First_Unknown_Thread_Id;


/** Defines maximum number of XSAL timers per process.
 */
uint32_t  SAL_I_Max_Number_Of_Timers;


/** Maximum number of buffer pools, application will have.
 */
uint32_t  SAL_I_Max_Number_Of_Buffer_Pools;


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 14-Apr-2015 Dan Carman
 *    Added intializer to SAK_I_Max_Number_Of_Threads to fix C++ race condition
 *    of declaring thread object before SAL_Config is run.
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
