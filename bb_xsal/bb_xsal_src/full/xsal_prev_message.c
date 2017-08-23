/* -------------------------------------------------------------------------
 *
 * File:        xsal_prev_message.c 
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Prev_Message() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 9); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/


/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but is used internally by the XSAL Light.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


void SAL_Prev_Message(SAL_Queue_Iterator_T* iterator)
{
   SAL_Message_Queue_T* q;

   PBC_Require(iterator != NULL, "NULL iterator");

   q = (*iterator)->queue;
   PBC_Require(q != NULL, "NULL queue pointer");

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      PBC_Require(q->node_tab != NULL, "NULL node table");
      PBC_Require(q->end_marker != NULL, "NULL end marker");

      /** Iterator mustn't points to the first message.
       */
      PBC_Require(*iterator != q->end_marker->next_node, "Iterator points to first message");

      *iterator = (*iterator)->prev_node;
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
