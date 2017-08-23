#ifndef INTERRUPTS_H
#   define INTERRUPTS_H
/*===========================================================================*/
/** 
 * @file interrupts.h
 *
 * Standard Interrupts APIs
 *
 * %full_filespec: interrupts.h~4:incl:kok_aud#5 %
 * @version %version: 4 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Fri Sep 28 13:18:34 2007 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2006 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    This file has API definitions for global and individual interrupt masking,
 *
  * ABBREVIATIONS:
 *   - @todo Llist any abbreviations, precede each with a dash ('-').
 *
 * TRACEABILITY INFO:
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
 * DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @defgroup global_intr_grp Global Interrupts
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Standard Header Files
\*===========================================================================*/
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

/**
 * Global Interrupt State
 *    - 0 interrupts are enabled.
 *    - > 0 Interrupts disabled, current nesting level.
 */
typedef uint8_t Interrupt_State_T;


/*===========================================================================*\
 * Other Header Files
\*===========================================================================*/

#   include "interrupts_acfg.h"

/*===========================================================================*\
 * Exported Object Declarations
\*===========================================================================*/

/* 
 * Only constant objects should be exported. Delete this 
 */

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/*---------------------------------------------------------------------------*\
 * Initialization Functions
\*---------------------------------------------------------------------------*/

/**
 * Initializes interrupt controller and disables all individual interrupts.
 *
 * @post
 *   - Interrupt control module initialized.
 *   - Global interrupts disabled.
 *   - Individual interrupts masked?? 
 *
 * Used at processor startup to intialize interrupt sub-system.
 *
 */
extern void Interrupt_Init(void);

/*---------------------------------------------------------------------------*\
 * Global Interrupt Functions
\*---------------------------------------------------------------------------*/


/**
 * Enables global interrupts.
 *
 * @post
 *    - Global interrupts enabled.
 *    - Disable interrupt nesting is reset to 0.
 *
 * @todo Do we need to worry about controlling level of enabled interrupts (e.g., FIRQ vs IRQ on ARM).
 *
 * @see Interrupt_Disable_All, Interrupt_Restore.
 *
 */
extern void Interrupt_Enable_All(void);


/**
 * Disables global interrupts.
 *
 * @post
 *    - Global Interrupts disabled.
 *    - Increments disable interrupt nesting level.
 *
 * @see Interrupt_Restore, Interrupt_Enable_All.
 *
 */
extern void Interrupt_Disable_All(void);


/**
 * Cancels a disable interrupts.
 *
 * @post
 *    - if disable interrupt nesting level is 1 or 0
 *       - Enable interrupts.
 *       - Set disable nesting level to 0.
 *    - else
 *       - decrement disable nesting level (leaving interrupts disabled).
 *
 * Decrements disable interrupt nesting level. Re-enables interrupts when level zero reached.
 * Used in pairs with Disable_Interrupts.
 *
 * @see Interrupt_Disable_All, Interrupt_Enable_All.
 *
 */
extern void Interrupt_Restore(void);


/**
 * Returns state of global interrupts.
 *
 * @returns 
 *    Current disable interrupt state.
 *       - 0 interrupts are enabled.
 *       - >0 disable interrupt nesting level.
 *
 */
extern Interrupt_State_T Interrupt_State(void);

/*---------------------------------------------------------------------------*\
 * Individual Interrupt Functions
\*---------------------------------------------------------------------------*/


/**
 * Unmask (allows) interrupt.
 *
 * @param id
 *   ID of interrupt source.
 *
 * @pre
 *   id must be valid interrupt source.
 *
 * @post
 *   Interrupt source is unmasked.
 *
 * Clears interrupt mask. Shall not affect the state of pending interrupt flag. 
 *
 */
extern void Interrupt_Enable(Interrupt_ID_T id);


/**
 * Masks (blocks) specfic interrupt.
 *
 * @param id
 *   ID of interrupt source.
 *
 * @pre
 *   id must be valid interrupt source.
 *
 * @post
 *   Interrupt source is masked.
 *
 * Sets interrupt mask. Shall not affect the state of pending interrupt flag.
 *
 */
extern void Interrupt_Disable(Interrupt_ID_T id);


/**
 * Clears pending interrupt flag.
 *
 * @param id
 *   ID of interrupt source.
 *
 * @pre
 *   id must be valid interrupt source.
 *
 * @post
 *   Pending interrupt flag shall be cleared (i.e., not pending).
 *
 */
extern void Interrupt_Clear(Interrupt_ID_T id);


/**
 * Returns state of pending interrupt flag.
 *
 * @returns 
 *    true - if pending interrupt.
 *
 * @param id
 *   ID of interrupt source.
 *
 * @pre
 *   id must be valid interrupt source.
 *
 */
extern bool Interrupt_Flag(Interrupt_ID_T id);


/**
 * Sets configuration for interrupt (priority, type, callback, etc).
 *
 * @param id
 *   ID of interrupt source.
 *
 * @param config
 *   application and processor specific configuration data.
 *
 * This function sets the configuration for a specific interrupt source.
 * At minimum it will include priority,
 * but it may include additional processor specific data.
 *
 */
extern void Interrupt_Configure(Interrupt_ID_T id, const Interrupt_Config_T * config);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

/** @} */ /* end of interrupt_grp */

/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc)
 * ----------- --------
 * 2007-sep-28 kirk bailey
 * + Fixed header Doxygen comment.
 *
 * 2007-03-07  LKO
 * + Moved Interrupt_Mode and Interrupt_Config definitions to interrupts_acfg.h.
 *
 * 2007-03-02  LKO
 * + Added gpio_intr_grp.
 *
 * 2006-10-26  DDC 
 * + Created initial file.
 *
\*===========================================================================*/
#endif /* INTERRUPTS_H */
