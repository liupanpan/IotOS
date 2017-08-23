/*===========================================================================*/
/**
 * @file hex_string.c
 *
 * Conversion function between binary data block and hex strings
 *
 * %full_filespec:hex_string.c~kok_inf#9:csrc:kok_aud#1 %
 * @version %version:kok_inf#9 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Jul 31 14:51:58 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * This module converts persistent data between binary form and null terminated hex string form
 *    String inclues one byte two complement checksum of binary data
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
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
#include "reuse.h"
#include "pbc_trace.h"
#include <string.h>
#include "hex_string.h"

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 8);    /* Identifies file for PbC/trace */

/**
 * Initial value for value checksum
 */
#define HS_CHKSUM_INIT    (0)

/**
 *  Using 1's complement chksum, therefore sum with data and chksum should always be -1
 */
#define HS_VALID_CHKSUM (0xFF)

/**
 * Hex String overhead 
 *    two hex digits for one byte checksum
 */
#define HS_STRING_OVERHEAD (sizeof(Hex_Byte_T))

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/
static Hex_Byte_T Bin_To_Hex(uint8_t x);
static uint8_t Hex_To_Bin(Hex_Byte_T hc, bool_t * error);

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/


/**
 * Convert one binary byte value into two packed ASCII Hex Digits
 *
 * @param x - byte value to convert
 *
 * @return Two Packed ASCII Hex Digits
 */
static Hex_Byte_T Bin_To_Hex(uint8_t x)
{
   Hex_Byte_T hc;

   if (x < 0xA0)
   {
      hc = '0' + (x >> 4);
   }
   else
   {
      hc = 'A' + ((x >> 4) - 0x0A);
   }

   x &= 0x0F;

   if (x < 0x0A)
   {
      hc |= ('0' + x) << 8;
   }
   else
   {
      hc |= ('A' + (x - 0x0A)) << 8;
   }

   return hc;
}

/*
 * Converts data arry to ASCII string with NULL termination.
 * Please refer to the detailed description in hex_string.h.
 */
void Bin_To_Hex_String(const uint8_t * data_buf, Hex_Byte_T * str_buf, size_t bin_size)
{
   size_t i;
   uint8_t checksum = HS_CHKSUM_INIT;

   for (i = 0; i < bin_size; i++)
   {
      checksum += *data_buf;
      *str_buf++ = Bin_To_Hex(*data_buf++);
   }

/* add checksum to end of string */
   *str_buf++ = Bin_To_Hex(~checksum);

/* add terminator */
   *str_buf = 0;
}

/**
 * Convert two packed ASCII chars into one binary byte value
 *    LSB of packed chars is MSN of byte 
 *    MSB of packed chars is LSN of byte
 *
 * @param hc - 2 packed ASCII Hex Digits (legal characters '0' - '9', 'A' - 'F')
 * @param error [out] - true if source has invalid ASCII Hex Digits (legal characters '0' - '9', 'A' - 'F')
 *
 * @return - binary byte value 
 */
static uint8_t Hex_To_Bin(Hex_Byte_T hc, bool_t * error)
{
   uint8_t c;                   /* Hex character */
   uint8_t b = 0;               /* byte value */

   c = (hc >> 8) & 0xFF;

   if ((c >= '0') && (c <= '9'))
   {
      b = c - '0';
   }
   else if ((c >= 'A') && (c <= 'F'))
   {
      b = c - ('A' - 0x0A);
   }
   else
   {
      Tr_Fault_1("Invalid Hex Digit %c", c);
      *error = true;
   }

   c = hc & 0xFF;

   if ((c >= '0') && (c <= '9'))
   {
      b |= (c - '0') << 4;
   }
   else if ((c >= 'A') && (c <= 'F'))
   {
      b |= (c - ('A' - 0x0A)) << 4;
   }
   else
   {
      Tr_Fault_1("Invalid Hex Digit %c", c);
      *error = true;
   }

   return b;
}

/*
 * Converts ASCII string to data array.
 * Please refer to the detailed description in hex_string.h.
 */
bool_t Hex_String_To_Bin(Hex_Byte_T const *str_buf, uint8_t * data_buf, ssize_t bin_size,size_t *read_size)
{
	size_t i;
	uint8_t b;
	uint8_t checksum = HS_CHKSUM_INIT;
	bool_t valid_string = false;
	bool_t conversion_error = false;
	bool ok = false;
	size_t copy_size=0;

	/*copy data only if positive dest_size matches stored sized exactly or the absolute
	value of a negative dest_size is greater or equal to stored data size*/
	if (bin_size >= 0)
	{
		ok = Hex_String_Size(bin_size) == strlen((char *)str_buf);
	}
	else
	{
		ok = ((size_t)(-Hex_String_Int_Size(bin_size)))  >= strlen((char *)str_buf);
	}

	if(ok)
	{
		if (bin_size >= 0)
		{
			copy_size = bin_size;
		}
		else
		{
			copy_size = (strlen((char *)str_buf) - HS_STRING_OVERHEAD)/ sizeof(Hex_Byte_T);
		}
		for (i = 0; (i < copy_size); i++)
		{
			b = Hex_To_Bin(*str_buf++, &conversion_error);
			*data_buf++ = b;
			checksum += b;
		}
		/*store the size of data actually read*/
		*read_size = copy_size;
		
		checksum += Hex_To_Bin(*str_buf, &conversion_error);

		if (HS_VALID_CHKSUM == checksum)
		{
			valid_string = !conversion_error;
		}
		else
		{
			Tr_Fault_2("Invalid Checksum : %u Value : %s", checksum, (const char *)str_buf);
		}
	}
	else
	{
		Tr_Fault_2("Wrong read length, Data Size : %d Value : %s", (int)bin_size, (const char *)str_buf);
	}

	return valid_string;
}

/* 
 * Gives the expected hex string length of a binary block without terminator
 */
size_t Hex_String_Size(size_t bin_size)
{
   return (bin_size * sizeof(Hex_Byte_T)) + HS_STRING_OVERHEAD;
}
/* 
 * Gives the expected hex string length of a binary block without terminator
 */
ssize_t Hex_String_Int_Size(ssize_t bin_size)
{
   return (bin_size * sizeof(Hex_Byte_T)) + HS_STRING_OVERHEAD;
}

/*===========================================================================*/
/*!
 * @file hex_string.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 20-March-2009 Dan Carman
 *   - Created initial file.
 *
 * - 18-Nov-2009 Kirk Bailey
 *   - Task kok_aud 59600: Fixed CR/CR/LF issue
 *
 * - 21-apr-2010 Kirk Bailey
 *   - Changed trace file number to a unique value.
 *
 * - 12-Jul-2010 Larry Piekarski Rev 6
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 15-Apr-2011 Vijayalakshmi Rev 7
 *   - Modified Hex_String_To_Bin() for persistent calls
 *
 * - 20-Sep-2013 Kirk Bailey Rev 8
 *   - Task 1164: Fixed warnings due to enabling of -Wextra
 *
 * - 31-Jul-2014 Kirk Bailey Rev 9
 *   - Fixed warnings in 64-bit Linux build
 */
/*===========================================================================*/
