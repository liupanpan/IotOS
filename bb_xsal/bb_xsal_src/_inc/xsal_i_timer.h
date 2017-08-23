/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_timer.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of internal interface of SAL timer
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_TIMER_H
#define XSAL_I_TIMER_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal.h"
#include "xsal_i_thread.h"

#define INVALID_TIMER_EVENT   ((SAL_Event_Id_T) -1)

/* ----------------------------
 *  Exported Type Declarations
 * ---------------------------- */


/** Timer-related data structure
 */
typedef struct SAL_I_Timer_Tag
{
   /** SAL event identifier
    */
   SAL_Event_Id_T event_id;

   /** ID of the thread which is the owner of the timer
    */
   SAL_Thread_Id_T thread_id;

   /** If given timer is used by thread (SAL_Create_Timer was called) then 
    *  this field points to previous timer owned by thread, otherwise it
    *  points to next free timer in the free timers list (with 
    *  SAL_I_Free_Timers_List as the head of the list)
    */
   struct SAL_I_Timer_Tag* prev_thread_timer;

   /** If given timer is used by thread (SAL_Create_Timer was called) then 
    *  this field points to then next timer owned by thread.
    */
   struct SAL_I_Timer_Tag* next_thread_timer;

   /** Variable use_param is true if SAL_Start_Timer_Ex function was called.
    */
   bool_t use_param;

   /** Value of param variable will be put to the message from timer
    *  if use_param value is true.
    */
   uintptr_t param;

#if XSAL_TIMER_IMPL == XSAL_TIMER_USE_PULSES
   timer_t os_timer_id;
#elif XSAL_TIMER_IMPL == XSAL_TIMER_USE_SIGNALS
   struct SAL_I_Timer_Tag* next_timer_by_time;
   struct SAL_I_Timer_Tag* prev_timer_by_time;

   struct timespec expiration_time;
   uint32_t period;
#elif XSAL_TIMER_IMPL == XSAL_TIMER_USE_WIN32_API
   /** Define if the timer is periodic
    */
   bool_t is_periodic;
#else
#error "Not supported platform"
#endif /* QNX_NTO && LINUX && WIN32 */

} SAL_I_Timer_T;


/* ------------------------------
 *  Exported Object Declarations
 * ------------------------------ */


/** Pointer to the SAL Timer's table
 */
extern SAL_I_Timer_T* SAL_I_Timers;

/** Pointer to the SAL Timer's free list
 */
extern SAL_I_Timer_T* SAL_I_Timers_Free_List;

/** Mutex to synchronize access to the SAL Timers table
 *  and SAL Timers free list.
 */
extern SAL_Mutex_T SAL_I_Timers_Mutex;


/* ------------------------------
 *  Exported Function Prototypes
 * ------------------------------ */


/** Function initializes XSAL Timer module.
 *  It allocates memory, creates synchronization objects, ...
 */
bool_t SAL_I_Init_Timer_Module(void);


/** Function releases resources allocated by 
 *  SAL_I_Init_Timer_Module() function.
 */
void SAL_I_Deinit_Timer_Module(void);


/** Function starts module/thread for SAL timer.
 */
void SAL_I_Start_Timer_Module(void);


/** Function stops SAL Thread module/thread.
 */
void SAL_I_Stop_Timer_Module(void);

#if (XSAL_TIMER_IMPL == XSAL_TIMER_USE_SIGNALS) || \
    (XSAL_TIMER_IMPL == XSAL_TIMER_USE_WIN32_API)

SAL_OS_TID_T SAL_I_Get_Timer_Thread_Id(void);

bool_t SAL_I_Get_Timer_Thread_Priority(
   SAL_Priority_T* priority,
   SAL_Sched_Policy_T* sched_policy);
#endif /* XSAL_TIMER_USE_SIGNALS || XSAL_TIMER_USE_WIN32_API */

#if defined (XSAL_LIGHT)
/** Router Thread Light main function.
 */
int32_t SAL_I_RT_Light(void);


/** Function terminates Router Thread Light.
 */
void SAL_I_Stop_RT_Light(int32_t status);
#endif /* XSAL_LIGHT */


/** Function creates timer with given ID.
 */
bool_t SAL_I_Create_Timer(SAL_I_Timer_T* timer);


/** Function starts given timer.
 */
void SAL_I_Start_Timer(
   SAL_I_Timer_T* timer,
   uint32_t interval_msec,
   bool_t is_periodic,
   bool_t use_param,
   uintptr_t param);


/** Function stops given timer.
 */
void SAL_I_Stop_Timer(SAL_I_Timer_T* timer);


/** Function destroys given timer.
 */
void SAL_I_Destroy_Timer(SAL_I_Timer_T* timer);


/** Function creates and post message to the message queue.
 */
void SAL_I_Post_Timer_Event(
   SAL_Event_Id_T event_id,
   SAL_Thread_Id_T thread_id,
   bool_t use_param,
   uintptr_t param);


/** Function binds timer with given id to the given thread.
 */
void SAL_I_Bind_Timer_To_Thread(SAL_I_Timer_T* timer);


/**
 *  Function unbinds timer with given id to the given thread.
 */
void SAL_I_Unbind_Timer_From_Thread(SAL_I_Timer_T* timer);


/** Function destroys all timers created by given thread.
 */
void SAL_I_Destroy_And_Unbind_Timers_From_Thread(SAL_I_Thread_Attr_T* thread_attr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_TIMER_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *    Initial revision.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * -----------------------------------------------------------------------*/
