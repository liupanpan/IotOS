#ifndef UART_H
#   define UART_H
/*===========================================================================*/
/**
 * @file uart.h
 *
 * UART Interface
 *
 * %full_filespec:uart.h~kok_basa#7:incl:kok_aud#13 %
 * @version %version:kok_basa#7 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Mon Oct 17 12:36:15 2011 %
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
 * The file defines the API to access UART channel 
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
 * @defgroup uart_grp UART interface
 * @ingroup base_drivers_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "reuse.h"
#   include "uart_acfg.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * UART Parity type
 */
typedef enum
{
   UA_NO_PARITY,
   UA_ODD_PARITY,
   UA_EVEN_PARITY,
   NUM_OF_UA_PARITY_OPTIONS
} UA_Parity_T;

/**
 * UART Number of stop bits
 */
typedef enum
{
   UA_1_STOP_BIT,
   UA_2_STOP_BITS,
   NUM_OF_UA_STOP_BITS_OPTIONS
} UA_Stop_Bits_T;

/**
 * UART Handshake type.
 */
typedef enum UA_Handshake_Tag
{
   UA_HANDSHAKE_NONE = 0,

#   ifdef UA_SW_HANDSHAKE_IS_SUPPORTED
   UA_HANDSHAKE_SW,
#   endif

#   ifdef UA_HW_HANDSHAKE_IS_SUPPORTED
   UA_HANDSHAKE_HW,
   UA_HANDSHAKE_HW_SW,
#   endif

   NUM_OF_UA_HANDSHAKE_OPTIONS
} UA_Handshake_T;

/**
 * UART Number of data bits
 */
typedef enum
{
   UA_7_DATA_BITS,
   UA_8_DATA_BITS,
   NUM_OF_UA_DATA_BITS_OPTIONS
} UA_Data_Bits_T;

/**
 * Configuration Options for UART 
 */
typedef struct UA_Attribute_Tag
{
   UA_Data_Bits_T data_bits;     /**< Size of data */
   UA_Stop_Bits_T stop_bits;     /**< Number of stop bits */
   UA_Parity_T parity;           /**< Byte parity */
   uint32_t bit_rate;            /**< Bit rate of uart channel @note The driver will try to match the request baudrate. */
   UA_Handshake_T handshake;     /**< Handshake type */
   size_t rx_buf_size;           /**< Size of RX Buffer (0 = no rx) */
   size_t tx_buf_size;           /**< Size of TX Buffer (0 = no tx) */
} UA_Attribute_T;

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/
#   ifdef __cplusplus
extern "C"
{          /* ! Place all function prototypes inside these braces ! */
#   endif  /* __cplusplus */
   
/**
 * Open an uart channel for communication.
 *
 * @param channel uart channel
 * @param attribute pointer to configuration parameter structure
 *
 * @return if (channel open okay) return the open channel, else UA_CHANNEL_INVALID.
 *
 * @note Channel is flushed automatically upon open.
 *
 * @see UA_Flush(), UA_Close()
 */
UA_Channel_T UA_Open(UA_Channel_T channel, const UA_Attribute_T * attribute);

/**
 * Close an uart channel.
 *
 * @param channel uart channel
 *
 * @return void
 *
 * @note
 *
 * @see UA_Flush(), UA_Close()
 */
void UA_Close(UA_Channel_T channel);

/**
 * Flush the uart channel of inputs.
 *
 * @param channel uart channel
 *
 * @return void
 *
 * @note
 *
 * @see UA_Flush(), UA_Open()
 */
void UA_Flush(UA_Channel_T channel);

/**
 * Get a byte from the uart channel that is received.
 *
 * @param channel uart channel
 *
 * @return if (received byte available) return next received byte, else EOF.
 *
 * @note
 *
 * @see UA_Open(), UA_Putc()
 */
int32_t UA_Getc(UA_Channel_T channel);

/**
 * Queue a byte to the uart channel for transmit
 *
 * @param channel uart channel
 * @param tx_byte byte to transmit
 *
 * @return if (byte queued for transmit okay) return byte sent, else EOF.
 *
 * @note  Channel must be open for communication.
 *
 * @see UA_Open(), UA_Getc()
 */
int32_t UA_Putc(UA_Channel_T channel, uint8_t tx_byte);

/**
 * Get an array of byte(s) from the uart channel
 *
 * @param channel uart channel
 * @param buf buffer to put received bytes
 * @param size max size of buf.
 *
 * @return Number of bytes received in buf.
 *
 * @note  Channel must be open for communication.
 *
 * @see UA_Open(), UA_Puts()
 */
int32_t UA_Gets(UA_Channel_T channel, uint8_t * buf, size_t size);

/**
 * Queue an array of byte(s) to the uart channel for transmit.
 *
 * @param channel uart channel
 * @param buf buffer to get bytes for transmission.
 * @param size max size of buf.
 *
 * @return Number of bytes queued for transmit.
 *
 * @note  Channel must be open for communication.
 *
 * @see UA_Open(), UA_Gets()
 */
int32_t UA_Puts(UA_Channel_T channel, const uint8_t * buf, size_t size);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}         /* end of extern "C" - function prototypes must precede this brace */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file uart.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 23-Feb-2010 Larry Piekarski Rev 5
 *   + SCR 723: Renamed function parameter to tx_byte to avoid gcc compiler warning.
 * 
 * 2009-aug-19 Kris Boultbee
 * + Added extern C wrapper around all function prototypes.
 * 
 * 2007-sep-28 kirk bailey
 * + Got rid of brief directive so that brief comment appears with version
 * + 1.5.3 of Doxygen.
 *
 * 18-June-2007 Dan Carman
 * + Merge bb_uart_control into base drivers as bb_uart
 *
 * 14-may-2007 henry yu
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* UART_H */
