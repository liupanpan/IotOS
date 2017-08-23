#ifndef IIC_SLAVE_CFG_H
#   define IIC_SLAVE_CFG_H
/*===========================================================================*/
/**
 * @file iic_slave_cfg.h
 *
 * Micro specific iic channel mapping for Linux
 *
 * %full_filespec:iic_slave_cfg.h~4:incl:kok_basa#1 %
 * @version %version:4 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Fri Oct 26 10:28:18 2012 %
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
 * Defines Micro specific iic public types namely the Available channels
 *    and handshake options
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
 * @addtogroup iic_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Preprocessor #define X-MACROS
\*===========================================================================*/


/**
 * Mapping of logical IIC Slave Channel to device driver name
 *
 */
#   define IIC_Slave_Define_Channels   \
   IIC_Slave_Define_Channel(IIC_CHANNEL_0, "/dev/slave-i2c-0") \
   IIC_Slave_Define_Channel(IIC_CHANNEL_1, NULL ) \
   IIC_Slave_Define_Channel(IIC_CHANNEL_2, NULL ) \
   IIC_Slave_Define_Channel(IIC_CHANNEL_3, NULL ) \

/**
 * Size of Transmit buffers
 */
#define IIC_SLV_MAX_TX_SIZE         128

/**
 * Size of REceive buffers
 */
#define IIC_SLV_MAX_RX_SIZE         128


/* State of Health timeout override in msec – define here only if overriding SOH default */
#define DEFAULT_IIC_SLAVE_SEND_SOH_TIMEOUT_MS (35000)

#if SOH_DEFAULT_TIMEOUT_MS > DEFAULT_IIC_SLAVE_SEND_SOH_TIMEOUT_MS
#   define IIC_SLAVE_SEND_SOH_TIMEOUT_MS      SOH_DEFAULT_TIMEOUT_MS
#else
#   define IIC_SLAVE_SEND_SOH_TIMEOUT_MS      DEFAULT_IIC_SLAVE_SEND_SOH_TIMEOUT_MS
#endif

/* State of Health timeout override in msec – define here only if overriding SOH default */
/* #define IIC_SLAVE_RECEIVE_SOH_TIMEOUT_MS  <override value> */


/*===========================================================================*\
 * Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file iic_slave_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 26-Oct-2012 Larry Piekarski Rev 4
 *   - Task 125886: Changed override of SOH timeout to use default value if it
 *     is greater than the override.
 *
 *  9-May-2012 Brian Bolinger
 *  SCR 22979 Incorporate system level SOH Timeout - bb_base_drv_linux
 *  
 * - 28-Apr-2011 Doug Srail
 *   moved max size limits to config file
 *
 * 20-Jan-2011 A. Fernandez
 * + Created initial file for Linux Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* IIC_SLAVE_CFG_H */
