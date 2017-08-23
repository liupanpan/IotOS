/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_message_queue_c.c
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

#include <stdlib.h>
#include <string.h>

EM_FILENUM(XSAL_Q_MODULE_ID, 0); /* Identify file for trace/assert purposes */

/* ---------------------------
 *  Local object definitions
 * --------------------------- */


/** Pointer to the "Queue full" callback
 */
SAL_Message_Dropped_Callback_T SAL_I_Message_Dropped_Callback;


/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/
 
/*  Functions: SAL_Lock_Mutex and SAL_Wait_Semaphore are not defined in 
 *  XSAL_LIGHT external interface but are used internally by the XSAL Light.
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);


/* --------------------------------
 *  Internal functions prototypes
 * --------------------------------*/


static void Insert_Node_To_List(
   SAL_Message_Queue_Node_T* pos, 
   SAL_Message_Queue_Node_T* node);

static SAL_Message_Queue_Node_T* Get_Free_Node(
   SAL_Message_Queue_T* queue, 
   bool_t is_urgent);


/* ---------------------------------
 *  Internal functions definitions
 * ---------------------------------*/


/** Function inserts node to the list before node pointed by pos.
 */
static void Insert_Node_To_List(
   SAL_Message_Queue_Node_T* pos, 
   SAL_Message_Queue_Node_T* node)
{
   node->next_node = pos;
   node->prev_node = pos->prev_node;
   pos->prev_node->next_node = node;
   pos->prev_node = node;
}


/** Function removes node from the list.
 */
void SAL_I_Remove_Node_From_List(SAL_Message_Queue_Node_T* node)
{
   node->prev_node->next_node = node->next_node;
   node->next_node->prev_node = node->prev_node;
}


/** Function frees message data.
 *  If the buffer was allocated by alloc_buf callback, it is dealocated 
 *  by free_buf callback.
 */
void SAL_I_Free_Message_Data_Buffer(SAL_Message_Queue_Node_T* node)
{
   if (
      (node->message.data_size > node->queue->stats.buffer_size) &&
      (node->queue->free_buf != NULL))
   {
      node->queue->free_buf(node->message.data);
   }
   node->message.data = NULL;
}


/** Function add given node to the begining of the free nodes list.
 */
void SAL_I_Add_Node_To_Free_List(SAL_Message_Queue_Node_T* node)
{
   node->next_node = node->queue->free_head;
   node->queue->free_head = node;
}


/** Function gets free node from the free list for new message.
 *  If the message is urgent and free nodes list is empty, the last 
 *  non-urgent message node is released and used by the urgent message.
 *
 * if Queue is full, will return NULL
 */
static SAL_Message_Queue_Node_T* Get_Free_Node(
   SAL_Message_Queue_T* queue, 
   bool_t is_urgent)
{
   SAL_Message_Queue_Node_T* free_node = queue->free_head;
   if (free_node != NULL)
   {
      queue->free_head = free_node->next_node;
   }
   else if (is_urgent)
   {
      if (queue->end_marker->prev_node != queue->urgent_tail)
      {
         /** There is not any free node in the queue. If there is at least 
          *  one non-urgent message then remove it and put new (urgent) message 
          *  in place of the removed one.
          */
         Tr_Fault("Queue is full and a new urgent message has arived. "
                  "The last non-urgent message is removed from queue");

         SAL_I_Free_Message_Data_Buffer(queue->end_marker->prev_node);
         free_node =  queue->end_marker->prev_node;
         SAL_I_Remove_Node_From_List(queue->end_marker->prev_node);

         /** Decrease message_count, because message was removed
          */
         queue->stats.message_count--;
         (void)SAL_Wait_Semaphore(&queue->message_available);
      }
   }
   return free_node;
}



bool_t SAL_I_Init_Queue_Structure(SAL_Message_Queue_T* queue)
{
   PBC_Require(queue != NULL, "NULL queue pointer");

   queue->node_tab = NULL;
   queue->free_head = NULL;
   return SAL_Create_Mutex(&queue->queue_lock, NULL);
}


void SAL_I_Deinit_Queue_Structure(SAL_Message_Queue_T* queue)
{
   PBC_Require(queue != NULL, "NULL queue pointer");

   queue->node_tab = NULL;
   queue->free_head = NULL;
   (void)SAL_Destroy_Mutex(&queue->queue_lock);
}


SAL_Message_Queue_Node_T* SAL_I_Get_Free_Node(
   SAL_Message_Queue_T* queue, 
   bool_t is_urgent, 
   size_t data_size,
   SAL_Message_Dropped_Reason_T* err)
{
   SAL_Message_Queue_Node_T* node = Get_Free_Node(queue, is_urgent);
   if (node != NULL)
   {
      if (data_size > 0u)
      {
         /* Allocate memory for message data (calling allocator or
          * from pre-allocated buffer).
          */
         if (data_size > node->queue->stats.buffer_size)
         {
            if (node->queue->alloc_buf != NULL)
            {
               node->message.data = node->queue->alloc_buf(data_size);
               if (node->message.data == NULL)
               {
                  SAL_I_Add_Node_To_Free_List(node);
                  *err = SAL_No_Memory;
                  node = NULL;
               }
            }
            else
            {
               SAL_I_Add_Node_To_Free_List(node);
               *err = SAL_Too_Small_Buffer;
               node = NULL;
            }
         }
         else
         {
            node->message.data = &node[1];
         }
      }
      else
      {
         node->message.data = NULL;
      }
   }
   else
   {
      if (queue->node_tab == NULL)
      {
         *err = SAL_No_Queue;
      }
      else
      {
         *err = SAL_Queue_Full;
      }
   }
   return node;
}


#if defined (XSAL)

void SAL_I_Free_Node(SAL_Message_Queue_Node_T* node)
{
   SAL_I_Free_Message_Data_Buffer(node);
   SAL_I_Add_Node_To_Free_List(node);
}

#endif /* XSAL */

bool_t SAL_I_Create_Message_Queue(
   SAL_Message_Queue_T* queue, 
   size_t queue_size,
   size_t buffer_size,
   void* (* alloc_buf)(size_t size),
   void  (* free_buf)(void *buf))
{
   size_t node_size;

   PBC_Require(queue != NULL, "NULL queue");
   PBC_Require(queue_size > 0u, "Zero queue size");
   
   /** Calculates size for single queue node.
    *  The node size is equal size of SAL_Message_Queue_Node_T plus 
    *  size of pre-allocated buffer.
    *
    *  Queue nodes are part of node_tab, so the size of the single 
    *  node has to be divided by the size of WORD (processor dependent).
    */
   node_size = sizeof(SAL_Message_Queue_Node_T) + buffer_size;
   node_size = ((node_size+sizeof(size_t)-1u)/sizeof(size_t))*sizeof(size_t);

   if (SAL_Lock_Mutex(&queue->queue_lock))
   {
      size_t i;
      uint8_t* p_queue_node;
      PBC_Require(queue->node_tab == NULL, "NULL node table pointer");

      /** Allocates table of queue nodes. 
       *  Increase queue_size by 1, because queue should be able to store 
       *  queue_size new messages, while thread is processing "current"
       *  message.
       */
      queue->node_tab = (SAL_Message_Queue_Node_T*)calloc(
         queue_size+1u,
         node_size);

      if (queue->node_tab == NULL)
      {
         Tr_Fault("Couldn't allocate memory for queue data");
         (void)SAL_Unlock_Mutex(&queue->queue_lock);
         return false;
      }

      if (!SAL_Create_Semaphore(&queue->message_available, NULL))
      {
         free(queue->node_tab);
         (void)SAL_Unlock_Mutex(&queue->queue_lock);
         return false;
      }

      p_queue_node = (uint8_t*)(void*)queue->node_tab;

      /** Initialize free node list
       */
      queue->free_head = queue->node_tab;

      /** All but one nodes belong to the free node list (unidirectional list)
       *  Initialize queue and next_node fields of each node structure
       */
      for(i=0; i < queue_size; i++, p_queue_node += node_size)
      {
         ((SAL_Message_Queue_Node_T*)(void*)p_queue_node)->queue = queue;
         ((SAL_Message_Queue_Node_T*)(void*)p_queue_node)->next_node = 
            (SAL_Message_Queue_Node_T*)(void*)(&p_queue_node[node_size]);
      }

      /** The last loop iteration added last node to the free nodes list.
       *  In fact the last node is not a part of free nodes list - it is used 
       *  as end list marker. 
       *  The next instruction unlinks end_marker from free nodes list.
       */
      ((SAL_Message_Queue_Node_T*)(void*)(p_queue_node-node_size))->next_node = 
         NULL;

      /** Initialize end queue marker.
       *  At the begining, when queue is empty both pointers of end_marker:
       *  next and prev points to itself.
       */
      queue->end_marker = queue->urgent_tail = 
         (SAL_Message_Queue_Node_T*)(void*)p_queue_node;
      queue->end_marker->queue = queue;
      queue->end_marker->next_node = 
         queue->end_marker->prev_node = queue->end_marker;

      /** Store memory management functions
       */
      queue->alloc_buf  = alloc_buf;
      queue->free_buf   = free_buf;

      /** Initialize queue statistic data
       */
      memset(&queue->stats, 0, sizeof(queue->stats));
      queue->stats.queue_size  = queue_size;
      queue->stats.buffer_size = buffer_size;

      (void)SAL_Unlock_Mutex(&queue->queue_lock);
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }
   return true;
}


void SAL_I_Destroy_Message_Queue(SAL_Message_Queue_T* queue)
{
   PBC_Require(queue != NULL, "NULL queue pointer");

   if (SAL_Lock_Mutex(&queue->queue_lock))
   {
      if (queue->node_tab != NULL)
      {
         SAL_Message_Queue_Node_T* node = queue->end_marker->next_node;

         /** end_marker->message.data always should has NULL value
          */
         PBC_Require(queue->end_marker->message.data == NULL, "Non-NULL end marker for queue");
         while(node != queue->end_marker)
         {
            SAL_I_Free_Message_Data_Buffer(node);
            node = node->next_node;
         }
        
         free(queue->node_tab);

         queue->node_tab = NULL;
         queue->free_head = NULL;
         (void)SAL_Destroy_Semaphore(&(queue->message_available));
      }
      (void)SAL_Unlock_Mutex(&queue->queue_lock);
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }
}


bool_t SAL_I_Post_Message(
   SAL_Message_Queue_T* queue, 
   const SAL_Message_T* message, 
   bool_t is_urgent,
   SAL_Message_Dropped_Reason_T* err)
{
   SAL_Message_Queue_Node_T* new_node = NULL;

   PBC_Require(queue != NULL, "NULL queue");
   PBC_Require(message != NULL, "NULL message");
   PBC_Require(err != NULL, "NULL err ptr");

   if (SAL_Lock_Mutex(&queue->queue_lock))
   {
      new_node = SAL_I_Get_Free_Node(queue, is_urgent, message->data_size, err);
      if (new_node != NULL)
      {
         /** Save pointer to message data buffer in temporary variable.
          *  Next copy the whole message header to new_node and restore 
          *  pointer to data
          */ 
         void* p_data = new_node->message.data;
         new_node->message = *message;
         new_node->message.data = p_data;

         if (message->data_size > 0u)
         {
            (void)memcpy(
               new_node->message.data, 
               message->data,
               message->data_size);

            /** Update queue statistic
             */         
            if (message->data_size > queue->stats.peak_message_size)
            {
               queue->stats.peak_message_size = message->data_size;
            }
         }

         /** Update queue statistic
          */
         queue->stats.message_count++;
         if (queue->stats.message_count > queue->stats.peak_message_count)
         {
            queue->stats.peak_message_count = queue->stats.message_count;
         }
         queue->stats.total_rcvd_msgs++;

         if (is_urgent)
         {
            Insert_Node_To_List(queue->urgent_tail->next_node, new_node);
            queue->urgent_tail = new_node;
         }
         else
         {
            Insert_Node_To_List(queue->end_marker, new_node);
         }
      }
      (void)SAL_Unlock_Mutex(&queue->queue_lock);
   }
   else
   {
       PBC_Failed("Failed to lock queue's mutex");
   }

   if (new_node != NULL)
   {
      (void)SAL_Signal_Semaphore(&queue->message_available);
   }

   return (bool_t)(new_node != NULL);
}

#if defined (XSAL)

void SAL_I_Post_Message_Node(
   SAL_Message_Queue_Node_T* node,
   bool_t is_urgent)
{
   SAL_Message_Queue_T* q = node->queue;
   
   if (is_urgent)
   {
      Insert_Node_To_List(q->urgent_tail->next_node, node);
      q->urgent_tail = node;
   }
   else
   {
      Insert_Node_To_List(q->end_marker, node);
   }

   /** Updates queue statistics data
    */
   q->stats.message_count++;
   if (q->stats.message_count > q->stats.peak_message_count)
   {
      q->stats.peak_message_count = q->stats.message_count;
   }

   if (node->message.data_size > q->stats.peak_message_size)
   {
      q->stats.peak_message_size = node->message.data_size;
   }
   q->stats.total_rcvd_msgs++;
   (void)SAL_Signal_Semaphore(&(q->message_available));
}

#endif /* XSAL */


void SAL_I_Release_Message(SAL_Message_T* message)
{
   SAL_Message_Queue_T* q;

   PBC_Require(message != NULL, "NULL message");

   q = ((SAL_Message_Queue_Node_T*)(void*)message)->queue;

   PBC_Require(q != NULL, "NULL queue");

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      SAL_I_Free_Message_Data_Buffer((SAL_Message_Queue_Node_T*)(void*)message);
      SAL_I_Add_Node_To_Free_List((SAL_Message_Queue_Node_T*)(void*)message);

      (void)SAL_Unlock_Mutex(&q->queue_lock);
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }
}


void SAL_I_Remove_Node_From_Queue(SAL_Queue_Iterator_T iterator)
{
   SAL_Message_Queue_T* q = iterator->queue;
   SAL_I_Remove_Node_From_List(iterator);
   if (iterator == q->urgent_tail)
   {
      q->urgent_tail = iterator->prev_node;
   }
   q->stats.message_count--;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
