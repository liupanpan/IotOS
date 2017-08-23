/* -------------------------------------------------------------------------
 *
 * File:        xsal_message_queue.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the SAL Message Queue.
 *
 * -------------------------------------------------------------------------*/

#ifndef XSAL_MESSAGE_QUEUE_H
#define XSAL_MESSAGE_QUEUE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


#include "xsal.h"


/* -------------------------------------------------------------------------
 *
 *                  Message queue - public data structures
 *
 * -----------------------------------------------------------------------*/


/**
 *  Definition of the queue node (SAL_Message_T data and 
 *  internal node structures)
 */
typedef struct SAL_Message_Queue_Node_Tag
{
   /** SAL Message data
    *
    *  @note This field must be a first member of the structure!
    */
   SAL_Message_T message;

   /** Pointer to the queue which message belongs to
    */
   struct SAL_Message_Queue_Tag* queue;

   /** Next queue node
    */
   struct SAL_Message_Queue_Node_Tag* next_node;
 
   /** Prev queue node
    */
   struct SAL_Message_Queue_Node_Tag* prev_node;

} SAL_Message_Queue_Node_T;


/**
 *  Definition of the message queue structure
 */
typedef struct SAL_Message_Queue_Tag
{
   /**  Queue statistics data
    */
   SAL_Stat_Queue_T stats;

   /** Pointer to internal queue structure
    */
   SAL_Message_Queue_Node_T* node_tab;

   /** Pointer to the last urgent message
    */
   SAL_Message_Queue_Node_T* urgent_tail;

   /** Queue end marker
    */
   SAL_Message_Queue_Node_T* end_marker;
   
   /** First free node in the queue
    */
   SAL_Message_Queue_Node_T* free_head;
   
   /** Mutex for synchronization access to the queue internal data
    */
   SAL_Mutex_T queue_lock;

   /** Semafore blocks threads until new message arrives
    */
   SAL_Semaphore_T message_available;

   /** Handler to the memory allocator
    */
   void *(*alloc_buf)(size_t size);

   /** Handler to the memory de-allocator
    */
   void (*free_buf)(void *buf);
} SAL_Message_Queue_T;


/** Pointer to the "Queue full" callback
 */
extern SAL_Message_Dropped_Callback_T SAL_I_Message_Dropped_Callback;


/**
 *  Function gets free queue node and allocates buffer for message data
 *
 *  @param [in] queue      queue the free node is allocated from
 *  @param [in] is_urgent  1 if allocated node is for urgent message
 *  @param [in] data_size  size of message data
 *  @param [out] err       if it is not possible to allocate queue node is set
 *                         to SAL_No_Queue, SAL_Queue_Full or SAL_No_Memory
 *
 *  @return pointer to the queue node or NULL if the queue is full 
 *          or there is no free memory
 */
SAL_Message_Queue_Node_T* SAL_I_Get_Free_Node(
   SAL_Message_Queue_T* queue, 
   bool_t is_urgent, 
   size_t data_size,
   SAL_Message_Dropped_Reason_T* err);


void SAL_I_Remove_Node_From_List(SAL_Message_Queue_Node_T* node);

void SAL_I_Free_Message_Data_Buffer(SAL_Message_Queue_Node_T* node);

void SAL_I_Add_Node_To_Free_List(SAL_Message_Queue_Node_T* node);

/**
 *  Function frees message data buffer and puts the node to the free list.
 *
 *  @param [in] node node to be give back to its queue
 */
void SAL_I_Free_Node(SAL_Message_Queue_Node_T* node);


/**
 *  Function appends given node (with message) the queue.
 *
 *  @param [in] node       node to be appended to the queue
 *  @param [in] is_urgent  1 if node contains urgent message
 */
void SAL_I_Post_Message_Node(SAL_Message_Queue_Node_T* node, bool_t is_urgent);


/**
 *  Function initializes queue structure. 
 *  
 *  It must be called only once, before any queue related functions
 *  will be called.
 *
 *  @param [in] queue pointer to the queue to be initialized
 *
 *  @return true success, fasle on failure
 *
 *  @see SAL_Create_Message_Queue(), SAL_Deinit_Queue_Structure()
 */
bool_t SAL_I_Init_Queue_Structure(SAL_Message_Queue_T* queue);


/**
 *  Function de-initializes internal queue structures.
 *  It should be called after SAL_Destroy_Message_Queue() 
 *  if the application do not intend use this queue any more 
 *  (e.g it will not create given queue again)
 *
 *  @param [in] queue pointer to the queue to be de-initialized
 *
 *  @see SAL_Init_Queue_Structure(), SAL_Destroy_Message_Queue()
 */
void SAL_I_Deinit_Queue_Structure(SAL_Message_Queue_T* queue);


/**
 *  Function creates a message queue. 
 *
 *  Each queue has its own callback functions to allocate and free memory 
 *  for queue elements. The pre-allocated buffers, alloc_buf and free_buf 
 *  or function operating on buffer pools could be used or user can define
 *  their own functions.
 * 
 *  Queue pre-allocates queue_size memory buffers of size message_size and 
 *  places the received messages in the buffers.
 *  If a message arrives which size is greater than message_size then XSAL 
 *  uses alloc_buf to dynamically allocate a memory buffer (and free_buf 
 *  to release it).
 *
 *  If message_size equals zero then memory buffers are not pre-allocated 
 *  and alloc_buf and free_buf are always used instead.
 *
 *  @note Pre-allocated buffers are fast (memory allocation/de-allocation
 *        is not required) but more memory may be used because queue_size 
 *        buffers are always needed (even if the queue is not full).
 *
 *  @param [in] queue       queue to be created
 *  @param [in] queue_size  defines queue size
 *  @param [in] buffer_size defines size of message
 *  @param [in] alloc_buf   pointer to the function which allocates memory 
 *                          for queue element
 *  @param [in] free_buf    pointer to the function which frees unused queue 
 *                          memory
 *
 *  @return 0 on failure
 *
 *  @pre Function SAL_Init_Queue_Structure() must be called before.
 *
 *  @see SAL_Init_Queue_Structure(), SAL_Destroy_Message_Queue(), 
 *       SAL_I_Post_Message(), SAL_I_Pend_Message(), SAL_I_Release_Message()
 */
bool_t SAL_I_Create_Message_Queue(
   SAL_Message_Queue_T* queue, 
   size_t queue_size,
   size_t buffer_size,
   void* (*alloc_buf)(size_t size),
   void  (*free_buf)(void *buf));


/**
 *  Function destroys message queue and destroys all its messages
 *
 *  @param [in] queue queue to be destroyed
 */
void SAL_I_Destroy_Message_Queue(SAL_Message_Queue_T* queue);


/**
 *  Function post message to the queue.
 *  Messages are prioritized. Urgent messages (is_urgent 
 *  is equal true) are delivered before normal ones.
 *
 *  @note If a message queue is full when an urgent message arrives, the 
 *        last normal message is removed from the queue and the urgent 
 *        message is put into the queue instead. If the queue contains 
 *        only urgent messages, then the arriving urgent message is discarded.
 * 
 *  @param [in]  queue      pointer to queue
 *  @param [in]  message    pointer to message data
 *  @param [in]  is_urgent  true if the message is urgent
 *  @param [out] err        error code if function failed
 *
 *  @return 0 if the message has not been posted to the queue
 */
bool_t SAL_I_Post_Message(
   SAL_Message_Queue_T* queue, 
   const SAL_Message_T* message, 
   bool_t is_urgent,
   SAL_Message_Dropped_Reason_T* err);


/**
 *  Function blocks the calling thread until a message became available 
 *  in the queue and returns a pointer to the message.
 *
 *  When the message is not needed anymore it must be returned to 
 *  the queue calling SAL_Release_Message()
 *
 *  @param [in] queue pointer to the queue which we are waiting 
 *                    for a message from
 * 
 *  @return  pointer to the message
 *
 *  @see SAL_I_Try_Pend_Message, SAL_I_Pend_Message_Timeout,
 *       SAL_Release_Message
 */
SAL_Message_T* SAL_I_Pend_Message(SAL_Message_Queue_T* queue);


/**
 *  Function checks if they is a message available.
 *  If yes it returns pointer to the message otherwise NULL.
 *
 *  Function doesn't block calling thread.
 *
 *  When the message is not needed anymore it must be returned to 
 *  the queue calling SAL_Release_Message()
 *
 *  @param [in] queue pointer to the queue which we are waiting 
 *                    for a message from
 * 
 *  @return  pointer to the message
 *
 *  @see SAL_I_Pend_Message, SAL_I_Pend_Message_Timeout,
 *       SAL_Release_Message
 */
SAL_Message_T* SAL_I_Try_Pend_Message(SAL_Message_Queue_T* queue);


/**
 *  Function blocks the calling thread until a message became available 
 *  in the queue or timeout occures.
 *
 *  When the message is not needed anymore it must be returned to 
 *  the queue calling SAL_Release_Message()
 *
 *  @param [in] queue       pointer to the queue which we are waiting 
 *                          for a message from
 *  @param [in] timeout_ms  timeout value
 * 
 *  @return  pointer to the message
 *
 *  @see SAL_I_Pend_Message, SAL_I_Try_Pend_Message,
 *       SAL_Release_Message
 */
SAL_Message_T* SAL_I_Pend_Message_Timeout(
   SAL_Message_Queue_T* queue, 
   uint32_t timeout_ms);

/**
 *  Function frees memory allocated for message data and puts 
 *  queue node to the free nodes list.
 *
 *  @param [in] message  pointer to message which is 
 *                       given back to the queue
 * 
 *  @see SAL_Pend_Message()
 */
void SAL_I_Release_Message(SAL_Message_T* message);


/**
 *  Function removes message pointed by iterator from message queue.
 *
 *  @param [in] iterator iterator to the message to be deleted from queue.
 */
void SAL_I_Remove_Node_From_Queue(SAL_Queue_Iterator_T iterator);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_MESSAGE_QUEUE_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
