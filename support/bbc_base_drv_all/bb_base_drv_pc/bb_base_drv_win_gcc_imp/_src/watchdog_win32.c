/*===========================================================================*/
/**
 * @file watchdog_win32.c
 *
 * @brief BASA Wrapper for dummy Win32 watchdog
 *
 * %full_filespec: watchdog_win32.c~kok_basa#3:csrc:kok_aud#3 %
 * @version %version: kok_basa#3 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon Nov 15 17:47:22 2010 %
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
 *   Provides standard interface to watchdog. Only supports use of the watchdog 
 *    timer as a watchdog and not as a general purpose timer.  
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
 *   - None
 *
 * @addtogroup watchdog_grp
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

#include "xsal.h"
#include "watchdog.h"

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
 * Initialize watchdog
 *    Configures watchdog settings and starts watchdog (if necessary)
 *    Watchdog should be configured to generate Reset if possible 
 *
 * @param timeout_mS Requested watchdog period - Resulting setting should be 
 *    the smallest setting period greater or equal requested timeout.
 */
void Watchdog_Initialize(uint32_t timeout_mS)
{
   (void)timeout_mS;
}

/*
 * Feeds the watchdog
 */
void Watchdog_Feed(void)
{
}

/*
 * Causes a watchdog reset to occur
 *    Disable interrupts 
 *    Set watchdog to short time delay
 *    Enter endless loop 
 */
void Watchdog_Force_Reset(void)
{
   SAL_Exit(-1);
}

/*===========================================================================*/
/*!
 * @file watchdog_win32.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 * 18-Apr-2007 Dan Carman
 * + Updated DbC macros to add value to log if assert fires
 *
 */
/*===========================================================================*/
