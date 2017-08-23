/* -------------------------------------------------------------------------
 *
 * File:        pm_os_native_thread.c
 *
 * Copyright:   2013 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman(at)delphi.com)
 *
 * Description: Implementation of the Create_OS_Native_Thread() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal_settings.h"
#include "xsal.h"
#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#if defined (XSAL_POSIX)
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#endif /* XSAL_POSIX */

#if defined(LINUX)
#include <sys/syscall.h>    /* For SYS_xxx definitions */
#endif

#if defined(WIN32)
#include <windows.h>
#elif defined(LINUX)
#include <sys/resource.h>
#endif /* LINUX */

EM_FILENUM(PROCMAN_MODULE_ID, 5);

/* --------------------------
 *  Local Type Declarations
 * -------------------------- */

typedef struct Create_OS_Native_Thread_Data_Tag
{
      void (*thread_main)(void*);
      void* thread_main_param;
#if defined(LINUX)
      int thread_initial_priority;
#endif /* LINUX */
} Create_OS_Native_Thread_Data_T;

#if defined (XSAL_POSIX)
static void* OS_Native_Thread_Main(void* thread_attr);
#elif defined (WIN32)
static DWORD WINAPI OS_Native_Thread_Main(void* thread_attr);
#else
#error "Not supported platform"
#endif /* XSAL_POSIX && WIN32 */

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

#if defined(LINUX)

#define Get_Linux_Tid()  ((pid_t)syscall(SYS_gettid))
/*
 pid_t Get_Linux_Tid(void)
 {
 return (pid_t)syscall(SYS_gettid);
 }
 */

static void Set_Priority_If_Other_Sched_Policy(pid_t t_id, SAL_Priority_T priority)
{
   int policy;
   struct sched_param sched_p;
   SAL_Int_T status = pthread_getschedparam(pthread_self(), &policy, &sched_p);

   if (status == EOK)
   {
      if (policy == SCHED_OTHER)
      {
         if (setpriority(PRIO_PROCESS, t_id, priority) != 0)
         {
            Tr_Fault_2("setpriority(%d, %d) failed", t_id, priority);
         }
      }
   }
   else
   {
      Tr_Fault_1("Set_Priority_If_Other_Sched_Policy/pthread_getschedparam failed: %d", status);
   }
}
#endif /* LINUX */

#if defined (XSAL_POSIX)
static void* OS_Native_Thread_Main(void* thread_attr_param)
#else
static DWORD WINAPI OS_Native_Thread_Main(void* thread_attr_param)
#endif
{
   Create_OS_Native_Thread_Data_T * thread_attr = (Create_OS_Native_Thread_Data_T *) thread_attr_param;
   /* Save thread_attr to local variables and next free memory allocated parent thread.
    */
   void (*thread_main)(void*) = (thread_attr)->thread_main;
   void *thread_main_param = (thread_attr)->thread_main_param;
#if defined(LINUX)
   Set_Priority_If_Other_Sched_Policy(Get_Linux_Tid(), thread_attr->thread_initial_priority);
#endif /* LINUX */
   free(thread_attr_param);

   /* Call user defined thread function
    */
   thread_main(thread_main_param);
   return 0;
}

/* ----------------------------
 *  Public function
 * ---------------------------- */

bool Create_OS_Native_Thread(void (*function)(void*), int priority, void* param)
{
   bool status = false;

#if defined (XSAL_POSIX)
   Create_OS_Native_Thread_Data_T* thread_data;

   int create_status;
   pthread_attr_t pt_attr;
   struct sched_param s_param =
      {  priority};
   pthread_t t_id;

   (void) pthread_attr_init(&pt_attr);
   (void) pthread_attr_setdetachstate(&pt_attr, PTHREAD_CREATE_DETACHED);
   (void) pthread_attr_setinheritsched(&pt_attr, PTHREAD_EXPLICIT_SCHED);
   (void) pthread_attr_setschedparam(&pt_attr, &s_param);
   (void) pthread_attr_setschedpolicy(&pt_attr, XSAL_SCHED_POLICY);

   thread_data = malloc(sizeof(Create_OS_Native_Thread_Data_T));
   if (thread_data != NULL)
   {
      thread_data->thread_main = function;
      thread_data->thread_main_param = param;
#if defined(LINUX)
      thread_data->thread_initial_priority = priority;
#endif /* LINUX */
      create_status = pthread_create(&t_id, &pt_attr, OS_Native_Thread_Main, (void*) thread_data);
      status = create_status == EOK;
      if (!status)
      {
         Tr_Fault_1("Create_OS_Native_Thread failed. pthread_create returns %d", create_status);
         free(thread_data);
      }
   }
   else
   {
      Tr_Fault("Create_OS_Native_Thread failed. malloc returns NULL");
   }
   (void) pthread_attr_destroy(&pt_attr);

#elif defined (WIN32)

   DWORD os_tid;
   HANDLE thread_handle;

   Create_OS_Native_Thread_Data_T* thread_data = malloc(sizeof(Create_OS_Native_Thread_Data_T));
   if (thread_data != NULL)
   {
      thread_data->thread_main = function;
      thread_data->thread_main_param = param;
      thread_handle = CreateThread(NULL, 0, OS_Native_Thread_Main, thread_data, CREATE_SUSPENDED, &os_tid);
      if (thread_handle != 0)
      {
         status = SetThreadPriority(thread_handle, priority);
         if (status)
         {
            ResumeThread(thread_handle);
         }
         else
         {
            /* Destroy thread */
            Tr_Fault_1("Create_OS_Native_Thread failed. SetThreadPriority returns %d", status);
            free(thread_data);
            CloseHandle(thread_handle);
         }
      }
      else
      {
         Tr_Fault("Create_OS_Native_Thread failed");
         free(thread_data);
      }
   }
   else
   {
      Tr_Fault("Create_OS_Native_Thread failed. malloc returns NULL");
   }
#endif /* WIN32 */
   return status;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 03 December 2013, Jacek Roman (jacek.roman(at)delphi.com)
 *   Initial revision.
 *
 * 14-May-2015 Kirk Bailey
 *   Fixed Cygwin build errors.
 *
 * -----------------------------------------------------------------------*/
