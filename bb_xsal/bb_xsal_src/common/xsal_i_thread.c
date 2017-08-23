/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_thread_c.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Common functions for XSAL thread functionality.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include "string_resource.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_engine.h"
#include "xsal_i_event_property.h"
#include "xsal_i_thread.h"
#include "xsal_i_timer.h"
#include "xsal_i_publish.h"

#if defined (XSAL_POSIX)

#   include <errno.h>
#   include <pthread.h>
#   include <stdlib.h>
#   include <string.h>
#   include <unistd.h>

#if defined (LINUX)
#   include <sys/resource.h>
#   include <sys/syscall.h>
#   include <sys/types.h>
#   include <unistd.h> 
#endif /* LINUX */

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_THREAD_MODULE_ID, 0); /* Identify file for trace/assert purposes */

/*  Functions: SAL_Lock_Mutex and SAL_Wait_Semaphore are not defined in 
 *  XSAL_LIGHT external interface but are used internally by the XSAL. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);

/* -------------------------------------------------------------------------
 *
 *                               Internal objects
 *
 * -----------------------------------------------------------------------*/

/** Thread Local Storage Key with SAL Thread ID.
 */
SAL_TLS_Key_T SAL_I_Thread_Id_Self;

/** Number of currently running SAL threads started by user.
 */
size_t SAL_I_Number_Of_Running_User_Threads;

/** Pointer to table with thread attributes
 */
SAL_I_Thread_Attr_T* SAL_I_Thread_Table;

/** Mutex to synchronize access to SAL_I_Thread_Table
 */
SAL_Mutex_T SAL_I_Thread_Table_Mutex;

#if defined (XSAL_POSIX)

typedef void* THREAD_RETURN_VALUE;

#elif defined (WIN32)

typedef LPTHREAD_START_ROUTINE THREAD_RETURN_VALUE;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */

/* -------------------------------------------------------------------------
 *
 *                           Internal functions
 *
 * -----------------------------------------------------------------------*/

#if defined (XSAL_POSIX)

#define SAL_I_Get_OS_TID()  pthread_self()

#elif defined (WIN32)

#define SAL_I_Get_OS_TID()  GetCurrentThread()

#else
#error "Not supported platform"
#endif

bool_t SAL_I_TLS_Key_Create(SAL_TLS_Key_T* tls_key)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status = pthread_key_create(tls_key, NULL);
   if (status != 0)
   {
      Tr_Fault_1("SAL_I_TLS_Create error: %s", strerror(status));
   }
   return (bool_t)(status == 0);

#elif defined (WIN32)

   bool_t status;
   DWORD key = TlsAlloc();

   if (key != TLS_OUT_OF_INDEXES)
   {
      *tls_key = key;
      status = true;
   }
   else
   {
      PBC_Failed_1("SAL_I_TLS_Create error: %d", (int) GetLastError());
      status = false;
   }

   return status;

#else
#error "Not supported platform"
#endif
}

bool_t SAL_I_TLS_Key_Delete(SAL_TLS_Key_T tls_key)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status = pthread_key_delete(tls_key);
   if (status != 0)
   {
      Tr_Fault_1("SAL_I_TLS_Key_Delete error: %s", strerror(status));
   }
   return (bool_t)(status == 0);

#elif defined (WIN32)

   bool_t status = (bool_t)(TlsFree(tls_key) != 0);
   PBC_Ensure_1(status, "SAL_I_TLS_Key_Delete error: %d", (int)GetLastError());

   return status;

#else
#error "Not supported platform"
#endif
}

bool_t SAL_I_TLS_Set_Specific(SAL_TLS_Key_T tls_key, const void* value)
{
#if defined (XSAL_POSIX)

   bool_t status = (bool_t)(pthread_setspecific(tls_key, value) == 0);
   if (!status)
   {
      Tr_Fault_3("SAL_I_TLS_Set_Specific(%d, %p) error: %s",
         (int)tls_key, (void*)value, strerror(status));
   }

   return status;

#elif defined (WIN32)

   BOOL status = TlsSetValue(tls_key, (LPVOID)value);
   PBC_Ensure_3(status != FALSE,
      "SAL_I_TLS_Create(%d, %p) error: %d",
      (int)tls_key, (void*)value, (int) GetLastError());

   return status != FALSE;

#else
#error "Not supported platform"
#endif
}

void* SAL_I_TLS_Get_Specific(SAL_TLS_Key_T tls_key)
{
#if defined (XSAL_POSIX)

   return pthread_getspecific(tls_key);

#elif defined (WIN32)

   return (void*)TlsGetValue(tls_key);

#else
#error "Not supported platform"
#endif
}

bool_t SAL_I_Init_Thread_Module(void)
{
   size_t thread_idx;
   static const char_t* Router_Thread_Name = "RT";
   SAL_Semaphore_Attr_T sem_attr;

   if (!SAL_Create_Mutex(&SAL_I_Thread_Table_Mutex, NULL))
   {
      Tr_Fault("SAL_I_Init_Threads_Module: SAL_Create_Mutex");
      return false;
   }

   /* Add 1 to the Max_Number_Of_Threads, because there is one "system"
    * thread - RouterThread.
    */
   SAL_I_Thread_Table = (SAL_I_Thread_Attr_T*) calloc(SAL_I_Max_Number_Of_Threads + 1u, sizeof(SAL_I_Thread_Attr_T));
   if (SAL_I_Thread_Table == NULL)
   {
      Tr_Fault("Cannot allocate memory for thread attributes table");
      return false;
   }

   SAL_Init_Semaphore_Attr(&sem_attr);
   sem_attr.initial_value = 1;

   for (thread_idx = 0; thread_idx <= SAL_I_Max_Number_Of_Threads; thread_idx++)
   {
      SAL_I_Thread_Table[thread_idx].thread_id = SAL_UNKNOWN_THREAD_ID;
      SAL_I_Thread_Table[thread_idx].is_ready = false;

      if (!SAL_Create_Semaphore(&SAL_I_Thread_Table[thread_idx].thread_ready_sem, 0) || !SAL_Create_Semaphore(
         &SAL_I_Thread_Table[thread_idx].thread_destroyed_sem, &sem_attr))
      {
         Tr_Fault("Cannot create semaphore for thread startup synchronization");
         return false;
      }

      /** Initialize queue structures. Router thread doesn't have queue.
       */
      if (thread_idx != (size_t) SAL_ROUTER_THREAD_ID)
      {
         if (!SAL_I_Init_Queue_Structure(&SAL_I_Thread_Table[thread_idx].message_queue))
         {
            Tr_Fault("Cannot initialize queue structure");
            return false;
         }
      }
      SAL_I_Thread_Table[thread_idx].current_message = NULL;
      SAL_I_Thread_Table[thread_idx].current_rx_only_message = NULL;

      /** Initialize thread timers list
       */
      SAL_I_Thread_Table[thread_idx].thread_timer_id = -1;
   }

   /** Assign OS thread ID to the current thread.
    */

   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].os_tid = SAL_I_Get_OS_TID();
#if defined (LINUX)
   /* get Linux Process / Thread ID */
   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].os_pid = syscall(SYS_gettid);
#endif

   /** Create Local Thread Storage key and assign SAL thread id 
    *  (SAL_ROUTER_THREAD_ID) to the current thread.
    */
   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_id = 0;

   /** Copy the thread name given by user to thread table.
    *  If the thread name given by user is longer then 
    *  SAL_THREAD_NAME_MAX_LENGTH-1, it will not be terminated by
    *  the function strncpy. In this case string is terminated 
    *  by the last byte of the table which was set to 0 by the function 
    *  calloc during creation of the thread table.
    */
   (void) Str_Full_Copy(SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_name, Router_Thread_Name, sizeof(SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_name));

   if (!SAL_I_TLS_Key_Create(&SAL_I_Thread_Id_Self) || !SAL_I_TLS_Set_Specific(SAL_I_Thread_Id_Self, SAL_I_Thread_Table))
   {
      Tr_Fault("Init_Threads_Module: couldn't create or initialize TLS");
      return false;
   }

   return true;
}

void SAL_I_Deinit_Thread_Module(void)
{
   size_t thread_idx;

   (void) SAL_I_TLS_Key_Delete(SAL_I_Thread_Id_Self);

   for (thread_idx = 0; thread_idx <= SAL_I_Max_Number_Of_Threads; thread_idx++)
   {
      (void) SAL_Destroy_Semaphore(&SAL_I_Thread_Table[thread_idx].thread_ready_sem);
      (void) SAL_Destroy_Semaphore(&SAL_I_Thread_Table[thread_idx].thread_destroyed_sem);

      if (thread_idx != (size_t) SAL_ROUTER_THREAD_ID)
      {
         SAL_I_Deinit_Queue_Structure(&SAL_I_Thread_Table[thread_idx].message_queue);
      }
   }

   free(SAL_I_Thread_Table);
   (void) SAL_Destroy_Mutex(&SAL_I_Thread_Table_Mutex);
}

/** Thread terminates itself
 */
void SAL_I_Free_Thread_Resources(SAL_Thread_Id_T thread_id)
{
   SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[thread_id];

   Tr_Info_Mid_1("Destroy_Thread_Itself %d", (int)thread_id);

   SAL_I_Destroy_And_Unbind_Timers_From_Thread(thread_attr);

   if (SAL_Lock_Mutex(&SAL_I_Event_Property_Mutex))
   {
      SAL_I_Unsubscribe_Thread_Events(thread_id);
      (void) SAL_Unlock_Mutex(&SAL_I_Event_Property_Mutex);
   }
   else
   {
      PBC_Failed("Unable to lock property mutex");
   }

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      /*
       * If this is the last XSAL thread then cancel Router Thread loop. This
       * will lead to SAL_Run() exiting.
       */
      SAL_I_Number_Of_Running_User_Threads--;

      if (SAL_I_Number_Of_Running_User_Threads == 0u)
      {
         SAL_I_Stop_Router_Thread(0);
      }

      /** "Reset" semaphore used in thread startup synchronization
       */
      if (thread_attr->is_ready)
      {
         (void) SAL_Wait_Semaphore(&thread_attr->thread_ready_sem);
         thread_attr->is_ready = false;
      }

      thread_attr->thread_id = SAL_UNKNOWN_THREAD_ID;

      if (thread_attr->current_message != NULL)
      {
         SAL_I_Release_Message(thread_attr->current_message);
         thread_attr->current_message = NULL;
      }
      if (thread_attr->current_rx_only_message != NULL)
      {
         SAL_I_Release_Message(thread_attr->current_rx_only_message);
         thread_attr->current_rx_only_message = NULL;
      }

      /** Do not synchronize destruction of our own queue
       */
      SAL_I_Destroy_Message_Queue(&thread_attr->message_queue);
      thread_attr->selected_count = 0;

      thread_attr->thread_fnc = NULL;

      (void) SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);
   }

   /** "Unlock" threads calling SAL_Wait_Destroyed();
    */
   (void) SAL_Signal_Semaphore(&SAL_I_Thread_Table[thread_id].thread_destroyed_sem);
}

void SAL_I_Exit_Thread(void)
{
#if defined (XSAL_POSIX)

   pthread_exit(NULL);

#elif defined (WIN32)

   ExitThread(0);

#else
#error "Not supported platform"
#endif
}

/** Returns pointer to calling thread data.
 */
SAL_I_Thread_Attr_T* SAL_I_Get_Thread_Attr(void)
{
   return (SAL_I_Thread_Attr_T*) SAL_I_TLS_Get_Specific(SAL_I_Thread_Id_Self);
}

/** Returns SAL Thread Id.
 */
SAL_Thread_Id_T SAL_I_Get_Thread_Id(void)
{
   SAL_I_Thread_Attr_T* thread_attr = (SAL_I_Thread_Attr_T*) SAL_I_TLS_Get_Specific(SAL_I_Thread_Id_Self);

   return (thread_attr != NULL) ? thread_attr->thread_id : SAL_ROUTER_THREAD_ID;
}

bool_t SAL_I_Set_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T priority)
{
#if defined (XSAL_POSIX)

   bool_t status = false;
   SAL_Int_T policy;
   struct sched_param sched_p;

   if (pthread_getschedparam(SAL_I_Thread_Table[thread_id].os_tid, &policy, &sched_p) == EOK)
   {

#if defined (LINUX)
      if (priority < SAL_REAL_TIME_PRIORITY)
      {
         policy = SCHED_OTHER; /* use CFS scheduler */
         sched_p.sched_priority = 0; /* static level must be 0 */
      }
      else
      {
         policy = SCHED_RR;
         sched_p.sched_priority = priority - (SAL_REAL_TIME_PRIORITY - 1);
      }
#else
      policy = XSAL_SCHED_POLICY;
      sched_p.sched_priority = priority;
#endif

      if (pthread_setschedparam(SAL_I_Thread_Table[thread_id].os_tid, policy, &sched_p) == EOK)
      {
         status = true;

#if defined (LINUX)
         if (priority < SAL_REAL_TIME_PRIORITY) /* set NICE level */
         {
            int nice_level = (SAL_REAL_TIME_PRIORITY/2) - priority;

            if (0 != setpriority(PRIO_PROCESS, SAL_I_Thread_Table[thread_id].os_pid, nice_level))
            {

               if (nice_level < 0)
               {
                  /* Most likely reason for failure is lack of permission to set negative nice (needs root) */
                  if (0 != setpriority(PRIO_PROCESS, SAL_I_Thread_Table[thread_id].os_pid, 0))
                  {
                     status = false;
                     Tr_Fault_4("SAL_I_Set_Thread_Priority failed: setprioriy [SAL tid=%d, OS PID=%d, nice =%d], %s",
                        (int)thread_id, SAL_I_Thread_Table[thread_id].os_pid, nice_level, strerror(errno));

                  }
                  else
                  {
                     Tr_Warn_1("Couldn't set negative nice %d. Using nice 0.", nice_level);
                  }
               }
               else
               {
                  status = false;
                  Tr_Fault_4("SAL_I_Set_Thread_Priority failed: setprioriy [SAL tid=%d, OS PID=%d, nice =%d], %s",
                     (int)thread_id, SAL_I_Thread_Table[thread_id].os_pid, nice_level, strerror(errno));
               }
            }
         }
#endif
      }
      else
      {
         Tr_Fault_4("SAL_I_Set_Thread_Priority failed: setschedparam [tid=%d, sc=%d, prio=%d], %s",
            (int)thread_id, (int)policy, (int)priority, strerror(errno));
      }
   }
   else
   {
      Tr_Fault_3("SAL_I_Set_Thread_Priority failed: getschedparam [tid=%d, prio=%d], %s",
         (int)thread_id, (int)priority, strerror(errno));
   }

   return status;

#elif defined (WIN32)

   bool_t status = true;

   if (!SetThreadPriority(SAL_I_Thread_Table[thread_id].os_tid, priority))
   {
      Tr_Fault_3("SAL_I_Set_Thread_Priority failed ([tid=%d, prio=%d], %d",
         (int)thread_id, (int)priority, (int)GetLastError());
      status = false;
   }

   return status;

#endif /* WIN32 */
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 19-Jul-2013 Larry Piekarski
 * - Task 161388: Removed deprecated util functions.
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
