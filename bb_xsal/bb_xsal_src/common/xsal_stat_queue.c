/* -------------------------------------------------------------------------
 *
 * File:        xsal_stat_queue.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Stat_Queue() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_message_queue.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_Q_MODULE_ID, 2); /* Identify file for trace/assert purposes */


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/


/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Stat_Queue(SAL_Thread_Id_T thread_id, SAL_Stat_Queue_T* queue_stat)
{
   bool_t status = false;
   SAL_Message_Queue_T* q;

   PBC_Require(thread_id > 0, "Invalid thread id");
   PBC_Require((size_t)thread_id <= SAL_I_Max_Number_Of_Threads, "Thread id > max");
   PBC_Require(queue_stat != NULL, "NULL queue status pointer");

   q = &SAL_I_Thread_Table[thread_id].message_queue;

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      if (q->node_tab != NULL)
      {
         *queue_stat = q->stats;
         /* Queue size was increased internally during queue creation. */
         queue_stat->queue_size--;
         status = true;
      }
      (void) SAL_Unlock_Mutex(&q->queue_lock);
   }
   else
   {
      PBC_Failed("Unable to lock queue's mutex");
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
