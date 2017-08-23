#ifndef UART_CFG_H
#   define UART_CFG_H
/*===========================================================================*/
/**
 * @file uart_cfg.h
 *
 * Micro specific uart channel mapping for Linux
 *
 * %full_filespec:uart_cfg.h~1:incl:kok_basa#2 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 08:19:21 2010 %
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
 * Defines Micro specific uart public types namely the Available channels
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
 * @addtogroup uart_grp
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
 * Mapping of logical UART Channel to device driver name
 *
 */
#   define UART_Define_Channels   \
   UART_Define_Channel(UA_CHANNEL_COM0, "/dev/com0" ) \
   UART_Define_Channel(UA_CHANNEL_COM1, "/dev/com1" ) \
   UART_Define_Channel(UA_CHANNEL_COM2, "/dev/com2" ) \
   UART_Define_Channel(UA_CHANNEL_COM3, "/dev/com3" ) \
   UART_Define_Channel(UA_CHANNEL_COM4, "/dev/com4" ) \

/*===========================================================================*\
 * Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file uart_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 11-Nov-2010 Dan Carman
 * + Created initial file for Linux Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* UART_CFG_H */
