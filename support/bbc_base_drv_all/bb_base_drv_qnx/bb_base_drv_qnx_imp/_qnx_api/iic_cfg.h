#ifndef IIC_CFG_H
#   define IIC_CFG_H
/*===========================================================================*/
/**
 * @file iic_cfg.h
 *
 * Micro specific iic channel mapping for qnx
 *
 * %full_filespec:iic_cfg.h~2:incl:kok_inf#7 %
 * @version %version:2 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Fri Sep  5 16:47:00 2014 %
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
 * Defines QNX standard device paths for IIC drivers
 *    Not all devices will be implemented. If BSP doesn't 
 *    implement a particular channel, you will get an error on open. 
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
 * Mapping of logical IIC Channel to device driver name
 *
 */
#   define IIC_Define_Channels   \
   IIC_Define_Channel(IIC_CHANNEL_0, "/dev/i2c0") \
   IIC_Define_Channel(IIC_CHANNEL_1, "/dev/i2c1") \
   IIC_Define_Channel(IIC_CHANNEL_2, "/dev/i2c2") \
   IIC_Define_Channel(IIC_CHANNEL_3, "/dev/i2c3") \
   IIC_Define_Channel(IIC_CHANNEL_4, "/dev/i2c4") \
   IIC_Define_Channel(IIC_CHANNEL_5, "/dev/i2c5") \

/*===========================================================================*\
 * Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file iic_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 5-Sept-2014 Dan Carman
 *   Task 11694 - Update QNX base drivers to work with J6 and IMX6
 *
 * 11-Nov-2010 Dan Carman
 * + Created initial file for qnx Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* IIC_CFG_H */
