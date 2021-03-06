/* -------------------------------------------------------------------------
 *
 * File:        xsal_start_timer_ex.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman(at)delphi.com)
 *
 * Description: Implementation of the SAL_Start_Timer() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_TIME_MODULE_ID, 3); /* Identify file for trace/assert purposes */

/* -----------------------------
 *  Public Function Definition
 * ----------------------------- */


void  SAL_Start_Timer_Ex(
   SAL_Timer_Id_T timer_id,
   uint32_t interval_msec,
   bool_t is_periodic,
   uintptr_t param)
{
   SAL_I_Timer_T* timer;

   /** Do not start non-existent timer or timer created by another thread
   */
   PBC_Require(timer_id >= 0, "Negative timer id");
   PBC_Require((size_t)timer_id < SAL_I_Max_Number_Of_Timers, "Timer id > max");

   timer = &SAL_I_Timers[timer_id];

   PBC_Require(timer->event_id  != INVALID_TIMER_EVENT, "Invalid event id");

   if (interval_msec > 0)
   {
      SAL_I_Start_Timer(timer, interval_msec, is_periodic, true, param);
   }
   else
   {
      SAL_I_Stop_Timer(timer);
      SAL_I_Post_Timer_Event(timer->event_id, timer->thread_id, true, param);
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman(at)delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
