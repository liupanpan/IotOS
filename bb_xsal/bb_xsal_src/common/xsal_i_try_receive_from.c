/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_try_receive_from.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_I_Try_Receive_From() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "xsal_i_event_property.h"
#include "xsal_i_select.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 6); /* Identify file for trace/assert purposes */


/* --------------------------------
 *  Internal functions prototypes
 * --------------------------------*/


static SAL_Message_T* SAL_I_Try_Receive_Selected_From(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property);


/* --------------------------------
 *  Internal functions definitions
 * --------------------------------*/


static SAL_Message_T* SAL_I_Try_Receive_Selected_From(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property)
{
   SAL_Message_T* msg;

   if (SAL_Lock_Mutex_Timeout(&queue->queue_lock, 5000))
   {
      SAL_Queue_Iterator_T it =
         SAL_I_Search_Selected_Message(queue, event_property);
      if (it == queue->end_marker)
      {
         msg = NULL;
      }
      else
      {
         msg = &it->message;
         SAL_I_Remove_Node_From_Queue(it);
      }
      (void)SAL_Unlock_Mutex(&queue->queue_lock);

      if (msg != NULL)
      {
         /* Update queue semaphore value */
         (void)SAL_Wait_Semaphore_Timeout(&queue->message_available, 1000);
      }
   }
   else
   {
      msg = NULL;
      PBC_Failed("Failed to lock queue's mutex");
   }

   return msg;
}


/* ---------------------
 *  Function definition
 * --------------------- */


const SAL_Message_T* SAL_I_Try_Receive_From(SAL_I_Thread_Attr_T* queue_owner_thread)
{
   SAL_I_Thread_Attr_T* calling_thread = SAL_I_Get_Thread_Attr();

   PBC_Require(queue_owner_thread != NULL, "NULL queue owner thread attribute");
   PBC_Require(calling_thread != NULL, "NULL calling thread attribute");

   /** Because there is no possibility to kill one thread by another, so
    *  operation of releasing "current_message" hasn’t to be synchronized.
    *
    *  If „killing thread” functionality is possible, the "releasing memory"
    *  activity will have to be synchronized.
    */
   if (calling_thread->current_message != NULL)
   {
      SAL_I_Release_Message(calling_thread->current_message);
      calling_thread->current_message = NULL;
   }

   if (queue_owner_thread->selected_count == 0u)
   {
      calling_thread->current_message = SAL_I_Try_Pend_Message(&queue_owner_thread->message_queue);
   }
   else
   {
      calling_thread->current_message = SAL_I_Try_Receive_Selected_From(
         &queue_owner_thread->message_queue, 
         SAL_I_Thread_Event_Property(queue_owner_thread->thread_id));
   }
   
   return calling_thread->current_message;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 12 March 2008, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
