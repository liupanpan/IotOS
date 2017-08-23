#ifndef DBG_TRACE_PS_H
#   define DBG_TRACE_PS_H
/*===========================================================================*/
/**
 * @file dbg_trace_ps.h
 *
 * Defines the callout interface for the debug trace module.
 *
 * %full_filespec:dbg_trace_ps.h~kok_inf#2:incl:kok_basa#1 %
 * @version %version:kok_inf#2 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 09:34:00 2013 %
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

/*===========================================================================*
 * Exported Preprocessor #define Constants for Callouts
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS for Callouts
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */
/*===========================================================================*
 * Exported Type Declarations for Callouts
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes for Callouts
 *===========================================================================*/

/**
 * Callout function that must be implemented to read the dbg_trace levels from the
 * specified file into the specified location.
 *
 * @param path  The path to the file from which to read the data.
 * @param levels The array into which the data is to be read.
 * @param num_levels The number of elements in the array.
 */
void Tr_PS_Read_Trace_Levels(char const *path, Tr_Trace_Level_T *levels, size_t num_levels);

/**
 * Callout function that must be implemented to save the dbg_trace levels to the
 * specified file.
 *
 * @param path The path to the file to which the data is to be written.
 * @param levels The array containing the data to write.
 * @param num_levels The number of elements in the array.
 */
void Tr_PS_Save_Trace_Levels(char const *path, Tr_Trace_Level_T const *levels, size_t num_levels);

#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */
#
/*===========================================================================*/
/*!
 * @file dbg_trace_ps.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 31-May-2011 kirk bailey
 *   - Created initial file.
 *
 * - 03-Oct-2013 Kirk Bailey REV 2
 *   - Fixed order of revision history comments - no functional changes
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* DBG_TRACE_PS_H */
