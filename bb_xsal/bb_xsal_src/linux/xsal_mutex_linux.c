/*===========================================================================*/
/**
 * @file xsal_mutex_linux.c
 *
 * Handles mutexes for Linux
 *
 * %full_filespec:xsal_mutex_linux.c~kok_inf#6:csrc:kok_basa#1 %
 * @version %version:kok_inf#6 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Mar 16 14:12:23 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Handles mutexes for Linux.  
 *   Linux pthread mutex do not guarantee fairness 
 *    Released mutexes are not granted immediate to equal priority waiting thread. This
 *    can lead to starvation of one of the threads.
 *   The approach here is add a reference count of the number of lock attempts. At unlock, if there
 *   are threads waiting, then the current thread sleeps giving the other threads a chance to acquire the mutex.
 *   If not threads are waiting, then the current thread continues to its time slot.
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "pbc_trace.h"
#include "reuse.h"
#include "xsal.h"
#include "xsal_i_time.h"
#include <errno.h>
#include <string.h>

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/
#undef EM_F_ID
#define EM_F_ID EM_FILE_ID(XSAL_MUTEX_MODULE_ID, 1) /**< Identifies file for PbC/trace */

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
/**
 * Sync intrinics are not available for ARM before GCC 4.4
 */
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/**
 * Increment Reference Count of number threads attempting to lock mutex
 */
static void Inc_Ref_Count(SAL_Mutex_T * mutex)
{
   /* Test for GCC > 4.4.0 */
#if GCC_VERSION < 40400
   mutex->count++;
#else
   __sync_add_and_fetch(&(mutex->count), 1);
#endif
}

/**
 * Decrement Reference Count of number threads attempting to lock mutex
 *
 * @return The update count
 */
static uint32_t Dec_Ref_Count(SAL_Mutex_T * mutex)
{
   /* Test for GCC > 4.4.0 */
#if GCC_VERSION < 40400
   return --mutex->count;
#else
   return __sync_sub_and_fetch(&(mutex->count), 1);
#endif
}

/* -----------------------------
 *  Public function definition
 * -----------------------------*/

/*
 * Create Mutex
 */
bool_t SAL_Create_Mutex(SAL_Mutex_T* mutex, const SAL_Mutex_Attr_T* attr)
{
   SAL_Int_T status;

   PBC_Require((mutex != NULL), "NULL mutex");

   mutex->count = 0;

   if (NULL == attr)
   {
      /** Create mutex with default (system dependent) settings
       */
      status = pthread_mutex_init(&(mutex->mutex), NULL);
   }
   else
   {
      pthread_mutexattr_t ptmutex_attr;
      status = pthread_mutexattr_init(&ptmutex_attr);
      if (EOK == status)
      {
         if (attr->recursive)
         {
            status = pthread_mutexattr_settype(&ptmutex_attr, PTHREAD_MUTEX_RECURSIVE);
         }
         if (EOK == status)
         {
            status = pthread_mutex_init(&(mutex->mutex), &ptmutex_attr);
         }
         (void) pthread_mutexattr_destroy(&ptmutex_attr);
      }
   }

   if (status != EOK)
   {
      PBC_Failed_1("SAL_Create_Mutex error: %s", strerror(status));
   }

   return (bool_t) (status == EOK);
}

/*
 *  Destroy a Mutex
 */
bool_t SAL_Destroy_Mutex(SAL_Mutex_T* mutex)
{
   SAL_Int_T status;

   PBC_Require((mutex != NULL), "NULL mutex");

   if (mutex->count != 0)
   {
      Tr_Warn_1("Destroying Mutex with %d active users", mutex->count);
      mutex->count = 0;
   }
   status = pthread_mutex_destroy(&(mutex->mutex));

   PBC_Ensure_1((EOK == status), "SAL_Destroy_Mutex error: %s", strerror(status));

   return (bool_t) (EOK == status);
}

/*
 * Lock Mutex
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex)
{
   SAL_Int_T status;

   PBC_Require(mutex != NULL, "NULL Mutex");

   Inc_Ref_Count(mutex);

   status = pthread_mutex_lock(&(mutex->mutex));

   if (EOK != status)
   {
      Dec_Ref_Count(mutex);
      PBC_Failed_1("SAL_Lock_Mutex error: %s", strerror(status));
   }
   else
   {
      mutex->owner = SAL_Get_Thread_Id();
   }

   return (bool_t) (EOK == status);
}

/*
 *  Lock mutex with timeout
 */
bool_t SAL_Lock_Mutex_Timeout(SAL_Mutex_T* mutex, uint32_t timeout_msec)
{
   SAL_Int_T status;
   SAL_I_Time_Spec_T timeout;

   PBC_Require(mutex != NULL, "NULL Mutex");

   Inc_Ref_Count(mutex);

   SAL_I_Get_Time(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   status = pthread_mutex_timedlock(&(mutex->mutex), &timeout);

   if (EOK != status)
   {
      Dec_Ref_Count(mutex);

      if (ETIMEDOUT != status)
      {
         PBC_Failed_1("SAL_Lock_Mutex_Timeout error: %s", strerror(status));
      }
      else
      {
         Tr_Warn_2("SAL_Lock_Mutex_Timeout - Timed out after %d mS. Mutex Owner: %d", timeout_msec, mutex->owner);
       }
   }
   else
   {
      mutex->owner = SAL_Get_Thread_Id();
   }

   return (bool_t) (EOK == status);
}

/*
 *  Try Lock Mutes
 */
bool_t SAL_Try_Lock_Mutex(SAL_Mutex_T* mutex)
{
   SAL_Int_T status;

   PBC_Require(mutex != NULL, "NULL Mutex");

   Inc_Ref_Count(mutex);

   status = pthread_mutex_trylock(&(mutex->mutex));

   if (EOK != status)
   {
      Dec_Ref_Count(mutex);

      if (status == EBUSY)
      {
         Tr_Info_Lo("SAL_Try_Lock_Mutex: BUSY");
      }
      else
      {
         PBC_Failed_1("SAL_Try_Lock_Mutex error: %s", strerror(status));
      }
   }

   return (bool_t) (EOK == status);
}

/*
 *   Unlock mutex
 */
bool_t SAL_Unlock_Mutex(SAL_Mutex_T* mutex)
{
   SAL_Int_T status;

   PBC_Require(mutex != NULL, "NULL Mutex");

   status = pthread_mutex_unlock(&(mutex->mutex));

   if (EOK == status)
   {
      if (Dec_Ref_Count(mutex) > 0)
      {
         /** allow waiting threads the opportunity to get mutex */
#if   defined(__i386__) || defined(__x86_64__)
         (void) usleep(50u);
#else
         sched_yield();
#endif
      }
   }
   else
   {
      PBC_Failed_1("SAL_Unlock_Mutex error: %s", strerror(status));
   }

   return (bool_t) (EOK == status);
}

/*===========================================================================*/
/*!
 * @file xsal_mutex_linux.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 19-Jul-2013 Larry Piekarski
 * - Task 161388: Fixed build warning
 *
 * - 20-Jan-2012 Dan Carman
 *   - Created initial file.
 *
 * 16-Mar-2015 Dan Carman
 * + Task 21696 Added compiler switch for either 32 / 64 bit x86 Linux builds
 */
/*===========================================================================*/
