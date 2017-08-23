#ifndef SPI_CFG_H
#   define SPI_CFG_H
/*===========================================================================*/
/**
 * @file spi_cfg.h
 *
 * Micro specific spi channel mapping
 *
 * %full_filespec:spi_cfg.h~6:incl:kok_basa#2 %
 * @version %version:6 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Thu Oct 27 11:13:29 2011 %
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
 *   - None
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
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
 * Define if User side Bit Bang SPI channel is supported
 */
#   define SPI_SUPPORT_BIT_BANG  false

/**
 * Mapping of logical SPI Channel to device driver name
 *    Lower Nybble is channel #
 *    Upper Nybble is chip select
 *
 *  1 is ECSPI 1
 *  2 is ECSPI 2
 *  3 is CSPI
 *  4 is kernel bit bang
 *  SPI_BB_CHANNEL is user side bit bang
 */
#   define SPI_Define_Channels \
   SPI_Define_Channel(0x01, "/dev/spidev1.0", NULL) \
   SPI_Define_Channel(0x02, "/dev/spidev2.0", "SPI_BUS_2") \
   SPI_Define_Channel(0x12, "/dev/spidev2.1", "SPI_BUS_2") \
   SPI_Define_Channel(0x03, "/dev/spidev3.0", NULL) \
   SPI_Define_Channel(0x04, "/dev/spidev4.0", NULL) \

/*===========================================================================*\
 * Type Declarations
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file spi_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 17-Oct-2011 Dan Carman
 * + Expanded list of SPI drivers to cover SBX and ICR
 *
 * 11-Nov-2010 Dan Carman
 * + Created initial file for Linux Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* SPI_CFG_H */
