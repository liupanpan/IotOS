/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_pend_message.c
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

EM_FILENUM(XSAL_MSG_MODULE_ID, 11); /* Identify file for trace/assert purposes */

/* -----------------------
 *  Function definitions
 * ----------------------- */


SAL_Message_T* SAL_I_Pend_Message(SAL_Message_Queue_T* queue)
{
   SAL_Message_Queue_Node_T* msg;
   SAL_Message_Queue_Node_T* end_marker;
   bool_t wait_for_msg = true;

   PBC_Require(queue != NULL, "NULL queue");
   PBC_Require(queue->node_tab != NULL, "NULL node table");

   end_marker = queue->end_marker;

   do
   {
      if (SAL_Wait_Semaphore(&queue->message_available))
      {
         if (SAL_Lock_Mutex(&queue->queue_lock))
         {
            /** Save pointer to the message and next remove it from the list
             *
             *  It is possible that semaphore value is > 1 but the message
             *  queue is empty (msg == end_marker). It is because message 
             *  filter may remove messages from queue before updates semaphore
             */
            msg = end_marker->next_node;
            if (msg != end_marker)
            {
               SAL_I_Remove_Node_From_Queue(msg);
               wait_for_msg = false;
            }
            (void)SAL_Unlock_Mutex(&queue->queue_lock);
         }
         else
         {
            /** Set msg to NULL and exit
             */
            wait_for_msg = false;
            msg = NULL;

            PBC_Failed("Failed to lock queue's mutex");
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
   
   return (SAL_Message_T*)(void*)msg;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
