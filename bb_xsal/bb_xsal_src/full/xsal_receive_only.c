/* -------------------------------------------------------------------------
 *
 * File:        xsal_receive_only.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Receive_Only() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 0); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ---------------------------- */


static SAL_Message_T* SAL_I_Wait_For_New_Messages(
   SAL_Message_Queue_T* queue,
   const SAL_Event_Id_T event_list[],
   size_t number_of_events);


/* -----------------------------
 *  Local function definitions
 * ----------------------------- */


static SAL_Message_T* SAL_I_Wait_For_New_Messages(
   SAL_Message_Queue_T* queue,
   const SAL_Event_Id_T event_list[],
   size_t number_of_events)
{
   SAL_Int_T msg_cnt = 0;
   SAL_Message_T* msg = NULL;
   bool_t wait_for_msg = true;

   do
   {
      while(SAL_Try_Wait_Semaphore(&queue->message_available))
      {
         msg_cnt++;
      }

      /** Unlock the queue and wait for new messages.
       *  Queue was locked on entry.
       */
      (void)SAL_Unlock_Mutex(&queue->queue_lock);

      if (SAL_Wait_Semaphore(&queue->message_available))
      {
         SAL_Queue_Iterator_T it;

         (void)SAL_Lock_Mutex(&queue->queue_lock);

         it = SAL_I_Search_Message_Queue(queue, event_list, number_of_events);
         if (it == queue->end_marker)
         {
            msg_cnt++;
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
         /** Set msg to NULL and exit
          */
         wait_for_msg = false;
         msg = NULL;

         PBC_Failed("SAL_Wait_Semaphore(&queue->message_available). Was the queue created?");
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


/* ------------------------------
 *  Public function definitions
 * ------------------------------ */


const SAL_Message_T* SAL_Receive_Only(
   const SAL_Event_Id_T event_list[], 
   uint32_t number_of_events)
{
   bool_t signal_sem;
   SAL_Message_Queue_T* q;
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   PBC_Require((event_list != NULL) || (number_of_events == 0), "Invalid event list data");
   PBC_Require(calling_thread != NULL, "NULL attributes for calling thread");

   q = &calling_thread->message_queue;

   /** Because there is no possibility to kill one thread by another, so
    *  operation of releasing "current_message" hasn�t to be synchronized.
    *
    *  If �killing thread� functionality is possible, the "releasing memory"
    *  activity will have to be synchronized.
    */
   if (calling_thread->current_rx_only_message != NULL)
   {
      SAL_I_Release_Message(calling_thread->current_rx_only_message);
      calling_thread->current_rx_only_message = NULL;
   }

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      SAL_Queue_Iterator_T it = 
         SAL_I_Search_Message_Queue(q, event_list, number_of_events);
      if (it != q->end_marker)
      {
         calling_thread->current_rx_only_message = &it->message;
         SAL_I_Remove_Node_From_Queue(it);
         signal_sem = true;
      }
      else
      {
         calling_thread->current_rx_only_message = SAL_I_Wait_For_New_Messages(q, event_list, number_of_events);
         signal_sem = false;
      }
      (void)SAL_Unlock_Mutex(&q->queue_lock);

      if (signal_sem)
      {
         (void)SAL_Wait_Semaphore(&q->message_available);
      }
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }

   return calling_thread->current_rx_only_message;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 25 April 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the
 *    enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
