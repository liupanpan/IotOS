#ifndef PWM_ACFG_H
#   define PWM_ACFG_H
/*===========================================================================*/
/**
 * @file pwm_acfg.h
 *
 * Device micro-specific PWM Channel IDs (VIP)
 *
 * %full_filespec: pwm_acfg.h~kok_basa#2:incl:kok_aud#3 %
 * @version %version: kok_basa#2 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 12:14:00 2010 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    Micro specific PWM channel IDs. This file is configured for the NEC VIP
 *
 * ABBREVIATIONS:
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *
 * DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @addtogroup pwm_grp
 * @{
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * PWM Channel type for VIP
 */
typedef uint8_t PWM_Channel_T;

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 1-Mar-2007 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* PWM_ACFG_H */
