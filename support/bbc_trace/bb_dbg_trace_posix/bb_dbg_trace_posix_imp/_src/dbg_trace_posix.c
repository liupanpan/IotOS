/*===========================================================================*/
/**
 * @file dbg_trace_posix.c
 *
 *   Implements the OS-specific dbg_trace callouts for POSIX systems.
 *
 * %full_filespec:dbg_trace_posix.c~kok_inf#8:csrc:kok_basa#1 %
 * @version %version:kok_inf#8 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed May 13 11:41:54 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2013-2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR = TRace.
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - none
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

#include "dbg_trace_cfg.h"

#if TR_STAND_ALONE == false

#include "dbg_trace_cbk.h"
#include <pthread.h>
#ifdef LINUX
#   ifndef _GNU_SOURCE
#      define _GNU_SOURCE
#   endif
#   include <sys/syscall.h>
#endif
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

#ifndef DBG_TRC_EPOCH_OFFSET
#   define DBG_TRC_EPOCH_OFFSET (0) /* UNIX Epoch value at 00:00:00 1-Jan-1970 */
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

Tr_Clock_T Tr_Clock(void)
{
   Tr_Clock_T timestamp;
   struct timespec time_buf;

   clock_gettime(CLOCK_REALTIME, &time_buf);
   /*
    * Instead of time since Unix Epoch (00:00:00 1-Jan-1970), allow a more
    * recent time base to be established.
    */
   if ((DBG_TRC_EPOCH_OFFSET) && (time_buf.tv_sec > DBG_TRC_EPOCH_OFFSET))
   {
      time_buf.tv_sec -= DBG_TRC_EPOCH_OFFSET;
   }
   timestamp.sec = (uint32_t) (time_buf.tv_sec & 0xFFFFFFFF);
   timestamp.ns = time_buf.tv_nsec;

   return timestamp;
}

uint32_t Tr_Get_App_Id(void)
{
   return (uint32_t)getpid();
}

uint32_t Tr_Get_Thread_Id(void)
{
   uint32_t tid;
#if defined (LINUX)
   tid = syscall(SYS_gettid);
#else
   tid = (uint32_t)pthread_self();
#endif
   return tid;
}

Tr_Clock_T Tr_Get_Thread_CPU_Time(void)
{
   Tr_Clock_T cpu_time = {0, 0};

#if DBG_USE_THREAD_CPU_TIME == true
   struct timespec thr_time;

   clock_gettime(CLOCK_THREAD_CPUTIME_ID, &thr_time);

   cpu_time.sec = thr_time.tv_sec;
   cpu_time.ns  = thr_time.tv_nsec;
#endif

   return cpu_time;
}

#endif /* TR_STAND_ALONE != false */

/*===========================================================================*/
/*!
 * @file dbg_trace_posix.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 28-Feb-2013 Kirk Bailey
 *   - Created initial file.
 * 
 * - 12-Mar-2013 Kirk Bailey Rev. 3
 *   - Added DBG_ENABLE_64BIT_TIMESTAMP support.
 *
 * - 1-July-2013 Dan Carman
 *    - Standardize timestamp format
 *
 * - 03-Oct-2013 Kirk Bailey REV 5
 *   - Fixed order of revision history comments - no functional changes
 *
 * - 12-May-2015 Kirk Bailey REV 7
 *   - Moved Printf_Lock/Unlock to separate compilation unit.
 *
 * - 13-May-2015 Kirk Bailey REV 8
 *   - Fixed QNX build warning.
 */
/*===========================================================================*/
