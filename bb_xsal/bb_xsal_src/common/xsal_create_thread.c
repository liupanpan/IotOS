/* -------------------------------------------------------------------------
 *
 * File:        xsal_create_thread.c
 *
 * Copyright:   2006,2011 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Create_Thread() function.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include "string_resource.h"
#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

#if defined (XSAL_POSIX)

#   include <errno.h>
#   include <pthread.h>
#   include <stdlib.h>
#   include <string.h>
#   if defined(LINUX)
#      include <sys/syscall.h>
#      include <unistd.h>
#      include <sys/prctl.h>
#   endif
#   include <unistd.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_THREAD_MODULE_ID, 2); /* Identify file for trace/assert purposes */

/* --------------------------
 *  Local Type Declarations
 * -------------------------- */

#if defined (XSAL_POSIX)

typedef void* SAL_I_Thread_Return_Value_T;

#elif defined (WIN32)

/* DDC #define snprintf _snprintf */

typedef LPTHREAD_START_ROUTINE SAL_I_Thread_Return_Value_T;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX && WIN32 */

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

#if defined (XSAL_LIGHT)

/** Functions: SAL_Lock_Mutex and SAL_Wait_Ready are not defined 
 *  in XSAL_LIGHT external interface but are used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);

void SAL_Wait_Ready(
   const SAL_Thread_Id_T thread_id_list[],
   size_t number_of_items);

#endif /* XSAL_LIGHT */

static SAL_Thread_Id_T SAL_I_Find_Free_Thread_Id(void);

static SAL_I_Thread_Return_Value_T SAL_I_Start_User_Thread(const SAL_I_Thread_Attr_T* thread_attr);

static SAL_Thread_Id_T Create_I_Thread(void(*fnc)(void*), void* param,
   const SAL_Thread_Attr_T* attr);

/* -----------------------------
 *  Local Function Definitions
 * ----------------------------- */

static SAL_Thread_Id_T SAL_I_Find_Free_Thread_Id(void)
{
   bool_t searching = true;
   SAL_Thread_Id_T thread_idx;
   SAL_Thread_Id_T tid = SAL_UNKNOWN_THREAD_ID;

   for (thread_idx = SAL_First_Unknown_Thread_Id; searching && ((size_t) thread_idx
      <= SAL_I_Max_Number_Of_Threads); thread_idx++)
   {
      if (SAL_I_Thread_Table[thread_idx].thread_id == -1)
      {
         tid = thread_idx;
         searching = false;
      }
   }
   return tid;
}

static SAL_I_Thread_Return_Value_T SAL_I_Start_User_Thread(const SAL_I_Thread_Attr_T* thread_attr)
{
   SAL_Thread_Id_T tid = thread_attr->thread_id;
   static SAL_Thread_Id_T wait_ready[] =
   { SAL_ROUTER_THREAD_ID };

   if (!SAL_I_TLS_Set_Specific(SAL_I_Thread_Id_Self, (const void*) thread_attr))
   {
      /** It is not allowed to print any debug messages 
       *  if SAL_I_TLS_Set_Specific failed
       */
      PBC_Failed("No thread-specific data");
      return NULL;
   }

   PBC_Require(thread_attr->current_message == NULL, "Non-NULL current_message");
   PBC_Require(thread_attr->selected_count == 0, "Non-zero selected_count");

#if defined (XSAL_POSIX)

   /** Wait until parent thread finishes its job.
    *  Mutex: SAL_I_Thread_Table_Mutex is locked by the parent thread, 
    *  so the current thread will lock until its parent unlock its
    */
   if (!SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      PBC_Failed("Failed to lock thread table mutex");
      return NULL;
   }
   
#if defined (LINUX)
   /* get Linux Process / Thread ID */
   SAL_I_Thread_Table[tid].os_pid = syscall(SYS_gettid);
#endif 
   
   if (!SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      PBC_Failed("Failed to unlock thread table mutex");
      return NULL;
   }

#if defined (LINUX)
   /* set initial thread priority */
   if (!SAL_I_Set_Thread_Priority(tid, thread_attr->base_priority))
   {
      Tr_Fault_1("Unable to set initial thread priority %d", thread_attr->base_priority);    
   }

   {
      char new_name[SAL_THREAD_NAME_MAX_LENGTH + 8];

      snprintf(new_name, sizeof(new_name), "%02d.%d-%s", (int)SAL_Get_App_Id(), (int)thread_attr->thread_id, thread_attr->thread_name);
      prctl(PR_SET_NAME, new_name);
   }

#endif

#if defined(QNX_NTO)
/**
 * Set thread name in QNX
 */
   {
      char new_name[SAL_THREAD_NAME_MAX_LENGTH + 8];

      snprintf(new_name, sizeof(new_name), "%02d.%d-%s", (int)SAL_Get_App_Id(), (int)thread_attr->thread_id, thread_attr->thread_name);
      pthread_setname_np(0, new_name);
   }
#endif

#endif /* XSAL_POSIX */

   /** Wait for Router Thread
    */
   SAL_Wait_Ready(wait_ready, sizeof(wait_ready) / sizeof(wait_ready[0]));

   /** Call user function
    */
   thread_attr->thread_fnc(thread_attr->thread_param);

   /** User thread treminate.
    *  Now free resources allocated by the thread:
    *  o) "reset" semaphore used in thread startup synchronization
    *  o) timers
    *  o) unsubscribe thread events
    *  o) message queue
    */
   SAL_I_Free_Thread_Resources(tid);

   return NULL;
}

static SAL_Thread_Id_T Create_I_Thread(void(*fnc)(void*), void* param,
   const SAL_Thread_Attr_T* attr)
{
   SAL_Thread_Id_T t_id;
   const char_t* t_name;
   SAL_Priority_T t_priority;
   size_t t_stack_size;

#if defined (XSAL_POSIX)
   pthread_attr_t pt_attr;
#endif /* XSAL_POSIX */

   if (attr == NULL)
   {
      t_id = SAL_UNKNOWN_THREAD_ID;
      t_name = NULL;
      t_priority = SAL_DEFAULT_THREAD_PRIORITY;
      t_stack_size = SAL_DEFAULT_THREAD_STACK_SIZE;
   }
   else
   {
      t_id = attr->id;
      t_name = attr->name;
      t_priority = attr->priority;
      t_stack_size = attr->stack_size;
   }

#if defined (XSAL_POSIX)

   (void)pthread_attr_init(&pt_attr);
   (void)pthread_attr_setdetachstate(&pt_attr, PTHREAD_CREATE_DETACHED);
   
#if defined (LINUX)
   if (t_priority < SAL_REAL_TIME_PRIORITY)
   {
      struct sched_param sparam = {0};

      (void)pthread_attr_setschedpolicy(&pt_attr, SCHED_OTHER);
      sparam.sched_priority = 0;
      (void)pthread_attr_setschedparam(&pt_attr, &sparam);
   }
   else
   {      
      struct sched_param sparam = {0};

      (void)pthread_attr_setschedpolicy(&pt_attr, SCHED_RR);
      sparam.sched_priority = 1;
      (void)pthread_attr_setschedparam(&pt_attr, &sparam);
    }
      
   (void)pthread_attr_setinheritsched(&pt_attr, PTHREAD_EXPLICIT_SCHED);
         
 #else
   (void)pthread_attr_setschedpolicy(&pt_attr, XSAL_SCHED_POLICY);
 #endif
   if (t_stack_size > 0u)
   {
      (void)pthread_attr_setstacksize(&pt_attr, t_stack_size);
   }

#endif /* XSAL_POSIX */

   if (SAL_Lock_Mutex(&SAL_I_Thread_Table_Mutex))
   {
      if (t_id == SAL_UNKNOWN_THREAD_ID)
      {
         t_id = SAL_I_Find_Free_Thread_Id();
      }

      if (t_id > 0)
      {
#if defined (XSAL_POSIX)
         int create_status;
#elif defined (WIN32)
         DWORD os_tid;
         HANDLE thread_handle;
#endif /* XSAL_POSIX && WIN32 */

         SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[t_id];
         PBC_Require(thread_attr->thread_id == SAL_UNKNOWN_THREAD_ID, "Uninitialized thread attribute");

         thread_attr->thread_id = t_id;
         thread_attr->thread_fnc = fnc;
         thread_attr->thread_param = param;
#if defined (LINUX)
         thread_attr->base_priority = t_priority;
#endif /* LINUX */

         /** Copy the thread name given by user to thread table.
          *  If the thread name given by user is longer then 
          *  SAL_THREAD_NAME_MAX_LENGTH-1, it will not be terminated by
          *  the function strncpy. In this case string is terminated 
          *  by the last byte of the table which was set to 0 by the function 
          *  calloc during creation of the thread table.
          */
         if (t_name == NULL)
         {
            (void) snprintf(thread_attr->thread_name, SAL_THREAD_NAME_MAX_LENGTH - 1, "%d",
               (int) t_id);
         }
         else
         {
            (void) Str_Full_Copy(thread_attr->thread_name, t_name, sizeof(thread_attr->thread_name));
         }

         /** "Lock" threads calling SAL_Wait_Destroyed();
          */
         (void) SAL_Try_Wait_Semaphore(&thread_attr->thread_destroyed_sem);

#if defined (XSAL_POSIX)

         create_status = pthread_create(
            &thread_attr->os_tid,
            &pt_attr,
            (void* (*)(void*))SAL_I_Start_User_Thread,
            (void*)thread_attr);

         if (create_status == EOK)
         {
         
/** can not set thread priority until pid is known */
         
#if !defined (LINUX)  
            (void)SAL_I_Set_Thread_Priority(t_id, t_priority);
#endif            
            SAL_I_Number_Of_Running_User_Threads++;
         }
         else
         {
            thread_attr->thread_fnc = NULL;
            thread_attr->thread_id = SAL_UNKNOWN_THREAD_ID;

            /** "Unlock" threads calling SAL_Wait_Destroyed()
             */
            (void)SAL_Signal_Semaphore(&thread_attr->thread_destroyed_sem);

            Tr_Fault_2("Thread_Create %d failed. Error: %d", (int)t_id, (int)create_status);
         }

#elif defined (WIN32)

         thread_handle = CreateThread(
            NULL,
            t_stack_size,
            (LPTHREAD_START_ROUTINE)SAL_I_Start_User_Thread,
            (void*)thread_attr,
            CREATE_SUSPENDED,
            &os_tid);

         if (thread_handle != 0)
         {
            thread_attr->os_tid = thread_handle;
            SAL_I_Set_Thread_Priority(t_id, t_priority);
            SAL_I_Number_Of_Running_User_Threads++;
            ResumeThread(thread_handle);
         }
         else
         {
            thread_attr->thread_fnc = NULL;
            thread_attr->thread_id = SAL_UNKNOWN_THREAD_ID;

            /** "Unlock" threads calling SAL_Wait_Destroyed();
             */
            /**  Thread wasn't started */
            (void)SAL_Signal_Semaphore(&thread_attr->thread_destroyed_sem);

            Tr_Fault_1("Thread_Create %d failed", (int)t_id);
         }

#endif /* WIN32 */
      }
      (void) SAL_Unlock_Mutex(&SAL_I_Thread_Table_Mutex);
   }
   else
   {
      PBC_Failed("Unable to lock thread table mutex");
   }

#if defined (XSAL_POSIX)
   (void)pthread_attr_destroy(&pt_attr);
#endif /* XSAL_POSIX */

   return t_id;
}

/* -----------------------------
 *  Public Function Definitions
 * ----------------------------- */

SAL_Thread_Id_T SAL_Create_Thread(void(*thread_function)(void*), void* param,
   const SAL_Thread_Attr_T* attr)
{
   /** attr must be NULL or thread attr->id must be in valid range:
    *  (id > 0 and id <= SAL_I_Max_Number_Of_Threads) or 
    *  (id == SAL_UNKNOWN_THREAD_ID)
    */
   PBC_Require((attr == NULL) || (attr->id > 0) || (attr->id == SAL_UNKNOWN_THREAD_ID), "Invalid thread id");
   PBC_Require((attr == NULL) || (attr->id <= (SAL_Thread_Id_T)SAL_I_Max_Number_Of_Threads), "Thread id > max");

   return Create_I_Thread(thread_function, param, attr);
}

#if defined (LINUX)
void SAL_Background_Process(void)
{
   if (daemon(0, 1) != 0)
   {
      Tr_Fault("Failed to background process");
   }

   /* Update Linux Process / Thread ID */
   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].os_pid = syscall(SYS_gettid);
}
#endif

/*===========================================================================*\
 * File Revision History (top to bottom: firt revision to last revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 11-Aug-2011 Dan Carman
 *    Modified Linux to use both Real-Time Round Robin and Completely Fair Schedulers
 *
 * 19-Jul-2013 Larry Piekarski
 * - Task 161388: Removed deprecated util functions.
 *
 * 27-May-2015 Kirk Bailey
 *    - Filled in unresolved comments.
 *
 * 15-Sept-2015 Dan Carman
 *     - Task kok_basa#222438 Set QNX Thread name similar to Linux 
 *
\*===========================================================================*/
