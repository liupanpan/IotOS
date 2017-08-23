#ifndef PRINTF_GATE_CFG_H
#   define PRINTF_GATE_CFG_H
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */
/*===========================================================================*/
/**
 * @file printf_gate_cfg.h
 *
 * Configures the printf_gate module implementation.
 *
 * %full_filespec:printf_gate_cfg.h~kok_basa#3:incl:kok_aud#46 %
 * @version %version:kok_basa#3 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Apr 15 18:16:54 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @todo Add full description here
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
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

/*===========================================================================*
 * #define Constants
 *===========================================================================*/
/**
 * If this symbol is defined, then the printf_gate module is built without any
 * dependencies to XSAL and the functions Printf_Lock() and Printf_Unlock()
 * do nothing. This is useful for test environments built without and RTOS
 * (XSAL) dependencies.
 */ 
/* #   define PRINTF_GATE_STANDALONE */

/*===========================================================================*
 * #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Custom Type Declarations
 *===========================================================================*/

/*===========================================================================*/
/*!
 * @file printf_gate_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 22-aug-2008 kirk bailey
 *   - Created initial file.
 */
/*===========================================================================*/
#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */
#endif                          /* PRINTF_GATE_CFG_H */
