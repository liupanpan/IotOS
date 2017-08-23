/*===========================================================================*/
/**
 * @file convert_endian.c
 *
 * Access routines to read / write fixed endian data
 *
 * %full_filespec: convert_endian.c~6:csrc:kok_aud#1 %
 * @version %version: 6 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified: Tue Jul  7 09:42:21 2009 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Access routines to read / write fixed endian data
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRA TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Utilities.doc
 *
 *   - Requirements Document(s):
 *     - SRS_BASA_Utilities
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

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "convert_endian.h"
#include "pbc_trace.h"
#include "reuse.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 4); /**< IDs file for EM assert handling */

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/*
 * Writes native endian data to Big Endian output
 */
void Util_Put_Big_Endian(uint8_t * dest, const uint32_t * src, size_t num_bytes)
{
   PBC_Require((num_bytes >= 1) &&
               (num_bytes <= 4), "Util_Put_Big_Endian only converts values of 1 to 4 bytes in length");
   PBC_Require((dest != NULL) && (src != NULL), "Util_Put_Big_Endian cannot be called with a NULL pointer");

   for (; 0 != num_bytes; num_bytes--)
   {
      *(dest++) = (uint8_t) ((*src) >> ((num_bytes - 1) << 3));
   }
}

/*
 * Writes native endian data to Little Endian output
 */
void Util_Put_Little_Endian(uint8_t * dest, const uint32_t * src, size_t num_bytes)
{
   size_t i;

   PBC_Require((num_bytes >= 1) &&
               (num_bytes <= 4), "Util_Put_Little_Endian only converts values of 1 to 4 bytes in length");
   PBC_Require((dest != NULL) && (src != NULL), "Util_Put_Little_Endian cannot be called with a NULL pointer");

   for (i = 0; i < num_bytes; i++)
   {
      *(dest++) = (uint8_t) ((*src) >> (i << 3));
   }
}

/*
 * Reads Big Endian data into native endian format
 */
void Util_Get_Big_Endian(uint32_t * dest, const uint8_t * src, size_t num_bytes)
{
   PBC_Require((num_bytes >= 1) &&
               (num_bytes <= 4), "Util_Get_Big_Endian only converts values of 1 to 4 bytes in length");
   PBC_Require((dest != NULL) && (src != NULL), "Util_Get_Big_Endian cannot be called with a NULL pointer");

   *dest = 0;
   for (; 0 != num_bytes; num_bytes--)
   {
      *(dest) |= (uint32_t) ((*(src++)) << ((num_bytes - 1) << 3));
   }
}

/*
 * Reads Little Endian data into native endian format
 */
void Util_Get_Little_Endian(uint32_t * dest, const uint8_t * src, size_t num_bytes)
{
   size_t i;

   PBC_Require((num_bytes >= 1) &&
               (num_bytes <= 4), "Util_Get_Little_Endian only converts values of 1 to 4 bytes in length");
   PBC_Require((dest != NULL) && (src != NULL), "Util_Get_Little_Endian cannot be called with a NULL pointer");

   *dest = 0;
   for (i = 0; i < num_bytes; i++)
   {
      *(dest) |= (uint32_t) ((*(src++)) << (i << 3));
   }
}

/*
 * Returns Big Endian data in native endian unsigned 16 bit format
 */
uint16_t Util_Get_Big_Endian_U16(const uint8_t * src)
{
   PBC_Require(src != NULL, "Util_Get_Big_Endian_U16 cannot be called with a NULL pointer");

   return ((uint16_t) ((((uint16_t) src[0]) << 8) | (uint16_t) src[1]));
}

/*
 * Returns Big Endian data in native endian unsigned 24 bit format
 */
uint32_t Util_Get_Big_Endian_U24(const uint8_t * src)
{
   PBC_Require(src != NULL, "Util_Get_Big_Endian_U24 cannot be called with a NULL pointer");

   return ((uint32_t) ((((uint32_t) src[0]) << 16) | (((uint32_t) src[1]) << 8) | (uint32_t) src[2]));
}

/*
 * Returns Big Endian data in native endian unsigned 32 bit format
 */
uint32_t Util_Get_Big_Endian_U32(const uint8_t * src)
{
   PBC_Require(src != NULL, "Util_Get_Big_Endian_U32 cannot be called with a NULL pointer");

   return ((uint32_t)
           ((((uint32_t) src[0]) << 24) | (((uint32_t) src[1]) << 16) | (((uint32_t) src[2]) << 8) | (uint32_t)
            src[3]));
}

/*
 * Returns Little Endian data in native endian unsigned 16 bit format
 */
uint16_t Util_Get_Little_Endian_U16(const uint8_t * src)
{
   PBC_Require(src != NULL, "Util_Get_Little_Endian_U16 cannot be called with a NULL pointer");

   return ((uint16_t) ((uint16_t) src[0] | (((uint16_t) src[1]) << 8)));
}

/*
 * Returns Little Endian data in native endian unsigned 24 bit format
 */
uint32_t Util_Get_Little_Endian_U24(const uint8_t * src)
{
   PBC_Require(src != NULL, "Util_Get_Little_Endian_U24 cannot be called with a NULL pointer");

   return ((uint32_t) ((uint32_t) src[0] | (((uint32_t) src[1]) << 8) | (((uint32_t) src[2]) << 16)));
}

/*
 * Returns Little Endian data in native endian unsigned 32 bit format
 */
uint32_t Util_Get_Little_Endian_U32(const uint8_t * src)
{
   PBC_Require(src != NULL, "Util_Get_Little_Endian_U32 cannot be called with a NULL pointer");

   return ((uint32_t)
           ((uint32_t) src[0] | (((uint32_t) src[1]) << 8) | (((uint32_t) src[2]) << 16) |
            (((uint32_t) src[3]) << 24)));
}

/*
 * Writes native endian unsigned 16 bit data to Little Endian output
 */
void Util_Put_Little_Endian_U16(uint8_t * dest, uint16_t src)
{
   PBC_Require(dest != NULL, "Util_Put_Little_Endian_U16 cannot be called with a NULL pointer");

   dest[0] = (uint8_t) ((src >> 0) & 0xFFu);
   dest[1] = (uint8_t) ((src >> 8) & 0xFFu);
   return;
}

/*
 * Writes native endian unsigned 24 bit data to Little Endian output
 */
void Util_Put_Little_Endian_U24(uint8_t * dest, uint32_t src)
{
   PBC_Require(dest != NULL, "Util_Put_Little_Endian_U24 cannot be called with a NULL pointer");

   dest[0] = (uint8_t) ((src >> 0) & 0xFFu);
   dest[1] = (uint8_t) ((src >> 8) & 0xFFu);
   dest[2] = (uint8_t) ((src >> 16) & 0xFFu);
   return;
}

/*
 * Writes native endian unsigned 32 bit data to Little Endian output
 */
void Util_Put_Little_Endian_U32(uint8_t * dest, uint32_t src)
{
   PBC_Require(dest != NULL, "Util_Put_Little_Endian_U32 cannot be called with a NULL pointer");

   dest[0] = (uint8_t) ((src >> 0) & 0xFFu);
   dest[1] = (uint8_t) ((src >> 8) & 0xFFu);
   dest[2] = (uint8_t) ((src >> 16) & 0xFFu);
   dest[3] = (uint8_t) ((src >> 24) & 0xFFu);
   return;
}

/*
 * Writes native endian unsigned 16 bit data to Big Endian output
 */
void Util_Put_Big_Endian_U16(uint8_t * dest, uint16_t src)
{
   PBC_Require(dest != NULL, "Util_Put_Big_Endian_U16 cannot be called with a NULL pointer");

   dest[0] = (uint8_t) ((src >> 8) & 0xFFu);
   dest[1] = (uint8_t) ((src >> 0) & 0xFFu);
   return;
}

/*
 * Writes native endian unsigned 24 bit data to Big Endian output
 */
void Util_Put_Big_Endian_U24(uint8_t * dest, uint32_t src)
{
   PBC_Require(dest != NULL, "Util_Put_Big_Endian_U24 cannot be called with a NULL pointer");

   dest[0] = (uint8_t) ((src >> 16) & 0xFFu);
   dest[1] = (uint8_t) ((src >> 8) & 0xFFu);
   dest[2] = (uint8_t) ((src >> 0) & 0xFFu);
   return;
}

/*
 * Writes native endian unsigned 32 bit data to Big Endian output
 */
void Util_Put_Big_Endian_U32(uint8_t * dest, uint32_t src)
{
   PBC_Require(dest != NULL, "Util_Put_Big_Endian_U32 cannot be called with a NULL pointer");

   dest[0] = (uint8_t) ((src >> 24) & 0xFFu);
   dest[1] = (uint8_t) ((src >> 16) & 0xFFu);
   dest[2] = (uint8_t) ((src >> 8) & 0xFFu);
   dest[3] = (uint8_t) ((src >> 0) & 0xFFu);
   return;
}

/*===========================================================================*/
/*!
 * @file convert_endian.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 27-May-2009 Larry Piekarski Rev 4-6
 *   - Cleaned up code from review checklist
 *
 * - 12-Oct-2007 Dan Carman
 *   - QAC / MISRA changes
 *
 * - 16-May-2007 Klaus Krosse
 *   - in Util_Get_Big_Endian() and Util_Get_Little_Endian() initialize dest with 0
 *   - added type size specific endian conversion functions which are easier to use
 *     in many applications.
 *
 * - 3-Apr-2007  Klaus Krosse
 *   - Get and put endian functions did not work for big endian machines like
 *     SH2-A for 3 byte (24bit) values and for 2 bytes if source was a 32 bit 
 *     integer. Changed all endian functions to use shifting instead of byte 
 *     swapping or byte moving.
 *     Changed types of src and dest parameter from void to integer types.
 *
 * - 1 Feb 2007  Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
