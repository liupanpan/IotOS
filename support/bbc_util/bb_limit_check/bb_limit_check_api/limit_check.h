#ifndef LIMIT_CHECK_H
#   define LIMIT_CHECK_H
/*===========================================================================*/
/**
 * @file limit_check.h
 *
 * API for functions to limit variable to min - max range
 *
 * %full_filespec:limit_check.h~kok_inf#7:incl:kok_aud#1 %
 * @version %version:kok_inf#7 %
 * @author  %derived_by:rzvkfk %
 * @date    %date_modified:Mon Jul 21 16:47:23 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page limit_check_page Limit Check User Manual
 *
 * @section DESC DESCRIPTION:
 * The limit and wrap check provides three functions:
 * -	#Util_Limit_Check - returns maximum value when input value exceeds given 
 *    range and minimum value when input value is below given range. If input 
 *    value is within given range the function returns input value.
 * 
 * -	#Util_Wrap_Check - returns minimum value when input value exceeds given 
 *    range and maximum value when input value is below given range. If input 
 *    value is within given range the function returns input value.
 * 
 * -	#Util_Wrap_Check_With_Offset - returns minimum value with added offset 
 *    when input value exceeds given range. Offset is calculated as difference 
 *    between input value and maximum value.  Returns maximum value with 
 *    subtracted offset when input value is below given range. In this case 
 *    offset is calculated as difference between minimum values and given input 
 *    value. If input value is within given range the function returns input value.
 *
 * @section USAGE USAGE INSTRUCTIONS:
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 *
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Utilities.doc
 *
 *   - Requirements Document(s):
 *     - SRS_Utilities.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup limit_check_grp Utilities to limit range of variables
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
 * Forces value to within min and max inclusive
 *
 * @return Limit checked value
 *
 * @param [in] value value to limit
 * @param [in] min   lower allowable value
 * @param [in] max   upper allowable value
 *
 * @pre 
 *    - max >= min
 *
 * @see Util_Wrap_Check(), Util_Wrap_Check_With_Offset()
 */
   int32_t Util_Limit_Check(int32_t value, int32_t min, int32_t max);

/**
 * Wraps a value if out of limits (i.e. > max goes to min)
 *
 * @return wrapped value
 *
 * @param [in] value value to wrap
 * @param [in] min   lower allowable value
 * @param [in] max   upper allowable value
 *
 * @pre 
 *    - max >= min
 *
 * @see Util_Limit_Check(), Util_Wrap_Check_With_Offset()
 */
   int32_t Util_Wrap_Check(int32_t value, int32_t min, int32_t max);

/**
 * Wraps a value if out of limits and adds an offset. For example, assume 
 * min = 1, max = 10. If the function is called with value=5, then the return 
 * value is 5. If the function is called with value=11, then the return 
 * value is 1. If the function is called with value=12, then the return 
 * value is 2.
 *
 * @return wrapped value increased with calculated offset
 *
 * @param [in] value value to wrap
 * @param [in] min   lower allowable value
 * @param [in] max   upper allowable value
 *
 * @pre 
 *    - max > min
 *
 * @see Util_Limit_Check(), Util_Wrap_Check()
 */
   int32_t Util_Wrap_Check_With_Offset(int32_t value, int32_t min, int32_t max);

/**
 * Add requested delta, if resulting value is higher than max this function will
 * wrap as many times as needed over the valid range (i.e. max - min). Note for a
 * complete wrap over the valid range a delta value of range + 1 will be needed
 *
 * Example1: min = 1, max = 10, inc_delta = 5. If the function is called with:
 * base_value = 1, return value will be 6
 * base_value = 5, return value will be 10
 * base_value = 6, return value will be 1
 * base_value = 8, return value will be 3
 * base_value = 10, return value will be 5
 *
 * Example2: min = 1, max = 10, inc_delta = 25. If the function is called with:
 * base_value = 1, return value will be 6
 * base_value = 5, return value will be 10
 * base_value = 6, return value will be 1
 * base_value = 8, return value will be 3
 * base_value = 10, return value will be 5
 *
 * Example3: min = 0, max = 10, inc_delta = 5. If the function is called with:
 * base_value = 0, return value will be 5
 * base_value = 5, return value will be 10
 * base_value = 6, return value will be 0
 * base_value = 8, return value will be 2
 * base_value = 10, return value will be 4
 *
 * Example4: min = 0, max = 10, inc_delta = 27. If the function is called with:
 * base_value = 0, return value will be 5
 * base_value = 5, return value will be 10
 * base_value = 6, return value will be 0
 * base_value = 8, return value will be 2
 * base_value = 10, return value will be 4
 *
 * @return wrapped value increased with requested delta
 *
 * @param [in] base_value   initial value where delta will be added
 * @param [in] inc_delta    delta value to be added
 * @param [in] min          lower allowable value
 * @param [in] max          upper allowable value
 *
 * @pre
 *    - max > min
 *    - min <= base_value <= max
 *
 * @see Util_Limit_Check(), Util_Wrap_Check()
 */
   int32_t Increment_Delta_With_Wrap_Check(int32_t base_value, int32_t inc_delta, int32_t min, int32_t max);

/**
 * Decrement requested delta, if resulting value is lower than min this function will
 * wrap as many times as needed over the valid range (i.e. max - min). Note for a
 * complete wrap over the valid range a delta value of range + 1 will be needed
 *
 * Example1: min = 1, max = 10, dec_delta = 5. If the function is called with:
 * base_value = 1, return value will be 6
 * base_value = 5, return value will be 10
 * base_value = 6, return value will be 1
 * base_value = 8, return value will be 3
 * base_value = 10, return value will be 5
 *
 * Example2: min = 1, max = 10, inc_delta = 25. If the function is called with:
 * base_value = 1, return value will be 6
 * base_value = 5, return value will be 10
 * base_value = 6, return value will be 1
 * base_value = 8, return value will be 3
 * base_value = 10, return value will be 5
 *
 * Example3: min = 0, max = 10, inc_delta = 5. If the function is called with:
 * base_value = 0, return value will be 6
 * base_value = 5, return value will be 0
 * base_value = 6, return value will be 1
 * base_value = 8, return value will be 3
 * base_value = 10, return value will be 5
 *
 * Example4: min = 0, max = 10, inc_delta = 27. If the function is called with:
 * base_value = 0, return value will be 6
 * base_value = 5, return value will be 0
 * base_value = 6, return value will be 1
 * base_value = 8, return value will be 3
 * base_value = 10, return value will be 5
 *
 * @return wrapped value decreased with requested delta
 *
 * @param [in] base_value    initial value where delta will be decreased
 * @param [in] dec_delta     delta value to be decreased
 * @param [in] min           lower allowable value
 * @param [in] max           upper allowable value
 *
 * @pre
 *    - max > min
 *    - min <= base_value <= max
 *
 * @see Util_Limit_Check(), Util_Wrap_Check()
 */
   int32_t Decrement_Delta_With_Wrap_Check(int32_t base_value, int32_t dec_delta, int32_t min, int32_t max);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file limit_check.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 17-Jul-2014 Jose Luis Pulido Rev 7
 *   - kok_inf#9931.- Add new Increment/Decrement Delta with offset Services
 *
 * - 21-apr-2010 Kirk Bailey Rev 6
 *   - Moved extern C block to only wrap function prototypes to match new
 *     template.
 *
 * - 27-May-2009 Larry Piekarski Rev 4-5
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 3
 *   - Updated Doxygen Comments
 *
 * - 22-May-2007 Bartosz Jamrozik
 *   - Added new function prototypes:
 *     - Util_Wrap_Check
 *     - Util_Wrap_Check_With_Offset
 *
 * - 09-Mar-2007 Dan Carman
 *   - Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif                          /* LIMIT_CHECK_H */
