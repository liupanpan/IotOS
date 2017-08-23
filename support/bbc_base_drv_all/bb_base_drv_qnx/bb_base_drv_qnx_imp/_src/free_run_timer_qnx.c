/*===========================================================================*/
/**
 * @file free_run_timer_qnx.h
 *
 * @brief Implements free running timer simualation for win32
 *
 * %full_filespec:free_run_timer_qnx.c~2:csrc:kok_inf#1 %
 * @version %version:2 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Dec 15 12:27:19 2014 %
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
 * Simulate interface for free running timer
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
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

#include "bb_free_run_timer.h"
#include <time.h>

/*===========================================================================*\
 * Configuration Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/*
 * Initialize the free running timer
 */
void FRT_Initialize_Timer(void)
{
}

/*
 * Return current free running timer count
 *
 * return nSecs
 */
FRT_Time_T FRT_Get_Timer_Count(void)
{
   struct timespec time_buf;

   (void)clock_gettime(CLOCK_MONOTONIC, &time_buf);

   return (FRT_Time_T) ((time_buf.tv_sec * 1000000000L) + time_buf.tv_nsec);
}

/*
 * Return delta in free running timer counts
 */
FRT_Time_T FRT_Get_Elapsed_Counts(FRT_Time_T previous_count)
{
   return (FRT_Time_T) (FRT_Get_Timer_Count() - previous_count);
}

/*===========================================================================*/
/*!
 * @file free_run_timer_qnx.h
 *
 * 12/15/14 - Dan Carman
 *    Changed Free running timer to use CLOCK_MONOTONIC o avoid discontinuities
 *    when system time is changed.
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 */
/*===========================================================================*/
