#ifndef SPI_CFG_H
#   define SPI_CFG_H
/*===========================================================================*/
/**
 * @file spi_cfg.h
 *
 * Micro specific spi channel mapping
 *
 * %full_filespec:spi_cfg.h~2:incl:kok_inf#5 %
 * @version %version:2 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Fri Sep  5 16:47:01 2014 %
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
 * Defines Micro specific spi driver paths for QNX 
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
 *    Upper Nybble is chip select (not used with QNX)
 *
 *  1 is ECSPI 1
 *  2 is ECSPI 2
 *  3 is CSPI
 */
#   define SPI_Define_Channels \
   SPI_Define_Channel(0x01, "/dev/spi1", "SPI Bus 1, CS x") \
   SPI_Define_Channel(0x02, "/dev/spi2", "SPI_BUS_2, CS x") \
   SPI_Define_Channel(0x03, "/dev/spi3", "SPI_BUS_3, CS x") \
   SPI_Define_Channel(0x04, "/dev/spi4", "SPI_BUS_4, CS x") \

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
 * + Created initial file for qnx Host.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* SPI_CFG_H */
