#ifndef CONVERSIONS_H
#   define CONVERSIONS_H
/*===========================================================================*/
/**
 * @file conversions.h
 *
 * API for functions used to perform conversions.
 *
 * %full_filespec:conversions.h~kok_basa#7:incl:tck_aud#1 %
 * @version %version:kok_basa#7 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Mon Jul 12 12:42:49 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page conversion_page Conversions User Manual
 *
 * @section DESC DESCRIPTION:
 *
 * The conversions sub-module provides set of functions to convert data from BCD 
 * coding to hexadecimal coding and vice-versa. It also provides functions to 
 * perform simple operations on BCD values like increment or decrement and check 
 * if given data is in BCD format. 
 *
 * The sub-module also contains the function #Scale which can be used to scale 
 * input value from one range to another.
 *
 * @section USAGE USAGE INSTRUCTIONS:
 *
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 *
 * @section ABBR ABBREVIATIONS:
 *   - BCD - Binary Coded Decimal
 *
 * @section TRA TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Utilities.doc
 *
 *   - Requirements Document(s):
 *     - SRS_BASA_Utilities.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup conversions_grp Data conversions
 * @ingroup utilities_grp
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
{

#   endif                       /* __cplusplus */
/**
 * The function converts a received BCD value to hexadecimal value.
 *
 * @return Converted value in hexadecimal format
 *
 * @param [in] BCD_Value   BCD value to be converted to hex
 * @param [in] num_digits  number of digits of BCD value to be converted
 *
 * @note input BCD_Value must be in BCD format - use IS_BCD function to check
 *
 * @see Hex_To_BCD()
 */
   uint32_t BCD_To_Hex(const uint32_t BCD_Value, size_t num_digits);

/**
 * The function converts hexadecimal to a BCD value.
 *
 * @return Converted value in BCD format
 *
 * @param [in] HexValue   Hexadecimal value to be converted to BCD
 * @param [in] num_digits number of digits in BCD result
 *
 * @see BCD_To_Hex()
 */
   uint32_t Hex_To_BCD(const uint32_t HexValue, size_t num_digits);

 /**
 * This routine tests if input value is in BCD value.
 *
 * @return True if BCD, false if not BCD
 *
 * @param [in] Potential_BCD_Value Value to check if BCD (BCD value to be converted to hex)
 *
 */
   bool_t Is_BCD(uint32_t Potential_BCD_Value);

 /**
 * The function increments a received BCD value and wraps at the specified number of BCD digits
 *
 * @return BCD value incremented by one if valid BCD received; 0xFFFFFFFF if invalid BCD received.
 *
 * @param [in] BCD_value BCD value to be incremented
 * @param [in] num_digits number of BCD digits at which wrapping occurs
 *
 * @see BCD_Decrement(), Is_BCD()
 */
   uint32_t BCD_Increment(uint32_t BCD_value, size_t num_digits);

 /**
 * The function decrements a received BCD value wraps at the specified number of BCD digits
 *
 * @return BCD value decremented by one if valid BCD received; 0xFFFFFFFF if invalid BCD received.
 *
 * @param [in] BCD_value BCD value to be decremented
 * @param [in] num_digits number of BCD digits at which wrapping occurs
 *
 * @see BCD_Increment(), Is_BCD()
 */
   uint32_t BCD_Decrement(uint32_t BCD_value, size_t num_digits);

 /**
 * Scales a value from one range to another.
 *
 * @return Scaled value
 *
 * @param [in,out] input   value to be scaled
 * @param [in,out] old_min minimum value of old range
 * @param [in,out] old_max maximum value of old range
 * @param [in,out] new_min minimum value of new scaled range
 * @param [in,out] new_max maximum value of new scaled range
 *
 */
   int32_t Scale(int32_t input, int32_t old_min, int32_t old_max, int32_t new_min, int32_t new_max);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file conversions.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 7
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 21-apr-2010 Kirk Bailey Rev 6
 *   - Moved extern C block to only wrap function prototypes to match new
 *     template.
 *
 * - 17_Jun-2009 Larry Piekarski Rev 4-5
 *   - Peer Review Updates
 *
 * - 27-May-2009 Larry Piekarski Rev 3
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 2
 *   - Updated Doxygen Comments
 *
 * - 02-May-2007 Bartosz Jamrozik
 *   - Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif                          /* CONVERSIONS_H */
