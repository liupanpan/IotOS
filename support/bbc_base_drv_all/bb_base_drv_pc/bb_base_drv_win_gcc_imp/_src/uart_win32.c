/*===========================================================================*/
/**
 * @file uart_win32.c
 *
 * @brief Implements the UART API for WIN32
 *
 * %full_filespec:uart_win32.c~kok_basa#6:csrc:kok_aud#1 %
 * @version %version:kok_basa#6 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Nov 15 17:47:19 2010 %
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
 * Implements the UART API for WIN32 based on posix standard terminal I/O
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
 * @addtogroup uart_grp
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include <stddef.h>
#include <stdio.h>              /* Standard input/output definitions */
#include <string.h>             /* String function definitions */
#include <unistd.h>             /* UNIX standard function definitions */
#include <fcntl.h>              /* File control definitions */
#include <termios.h>            /* POSIX terminal control definitions */

#include "reuse.h"
#include "pbc_trace.h"
#include "uart.h"

#include "uart_cfg.h"            /* mapping of UART channel IDs to device_names */

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(BASE_DRIVERS_MODULE_ID, 6);

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/
#define INVALID_FILE_DESCRIPTOR  0

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/** 
 * Struct to hold conversion from numerical baud rate to posix baud rate parameter 
 */
typedef struct baud_rate_conversion_Tag
{
   uint32_t baud_rate;           /**< numerical baud rate (e.g. 115200) */
   uint32_t posix_value;         /**< posix equilavent parameter (e.g. B115200) */
} baud_rate_conversion_T;

/**
 * Structure to hold channel to device name mapping
 */
typedef struct channel_map_Tag
{
   UA_Channel_T  channel;       /**< numerical baud rate (e.g. 115200) */
   char const * device_name;         /**< posix equilavent parameter (e.g. B115200) */
} channel_map_T;

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/**
 * Holds OS file descriptor for open channels
 */
static int file_descriptor[NUM_OF_UA_CHANNEL];


/**
 * Conversion table from baud rate to posix bit rate values
 */
static const baud_rate_conversion_T bit_rate[] = {
   {0, B0},                     /* 0 baud (drop DTR) */
   {50, B50},
   {75, B75},
   {110, B110},
   {134, B134},                 /* 134.5 baud */
   {150, B150},
   {200, B200},
   {300, B300},
   {600, B600},
   {1200, B1200},
   {1800, B1800},
   {2400, B2400},
   {4800, B4800},
   {9600, B9600},
   {19200, B19200},
   {38400, B38400},
   {57600, B57600},
   {115200, B115200},
   {128000, B128000},
   {230400, B230400},
   {256000, B256000},
};


#define UART_Define_Channel(chan, dev) {chan, dev},

/** Define mapping between channel and device name */
static const channel_map_T channel_map[] =
{
   UART_Define_Channels
};

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/
static uint32_t ua_get_posix_bit_rate(uint32_t baud_rate);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

static char const * device_name(UA_Channel_T channel)
{
   uint_fast8_t i;
   
   for (i = 0; i < Num_Elems(channel_map); i++)
   {
      if (channel_map[i].channel == channel)
      {
         return channel_map[i].device_name;
      }
   }
   
   PBC_Failed_1("Unsupported UART Channel %d", channel);
   return NULL;
}

/**
 * Converts requested baud rate into posix control value 
 *
 * @post baud rate must match value conversion 
 */
static uint32_t ua_get_posix_bit_rate(uint32_t baud_rate)
{
   uint_fast8_t i;

   for (i = 0; ((i < Num_Elems(bit_rate)) && (baud_rate != bit_rate[i].baud_rate)); i++)
   {
   }

   PBC_Ensure_1(i < Num_Elems(bit_rate), "Invalid baud rate %d", baud_rate);

   return bit_rate[i].posix_value;
}


UA_Channel_T UA_Open(UA_Channel_T channel, const UA_Attribute_T * attribute)
{
   struct termios options;
   UA_Channel_T return_value = (UA_Channel_T) -1;

   PBC_Require_1(channel < NUM_OF_UA_CHANNEL, "Invalid channel %d", channel);
   PBC_Require((NULL != attribute), "NULL Attribute");
   PBC_Require_1(NUM_OF_UA_DATA_BITS_OPTIONS > attribute->data_bits, "invalid number of data bits %d", attribute->data_bits);
   PBC_Require_1(NUM_OF_UA_PARITY_OPTIONS > attribute->parity, "Invalid Parity %d", attribute->parity);
   PBC_Require_1(NUM_OF_UA_STOP_BITS_OPTIONS > attribute->stop_bits, "Invalid Stop Bits %d", attribute->stop_bits);
   PBC_Require_1(NUM_OF_UA_HANDSHAKE_OPTIONS > attribute->handshake, "Invalid Handshake %d", attribute->handshake);

   file_descriptor[channel] = open(device_name(channel), (O_RDWR | O_NDELAY));

   PBC_Ensure_1(file_descriptor[channel] > 0, "UART open failed %d", file_descriptor[channel]);

   return_value = channel;

   tcgetattr(file_descriptor[channel], &options);
   cfsetispeed(&options, ua_get_posix_bit_rate(attribute->bit_rate));
   cfsetospeed(&options, ua_get_posix_bit_rate(attribute->bit_rate));

   switch (attribute->parity)
   {
      case UA_NO_PARITY:
         options.c_cflag &= ~PARENB;
         break;

      case UA_ODD_PARITY:
         options.c_cflag |= PARENB;
         options.c_cflag &= ~PARODD;
         options.c_iflag |= (INPCK | ISTRIP);
         break;

      case UA_EVEN_PARITY:
         options.c_cflag |= PARENB;
         options.c_cflag |= PARODD;
         options.c_iflag |= (INPCK | ISTRIP);
         break;

      default:
         break;
   }

   switch (attribute->stop_bits)
   {
      case UA_1_STOP_BIT:
         options.c_cflag &= ~CSTOPB;
         break;

      case UA_2_STOP_BITS:
         options.c_cflag |= CSTOPB;
         break;

      default:
         break;
   }

   options.c_cflag &= ~CSIZE;   /* Mask the character size bits */

   switch (attribute->data_bits)
   {
      case UA_7_DATA_BITS:
         options.c_cflag |= CS7;
         break;

      case UA_8_DATA_BITS:
         options.c_cflag |= CS8;
         break;

      default:
         break;
   }

   if (0 == (UA_HANDSHAKE_SW & attribute->handshake))
   {
      options.c_iflag &= ~(IXON | IXOFF | IXANY);       /* SW control OFF */
   }
   else
   {
      options.c_iflag |= (IXON | IXOFF | IXANY);        /* SW control ON */
   }

#ifdef UA_HW_HANDSHAKE_IS_SUPPORTED
   if (0 == (UA_HANDSHAKE_HW & attribute->handshake))
   {
      options.c_cflag &= ~CNEW_RTSCTS;  /* HW control OFF */
   }
   else
   {
      options.c_cflag |= CNEW_RTSCTS;   /* HW control ON */
   }
#endif

   options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* input raw mode */
   options.c_oflag &= ~OPOST;   /* output raw mode */

   tcsetattr(file_descriptor[channel], TCSAFLUSH, &options);

   return (return_value);
}

void UA_Close(UA_Channel_T channel)
{
   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);

   (void) close(file_descriptor[channel]);
   
   file_descriptor[channel] = INVALID_FILE_DESCRIPTOR;
}

void UA_Flush(UA_Channel_T channel)
{
   int32_t value;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);

   do
   {
      value = UA_Getc(channel);
   }
   while (EOF != value);
}

int32_t UA_Getc(UA_Channel_T channel)
{
   int32_t return_value = EOF;
   uint8_t buf;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);

   /* channel must be open */
   PBC_Require_1(INVALID_FILE_DESCRIPTOR < file_descriptor[channel], "Invalid File Descriptor %d", file_descriptor[channel]);

   if (0 < read(file_descriptor[channel], &buf, 1))
   {
      return_value = buf;
   }

   return (return_value);
}

int32_t UA_Putc(UA_Channel_T channel, uint8_t tx_byte)
{
   int32_t return_value = EOF;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);

   /* channel must be open */
   PBC_Require_1(INVALID_FILE_DESCRIPTOR < file_descriptor[channel], "Invalid File Descriptor %d", file_descriptor[channel]);

   if (0 < write(file_descriptor[channel], (void *)&tx_byte, 1))
   {
      return_value = tx_byte;
   }

   return (return_value);
}

int32_t UA_Gets(UA_Channel_T channel, uint8_t * buf, size_t size)
{
   int32_t byte_count;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);
   PBC_Require(NULL != buf, "NULL buffer");
   /* channel must be open */
   PBC_Require_1(INVALID_FILE_DESCRIPTOR < file_descriptor[channel], "Invalid File Descriptor %d", file_descriptor[channel]);

   byte_count = (int32_t) read(file_descriptor[channel], buf, size);

   /* return 0 if no bytes read */
   if (byte_count < 0)
   {
      byte_count = 0;
   }

   return (byte_count);
}

int32_t UA_Puts(UA_Channel_T channel, const uint8_t * buf, size_t size)
{
   int32_t byte_count;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);
   PBC_Require(NULL != buf, "NULL buffer");
   /* channel must be open */
   PBC_Require_1(INVALID_FILE_DESCRIPTOR < file_descriptor[channel], "Invalid File Descriptor %d", file_descriptor[channel]);

   byte_count = (int32_t) write(file_descriptor[channel], buf, size);

   return (byte_count);
}

/*===========================================================================*/
/*!
 * @file uart_win32.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 * 03-Mar-2010 Larry Piekarski Rev 5
 *  + SCR 1027: Fixed error where parameter was renamed from byte to tx_byte, 
 *              but the usage of that parameter in the function was not updated.
 *
 * 23-Feb-2010 Larry Piekarski Rev 4
 *  + Renamed function parameter from byte to tx_byte to fix wgcc warning.
 *
 * 21-June-2007 Dan Carman
 *  + Fixed UA_Gets to always return 0 if no data is available
 *    was return -1 / EOF if no input was available
 *  + Implemented UA_Flush
 *
 * 15-June-2007 Dan Carman
 * + Merged bb_uart_control into base drivers as bb_uart
 * + switched to standard attribute types
 * + Added DbC support
 *
 * 22-may-2007 henry yu
 * + Added calls to win32 gcc.
 *
 * 09-may-2007 henry yu
 * + Created initial file uart_win32.
 */
/*===========================================================================*/
