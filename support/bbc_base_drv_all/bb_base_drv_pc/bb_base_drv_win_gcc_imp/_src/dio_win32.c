/*===========================================================================*/
/**
 * @file dio_win32.c
 *
 * @brief Discrete I/O wrapper
 *
 * %full_filespec: dio_win32.c~kok_basa#4:csrc:kok_aud#1 %
 * @version %version: kok_basa#4 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 12:16:39 2010 %
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
 *   This module provides a dummy implementation of GPIO wrapper
 *
 * @section ABBR ABBREVIATIONS:
 *   - DIO - Discrete I/O (standard binary (high / low) input / output pin)
 *   - GPIO - General Purpose I/O another common name for DIO
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
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "dio.h"
#include "pbc_trace.h"
#include "xsal.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/

#define NUM_PORTS    7

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 0);

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/** Simulated Port values 
 */
uint16_t GPIO_Ports[NUM_PORTS];

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/** initialization flag
 */
static bool dio_initialized = false;

/** Sychronization for AD Values
 */
static SAL_Semaphore_T dio_sem;

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

static void dio_initialize(void);
static void dio_Lock(void);
static void dio_Unlock(void);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/*
 * Initialize DIO module
 */
bool DIO_Initialize(void)
{
   return true;
}

/*
 * Read DIO input
 *    Because port A is either A/D inputs or discrete input on port wide
 *       basis, use special read to allow "mixed" operation.
 */
bool DIO_Is_High(DIO_Port_T port, uint8_t pin)
{
   bool state;


   dio_Lock();

   state = (0 != (GPIO_Ports[port] & (1U << pin)));

   Tr_Info_Mid_3("GPIO Port %d Pin %d : %d", port, pin, state);
   
   dio_Unlock();

   return state;
}

/*
 * Sets DIO output
 */
void DIO_Set_High(DIO_Port_T port, uint8_t pin, bool high)
{
   PBC_Require_2((port < NUM_PORTS) && (pin < 32), "Invalid port %d or pin %d", port, pin);

   dio_Lock();

   if (high)
   {
      GPIO_Ports[port] |= (1U << pin);
   }
   else
   {
      GPIO_Ports[port] &= ~(1U << pin);
   }

   Tr_Info_Mid_2("GPIO Port %d : %u", port, GPIO_Ports[port]);
   
   dio_Unlock();
}

/*
 * Set Direction for DIO pin
 */
void DIO_Set_Direction(DIO_Port_T port, uint8_t pin, DIO_Direction_T direction)
{
   PBC_Require_2((port < NUM_PORTS) && (pin < 32), "Invalid port %d or pin %d", port, pin);
}

static void dio_initialize(void)
{
   SAL_Semaphore_Attr_T sem_attr;

   SAL_Init_Semaphore_Attr(&sem_attr);
   sem_attr.initial_value = 1;

   SAL_Create_Semaphore(&dio_sem, &sem_attr);

   dio_initialized = true;
}

static void dio_Lock(void)
{
   if (!dio_initialized)
   {
      dio_initialize();
   }

   if (!SAL_Wait_Semaphore_Timeout(&dio_sem, 1000))
   {
      PBC_Failed("Semaphore failed to lock in function dio_Lock().");
   }
}

static void dio_Unlock(void)
{
   if (!SAL_Signal_Semaphore(&dio_sem))
   {
      PBC_Failed("Semaphore failed to lock in function dio_Unlock().");
   }
}

/*===========================================================================*/
/*!
 * @file dio_win32.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 * 18 Apr 2007 Dan Carman
 * + Update for New DbC macros
 */
/*===========================================================================*/
