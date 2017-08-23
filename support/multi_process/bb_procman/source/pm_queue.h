/*--------------------------------------------------------------------------
 * File:           pm_queue.h
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Dominik Schaefer
 *
 * Description:    Interface to FIFO queue.
 * Purpose of the queue is to handle indexes to procman process registry table.
 * Queue have limited size of SAL_MAX_APPS.
 *
 *--------------------------------------------------------------------------
 */
#ifndef PM_QUEUE_H
#define PM_QUEUE_H

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/**
 * Constant used in PM_Queue_Pop function
 *
 */
typedef enum PM_Queue_Status_Tag
{
   PM_QUEUE_EMPTY_ELEMENT = -1,
} PM_Queue_Status_T;

/**
 * Performs initialization steps.
 * This function shall be called once before using queue.
 *
 */
void PM_Queue_Init();

/**
 * Get next queue element.
 *
 * @return next element or PM_QUEUE_EMPTY_ELEMENT if the queue is empty
 */
int PM_Queue_Pop();

/**
 * Add element at the end of the queue.
 * If element already exist in a queue no action is taken.
 * @param element item to be put into the queue.
 *    It shall be >= 0 and < SAL_MAX_APPS.
 */
void PM_Queue_Push(int element);

/**
 * Remove all elements from queue.
 *
 */
void PM_Queue_Clear();

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */

#endif /* PM_QUEUE_H */

/*-----------------------------------------------------------------------------
 * History
 *
 * 20 May. 2009, Dominik Schaefer:  Initial revision.
 *-----------------------------------------------------------------------------*/
