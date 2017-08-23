/*===========================================================================*/
/**
 * @file dbg_trace_verbose.c
 *
 *   Implements the output of verbose dbg_trace data.
 *
 * %full_filespec: dbg_trace_verbose.c~kok_inf#7:csrc:kok_aud#1 %
 * @version %version: kok_inf#7 %
 * @author  %derived_by: gzt3xl %
 * @date    %date_modified: Mon Feb 24 16:15:08 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR = TRace
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
#include "dbg_trace_cbk.h"
#include "dbg_trace_cfg.h"
#include "dbg_trace_imp.h"
#include <stdarg.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

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

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace.h
 *
 *===========================================================================*/
void Tr_Trace_Verbose(FILE* fp, Tr_Trace_Level_T lvl, uint16_t f_num, char_t const *f_name,
                      uint16_t line_num, char_t const *msg, ...)
{
   if (tr_Is_Trace_Level_Enabled(f_num, lvl))
   {
      va_list arg_ptr;
      Tr_Compact_Data_T tr_data;

      tr_data.trace_level = lvl;
      tr_data.file_id     = f_num;
      tr_data.line_num    = line_num;
      tr_data.timestamp   = Tr_Clock();
      tr_data.thread_id   = Tr_Get_Thread_Id();
      tr_data.app_id      = Tr_Get_App_Id();
#if defined(DBG_USE_THREAD_CPU_TIME) && (DBG_USE_THREAD_CPU_TIME==true)
      tr_data.thr_cpu_time = Tr_Get_Thread_CPU_Time();
#else
      tr_data.thr_cpu_time = ((Tr_Clock_T) {0, 0});
#endif
      va_start(arg_ptr, msg);
      Tr_Store_Verbose_Data(fp, &tr_data, f_name, msg, arg_ptr);
      va_end(arg_ptr);
   }
}

/*===========================================================================*/
/*!
 * @file dbg_trace_verbose.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 10-May-2012 kirk bailey Rev. 3
 *   - Added option for CPU thread time.
 *
 * - 28-Feb-2013 Kirk Bailey Rev. 5
 *   - Decoupled trace from XSAL, moving that information to callouts.
 *
 * - 03-Oct-2013 Kirk Bailey REV 6
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 *
 * - 24-Feb-2014 Paul Casto REV 7
 *   - Added support for different output streams.
 */
/*===========================================================================*/
