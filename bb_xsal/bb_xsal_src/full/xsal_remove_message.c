/* -------------------------------------------------------------------------
 *
 * File:        xsal_remove_message.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Remove_Message function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 11); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/


/*  Functions SAL_Lock_Mutex and SAL_Wait_Semaphore are not defined in 
 *  XSAL_LIGHT external interface but are used internally by the XSAL Light.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


void SAL_Remove_Message(SAL_Queue_Iterator_T* iterator)
{
   SAL_Message_Queue_T* q;
   SAL_Queue_Iterator_T new_iterator;

   PBC_Require(iterator != NULL, "NULL iterator");

   q = (*iterator)->queue;
   PBC_Require(q != NULL, "NULL queue pointer");

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      PBC_Require(q->node_tab != NULL, "NULL node table");
      PBC_Require(q->end_marker != NULL, "NULL end marker");
      PBC_Require(*iterator != q->end_marker, "Attempt to remove end marker");

      new_iterator = (*iterator)->next_node;

      SAL_I_Free_Message_Data_Buffer(*iterator);
      SAL_I_Remove_Node_From_Queue(*iterator);
      SAL_I_Add_Node_To_Free_List(*iterator);

      *iterator = new_iterator;

      /** Decrement message counter in the semaphore
       */
      (void)SAL_Wait_Semaphore(&q->message_available);

      (void)SAL_Unlock_Mutex(&q->queue_lock);
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
