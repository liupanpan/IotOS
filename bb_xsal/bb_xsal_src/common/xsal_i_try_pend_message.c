/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_try_pend_message.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Definition of the SAL Message Queue functions.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 5); /* Identify file for trace/assert purposes */


/* -----------------------
 *  Function definitions
 * ----------------------- */


SAL_Message_T* SAL_I_Try_Pend_Message(SAL_Message_Queue_T* queue)
{
   SAL_Message_Queue_Node_T* msg = NULL;
   SAL_Message_Queue_Node_T* end_marker;

   PBC_Require(queue != NULL, "NULL queue");
   PBC_Require(queue->node_tab != NULL, "NULL queue node table");

   if (SAL_Try_Wait_Semaphore(&queue->message_available))
   {
      if (SAL_Lock_Mutex_Timeout(&queue->queue_lock, 10000))
      {
         /** Save pointer to the message and next remove it from the list
          *
          *  It is possible that semaphore value is > 1 but the message
          *  queue is empty (msg == end_marker). It is because message 
          *  filter may remove messages from queue before updates semaphore
          */
         end_marker = queue->end_marker;
         if (end_marker->next_node != end_marker)
         {
            msg = end_marker->next_node;
            SAL_I_Remove_Node_From_Queue(msg);
         }
         (void)SAL_Unlock_Mutex(&queue->queue_lock);
      }
      else
      {
         PBC_Failed("Failed to lock queue's mutex");
      }
   }
   else
   {
      Tr_Info_Lo("SAL_I_Try_Pend_Message: No messages in the queue");
   }

   return (SAL_Message_T*)(void*)msg;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
