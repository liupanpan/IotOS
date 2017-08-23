#ifndef INTERPOLATE_H
#   define INTERPOLATE_H
/*===========================================================================*/
/**
 * @file interpolate.h
 *
 * API for functions to support Interpolation
 *
 * %full_filespec:interpolate.h~kok_basa#9:incl:tck_aud#1 %
 * @version %version:kok_basa#9 %
 * @author  %derived_by:fz1nnr %
 * @date    %date_modified:Wed Jul 11 09:58:43 2012 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page interpolation_page Interpolation User Manual
 *
 * @section DESC DESCRIPTION:
 *   Interpolation is a method of constructing new data points from a discrete 
 *   set of known data point. For new data calculation linear interpolation 
 *   algorithm is used. It implements the following formula:
 *
 * @image html interpolate.jpg 
 *
 * @section USAGE USAGE INSTRUCTIONS:
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 * -# Define a number of points to use in the interpolation
 * -# Define two tables, one of X values and one of Y values. These tables are 
 *    defined as either int16_t or uint16_t values. Both tables MUST contain the 
 *    same number of values, and must match that in the previous step.
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRACE TRACEABILITY INFO:
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
 * @defgroup interpolate_grp Interpolation
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
/**
 * Remap Interpolate_I16 for backwards compatibility
 */
#   define Interpolate Interpolate_I16

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
 * The function computes the y value corresponding to the x value
 * on a line between (x[i-1]/y[i-1]) and (x[i]/y[i])
 *
 * @return 16-bit interpolated y value
 *
 * @param [in] x           x value
 * @param [in] n_points    number of points in x,y tables
 * @param [in] x_table     x table
 * @param [in] y_table     y table
 *
 * @pre
 *    - n_points >= 2
 *
 * @note x,y tables shall have the same size. The function operates
 * on signed variables.
 *
 */
   int16_t Interpolate_I16(int16_t x, int16_t n_points, const int16_t x_table[], const int16_t y_table[]);

/**
 * The function computes the y value corresponding to the x value
 * on a line between (x[i-1]/y[i-1]) and (x[i]/y[i])
 *
 * @return 16-bit interpolated y value
 *
 * @param [in] x           x value
 * @param [in] n_points    number of points in x,y tables
 * @param [in] x_table     x table
 * @param [in] y_table     y table
 *
 * @pre
 *    - n_points >= 2
 *
 * @note x,y tables shall have the same size. The function operates
 * on unsigned variables.
 *
 */
   uint16_t Interpolate_U16(uint16_t x, uint16_t n_points, const uint16_t x_table[], const uint16_t y_table[]);
   
/**
 * The function computes the y value corresponding to the x value
 * on a line between (x[i-1]/y[i-1]) and (x[i]/y[i])
 *
 * This signed variation is for when the inputs x & x_table are 8 
 * bit and the outputs y & y_table are 16 bit
 *
 * @return 16-bit interpolated y value
 *
 * @param [in] x           x value
 * @param [in] n_points    number of points in x,y tables
 * @param [in] x_table     x table
 * @param [in] y_table     y table
 *
 * @pre
 *    - n_points >= 2
 *
 * @note x,y tables shall have the same size. The function operates
 * on signed variables.
 *
 */
   int16_t Interpolate_I8_I16(int8_t x, int16_t n_points, const int8_t x_table[], const int16_t y_table[]);

/**
 * The function computes the y value corresponding to the x value
 * on a line between (x[i-1]/y[i-1]) and (x[i]/y[i])
 *
 * This unsigned variation is for when the inputs x & x_table are 8 
 * bit and the outputs y & y_table are 16 bit
 *
 * @return 16-bit interpolated y value
 *
 * @param [in] x           x value
 * @param [in] n_points    number of points in x,y tables
 * @param [in] x_table     x table
 * @param [in] y_table     y table
 *
 * @pre
 *    - n_points >= 2
 *
 * @note x,y tables shall have the same size. The function operates
 * on unsigned variables.
 *
 */
   uint16_t Interpolate_U8_U16(uint8_t x, uint16_t n_points, const uint8_t x_table[], const uint16_t y_table[]);
   
/**
 * The function computes the y value corresponding to the x value
 * on a line between (x[i-1]/y[i-1]) and (x[i]/y[i])
 *
 * @return 8-bit interpolated y value
 *
 * @param [in] x           x value
 * @param [in] n_points    number of points in x,y tables
 * @param [in] x_table     x table
 * @param [in] y_table     y table
 *
 * @pre
 *    - n_points >= 2
 *
 * @note x,y tables shall have the same size. The function operates
 * on signed variables.
 *
 */
   int8_t Interpolate_I8(int8_t x, size_t n_points, const int8_t x_table[], const int8_t y_table[]);

/**
 * The function computes the y value corresponding to the x value
 * on a line between (x[i-1]/y[i-1]) and (x[i]/y[i])
 *
 * @return 8-bit interpolated y value
 *
 * @param [in] x           x value
 * @param [in] n_points    number of points in x,y tables
 * @param [in] x_table     x table
 * @param [in] y_table     y table
 *
 * @pre
 *    - n_points >= 2
 *
 * @note x,y tables shall have the same size. The function operates
 * on unsigned variables.
 *
 */
   uint8_t Interpolate_U8(uint8_t x, size_t n_points, const uint8_t x_table[], const uint8_t y_table[]);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file interpolate.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 10-Jul-2012 Kevin Werner
 *   Task kok_basa#107801 Revision 8-9
 *   - Added functions for 8 bit to 16 bit interpolation.
 * 
 * - 21-apr-2010 Kirk Bailey Rev 7
 *   - Moved extern C block to only wrap function prototypes to match new
 *     template.
 *
 * - 28-Aug-2009 Kris Boultbee
 *   Task kok_aud#56145 Revision 6
 *   - Added functions for 8 bit interpolation.
 * 
 * - 27-May-2009 Larry Piekarski Rev 4-5
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 3
 *   - Updated Doxygen Comments
 *
 * - 05-Oct-2007 Bartosz Jamrozik
 *   - Added interpolate function for unsigned values
 *
 * - 02-May-2007 Bartosz Jamrozik
 *   - Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif                          /* INTERPOLATE_H */
