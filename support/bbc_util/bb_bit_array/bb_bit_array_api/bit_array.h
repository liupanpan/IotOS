#ifndef BIT_ARRAY_H
#   define BIT_ARRAY_H
/*===========================================================================*/
/**
 * @file bit_array.h
 *
 * API for functions to access an array of packed bits
 *
 * %full_filespec:bit_array.h~kok_basa#9:incl:kok_aud#4 %
 * @version %version:kok_basa#9 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Mon Jul 12 12:42:49 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page bit_array_page Bit Array User Manual
 *
 * @section DESC DESCRIPTION:
 *
 * The bit handling sub-module contains set of functions used for bit operations.  
 * In order to maintain compatibility between machines with little and big endian 
 * it is very important to use only functions provided by utilities module in 
 * case implementing queues or manipulating flags
 *
 * @subsection LSB Least Significant Bit Order
 * While it seems obvious, the difference between LSB and MSB is not all that 
 * simple. This function reads and writes bits in a LSB fashion. The data is 
 * stored in a byte array of unsigned 8-bit values. A true bit for bit zero would 
 * be stored as a 0x01 in array[0]. A true bit for bit one would be stored as a 0x02 
 * in array[0]. The first 8 bits are all stored in array[0], and next 8 bits are 
 * all stored in array[1], etc.
 *
 * The LSB bit order looks like this:
 * @code
 *          MSB                                LSB
 *         +----+----+----+----+----+----+----+----+
 * byte[0] |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *         +----+----+----+----+----+----+----+----+
 *         +----+----+----+----+----+----+----+----+
 * byte[1] | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |
 *         +----+----+----+----+----+----+----+----+
 * etc...
 * @endcode
 *
 * @subsection MSB Most Significant Bit Order
 * This is, as you would expect, the opposite of LSB. However, ONLY the bit order
 * is changed, the bytes still progress from byte 0, to 1, etc. A true bit for bit
 * zero would be stored as a 0x80 in array[0], A trube bit for bit 1 would be 
 * stored as a 0x40 in array[0]. etc. 
 *
 * The MSB bit order looks like this:
 * @code
 *          MSB                                LSB
 *         +----+----+----+----+----+----+----+----+
 * byte[0] |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
 *         +----+----+----+----+----+----+----+----+
 *         +----+----+----+----+----+----+----+----+
 * byte[1] |  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 |
 *         +----+----+----+----+----+----+----+----+
 * etc...
 * @endcode
 *
 * @note These functions are intended to maintain an array of bits, like in a 
 * queue. These are not endian-friendly functions and the bits may not be stored
 * in memory in the order you expect. However, if you always use these functions
 * to access the bit arrays, then they will work. These functions are not 
 * intended to read, write, or modify data in a microprocesss register.
 *
 * @section USAGE USAGE INSTRUCTIONS:
 *
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 *
 * @section ABBR ABBREVIATIONS:
 *   - LSB - Least Significant Bit
 *   - MSB - Most Significant Bit
 *
 * @section TRAC TRACEABILITY INFO:
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
 * @section DEV DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup bit_array_grp Bit Array Functions
 * @ingroup utilities_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "reuse.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/
/**
 * Returns bytes size for a bit array
 *
 * @param num_bits  total number of bits in array
 *
 * @return number of bytes to store number of bits
 */
#   define BIT_ARRAY_NUM_BYTES(num_bits)     ((num_bits + 7) >> 3)

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/
#   ifdef __cplusplus
extern "C"
{

#   endif                       /* __cplusplus */
/**
 *  Set a bit given an base address and a bit offset from MSB
 *    - used to implement a packed array of booleans
 *    - offset 0 is bit 7 of byte 0
 *
 *  @param [out] addr address of object in which to set the bit
 *  @param [in] offset The bit offset, counted from the lowest address, MSB
 *  @param [in] num_bits   The number of valid bits in array
 *  @param [in] new_value  The new state of bit
 *
 *  @see Bit_Set_LSB(), Bit_Is_True_MSB(), Bit_Is_True_LSB()
 *
 * @pre
 *    - addr != NULL
 *    - offset < num_bits
 */
   void Bit_Set_MSB(void *addr, size_t offset, size_t num_bits, bool_t new_value);

/**
 *  Set a bit given an base address and a bit offset from LSB
 *    - used to implement a packed array of booleans
 *    - offset 0 is bit 0 of byte 0
 *
 *  @param [out] addr address of object in which to set the bit
 *  @param [in] offset The bit offset, counted from the lowest address, LSB
 *  @param [in] num_bits   The number of valid bits in array
 *  @param [in] new_value  The new state of bit
 *
 *  @see Bit_Set_LSB(), Bit_Is_True_MSB(), Bit_Is_True_LSB()
 *
 * @pre
 *    - addr != NULL
 *    - offset < num_bits
 */
   void Bit_Set_LSB(void *addr, size_t offset, size_t num_bits, bool_t new_value);

/**
 * Returns a bit given a base address and a bit offset from MSB
 *   - used to implement a packed array of booleans
 *   - offset 0 is bit 7 of byte 0
 *
 * @param [out] addr address of object in which to set the bit
 * @param [in] offset The bit offset, counted from the lowest address, MSB
 *
 * @return true if bit is 1, false if bit is 0
 *
 * @see Bit_Set_LSB(), Bit_Is_True_MSB(), Bit_Is_True_LSB()
 *
 * @pre
 *    - addr != NULL
 */
   bool_t Bit_Is_True_MSB(const void *addr, size_t offset);

/**
 * Returns a bit given a base address and a bit offset from LSB
 *   - used to implement a packed array of booleans
 *   - offset 0 is bit 0 of byte 0
 *
 * @param [out] addr address of object in which to set the bit
 * @param [in] offset The bit offset, counted from the lowest address, LSB
 *
 * @return true if bit is 1, false if bit is 0
 *
 * @see Bit_Set_LSB(), Bit_Is_True_MSB(), Bit_Is_True_LSB()
 *
 * @pre
 *    - addr != NULL
 */
   bool_t Bit_Is_True_LSB(const void *addr, size_t offset);

/**
 * Find and optionally clear the first active bit from an LS Bit array. The
 * array is searched from least significant bit to most significant bit of 
 * byte 0, then least significant bit to most significant bit of byte 1, etc.
 * Example:
 * 
 * @code
 * array = 0x00 0x16 =
 *         +-----------+
 * byte[0] | 0000 0000 |
 *         +-----------+
 * byte[1] | 0001 0110 |
 *         +-----------+
 * @endcode
 * Bit_Find_First_LSB(&array, 16, false) = 9, and array is unchanged.
 *
 * or
 *
 * Bit_Find_First_LSB(&array, 16, true) = 9, and array is 0x00 0x14 on exit.
 *
 * @param [in] addr     The address of the array
 * @param [in] num_bits The size of the array in bits (not bytes)
 * @param [in] consume_bit if true the bit is cleared
 *
 * @return - (-1) if no bit is set,
 *    - otherwise the offset, in bits, from the MSB of the lowest addressed byte in the array
 *
 * @pre
 *    - addr != NULL
 */
   int32_t Bit_Find_First_LSB(void *addr, size_t num_bits, bool_t consume_bit);

/**
 * Find and clear the first active bit from an MS Bit array. The
 * array is searched from most significant bit to least significant bit of 
 * byte 0, then most significant bit to least significant bit of byte 1, etc.
 * Example:
 * 
 * @code
 * array = 0x00 0x16 =
 *         +-----------+
 * byte[0] | 0000 0000 |
 *         +-----------+
 * byte[1] | 0001 0110 |
 *         +-----------+
 * @endcode
 * Bit_Find_First_MSB(&array, 16, false) = 11, and array is unchanged.
 *
 * or
 *
 * Bit_Find_First_MSB(&array, 16, true) = 11, and array is 0x00 0x06 on exit.
 *
 * @param [in] addr     The address of the array
 * @param [in] num_bits The size of the array in bits (not bytes)
 * @param [in] consume_bit if true the bit is cleared
 *
 * @return - (-1) if no bit is set,
 *    - otherwise the offset, in bits, from the MSB of the lowest addressed byte in the array
 *
 * @pre
 *    - addr != NULL
 */
   int32_t Bit_Find_First_MSB(void *addr, size_t num_bits, bool_t consume_bit);

/**
 * Finds next set bit in direction specified of a LS Bit Array. If the last bit is
 * reached without finding a match, the search will resume with the first bit and
 * will only end when a match is found or current_bit is reached.
 *
 * @param [in] addr     The address of the array
 * @param [in] num_bits The size of the array in bits (not bytes)
 * @param [in] current_bit The bit position to start from (does not look at starting position)
 * @param [in] up - True = look in up direction (increasing offset)
 *
 * @return -1 if no bit is set, otherwise the offset, in bits, of the located
 *            bit (relative to bit 0).
 *
 * @pre
 *    - addr != NULL
 */
   int32_t Bit_Find_Next_Bit_LSB(const void *addr, size_t num_bits, int32_t current_bit, bool_t up);

/**
 * Finds next set bit in direction specified of a MS Bit Array. If the last bit is
 * reached without finding a match, the search will resume with the first bit and
 * will only end when a match is found or current_bit is reached.
 *
 * @param [in] addr     The address of the array
 * @param [in] num_bits The size of the array in bits (not bytes)
 * @param [in] current_bit The bit position to start from (does not look at starting position)
 * @param [in] up - True = look in up direction (increasing offset)
 *
 * @return -1 if no bit is set, otherwise the offset, in bits, of the located
 *            bit (relative to bit 0).
 *
 * @pre
 *    - addr != NULL
 */
   int32_t Bit_Find_Next_Bit_MSB(const void *addr, size_t num_bits, int32_t current_bit, bool_t up);

/**
 * Toggle a bit given an address and a bit offset from LSB.
 * Used to implement a packed array of booleans
 *
 * @param [in,out] addr     The address of the array
 * @param [in]     offset   the bit offset, counted from the lowest address, LSB
 * @param [in]     num_bits The number of valid bits in array
 *
 * @pre
 *    - addr != NULL
 *    - offset < num_bits
 */
   void Bit_Toggle_LSB(void *addr, size_t offset, size_t num_bits);

/**
 * Toggle a bit given an address and a bit offset from MSB.
 * Used to implement a packed array of booleans.
 *
 * @param [in,out] addr     The address of the array
 * @param [in]     offset   the bit offset, counted from the lowest address, LSB
 * @param [in]     num_bits The number of valid bits in array
 *
 * @pre
 *    - addr != NULL
 *    - offset < num_bits
 */
   void Bit_Toggle_MSB(void *addr, size_t offset, size_t num_bits);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file bit_array.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 9
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 21-apr-2010 Kirk Bailey Rev 8
 *   - SCR 1297 - Improved comments for "Find_Next" functions.
 *
 * - 08-Jul-2009 Larry Piekarski Rev 7
 *   - Peer Review Updates
 *
 * - 08-Jun-2009 Larry Piekarski Rev 6
 *   - Added Bit_Find_First_MSB & Bit_Find_Next_Bit_MSB functions.
 *
 * - 27-May-2009 Larry Piekarski Rev 5
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 4
 *   - Updated Doxygen Comments
 *
 * - 14-Jun-2007 Bartosz Jamrozik Rev 3
 *   - Changed variable types in functions Bit_Find_Next_Bit_LSB and Bit_Find_First_LSB
 *     to conform with coding standard.
 *
 * - 22-May-2007 Bartosz Jamrozik Rev 2
 *   - Added function prototypes
 *     - Bit_Toggle_LSB
 *     - Bit_Toggle_LSB
 *   - Comments updated
 *
 * - 02-Feb-2007 Dan Carmen Rev 1
 *   - Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif                          /* BIT_ARRAY_H */
