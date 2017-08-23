/* -------------------------------------------------------------------------
 *
 * File:        xsal_create_timer.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Create_Timer() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_TIME_MODULE_ID, 0); /* Identify file for trace/assert purposes */

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

/** Function SAL_Lock_Mutex is not defined in XSAL Light public interface
 *  (in xsal.h file) but is internally used in XSAL Light implementation.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);

/* -----------------------------
 *  Public Function Definition
 * ----------------------------- */

bool_t SAL_Create_Timer(SAL_Event_Id_T event_id, SAL_Timer_Id_T* timer_id)
{
   bool_t status = false;

   PBC_Require(timer_id != NULL, "Null timer");

   if (SAL_Lock_Mutex(&SAL_I_Timers_Mutex))
   {
      SAL_I_Timer_T* timer = SAL_I_Timers_Free_List;
      if (timer != NULL)
      {
         SAL_I_Timers_Free_List = timer->next_thread_timer;
      }
      (void) SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);

      if (timer != NULL)
      {
         PBC_Require(timer->event_id == INVALID_TIMER_EVENT, "invalid timer");

         timer->event_id = event_id;
         timer->thread_id = SAL_I_Get_Thread_Id();
         status = SAL_I_Create_Timer(timer);
         if (status)
         {
            if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
            {
               SAL_I_Bind_Timer_To_Thread(timer);
               (void) SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);

               *timer_id = timer - SAL_I_Timers;
            }
            else
            {
               /* Returns timer to the free list */
               if (SAL_Lock_Mutex(&SAL_I_Timers_Mutex))
               {
                  timer->next_thread_timer = SAL_I_Timers_Free_List;
                  SAL_I_Timers_Free_List = timer;
                  (void) SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
               }
               status = false;
            }
         }
      }
      else
      {
         Tr_Fault("Too many timers were created.\nIncrease max number of timer is the SAL configuration (see SAL_Init)");
      }
   }
   else
   {
      PBC_Failed("Create timer");
   }
   return status;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
