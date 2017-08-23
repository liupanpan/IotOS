#ifndef BB_FREE_RUN_TIMER_H
#   define BB_FREE_RUN_TIMER_H
/*===========================================================================*/
/**
 * @file bb_free_run_timer.h
 *
 * This API provides access to a high resolution free running hardware
 * timer 
 *
 * %full_filespec:bb_free_run_timer.h~kok_inf#8:incl:kok_aud#4 %
 * @version %version:kok_inf#8 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Fri May 16 12:20:44 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Provides standard access to free running hardware timer 
 *    Actually resolution bits and time step are micro depdendent
 *
 * @section ABBR ABBREVIATIONS:
 *   - FRT - Free Running Time
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
 * @defgroup bb_free_run_timer_grp Free running timer access
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "free_run_timer_acfg.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/
/**
 *  Conversion macro uS to Free running timer counts 
 * 
 *  @param uS microseconds to convert 
 * 
 *  @return timer counts 
 */
#   define FRT_uS_to_Counts(uS)     ((FRT_Time_T) (((uS) * (FRT_FREQ_HZ / 1000UL)) / 1000UL))

/**
 *  Conversion macro nS to Free running timer counts 
 * 
 *  @param nS microseconds to convert 
 * 
 *  @return timer counts 
 */
#   define FRT_nS_to_Counts(nS)     (((FRT_Time_T) (((nS) * (FRT_FREQ_HZ / 10000UL)) + 99999UL) / 100000UL))

/**
 *  Conversion macro Free running timer counts to uS 
 * 
 *  @param cnts timer counts
 * 
 *  @return time in uS
 */
#   define FRT_Counts_to_uS(cnts)   (((cnts) * 1000UL) / (FRT_FREQ_HZ / 1000UL))

/**
 *  Conversion macro Free running timer counts to nS 
 * 
 *  @param cnts timer counts
 * 
 *  @return time in nS
 */
#   define FRT_Counts_to_nS(cnts)   (((cnts) * 10000UL) / (FRT_FREQ_HZ / 100000UL))

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/
#   ifdef __cplusplus
extern "C"
{          /* ! Place all function prototypes inside these braces ! */
#   endif  /* __cplusplus */
   
/** 
 * Initialize the free running timer 
 *
 */
   void FRT_Initialize_Timer(void);

/** 
 * Return current free running timer count 
 *   Timer is always returned as count up timer regardless of hardware 
 *
 *  @return timer count 
 */
   FRT_Time_T FRT_Get_Timer_Count(void);


/** 
 * Return delta in free running timer counts 
 *
 * @param [in] previous_count Reference starting point 
 *
 * @return Elapsed number of timer counts  
 *
 * @note The elapsed time will be modulo of the number bits in the counter.
 *    (i.e., it ignores multiple wraps of the counter)
 */
   FRT_Time_T FRT_Get_Elapsed_Counts(FRT_Time_T previous_count);

#   ifndef FRT_FREQ_HZ

#      define FRT_FREQ_HZ FRT_Freq_Hz()

/** 
 * Return frequency of free running timer in Hz 
 *
 * @return Frequency (Hz)
 *
 */
   uint32_t FRT_Freq_Hz(void);
#   endif

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}         /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file bb_free_run_timer.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 2014-May-16 Kirk Bailey Rev. 8
 * + Fixed FRT_nS_to_Counts().
 *
 * 2009-aug-19 Kris Boultbee
 * + Added extern C wrapper around all function prototypes.
 * + Fixed line ending issue with this file.
 * 
 * 14-Jul-2008 Dan Carman
 * + Move common macros here based on new interface to return frequency
 *
 * 2007-sep-28 kirk bailey
 * + Got rid of brief directive so that brief comment appears with version
 * + 1.5.3 of Doxygen.
 *
 * 24-may-2007 kirk bailey
 * + Fixed spelling of "Elasped".
 *
 * 12-feb-2007 Dan Carman
 * + Created initial file.
 *
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* FREE_RUN_TIMER_H */
