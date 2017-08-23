/*===========================================================================*/
/**
 * @file posix_printf_gate.c
 *
 *   Implements Printf_Lock/Unlock for POSIX systems.
 *
 * %full_filespec: posix_printf_gate.c~1:csrc:kok_inf#1 %
 * @version %version: 1 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Tue May 12 09:23:19 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR = TRace.
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
 *   - none
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

#include "dbg_trace_cfg.h"

#if TR_STAND_ALONE == false

#include "dbg_trace_cbk.h"
#include <pthread.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/
/** Used to output to display from multiple threads with min. confusion. */
static pthread_mutex_t Display_Mutex = PTHREAD_MUTEX_INITIALIZER;

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

void Printf_Lock(void)
{
   /* if we failed, continue anyways */
   (void) pthread_mutex_lock(&Display_Mutex);
}

void Printf_Unlock(void)
{
   /* if we failed, continue anyways */
   (void) pthread_mutex_unlock(&Display_Mutex);
}

#endif /* TR_STAND_ALONE != false */

/*===========================================================================*/
/*!
 * @file posix_printf_gate.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 12-May-2015 Kirk Bailey
 *   - Moved Printf_Lock/Unlock out of dbg_trace_posix.c
 */
/*===========================================================================*/
