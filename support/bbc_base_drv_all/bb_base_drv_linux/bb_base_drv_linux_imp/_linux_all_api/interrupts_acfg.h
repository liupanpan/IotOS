#ifndef INTERRUPTS_ACFG_H
#   define INTERRUPTS_ACFG_H
/*===========================================================================*/
/**
 * @file interrupts_acfg.h
 *
 * %full_filespec: interrupts_acfg.h~2:incl:kok_basa#12 %
 * @version %version: 2 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 13:21:59 2010 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2006 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * @brief Micro specific (WIN32) types for Standard Interrupts APIs
 *
 * DESCRIPTION :
 *    This file defines the micro specific interrupts types for SH2A
 *       - Interrupt_ID_T          - Interrupt source
 *       - Interrupt_Config_T      - interrupt configuration information (priority, etc)
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
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/* SH2A Interrupt Priority limits */
#   define  INT_PRI_MIN 0x0
#   define  INT_PRI_MAX 0xF

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * Interrupt Mode
 */
typedef enum Interrupt_Mode_Tag
{
   INT_MODE_DISABLE,
   INT_MODE_ENABLE,
   INT_MODE_EDGE_FALL,
   INT_MODE_EDGE_RISE,
   INT_MODE_EDGE_BOTH,
   INT_MODE_PULSE,

   INT_MODE_NUM_VALUES /* Number of different Interrupt mode */
} Interrupt_Mode_T;

/**
 * Interrupt Configuration parameters
 *    - Priority
 *    - Mode
 *    - Interrupt Service Routine Address
 */
typedef struct Interrupt_Config_Tag
{
   uint8_t priority;
   Interrupt_Mode_T mode;
   void *isr_addr;
} Interrupt_Config_T;

/**
 * IDs for all GPIO interrupt sources
 */
typedef enum Interrupt_ID_Tag
{
   INT_ID_0,
   INT_ID_1,
   INT_ID_2,
   INT_ID_3,
   INT_ID_4,
   INT_ID_5,
   INT_ID_6,
   INT_ID_7,
   INT_ID_8,
   INT_ID_9,

   INT_ID_10,
   INT_ID_11,
   INT_ID_12,
   INT_ID_13,
   INT_ID_14,
   INT_ID_15,
   INT_ID_16,
   INT_ID_17,
   INT_ID_18,

   INT_ID_19,
   INT_ID_20,
   INT_ID_21,
   INT_ID_22,
   INT_ID_23,
   INT_ID_24,
   INT_ID_25,
   INT_ID_26,
   INT_ID_27,

   INT_ID_28,
   INT_ID_29,
   INT_ID_30,
   INT_ID_31,
   INT_ID_DUMMY
} Interrupt_ID_T;

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc)
 * ----------- --------
 *
 * 20-aug-2010 Kris Boultbee
 * + Created initial file for Linux Host.
 *
\*===========================================================================*/
#endif /* INTERRUPTS_ACFG_H */
