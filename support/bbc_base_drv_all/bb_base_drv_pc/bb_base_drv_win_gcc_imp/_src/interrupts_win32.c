/*===========================================================================*/
/**
 * @file interrupts_win32.c
 *
 * @brief Implementation of the Global Interrupts for WIN32
 *
 * %full_filespec: interrupts_win32.c-1:csrc:kok_aud#3 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon May  7 07:15:16 2007 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    Dummy Implementation of Global Interrupts for WIN32
 *
 * ABBREVIATIONS:
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *         SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - HI7000/4 Series (HI7000/4 V.2.02, HI7700/4 V.2.02, and HI7750/4 V.2.02) User’s Manual
 *
 * DEVIATIONS FROM STANDARDS:
 *   - 
 *
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "em.h"
#include "interrupts.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

EM_FILENUM(INTERRUPTS_MODULE_ID, 0);    /* define this file for assert handling */

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/
/**
 * Interrupt State Count
 *       - 0 interrupts are enabled
 *       - >0 disable interrupt nesting level        
 * Set at initialization 
 */
STATIC_VAR Interrupt_State_T Interrupt_State_Count;

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local CONST Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/


/*
 * Initializes interrupt controller and disables all individual interrupts
 *   - Interrupt control module initialized
 *   - Global interrupts disabled
 *   - Individual interrupts masked ?? 
 * Used at processor startup to intialize interrupt sub-system.
 */
void Interrupt_Init(void)
{
   Interrupt_State_Count = 0;
}


/*
 * Enables global interrupts
 *    - Global interrupts enabled
 *    - Disable interrupt nesting is reset to 0
 * Clears the global interrupt mask, allowing interrupts. Resets disable interrupt
 * nesting level to 0.
 * Todo Do we need to worry about controlling level of enabled interrupts (e.g., FIRQ vs IRQ on ARM).
 * See Interrupt_Disable_All, Interrupt_Restore
 */
void Interrupt_Enable_All(void)
{
   Interrupt_State_Count = 0;        /* Lock before changing Count */
}


/*
 * Disables global interrupts
 *    - Global Interrupts disabled
 *    - Increments disable interrupt nesting level
 * See Interrupt_Restore, Interrupt_Enable_All
 */
void Interrupt_Disable_All(void)
{
   Interrupt_State_Count++;
}


/*
 * Cancels a disable interrupts.
 *    - if disable interrupt nesting level is 1 or 0
 *       - Enable interrupts
 *       - Set disable nesting level to 0
 *    - else
 *       - decrement disable nesting level (leaving interrupts disabled)
 * Decrements disable interrupt nesting level. Re-enables interrupts when level zero reached.
 * Used in pairs with Disable_Interrupts
 * See Interrupt_Disable_All, Interrupt_Enable_All
 */
void Interrupt_Restore(void)
{
   Interrupt_State_Count--;     /* Lock before changing Count */

   if (Interrupt_State_Count <= 0)
   {
      Interrupt_State_Count = 0;
   }
}


/*
 * Returns state of global interrupts
 * Returns 
 *    Current disable interrupt state
 *       - -1 interrupts are disable due to ISR 
 *       - 0 interrupts are enabled
 *       - >0 disable interrupt nesting level        
 */
Interrupt_State_T Interrupt_State(void)
{
   return (Interrupt_State_Count);
}


/*
 * Unmask (allows) interrupt
 * Param id:   ID of interrupt source
 * Pre:        ID must be valid interrupt source
 * Post:       Interrupt source is unmasked 
 * Clears interrupt mask. Shall not affect the state of pending interrupt flag. 
 */
void Interrupt_Enable(Interrupt_ID_T id)
{
}


/*
 * Masks (blocks) specfic interrupt 
 * Param id:   ID of interrupt source
 * Pre:        ID must be valid interrupt source
 * Post:       Interrupt source is masked 
 * Sets interrupt mask. Shall not affect the state of pending interrupt flag. 
 */
void Interrupt_Disable(Interrupt_ID_T id)
{
}


/*
 * Clears pending interrupt flag
 * Param id:   ID of interrupt source
 * Pre:        ID must be valid interrupt source
 * Post:       Pending interrupt flag shall be cleared (i.e., not pending)
 */
void Interrupt_Clear(Interrupt_ID_T id)
{
}


/*
 * Returns state of pending interrupt flag
 * Returns :   true - if pending interrupt 
 * Param id:   ID of interrupt source
 * Pre:        ID must be valid interrupt source
 */
bool Interrupt_Flag(Interrupt_ID_T id)
{
   return (false);
}


/*
 * Sets configuration for interrupt (priority, type, callback, etc.)
 * Param id:   ID of interrupt source
 * Param config:  application and processor specific configuration data
 * Param id:   ID of interrupt source
 * This function sets the configuration for a specific interrupt source. At minimum 
 * it will include priority, but it may include additional processor specific data.
 */
void Interrupt_Configure(Interrupt_ID_T id, const Interrupt_Config_T * config)
{
}


/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 2007-04-30 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
