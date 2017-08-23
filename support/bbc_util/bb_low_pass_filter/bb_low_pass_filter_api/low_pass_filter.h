#ifndef LOW_PASS_FILTER_H
#   define LOW_PASS_FILTER_H
/*===========================================================================*/
/**
 * @file low_pass_filter.h
 *
 * API for simple low pass filter implementation
 *
 * %full_filespec:low_pass_filter.h~kok_basa#6:incl:tck_aud#1 %
 * @version %version:kok_basa#6 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed Apr 21 10:10:35 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page low_pass_filter_page Low Pass Filter User Manual
 *
 * @section DESC DESCRIPTION:
 * Digital low pass filter is designed for filtering samples of measured data. 
 * Using low pass filtering ensure that the filtered parameter is resistant of 
 * random variation. 
 * Formulas describing low pass filter implementation are shown below:
 * @image html low_pass_filter.jpg
 *
 * The low pass filter algorithm is implemented in #Low_Pass_Filter. The function 
 * #Low_Pass_Filter_Init initializes the running sum.
 *
 * Use the file low pass filter design.xls to aid in setting the integrator value. The
 * spreadsheet may be found in the doc directory.
 *
 * @image html excel_tool.jpg
 *  
 * You need to specificaitons to caculate the integrator, delta_t and Tau.
 * - delta_t is the peridoc rate at which the filter is called.
 * - Tau is the desired time constant
 *
 * Asumming periodic sample time delta_t, the time constant for the filter = 
 *
 * @image html tau_equation.jpg
 *
 * Given a desired time constant and sample time delta_t, the integrator 
 * constant can be computed as:
 *
 * @image html integrator_equation.jpg
 *
 * The filter cutoff frequency in Hertz for a given integrator constant and sample rate is
 *
 * @image html cutoff_freq_equation.jpg
 *
 * @section USAGE USAGE INSTRUCTIONS:
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 * -# Define a signed 32-bit variable for the running sum.
 * -# Detmine the integrator value
 * -# Call #Low_Pass_Filter_Init to set the running sum
 * -# Call #Low_Pass_Filter at the delta_t rate used to calculate the integrator
 *
 * @section ABBR ABBREVIATIONS:
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
 * @defgroup low_pass_filter_grp Low Pass Filter
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
 * The function implements simple low pass filter (used to
 * filter measured value).
 *
 * Assuming periodic sample time delta_t, the time constant for the filter = 
 *
 *    time_constant = -delta_t / ln((integrator - 1)/ integrator)
 *
 * Given a desired time constant and sample time delta_t, the integrator 
 * constant can be computed as:
 *
 *    integrator = round(1 / ( 1 - e^(-delta_t / time_constant)))  ~= time_constant / delta_t
 *
 * The filter cutoff frequency in Hertz for a given integrator constant and sample rate is
 *
 =    fc = - ln((integrator-1)/integrator)/(2*PI*delta_t)  
 *
 * @return filtered value - average
 *
 * @param [in] new_sample   new sample to filter
 * @param [in] running_sum  pointer to running sum
 * @param [in] integrator   integrator - number of samples to be taken
 *                          for average calculation
 * 
 * @pre
 *    - running_sum != NULL 
 */
   int32_t Low_Pass_Filter(int32_t new_sample, int32_t * running_sum, int32_t integrator);

/**
 * The function initialize low pass filter variables
 *
 * @param [in] initial_value  initial filtered value
 * @param [in] running_sum    pointer to running sum
 * @param [in] integrator     integrator - number of samples to be taken
 *                            for average calculation
 * 
 * @pre
 *    - running_sum != NULL 
 */
   void Low_Pass_Filter_Init(int32_t initial_value, int32_t * running_sum, int32_t integrator);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/**
 * @file low_pass_filter.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
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
 * - 09-May-2008 Dan Carman
 *    - Update description of low pass filter 
 *
 * - 02-May-2007 Bartosz Jamrozik
 *    - Created initial file.
 *
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* LOW_PASS_FILTER_H */
