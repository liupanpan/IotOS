/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_receive_from_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_I_Receive_From_Timeout() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_time.h"
#include "xsal_i_event_property.h"
#include "xsal_i_select.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 2); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/


/*  Functions: SAL_Lock_Mutex and SAL_Wait_Semaphore are not defined in 
 *  XSAL_LIGHT external interface but are used internally by the XSAL Light.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);


/* --------------------------------
 *  Internal functions prototypes
 * --------------------------------*/


#if defined (XSAL)

static SAL_Message_T* SAL_I_Receive_Selected_From_Timeout(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property,
   uint32_t timeout_ms);

static SAL_Message_T* SAL_I_Wait_For_New_Messages_Timeout(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property,
   uint32_t timeout_ms);

#endif /* XSAL */


/* --------------------------------
 *  Internal functions definitions
 * --------------------------------*/


#if defined (XSAL)

static SAL_Message_T* SAL_I_Receive_Selected_From_Timeout(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property,
   uint32_t timeout_ms)
{
   SAL_Message_T* msg;
   bool_t sem_decrease;


   if (SAL_Lock_Mutex(&queue->queue_lock))
   {
      SAL_Queue_Iterator_T it = SAL_I_Search_Selected_Message(queue, event_property);
      if (it != queue->end_marker)
      {
         msg = &it->message;
         SAL_I_Remove_Node_From_Queue(it);
         sem_decrease = true;
      }
      else
      {
         msg = SAL_I_Wait_For_New_Messages_Timeout(queue, event_property, timeout_ms);
         sem_decrease = false;
      }
      (void)SAL_Unlock_Mutex(&queue->queue_lock);

      if (sem_decrease)
      {
         (void)SAL_Wait_Semaphore(&queue->message_available);
      }
   }
   else
   {
      msg = NULL;
      PBC_Failed("Failed to lock queue's mutex");
   }

   return msg;
}


static SAL_Message_T* SAL_I_Wait_For_New_Messages_Timeout(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property,
   uint32_t timeout_ms)
{
   SAL_Int_T msg_cnt = 0;
   SAL_I_Time_Spec_T time1;
   bool_t wait_for_msg = true;
   SAL_Message_T* msg = NULL;

   SAL_I_Get_Time_Monotonic(&time1);

   do
   {
      while(SAL_Try_Wait_Semaphore(&queue->message_available))
      {
         msg_cnt++;
      }

      /** Unlock the queue and wait for new messages.
       *  Queue is locked on entry.
       */
      (void)SAL_Unlock_Mutex(&queue->queue_lock);

      if (SAL_Wait_Semaphore_Timeout(&queue->message_available, timeout_ms))
      {
         SAL_Queue_Iterator_T it;

         (void)SAL_Lock_Mutex(&queue->queue_lock);
         it = SAL_I_Search_Selected_Message(queue, event_property);
         if (it == queue->end_marker)
         {
            uint32_t diff;
            SAL_I_Time_Spec_T time2;

            msg_cnt++;

            SAL_I_Get_Time_Monotonic(&time2);
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
         else
         {
            msg = &it->message;
            SAL_I_Remove_Node_From_Queue(it);
            wait_for_msg = false;
         }
      }
      else
      {
         (void)SAL_Lock_Mutex(&queue->queue_lock);
         wait_for_msg = false;
      }
   } while(wait_for_msg);

   /** Restore semaphore value
    */
   for(; msg_cnt > 0; msg_cnt--)
   {
      (void)SAL_Signal_Semaphore(&queue->message_available);
   }

   return msg;
}

#endif /* XSAL */

/* ---------------------
 *  Function definition
 * --------------------- */


const SAL_Message_T* SAL_I_Receive_From_Timeout(
   SAL_I_Thread_Attr_T* queue_owner_thread,
   uint32_t timeout_ms)
{
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   PBC_Require(queue_owner_thread != NULL, "NULL owner thread attribute");
   PBC_Require(calling_thread != NULL, "NULL calling thread attribute");

   /** Because there is no possibility to kill one thread by another, so
    *  operation of releasing "current_message" hasn�t to be synchronized.
    *
    *  If �killing thread� functionality is possible, the "releasing memory"
    *  activity will have to be synchronized.
    */
   if (calling_thread->current_message != NULL)
   {
      SAL_I_Release_Message(calling_thread->current_message);
      calling_thread->current_message = NULL;
   }
#if defined (XSAL)
   if (queue_owner_thread->selected_count == 0u)
   {
      calling_thread->current_message = SAL_I_Pend_Message_Timeout(
         &queue_owner_thread->message_queue,
         timeout_ms);
   }
   else
   {
      calling_thread->current_message = SAL_I_Receive_Selected_From_Timeout(
         &queue_owner_thread->message_queue, 
         SAL_I_Thread_Event_Property(queue_owner_thread->thread_id),
         timeout_ms);
   }
#else /* XSAL_LIGHT */
   calling_thread->current_message = SAL_I_Pend_Message_Timeout(
         &queue_owner_thread->message_queue,
         timeout_ms);
#endif /* XSAL && XSAL_LIGHT */

   return calling_thread->current_message;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 16-Dec-2014 Dan Carman
 *    Change Message Timeout to use Monotonic Time
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
