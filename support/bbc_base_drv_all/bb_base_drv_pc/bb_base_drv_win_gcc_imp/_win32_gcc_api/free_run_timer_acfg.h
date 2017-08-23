#ifndef FREE_RUN_TIMER_ACFG_H
#   define FREE_RUN_TIMER_ACFG_H
/*===========================================================================*/
/**
 * @file free_run_timer_acfg.h
 *
 * @brief Free running timer configuration for PC Simulation
 *
 * %full_filespec:free_run_timer_acfg.h~kok_basa#3:incl:kok_aud#1 %
 * @version %version:kok_basa#3 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 08:20:11 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @todo This is a example of a free running timer API configuration file
 *
 * @section ABBR ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @addtogroup bb_free_run_timer_grp Free running timer access
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

#include "reuse.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/**
 *  Frequency in Hz 
 */
#   define FRT_FREQ_HZ    (1000000)

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/** 
 * Free running timer width
 */
   typedef uint32_t FRT_Time_T;

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file api_acfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 * 24-July-2009 Dan Carman
 * + Moved conversion macros to main header file
 *
 * 02-feb-2007 Dan Carman
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* FREE_RUN_TIMER_ACFG_H */

