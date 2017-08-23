#ifndef XSAL_DATA_TYPES_LINUX_H
#   define XSAL_DATA_TYPES_LINUX_H
/*===========================================================================*/
/**
 * @file xsal_data_types_linux.h
 *
 * Defines basic XSAL data types for Linux.
 *
 * %full_filespec:xsal_data_types_linux.h~kok_inf#13:incl:kok_aud#1 %
 * @version %version:kok_inf#13 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Mar 16 14:16:17 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007,2011 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 *  This has XSAL data type declarations that are unique to Linux.
 *
 * ABBREVIATIONS:
 *   - XSAL = eXtended System Abstraction Layer
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     - XSAL_API.doc March 1, 2007.
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%
20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup xsal_data_types_grp SAL OS Specific data types
 * @{
 */
/*==========================================================================*/


/*===========================================================================*\
 * Header Files
\*===========================================================================*/

#   include <pthread.h>
#   include <semaphore.h>
#   include <sys/uio.h>
#   include <sys/types.h>
#   include <limits.h>
#   include <unistd.h> 

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

#   if !defined(XSAL_PRIORITY_REAL_TIME)
/**
 * define split between (Completely fair scheduler) and Real time round robin 
 *    Priorities CFS uses Nice values 20 to -19, RR uses  1-100
 */
#define SAL_REAL_TIME_PRIORITY  40
#   endif /* SAL_REAL_TIME_PRIORITY */
 
/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * SAL_Buffer_Pool_Id_T defines the type used to identify buffer pools.
 */
typedef SAL_Int_T SAL_Buffer_Pool_Id_T;

/**
 * SAL_Priority_T defines the type used to store thread priorities.
 *
 * LINUX Priority range from 0 (lowest) to 138 (highest)
 *
 *   0 - 39 are Completely Fair Scheduler with Nice value of 20 to -19
 *  40 to 138 are Real-time Round Robin with priority 1-99
 */
typedef SAL_Int_T SAL_Priority_T;

/**
 * Define the highest possible task priority value  
 */
#define SAL_MAX_TASK_PRIORITY    138

/**
 * Define the lowest possible task priority value
 */
#define SAL_MIN_TASK_PRIORITY    0


/**
 * SAL_Thread_Id_T defines the type used to identify threads.
 */
typedef SAL_Int_T SAL_Thread_Id_T;

/**
 * SAL_Timer_Id_T defines the type used to save timer ids.
 */
typedef intptr_t SAL_Timer_Id_T;

/**
 *  XSAL Mutex type.
 */
typedef struct SAL_Mutex_Tag
{
   volatile int32_t count;
   SAL_Thread_Id_T owner;
   pthread_mutex_t mutex;
} SAL_Mutex_T;

#define SAL_MUTEX_INITIALIZER    { 0, (SAL_Thread_Id_T) -1, PTHREAD_MUTEX_INITIALIZER}

typedef struct SAL_Mutex_Attr_Tag
{
   bool_t recursive;
} SAL_Mutex_Attr_T;


#define SAL_DEFAULT_MUTEX_ATTR { false }

/**
 *  XSAL Semaphore type.
 */
typedef sem_t SAL_Semaphore_T;

typedef struct SAL_Semaphore_Attr_Tag
{
   uint32_t initial_value;
} SAL_Semaphore_Attr_T;


#define SAL_DEFAULT_SEMAPHORE_ATTR { 0 }


/**
 *  XSAL Clock type.
 */
typedef uint32_t SAL_Clock_T;


/**
 *  XSAL Read_Write lock.
 */
typedef pthread_rwlock_t SAL_RWLock_T;

typedef void* SAL_RWLock_Attr_T;

#define SAL_DEFAULT_RWLOCK_ATTR  NULL


/**
 *  XSAL Conditional Variable
 */
typedef pthread_cond_t SAL_Cond_T;

typedef void* SAL_Cond_Attr_T;

#define SAL_DEFAULT_COND_ATTR  NULL



/**
 *  XSAL synchronous communication channel type. 
 */
typedef struct SAL_Channel_Tag
{
   pid_t app_pid;
   int socket_d;
   uid_t user_id;
} SAL_Channel_T;


/**
 *  XSAL synchronous receive data channel type.
 */
typedef int SAL_Receive_T;


/**
 *  Connection to SAL synchronous channel type.
 */
typedef struct SAL_Connection_Tag
{
   pid_t app_pid;
   int socket_d;
   uid_t user_id;
} SAL_Connection_T;


#define SAL_Is_Connected(connection)           ((bool_t)((connection)->app_pid > 0))
#define SAL_Invalidate_Connection(connection)  ((connection)->app_pid = 0)


/**
 *  SAL Pulse type.
 */
typedef struct SAL_Pulse_Tag
{
   int8_t  code;
   union Value_Tag
   {
      int32_t sival_int;
      void*   sival_ptr; 
   } value;
} SAL_Pulse_T;


/**
 *  SAL multi-part buffer type.
 */
typedef struct iovec SAL_IOV_T;


/* -------------------------------------------------------------------------
 *
 *                           Thread management
 *
 * -----------------------------------------------------------------------*/

/**
 *  Thread attributes
 */
typedef struct SAL_Thread_Attr_Tag 
{
   const char*     name;
   SAL_Thread_Id_T id;
   SAL_Priority_T  priority;
   size_t          stack_size;
} SAL_Thread_Attr_T;


/** Default priority for Thread
 */
#if !defined(SAL_DEFAULT_THREAD_PRIORITY)
#define SAL_DEFAULT_THREAD_PRIORITY  SAL_NORMAL_PRIORITY
#endif /* SAL_DEFAULT_THREAD_PRIORITY */


/** Default thread stack size
 */
#if !defined(SAL_DEFAULT_THREAD_STACK_SIZE)
/* Use OS default value */
#define SAL_DEFAULT_THREAD_STACK_SIZE  0
#endif /* SAL_DEFAULT_THREAD_STACK_SIZE */


#define SAL_DEFAULT_THREAD_ATTR { "", SAL_UNKNOWN_THREAD_ID, SAL_DEFAULT_THREAD_PRIORITY, SAL_DEFAULT_THREAD_STACK_SIZE }


/**
 *  Thread Local Storage key
 */
typedef pthread_key_t SAL_TLS_Key_T;

/**
 *  Thread handle
 */
typedef pthread_t SAL_OS_TID_T;

typedef pid_t SAL_OS_PID_T;

#define SAL_UNKNOWN_PRIORITY (-1)

/* -------------------------------------------------------------------------
 *
 *                           Shared memory management
 *
 * -----------------------------------------------------------------------*/

#if !defined(XSAL_LIGHT)

 /**
  *  Handler to the Shared Memory area
  */
typedef struct SAL_Shared_Memory_Handle_Tag
{
   /** Pointer to Shared Memory area
    */
   void* data;

   /** Private section (OS dependent)
    */
   SAL_Int_T fd;
   SAL_Shared_Memory_Key_T key;
   SAL_Shared_Memory_Key_T subkey;
   size_t size;
   bool_t owner;

} SAL_Shared_Memory_Handle_T;

#endif /* !defined(XSAL_LIGHT) */

#if !defined(EOK)
#define EOK 0
#endif /* !EOK */

/*===========================================================================*\
 * File Revision History (top to bottom: laast revision to first revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * - 11-Aug-2011 Dan Carman
 *    - Add MIN and MAX Priority definitions
 *
 * - 24-Jul-2010 Kirk Bailey
 *    - Change "bool" to "bool_t".
 *
 * 30-may-2010 kirk bailey
 * + Added SAL_Clock_T.
 *
 * 27-mar-2008 kirk bailey
 * + Updated to latest DB48x version.
 *
 * 16-Mar-2015 Dan Carman
 * + Task 21696 Change SAL_Timer_Id_T to hold either int or pointer for 64 bit Linux
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* XSAL_DATA_TYPES_LINUX_H */
