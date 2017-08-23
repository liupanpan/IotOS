/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_thread.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of internal interface of SAL thread.
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_THREAD_H
#define XSAL_I_THREAD_H

#include "xsal_i_message_queue.h"

/* -------------------------------------------------------------------------
 *
 *                         Internal data structures
 *
 * -----------------------------------------------------------------------*/


typedef void (* SAL_Thread_Fnc_T)(void*);


/**
 *  SAL thread attributes
 */
typedef struct SAL_I_Thread_Attr_Tag
{
   /** Main function of the thread
    */
   SAL_Thread_Fnc_T thread_fnc;

   /** Thread function parameter
    */
   void* thread_param;

   /** Thread name
    */
   char_t thread_name[SAL_THREAD_NAME_MAX_LENGTH];

   /** SAL thread id
    */
   SAL_Thread_Id_T thread_id;

   /** OS thread id
    */
   SAL_OS_TID_T os_tid;

   /** Thread's queue
    */
   SAL_Message_Queue_T message_queue;

   /** Current thread's message
    */
   SAL_Message_T* current_message;

   /** Current thread's receive only message
    */
   SAL_Message_T* current_rx_only_message;

   /** Semaphore to synchronize threads startup
    */
   SAL_Semaphore_T thread_ready_sem;

   /** Number of selected events
    */
   size_t selected_count;

   /** 'is_ready' means whether function SAL_Signal_Ready was called or not
    */
   bool_t is_ready;

   /** Semaphore to synchronize threads destroy
    */
   SAL_Semaphore_T thread_destroyed_sem;

   /** ID of the first timer owned by thread
    */
   SAL_Timer_Id_T thread_timer_id;

#if defined (LINUX)

   /**
    *  Thread's Linux TID
    */
   SAL_OS_PID_T  os_pid;

   /**
    * Initial thread priority
    *    Use be thread to set it own priority at startup.
    */
   SAL_Priority_T base_priority;

#else
   /** Pointer to the first timer owned by thread
    */
   struct SAL_I_Timer_Tag* thread_timers;
#endif 

} SAL_I_Thread_Attr_T;



/* -------------------------------------------------------------------------
 *
 *                               Internal objects
 *
 * -----------------------------------------------------------------------*/


/** Thread Local Storage Key with SAL Thread ID.
 */
extern SAL_TLS_Key_T SAL_I_Thread_Id_Self;


/** Number of currently running SAL threads started by user.
 */
extern size_t SAL_I_Number_Of_Running_User_Threads;


/** Pointer to table with thread attributes
 */
extern SAL_I_Thread_Attr_T* SAL_I_Thread_Table;


/** Mutex to synchronize access to SAL_I_Thread_Table
 */
extern SAL_Mutex_T  SAL_I_Thread_Table_Mutex;


/* -------------------------------------------------------------------------
 *
 *                               Internal functions
 *
 * -----------------------------------------------------------------------*/


/** Function creates Thread Local Storage (TLS) key.
 */
bool_t SAL_I_TLS_Key_Create(SAL_TLS_Key_T* tls_key);


/** Function deletes Thread Local Storage (TLS) key.
 */
bool_t SAL_I_TLS_Key_Delete(SAL_TLS_Key_T tls_key);


/** Function assigns value to key in the Thread Local Storage.
 */
bool_t SAL_I_TLS_Set_Specific(SAL_TLS_Key_T tls_key, const void* value);


/** Function returns value of the key from the Thread Local Storage.
 */
void* SAL_I_TLS_Get_Specific(SAL_TLS_Key_T tls_key);


/** Function sets priority for given thread.
 */
bool_t SAL_I_Set_Thread_Priority(
   SAL_Thread_Id_T thread_id, 
   SAL_Priority_T priority);


/** Function gets priority for given thread.
 */
bool_t SAL_I_Get_Thread_Priority(
   SAL_Thread_Id_T thread_id, 
   SAL_Priority_T* priority);


/** Returns SAL Thread Id.
 */
SAL_Thread_Id_T  SAL_I_Get_Thread_Id(void);


/** Returns pointer to the calling thread attributes
 */
SAL_I_Thread_Attr_T*  SAL_I_Get_Thread_Attr(void);


/** Function initializes thread module. It is called by SAL_Init()
 */
bool_t SAL_I_Init_Thread_Module(void);


/** Function de-initializes thread module.
 */
void SAL_I_Deinit_Thread_Module(void);


/** Function frees thread's resources
 */
void SAL_I_Free_Thread_Resources(SAL_Thread_Id_T thread_id);


/** Function terminates calling thread
 */
void SAL_I_Exit_Thread(void);


#if defined (LINUX)
/**
 * Used before SAL_Run to move process to background
 *    Useful for ensuring some initialization is finished before starting other proceses
 *    Intended to ensure that procman has created shared memory for name server.
 */
void SAL_Background_Process(void);

#endif

#endif /* XSAL_I_THREAD_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *    Initial revision.
 *
 * 16-Mar-2015 Dan Carman
 * + Task 21696 Used Timer Id Type
 *
 * -----------------------------------------------------------------------*/

