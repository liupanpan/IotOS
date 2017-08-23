#ifndef XSAL_DATA_TYPES_WIN32_H
#   define XSAL_DATA_TYPES_WIN32_H
/*===========================================================================*/
/**
 * @file xsal_data_types_win32.h
 *
 * Defines basic XSAL data types for Win32.
 *
 * %full_filespec:xsal_data_types_win32.h~kok_basa#7:incl:kok_aud#3 %
 * @version %version:kok_basa#7 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu May 14 11:13:31 2015 %
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
 *  This has XSAL data type declarations unique to Win32.
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

#   include <windows.h>
#   include <limits.h>

#   if ! defined(PATH_MAX)
#      define PATH_MAX MAX_PATH 
#   endif /* PATH_MAX */

/*===========================================================================*\
 * Exported Preprocessor #define Constants
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
#define SAL_MAX_TASK_PRIORITY    THREAD_PRIORITY_HIGHEST

/**
 * Define the lowest possible task priority value
 */
#define SAL_MIN_TASK_PRIORITY    THREAD_PRIORITY_LOWEST

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
typedef HANDLE SAL_Mutex_T;

#define SAL_MUTEX_INITIALIZER NULL


typedef struct SAL_Mutex_Attr_Tag
{
   bool_t recursive;
} SAL_Mutex_Attr_T;


#define SAL_DEFAULT_MUTEX_ATTR { false }


/**
 *  XSAL Semaphore type.
 */
typedef HANDLE SAL_Semaphore_T;

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
typedef struct SAL_RWLock_Tag
{
   SAL_Semaphore_T  read_sem;
   SAL_Semaphore_T  write_sem;
   SAL_Int_T        number_of_readers;
   bool_t             write;
} SAL_RWLock_T;

typedef void* SAL_RWLock_Attr_T;

#define SAL_DEFAULT_RWLOCK_ATTR  NULL


/**
 *  XSAL Conditional Variable
 */
typedef void* SAL_Cond_T;

typedef void* SAL_Cond_Attr_T;

#define SAL_DEFAULT_COND_ATTR  NULL


/**
 *  XSAL synchronous communication channel type.
 */
typedef struct SAL_Channel_Tag
{
   SAL_Int_T  tcp_port;
   SOCKET     socket_fd;
} SAL_Channel_T;


/**
 *  XSAL synchronous receive data channel type.
 */
typedef struct SAL_Receive_Tag
{
   size_t     data_to_read;
   SAL_Int_T  port;
   SOCKET     socket_fd;
} SAL_Receive_T;


/**
 *  Connection to XSAL synchronous channel type.
 */
typedef SAL_Int_T SAL_Connection_T;


#define SAL_Is_Connected(connection)          ((bool_t)(*connection > 0))
#define SAL_Invalidate_Connection(connection) (*connection = 0)


/**
 *  XSAL Pulse type.
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
 *  XSAL multi-part buffer type. 
 */
typedef struct SAL_IOV_Tag
{
   void*  iov_base;
   size_t iov_len;
} SAL_IOV_T;


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
typedef DWORD SAL_TLS_Key_T;


/**
 *  Thread handle
 */
typedef HANDLE SAL_OS_TID_T;


#define SAL_UNKNOWN_PRIORITY THREAD_PRIORITY_ERROR_RETURN

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
   HANDLE handle;
} SAL_Shared_Memory_Handle_T;



/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 27-mar-2008 kirk bailey
 * + Updated to latest DB48x version.
 *
 * 02-jun-2008 kirk bailey
 * + Added SAL_Clock_T.
 *
 * 24-Jul-2010 Kirk Bailey
 * + Change "bool" to "bool_t".
 *
 * 14-May-2015 Kirk Bailey
 * + Fixed Cygwin build warnings.
\*===========================================================================*/
/** @} doxygen end group */
#endif /* XSAL_DATA_TYPES_WIN32_H */
