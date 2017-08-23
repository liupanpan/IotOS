/*===========================================================================*/
/**
 * @file uart_qnx.c
 *
 * @brief Implements the UART API for qnx Host
 *
 * %full_filespec:uart_qnx.c~3:csrc:kok_inf#1 %
 * @version %version:3 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Oct 20 14:46:59 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Implements the UART API for qnx based on posix standard terminal I/O
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
 * @addtogroup uart_grp
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include <stddef.h>
#include <stdio.h>              /* Standard input/output definitions */
#include <stdlib.h>
#include <string.h>             /* String function definitions */
#include <unistd.h>             /* UNIX standard function definitions */
#include <fcntl.h>              /* File control definitions */
#include <termios.h>            /* POSIX terminal control definitions */
#include <errno.h>
#include <sys/select.h>
#include "reuse.h"
#include "pbc_trace.h"
#include "uart.h"
#include "uart_cfg.h"            /* mapping of UART channel IDs to device_names */

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
#undef EM_F_ID
#define EM_F_ID EM_FILE_ID(BASE_DRIVERS_MODULE_ID, 2) /**< Identifies file for PbC/trace */

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/
#define INVALID_FILE_DESCRIPTOR  -1

/**
 * Default size for rx buffer
 */
#ifndef UART_RX_BUF_SIZE
#define UART_RX_BUF_SIZE  64
#endif

/**
 * Default timeout in seconds on receive with no data
 */
#ifndef UART_PORT_TIME_OUT
#ifndef UART_RX_TIMEOUT_S
#define UART_PORT_TIME_OUT(X)  5
#else
#define UART_PORT_TIME_OUT(X)  UART_RX_TIMEOUT_S
#endif
#endif



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

/**
 * Receive data buffer
 */
typedef  struct UA_Rx_Buffer_Tag
{
   uint8_t * buffer;       /**< holds receive data */
   uint8_t const * pdata;  /**< pointer to next rx byte */
   size_t bytes;           /**< unread data in buffer */
} UA_Rx_Buffer_T;

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

static UA_Rx_Buffer_T Rx_Buf[NUM_OF_UA_CHANNEL];

/**
 * Conversion table from baud rate to posix bit rate values
 */
static const baud_rate_conversion_T bit_rate[] = {
   {0,         B0},                     /* 0 baud (drop DTR) */
   {50,        B50},
   {75,        B75},
   {110,       B110},
   {134,       B134},                 /* 134.5 baud */
   {150,       B150},
   {200,       B200},
   {300,       B300},
   {600,       B600},
   {1200,      B1200},
   {1800,      B1800},
   {2400,      B2400},
   {4800,      B4800},
   {9600,      B9600},
   {19200,     B19200},
   {38400,     B38400},
   {57600,     B57600},
   {115200,    B115200},
   {230400,    230400L},
   {460800,    460800L},
   {500000,    500000L},
   {576000,    576000L},
   {921600,    921600L},
   {1000000,   1000000L},
   {1152000,   1152000L},
   {1500000,   1500000L},
   {2000000,   2000000L},
   {2500000,   2500000L},
   {3000000,   3000000L},
   {3500000,   3500000L},
   {4000000,   4000000L},
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

   memset(&options, 0, sizeof(options));

   PBC_Require_1(channel < NUM_OF_UA_CHANNEL, "Invalid channel %d", channel);
   PBC_Require((NULL != attribute), "NULL Attribute");
   PBC_Require_1(NUM_OF_UA_DATA_BITS_OPTIONS > attribute->data_bits, "invalid number of data bits %d", attribute->data_bits);
   PBC_Require_1(NUM_OF_UA_PARITY_OPTIONS > attribute->parity, "Invalid Parity %d", attribute->parity);
   PBC_Require_1(NUM_OF_UA_STOP_BITS_OPTIONS > attribute->stop_bits, "Invalid Stop Bits %d", attribute->stop_bits);
   PBC_Require_1(NUM_OF_UA_HANDSHAKE_OPTIONS > attribute->handshake, "Invalid Handshake %d", attribute->handshake);

   file_descriptor[channel] = open(device_name(channel), (O_RDWR | O_NDELAY));

   PBC_Ensure_1(file_descriptor[channel] >= 0, "UART open failed %d", file_descriptor[channel]);

   return_value = channel;

   /* Create empty receive buffer */
   Rx_Buf[channel].buffer = (uint8_t * )malloc(UART_RX_BUF_SIZE);
   Rx_Buf[channel].bytes = 0;
   PBC_Ensure_1(Rx_Buf[channel].buffer != NULL, "Unable to Allocate Channel %d RX Buffer", channel);

   tcgetattr(file_descriptor[channel], &options);
   cfsetispeed(&options, ua_get_posix_bit_rate(attribute->bit_rate));
   cfsetospeed(&options, ua_get_posix_bit_rate(attribute->bit_rate));

   switch (attribute->parity)
   {
      case UA_NO_PARITY:
         options.c_cflag &= ~PARENB;
         options.c_iflag &= ~(ISTRIP);
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

   options.c_cflag &= ~CSIZE; /* Mask the character size bits */

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
      options.c_iflag &= ~(IXON | IXOFF | IXANY); /* SW control OFF */
   }
   else
   {
      options.c_iflag |= (IXON | IXOFF | IXANY); /* SW control ON */
   }

#ifdef UA_HW_HANDSHAKE_IS_SUPPORTED
   if (0 == (UA_HANDSHAKE_HW & attribute->handshake))
   {
      options.c_cflag &= ~CNEW_RTSCTS; /* HW control OFF */
   }
   else
   {
      options.c_cflag |= CNEW_RTSCTS; /* HW control ON */
   }
#endif

   options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | INLCR | IGNCR | ICRNL); /* input raw mode */
   options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHONL | IEXTEN); /* input raw mode */
   options.c_oflag &= ~OPOST; /* output raw mode */

   tcsetattr(file_descriptor[channel], TCSAFLUSH, &options);

   /* change to blocking mode */
   fcntl(file_descriptor[channel], F_SETFL, fcntl(file_descriptor[channel], F_GETFL) | O_NONBLOCK);

   return (return_value);
}

void UA_Close(UA_Channel_T channel)
{
   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);

   (void) close(file_descriptor[channel]);
   file_descriptor[channel] = INVALID_FILE_DESCRIPTOR;

   if (Rx_Buf[channel].buffer != NULL)
   {
      free(Rx_Buf[channel].buffer);
      memset(&Rx_Buf[channel], 0, sizeof(Rx_Buf[channel]));
   }
}

void UA_Flush(UA_Channel_T channel)
{
   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);

   tcflush(file_descriptor[channel], TCIFLUSH);
   Rx_Buf[channel].bytes = 0;
}

/**
 * Read UART device
 *    if wait then use select to wait for data to become available
 *    Read all available data into buffer
 *
 *    @return the number of bytes read
 */
static int ua_Read(UA_Channel_T channel, bool_t wait, uint8_t * buf, size_t bytes)
{
   int nread = 0;
   int ret = 1;

   if (wait)
   {
      fd_set fds;
      struct timeval timeout;

      FD_ZERO(&fds);
      FD_SET(file_descriptor[channel], &fds);

      /* set timeout value within input loop */
      timeout.tv_usec = 0; /* milliseconds */
      timeout.tv_sec = UART_PORT_TIME_OUT(channel); /* seconds */

      /* Block until data is available */
      ret = select(file_descriptor[channel] + 1, &fds, NULL, NULL, &timeout);
   }

   if (-1 == ret)
   {
      Tr_Warn_2("UART %d Select Error, %s", channel, strerror(errno));
   }
   else if (0 == ret) /* timeout expired */
   {
      Tr_Info_Mid_1("UART %d Rx Timeout", channel);
   }
   else /* else, ret > 0, data available */
   {
      /* Read all (up to buffer size) data in one go */
      nread = read(file_descriptor[channel], buf, bytes);
      if (nread < 0)
      {
         Tr_Warn_2("UART %d Read Error, %s", channel, strerror(errno));
         nread = 0;
      }
   }

   return nread;
}

/**
 * Read a data block from receive buffer
 *
 * @return is number of bytes read (either the requested size or the number that was in buffer)
 */
static size_t ua_Read_Buf(uint8_t ** dest, UA_Rx_Buffer_T * src, size_t size)
{
   if (src->bytes < size)
   {
      size = src->bytes;
   }

   if (size > 0)
   {
      memcpy(*dest, src->pdata, size);
      src->pdata += size;
      *dest += size;
      src->bytes -= size;
   }
   return size;
}

int32_t UA_Getc(UA_Channel_T channel)
{
   int32_t return_value = EOF;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);
   PBC_Require_1(Rx_Buf[channel].buffer != NULL, "UART channel %d not opened", channel);

   if (0 == Rx_Buf[channel].bytes)
   {
      Rx_Buf[channel].pdata = Rx_Buf[channel].buffer;
      Rx_Buf[channel].bytes = ua_Read(channel, true, Rx_Buf[channel].buffer, UART_RX_BUF_SIZE);
   }

   if (Rx_Buf[channel].bytes > 0)
   {
      return_value = *Rx_Buf[channel].pdata++;
      Rx_Buf[channel].bytes--;
   }

   return (return_value);
}

int32_t UA_Putc(UA_Channel_T channel, uint8_t tx_byte)
{
   int32_t return_value = EOF;
   int32_t byte_count;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);
   /* channel must be open */
   PBC_Require_1(Rx_Buf[channel].buffer != NULL, "UART %d not open", channel);

   byte_count = write(file_descriptor[channel], (void *) &tx_byte, 1);
   if (byte_count > 0)
   {
      return_value = tx_byte;
   }
   else if (byte_count < 0)
   {
      Tr_Fault_3("Error writing to UART, errno: %d, byte_count: %d, error description: %s", errno, byte_count, strerror(errno));
   }
   else
   {
      Tr_Warn("No bytes written to UART.");
   }
   return (return_value);
}

int32_t UA_Gets(UA_Channel_T channel, uint8_t * buf, size_t size)
{
   size_t byte_count = 0;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);
   PBC_Require(NULL != buf, "NULL buffer");
   /* channel must be open */
   PBC_Require_1(Rx_Buf[channel].buffer != NULL, "UART %d not open", channel);

   /* get any existing data from buffer */
   byte_count = ua_Read_Buf(&buf, &Rx_Buf[channel], size);

   /* if more is requested then go read it directly into callers buffer */
   if (size > byte_count)
   {
      byte_count += ua_Read(channel, (0 == byte_count), buf, (size - byte_count));
   }

   return byte_count;
}

int32_t UA_Puts(UA_Channel_T channel, const uint8_t * buf, size_t size)
{
   int32_t byte_count;

   PBC_Require_1(NUM_OF_UA_CHANNEL > channel, "Invalid channel %d", channel);
   PBC_Require(NULL != buf, "NULL buffer");
   /* channel must be open */
   PBC_Require_1(Rx_Buf[channel].buffer != NULL, "UART %d not open", channel);

   byte_count = (int32_t) write(file_descriptor[channel], buf, size);
   if (byte_count < 0)
   {
      /*indicates an error*/
      Tr_Fault_3("Error writing to UART, errno: %d, byte_count: %d, error description: %s", errno, byte_count, strerror(errno));
   }
   else if (byte_count == 0)
   {
      Tr_Warn("No bytes written to UART");
   }
   return (byte_count);
}

/*===========================================================================*/
/*!
 * @file uart_qnx.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 20-Oct-2014 Dan Carman
 *   Update dest pointer when pulling partial read from cache.
 *
 * 09-Nov_2010 Dan Carman
 *  + Modified to suporrt configurable device paths
 *
 * 14Dec10  David Origer (hz1941)  Rev 2
 * SCR kok_aud#4967 : Make sure UART is completely initialized for raw mode.
 *
 * 25Apr12  Brian Harmison (gzq2ns)  Rev 3
 * SCR 24842 - add in additional logging on failure of UART send/receive.
 *
 * 27-June-2012 Dan Carman
 *   - Modified to buffer received data on App Side to reduce number of reads from kernel
 *   UART driver configuration was changed to be non-blocking
 *   Get functions block only when no data is available
 *
 * 31-jul-2013 Franco Arias (kzlfpq) rev 7
 * + kok_basa#162391 include a form to configure the uart time out
 *
 * - 26-Sep-2013 Larry Piekarski Rev 8
 *   - Task 1331: Merged branched revisions created due to DCM to kok_inf. 
 *                Reversed version control comments - no functional change
 *
 * - 26-Sep-2013 Larry Piekarski Rev 9
 *   - Task 1331: Fixed compiler warnings
 */
/*===========================================================================*/
