/*===========================================================================*/
/**
 * @file bit_array.c
 *
 * Implements common bit array access routines
 *
 * %full_filespec: bit_array.c~kok_basa#8:csrc:kok_aud#4 %
 * @version %version: kok_basa#8 %
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
 * bit manipulation functions to manage a packed array of booleans.
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRA TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - SRS_BASA_Utilities
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DESC DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "bit_array.h"
#include "pbc_trace.h"
#include "reuse.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 0); /**< IDs file for EM assert handling */

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

/**
 * Returns a byte with a single bit set offset from the LSB
 */
#define lsb_bit_mask(offset)     (uint8_t)(1U << ((offset) % 8))

/**
 * Returns a byte with a single bit clear offset from the LSB
 */
#define lsb_nbit_mask(offset)    (uint8_t)(~(1U << ((offset) % 8)))

/**
 * Returns a byte with a single bit set offset from the MSB
 */
#define msb_bit_mask(offset)     (uint8_t)(1U << (7 - ((offset) % 8)))

/**
 * Returns a byte with a single bit clear offset from the MSB
 */
#define msb_nbit_mask(offset)    (uint8_t)(~(1U << (7 - ((offset) % 8))))

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
 *  Set a bit given an base address and a bit offset from MSBit
 */
void Bit_Set_MSB(void *addr, size_t offset, size_t num_bits, bool_t new_value)
{
   uint8_t *bit_ptr = (uint8_t *) addr;

   PBC_Require(addr != NULL, "Bit_Set_MSB called with NULL pointer");
   PBC_Require_2(offset < num_bits, "Bit_Set_MSB called with offset(%d) >= number of bits(%d)", (int)offset, (int)num_bits);

   if (new_value)
   {
      *(bit_ptr + (offset / 8)) |= msb_bit_mask(offset);
   }
   else
   {
      *(bit_ptr + (offset / 8)) &= msb_nbit_mask(offset);
   }
}

/*
 *  Set a bit given an base address and a bit offset from LSBit
 */
void Bit_Set_LSB(void *addr, size_t offset, size_t num_bits, bool_t new_value)
{
   uint8_t *bit_ptr = (uint8_t *) addr;

   PBC_Require(addr != NULL, "Bit_Set_LSB called with NULL pointer");
   PBC_Require_2(offset < num_bits, "Bit_Set_LSB called with offset(%d) >= number of bits(%d)", (int)offset, (int)num_bits);

   if (new_value)
   {
      *(bit_ptr + (offset / 8)) |= lsb_bit_mask(offset);
   }
   else
   {
      *(bit_ptr + (offset / 8)) &= lsb_nbit_mask(offset);
   }
}


/*
 *  Returns a bit given an base address and a bit offset from MSBit
 */
bool_t Bit_Is_True_MSB(const void *addr, size_t offset)
{
   const uint8_t *bit_ptr = (const uint8_t *)addr;

   PBC_Require(addr != NULL, "Bit_Is_True_MSB called with NULL pointer");

   return (0 != (*(bit_ptr + (offset / 8)) & msb_bit_mask(offset)));
}

/*
 *  Returns a bit given an base address and a bit offset from LSBit
 */
bool_t Bit_Is_True_LSB(const void *addr, size_t offset)
{
   const uint8_t *bit_ptr = (const uint8_t *)addr;

   PBC_Require(addr != NULL, "Bit_Is_True_LSB called with NULL pointer");

   return (0 != (*(bit_ptr + (offset / 8)) & lsb_bit_mask(offset)));
}

/*
 * Toogle a bit given an address and a bit offset from LSB.
 */
void Bit_Toggle_LSB(void *addr, size_t offset, size_t num_bits)
{
   uint8_t *bit_ptr = (uint8_t *) addr;

   PBC_Require(addr != NULL, "Bit_Toggle_LSB called with NULL pointer");
   PBC_Require_2(offset < num_bits, "Bit_Toggle_LSB called with offset(%d) >= number of bits(%d)", (int)offset, (int)num_bits);

   *(bit_ptr + (offset / 8)) ^= lsb_bit_mask(offset);
}

/*
 * Toogle a bit given an address and a bit offset from MSB.
 */
void Bit_Toggle_MSB(void *addr, size_t offset, size_t num_bits)
{
   uint8_t *bit_ptr = (uint8_t *) addr;

   PBC_Require(addr != NULL, "Bit_Toggle_MSB called with NULL pointer");
   PBC_Require_2(offset < num_bits, "Bit_Toggle_MSB called with offset(%d) >= number of bits(%d)", (int)offset, (int)num_bits);

   *(bit_ptr + (offset / 8)) ^= msb_bit_mask(offset);
}

/*===========================================================================*/
/*!
 * @file bit_array.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 8
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 17_Jun-2009 Larry Piekarski Rev 6-7
 *   - Peer Review Updates
 *
 * - 27-May-2009 Larry Piekarski Rev 3
 *   - Cleaned up code from review checklist
 *
 * - 23 Apr 2007  Dan Carman
 *   - Updated for new DbC API
 *
 * - 1 Feb 2007  Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
