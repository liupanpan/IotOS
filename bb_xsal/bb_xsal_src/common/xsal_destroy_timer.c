/* -------------------------------------------------------------------------
 *
 * File:        xsal_destroy_timer.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Destroy_Timer() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_TIME_MODULE_ID, 1); /* Identify file for trace/assert purposes */


/* ---------------------------
 *  Local Function Prototypes
 * --------------------------- */


/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);


/* -----------------------------
 *  Public Function Definition
 * ----------------------------- */


void SAL_Destroy_Timer(SAL_Timer_Id_T timer_id)
{
   SAL_I_Timer_T* timer;
   SAL_Thread_Id_T tid = SAL_I_Get_Thread_Id();

   /** Do not destroy non-existent timer or timer created by another thread
   */
   PBC_Require(timer_id >= 0, "Negative timer id");
   PBC_Require((size_t)timer_id < SAL_I_Max_Number_Of_Timers, "Timer id > max");

   timer = &SAL_I_Timers[timer_id];

   PBC_Require(timer->event_id  != INVALID_TIMER_EVENT, "Invalid event id");
   PBC_Require_1(tid == timer->thread_id, "Cannot destroy timer owned by another thread %d", timer->thread_id);
 
   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      SAL_I_Unbind_Timer_From_Thread(timer);
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);

      SAL_I_Destroy_Timer(timer);
      if (SAL_Lock_Mutex(&SAL_I_Timers_Mutex))
      {
         timer->event_id = INVALID_TIMER_EVENT;
         timer->next_thread_timer = SAL_I_Timers_Free_List;
         SAL_I_Timers_Free_List = timer;
         (void)SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
      }
      else
      {
         PBC_Failed("Failed to lock timers mutex");
      }
   }
   else
   {
      PBC_Failed("Failed to lock thread table mutex");
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
