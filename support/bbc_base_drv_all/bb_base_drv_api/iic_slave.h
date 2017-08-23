#ifndef IIC_SLAVE_H
#   define IIC_SLAVE_H
/*===========================================================================*/
/**
 * @file iic_slave.h
 *
 * Standard interface for IIC slave operations
 *
 * %full_filespec:iic_slave.h~3:incl:kok_basa#3 %
 * @version %version:3 %
 * @author  %derived_by:fzjt2c %
 * @date    %date_modified:Mon Jul 26 13:36:58 2010 %
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
 * Standard interface for IIC slave operations
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
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Enable IIC Slave operation on a given iIC channel
 *
 * @param chan_num IIC HW Channel Number
 * @param slave_addr Left-Justified address this node is to respond to
 *
 * @return 0 is channel was enabled, otherwise error 
 */
   int32_t IIC_SLV_Enable(IIC_Channel_T chan_num, uint8_t slave_addr);

/**
 * Disable IIC slave operation for channel
 *
 * @param chan_num IIC HW Channel Number
 */
   void IIC_SLV_Disable(IIC_Channel_T chan_num);

/**
 * Set up new transmit data to be read by master
 *
 * @param chan_num IIC HW Channel Number
 * @param transmit_data Data to be buffer until Master reads it
 * @param num_bytes - size of transmit data 
 *
 * @return 0 is no error 
 */
   int32_t IIC_SLV_Transmit(IIC_Channel_T chan_num, const uint8_t * transmit_data, size_t num_bytes);

/**
 * Report if last trasnmit message has been read by master
 *
 * @param chan_num IIC HW Channel Number
 *
 * @return true is data was read and transmitter is free for new message
 */
   bool_t IIC_SLV_Transmit_Available(IIC_Channel_T chan_num);

/**
 * Allow new transmit message to be sent without requiring last message to have been read
 *
 * @param chan_num IIC HW Channel Number
 */
   void IIC_SLV_Transmit_Reset(IIC_Channel_T chan_num);

#   ifdef __cplusplus
}                               /* extern "C" - function prototypes must precede this brace */
#   endif                       /* __cplusplus */

/*===========================================================================*/
/*!
 * @file iic_slave.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 *  - 26-July-2010 Jaime Almeida
 *    + SCR kok_basa#2373/Task kok_basa#12696: Replace bool with bool_t.
 *
 * - 25-Apr-2010 Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* IIC_SLAVE_H */
