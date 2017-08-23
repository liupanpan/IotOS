#ifndef WATCHDOG_H
#   define WATCHDOG_H
/*===========================================================================*/
/**
 * @file watchdog.h
 *
 * This is the standard interface to the watchdog driver
 *
 * %full_filespec: watchdog.h~kok_basa#4:incl:kok_aud#15 %
 * @version %version: kok_basa#4 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 08:19:48 2010 %
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
 *   Simple interface to H/W watchdog
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
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @defgroup watchdog_grp Watchdog Driver Interface  
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "reuse.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

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
 * Initialize watchdog
 *    Configures watchdog settings and starts watchdog (if necessary)
 *    Watchdog should be configured to generate Reset if possible 
 *
 * @param timeout_mS Requested watchdog period - Resulting setting should be 
 *    the smallest setting period greater or equal requested timeout.
 */
void Watchdog_Initialize(uint32_t timeout_mS);

/**
 * Feeds the watchdog
 *    Restarts the watchdog timeout period.
 */
void Watchdog_Feed(void);

/**
 * Causes a watchdog reset to occur
 */
void Watchdog_Force_Reset(void);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}         /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file watchdog.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 2009-aug-19 Kris Boultbee
 * + Added extern C wrapper around all function prototypes.
 * 
 * 2007-sep-28 kirk bailey
 * + Got rid of brief directive so that brief comment appears with version
 * + 1.5.3 of Doxygen.
 *
 * 23-Feb-2007 Dan Carman
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* WATCHDOG_H */
