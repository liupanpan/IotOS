/* -------------------------------------------------------------------------
 *
 * File:        xsal_receive_only_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Receive_Only_Timeout() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_time.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 4); /* Identify file for trace/assert purposes */

/* ----------------------------
 *  Local function prototypes
 * ---------------------------- */

/*  Functions: SAL_Lock_Mutex and SAL_Wait_Semaphore are not defined in 
 *  XSAL_LIGHT external interface but are used internally by the XSAL Light.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);

static SAL_Message_T* SAL_I_Wait_For_New_Messages_Timeout(
   SAL_Message_Queue_T* queue,
   const SAL_Event_Id_T event_list[],
   size_t number_of_events,
   uint32_t timeout_ms);

/* -----------------------------
 *  Local function definitions
 * ----------------------------- */

static SAL_Message_T* SAL_I_Wait_For_New_Messages_Timeout(
   SAL_Message_Queue_T* queue,
   const SAL_Event_Id_T event_list[],
   size_t number_of_events,
   uint32_t timeout_ms)
{
   SAL_Int_T msg_cnt = 0;
   SAL_I_Time_Spec_T time1;
   SAL_Message_T* msg = NULL;
   bool_t wait_for_msg = true;

   SAL_I_Get_Time_Monotonic(&time1);

   do
   {
      while (SAL_Try_Wait_Semaphore(&queue->message_available))
      {
         msg_cnt++;
      }

      /** Unlock the queue and wait for new messages.
       *  Queue is locked on entry.
       */
      (void) SAL_Unlock_Mutex(&queue->queue_lock);

      if (SAL_Wait_Semaphore_Timeout(&queue->message_available, timeout_ms))
      {
         SAL_Queue_Iterator_T it;

         (void) SAL_Lock_Mutex(&queue->queue_lock);

         it = SAL_I_Search_Message_Queue(queue, event_list, number_of_events);
         if (it != queue->end_marker)
         {
            msg = &it->message;
            SAL_I_Remove_Node_From_Queue(it);
            wait_for_msg = false;
         }
         else
         {
            uint32_t diff;
            SAL_I_Time_Spec_T time2;

            SAL_I_Get_Time_Monotonic(&time2);
            msg_cnt++;

            diff = SAL_I_Time_Diff(&time1, &time2);
            if (diff < timeout_ms)
            {
               timeout_ms -= diff;
               time1 = time2;
            }
            else
            {
               /** Set msg to NULL and exit
                */
               wait_for_msg = false;
               msg = NULL;
            }
         }
      }
      else
      {
         (void) SAL_Lock_Mutex(&queue->queue_lock);

         /** Set msg to NULL and exit
          */
         wait_for_msg = false;
         msg = NULL;
      }
   } while (wait_for_msg);

   /** Restore semaphore value
    */
   for (; msg_cnt > 0; msg_cnt--)
   {
      (void) SAL_Signal_Semaphore(&queue->message_available);
   }
   return msg;
}

/* ------------------------------
 *  Public function definitions
 * ------------------------------ */


const SAL_Message_T* SAL_Receive_Only_Timeout(
   const SAL_Event_Id_T event_list[], 
   uint32_t number_of_events,
   uint32_t timeout_ms)
{
   bool_t signal_sem;
   SAL_Message_Queue_T* q;
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   PBC_Require((event_list != NULL) || (number_of_events == 0), "Invalid event list data");
   PBC_Require(calling_thread != NULL, "NULL thread attribute pointer");

   q = &calling_thread->message_queue;

   /** Because there is no possibility to kill one thread by another, so
    *  operation of releasing "current_message" hasn't to be synchronized.
    *
    *  If "killing thread" functionality is possible, the "releasing memory"
    *  activity will have to be synchronized.
    */
   if (calling_thread->current_rx_only_message != NULL)
   {
      SAL_I_Release_Message(calling_thread->current_rx_only_message);
      calling_thread->current_rx_only_message = NULL;
   }

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      SAL_Queue_Iterator_T it = SAL_I_Search_Message_Queue(q, event_list, number_of_events);
      if (it != q->end_marker)
      {
         calling_thread->current_rx_only_message = &it->message;
         SAL_I_Remove_Node_From_Queue(it);
         signal_sem = true;
      }
      else
      {
         calling_thread->current_rx_only_message = SAL_I_Wait_For_New_Messages_Timeout(q, event_list, number_of_events, timeout_ms);
         signal_sem = false;
      }
      (void) SAL_Unlock_Mutex(&q->queue_lock);

      if (signal_sem)
      {
         (void) SAL_Wait_Semaphore(&q->message_available);
      }
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }

   return calling_thread->current_rx_only_message;
}

const SAL_Message_T* SAL_Try_Receive_Only(const SAL_Event_Id_T event_list[], uint32_t number_of_events)
{
   SAL_Message_T * msg = NULL;
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   PBC_Require((event_list != NULL) || (number_of_events == 0), "Invalid event list data");
   if (calling_thread != NULL) /* ignore for non-xsal threads (no message queue) */
   {
      SAL_Message_Queue_T* q;

      q = &calling_thread->message_queue;

      if (calling_thread->current_rx_only_message != NULL)
      {
         SAL_I_Release_Message(calling_thread->current_rx_only_message);
         calling_thread->current_rx_only_message = NULL;
      }

      if (SAL_Lock_Mutex(&q->queue_lock))
      {
         bool_t signal_sem = false;

         SAL_Queue_Iterator_T it = SAL_I_Search_Message_Queue(q, event_list, number_of_events);
         if (it != q->end_marker)
         {
            calling_thread->current_rx_only_message = &it->message;
            SAL_I_Remove_Node_From_Queue(it);
            signal_sem = true;
         }
         (void) SAL_Unlock_Mutex(&q->queue_lock);

         if (signal_sem)
         {
            (void) SAL_Wait_Semaphore(&q->message_available);
         }
      }
      else
      {
         PBC_Failed("Failed to lock queue's mutex");
      }

      msg = calling_thread->current_rx_only_message;
   }
   return msg;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 21-May-2015 Dan Carman
 *    Modify Try_Receive_only to simply return no message (NULL) if called from non-xsal thread.
 *
 * 16-Dec-2014 Dan Carman
 *    Change Message Timeout to use Monotonic Time
 *
 * 25 April 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 30 December 2011 Dan Carman
 *    Added SAL_Try_Receive_Only
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the
 *   enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
