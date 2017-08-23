#ifndef UART_ACFG_H
#   define UART_ACFG_H
/*===========================================================================*/
/**
 * @file uart_acfg.h
 *
 * @brief Micro specific uart public types for WIN32
 *
 * %full_filespec:uart_acfg.h~kok_inf#5:incl:kok_basa#9 %
 * @version %version:kok_inf#5 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed May  6 19:20:14 2015 %
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
   UA_CHANNEL_COM5,
   UA_CHANNEL_COM6,
   UA_CHANNEL_COM7,
   UA_CHANNEL_COM8,
   UA_CHANNEL_COM9,
   UA_CHANNEL_COM10,
   NUM_OF_UA_CHANNEL,
} UA_Channel_T;


/**
 * Define time out in seconds for each port
 */


/*
you can define a specific time for each port

#define UART_PORT_TIME_OUT(X) (X == UA_CHANNEL_COM0) ? 5 : \
                              (X == UA_CHANNEL_COM1) ? 5 : \
                              (X == UA_CHANNEL_COM2) ? 5 : \
                              (X == UA_CHANNEL_COM3) ? 5 : \
                              (X == UA_CHANNEL_COM4) ? 5 : \
                              (X == UA_CHANNEL_COM5) ? 0 : 5
*/
/*===========================================================================*/
/*!
 * @file uart_acfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 21-Aug-2013 Larry Piekarski Rev 4
 *   - Task 163984: Fix compiler warning for nexted comment
 *
 * 31-jul-2013 Franco Arias (kzlfpq) rev 3
 * + kok_basa#162391 include a form to configure the uart time out
 *
 * 20-aug-2010 Kris Boultbee
 * + Created initial file for Linux Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* UART_ACFG_H */
