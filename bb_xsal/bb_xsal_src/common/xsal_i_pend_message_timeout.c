/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_pend_message_timeout.c
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
#include "xsal_i_time.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 1); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/

/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT external interface 
 *  but are used internally by the XSAL Light.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);


/* -----------------------
 *  Function definitions
 * ----------------------- */


SAL_Message_T* SAL_I_Pend_Message_Timeout(
   SAL_Message_Queue_T* queue,
   uint32_t timeout_ms)
{
   SAL_I_Time_Spec_T time1;
   bool_t wait_for_msg = true;

   SAL_Message_Queue_Node_T* msg;
   SAL_Message_Queue_Node_T* end_marker;

   PBC_Require(queue != NULL, "NULL queue");
   PBC_Require(queue->node_tab != NULL, "NULL queue node table");

   end_marker = queue->end_marker;

   SAL_I_Get_Time_Monotonic(&time1);

   do
   {
      if (SAL_Wait_Semaphore_Timeout(&queue->message_available, timeout_ms))
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
            else
            {
               uint32_t diff;
               SAL_I_Time_Spec_T time2;

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
            (void)SAL_Unlock_Mutex(&queue->queue_lock);
         }
         else
         {
            wait_for_msg = false;
            msg = NULL;

            PBC_Failed("Failed to lock queue's mutex");
         }
      }
      else
      {
         /** Set msg to NULL and exit. This is a "normal" condition and so should not
          *  generate a trace message.
          */
         wait_for_msg = false;
         msg = NULL;
      }
   } while(wait_for_msg);

   return (SAL_Message_T*)(void*)msg;
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
