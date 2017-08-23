/*===========================================================================*/
/**
 * @file bit_array_find.c
 *
 * Implements search routines for bit arrays
 *
 * %full_filespec: bit_array_find.c~kok_basa#9:csrc:kok_aud#4 %
 * @version %version: kok_basa#9 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified: Mon Jul 12 12:42:50 2010 %
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
 * Implements search routines for bit arrays
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRA TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Utilities.doc
 *
 *   - Requirements Document(s):
 *     - SRS_BASA_Utilities.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "bit_array.h"
#include "pbc_trace.h"
#include "reuse.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 1); /**< IDs file for EM assert handling */

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
 * Find and clear the first active bit from an LS Bit array
 */
int32_t Bit_Find_First_LSB(void *addr, size_t num_bits, bool_t consume_bit)
{
   uint8_t *bit_ptr = (uint8_t *) addr;
   size_t byte_index;
   uint_fast8_t bit_index;
   uint8_t bit_mask;

   PBC_Require(addr != NULL, "Bit_Find_First_LSB called with NULL pointer");

   for (byte_index = 0; (byte_index * 8) < num_bits; byte_index++)
   {
      if (0 != *bit_ptr)        /* is byte non-zero? */
      {
         bit_mask = 0x01;
         for (bit_index = 0; bit_index < 8; bit_index++)        /* search for bit within byte */
         {
            if (0 != (bit_mask & *bit_ptr))     /* find it? */
            {
               if (consume_bit)
               {
                  *bit_ptr &= (uint8_t) ~ bit_mask;     /* clear bit */
               }

               if (((byte_index * 8) + bit_index) < num_bits)
               {
                  return (int32_t) ((byte_index * 8) + bit_index);
               }
               else
               {
                  return (-1);  /* bit was past last valid bit in bit_ptr */
               }
            }
            bit_mask <<= 1;
         }
      }
      bit_ptr++;
   }
   return (-1);                 /* no bits set */
}

/*
 * Find and clear the first active bit from an MS Bit array
 */
int32_t Bit_Find_First_MSB(void *addr, size_t num_bits, bool_t consume_bit)
{
   uint8_t *bit_ptr = (uint8_t *) addr;
   size_t byte_index;
   uint_fast8_t bit_index;
   uint8_t bit_mask;

   PBC_Require(addr != NULL, "Bit_Find_First_MSB called with NULL pointer");

   for (byte_index = 0; (byte_index * 8) < num_bits; byte_index++)
   {
      if (0 != *bit_ptr)        /* is byte non-zero? */
      {
         bit_mask = 0x80;
         for (bit_index = 0; bit_index < 8; bit_index++)        /* search for bit within byte */
         {
            if (0 != (bit_mask & *bit_ptr))     /* find it? */
            {
               if (consume_bit)
               {
                  *bit_ptr &= (uint8_t) ~ bit_mask;     /* clear bit */
               }

               if (((byte_index * 8) + bit_index) < num_bits)
               {
                  return (int32_t) ((byte_index * 8) + bit_index);
               }
               else
               {
                  return (-1);  /* bit was past last valid bit in bit_ptr */
               }
            }
            bit_mask >>= 1;
         }
      }
      bit_ptr++;
   }
   return (-1);                 /* no bits set */
}

/*
 * Finds next set bit in direction specified of a LS Bit Array
 */
int32_t Bit_Find_Next_Bit_LSB(const void *addr, size_t num_bits, int32_t current_bit, bool_t up)
{
   size_t bits_tried = 0;
   int32_t increment = (up ? 1 : -1);

   PBC_Require(addr != NULL, "Bit_Find_Next_Bit_LSB called with NULL pointer");

   do
   {
      current_bit += increment;
      if (current_bit < 0)
      {
         current_bit = (int32_t) (num_bits - 1);
      }
      else if (current_bit >= (int32_t) num_bits)
      {
         current_bit = 0;
      }

      bits_tried++;
   }
   while ((!Bit_Is_True_LSB(addr, (size_t) current_bit)) && (bits_tried < num_bits));

   if (bits_tried >= num_bits)
   {
      current_bit = -1;         /* no bits set */
   }

   return (current_bit);
}

/*
 * Finds next set bit in direction specified of a MS Bit Array
 */
int32_t Bit_Find_Next_Bit_MSB(const void *addr, size_t num_bits, int32_t current_bit, bool_t up)
{
   size_t bits_tried = 0;
   int32_t increment = (up ? 1 : -1);

   PBC_Require(addr != NULL, "Bit_Find_Next_Bit_MSB called with NULL pointer");

   do
   {
      current_bit += increment;
      if (current_bit < 0)
      {
         current_bit = (int32_t) (num_bits - 1);
      }
      else if (current_bit >= (int32_t) num_bits)
      {
         current_bit = 0;
      }

      bits_tried++;
   }
   while ((!Bit_Is_True_MSB(addr, (size_t) current_bit)) && (bits_tried < num_bits));

   if (bits_tried >= num_bits)
   {
      current_bit = -1;         /* no bits set */
   }

   return (current_bit);
}



/*===========================================================================*/
/*!
 * @file bit_array_find.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 9
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 17-Jun-2009 Larry Piekarski Rev 7-8
 *   - Peer Review Updates
 *
 * - 08-Jun-2009 Larry Piekarski Rev 5
 *   - Added Bit_Find_First_MSB & Bit_Find_Next_Bit_MSB functions.
 *
 * - 27-May-2009 Larry Piekarski Rev 4
 *   - Cleaned up code from review checklist
 *
 * - 12-Oct-2007 Dan Carman
 *   - QAC / MISRA changes
 *
 * - 19-Jun-2007 Bartosz Jamrozik
 *   - Changed variable types in functions Bit_Find_Next_Bit_LSB and Bit_Find_First_LSB
 *     to conform with coding standard.
 *
 * - 04 May 2007  Dan Carman
 *   - shift of bit mask in Bit_Find_First_LSB was one level too deep.
 *
 * - 1 Feb 2007  Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
