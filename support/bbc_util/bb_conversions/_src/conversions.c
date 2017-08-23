/*===========================================================================*/
/**
 * @file conversions.c
 *
 * Implements generic functions for conversions
 *
 * %full_filespec:  conversions.c~kok_basa#3:csrc:tck_aud#1 %
 * @version %version:  kok_basa#3 %
 * @author  %derived_by: czhswm %
 * @date    %date_modified: Mon Jul 12 12:42:51 2010 %
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
 * Generic functions for conversions
 *
 * @section ABBR ABBREVIATIONS:
 *   - BCD - Binary Coded Decimal
 *
 * @section TRA TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Utilities.doc
 *
 *   - Requirements Document(s):
 *     - SRS_Utilities.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None.
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

#include "conversions.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
/**
 * Max number of BCD digits in a single number
 */
#define MAX_BCD_DIGITS_NUM ((size_t)8)

/**
 * Largest BCD digit in any position 
 */
#define MAX_BCD_DIGIT ((uint32_t)9)

/**
 * Number of bits per nibble
 */
#define BITS_PER_NIBBLE ((uint8_t)4)

/**
 * Mask for lowest nibble (lowest 4 bits)
 */
#define LOWEST_NIBBLE_MASK ((uint32_t)0x0000000F)

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
 * The function converts a received BCD value to hexidecimal value.
 */
uint32_t BCD_To_Hex(const uint32_t BCD_Value, size_t num_digits)
{
   uint32_t result = 0;
   uint8_t shift_by = 0;
   uint32_t multiply_by = 1;
   uint32_t nibble_mask = LOWEST_NIBBLE_MASK;

   /* num_digits parameter check */
   if ((num_digits < 1) || (num_digits > MAX_BCD_DIGITS_NUM))
   {
      /* use max range if parameter is out of reasonable range */
      num_digits = MAX_BCD_DIGITS_NUM;
   }
   for (; num_digits > 0; num_digits--)
   {
      /*
         result =
         (BCD_Value & 0x0000000F)
         + ((BCD_Value & 0x000000F0) >>  4)  * 10
         + ((BCD_Value & 0x00000F00) >>  8)  * 100
         + ((BCD_Value & 0x0000F000) >> 12) * 1000)
         + ((BCD_Value & 0x000F0000) >> 16) * 10000)
         + ((BCD_Value & 0x00F00000) >> 20) * 100000)
         + ((BCD_Value & 0x0F000000) >> 24) * 1000000)
         + ((BCD_Value & 0xF0000000) >> 28) * 10000000);
       */
      result += (((BCD_Value & nibble_mask) >> shift_by) * multiply_by);
      shift_by += BITS_PER_NIBBLE;
      multiply_by *= 10;
      nibble_mask = nibble_mask << BITS_PER_NIBBLE;
   }

   return result;

}


/*
 * The function converts hexidecimal to a BCD value.
 *
 * num_digits - decimal limit for input HexValue
 * 1              - 9
 * 2              - 99
 * 3              - 999
 * 4              - 9999
 * 5              - 99999
 * 6              - 999999
 * 7              - 9999999
 * 8              - 99999999
 */
uint32_t Hex_To_BCD(const uint32_t HexValue, size_t num_digits)
{
   uint32_t result = 0;
   uint32_t result_limited = 0;
   uint32_t limit = 0;
   uint32_t bcd_divisor = 1;
   uint32_t hex_multiplier = 0x00000001;

   /* num_digits parameter check */
   if ((num_digits < 1) || (num_digits > MAX_BCD_DIGITS_NUM))
   {
      /* use max range if parameter is out of reasonable range */
      num_digits = MAX_BCD_DIGITS_NUM;
   }

   for (; num_digits > 0; num_digits--)
   {
      /* calculate limit for input value */
      limit = limit * 10 + MAX_BCD_DIGIT;
      /*pre-calculate result value for given limit */
      result_limited = (result_limited * 0x10) + MAX_BCD_DIGIT;

      /*convert hex to bcd value */
      /*
         //                       modulo_divisor, divisor, multiplier
         //  modulo_divisor = 10* divisor, multiplier += multiplier*0x10,
         result = (
         (((HexValue % 100000000) / 10000000) * 0x10000000) +
         (((HexValue % 10000000)  / 1000000)  * 0x1000000) +
         (((HexValue % 1000000)   / 100000)   * 0x100000) +
         (((HexValue % 100000)    / 10000)    * 0x10000) +
         (((HexValue % 10000)     / 1000)     * 0x1000) +
         (((HexValue % 1000)      / 100)      * 0x100) +
         (((HexValue % 100)       / 10)       * 0x10) +
         (((HexValue % 10)        / 1)        * 0x01)
         );
       */
      result += (((HexValue % (bcd_divisor * 10)) / bcd_divisor) * hex_multiplier);
      hex_multiplier *= 0x10;
      bcd_divisor *= 10;

   }


   if (HexValue >= limit)
   {
      result = result_limited;
   }


   return result;

}


/*
 * This routine tests if input value is in BCD value.
 */
bool_t Is_BCD(uint32_t Potential_BCD_Value)
{
   bool_fast result = true;
   size_t nibble_no;

   /* check each nibble is max MAX_BCD_DIGIT */
   for (nibble_no = 0; nibble_no < MAX_BCD_DIGITS_NUM; nibble_no++)
   {
      if (((Potential_BCD_Value >> (BITS_PER_NIBBLE * nibble_no)) & LOWEST_NIBBLE_MASK) > MAX_BCD_DIGIT)
      {
         result = false;
      }
   }

   return result;
}

/*
 * The function increments a received BCD value and wraps at the specified number of BCD digits
 */
uint32_t BCD_Increment(uint32_t BCD_value, size_t num_digits)
{
   uint32_t result;
   uint32_t nibble_mask = LOWEST_NIBBLE_MASK;

   if (false == Is_BCD(BCD_value))
   {
      result = UINT32_MAX;
   }
   else
   {

      /* num_digits parameter check */
      if ((num_digits < 1) || (num_digits > MAX_BCD_DIGITS_NUM))
      {
         /* use max range if parameter is out of reasonable range */
         num_digits = MAX_BCD_DIGITS_NUM;
      }

      result = BCD_value;

      while (num_digits > 0)
      {
         num_digits--;

         if (MAX_BCD_DIGIT == (BCD_value & LOWEST_NIBBLE_MASK))
         {
            /* clear current nibble -> 9 incremented becomes 0 */
            result &= ~nibble_mask;
         }
         else
         {
            /* add 1 and finish [to get 1 at correct position change Fh to 1h in the mask] */
            result += (nibble_mask & 0x11111111);
            num_digits = 0;
         }
         BCD_value >>= BITS_PER_NIBBLE;
         nibble_mask <<= BITS_PER_NIBBLE;

      }
   }

   return result;
}

/*
 * The function decrements a received BCD value wraps at the specified number of BCD digits
 */
uint32_t BCD_Decrement(uint32_t BCD_value, size_t num_digits)
{
   uint32_t result;
   uint32_t nibble_1 = 0x00000001;

   if (false == Is_BCD(BCD_value))
   {
      result = UINT32_MAX;
   }
   else
   {

      /* num_digits parameter check */
      if ((num_digits < 1) || (num_digits > MAX_BCD_DIGITS_NUM))
      {
         /* use max range if parameter is out of reasonable range */
         num_digits = MAX_BCD_DIGITS_NUM;
      }

      result = BCD_value;

      while (num_digits > 0)
      {
         num_digits--;

         if (0 == (BCD_value & LOWEST_NIBBLE_MASK))
         {
            /* clear current nibble -> 0 decremented becomes 9 */
            result += (nibble_1 * MAX_BCD_DIGIT);
         }
         else
         {
            /* substract 1 at current position and finish */
            result -= nibble_1;
            num_digits = 0;
         }
         BCD_value >>= BITS_PER_NIBBLE;
         nibble_1 <<= BITS_PER_NIBBLE;

      }
   }
   return result;
}

/*
 * Scales a value from one range to another.
 */
int32_t Scale(int32_t input, int32_t old_min, int32_t old_max, int32_t new_min, int32_t new_max)
{
   int32_t old_range = old_max - old_min;

   return (((((input - old_min) * (new_max - new_min)) + (old_range / 2)) / old_range) + new_min);
}

/*===========================================================================*/
/*!
 * @file conversions.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 3
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 27-May-2009 Larry Piekarski Rev 2
 *   - Cleaned up code from review checklist
 *
 * - 10-May-2007  Grzegorz Golabek
 *   - Created initial file.
 *
\*===========================================================================*/
