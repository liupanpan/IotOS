#ifndef TIME_H
#   define TIME_H
/*===========================================================================*/
/**
 * @file time.h
 *
 * @brief Standard Library Date and Time Functions
 *
 * %full_filespec:time.h-2:incl:kok_aud#1 %
 * @version %version:2 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Fri Mar  9 16:55:13 2007 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 *  This is a partial implementation of standard time.h for SH2a which does not 
 *    provide this header.  
 *
 * ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * TRACEABILITY INFO:
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
 * DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @defgroup time_grp Standard date and time routines
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
 * Number of clock ticks per second. A clock tick is the unit by which
 * processor time is measured and is returned by 'clock'.
 */
#define  CLOCKS_PER_SEC ((clock_t)1000)

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

#ifndef _TIME_T
/**
 * A type for storing the current time and date. 
 * This is the number of seconds since midnight Jan 1, 1970 .
 * @todo is this the right time_t definition?
 */
typedef uint32_t  time_t;
#define _TIME_T
#endif 

#ifndef _CLOCK_T
/**
 * A type for measuring processor time (in clock ticks).
 */
typedef uint32_t clock_t;
#define _CLOCK_T
#endif 


#ifndef _TM_DEFINED
/**
 * A structure for storing all kinds of useful information about the
 * current (or another) time.
 */
struct tm
{
   int   tm_sec;     /**< Seconds: 0-59 */
   int   tm_min;     /**< Minutes: 0-59 */
   int   tm_hour;    /**< Hours since midnight: 0-23 */
   int   tm_mday;    /**< Day of the month: 1-31 */
   int   tm_mon;     /**< Months *since* january: 0-11 */
   int   tm_year;    /**< Years since 1900 */
   int   tm_wday;    /**< Days since Sunday (0-6) */
   int   tm_yday;    /**< Days since Jan. 1: 0-365 */
   int   tm_isdst;   /**< +1 Daylight Savings Time, 0 No DST, -1 don't know */
};
#define _TM_DEFINED
#endif

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/**
 *  Standard C library function for processor time 
 *
 *  @return mS since system startup 
 */ 
clock_t clock (void);


/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 07-Feb-2007 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* TIME_H */
