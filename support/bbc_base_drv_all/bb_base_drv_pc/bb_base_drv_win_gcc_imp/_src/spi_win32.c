/*===========================================================================*/
/**
 * @file spi_win32.c
 *
 * implements simple 3-wire bit-bang SPI master interface
 *
 * %full_filespec:spi_win32.c~2:csrc:kok_basa#1 %
 * @version %version:2 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Mon Oct 17 13:16:48 2011 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Simulates SPI interface by reading / writing to files
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
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
 
#include <fcntl.h> 
#include "pbc_trace.h"
#include "reuse.h"
#include "spi.h"
#include "spi_cfg.h"
#include <stdio.h>
#include <unistd.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/


/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 6);  /* define this file for assert handling */

#define PERMS  0666

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

typedef struct SPI_Channel_Name_Tag
{
   uint8_t channel;
   const char * input_name;
   const char * output_name;
} SPI_Channel_Name_T;

typedef struct SPI_Attributes_Tag
{
   int in_fd;
   int out_fd;
} SPI_Attributes_T;

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

#define SPI_Define_Channel(chan, in, out) {chan, in, out},

static const SPI_Channel_Name_T spi_names[SPI_NUM_CHANNELS] =
{
   SPI_Define_Channels
};

SPI_Attributes_T spi_attributes[SPI_NUM_CHANNELS];

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

static int Get_SPI_Index(uint8_t channel)
{
   uint_fast8_t i;

   for (i = 0; (i < SPI_NUM_CHANNELS) && (spi_names[i].channel != channel); i++)
   {
      /* do nothing here */
   }

   PBC_Ensure_1(i < SPI_NUM_CHANNELS, "Invalid SPI channel %d", channel);

   return i;
}

/*
 * Initializes SPI Master channel
 */
int8_t SPI_Open(uint8_t channel, const SPI_Config_T * config)
{
   uint_fast8_t i = Get_SPI_Index(channel);

   (void) config;

   if ( spi_names[i].input_name != NULL)
   {
      spi_attributes[i].in_fd = open(spi_names[i].input_name, O_RDONLY, PERMS);
      PBC_Ensure_2(spi_attributes[i].in_fd >= 0, "Error opening SPI channel %d Input File %s", channel, spi_names[i].input_name);
   }
   else
   {
      spi_attributes[i].in_fd = -1;
   }

   if ( spi_names[i].output_name != NULL)
   {
      spi_attributes[i].out_fd = creat(spi_names[i].output_name, PERMS);
      PBC_Ensure_2(spi_attributes[i].out_fd >= 0, "Error opening SPI channel %d Output File %s", channel, spi_names[i].output_name);
   }
   else
   {
      spi_attributes[i].out_fd = -1;
   }

   return channel;
}

/**
 * Shuts down SPI channel
 * @param channel - Hardware specific channel identifer
 */
void SPI_Close(uint8_t channel)
{
   uint_fast8_t i = Get_SPI_Index(channel);

   if (spi_attributes[i].in_fd >= 0)
   {
      (void) close(spi_attributes[i].in_fd);
   }

   if (spi_attributes[i].out_fd >= 0)
   {
      (void) close(spi_attributes[i].out_fd);
   }
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
bool_t SPI_Write(uint8_t channel, const void *wdata, size_t num_bytes)
{
   int bytes;
   uint_fast8_t spi = Get_SPI_Index(channel);
   bool_t success = true;
   
#if SPI_SIM_ASCII   
   int i;
   uint8_t * data = (uint8_t *) wdata;
   char buf[4] = {0,0,0,0};

   for (i = 0; i < num_bytes; i++)
   {
      snprintf(buf, 4, "%02X ", *data++);
      bytes = write(spi_attributes[spi].out_fd, buf, 3);
      success &= (bytes == 3);            
   }
   bytes = write(spi_attributes[spi].out_fd, "/r/n", 2);
#else
   bytes = write(spi_attributes[spi].out_fd, wdata, num_bytes);
   success = (bytes == num_bytes);
#endif 
   return success;
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
bool_t SPI_Read(uint8_t channel, void *rdata, size_t num_bytes)
{
   int bytes;
   uint_fast8_t i = Get_SPI_Index(channel);

   bytes = read(spi_attributes[i].in_fd, rdata, num_bytes);

   return (bytes == num_bytes);
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
bool_t SPI_Transfer(uint8_t channel, const void *wdata, void *rdata, size_t num_bytes)
{
   bool_t success;
    
   success = SPI_Write(channel, wdata, num_bytes);
   success &= SPI_Read(channel, rdata, num_bytes);
   
   return success;
}

/*===========================================================================*/
/*!
 * @file spi_win32.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 *    16-Nov-2010 Dan Carman
 *       + Created initial version
 */
/*===========================================================================*/
