#ifndef IIC_CFG_H
#   define IIC_CFG_H
/*===========================================================================*/
/**
 * @file iic_cfg.h
 *
 * Micro specific iic channel mapping for Linux
 *
 * %full_filespec:iic_cfg.h~kok_inf#5:incl:kok_basa#2 %
 * @version %version:kok_inf#5 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue May  5 10:12:35 2015 %
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
 * Defines if virtual VIP channels are supported
 */
#   define IIC_SUPPORT_VIP    true

/**
 * Mapping of logical IIC Channel to device driver name
 *
 */
#   define IIC_Define_Channels   \
   IIC_Define_Channel(IIC_CHANNEL_0, NULL) \
   IIC_Define_Channel(IIC_CHANNEL_1, "/dev/i2c-0" ) \
   IIC_Define_Channel(IIC_CHANNEL_2, "/dev/i2c-1" ) \
   IIC_Define_Channel(IIC_CHANNEL_3, "/dev/i2c-2" ) \
   IIC_Define_Channel(IIC_CHANNEL_4, "/dev/i2c-3" ) \
   IIC_Define_Channel(IIC_CHANNEL_5, "/dev/i2c-4" ) \

/*===========================================================================*\
 * Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file iic_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 05-May-2015 Dan Carman
 *   + Increase default number of I2C to cover jacinto parts.
 *
 * 11-Nov-2010 Dan Carman
 * + Created initial file for Linux Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* IIC_CFG_H */
