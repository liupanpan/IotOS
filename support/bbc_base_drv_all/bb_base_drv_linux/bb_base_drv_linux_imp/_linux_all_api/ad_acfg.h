#ifndef AD_ACFG_H
#   define AD_ACFG_H
/*===========================================================================*/
/**
 * @file ad_acfg.h
 *
 * Defines Available micro-specific A/D channels
 *
 * %full_filespec: ad_acfg.h~1:incl:kok_basa#54 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 13:21:04 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *    Defines micro-specific A/D channels.
 *
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @addtogroup ad_grp
 * @{
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/
/**
 * A/D Reference voltage in mV
 *   For a single channel read, the voltage at the pin is
 *       mV = ((counts * AD_REFERENCE_mVOLTS) / 0xFFFF)
 */
#   define AD_REFERENCE_mVOLTS  (3300)

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * List of available A/D Channels
 */
typedef enum AD_Channel_Tag
{
VAD_ANI0, /**< VIP Analog Channel 0 */
      VAD_ANI1, /**< VIP Analog Channel 1 */
      VAD_ANI2, /**< VIP Analog Channel 2 */
      VAD_ANI3, /**< VIP Analog Channel 3 */
      VAD_ANI4, /**< VIP Analog Channel 4 */
      VAD_ANI5, /**< VIP Analog Channel 5 */
      VAD_ANI6, /**< VIP Analog Channel 6 */
      VAD_ANI7, /**< VIP Analog Channel 7 */
      VAD_ANI8, /**< VIP Analog Channel 8 */
      VAD_ANI9, /**< VIP Analog Channel 9 */
      VAD_ANI10, /**< VIP Analog Channel 10 */
      VAD_ANI11, /**< VIP Analog Channel 11 */
   AD_NUM_CHANNELS
} AD_Channel_T;

/*===========================================================================*/
/*!
 * @file ad_acfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 29-Sept-2008 Dan Carman
 * + Modified to define generic channel names, logical names defined in io_mapping.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* AD_ACFG_H */
