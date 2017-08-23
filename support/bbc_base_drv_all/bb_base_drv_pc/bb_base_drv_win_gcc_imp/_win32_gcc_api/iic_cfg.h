#ifndef IIC_CFG_H
#   define IIC_CFG_H
/*===========================================================================*/
/**
 * @file iic_cfg.h
 *
 * Micro specific iic channel mapping for Linux
 *
 * %full_filespec:iic_cfg.h~1:incl:kok_basa#1 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 08:19:18 2010 %
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
#define IIC_SUPPORT_VIP    true

/**
 * Mapping of logical IIC Channel to device driver name
 *
 */
#   define IIC_Define_Channels   \
   IIC_Define_Channel(IIC_CHANNEL_1, "/dev/i2c-1" ) \
   IIC_Define_Channel(IIC_CHANNEL_2, "/dev/i2c-2" ) \
   IIC_Define_Channel(IIC_CHANNEL_3, "/dev/i2c-3" ) \

/*===========================================================================*\
 * Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file iic_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 11-Nov-2010 Dan Carman
 * + Created initial file for Linux Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* IIC_CFG_H */
