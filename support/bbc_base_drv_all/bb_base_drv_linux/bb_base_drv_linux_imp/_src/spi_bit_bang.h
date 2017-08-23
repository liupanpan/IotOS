#ifndef SPI_BIT_BANG_H
#   define SPI_BIT_BANG_H
/*===========================================================================*/
/**
 * @file spi_bit_bang.h
 *
 * Standard interface to basic 3 wire SPI interface
 *
 * %full_filespec:spi_bit_bang.h~1:incl:kok_basa#1 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 13:21:35 2010 %
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
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Initializes SPI Master channel
 *
 * @param channel - Hardware specific channel identifer
 * @param config - SPI configuration
 *
 * @return >= 0 channel number if sucessful, < 0 error code
 */
void SPI_BB_Open(void);

/**
 * Shuts down SPI channel
 * @param channel - Hardware specific channel identifer
 */
void SPI_BB_Close(void);

/**
 * SPI Master Transmit  (read data is ignored)
 *    Blocks until write finishes
 *
 * @param channel - Hardware specific channel identifer
 * @param wdata - pointer to data to transmit
 * @param num_bytes - number of bytes to transmit
 *
 * @return true if write successful
 */
bool_t SPI_BB_Write(const void *wdata, size_t num_bytes);

/**
 * SPI Master Recieve  (transmit data is fixed)
 *    Blocks until read finishes
 *
 * @param channel - Hardware specific channel identifer
 * @param rdata - pointer to recieve data buffer
 * @param num_bytes - number of bytes to transmit
 *
 * @return true if read successful
 */
bool_t SPI_BB_Read(void *rdata, size_t num_bytes);

/**
 * SPI Master Parallel Transmit / Recieve
 *    Blocks until transfer finishes
 *
 * @param channel - Hardware specific channel identifer
 * @param wdata - pointer to data to transmit
 * @param rdata - pointer to recieve data buffer
 * @param num_bytes - number of bytes to transfer
 *
 * @return true if transfer successful
 */
bool_t SPI_BB_Transfer(const void *wdata, void *rdata, size_t num_bytes);

#   ifdef __cplusplus
} /* extern "C" - function prototypes must precede this brace */
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file spi_bit_bang.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 *  - 19 July 2010 Jaime Almeida
 *   - Replace bool with bool_t.
 *
 * - 13-Oct-2009 Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* SPI_BIT_BANG_H */
