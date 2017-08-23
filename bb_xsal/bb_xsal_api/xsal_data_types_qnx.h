#ifndef XSAL_DATA_TYPES_QNX_H
#   define XSAL_DATA_TYPES_QNX_H
/*===========================================================================*/
/**
 * @file xsal_data_types_qnx.h
 *
 * Defines basic XSAL data types for QNX.
 *
 * %full_filespec:xsal_data_types_qnx.h~kok_inf#7:incl:kok_aud#1 %
 * @version %version:kok_inf#7 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Sep 10 11:15:16 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 *  This has XSAL data type declarations that are unique to QNX.
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

#   include <sys/neutrino.h>
#   include <pthread.h>
#   include <semaphore.h>

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

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
 */
typedef SAL_Int_T SAL_Priority_T;

/**
 * Define the highest possible task priority value  
 */
#define SAL_MAX_TASK_PRIORITY    63

/**
 * Define the lowest possible task priority value
 */
#define SAL_MIN_TASK_PRIORITY    1


/**
 * SAL_Thread_Id_T defines the type used to identify threads.
 */
typedef SAL_Int_T SAL_Thread_Id_T;

/**
 * SAL_Timer_Id_T defines the type used to save timer ids.
 */
typedef SAL_Int_T SAL_Timer_Id_T;

/**
 *  XSAL Mutex type.
 */
typedef pthread_mutex_t SAL_Mutex_T;

#define SAL_MUTEX_INITIALIZER    PTHREAD_MUTEX_INITIALIZER

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
   int app_pid;
   int channel_id;
} SAL_Channel_T;


/**
 *  XSAL synchronous receive data channel type.
 */
typedef struct SAL_Receive_Tag
{
   int receive_id;
   size_t offset;
} SAL_Receive_T;


/**
 *  Connection to SAL synchronous channel type.
 */
typedef int SAL_Connection_T;


#define SAL_Is_Connected(connection)           ((bool_t)(*connection > 0))
#define SAL_Invalidate_Connection(connection)  (*connection = 0)


/**
 *  SAL Pulse type.
 */
typedef struct _pulse SAL_Pulse_T;


/**
 *  SAL multi-part buffer type.
 */
typedef iov_t SAL_IOV_T;


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


#define SAL_UNKNOWN_PRIORITY (-1)

/* -------------------------------------------------------------------------
 *
 *                           Shared memory management
 *
 * -----------------------------------------------------------------------*/

 
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
   int fd;
   int key;
   int subkey;
   size_t size;
   int owner;
} SAL_Shared_Memory_Handle_T;


#if !defined(EOK)
#define EOK 0
#endif /* !EOK */

/*===========================================================================*\
 * File Revision History (top to bottom: laast revision to first revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * - 24-Jul-2010 Kirk Bailey
 *    - Change "bool" to "bool_t".
 *
 * 02-jun-2008 kirk bailey
 * + Added SAL_Clock_T.
 *
 * 27-mar-2008 kirk bailey
 * + Updated to latest DB48x version.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* XSAL_DATA_TYPES_QNX_H */
