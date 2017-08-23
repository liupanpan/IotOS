/* -------------------------------------------------------------------------
 *
 * File:        xsal_purge.c
 *
 * Copyright:   2011 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman(at)delphi.com)
 *
 * Description: Implementation of the SAL_Purge function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_Q_MODULE_ID, 7); /* Identify file for trace/assert purposes */

/* -----------------------------
 *  Public function definition
 * -----------------------------*/

void SAL_Purge(void)
{
   SAL_Message_Queue_T* q;
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();

   PBC_Require(thread_attr != NULL, "NULL thread attribute");
   PBC_Require(thread_attr->thread_id > 0, "Illegal thread id");
   PBC_Require((size_t) thread_attr->thread_id <= SAL_I_Max_Number_Of_Threads, "Illegal thread id");

   q = &thread_attr->message_queue;
   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      SAL_Message_Queue_Node_T* n;

      PBC_Require(q->node_tab != NULL, "NULL node"); /* Check if queue was created */
      PBC_Require(q->end_marker != NULL, "NULL end marker"); /* Check end_marker before first use */

      /** Get first message (end_marker->next_node points to the first
       *  message, end_marker->prev_node points to the last message).
       */
      n = q->end_marker->next_node;
      while (n != q->end_marker)
      {
         SAL_Message_Queue_Node_T* next_n = n->next_node;

         SAL_I_Free_Message_Data_Buffer(n);
         SAL_I_Remove_Node_From_Queue(n);
         SAL_I_Add_Node_To_Free_List(n);
         n = next_n;

         /** Decrement semaphore counter.
          *
          *  Use SAL_Try_Wait_Semaphore() instead SAL_Wait_Semaphore() because
          *  semaphore's counter might be not synch with number of messages 
          *  in the queue. It might happen when another thread receives message
          *  from calling thread's queue (e.g. calling SAL_Receive_From()).
          */
         (void) SAL_Try_Wait_Semaphore(&q->message_available);
      }
      (void) SAL_Unlock_Mutex(&q->queue_lock);
   }
   else
   {
      PBC_Failed("Failed to lock queue Mutex");
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 10 May 2011, Jacek Roman (jacek.roman(at)delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
