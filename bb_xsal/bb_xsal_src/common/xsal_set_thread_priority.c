/* -------------------------------------------------------------------------
 *
 * File:        xsal_set_thread_priority.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Set_Thread_Priority() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_THREAD_MODULE_ID, 10); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/


/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but it is used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Set_Thread_Priority(
   SAL_Thread_Id_T thread_id, 
   SAL_Priority_T priority)
{
   bool_t status;
   PBC_Require(thread_id >= 0, "Negative thread id");
   PBC_Require((size_t)thread_id <= SAL_I_Max_Number_Of_Threads, "Thread id > max");

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      status = SAL_I_Set_Thread_Priority(thread_id, priority);
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);

#if !defined(XSAL_NO_ADMIN_RIGHTS)
      /* Some systems do not allow changing priority without admin rights.
       * In this case the post condition should not be checked.
       */
      PBC_Require_1(status, "Priority change to %d failed", (int)priority);
#endif /* XSAL_NO_ADMIN_RIGHTS */
   }
   else
   {
       PBC_Failed("Failed to lock thread table mutex");
      status = false;
   }
   return status;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
