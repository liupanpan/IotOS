/* -------------------------------------------------------------------------
 *
 * File:        xsal_last_message.c 
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Last_Message() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 7); /* Identify file for trace/assert purposes */


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


void SAL_Last_Message(SAL_Queue_Iterator_T* iterator)
{
   SAL_Message_Queue_T* q;
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();

   PBC_Require(iterator != NULL, "NULL iterator");
   PBC_Require(thread_attr != NULL, "NULL thread attribute");

   q = &thread_attr->message_queue;

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      PBC_Require(q->node_tab != NULL, "NULL node table");
      PBC_Require(q->end_marker != NULL, "NULL end pointer");
      PBC_Require(q->end_marker->prev_node != NULL, "NULL previous link");

      *iterator = q->end_marker->prev_node;
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
