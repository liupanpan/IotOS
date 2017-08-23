#ifndef I2C_QNX_H
#   define I2C_QNX_H
/*===========================================================================*/
/**
 * @file i2c_qnx.h
 *
 * This is interface for IIC bus
 *
 * %full_filespec: i2c_qnx.h~1:incl:kok_inf#1 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon Apr 14 14:11:40 2014 %
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
 * Standard interface for IIC operations
 *
 * @section ABBR ABBREVIATIONS:
 *   - IIC - Philips I2C (Inter-IC) bus
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *         SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - <a href="http://www.esacademy.com/faq/i2c/I2C_Bus_specification.pdf">
 *          THE I2C-BUS SPECIFICATION VERSION 2.1 JANUARY 2000 Philips Semiconductors</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *
 * @defgroup iic  IIC Device Driver Interface
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/
#include "reuse.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
 \*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
 \*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
 \*===========================================================================*/

#ifndef _I2C_LIB_H_INCLUDED

typedef enum
{
   I2C_ADDRFMT_10BIT = 0x0001,
   I2C_ADDRFMT_7BIT = 0x0002,
} i2c_addrfmt_t;

#endif

/*===========================================================================*\
 * Exported Const Object Declarations
 \*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
 \*===========================================================================*/
#ifdef __cplusplus
extern "C"
{ /* ! Place all function prototypes inside these braces ! */
#   endif  /* __cplusplus */

/*===========================================================================*/
/**
 * @fn    I2C_Open
 * @brief The I2C_Open function initializes the master interface.
 *
 * @param I2C_Handle The handle needed by all other I2C functions.
 *
 * The function returns a handle that's passed to all other functions, or NULL if an error occurred
 *
 */
/*===========================================================================*/
int32_t I2C_Open(int32_t *I2C_Handle, char const * device);

/*===========================================================================*/
/**
 * @fn    I2C_Set_Busspeed
 * @brief Will be called just before the scheduler is called.
 *
 * @param I2C_Handle The handle returned by the I2C_Open function.
 * @param bus_Speed  The I2C bus speed (I2C_SPEED_STANDARD, I2C_SPEED_FAST, I2C_SPEED_HIGH)
 *
 * If an invalid bus speed is requested, this function should return a failure and leave the bus speed unchanged
 */
/*===========================================================================*/
int32_t I2C_Set_Busspeed(int32_t I2C_Handle, uint32_t bus_Speed);

int32_t I2C_Lock(int32_t I2C_Handle);

int32_t I2C_UnLock(int32_t I2C_Handle);

int32_t I2C_Write(int32_t I2C_Handle, uint32_t Slave_Addr, uint32_t Format, const uint8_t *Buffer, int32_t Bytes, uint32_t Stop);

int32_t I2C_Send_Receive(int32_t I2C_Handle, uint32_t Slave_Addr, uint32_t Format, uint8_t const *Tx_Buffer, int32_t Tx_Bytes,
   uint8_t *Rx_Buffer, int32_t Rx_Bytes);

int32_t I2C_Read(int32_t I2C_Handle, uint32_t Slave_Addr, uint32_t Format, uint8_t *Buffer, int32_t Bytes,  uint32_t Stop);

int32_t I2C_Close(int32_t *I2C_Handle);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
 \*===========================================================================*/

#   ifdef __cplusplus
} /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file i2c_qnx.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 26-Mar-2014 Dan Carman
 *    + Port API for qnx - Removed IIC Slave interface. 
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* iic_H */
