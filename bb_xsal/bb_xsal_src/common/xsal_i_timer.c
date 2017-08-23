/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_timer.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Definition of XSAL Timers functionality.
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_assert.h"
#include "xsal_i_timer.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#if defined (XSAL)
#include "xsal_i_event_property.h"
#endif /* XSAL */

#include <sys/timeb.h>

EM_FILENUM(XSAL_TIME_MODULE_ID, 6); /* Identify file for trace/assert purposes */

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

#if defined(XSAL_LIGHT)
/** Function SAL_Lock_Mutex is not defined in XSAL Light public interface
 *  (in xsal.h file) but is internally used in XSAL Light implementation.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
#endif /* XSAL_LIGHT */

/* ---------------------
 *  Object definitions
 * --------------------- */

/** Pointer to the SAL Timer's table
 */
SAL_I_Timer_T* SAL_I_Timers;

/** Head of free timers list.
 */
SAL_I_Timer_T* SAL_I_Timers_Free_List;

/** Mutex to synchronize the SAL Timer's table access
 */
SAL_Mutex_T SAL_I_Timers_Mutex;

/* -----------------------
 *  Function definitions
 * ----------------------- */

void SAL_I_Bind_Timer_To_Thread(SAL_I_Timer_T* timer)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[timer->thread_id];

   timer->prev_thread_timer = NULL;
   timer->next_thread_timer = thread_attr->thread_timers;
   if (thread_attr->thread_timers != NULL)
   {
      thread_attr->thread_timers->prev_thread_timer = timer;
   }
   thread_attr->thread_timers = timer;
}

void SAL_I_Unbind_Timer_From_Thread(SAL_I_Timer_T* timer)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[timer->thread_id];

   (void) SAL_Lock_Mutex(&SAL_I_Timers_Mutex);
   if (timer->next_thread_timer != NULL)
   {
      timer->next_thread_timer->prev_thread_timer = timer->prev_thread_timer;
   }

   if (timer->prev_thread_timer != NULL)
   {
      timer->prev_thread_timer->next_thread_timer = timer->next_thread_timer;
   }
   else
   {
      thread_attr->thread_timers = timer->next_thread_timer;
   }
   (void)SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
}

void SAL_I_Destroy_And_Unbind_Timers_From_Thread(SAL_I_Thread_Attr_T* thread_attr)
{
   SAL_I_Timer_T* timer;

   (void) SAL_Lock_Mutex(&SAL_I_Timers_Mutex);

   timer = thread_attr->thread_timers;

   while (timer != NULL)
   {
      SAL_I_Timer_T* to_delete = timer;
      timer = timer->next_thread_timer;
      SAL_I_Destroy_Timer(to_delete);
      to_delete->next_thread_timer = SAL_I_Timers_Free_List;
      SAL_I_Timers_Free_List = to_delete;
   }
   thread_attr->thread_timers = NULL;
   (void) SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
}

void SAL_I_Post_Timer_Event(SAL_Event_Id_T event_id, SAL_Thread_Id_T thread_id, bool_t use_param, uintptr_t param)
{
   bool_t send_status;
   SAL_Message_T msg;
   struct timeb current_time;
   SAL_Message_Dropped_Reason_T err;
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[thread_id];

#if defined (XSAL)
   bool_t
      is_urgent =
         (bool_t) (((int16_t) event_id >= SAL_FIRST_EVENT_ID) && ((int16_t) event_id < SAL_LAST_EVENT_ID)) ? (SAL_I_Is_Urgent(thread_id, (int16_t)event_id))
            : false;
#else /* XSAL_LIGHT */
   bool_t is_urgent = false;
#endif /* XSAL && XSAL_LIGHT */

   (void) ftime(&current_time);

   msg.event_id = (SAL_Event_Id_T) (int16_t) event_id;
#if defined(XSAL)
   msg.object_id = (int16_t) ((uint32_t) event_id >> 16);
   msg.sender_app_id = SAL_I_App_Id;
#endif /* XSAL */
   msg.receiver_thread_id = msg.sender_thread_id = thread_id;
   msg.timestamp_sec = current_time.time;
   msg.timestamp_msec = current_time.millitm;
   if (use_param)
   {
      msg.data_size = sizeof(uintptr_t);
      msg.data = &param;
   }
   else
   {
      msg.data_size = 0;
      msg.data = NULL;
   }

   send_status = SAL_I_Post_Message(&thread_attr->message_queue, &msg, is_urgent, &err);
   if (!send_status)
   {
      Tr_Fault_2("Dropped timer event: TID=%d, EV_ID=%d", thread_id, event_id);
   }
   if ((!send_status) && (SAL_I_Message_Dropped_Callback != NULL))
   {
      SAL_I_Message_Dropped_Callback(thread_id, err, &msg);
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 26-Aug-2015 Kirk Bailey
 *    - Fixed Coverity issue: Value not atomically updated.
 *
 * 27-Aug-2015 Kirk Bailey
 *    - Fixed Coverity issue: Missing lock.
 *
 * -----------------------------------------------------------------------*/
