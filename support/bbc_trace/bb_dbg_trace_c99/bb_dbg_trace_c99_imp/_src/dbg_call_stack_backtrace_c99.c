/*===========================================================================*/
/**
 * @file dbg_call_stack_backtrace_c99.c
 *
 * Displays the current call stack backtrace.
 *
 * %full_filespec: dbg_call_stack_backtrace_c99.c~kok_inf#11:csrc:kok_basa#1 %
 * @version %version: kok_inf#11 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Thu Oct  3 13:41:58 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Displays the current call stack backtrace if call stack backtrace
 * is available on the platform. If call stack backtrace is not available
 * nothing will be printed.
 *
 * @section ABBR ABBREVIATIONS:
 *   - PbC Programming by Contract
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
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "dbg_trace.h"
#include "dbg_trace_cbk.h"
#include "dbg_trace_cfg.h"

#if defined(DBG_ENABLE_STACK_BACKTRACE) && (DBG_ENABLE_STACK_BACKTRACE == true)
#   ifdef LINUX
#      include <execinfo.h>
#   endif /* ifdef LINUX */
#   include <stdlib.h>
#   include <stdio.h>
#   include <string.h>
#   include <syslog.h>
#   include <unistd.h>
#endif /* defined(DBG_ENABLE_STACK_BACKTRACE) && (DBG_ENABLE_STACK_BACKTRACE == true) */

#if defined(DBG_USE_PID_TID_NAMES) && (DBG_USE_PID_TID_NAMES != 0)

#include <sys/prctl.h>
extern char const *__progname;

#endif

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

#ifndef DBG_MAX_CALL_DEPTH
/**
 * Maximum stack callback depth to track/log.
 */
#   define DBG_MAX_CALL_DEPTH 32
#endif

#ifndef DBG_BT_LOG_BUF_SZ
/**
 * Size of output buffer to use for forming log messages.
 */
#   define DBG_BT_LOG_BUF_SZ 512
#endif

#ifndef LINUX_TID_NAME_SIZE
#   define LINUX_TID_NAME_SIZE 17
#endif

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

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

/* ========================================================================
 *
 * Please refer to dbg_trace_cbk.h for more details.
 *
 * ========================================================================*/
void Tr_Print_Stack_Backtrace(void)
{
#if defined(DBG_ENABLE_STACK_BACKTRACE) && (DBG_ENABLE_STACK_BACKTRACE == true) && defined(LINUX)
   size_t buf_ofst = 0;
   int num_addrs;
   char sigbuf[DBG_BT_LOG_BUF_SZ];
   void *bt_addrs[DBG_MAX_CALL_DEPTH];
   Tr_Clock_T timestamp = Tr_Clock();
#   if defined(DBG_USE_PID_TID_NAMES) && (DBG_USE_PID_TID_NAMES != 0)
   char tid_name[LINUX_TID_NAME_SIZE];
   prctl(PR_GET_NAME, tid_name);
   tid_name[sizeof(tid_name)-1] = '\0';
#   endif
   num_addrs = backtrace(bt_addrs, Num_Elems(bt_addrs));

   if (num_addrs <= 0)
   {
#   if defined(DBG_USE_PID_TID_NAMES) && (DBG_USE_PID_TID_NAMES != 0)
      snprintf(sigbuf + buf_ofst, sizeof(sigbuf) - buf_ofst,
         "[%u.%06u] %s:%s; dbg_call_stack_backtrace_c99.c:%u; FAULT - No stack backtrace data available (stack corrupted?)",
         timestamp.sec, (timestamp.ns / 1000),
         __progname, tid_name, (unsigned) __LINE__);
#   else
      snprintf(sigbuf + buf_ofst, sizeof(sigbuf) - buf_ofst,
         "[%u.%06u] %d:%d; dbg_call_stack_backtrace_c99.c:%u; FAULT - No stack backtrace data available (stack corrupted?)",
         timestamp.sec, (timestamp.ns / 1000),
         (int) Tr_Get_App_Id(), (int) Tr_Get_Thread_Id(), (unsigned) __LINE__);
#endif
   }
   else
   {
      size_t i;
      ssize_t new_ofst;
      size_t unum_addrs = (size_t)num_addrs;

#   if defined(DBG_USE_PID_TID_NAMES) && (DBG_USE_PID_TID_NAMES != 0)
      new_ofst = snprintf(sigbuf + buf_ofst, sizeof(sigbuf) - buf_ofst,
         "[%u.%06u] %s:%s; dbg_call_stack_backtrace_c99.c:%u; FAULT - Stack backtrace:",
         timestamp.sec,  (timestamp.ns / 1000),
         __progname, tid_name, (unsigned) __LINE__);
#   else
      new_ofst = snprintf(sigbuf + buf_ofst, sizeof(sigbuf) - buf_ofst,
         "[%u.%06u] %d:%d; dbg_call_stack_backtrace_c99.c:%u; FAULT - Stack backtrace:",
         timestamp.sec,  (timestamp.ns / 1000),
         (int) Tr_Get_App_Id(), (int) Tr_Get_Thread_Id(), (unsigned) __LINE__);
#endif
      if (new_ofst >= 0)
      {
         buf_ofst += new_ofst;
      }

      for (i = 1; i < unum_addrs && buf_ofst < sizeof(sigbuf); i++)
      {
         new_ofst = snprintf(sigbuf + buf_ofst, sizeof(sigbuf) - buf_ofst, " %p", bt_addrs[i]);
         if (new_ofst >= 0)
         {
            buf_ofst += new_ofst;
         }
      }
   }

   syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), "%s", sigbuf);
   printf("%s\n", sigbuf);
#endif
}

/*===========================================================================*/
/*!
 * @file dbg_call_stack_backtrace_c99.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 03-may-2011 Kris Boultbee
 *   Task kok_basa#30663 Revision 1
 *   - Created initial file.
 *
 * - 17-may-2011 Kirk Bailey Rev. 2
 *   - Re-did logic to use addr2line to extract symbol information.
 *
 * - 03-Nov-2011 Kirk Bailey Rev. 4
 *   - Fixed logic to work even when backtrace data is not available.
 *
 * - 10-Jan-2012 Kirk Bailey Rev. 5
 *   - Simplified backtrace and separated from regular trace output.
 *
 * - 28-Feb-2013 Kirk Bailey Rev. 6
 *   - Decoupled trace from XSAL, moving that information to callouts.
 *
 * - 20-Sep-2013 Kirk Bailey Rev. 9
 *   - Fixed signed/unsigned comparison warnings due to -Wextra.
 *
 * - 03-Oct-2013 Kirk Bailey REV 10
 *   - Fixed order of revision history comments - no functional changes
 *
 *  - 03-Oct-2013 Kirk Bailey REV 11
 *   -  Added DBG_USE_PID_TID_NAMES option to use process/thread names instead
 *      of numbers.
 */
/*===========================================================================*/
