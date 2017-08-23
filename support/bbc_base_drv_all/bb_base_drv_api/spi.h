#ifndef SPI_H
#   define SPI_H
/*===========================================================================*/
/**
 * @file spi.h
 *
 * Standard interface to basic 3 wire SPI interface
 *
 * %full_filespec:spi.h~kok_basa#4:incl:kok_aud#35 %
 * @version %version:kok_basa#4 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 08:19:42 2010 %
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

/**
 * Channel number for bit bang SPI
 */ 
#define SPI_BB_CHANNEL  (0x7F)

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/** SPI configuration
 */
typedef struct SPI_Config_Tag
{
   uint32_t frequency;     /**< SPI clock frequency in Hertz */
   uint8_t char_length;    /**< bits per transfer */
   bool_t clock_high;      /**< Clock Idle high (true) or Low (false) */
   bool_t std_phase;       /**< true - change 1st edge, read 2nd, false read 1st */
   bool_t MSB;             /**< true - data sent MSB, false LSB */
   uint8_t delay_uS;       /**< delay between transfers */
} SPI_Config_T;


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
   int8_t SPI_Open(uint8_t channel, const SPI_Config_T * config);

/**
 * Shuts down SPI channel
 * @param channel - Hardware specific channel identifer
 */
   void SPI_Close(uint8_t channel);

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
   bool_t SPI_Write(uint8_t channel, const void *wdata, size_t num_bytes);

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
   bool_t SPI_Read(uint8_t channel, void *rdata, size_t num_bytes);

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
   bool_t SPI_Transfer(uint8_t channel, const void *wdata, void *rdata, size_t num_bytes);

#   ifdef __cplusplus
}                               /* extern "C" - function prototypes must precede this brace */
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file spi.h
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
#endif                          /* SPI_H */
