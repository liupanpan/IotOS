#ifndef IIC_ACFG_H
#   define IIC_ACFG_H
/*===========================================================================*/
/**
 * @file iic_acfg.h
 *
 * @brief Device micro-specific IIC channel names
 *
 * %full_filespec: iic_acfg.h~1:incl:kok_aud#82 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon Nov  2 17:08:01 2009 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    Micro specific IIC channel names. This file is configured for the Renesas SH7263
 *       which supports 4 channels.
 *
 * ABBREVIATIONS:
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *
 * DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @addtogroup iic
 * @{
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define X-MACROS 
\*===========================================================================*/

#   define IIC_Define_Channels   \
	IIC_Define_Channel(IIC_CHANNEL_0,"dev\\iic\\0"	)	\
	IIC_Define_Channel(IIC_CHANNEL_1,"dev\\iic\\1"	)	\
	IIC_Define_Channel(IIC_CHANNEL_2,"dev\\iic\\2"	)	\
	IIC_Define_Channel(IIC_CHANNEL_3,"dev\\iic\\3"	)

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

#   undef IIC_Define_Channel
#   define IIC_Define_Channel(ID, name)	ID,

/**
 * Define IIC Channels
 */
typedef enum IIC_Channel_Tag
{
   IIC_Define_Channels NUMBER_IIC_CHANNELS
} IIC_Channel_T;

#   undef IIC_Define_Channel

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * DAY-MON-DATE initials-location (example KWK-KOK)
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* iic_H */
