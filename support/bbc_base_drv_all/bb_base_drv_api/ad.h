#ifndef AD_H
#   define AD_H
/*===========================================================================*/
/**
 * @file ad.h
 *
 * This is standard simple interface for an A/D Driver
 *
 * %full_filespec: ad.h~kok_basa#5:incl:kok_aud#4 %
 * @version %version: kok_basa#5 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 08:19:25 2010 %
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
 *   This API provides a simple immediate A/D ratiometric read.
 *    Use a different API if you need access to high end A/D functionality
 *    such as hardware trigger, multi-chan scan, continous conversions, etc.
 *
 * @section ABBR ABBREVIATIONS:
 *   - A/D Analog to Digital
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
 * @defgroup ad_grp Simple A/D Driver interface
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "reuse.h"

/*
 * Defines Device specific characteristics
 *    - List of A/D channel
 *    - Reference voltage in mV
 */
#   include "ad_acfg.h"

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
 * Reads an single A/D channel
 *
 *  @param [in] channel channel to read
 *
 *  @param [out] counts A/D counts left justified (range 0 - 0xFFFF)
 *
 *  @return true if read was successful
 */
bool_t AD_Read_Channel(AD_Channel_T channel, uint16_t * counts);

/**
 * Reads an A/D Channel relative to another A/D channel
 *
 * @param [in] chan - Identifies which A/D Channel to read
 *
 * @param [in] ref_chan - Identifies A/D reference channel.
 *
 * @param [out] ratio - Ratio of two voltages - ((chan * 0x10000) / ref_chan)
 *       0xFFFF is returned if chan >= ref_chan
 *
 * @return true if read was successful
 */
bool_t AD_Read_Ratio(AD_Channel_T chan, AD_Channel_T ref_chan, uint16_t * ratio);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}         /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file ad.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 15-Nov-2010 Dan Carman
 *    + Updated for Linux
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* AD_H */
