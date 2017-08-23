/*===========================================================================*/
/**
 * @file spi_linux.c
 *
 * implements simple 3-wire bit-bang SPI master interface
 *
 * %full_filespec:spi_linux.c~5:csrc:kok_basa#1 %
 * @version %version:5 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Thu Oct 27 11:13:30 2011 %
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
#include "spi_cfg.h"
#include "xsal.h"
#include "xsal_util.h"
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#if SPI_SUPPORT_BIT_BANG
#   include "spi_bit_bang.h"
#endif

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/
EM_FILENUM(BASE_DRIVERS_MODULE_ID, 6);  /* define this file for assert handling */

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

#ifndef SPI_LOCK_TIMEOUT_MS
/**
 * Default time limit to wait for a SPI channel (on SPI_Open)
 */
#define SPI_LOCK_TIMEOUT_MS      (10000)
#endif 

/**
 * Define number of unique spi channels
 */
#define SPI_NUM_CHANNELS   Num_Elems(spi_names)

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

typedef struct SPI_Channel_Name_Tag
{
   uint8_t channel;              /**< Channel ID */
   const char * dev_name;        /**< Device */
   const char * lock;            /**< lock */
} SPI_Channel_Name_T;

typedef struct SPI_Info_Tag
{
   int handle;       /**< device handle */

	uint32_t speed_hz;      /**< device's bitrate */
	uint16_t delay_usecs;   /**< device's wordsize */
	uint8_t	bits_per_word; /**< If nonzero, how long to delay after the last bit transfer
                           before optionally deselecting the device before the next transfer. */
	uint8_t  cs_change;     /**<  True to deselect device before starting the next transfer */
} SPI_Info_T;

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

#define SPI_Define_Channel(chan, dev, lock) {chan, dev, lock},

static const SPI_Channel_Name_T spi_names[] =
{
   SPI_Define_Channels
};

static SPI_Info_T spi_info[SPI_NUM_CHANNELS];

/**
 * Determine if SPI has been initialized
 */
static SAL_Util_Once_T SPI_Init = SAL_UTIL_ONCE_INIT;

/**
 * Locks used to restrict access to single physical channel
 */
static SAL_PMutex_T SPI_Locks[SPI_NUM_CHANNELS];

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/
static SPI_Info_T * Get_SPI(uint8_t channel);
static const char *Get_SPI_Driver(uint8_t channel);
static void Set_Configuration(uint8_t channel, int fd, const SPI_Config_T * config);
static void SPI_Initialize(void);
static bool_t SPI_Lock(uint8_t channel);
static void SPI_Unlock(uint8_t channel);

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
   uint_fast8_t i;

   for (i = 0; (i < SPI_NUM_CHANNELS) && (spi_names[i].channel != channel); i++)
   {
      /* do nothing here */
   }

   PBC_Ensure_1(i < SPI_NUM_CHANNELS, "Invalid SPI channel %d", channel);

   return &spi_info[i];
}

/**
 * Returns a pointer to the SPI Device Name for a given channel
 *
 * @param channel - SPI channel ID
 *
 * @return pointer to SPI Device Name
 */
static const char *Get_SPI_Driver(uint8_t channel)
{
   uint_fast8_t i;

   for (i = 0; (i < SPI_NUM_CHANNELS) && (spi_names[i].channel != channel); i++)
   {
      /* do nothing here */
   }

   PBC_Ensure_1(i < SPI_NUM_CHANNELS, "Invalid SPI channel %d", channel);

   return spi_names[i].dev_name;
}

/**
 * Configures a SPI channel
 *
 * @param channel - SPI channel ID
 * @param fd - handle to SPI device
 * @param config - Desired configuration options
 *
 */
static void Set_Configuration(uint8_t channel, int fd, const SPI_Config_T * config)
{
   uint32_t speed;
   int results;
   uint8_t mode;
   uint8_t lsb;
   uint8_t bits;

   mode = 0;
   if (config->clock_high)
   {
      mode = SPI_CPOL;
   }
   if (config->std_phase)
   {
      mode |= SPI_CPHA;
   }
   results = ioctl(fd, SPI_IOC_WR_MODE, &mode);
   PBC_Ensure_3(results >= 0, "Error %d Setting SPI %d Mode %d", results, channel, mode);

   lsb = !config->MSB;
   results = ioctl(fd, SPI_IOC_WR_LSB_FIRST, &lsb);
   PBC_Ensure_3(results >= 0, "Error %d Setting SPI %d bit direction %d", results, channel, lsb);

   bits = config->char_length;
   results = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
   PBC_Ensure_3(results >= 0, "Error %d Setting SPI %d word size %d", results, channel, bits);

   speed = config->frequency;
   results = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
   PBC_Ensure_3(results >= 0, "Error %d Setting SPI %d speed %d", results, channel, speed);

}

/**
 * Initialize channel mutexes
 */
static void SPI_Initialize(void)
{
   uint_fast8_t i;

   for (i = 0; i < SPI_NUM_CHANNELS; i++)
   {
      if (NULL != spi_names[i].lock)
      {
         SAL_Create_PMutex(&SPI_Locks[i], spi_names[i].lock);
      }
   }
}

/**
 * Lock physical channel
 *    Multiple "logical" channel can be tied to same SPI Bus
 */
static bool_t SPI_Lock(uint8_t channel)
{
   uint_fast8_t i;
   bool_t success = false;

   for (i = 0; i < SPI_NUM_CHANNELS; i++)
   {
      if (spi_names[i].channel == channel)
      {
         if (NULL != spi_names[i].lock)
         {
            success = SAL_Lock_PMutex(SPI_Locks[i]);
         }
         else
         {
            success = true;
         }
         break;
      }
   }

   return success;
}

/**
 * Release physical channel
 */
static void SPI_Unlock(uint8_t channel)
{
   uint_fast8_t i;

   for (i = 0; i < SPI_NUM_CHANNELS; i++)
   {
      if (spi_names[i].channel == channel)
      {
         if (NULL != spi_names[i].lock)
         {
            (void) SAL_Unlock_PMutex(SPI_Locks[i]);
         }
         break;
      }
   }
}

/*
 * Initializes SPI Master channel
 */
int8_t SPI_Open(uint8_t channel, const SPI_Config_T * config)
{
   int8_t results = (int8_t) channel;

   SAL_Util_Once(&SPI_Init, SPI_Initialize);

   if (SPI_Lock(channel))
   {

#if SPI_SUPPORT_BIT_BANG
      if (channel == SPI_BB_CHANNEL)
      {
         SPI_BB_Open();
      }
      else
#endif
      {
         SPI_Info_T * spi = Get_SPI(channel);

         spi->handle = open(Get_SPI_Driver(channel), O_RDONLY);

         if (spi->handle >= 0)
         {
            Set_Configuration(channel, spi->handle, config);
         }
         else
         {
            Tr_Fault_3("Error %d opening SPI channel %d Input File %s", (int)spi->handle, channel, Get_SPI_Driver(channel));
            results = -1;
         }
      }
   }
   else
   {
      Tr_Fault_1("Unable to lock SPI channel %d", channel);
      results = -1;
   }

   return results;
}

/**
 * Shuts down SPI channel
 * @param channel - Hardware specific channel identifer
 */
void SPI_Close(uint8_t channel)
{
#if SPI_SUPPORT_BIT_BANG
   if (channel == SPI_BB_CHANNEL)
   {
      SPI_BB_Close();
   }
   else
#endif
   {
      SPI_Info_T * spi = Get_SPI(channel);

      close(spi->handle);
      spi->handle = -1;
   }

   SPI_Unlock(channel);
}

/**
 * Routine to request a SPI operation from driver
 *
 * @param spi - pointer to SPI descriptor
 * @param wdata - pointer to data to transmit (NULL if read only)
 * @param rdata - pointer to recieve data buffer (NULL if write only)
 * @param num_bytes - number of bytes to transfer
 *
 * @return true if successful
 */
static bool_t Do_Transfer(SPI_Info_T * spi, const void * wdata, void * rdata, size_t len)
{
   struct spi_ioc_transfer xfer;
   int status;

   PBC_Require(spi->handle > 0, "SPI device is not open");

   memset(&xfer, 0, sizeof(xfer));

   xfer.tx_buf = (unsigned long) wdata;
   xfer.rx_buf = (unsigned long) rdata;
   xfer.len = len;

   /* Potential override fields
    xfer.speed_hz =      spi->speed_hz;
    xfer.delay_usecs =   spi->delay_usecs;
    xfer.bits_per_word = spi->bits_per_word;
    xfer.cs_change =     spi->cs_change;
    */

   status = ioctl(spi->handle, SPI_IOC_MESSAGE(1), &xfer);

   if (status < 0)
   {
      Tr_Fault_1("SPI Transfer Error %d", status);
   }

   return (status >= 0);
}

/*
 * SPI Master Transmit  (read data is ignored)
 *    Blocks until write finishes
 */
bool_t SPI_Write(uint8_t channel, const void *wdata, size_t num_bytes)
{
   bool_t success = true;

#if SPI_SUPPORT_BIT_BANG
   if (channel == SPI_BB_CHANNEL)
   {
      success = SPI_BB_Write(wdata, num_bytes);
   }
   else
#endif
   {
      success = Do_Transfer(Get_SPI(channel), wdata, NULL, num_bytes);
   }

   return success;
}

/*
 * SPI Master Recieve  (transmit data is fixed)
 *    Blocks until read finishes
 */
bool_t SPI_Read(uint8_t channel, void *rdata, size_t num_bytes)
{
   bool_t success;

#if SPI_SUPPORT_BIT_BANG
   if (channel == SPI_BB_CHANNEL)
   {
      success = SPI_BB_Read(rdata, num_bytes);
   }
   else
#endif
   {
      success = Do_Transfer(Get_SPI(channel), NULL, rdata, num_bytes);
   }

   return success;
}

/*
 * SPI Master Parallel Transmit / Recieve
 *    Blocks until transfer finishes
 */
bool_t SPI_Transfer(uint8_t channel, const void *wdata, void *rdata, size_t num_bytes)
{
   bool_t success;

#if SPI_SUPPORT_BIT_BANG
   if (channel == SPI_BB_CHANNEL)
   {
      success = SPI_BB_Transfer(wdata, rdata, num_bytes);
   }
   else
#endif
   {
      success = Do_Transfer(Get_SPI(channel), wdata, rdata, num_bytes);
   }

   return success;
}

/*===========================================================================*/
/*!
 * @file spi_linux.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 17-Oct-2011 Dan Carman
 *    - Added implicit locks on open.  Different drivers are used for same SPI Bus with different chip selects
 *
 *  - 17-Nov-2010 Dan Carman
 *   - Initial Linux wrapper
 */
/*===========================================================================*/
