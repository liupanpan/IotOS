#ifndef SPI_BB_CBK_H
#   define SPI_BB_CBK_H
/*===========================================================================*/
/**
 * @file spi_bb_CBK.h
 *
 * Bit bang SPI I/O callouts
 *
 * %full_filespec:spi_bb_cbk.h~1:incl:kok_basa#1 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 13:21:00 2010 %
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
 * Provides a simple 3-wire Master SPI interface
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
 * @defgroup spi Basic SPI Interface
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/
#   ifdef __cplusplus
extern "C"
{                               /* ! Place all function prototypes inside these braces ! */

#   endif                       /* __cplusplus */

/*===========================================================================*
 * Imported Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Configure the SPI clock pin as output
 */
   void SPI_BB_Clock_Enable(void);

/**
 * Set the SPI clock pin
 *
 * @param active - true active state, false idle state
 */
   void SPI_BB_Clock(bool_t active);


/**
 * Configure the SPI SIMO pin as output
 */
   void SPI_BB_SIMO_Enable(void);

/**
 * Set the SPI Master Out pin
 *
 * @param high
 */
   void SPI_BB_SIMO(bool_t high);


/**
 * Configure the SPI SOMI pin as input
 */
   void SPI_BB_SOMI_Enable(void);

/**
 * Return state of SOMI
 *
 * @return  true if SOMI is high
 */
     bool_t SPI_BB_SOMI(void);

#   ifdef __cplusplus
} /* extern "C" - function prototypes must precede this brace */
#   endif                       /* __cplusplus */

/*===========================================================================*/
/*!
 * @file spi_bb_cbk.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 *  - 19 July 2010 Jaime Almeida
 *   - Replace bool with bool_t.
 *
 * - 22-Oct-2009 Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* SPI_BB_H */
