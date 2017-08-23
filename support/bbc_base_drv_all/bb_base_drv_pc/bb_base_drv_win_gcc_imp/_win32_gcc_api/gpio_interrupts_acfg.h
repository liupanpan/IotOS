#ifndef GPIO_INTERRUPTS_ACFG_H
#   define GPIO_INTERRUPTS_ACFG_H
/*===========================================================================*/
/**
 * @file gpio_interrupts_acfg.h
 *
 * %full_filespec: gpio_interrupts_acfg.h-1:incl:kok_aud#6 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon May  7 07:15:23 2007 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2006 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * @brief Micro specific WIIN32 types for Standard GPIO Interrupts APIs
 *
 * DESCRIPTION :
 *    This file defines the micro specific interrupts types for WIN32
 *
 * ABBREVIATIONS:
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     - REJ09B0290-0050 Renesas SH7263 Group Hardware Manual
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *
 * DEVIATIONS FROM STANDARDS:
 *    - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Standard Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Other Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * List of potential GPIO interrupts
 */
typedef enum GPIO_Interrupt_ID_Tag
{
   GPIO_DUMMY_IRQ,                   /**< Dummy placeholder */
   GPIO_INT_ID_NUM_VALUES     /* Total number of Pins that can be set as GPIO Interrupts */
} GPIO_Interrupt_ID_T;

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc)
 * ----------- --------
 * 2007-05-01  Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
#endif /* GPIO_INTERRUPTS_ACFG_H */
