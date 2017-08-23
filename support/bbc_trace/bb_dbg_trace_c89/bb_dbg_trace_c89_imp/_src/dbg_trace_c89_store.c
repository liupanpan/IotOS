/*===========================================================================*/
/**
 * @file dbg_trace_c89_store.c
 *
 *   Implements dbg_trace "storage" callouts for C89.
 *
 * %full_filespec:dbg_trace_c89_store.c~kok_inf#7:csrc:kok_basa#1 %
 * @version %version:kok_inf#7 %
 * @author  %derived_by:gzt3xl %
 * @date    %date_modified:Mon Feb 24 16:16:01 2014 %
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
#include "dbg_trace_c89.h"
#include "dbg_trace_cbk.h"
#include "dbg_trace_cfg.h"

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
#ifndef DBG_TRACE_MAX_LINE_LENGTH
#   define DBG_TRACE_MAX_LINE_LENGTH 256
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
void Tr_Store_Compact_Data(FILE* fp, Tr_Compact_Data_T const *tr_data)
{
   char_t trace_output[DBG_TRACE_MAX_LINE_LENGTH];

   Tr_Sprintf_Compact(trace_output, DBG_TRACE_MAX_LINE_LENGTH, tr_data);

   Tr_Log(fp, trace_output);
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_cbk.h
 *
 *===========================================================================*/
void Tr_Store_Verbose_Data(FILE* fp, Tr_Compact_Data_T const *tr_data,
                           char_t const *f_name, char_t const *msg,
                           va_list arg_ptr)
{
   char_t trace_output[DBG_TRACE_MAX_LINE_LENGTH];

   Tr_Sprintf_Verbose(trace_output, DBG_TRACE_MAX_LINE_LENGTH,
                      tr_data, f_name, msg, arg_ptr);

   Tr_Log(fp, trace_output);
}

/*===========================================================================*/
/*!
 * @file dbg_trace_c89_store.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 10-jan-2011 kirk bailey Rev. 3
 *   - Task 19509 - Changed EOL to '\n' and made it changeable.
 *
 * - 25-jan-2011 kirk bailey Rev. 4
 *   - Task 17849 - Added syslog support.
 *
 * - 07-may-2011 Kris Boultbee
 *   Task kok_basa#30663 Revision 5
 *   - Refactored Tr_Store functions to use the new function Tr_Log for the code
 *     that was repeated in all of the Tr_Store functions.
 *   - Renamed file from dbg_trace_c89_printf.c to dbg_trace_c89_store.c
 *
 * - 03-Oct-2013 Kirk Bailey REV 6
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 *
 * - 24-Feb-2014 Paul Casto REV 7
 *   - Added support for different output streams.
 */
/*===========================================================================*/
/** @} doxygen end group */
