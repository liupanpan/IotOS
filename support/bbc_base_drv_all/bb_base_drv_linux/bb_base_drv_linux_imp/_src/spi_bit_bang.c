/*===========================================================================*/
/**
 * @file spi_bit_bang.c
 *
 * implements simple 3-wire bit-bang SPI master interface
 *
 * %full_filespec:spi_bit_bang.c~kok_inf#2:csrc:kok_basa#1 %
 * @version %version:kok_inf#2 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Thu Sep 26 12:24:16 2013 %
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
 * implements simple 3-wire bit-bang SPI master interface
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
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "pbc_trace.h"
#include "reuse.h"
#include "spi_bit_bang.h"
#include "spi_bb_cbk.h"
#include "xsal.h"

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 7);      /* define this file for assert handling */

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/** high time on clock */
uint8_t SPI_BB_Clock_High = 8;

/** Low time on clock */
uint8_t SPI_BB_Clock_Low = 7;

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/
static void spi_enable_io(void);
static uint8_t spi_transfer_byte(uint8_t tx);

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/*
 * Initializes SPI Master channel
 */
void SPI_BB_Open(void)
{
   spi_enable_io();
   SPI_BB_Clock(0);
}

/**
 * Shuts down SPI channel
 * @param channel - Hardware specific channel identifer
 */
void SPI_BB_Close(void)
{

}

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
bool_t SPI_BB_Write(const void *wdata, size_t num_bytes)
{
   size_t i;
   const uint8_t *tx_data = (uint8_t *) wdata;

   spi_enable_io();

   for (i = 0; i < num_bytes; i++)
   {
      (void)spi_transfer_byte(*tx_data++);
   }

   return true;
}


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
bool_t SPI_BB_Read(void *rdata, size_t num_bytes)
{
   size_t i;
   uint8_t *rx_data = (uint8_t *) rdata;

   spi_enable_io();

   for (i = 0; i < num_bytes; i++)
   {
      *rx_data++ = spi_transfer_byte(0xFF);
   }

   return true;
}


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
bool_t SPI_BB_Transfer(const void *wdata, void *rdata, size_t num_bytes)
{
   size_t i;
   const uint8_t *tx_data = (uint8_t *) wdata;
   uint8_t *rx_data = (uint8_t *) rdata;

   spi_enable_io();

   for (i = 0; i < num_bytes; i++)
   {
      *rx_data++ = spi_transfer_byte(*tx_data++);
   }

   return true;
}


/**
 * Bit band one byte on spi
 *
 * @parm tx - transmit byte
 *
 * @return - recieved byte
 */

static uint8_t spi_transfer_byte(uint8_t tx)
{
   int i;
   uint8_t rx = 0;
   uint8_t mask = 0x80;

   for (i = 0; i < 8; i++)
   {
      SPI_BB_Clock(1);
      SPI_BB_SIMO(0 != (tx & mask));
      SAL_Delay(SPI_BB_Clock_High);
      SPI_BB_Clock(0);
      if (SPI_BB_SOMI())
      {
         rx |= mask;
      }
      mask >>= 1;
      SAL_Delay(SPI_BB_Clock_Low);
   }

   return rx;
}


/**
 * Enable I/O pins
 */
static void spi_enable_io(void)
{
   SPI_BB_Clock_Enable();
   SPI_BB_SIMO_Enable();
   SPI_BB_SOMI_Enable();
}

/*===========================================================================*/
/*!
 * @file spi_bit_bang.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 *  - 16-Nov-2010 Dan Carman
 *   - Created separate bit bang driver
 *
 * - 26-Sep-2013 Larry Piekarski Rev 2
 *   - Task 1331: Fixed compiler warnings
 */
/*===========================================================================*/
