#ifndef FREE_RUN_TIMER_ACFG_H
#   define FREE_RUN_TIMER_ACFG_H
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */
/*===========================================================================*/
/**
 * @file free_run_timer_acfg.h
 *
 * @brief Free running timer configuration for J2
 *
 * %full_filespec:free_run_timer_acfg.h~kok_basa#2:incl:kok_aud#14 %
 * @version %version:kok_basa#2 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Feb  4 13:25:24 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008 Delphi Technologies, Inc., All Rights Reserved.
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

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/** 
 * Define frequency of free running timer in hertz
 *   If this macro is not defined, the header will assume that the function
 *   FRT_Freq_Hz() will return the frequency
 */
#define FRT_FREQ_HZ    (20000000UL)

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/** 
 * Free running timer width
 * Using 32 bit RTI0 timer as free running timer
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
 * 02-feb-2007 Dan Carman
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */
#endif /* FREE_RUN_TIMER_ACFG_H */
