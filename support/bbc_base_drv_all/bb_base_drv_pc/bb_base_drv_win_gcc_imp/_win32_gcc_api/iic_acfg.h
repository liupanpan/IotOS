#ifndef IIC_ACFG_H
#   define IIC_ACFG_H
/*===========================================================================*/
/**
 * @file iic_acfg.h
 *
 * IIC channel names
 *
 * %full_filespec: iic_acfg.h~kok_basa#2:incl:kok_aud#3 %
 * @version %version: kok_basa#2 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Nov 16 08:20:14 2010 %
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
      NUMBER_IIC_CHANNELS, /**< Number of AP IIC Channels  */
      
      IIC_CHANNEL_V0 = NUMBER_IIC_CHANNELS, /**< VIP IIC Channel 0 */
      IIC_CHANNEL_V1,      /**< VIP IIC Channel 1 */
      IIC_CHANNEL_V2,      /**< VIP IIC Channel 2 */
      IIC_CHANNEL_V3,      /**< VIP IIC Channel 3 */
      TOTAL_IIC_CHANNELS   /**< Total AP & VIP IIC Channels */
} IIC_Channel_T;

/*===========================================================================*/
/*!
 * @file iic_acfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 * DAY-MON-DATE initials-location (example KWK-KOK)
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* IIC_ACFG_H */
