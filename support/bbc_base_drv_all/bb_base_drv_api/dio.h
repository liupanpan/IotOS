#ifndef DIO_H
#   define DIO_H
/*===========================================================================*/
/**
 * @file dio.h
 *
 * Discrete I/O (GPIO) driver interface
 *
 * %full_filespec: dio.h~kok_inf#6:incl:kok_aud#2 %
 * @version %version: kok_inf#6 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Fri Sep  5 11:16:08 2014 %
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
 *   This API defines the interface to Discrete I/O
 *
 * @section ABBR ABBREVIATIONS:
 *   - DIO - Discrete I/O - a microprocesor pin that can be either high or low
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @defgroup dio_grp Discrete I/O Interface
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "reuse.h"
#   include "dio_acfg.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/
#   ifdef __cplusplus
extern "C"
{          /* ! Place all function prototypes inside these braces ! */
#   endif  /* __cplusplus */

/**
 * Initialize DIO module
 *
 * @return true if successful
 */
bool_t DIO_Initialize(void);

/**
 * Shutdown DIO module
 */
void DIO_Shutdown(void);

/**
 * Reads DIO input
 *
 * @param [in] port - Identifies which port to read
 *
 * @param [in] pin - Identifies pin within port range [0..15]
 *
 * @return true if pin is high
 */
bool_t DIO_Is_High(DIO_Port_T port, uint8_t pin);

/**
 * Sets DIO output state
 *
 * @param [in] port - Identifies which port to read
 *
 * @param [in] pin - Identifies pin within port range [0..15]
 *
 * @param [in] high - true if desired output state of pin is high
 */
void DIO_Set_High(DIO_Port_T port, uint8_t pin, bool_t high);

/**
 * Set Direction for DIO pin
 *
 * @param [in] port - Identifies which port
 *
 * @param [in] pin - Identifies pin within port range [0..15]
 *
 * @param [in] direction - Desired state of pin direction
 */
void DIO_Set_Direction(DIO_Port_T port, uint8_t pin, DIO_Direction_T direction);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}         /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file dio.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 2014-Sept-5 Dan Carman
 * + Add Shutdown API.
 *
 * 2010-jul-26 Jaime Almeida
 * + Replaced bool with bool_t.
 *
 * 2009-aug-19 Kris Boultbee
 * + Added extern C wrapper around all function prototypes.
 *
 * 2007-sep-28 kirk bailey
 * + Got rid of brief directive so that brief comment appears with version
 * + 1.5.3 of Doxygen.
 *
 * DAY-MON-DATE initials-location (example KWK-KOK)
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* DIO_H */
