/*===========================================================================*/
/**
 * @file printf_gate.c
 *
 *  @brief Provides a mutex to synchronize use of printf
 *
 * %full_filespec:printf_gate.c~1:csrc:kok_inf#1 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Apr 15 17:34:27 2015 %
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
 * ABBREVIATIONS:
 *   - EM = Error Management
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Error_Management.doc (version 0.7)
 *
 *   - Requirements Document(s):
 *     - BASA_SRS_Error_Management_1.2.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @addtogroup em
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "pbc_trace.h"
#include "printf_gate.h"
#include "printf_gate_cfg.h"
#include "reuse.h"

#ifndef PRINTF_GATE_STANDALONE
#   include "xsal.h"
#endif /* PRINTF_GATE_STANDALONE */

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(TEST_MODULE_ID, 12); /* define this file for assert handling */

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

#ifndef PRINTF_GATE_TIMEOUT
/** Maximum timeout to wait for mutex */
#define PRINTF_GATE_TIMEOUT   (1000)
#endif

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/** mutex to control access to printf 
 */
#ifndef PRINTF_GATE_STANDALONE
static SAL_Mutex_T printf_gate = SAL_MUTEX_INITIALIZER;
#endif

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

void Printf_Lock(void)
{
#ifndef PRINTF_GATE_STANDALONE
   (void) SAL_Lock_Mutex_Timeout(&printf_gate, PRINTF_GATE_TIMEOUT);
#endif
}

void Printf_Unlock(void)
{
#ifndef PRINTF_GATE_STANDALONE
   (void) SAL_Unlock_Mutex(&printf_gate);
#endif
}

/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 25-Jul-2010 Kirk Bailey Rev 7
 * + Replaced "bool" with "bool_t".
 *
 * 22-aug-2008 kirk bailey
 * + Added PRINTF_GATE_STANDALONE configuration option.
 *
 * 12-sep-2007 kirk bailey
 * + Change EM_ASSERT_ERROR args. so the module will build with QNX. The
 * + previous implementation only worked if the semaphore type was not a
 * + struct.
 *
 * 16-Aug-2007 Dan Carman
 * + Used self_init for thread safety
 * + MISRA changes
 *
 * 04-apr-2007 kirk bailey
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
