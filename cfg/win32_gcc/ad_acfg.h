#ifndef AD_ACFG_H
#   define AD_ACFG_H
/*===========================================================================*/
/**
 * @file ad_acfg.h
 *
 * @brief Defines Project / micro-specific A/D channels
 *
 * %full_filespec: ad_acfg.h~1:incl:kok_aud#80 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon Nov  2 17:07:55 2009 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    Defines Project / micro-specific A/D channels. 
 *    This file is configured for the VW3 A Sample board
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

/** X macro to  define muxed A/D inputs. 
 *  if no muxed inputs present then leave undefined. 
 */
#      define AD_MUX_INPUTS   \
   AD_MUX_INPUT(AD_PSGOOD,          AD_MUX_IN1, 0)   \
   AD_MUX_INPUT(AD_CLIP,            AD_MUX_IN1, 1)   \
   AD_MUX_INPUT(AD_ANT1_DIAG_REF,   AD_MUX_IN1, 2)   \
   AD_MUX_INPUT(AD_ANT2_DIAG_REF,   AD_MUX_IN1, 3)   \
   AD_MUX_INPUT(AD_DAB_DIAG_REF,    AD_MUX_IN1, 4)   \
   AD_MUX_INPUT(AD_DIAG_5V6,        AD_MUX_IN1, 5)   \
   AD_MUX_INPUT(AD_DIAG_SDARS_ANT,  AD_MUX_IN1, 6)   \
   AD_MUX_INPUT(AD_AUX_R,           AD_MUX_IN1, 7)   \
   AD_MUX_INPUT(AD_CDX_DIAG,        AD_MUX_IN2, 0)   \
   AD_MUX_INPUT(AD_FET_DIAG,        AD_MUX_IN2, 1)   \
   AD_MUX_INPUT(AD_ANT1_DIAG,       AD_MUX_IN2, 2)   \
   AD_MUX_INPUT(AD_ANT2_DIAG,       AD_MUX_IN2, 3)   \
   AD_MUX_INPUT(AD_DAB_DIAG,        AD_MUX_IN2, 4)   \
   AD_MUX_INPUT(AD_TFT_TEMP1,       AD_MUX_IN2, 5)   \
   AD_MUX_INPUT(AD_FBATT_SW_SC,     AD_MUX_IN2, 6)   \
   AD_MUX_INPUT(AD_AUX_GND,         AD_MUX_IN2, 7)

/* X-macro to define enumerated IDs for muxed A/D channels */
#   undef AD_MUX_INPUT
#   define AD_MUX_INPUT(channel_name, ad_chan, mux_source)     channel_name,

/**
 * List of available A/D Channels
 * @Note Any user choosen name may be used 
 * @Note Order is important first entry is physical A/D channel 0 (AN0), 2nd is AN1 ....  
 * @Note Last should come any muxed inputs 
 */
typedef enum AD_Channel_Tag
{
   AD_KEYB_1,        /**< AN0 */
   AD_KEYB_2,        /**< AN1 */
   AD_KEYB_3,        /**< AN2 */
   AD_LED_BIN,       /**< AN3 */
   AD_PHOTOTRANS,    /**< AN4 */
   AD_BATT_MEAS,     /**< AN5 */
   AD_MUX_IN1,       /**< AN6 */
   AD_MUX_IN2,       /**< AN7 */
   AD_MUX_INPUTS     /**< Additional muxed channels (do not add if no muxed channels) */
   AD_NUM_CHANNELS    
} AD_Channel_T;

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 17-Jul-2007 Dan Carman
 * + Modified to support muxed inputs 
 * + Used example channel names (from VW3)
 *
 * 14-Feb-2007 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* AD_ACFG_H */
