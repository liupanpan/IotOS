/*===========================================================================*/
/**
 * @file app_stub.c
 *
 *   Implements misc application callouts
 *
 * %full_filespec:app_stub.c~5:csrc:kok_aud#1 %
 * @version %version:5 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Jul 15 17:35:06 2009 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * implements Message log and dropped messages callouts
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
 * @addtogroup app_callouts_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "app_stub_friend.h"
#include "pbc_trace.h"
#include "reuse.h"
#include "xsal_cbk.h"
#include "xsal_util.h"
#include <string.h>

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(TEST_MODULE_ID, 13); /* define this file for assert handling */

/** Number of messages to log (older message are overwritten by newer  
 */
#define MESSAGE_LOG_SIZE         50

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/**
 * Debug buffer to hold the last MESSAGE_LOG_SIZE message sent 
 * Read only public access 
 */
SAL_Message_T Msg_Log_Buffer[MESSAGE_LOG_SIZE];

/** Index for next message to go into log buffer 
 *  Read only public access 
 */
uint32_t Msg_Log_Index;

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/** mutex to control access to msg_log 
 */
static SAL_Mutex_T msg_log_mutex;

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/
static void msg_log_initialize(void);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Please refer to the detailed description in xsal_cbk.h
 *
 * This verison call periodic publish every 5 mS
\*===========================================================================*/
void SAL_1_mS_Tasks(void)
{
   static uint_fast8_t half_count = 0;

   half_count++;
   if (half_count >= 5)
   {
      half_count = 0;
/*      Periodic_Publish();   */
   }
}

/*===========================================================================*\
 * Please refer to the detailed description in xsal_cbk.h
 *
 * This version logs the last MESSAGE_LOG_SIZE to a circular ram buffer
 * Msg_Log_Buffer.  Note, the data pointer is replaced with the first 4 bytes 
 * of the actual message data 
\*===========================================================================*/
void SAL_Log_Message(const SAL_Message_T * message)
{
   /** tracks if initialization has been performed */
   static SAL_Util_Once_T msg_log_initialized;

   SAL_Util_Once(&msg_log_initialized, msg_log_initialize);

   PBC_Internal_1(Msg_Log_Index < MESSAGE_LOG_SIZE, "Invalid log index %d", Msg_Log_Index);

   if (!SAL_Lock_Mutex_Timeout(&msg_log_mutex, 1000))
   {
      PBC_Failed("Timeout accessing log");
   }

   Msg_Log_Buffer[Msg_Log_Index] = *message;

   if (message->data_size > 0)  /* copy over first 4 bytes */
   {
      memcpy(&Msg_Log_Buffer[Msg_Log_Index].data, message->data, sizeof(Msg_Log_Buffer[Msg_Log_Index].data));
   }

   Msg_Log_Index = (Msg_Log_Index + 1) % MESSAGE_LOG_SIZE;

   SAL_Unlock_Mutex(&msg_log_mutex);
}

/**
 * Creates the Mutex used to serialize logging of message (executes one time)
 */
static void msg_log_initialize(void)
{
   SAL_Mutex_Attr_T attr;

   SAL_Init_Mutex_Attr(&attr);

   if (!SAL_Create_Mutex(&msg_log_mutex, &attr))
   {
      PBC_Failed("Failed Create Log Mutex");
   }
}

/*
 *  Callout when message is dropped due to queue full
 */
void SAL_Dropped_Message(const SAL_Message_T * dropped_message)
{
#ifdef RELEASE
   PBC_Failed_3("Dropped message %d to to thread %d %s", (int)dropped_message->event_id,
                (int)dropped_message->receiver_thread_id, SAL_Get_Thread_Name(dropped_message->receiver_thread_id));
#else
   SAL_Stat_Queue_T queue_stat;

   Tr_Fault_3("Dropped message %d to to thread %d %s", (int)dropped_message->event_id,
              (int)dropped_message->receiver_thread_id, SAL_Get_Thread_Name(dropped_message->receiver_thread_id));

   if ((dropped_message->receiver_thread_id > 0) && (SAL_Stat_Queue(dropped_message->receiver_thread_id, &queue_stat)))
   {
      Tr_Fault_4("Queue Full - Message Count: %d Limbo Count: %d Buffer Size: %d Peak Message Size: %d",
                 queue_stat.message_count, queue_stat.limbo_message_count, queue_stat.buffer_size,
                 queue_stat.peak_message_size);
   }
   else
   {
      Tr_Fault("Invalid queue");
   }
#endif
}

/*===========================================================================*/
/*!
 * @file app_stub.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 30-June-2009 Dan Carman
 * + Replace printf with Trace calls
 * + Reduced size of trace messages
 *
 * 15-Oct-2008 Dan Carman
 * + Modified so not stop system if message is dropped when not built for release.
 *   Queue overflow tends to happen if thread is stopped during debug.
 *
 * 18-aug-2008 kirk bailey
 * + Modified so it builds for QNX.
 *
 * 16-Aug-2007 Dan Carman
 * + MISRA changes
 *
 * 25-apr-2007 kirk bailey
 * + Fixed printf format to eliminate gcc warnings.
 *
 * 17-apr-2007 Dan Carman
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
