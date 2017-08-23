#ifndef GPIO_INTERRUPTS_H
#   define GPIO_INTERRUPTS_H
/*===========================================================================*/
/** 
 * @file gpio_interrupts.h
 *
 * Standard API for controling GPIO interrupts
 *
 * %full_filespec: gpio_interrupts.h~3:incl:kok_aud#4 %
 * @version %version: 3 %
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
 *    This file has API definitions for using GPIO interrrupts
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
 * @defgroup gpio_intr_grp GPIO Interrupts
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

/*===========================================================================*\
 * Other Header Files
\*===========================================================================*/

#   include "interrupts.h"
#   include "gpio_interrupts_acfg.h"

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
 * GPIO Interrupt Functions
\*---------------------------------------------------------------------------*/

/*===========================================================================*/
/**
 * GPIO Interrupt Initialization.
 *
 * @post
 *    - Disable all GPIO Interrupts.
 *    - Clear all GPIO Interrupt Priorities and Modes. 
 *
 * Used at processor startup to intialize the GPIO interrupt sub-system.
 */
/*===========================================================================*/
extern void GPIO_Interrupt_Initialize(void);

/*===========================================================================*/
/**
 * Configures a GPIO Pin for interrupt.
 *
 * @param pin_id
 *    Identifies an unique GPIO interrupt.
 *
 * @param config
 *    - Micro specific GPIO configuration. 
 *    - Interrrupt Mode (none, rise, fall, both, low, or high).
 *    - Priority and/or other micro specific information.
 *
 * @pre
 *   - pin_id must be a valid GPIO reference.
 *   - config must have valid parameters.
 *
 * @post
 *    - Configures pin_id for interrupt operation. 
 *    - Set desired mode.
 *    - Set Priority and/or other micro specific information.
 *    - Configures underlying interrupt vector.
 *
 * Configure will not enable the interrupt. 
 *
 */
/*===========================================================================*/
extern void GPIO_Interrupt_Configure(GPIO_Interrupt_ID_T pin_id, const Interrupt_Config_T * config);

/*===========================================================================*/
/**
 * Enables (unmasks) GPIO interrupt.
 *
 * @param pin_id
 *   Identifies an unique GPIO interrupt.
 *
 * @pre
 *   - pin_id must be a valid GPIO reference.
 *   - that has been configured as a GPIO interrupt.
 *
 * @post
 *   Interrupt enabled.
 *
 */
/*===========================================================================*/
extern void GPIO_Interrupt_Enable(GPIO_Interrupt_ID_T pin_id);

/*===========================================================================*/
/**
 * Disables (masks) GPIO interrupt.
 *
 * @param pin_id
 *   Identifies an unique GPIO interrupt.
 *
 * @pre
 *   - pin_id must be a valid GPIO reference.
 *   - that has been configured as a GPIO interrupt.
 *
 * @post
 *   Interrupt disabled.
 *
 */
/*===========================================================================*/
extern void GPIO_Interrupt_Disable(GPIO_Interrupt_ID_T pin_id);

/*===========================================================================*/
/**
 * Clears pending GPIO interrupt flag.
 *
 * @param pin_id
 *   Identifies an unique GPIO interrupt.
 *
 * @pre
 *   - pin_id must be a valid GPIO reference.
 *   - that has been configured as a GPIO interrupt.
 *
 * @post
 *   Pending interrupt flag shall be cleared (i.e., made not pending).
 *
 */
/*===========================================================================*/
extern void GPIO_Interrupt_Clear(GPIO_Interrupt_ID_T pin_id);

/*===========================================================================*/
/**
 * Returns state of pending GPIO interrupt flag.
 *
 * @returns 
 *    true - if pending interrupt.
 *
 * @param pin_id
 *   Identifies an unique GPIO interrupt.
 *
 * @pre
 *   - pin_id must be a valid GPIO reference.
 *   - that has been configured as an GPIO interrupt.
 *
 */
/*===========================================================================*/
extern bool GPIO_Interrupt_Flag(GPIO_Interrupt_ID_T pin_id);

/*===========================================================================*/
/**
 * Returns current input state of GPIO interrupt pin.
 *
 * @returns 
 *    - true, if pin_id is logic high.
 *    - false, if pin_id is logic low.
 *
 * @param pin_id
 *   Identifies an unique GPIO interrupt.
 *
 * @pre
 *   - pin_id must be a valid GPIO reference.
 *   - that has been configured as an GPIO interrupt.
 *
 * Shall not affect the setting of the GPIO interrupt detection. 
 *
 */
/*===========================================================================*/
extern bool GPIO_Interrupt_Pin(GPIO_Interrupt_ID_T pin_id);

/*===========================================================================*/
/**
 * Returns currently configured interrupt GPIOs.
 *
 * @returns 
 *    The mode the GPIO interrupt is configured for.
 *
  * @param pin_id
 *    Identifies an unique GPIO interrupt.
 *
 * @pre
 *   - pin_id must be a valid GPIO reference.
 *   - that has been configured as an GPIO interrupt.
 *
 */
/*===========================================================================*/
extern Interrupt_Mode_T GPIO_Interrupt_Mode(GPIO_Interrupt_ID_T pin_id);

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
 * 2007-03-02  LKO
 * + Added gpio_intr_grp.
 *
 * 2007-01-31  lko-kok
 * + Created initial file.
 *
\*===========================================================================*/
#endif /* GPIO_INTERRUPTS_H */
