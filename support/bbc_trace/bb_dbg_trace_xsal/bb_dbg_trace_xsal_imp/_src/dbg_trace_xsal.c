/*===========================================================================*/
/**
 * @file dbg_trace_xsal.c
 *
 *   Implements the OS-specific dbg_trace callouts for POSIX systems.
 *
 * %full_filespec:dbg_trace_xsal.c~kok_inf#3:csrc:kok_basa#1 %
 * @version %version:kok_inf#3 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 10:22:36 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2013 Delphi Technologies, Inc., All Rights Reserved.
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
#include "xsal.h"
#ifdef LINUX
#   include <time.h>
#endif

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

#ifdef LINUX
   struct timespec time_buf;

   clock_gettime(CLOCK_REALTIME, &time_buf);
   /*
    * Instead of time since Unix Epoch (00:00:00 1-Jan-1970), allow a more
    * recent time base to be established.
    */
   if ((DBG_TRC_EPOCH_OFFSET) && (time_buf.tv_sec >= DBG_TRC_EPOCH_OFFSET))
   {
      time_buf.tv_sec -= DBG_TRC_EPOCH_OFFSET;
   }

   timestamp.sec = (uint32_t) (time_buf.tv_sec & 0xFFFFFFFF);
   timestamp.ns = time_buf.tv_nsec;
#else
    SAL_Clock_T sal_time = SAL_Clock();

   timestamp.sec = sal_time / 1000;
   timestamp.ns = (sal_time % 1000) * 1000000;
#endif

   return timestamp;
}

uint32_t Tr_Get_App_Id(void)
{
   return (uint32_t)SAL_Get_App_Id();
}

uint32_t Tr_Get_Thread_Id(void)
{
   return SAL_Get_Thread_Id();
}

Tr_Clock_T Tr_Get_Thread_CPU_Time(void)
{
   Tr_Clock_T cpu_time = {0,0};
#ifdef LINUX
   struct timespec thr_time;

   clock_gettime(CLOCK_THREAD_CPUTIME_ID, &thr_time);

   cpu_time.sec = thr_time.tv_sec;
   cpu_time.ns = thr_time.tv_nsec;
#endif

   return cpu_time;
}

#endif /* TR_STAND_ALONE != false */

/*===========================================================================*/
/*!
 * @file dbg_trace_xsal.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 28-Feb-2013 Kirk Bailey
 *   - Created initial file.
 *
 * - 03-Oct-2013 Kirk Bailey REV 3
 *   - Fixed order of revision history comments - no functional changes
 */
/*===========================================================================*/
