#ifndef DBG_TRACE_C99_H
#   define DBG_TRACE_C99_H
/*===========================================================================*/
/**
 * @file dbg_trace_c99.h
 *
 *   API for the debug trace support routines using the C standard library.
 *
 * %full_filespec:dbg_trace_c99.h~kok_inf#5:incl:kok_aud#1 %
 * @version %version:kok_inf#5 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 10:15:48 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR - TRace
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
 *
 * @defgroup dbg_trace_c99 Debug Trace Support for C99 Compilers
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "dbg_trace_cbk.h"
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/
 
/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/
 
/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Translates the compact trace data into a string within the specified
 * buffer. Like the "snprintf" function from which the name is derived,
 * the logic does not write past the end of the buffer, regardless of the
 * expanded size of the resulting string.
 *
 * @return Number of bytes written to buffer; negative value on error.
 *
 * @param buf [inout] Buffer into which result is written.
 * @param buf_size [in] Number of bytes available in output buffer.
 * @param tr_data [in] Structure containing compact trace data to output.
 */
int32_t Tr_Snprintf_Compact(char_t *buf, size_t buf_size,
                            Tr_Compact_Data_T const *tr_data);

/**
 * Translates the trace data into a string within the specified
 * buffer. Like the "snprintf" function from which the name is derived,
 * the logic does not write past the end of the buffer, regardless of the
 * expanded size of the resulting string.
 *
 * @return Number of bytes written to buffer; negative value on error.
 *
 * @param buf [inout] Buffer into which result is written.
 * @param buf_size [in] Number of bytes available in output buffer.
 * @param tr_data [in] Structure containing compact trace data to output.
 * @param f_name [in] name of file containing trace statement.
 * @param msg [in] printf-like format string for trace message
 * @param arg_ptr [in] any variable argument list referenced by format string.
 */
int32_t Tr_Snprintf_Verbose(char_t *buf, size_t buf_size,
                            Tr_Compact_Data_T const *tr_data,
                            char_t const *f_name, char_t const *msg,
                            va_list arg_ptr);

#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */

/*===========================================================================*/
/*!
 * @file dbg_trace_c99.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 08-aug-2008 kirk bailey
 *   - Created initial file.
 *
 * - 03-Oct-2013 Kirk Bailey REV 5
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* DBG_TRACE_C99_H */
