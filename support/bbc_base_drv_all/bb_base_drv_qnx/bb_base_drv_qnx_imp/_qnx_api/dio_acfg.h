#ifndef DIO_ACFG_H
#   define DIO_ACFG_H
/*===========================================================================*/
/**
 * @file dio_acfg.h
 *
 * @brief Device micro-specific Discrete I/O types
 *
 * %full_filespec: dio_acfg.h~2:incl:kok_inf#9 %
 * @version %version: 2 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Fri Sep  5 16:46:56 2014 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * DESCRIPTION :
 *    Micro specific Discrete I/O types. This file is configured for all QNX 
 *      implementations
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
 * @addtogroup dio_grp
 * @{
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define X-MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/*
 * Identifiers for Discrete I/O (GPIO) ports
 *
 * Not all ports are available on all micros
 *    J6  1-7
 *    IMX6 1-7
 *    J5 0-3
 */
typedef enum DIO_Port_Tag
{
      DIO_GPIO0, /**< Port 0 */
      DIO_GPIO1, /**< Port 1 */
      DIO_GPIO2, /**< Port 2 */
      DIO_GPIO3, /**< Port 3 */
      DIO_GPIO4, /**< Port 4 */
      DIO_GPIO5, /**< Port 5 */
      DIO_GPIO6, /**< Port 6 */
      DIO_GPIO7, /**< Port 7 */
      DIO_GPIO8, /**< Port 8 */
} DIO_Port_T;

/**
 * DIO Direction Settings
 */
typedef enum DIO_Direction_Tag
{
   DIO_INPUT,                                   /**< pin is input  */
   DIO_OUTPUT,                                  /**< pin is standard output */
} DIO_Direction_T;

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
  *
 * - 5-Sept-2014 Dan Carman
 *   Task 11694 - Update QNX base drivers to work with J6 and IMX6
 *     Set Enums range to cover all micros
 *
 * 20-aug-2010 Kris Boultbee
 * + Created initial file for qnx Host.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* DIO_ACFG_H */
