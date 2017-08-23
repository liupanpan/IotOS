#ifndef IIC_H
#   define IIC_H
/*===========================================================================*/
/**
 * @file iic.h
 *
 * This is interface for IIC bus
 *
 * %full_filespec: iic.h~kok_basa#5:incl:kok_aud#16 %
 * @version %version: kok_basa#5 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 08:19:33 2010 %
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
#   include "iic_acfg.h"
#   include "reuse.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*---------------------------------------------------------------------
 * Control byte constants
 * ------------------------------------------------------------------*/

/** IIC control parameter requesting Start at beginning of master operation */
#   define IIC_START             0x80
/** IIC control parameter requesting a Stop at end of master operation */
#   define IIC_STOP              0x40
/** IIC control parameter mask for number of retries */
#   define IIC_RETRY_MASK        0x0F

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/** IIC master control flags
 *  bits 0-3 number of retries if request fails
 *  bit 4-5 unused
 *  bit 6 Generate stop
 *  bit 7 Generate start
 */
typedef uint8_t IIC_Control_T;

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/
#   ifdef __cplusplus
extern "C"
{          /* ! Place all function prototypes inside these braces ! */
#   endif  /* __cplusplus */

/*---------------------------------------------------------------------------*\
 * IIC bus ownership functions
\*---------------------------------------------------------------------------*/

/**
 * Locks and configures IIC channel for use by thread
 *
 * @param [in] channel - Identifies IIC channel
 * @param [in] speed_kHz   - Requested bus speed in kHz (typically 100 or 400)
 * @param [in] timeout_msec - Maximum time in mS to wait for channel
 *
 * @return true if channel access granted
 *
 * Allocates sole access to channel to requesting thread and configures the bus
 * for indicated bus speed.  The driver shall use the highest possible speed
 * equal or below the requested speed.
 *
 * @code
 *  Typical usage:
 *
 *    if (IIC_Lock(chan))
 *    {
 *       ...
 *       IIC_Write(chan, dev, IIC_START | IIC_STOP, 5, my_data, 100);
 *       ...
 *       IIC_Unlock(chan);
 *    }
 *
 * @endcode
 *
 * @see IIC_Unlock
 */
bool_t IIC_Lock(IIC_Channel_T channel, uint16_t speed_kHz, uint32_t timeout_msec);

/**
 * Releases IIC channel for use by other threads
 *
 * @param [in] channel - Identifies IIC channel
 *
 * @see IIC_Lock
 *
 * @note Must be called after successful Lock to allow other threads access
 */
void IIC_Unlock(IIC_Channel_T channel);

/*---------------------------------------------------------------------------*\
 * IIC bus master functions
\*---------------------------------------------------------------------------*/

/**
 * IIC Master transmit
 *
 * @param [in] channel - Identifies IIC channel
 * @param [in] device - Device address (left shifted)
 * @param [in] control - message control
 * @param [in] wdata - pointer to bytes to transmit
 * @param [in] num_bytes - number of data bytes
 * @param [in] timeout_msec - Maximum time to transmit data
 *
 * @return true if write is successful
 *
 * @note Channel must be locked by thread before usage
 *
 * Unless the control indicates a restart, the driver will do multiple attempts
 * to send the data as IIC master.
 *
 */
bool_t IIC_Write(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, const uint8_t * wdata, size_t num_bytes,
               uint32_t timeout_msec);

/**
 * IIC Master Recieve
 *
 * @param [in] channel - Identifies IIC channel
 * @param [in] device - Device address (left shifted)
 * @param [in] control - message control
 * @param [out] rdata - Pointer to buffer to store data bytes
 * @param [in] num_bytes - Number of data bytes
 * @param [in] timeout_msec - Maximum time to read data
 *
 * @return true if read is successful
 *
 * @note Channel must be locked by thread before usage
 *
 * Unless the control indicates a restart, the driver will do multiple attempts
 * to read the data as IIC master.
 *
 */
bool_t IIC_Read(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, uint8_t * rdata, size_t num_bytes,
              uint32_t timeout_msec);

/*---------------------------------------------------------------------------*\
 * IIC module functions
\*---------------------------------------------------------------------------*/

/**
 * Initializes IIC module
 *
 * @note Must be done before first usage.
 */
void IIC_Initialize(void);

/**
 * Disables all IIC channel.
 *
 * @note Must call IIC_Initialize before IIC channels can be used again.
 *
 *  All active IIC channels are disabled and placed into low current state
 */
void IIC_Shutdown(void);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}         /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file iic.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 15-Nov-2010 Dan Carman
 *    + Port API for Linux - Removed IIC Slave interface. 
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* iic_H */
