/*===========================================================================*/
/**
 * @file spi_qnx.c
 *
 * implements simple 3-wire SPI master interface
 *
 * %full_filespec:spi_qnx.c~1:csrc:kok_inf#1 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Apr 14 14:12:06 2014 %
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
 * Simulates SPI interface by reading / writing to files
 *
 * @section ABBR ABBREVIATIONS:
 *   - SPI - Serial Peripheral Interface
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
#include "spi.h"
#include <string.h>
#include <errno.h>
#include  <hw/spi-master.h>

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/
EM_FILENUM(BASE_DRIVERS_MODULE_ID, 6);
/* define this file for assert handling */

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

#define SPI_NUM_BUSES   4

/* returns the SPI bus (which SPI device) */
#define SPI_Get_Bus(channel)     (((channel) - 1) & 0x03)

/** returns the device number on SPI bus */
#define SPI_Get_Device(channel)  (((channel) >> 4) & 0x0F)

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/
typedef struct SPI_Info_Tag
{
      int handle;
      const char * driver_name;
} SPI_Info_T;

static SPI_Info_T spi_info[SPI_NUM_BUSES] =
   {
      { -1, "/dev/spi1" },
      { -1, "/dev/spi2" },
      { -1, "/dev/spi3" },
      { -1, "/dev/spi4" } };

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/
static SPI_Info_T * Get_SPI(uint8_t channel);
static int Set_Configuration(uint8_t channel, int fd, const SPI_Config_T * config);

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/**
 * Returns a pointer to the SPI Info for a given channel
 *
 * @param channel - SPI channel ID
 *
 * @return pointer to SPI info descriptor
 */
static SPI_Info_T * Get_SPI(uint8_t channel)
{
   return &spi_info[SPI_Get_Bus(channel)];
}

/**
 * Configures a SPI channel
 *
 * @param channel - SPI channel ID
 * @param fd - handle to SPI device
 * @param config - Desired configuration options
 *
 */
static int Set_Configuration(uint8_t channel, int fd, const SPI_Config_T * config)
{
   int results;

   spi_cfg_t cfg =
      { 0, 0 };

   cfg.mode = config->char_length & SPI_MODE_CHAR_LEN_MASK;

   if (config->clock_high)
   {
      cfg.mode = SPI_MODE_CKPOL_HIGH;
   }

   if (config->std_phase)
   {
      cfg.mode |= SPI_MODE_CKPHASE_HALF;
   }

   if (config->MSB)
   {
      cfg.mode |= SPI_MODE_BODER_MSB;
   }

   if (config->delay_uS)
   {
      cfg.mode |= SPI_MODE_IDLE_INSERT;
   }

   cfg.clock_rate = config->frequency;

   results = spi_setcfg(fd, SPI_Get_Device(channel), &cfg);

   if (results)
   {
      Tr_Warn_1("Set_Configuration: %s", strerror(results));
   }

   return results;
}

/*
 * Initializes SPI Master channel
 */
int8_t SPI_Open(uint8_t channel, const SPI_Config_T * config)
{
   int8_t results = -1;

   SPI_Info_T * spi = Get_SPI(channel);

   spi->handle = spi_open(spi->driver_name);

   if (spi->handle >= 0)
   {
      if (0 == Set_Configuration(channel, spi->handle, config))
      {
         results = (int8_t) channel;
      }
   }

   if (results < 0)
   {
      Tr_Fault_2("Error opening SPI channel %d Driver %s", channel, spi->driver_name);
      spi->handle = -1;
   }

   return results;
}

/**
 * Shuts down SPI channel
 * @param channel - Hardware specific channel identifer
 */
void SPI_Close(uint8_t channel)
{
   SPI_Info_T * spi = Get_SPI(channel);

   if (spi->handle >= 0)
   {
      spi_close(spi->handle);
   }
   spi->handle = -1;
}

/*
 * SPI Master Transmit  (read data is ignored)
 *    Blocks until write finishes
 */
bool_t SPI_Write(uint8_t channel, const void *wdata, size_t num_bytes)
{
   int results = EBADF;

   SPI_Info_T * spi = Get_SPI(channel);

   if (spi->handle >= 0)
   {
      results = spi_write(spi->handle, SPI_Get_Device(channel), (void *) wdata, num_bytes);
   }

   if ((size_t)results != num_bytes)
   {
      Tr_Warn_2("SPI_Write: Error %d - %s", results, strerror(results));
   }

   return ((size_t)results == num_bytes);
}

/*
 * SPI Master Recieve  (transmit data is fixed)
 *    Blocks until read finishes
 */
bool_t SPI_Read(uint8_t channel, void *rdata, size_t num_bytes)
{
   int results = EBADF;

   SPI_Info_T * spi = Get_SPI(channel);

   if (spi->handle >= 0)
   {
      results = spi_read(spi->handle, SPI_Get_Device(channel), rdata, num_bytes);
   }

   if ((size_t)results != num_bytes)
   {
      Tr_Warn_1("SPI_Read: %s", strerror(results));
   }

   return ((size_t)results == num_bytes);
}

/*
 * SPI Master Parallel Transmit / Recieve
 *    Blocks until transfer finishes
 */
bool_t SPI_Transfer(uint8_t channel, const void *wdata, void *rdata, size_t num_bytes)
{
   int results = EBADF;

   SPI_Info_T * spi = Get_SPI(channel);

   if (spi->handle >= 0)
   {
#if 1
      results = spi_xchange(spi->handle, SPI_Get_Device(channel), (void *) wdata, rdata, num_bytes);
#else
      results = spi_dma_xchange(spi->handle, SPI_Get_Device(channel), (void *) wdata, rdata, num_bytes);
#endif
   }

   if ((size_t)results != num_bytes)
   {
      Tr_Warn_1("SPI_Transfer: %s", strerror(results));
   }

   return ((size_t)results == num_bytes);
}

/*===========================================================================*/
/*!
 * @file spi_qnx.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 *  - 27-Mar-2014 Dan Carman
 *   - Initial qnx wrapper
 */
/*===========================================================================*/
