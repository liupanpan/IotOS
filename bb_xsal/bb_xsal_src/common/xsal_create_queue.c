/*===========================================================================*/
/**
 * @file  xsal_create_queue.c
 *
 * Implementation of the SAL_Create_Queue() function.
 *
 * %full_filespec:xsal_create_queue.c~kok_inf#9:csrc:kok_aud#2 %
 * @version %version:kok_inf#9 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Mar 23 08:58:15 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2006,2011 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Implementation of the SAL_Create_Queue() function.
 *    Original Author: Jacek Roman (jacek.roman@delphi.com)
 *
 * @section ABBR ABBREVIATIONS:
 *   -
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - none
 *
 *   - Requirements Document(s):
 *     - none
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - none
 *
 * @addtogroup xsal
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"
#include "pbc_trace.h"

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
EM_FILENUM(XSAL_Q_MODULE_ID, 1); /* Identify file for trace/assert purposes */

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/
/**
 * Define default limit to queue size (number of messages)
 *    Value can be overwritten in xsal_settings
 */
#ifndef SAL_MAX_QUEUE_SIZE
#define SAL_MAX_QUEUE_SIZE    250
#endif

/**
 * Define default limit to preallocate message buffer size (bytes allocated per messages)
 *    Value can be overwritten in xsal_settings
 */
#ifndef SAL_MAX_MESSAGE_BUF_SIZE
#define SAL_MAX_MESSAGE_BUF_SIZE 0x40
#endif

/**
 * Define default limit for minmum preallocate message buffer size (bytes allocated per messages)
 *    Value can be overwritten in xsal_settings
 */
#ifndef SAL_MIN_MESSAGE_BUF_SIZE
#define SAL_MIN_MESSAGE_BUF_SIZE 0
#endif

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

bool_t SAL_Create_Queue(size_t queue_size, size_t message_size, void* (*alloc_buf)(size_t size), void(*free_buf)(void* buf))
{
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();

   PBC_Require(thread_attr != NULL, "NULL thread attribute");

   if (queue_size > SAL_MAX_QUEUE_SIZE)
   {
      Tr_Fault_2("Requested queue size %zu is greater than max %d", queue_size, SAL_MAX_QUEUE_SIZE);
      queue_size = SAL_MAX_QUEUE_SIZE;
   }

   if (message_size > SAL_MAX_MESSAGE_BUF_SIZE)
   {
      Tr_Fault_2("Requested message buffer size %zu is greater than max %d", message_size, SAL_MAX_MESSAGE_BUF_SIZE);
      message_size = SAL_MAX_MESSAGE_BUF_SIZE;
   }
#if (SAL_MIN_MESSAGE_BUF_SIZE > 0)
   else if (message_size < SAL_MIN_MESSAGE_BUF_SIZE)
   {
      Tr_Fault_2("Requested message buffer size %zu is smaller than minimum %d", message_size, SAL_MIN_MESSAGE_BUF_SIZE);
      message_size = SAL_MIN_MESSAGE_BUF_SIZE;
   }
#endif

   /** Queue should be able to store queue_size new messages when 
    *  thread is processing a current message. Therefore queue_size 
    *  parameter is increased by 1
    */
   return SAL_I_Create_Message_Queue(&thread_attr->message_queue, queue_size + 1u, message_size, alloc_buf, free_buf);
}

/*===========================================================================*/
/*!
 * @file xsal_create_queue
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *    Initial revision.
 *
 * - 30-Nov-2011 Dan Carman
 *   - Added checks on maximum queue and message buffer sizes
 *
 * - 26-Sep-2013 Larry Piekarski Rev 7
 *   - Task 1334: Fixed compiler warnings
 *
 * 16-Mar-2015 Dan Carman
 * + Task 21696 Add z format specificer for size_t which maybe different from native int
*/
/*===========================================================================*/
/** @} doxygen end group */
