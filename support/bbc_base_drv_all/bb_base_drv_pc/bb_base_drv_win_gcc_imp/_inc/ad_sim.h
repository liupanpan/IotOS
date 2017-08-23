#ifndef AD_SIM_H
#   define AD_SIM_H
/*===========================================================================*/
/**
 * @file ad_sim.h
 *
 * @brief Internal routines for A/D driver on SH2A
 *
 * %full_filespec: ad_sim.h-1:incl:kok_aud#1 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon May  7 07:15:09 2007 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    Allows discrete I/O reads from port A which is shared with A/D pins
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
#   include "reuse.h"
#   include "ad.h"

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/**
 * Friend function to adjust simulated A/D values
 *
 *  @param [in] channel channel to set 
 *
 *  @param [in] counts A/D counts left justified (range 0 - 0xFFFF)
 */
void AD_Set_Counts(AD_Channel_T channel, uint16_t counts);


/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 5-Mar-2007 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* AD_SIM_H */
