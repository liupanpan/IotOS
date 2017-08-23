/* -------------------------------------------------------------------------
 *
 * File:        xsal_is_queue_end.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Is_Queue_End() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_Q_MODULE_ID, 6); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Is_Queue_End(const SAL_Queue_Iterator_T* iterator)
{
   bool_t status = false;
   SAL_Message_Queue_T* q;

   PBC_Require(iterator != NULL, "NULL iterator");

   q = (*iterator)->queue;
   PBC_Require(q != NULL, "NULL queue pointer");
   PBC_Require(q->end_marker != NULL, "NULL end marker");

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      status = (bool_t)((*iterator) == (*iterator)->queue->end_marker);
      (void)SAL_Unlock_Mutex(&q->queue_lock);
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }

   return status;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
