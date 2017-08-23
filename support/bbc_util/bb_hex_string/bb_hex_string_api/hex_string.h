#ifndef HEX_STRING_H
#   define HEX_STRING_H
/*===========================================================================*/
/**
 * @file hex_string.h
 *
 * Hex string conversion
 *
 * %full_filespec:hex_string.h~kok_basa#9:incl:kok_aud#1 %
 * @version %version:kok_basa#9 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Jan 26 11:08:24 2012 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page hex_string_page Hex String User Manual
 *
 * @section DESC DESCRIPTION:
 *
 * Routines to convert between block binary data and checksummed hex ASCII strings
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
 * @defgroup hex_string_grp Hex String Functions
 * @ingroup utilities_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/**
 * type to hold two packed hex ASCII digits representing one byte value
 */
typedef uint16_t Hex_Byte_T;

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */

/**
 * Converts binary data block to ASCII HEX string with NULL termination.
 *    Also includes a 1's complement checksum of the source data 
 *
 * @param data_buf - pointer to data to be converted
 * @param str_buf - Output: str_buf - pointer to write hex string
 * @param bin_size - size of data to be converted 
 */
void Bin_To_Hex_String(const uint8_t * data_buf, Hex_Byte_T * str_buf, size_t bin_size);

/**
 * Converts ASCII HEX string to data array.
 *    Only valid uppercase hex digits are supported (0-F) 
 *    String is assumed to end with a 1's complemented checksum of data block
 * 
 * @param str_buf - pointer to string to be converted
 * @param data_buf - pointer to write converted data
 * @param bin_size - size of data buffer
 * @param read_size - ptr. to were size of result is written.
 *
 * @return true if valid conversion 
 */
bool_t Hex_String_To_Bin(Hex_Byte_T const *str_buf, uint8_t * data_buf, ssize_t bin_size,size_t *read_size);

/**
 * Returns the expected length of Hex String including check values for a given binary size
 *
 * @param bin_size - size of binary block 
 * @return size of hex string representation
 */
size_t Hex_String_Size(size_t bin_size);
/**
 * Returns the expected length of Hex String including check values for a given binary size
 *
 * @param bin_size - size of binary block 
 * @return size of hex string representation
 */

ssize_t Hex_String_Int_Size(ssize_t bin_size);

#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/*===========================================================================*/
/*!
 * @file hex_string.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 26-Jan-2012 Kirk Bailey Rev 9
 *   - Added missing doxygen tag.
 *
 * - 15-Apr-2011 Vijayalakshmi Rev 7
 *   - Modified Hex_String_To_Bin() for persistent calls
 *
 * - 12-Jul-2010 Larry Piekarski Rev 7
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 21-apr-2010 Kirk Bailey Rev 6
 *   - Got rid of parallels.
 *
 * - 18-Nov-2009 Kirk Bailey
 *   - Task kok_aud 59600: Fixed CR/CR/LF issue
 *
 * - 20-may-2008 Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* HEX_STRING_H */
