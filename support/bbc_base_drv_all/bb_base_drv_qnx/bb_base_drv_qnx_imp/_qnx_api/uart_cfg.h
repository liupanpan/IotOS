#ifndef UART_CFG_H
#   define UART_CFG_H
/*===========================================================================*/
/**
 * @file uart_cfg.h
 *
 * Specific uart channel mapping for qnx
 *    This file works for QNX implementations (IMX6, J6, etc.)
 *
 * %full_filespec:uart_cfg.h~3:incl:kok_inf#10 %
 * @version %version:3 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed May  6 19:02:55 2015 %
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
 * Mapping of logical UART Channel to actual device driver name
 *
 *  FYI V2X - DEBUG - COM1, GPS - COM2, DESIP - COM3
 *
 */
#   define UART_Define_Channels   \
   UART_Define_Channel(UA_CHANNEL_COM0, "/dev/ser0" ) \
   UART_Define_Channel(UA_CHANNEL_COM1, "/dev/ser1" ) \
   UART_Define_Channel(UA_CHANNEL_COM2, "/dev/ser2" ) \
   UART_Define_Channel(UA_CHANNEL_COM3, "/dev/ser3" ) \
   UART_Define_Channel(UA_CHANNEL_COM4, "/dev/ser4" ) \
   UART_Define_Channel(UA_CHANNEL_COM5, "/dev/ser5" ) \
   UART_Define_Channel(UA_CHANNEL_COM6, "/dev/ser6" ) \
   UART_Define_Channel(UA_CHANNEL_COM7, "/dev/ser7" ) \
   UART_Define_Channel(UA_CHANNEL_COM8, "/dev/ser8" ) \
   UART_Define_Channel(UA_CHANNEL_COM9, "/dev/ser9" ) \
   UART_Define_Channel(UA_CHANNEL_COM10, "/dev/ser10" )

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
 * + Created initial file for qnx Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* UART_CFG_H */
