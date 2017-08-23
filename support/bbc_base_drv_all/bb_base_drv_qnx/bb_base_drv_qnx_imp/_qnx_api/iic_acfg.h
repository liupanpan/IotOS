#ifndef IIC_ACFG_H
#   define IIC_ACFG_H
/*===========================================================================*/
/**
 * @file iic_acfg.h
 *
 * IIC channel names
 *
 * %full_filespec: iic_acfg.h~2:incl:kok_inf#5 %
 * @version %version: 2 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Fri Sep  5 16:46:58 2014 %
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
 * Defines standard set of IIC channels
 *
 * @section ABBR ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *
 * @addtogroup iic_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define X-MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * Define IIC Channels
 */
typedef enum IIC_Channel_Tag
{
   IIC_CHANNEL_0,       /**< AP IIC Channel 0 */
   IIC_CHANNEL_1,       /**< AP IIC Channel 1 */
   IIC_CHANNEL_2,       /**< AP IIC Channel 2 */
   IIC_CHANNEL_3,       /**< AP IIC Channel 3 */
   IIC_CHANNEL_4,       /**< AP IIC Channel 4 */
   IIC_CHANNEL_5,       /**< AP IIC Channel 5 */
   NUMBER_IIC_CHANNELS
} IIC_Channel_T;


/*===========================================================================*/
/*!
 * @file iic_acfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 20-aug-2010 Kris Boultbee
 * + Created initial file for qnx Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* IIC_ACFG_H */
