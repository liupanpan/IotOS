/* -------------------------------------------------------------------------
 *
 * File:        xsal_get_thread_priority.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Get_Thread_Priority() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"


#if defined (XSAL_POSIX)

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#if defined (LINUX)
#include <sys/resource.h>
#endif
#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_THREAD_MODULE_ID, 4); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/


/** Function SAL_Lock_Mutex are not defined in XSAL_LIGHT 
 *  external interface but are used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);

/* -----------------------
 *  Function Definitions
 * ----------------------- */


bool_t SAL_I_Get_Thread_Priority(
   SAL_Thread_Id_T thread_id, 
   SAL_Priority_T* priority)
{
#if defined (XSAL_POSIX)

   SAL_Int_T policy;
   struct sched_param sched_p;

   SAL_Int_T status = pthread_getschedparam(
      SAL_I_Thread_Table[thread_id].os_tid, 
      &policy, 
      &sched_p);

   if (status == EOK)
   {
#if defined (LINUX)
      if (SCHED_OTHER == policy)
      {
         /** using CFS - Get NICE level */
         int nice_level;

         nice_level = getpriority(PRIO_PROCESS, SAL_I_Thread_Table[thread_id].os_pid);
         *priority = (SAL_REAL_TIME_PRIORITY/2) - nice_level;
      }
      else  /* using RR */
      {
          *priority = (SAL_REAL_TIME_PRIORITY - 1) + sched_p.sched_priority;
      }
#else
      *priority = sched_p.sched_priority;
#endif
   }
   else
   { 
      PBC_Failed_2("SAL_I_Get_Thread_Priority failed/pthread_getschedparam [%d], %s", 
                   (int)thread_id,  strerror(status));
   }
   return (bool_t)(status == EOK);

#elif defined (WIN32)

   *priority = GetThreadPriority(SAL_I_Thread_Table[thread_id].os_tid);
   return (bool_t)(*priority != THREAD_PRIORITY_ERROR_RETURN);

#endif /* WIN32 */
}



/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Get_Thread_Priority(
   SAL_Thread_Id_T thread_id,
   SAL_Priority_T* priority)
{
   bool_t status;

   PBC_Require(thread_id >= 0, "Negative thread id");
   PBC_Require((size_t)thread_id <= SAL_I_Max_Number_Of_Threads, "Thread id > max");

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      if (SAL_I_Thread_Table[thread_id].thread_id != -1)
      {
         status = SAL_I_Get_Thread_Priority(thread_id, priority);
      }
      else
      {
         status = false;
         Tr_Fault_1("Get_Thread_Priority: thread %d is not running", (int)thread_id);
      }
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);

   }
   else
   {
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
