#ifndef UTIL_CFG_H
#   define UTIL_CFG_H
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */
/*===========================================================================*/
/**
 * @file util_cfg.h
 *
 *   Configuration options for bb_util
 *
 * %full_filespec: %
 * @version %version: %
 * @author  %derived_by: %
 * @date    %date_modified: %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * This file has the project specific options for the utilities:
 *    These options are:
 *          Checksum prohibits NULL addresses
 *
 * @section ABBR ABBREVIATIONS:
 *   - none
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *     - BASA RTOS SERVICES Software Requirement Specification
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
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
 * Defines whether checksum routines will allow the start address to be 0.
 * A NULL address may be allowed in systems that allow direct addressing to
 * address 0, so this define can be set to false.  If set to true, the checksum
 * functions will fail PBC checks for NULL addresses.
 */
#   define CHKS_NULL_ADDRESS_PROHIBITED (true)

/*===========================================================================*\
 * #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Custom Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file util_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 08-Nov-2011 Kirk Bailey
 *   - Brought MDF to tip.
 */
/*===========================================================================*/
#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */
#endif                          /* UTIL_CFG_H */
