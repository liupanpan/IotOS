/*===========================================================================*/
/**
 * @file limit_check.c
 *
 * Implements function to limit check variable
 *
 * %full_filespec: limit_check.c~kok_inf#7:csrc:kok_aud#1 %
 * @version %version: kok_inf#7 %
 * @author  %derived_by:rzvkfk %
 * @date    %date_modified: Mon Jul 21 16:47:17 2014 %
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
 * Function to limit check variable to min..max range
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
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
#include "limit_check.h"
#include "pbc_trace.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 6); /**< IDs file for EM assert handling */

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
 * Forces value to within min and max inclusive
 */
int32_t Util_Limit_Check(int32_t value, int32_t min, int32_t max)
{
   PBC_Require(max >= min, "Util_Limit_Check must be called with max >= min");

   if (value < min)
   {
      value = min;
   }
   else if (value > max)
   {
      value = max;
   }
   else
   {
      /* do nothing - value in range */
   }
   return (value);
}

/*
 * Wraps a value if out of limits (i.e > max goes to min)
 */
int32_t Util_Wrap_Check(int32_t value, int32_t min, int32_t max)
{
   PBC_Require(max >= min, "Util_Wrap_Check must be called with max >= min");

   if (value < min)
   {
      value = max;
   }
   else if (value > max)
   {
      value = min;
   }
   else
   {
      /* do nothing - value in range */
   }

   return (value);
}

/*
 * Wraps a value if out of limits and adds offset
 */
int32_t Util_Wrap_Check_With_Offset(int32_t value, int32_t min, int32_t max)
{
   int32_t range = max - min;
   int32_t delta;

   PBC_Require(max > min, "Util_Wrap_Check_With_Offset must be called with max > min");

   if (value < min)
   {
      delta = ((min - value) / range) * range;
      value = range + value + delta;
   }
   else if (value > max)
   {
      delta = ((value - 1 - max) / range) * range;
      value = value - range - delta;
   }
   else
   {
      /* do nothing - value in range */
   }

   return (value);
}

/*
 * Add requested delta, if resulting value is higher than max_value this function will
 * wrap as many times as needed over the valid range (i.e. max - min).
 */
int32_t Increment_Delta_With_Wrap_Check(int32_t base_value, int32_t inc_delta, int32_t min, int32_t max)
{
   int32_t full_wrap = (max - min) + 1;
   int32_t calculated_value = 0;
   int32_t actual_delta = inc_delta;

   PBC_Require(max > min, "Increment_Delta_With_Wrap_Check must be called with max > min");
   PBC_Require(min <= base_value, "Increment_Delta_With_Wrap_Check must be called with min <= base_value");
   PBC_Require(max >= base_value, "Increment_Delta_With_Wrap_Check must be called with max >= base_value");

   if(inc_delta > full_wrap)
   {
      actual_delta = inc_delta%full_wrap;
   }

   calculated_value = base_value + actual_delta;
   if(calculated_value > max)
   {
      actual_delta = actual_delta - (max - base_value) - 1;
      calculated_value = min + actual_delta;
   }

   return(calculated_value);
}

/*
 * Decrement requested delta, if resulting value is lower than min_value this function will
 * wrap as many times as needed over the valid range (i.e. max - min).
 */
int32_t Decrement_Delta_With_Wrap_Check(int32_t base_value, int32_t dec_delta, int32_t min, int32_t max)
{
   int32_t full_wrap = (max - min) + 1;
   int32_t calculated_value = 0;
   int32_t actual_delta = dec_delta;

   PBC_Require(max > min, "Decrement_Delta_With_Wrap_Check must be called with max > min");
   PBC_Require(min <= base_value, "Decrement_Delta_With_Wrap_Check must be called with min <= base_value");
   PBC_Require(max >= base_value, "Decrement_Delta_With_Wrap_Check must be called with max >= base_value");

   if(dec_delta > full_wrap)
   {
      actual_delta = dec_delta%full_wrap;
   }

   calculated_value = base_value - actual_delta;
   if(calculated_value < min)
   {
      actual_delta = actual_delta - (base_value - min) - 1;
      calculated_value = max - actual_delta;
   }

   return(calculated_value);
}

/*===========================================================================*/
/*!
 * @file limit_check.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 17-Jul-2014 Jose Luis Pulido Rev 7
 *   - kok_inf#9931.- Add new Increment/Decrement Delta with offset Services
 *
 * - 27-May-2009 Larry Piekarski Rev 3-5
 *   - Cleaned up code from review checklist
 *
 * - 03-Apr-2007  Klaus Krosse
 *   - added Util_ prefix to function name
 *
 * - 09-Mar-2007  Dan Carman
 *   - Created initial file.
 *
\*===========================================================================*/
