/*===========================================================================*/
/**
 * @file dbg_trace_c99_log.c
 *
 *   Implements dbg_trace logging to the final output sink(s).
 *
 * %full_filespec:dbg_trace_c99_log.c~kok_inf#4:csrc:kok_basa#1 %
 * @version %version:kok_inf#4 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed Feb 26 13:09:22 2014 %
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
 *
 * @section ABBR ABBREVIATIONS:
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
 *   - None
 *
 * @ingroup unit_test
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "dbg_trace_cfg.h"
#include "printf_gate.h"
#include <stdio.h>

#if defined(DBG_ENABLE_SYSLOG)
#   if defined(QNX_NTO)
#      include <sys/slog.h>
#      include <sys/slogcodes.h>
#   else
#      include <syslog.h>
#   endif /* QNX_NTO */
#endif /* DBG_ENABLE_SYSLOG */

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
#ifndef DBG_TRACE_EOL
#   ifdef UITRON
#      define DBG_TRACE_EOL "\r\n"
#   else
#      define DBG_TRACE_EOL "\n"
#   endif
#endif

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_cbk.h
 *
 *===========================================================================*/
void Tr_Log(FILE* fp, char_t * trace_output)
{
   /*========================================================================*\
    * Special maintenance note: This function is also implemented in
    * dbg_trace_c89_log.c. Any changes made here must be investigated in
    * the other file.
   \*========================================================================*/
#if defined(DBG_ENABLE_SYSLOG)
#  if defined(QNX_NTO)
   slogf(_SLOG_SETCODE(_SLOGC_TEST, 0), _SLOG_DEBUG1, "%s", trace_output);
#  else
   syslog(LOG_MAKEPRI(LOG_USER, LOG_DEBUG), "%s", trace_output);
#  endif /* QNX_NTO */
#endif /* DBG_ENABLE_SYSLOG */

   if( stdout == fp )
   {
#if !defined (DBG_DISABLE_STDOUT)
      Printf_Lock();
      printf("%s"DBG_TRACE_EOL, (char const*)trace_output);
      Printf_Unlock();
#endif /* !DBG_DISABLE_STDOUT */
   }

   else
   {
      Printf_Lock();
      fprintf(fp, "%s"DBG_TRACE_EOL, (char const*)trace_output);
      Printf_Unlock();
   }
}

/*===========================================================================*/
/*!
 * @file dbg_trace_c99_log.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 07-may-2011 Kris Boultbee
 *   Task kok_basa#30663 Revision 1
 *   - Created initial file
 *   - Added function Tr_Log() to do the final logging of Trace data.
 *
 * - 03-Oct-2013 Kirk Bailey REV 2
 *   - Fixed order of revision history comments - no functional changes
 *
 * - 24-Feb-2014 Paul Casto REV 3
 *   - Added support for different output streams.
 *
 * - 26-Feb-2014 Kirk Bailey REV 4
 *   - Fixed Klocwork defects caused by using char_t instead of char * for
 *     printf-type arguments.
 */
/*===========================================================================*/
/** @} doxygen end group */
