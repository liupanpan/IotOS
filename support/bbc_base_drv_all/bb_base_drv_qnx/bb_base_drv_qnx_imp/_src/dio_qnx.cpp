/*===========================================================================*/
/**
 * @file dio_qnx.c
 *
 * @brief Discrete I/O wrapper for qnx GPIO 
 *
 * %full_filespec: dio_qnx.cpp~3:ascii:kok_inf#1 %
 * @version %version: 3 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Sat May 16 18:17:40 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   Since QNX does not have uniform way to access gpio.  This file will include target specific
 *   Implementation files.
 *
 * @section ABBR ABBREVIATIONS:
 *   - DIO - Discrete I/O (standard binary (high / low) input / output pin)
 *   - GPIO - General Purpose I/O another common name for DIO
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * include Files
 \*===========================================================================*/
#include "pbc_trace.h"
#include "reuse.h"
#include "dio.h"


/* Select appropriate H/W module based on top level make define of
 * $(_build_)_CPU
 */
#if defined(_MICRO_J6_) || defined(_MICRO_J6Eco_)
#include "gpio_j6.cpp"
#elif defined(_MICRO_IMX6_)
#include "gpio_imx6.cpp"
#else

/* NO MICRO specified - default to a do nothing implementation */

/*===========================================================================*\
 * Local Preprocessor #define MACROS
 \*===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 0);


/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

/*
 * Initialize DIO module
 */
bool_t DIO_Initialize(void)
{
   Tr_Fault_1("%s: No Micro / GPIO implementation defined", __FUNCTION__);
   return true;
}

void DIO_Shutdown(void)
{
   Tr_Fault_1("%s: No  Micro / GPIO implementation defined", __FUNCTION__);
}

/*
 * Read DIO input
 */
bool_t DIO_Is_High(DIO_Port_T port, uint8_t pin)
{
   Tr_Fault_1("%s: No  Micro /GPIO implementation defined", __FUNCTION__);
   return false;
}

/*
 * Sets DIO output
 */
void DIO_Set_High(DIO_Port_T port, uint8_t pin, bool_t high)
{
   Tr_Fault_1("%s: No  Micro / GPIO implementation defined", __FUNCTION__);
}

/*
 * Set Direction for DIO pin
 */
void DIO_Set_Direction(DIO_Port_T port, uint8_t pin, DIO_Direction_T direction)
{
   Tr_Fault_1("%s: No  Micro / GPIO implementation defined", __FUNCTION__);
}

#endif

/*===========================================================================*/
/*!
 * @file x dio_qnx.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 5-Sept-2014 Dan Carman
 *   Task 11694 - Update QNX base drivers to work with J6 and IMX6
 *   - Initial version with support for IMX6 and J6 micros
 *
 */
/*===========================================================================*/
