#ifndef IIC_SLAVE_CBK_H
#   define IIC_SLAVE_CBK_H
/*===========================================================================*/
/**
 * @file iic_slave_cbk.h
 *
 * Callouts for IIC Slave Wrapper
 *
 * %full_filespec:iic_slave_cbk.h~6:incl:kok_basa#1 %
 * @version %version:6 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Mon Oct 17 12:36:15 2011 %
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
 * Provides a simple 3-wire Master IIC_SLAVE interface
 *
 * @section ABBR ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
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
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @defgroup iic_slave Basic IIC_SLAVE Interface
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

#   include "iic.h"
#   include "reuse.h"
#   include "xsal.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/
#   ifdef __cplusplus
extern "C"
{                               /* ! Place all function prototypes inside these braces ! */
#   endif                       /* __cplusplus */

/*===========================================================================*
 * Imported Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Gets thread attributes for IIC Slave Receiver Thread (Linux)
 *
 * @param thread_attr - Attributes for IIC Slave Monitor Thread
 */
   void IIC_SLV_Get_Send_Thread_Attr(SAL_Thread_Attr_T * thread_attr);


/**
 * Gets thread attributes for IIC Slave Transmitter Thread (Linux)
 *
 * @param thread_attr - Attributes for IIC Slave Monitor Thread
 */
   void IIC_SLV_Get_Receive_Thread_Attr(SAL_Thread_Attr_T * thread_attr);

/**
 * User function for newly receive IIC Slave Read
 *
 * @param chan_num - Channel being read.
 * @param rx_msg - buffer containing data to be read.
 * @param rx_len - size of buffer.
 */
   void IIC_SLV_Receive(IIC_Channel_T chan_num, const uint8_t * rx_msg, uint32_t rx_len);

/**
 * Callback Function called every time the slave completes a transmit.
 *
 */
   void IIC_SLV_Transmit_Done(void);

/**
 * Gets thread attributes for IIC Slave Monitor Thread (Jacinto2 - uItron).
 *
 * @param thread_attr - Attributes for IIC Slave Monitor Thread
 */
   void IIC_SLV_Get_Thread_Attr(SAL_Thread_Attr_T * thread_attr);

/**
 * Callback Function called every time the slave is addressed (Jacinto2 - uItron).
 *
 * @param *appSlvHandle - Slave Handle.  In this case the channel number.
 * @param event - In this case 0 = Master Write detected, 1 = Master Read detected
 * @param data - Not used.
 */
   void IIC_SLV_AAS_Callback(void *appSlvHandle, int8_t event, uint8_t data);

#   ifdef __cplusplus
}                               /* extern "C" - function prototypes must precede this brace */
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file iic_slave.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 25-Jan-2011 Jaime Almeida
 *   - Added function to get thread attributes for IIC Slave Monitor Thread
 *     which was removed in previous change.
 *   - Added Callback Function called every time the slave is addressed
 *     which was removed in the previous change.
 *   - Added additional comments for clarification purposes.
 *
 * - 19-Jan-2011 Antonio Fernandez
 *   - Added function to get thread attributes for IIC Slave Monitor Thread
 *   - Added Callback Function called every time the slave completes a transmit
 *   - Removed Callback Function called every time the slave is addressed
 *   - Rename function to Get thread attributes for IIC Slave Monitor Thread
 *
 * - 19-Jul-2010 Jaime Almeida
 *   - Added iic.h header file.
 *   - Added IIC_SLV_AAS_Callback declaration.
 *
 * - 25-Apr-2010 Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* IIC_SLAVE_H */
