/*===========================================================================*/
/**
 * @file em_stub.c
 *
 * Implements the EM store trace log callout
 *       This version simply prints out the debug information.
 *
 * %full_filespec:em_stub.c~9:csrc:kok_aud#3 %
 * @version %version:9 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Jul 15 17:35:14 2009 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *    - EM_Bench places the thread into an endless loop
 *    - EM_Restart forces watchdog reset to restart system
 *
 * @section ABBR ABBREVIATIONS:
 *   - EM = Error Management
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @addtogroup em
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "em_cbk.h"
#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include "xsal.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(TEST_MODULE_ID, 14); /* define this file for assert handling */

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

/*===========================================================================*\
 * Please refer to the detailed description in em_cbk.h
\*===========================================================================*/
void EM_Bench(void)
{
   Tr_Fault("Entered EM_Bench ... will not exit\n");
   while (1)
   {
      SAL_Sleep(1);
   }
}

/*===========================================================================*\
 * Please refer to the detailed description in em_cbk.h
\*===========================================================================*/
void EM_Restart(void)
{
   Tr_Fault("Entered EM_Restart ... will not return\n");
   SAL_Sleep(250);
   while (1)
   {
      SAL_Sleep(1);
   }
}

/*===========================================================================*/
/*!
 * @file em_stub.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 30-June-2009 Dan Carman
 * + Replace printf with Trace calls
 *
 * 07-Jan-2009 Dan Carman
 * + Remove EM Trace Storage functions
 *
 * 15-Sept-2008 Dan Carman
 * + Added SAL_Sleep to EM_Bench to allow debugger threads to run.
 *
 * 16-Aug-2007 Dan Carman
 * + MISRA changes
 *
 * 25-apr-2007 kirk bailey
 * + Changed %d to %u to eliminate gcc warnings.
 *
 * 04-apr-2007 kirk bailey
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
