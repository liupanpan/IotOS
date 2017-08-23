/*===========================================================================*/
/**
 * @file low_pass_filter.c
 *
 * Implementation of s simple low pass filter
 *
 * %full_filespec:low_pass_filter.c~kok_aud#4:csrc:tck_aud#1 %
 * @version %version:kok_aud#4 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Wed Jul  8 13:55:22 2009 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Implementation of a simple low pass filter. See user guide for details.
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Utilities.doc
 *
 *   - Requirements Document(s):
 *     - SRS_BASA_Utilities
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "low_pass_filter.h"
#include "pbc_trace.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 9); /**< IDs file for EM assert handling */

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
 *
 * The function implements simple low pass filter.
 *
\*===========================================================================*/
int32_t Low_Pass_Filter(int32_t new_sample, int32_t * running_sum, int32_t integrator)
{
   int32_t new_value;

   PBC_Require(running_sum != NULL, "Low_Pass_Filter called with NULL pointer");

   new_value = (new_sample + *running_sum) / integrator;

   *running_sum = new_sample + *running_sum - new_value;

   return (new_value);
}

/*===========================================================================*\
 *
 * The function initialize low pass filter variables
 *
\*===========================================================================*/
void Low_Pass_Filter_Init(int32_t initial_value, int32_t * running_sum, int32_t integrator)
{
   PBC_Require(running_sum != NULL, "Low_Pass_Filter_Init called with NULL pointer");

   *running_sum = initial_value * (integrator - 1);
}

/*===========================================================================*/
/*!
 * @file low_pass_filter.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 08-Jul-2009 Larry Piekarski Rev 4
 *   - Peer Review Updates
 *
 * - 27-May-2009 Larry Piekarski Rev 3
 *   - Updated Doxygen comments.
 *
 * - 16-Aug-2007 Dan Carman
 *   - Corrected Low_Pass_Filter_Init so that if inital value is passed to 
 *     Low_Pass_Filter the return value will match the input value
 *
 * - 22-May-2007 Bartosz Jamrozik
 *   - Created initial file.
 *
\*===========================================================================*/
