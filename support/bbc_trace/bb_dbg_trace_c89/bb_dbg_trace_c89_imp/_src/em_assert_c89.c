/*===========================================================================*/
/**
 * @file em_assert_c89.c
 *
 *   Implements default assert logic for bb_trace (calls ANSI C assert).
 *
 * %full_filespec:em_assert_c89.c~kok_inf#2:csrc:kok_basa#1 %
 * @version %version:kok_inf#2 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 09:58:41 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *
 * @section ABBR ABBREVIATIONS:
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
 *   - None
 *
 * @ingroup unit_test
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include <assert.h>
#include "em_cbk.h"

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
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/*===========================================================================*
 *
 * Please refer to the detailed description in em_cbk.h
 *
 *===========================================================================*/
void EM_Assert()
{
   assert(0);
}

/*===========================================================================*/
/*!
 * @file em_assert_c89.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 12-Jun-2011 Kirk Bailey Rev. 1
 *   - Eliminated configurable assert types; replaced with a single callout.
 *
 * - 03-Oct-2013 Kirk Bailey REV 2
 *   - Fixed order of revision history comments - no functional changes
 */
/*===========================================================================*/
/** @} doxygen end group */
