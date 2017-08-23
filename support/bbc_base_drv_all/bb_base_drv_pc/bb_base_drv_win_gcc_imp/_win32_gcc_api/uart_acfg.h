#ifndef UART_ACFG_H
#   define UART_ACFG_H
/*===========================================================================*/
/**
 * @file uart_acfg.h
 *
 * @brief Micro specific uart public types for WIN32
 *
 * %full_filespec:uart_acfg.h~kok_basa#2:incl:kok_aud#1 %
 * @version %version:kok_basa#2 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 08:20:17 2010 %
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
#   include <termios.h>         /* POSIX terminal control definitions */

/*===========================================================================*\
 * Exported Preprocessor #define X-MACROS
\*===========================================================================*/
#   ifdef CNEW_RTSCTS
#      define UA_HW_HANDSHAKE_IS_SUPPORTED
#   endif

/** 
 * Enable use of underlying software handshake 
 */
#   define UA_SW_HANDSHAKE_IS_SUPPORTED

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/
/**
 * Channel Destination
 */
typedef enum
{
   UA_CHANNEL_COM0,
   UA_CHANNEL_COM1,
   UA_CHANNEL_COM2,
   UA_CHANNEL_COM3,
   UA_CHANNEL_COM4,
   NUM_OF_UA_CHANNEL,
} UA_Channel_T;

/*===========================================================================*/
/*!
 * @file uart_acfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 *
 * 15-June-2007 Dan Carman
 * + Merged bb_uart_control into base drivers as bb_uart
 * + Change to common uart attributes
 * + Moved bit rate translation definitions to implementation
 *
 * 23-may-2007 henry yu
 * + Update for POSIX serial control.
 *
 * 11-may-2007 henry yu
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* UART_ACFG_H */
