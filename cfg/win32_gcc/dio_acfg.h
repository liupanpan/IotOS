#ifndef DIO_ACFG_H
#   define DIO_ACFG_H
/*===========================================================================*/
/**
 * @file dio_acfg.h
 *
 * @brief Device micro-specific Discrete I/O types
 *
 * %full_filespec: dio_acfg.h~kok_basa#2:incl:kok_aud#19 %
 * @version %version: kok_basa#2 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Tue Mar  2 12:30:45 2010 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    Micro specific Discrete I/O types. This file is configured for the Renesas SH7263
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
 * @addtogroup dio_grp
 * @{
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define X-MACROS 
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 *	Identifiers for Discrete I/O ports
 */
typedef enum DIO_Port_Tag
{
   GPIO_PORT_A,
   GPIO_PORT_B,
   GPIO_PORT_C,
   GPIO_PORT_D,
   GPIO_PORT_E,
   GPIO_PORT_F,
   GPIO_PORT_F_HI
} DIO_Port_T; 

/**
 *	DIO Direction Settings
 */
typedef enum DIO_Direction_Tag
{
   DIO_INPUT,                                   /**< pin is input  */
   DIO_OUTPUT,                                  /**< pin is standard output */
} DIO_Direction_T;

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 14-Feb-2007 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* DIO_ACFG_H */
