#ifndef PWM_H
#   define PWM_H
/*===========================================================================*/
/**
 * @file pwm.h
 *
 * This is standard interface to generate a PWM output
 *
 * %full_filespec: pwm.h~kok_basa#4:incl:kok_aud#7 %
 * @version %version: kok_basa#4 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 08:19:39 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *    Simple interface for PWM output
 *
 * @section ABBR ABBREVIATIONS:
 *   - PWM - Pulse Width Modulation 
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @defgroup pwm_grp PWM Driver interface 
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*
 * Defines Device specific characteristics 
 *    - List of PWM channel 
 */
#   include "pwm_acfg.h"
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
{          /* ! Place all function prototypes inside these braces ! */
#   endif  /* __cplusplus */
   
/** 
 *  Generate a PWM output 
 *    This function will configure the necessary I/O and timer to generate the 
 *    requested output signal
 *
 * @param [in] channel_num - PWM output channel number (0-(number of PWM outputs-1))
 * @param [in] freq_hz - Frequency of output signal in hertz.  
 *    The actual lower and upper values may be limit by hardware but need to cover minimum of 90 Hz - 300 KHz. 
 *    A frequency of 0, disables the associated timer and PWM output.  
 * @param duty_cycle - This value specifies the percent high time (duty_cycle / 0x10000) of each pulse.
 *    Actual resolution will be based on timer clock source.
 *    - 0 will generate a continous low output  
 *    - 0xFFFF will generate a continous high output  
 */
void PWM_Set_Output(PWM_Channel_T channel_num, uint32_t freq_hz, uint16_t duty_cycle);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}         /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file pwm.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 2009-aug-19 Kris Boultbee
 * + Added extern C wrapper around all function prototypes.
 * 
 * 2007-sep-28 kirk bailey
 * + Got rid of brief directive so that brief comment appears with version
 * + 1.5.3 of Doxygen.
 *
 * 23-Feb-2007 Dan Carman
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* PWM_H */
