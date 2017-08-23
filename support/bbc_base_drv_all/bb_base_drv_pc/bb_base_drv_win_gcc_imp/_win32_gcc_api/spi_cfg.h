#ifndef SPI_CFG_H
#   define SPI_CFG_H
/*===========================================================================*/
/**
 * @file spi_cfg.h
 *
 * Micro specific spi channel mapping
 *
 * %full_filespec:spi_cfg.h~1:incl:kok_basa#1 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Nov 16 12:13:52 2010 %
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
 * Defines Micro specific spi public types namely the Available channels
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
 * @addtogroup spi_grp
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
 * Define format of input / output data (binary or ASCII)
*/
#define SPI_SIM_ASCII   true 

/**
 * Define number of unique spi channels 
 */
#define SPI_NUM_CHANNELS  3
 
/**
 * Mapping of logical SPI Channel to device driver name
 *
 */
#   define SPI_Define_Channels   \
   SPI_Define_Channel(0, "spi_in_0.dat",  "spi_out_0.dat") \
   SPI_Define_Channel(1, "spi_in_1.dat",  "spi_out_1.dat" ) \
   SPI_Define_Channel(2, "spi_in_2.dat",  "spi_out_2.dat" ) \

/*===========================================================================*\
 * Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file spi_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 11-Nov-2010 Dan Carman
 * + Created initial file for Linux Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* SPI_CFG_H */
