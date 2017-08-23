#ifndef INTERRUPTS_ACFG_H
#   define INTERRUPTS_ACFG_H
/*===========================================================================*/
/**
 * @file interrupts_acfg.h
 *
 * %full_filespec: interrupts_acfg.h-1:incl:kok_aud#7 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon May  7 07:15:25 2007 %
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
#define  INT_PRI_MIN 0x0
#define  INT_PRI_MAX 0xF

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * Interrupt Mode
 *  - For General Interrupts, usually fixed by the interrupt source.
 *  - For GPIO Interrupts, If Mode is undefined, maps to Disable.
 */
typedef enum Interrupt_Mode_Tag
{
   INT_DISABLE,
   INT_LEVEL_LOW,
   INT_LEVEL_HIGH,
   INT_EDGE_FALL,
   INT_EDGE_RISE,
   INT_EDGE_BOTH,
   INT_SPECIAL,
   INT_MODE_NUM_VALUES     /* Number of different Interrupt mode */
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
 * IDs for all interrupt sources
 */
typedef enum Interrupt_ID_Tag
{
   INTVECT_DUMMY 
} Interrupt_ID_T;


/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc)
 * ----------- --------
 *
 * 30-Apr-2007  Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
#endif /* INTERRUPTS_ACFG_H */
