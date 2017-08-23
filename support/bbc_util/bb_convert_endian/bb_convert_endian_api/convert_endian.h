#ifndef CONVERT_ENDIAN_H
#   define CONVERT_ENDIAN_H
/*===========================================================================*/
/**
 * @file convert_endian.h
 *
 * API for functions that copy data to specific endianess
 *
 * %full_filespec:convert_endian.h~kok_basa#7:incl:kok_aud#1 %
 * @version %version:kok_basa#7 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed Apr 21 10:10:34 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page convert_endian_page Endian Conversion User Manual
 *
 * @section DESC DESCRIPTION:
 *
 * In computing, endianness is the byte ordering in memory used to represent some 
 * kind of data. Typical cases are the order in which integer values are stored 
 * as bytes in computer memory (relative to a given memory addressing scheme) 
 * and the transmission order over a network or other medium. When specifically 
 * talking about bytes, endianness is also referred to simply as byte order. 
 *
 * Examples of storing the value 0x0A0B0C0D in memory:
 *
 * Big-Endian
 * With 8-bit atomic element size and 1-byte (octet) address increment:
 * @code
 * Increasing Address --->
 * +------+------+------+------+
 * | 0x0A | 0x0B | 0x0C | 0x0D |
 * +------+------+------+------+
 * @endcode
 * The most significant byte (MSB) value, which is 0x0A in our example, is stored 
 * at the memory location with the lowest address, the next byte value in 
 * significance, 0x0B, is stored at the following memory location and so on.
 *
 * Little Endian
 * With 16-bit atomic element size:
 * @code
 * Increasing Address --->
 * +------+------+------+------+
 * | 0x0D | 0x0C | 0x0B | 0x0A |
 * +------+------+------+------+
 * @endcode
 * The least significant byte (LSB) value, 0x0D, is at the lowest address. The 
 * other bytes follow in increasing order of significance.
 *
 * Functions which implements conversion between endian are as follows:
 * - #Util_Put_Big_Endian
 * - #Util_Put_Little_Endian
 * - #Util_Get_Big_Endian
 * - #Util_Get_Little_Endian
 * - #Util_Get_Big_Endian_U16
 * - #Util_Get_Big_Endian_U24
 * - #Util_Get_Big_Endian_U32
 * - #Util_Get_Little_Endian_U16
 * - #Util_Get_Little_Endian_U24
 * - #Util_Get_Little_Endian_U32
 * - #Util_Put_Little_Endian_U16
 * - #Util_Put_Little_Endian_U24
 * - #Util_Put_Little_Endian_U32
 * - #Util_Put_Big_Endian_U16
 * - #Util_Put_Big_Endian_U24
 * - #Util_Put_Big_Endian_U32
 *
 * @section USAGE USAGE INSTRUCTIONS:
 *
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
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
 *   - None
 *
 * @defgroup convert_endian_grp Utilities to convert data between big or small endian
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
 * Writes native endian data to Big Endian output
 *
 * @param [out] dest      pointer to destination of the big endian data
 * @param [in] src        pointer to source of native data
 * @param [in] num_bytes  number of bytes to convert (1-4)
 *
 * @see Util_Put_Little_Endian(),Util_Get_Big_Endian(), Util_Get_Little_Endian(),
 *
 * @pre
 *    - 1 <= num_bytes <= 4
 *    - dest != NULL
 *    - src != NULL
 */
   void Util_Put_Big_Endian(uint8_t * dest, const uint32_t * src, size_t num_bytes);

/**
 * Writes native endian data to Little Endian output
 *
 * @param [out] dest      pointer to destination of the little endian data
 * @param [in] src        pointer to source of native data
 * @param [in] num_bytes  number of bytes to convert (1-4)
 *
 * @see Util_Put_Big_Endian(),Util_Get_Big_Endian(), Util_Get_Little_Endian(),
 *
 * @pre
 *    - 1 <= num_bytes <= 4
 *    - dest != NULL
 *    - src != NULL
 */
   void Util_Put_Little_Endian(uint8_t * dest, const uint32_t * src, size_t num_bytes);

/**
 * Reads Big Endian data into native endian format
 *
 * @param [out] dest      pointer to destination of the native endian data
 * @param [in] src        pointer to source of big endian data
 * @param [in] num_bytes  number of bytes to convert (1-4)
 *
 * @see Util_Put_Big_Endian(),Util_Put_Little_Endian(), Util_Get_Little_Endian(),
 *
 * @pre
 *    - 1 <= num_bytes <= 4
 *    - dest != NULL
 *    - src != NULL
 */
   void Util_Get_Big_Endian(uint32_t * dest, const uint8_t * src, size_t num_bytes);

/**
 * Reads Little Endian data into native endian format
 *
 * @param [out] dest      pointer to destination of the native endian data
 * @param [in] src        pointer to source of little endian data
 * @param [in] num_bytes  number of bytes to convert (1-4)
 *
 * @see Util_Put_Big_Endian(),Util_Put_Little_Endian(), Util_Get_Big_Endian(),
 *
 * @pre
 *    - 1 <= num_bytes <= 4
 *    - dest != NULL
 *    - src != NULL
 */
   void Util_Get_Little_Endian(uint32_t * dest, const uint8_t * src, size_t num_bytes);

/**
 * Returns 16 bit Big Endian data in native endian format
 *
 * @return 16bit unsigned native endian data
 *
 * @param [in] src pointer to source of big endian 2 byte data
 *
 * @pre
 *    - src != NULL
 */
   uint16_t Util_Get_Big_Endian_U16(const uint8_t * src);

/**
 * Returns 24 bit Big Endian data in native endian format
 *
 * @return 32bit unsigned native endian data
 *
 * @param [in] src pointer to source of big endian 3 byte data
 *
 * @pre
 *    - src != NULL
 */
   uint32_t Util_Get_Big_Endian_U24(const uint8_t * src);

/**
 * Returns 32 bit Big Endian data in native endian format
 *
 * @return 32bit unsigned native endian data
 *
 * @param [in] src pointer to source of big endian 4 byte data
 *
 * @pre
 *    - src != NULL
 */
   uint32_t Util_Get_Big_Endian_U32(const uint8_t * src);

/**
 * Returns 16 bit Little Endian data in native endian format
 *
 * @return 16bit unsigned native endian data
 *
 * @param [in] src pointer to source of big endian 2 byte data
 *
 * @pre
 *    - src != NULL
 */
   uint16_t Util_Get_Little_Endian_U16(const uint8_t * src);

/**
 * Returns 24 bit Little Endian data in native endian format
 *
 * @return 32bit unsigned native endian data
 *
 * @param [in] src pointer to source of big endian 3 byte data
 *
 * @pre
 *    - src != NULL
 */
   uint32_t Util_Get_Little_Endian_U24(const uint8_t * src);

/**
 * Returns 32 bit Big Endian data in native endian format
 *
 * @return 32bit unsigned native endian data
 *
 * @param [in] src pointer to source of big endian 4 byte data
 *
 * @pre
 *    - src != NULL
 */
   uint32_t Util_Get_Little_Endian_U32(const uint8_t * src);

/**
 * Writes native endian unsigned 16 bit data to 2 byte Little Endian output
 *
 * @param [out] dest pointer to destination of the little endian data
 * @param [in]  src  native source 16 bit data
 *
 * @pre
 *    - dest != NULL
 */
   void Util_Put_Little_Endian_U16(uint8_t * dest, uint16_t src);

/**
 * Writes native endian unsigned 24 bit data to 3 byte Little Endian output
 *
 * @param [out] dest pointer to destination of the little endian data
 * @param [in]  src  native source 24 bit data
 *
 * @pre
 *    - dest != NULL
 */
   void Util_Put_Little_Endian_U24(uint8_t * dest, uint32_t src);

/**
 * Writes native endian unsigned 32 bit data to 4 byte Little Endian output
 *
 * @param [out] dest pointer to destination of the little endian data
 * @param [in]  src  native source 32 bit data
 *
 * @pre
 *    - dest != NULL
 */
   void Util_Put_Little_Endian_U32(uint8_t * dest, uint32_t src);

/**
 * Writes native endian unsigned 16 bit data to 2 byte Big Endian output
 *
 * @param [out] dest pointer to destination of the big endian data
 * @param [in]  src  native source 16 bit data
 *
 * @pre
 *    - dest != NULL
 */
   void Util_Put_Big_Endian_U16(uint8_t * dest, uint16_t src);

/**
 * Writes native endian unsigned 24 bit data to 3byte Big Endian output
 *
 * @param [out] dest pointer to destination of the big endian data
 * @param [in]  src  native source 32 bit data
 *
 * @pre
 *    - dest != NULL
 */
   void Util_Put_Big_Endian_U24(uint8_t * dest, uint32_t src);

/**
 * Writes native endian unsigned 32 bit data to Big Endian output
 *
 * @param [out] dest pointer to destination of the big endian data
 * @param [in]  src  native source 32 bit data
 *
 * @pre
 *    - dest != NULL
 */
   void Util_Put_Big_Endian_U32(uint8_t * dest, uint32_t src);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file convert_endian.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 21-apr-2010 Kirk Bailey Rev 7
 *   - Moved extern C block to only wrap function prototypes to match new
 *     template.
 *
 * - 27-May-2009 Larry Piekarski Rev 5-6
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 4
 *   - Updated Doxygen Comments
 *
 * - 16-May-2007 Klaus Krosse
 *   - added type size specific endian conversion functions which are easier to use
 *     in many applications.
 *
 * - 03-Apr-2007  Klaus Krosse
 *   - Get and put endian functions did not work for big endian machines like
 *     SH2-A for 3 byte (24bit) values and for 2 bytes if source was a 32 bit
 *     integer.
 *   - Changed types of src and dest parameter from void to integer types.
 *
 * - 09-Mar-2007 Dan Carman
 *   - Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif                          /* CONVERT_ENDIAN_H */
