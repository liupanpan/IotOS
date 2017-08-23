/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_receive_from.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_I_Receive_From() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_event_property.h"
#include "xsal_i_select.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 12); /* Identify file for trace/assert purposes */


/* --------------------------------
 *  Internal functions prototypes
 * --------------------------------*/

static SAL_Message_T* SAL_I_Receive_Selected_From(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property);

static SAL_Message_T* SAL_I_Wait_For_New_Messages(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property);


/* --------------------------------
 *  Internal functions definitions
 * --------------------------------*/


static SAL_Message_T* SAL_I_Receive_Selected_From(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property)
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
         msg = SAL_I_Wait_For_New_Messages(queue, event_property);
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


static SAL_Message_T* SAL_I_Wait_For_New_Messages(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property)
{
   SAL_Message_T* msg = NULL;
   SAL_Int_T msg_cnt = 0;


   do
   {
      SAL_Queue_Iterator_T it;
      while(SAL_Try_Wait_Semaphore(&queue->message_available))
      {
         msg_cnt++;
      }

      /** Unlock the queue and wait for new messages.
       *  Queue is locked on entry.
       */
      (void)SAL_Unlock_Mutex(&queue->queue_lock);
      if (SAL_Wait_Semaphore(&queue->message_available))
      {
         (void)SAL_Lock_Mutex(&queue->queue_lock);
         it = SAL_I_Search_Selected_Message(queue, event_property);
         if (it == queue->end_marker)
         {
            msg_cnt++;
         }
         else
         {
            msg = &it->message;
            SAL_I_Remove_Node_From_Queue(it);
         }
      }
   } while(msg == NULL);

   /** Restore semaphore value
    */
   for(; msg_cnt > 0; msg_cnt--)
   {
      (void) SAL_Signal_Semaphore(&queue->message_available);
   }

   return msg;
}


/* ---------------------
 *  Function definition
 * --------------------- */

const SAL_Message_T* SAL_I_Receive_From(SAL_I_Thread_Attr_T* queue_owner)
{
   SAL_Message_Queue_T* queue;
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   PBC_Require(queue_owner != NULL, "NULL queue owner thread attribute");
   PBC_Require(calling_thread != NULL, "NULL calling thread attribute");

   queue = &queue_owner->message_queue;
   PBC_Require(queue != NULL, "NULL queue");

   if (calling_thread->current_message != NULL)
   {
      SAL_I_Release_Message(calling_thread->current_message);
      calling_thread->current_message = NULL;
   }
   
   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      (void)SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);

      if (queue_owner->selected_count == 0)
      {
         calling_thread->current_message = SAL_I_Pend_Message(queue);
      }
      else
      {
         calling_thread->current_message = SAL_I_Receive_Selected_From(queue, SAL_I_Thread_Event_Property(queue_owner->thread_id));
      }
   }
   else
   {
      PBC_Failed("Failed to lock thread table mutex");
   }

   return calling_thread->current_message;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 25-Aug-2015 Kirk Bailey
 *   Fixed Coverity issue: Unchecked return value.
 * -----------------------------------------------------------------------*/
