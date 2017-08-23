#ifndef DBG_TRACE_IMP_H
#   define DBG_TRACE_IMP_H
/*===========================================================================*/
/**
 * @file dbg_trace_imp.h
 *
 * Defines the interface between files within the dbg_trace implementation.
 *
 * %full_filespec: dbg_trace_imp.h~kok_inf#4:incl:kok_aud#1 %
 * @version %version: kok_inf#4 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Thu Oct  3 09:34:05 2013 %
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
 * @addtogroup dbg_trace
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "dbg_trace.h"
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/
 
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/**
 * Returns an indication of whether the specified trace level is enabled for
 * the module associated with the specified file id.
 *
 * @return true if the trace level is enabled; false otherwise.
 *
 * @param file_id [in] identifier for file issuing trace statement.
 * @param level [in] trace level associated with trace statement.
 */
bool_t tr_Is_Trace_Level_Enabled(uint16_t file_id, Tr_Trace_Level_T level);

#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */

/*===========================================================================*/
/*!
 * @file dbg_trace_imp.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 03-Oct-2013 Kirk Bailey REV 4
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* DBG_TRACE_IMP_H */
