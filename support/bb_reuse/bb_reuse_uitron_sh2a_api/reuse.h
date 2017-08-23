#ifndef REUSE_H
#   define REUSE_H
/*===========================================================================*/
/**
 * @file bb_reuse_uitron_sh2a_api/reuse.h
 *
 * @brief Declares base types required by the Delphi C Coding Standards for
 * the Renesas tools for the SH2A
 *
 * %full_filespec: reuse.h~kok_inf#14:incl:kok_aud#18 %
 * @version %version: kok_inf#14 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Mon Aug 25 14:20:38 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   The types defined by this file typically represent types present in
 *   C Language implementations that conform to the C99 ANSI standard. In
 *   order to support older tools sets that are not conformant to C99, these
 *   types should be obtained through this header file instead of the C99
 *   standard locations. This allows the types to be defined for non-conformant
 *   tool sets based upon the types that the tool set does support.
 *
 * ABBREVIATIONS:
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - Defines the types required by the standard but does not define
 *     the required functions for enabling/disabling interrupts, etc.
 *
 * @defgroup reuse_SH2A Base type declarations for SH2A with Renesas tools.
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include <stddef.h>          /* get ptrdiff_t, size_t, NULL, offsetof */

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/* -------------------------------------------------------------------
 * Exact Width integer types
 * ---------------------------------------------------------------- */

#   define UINT8_MIN    (0)
#   define UINT8_MAX    (255)
#   define UINT16_MIN   (0)
#   define UINT16_MAX   (65535)
#   define UINT32_MIN   (0)
#   define UINT32_MAX   (4294967295U)

#   define INT8_MIN     (-128)
#   define INT8_MAX     (127)
#   define INT16_MIN    (-32768)
#   define INT16_MAX    (32767)
#   define INT32_MIN    (-2147483648)
#   define INT32_MAX    (2147483647)

/* -------------------------------------------------------------------
 * Minimum width integer types
 * ---------------------------------------------------------------- */

#   define UINT_LEAST8_MIN    UINT32_MIN
#   define UINT_LEAST8_MAX    UINT32_MAX
#   define UINT_LEAST16_MIN   UINT32_MIN
#   define UINT_LEAST16_MAX   UINT32_MAX
#   define UINT_LEAST32_MIN   UINT32_MIN
#   define UINT_LEAST32_MAX   UINT32_MAX

#   define INT_LEAST8_MIN     INT32_MIN
#   define INT_LEAST8_MAX     INT32_MAX
#   define INT_LEAST16_MIN    INT32_MIN
#   define INT_LEAST16_MAX    INT32_MAX
#   define INT_LEAST32_MIN    INT32_MIN
#   define INT_LEAST32_MAX    INT32_MAX

/* -------------------------------------------------------------------
 * Fastest minimum-width integer types: 
 * ---------------------------------------------------------------- */

#   define UINT_FAST8_MIN     UINT32_MIN
#   define UINT_FAST8_MAX     UINT32_MAX
#   define UINT_FAST16_MIN    UINT32_MIN
#   define UINT_FAST16_MAX    UINT32_MAX
#   define UINT_FAST32_MIN    UINT32_MIN
#   define UINT_FAST32_MAX    UINT32_MAX

#   define INT_FAST8_MIN      INT32_MIN
#   define INT_FAST8_MAX      INT32_MAX
#   define INT_FAST16_MIN     INT32_MIN
#   define INT_FAST16_MAX     INT32_MAX
#   define INT_FAST32_MIN     INT32_MIN
#   define INT_FAST32_MAX     INT32_MAX

/* -------------------------------------------------------------------
 * Atomic type:
 * ---------------------------------------------------------------- */

#   define ATOMIC_MIN INT32_MIN
#   define ATOMIC_MAX INT32_MAX

/* -------------------------------------------------------------------
 * Boolean type:
 * ---------------------------------------------------------------- */

#   define false  (0)
#   define true   (!false)

/* -------------------------------------------------------------------
 * CPU_BIT_ORDER, CPU_BYTE_ORDER
 * ---------------------------------------------------------------- */

/* Value for CPU_BYTE_ORDER if high byte is located in memory before low byte */
#   define HIGH_BYTE_FIRST (0)
/* Value for CPU_BYTE_ORDER if low byte is located in memory before high byte */
#   define LOW_BYTE_FIRST  (1)

#   define CPU_BYTE_ORDER HIGH_BYTE_FIRST

/* Value for CPU_BIT_ORDER if the most significant bit is the first bit of a bit field */
#   define MSB_FIRST (0)
/* Value for CPU_BIT_ORDER if the least significant bit is the first bit of the bit field */
#   define LSB_FIRST (1)

#   define CPU_BIT_ORDER LSB_FIRST

/* -------------------------------------------------------------------
 * BIT Mask Definitions
 * ---------------------------------------------------------------- */

#   ifndef _BITXX_MASKS
#      define BIT0  (0x00000001)
#      define BIT1  (0x00000002)
#      define BIT2  (0x00000004)
#      define BIT3  (0x00000008)
#      define BIT4  (0x00000010)
#      define BIT5  (0x00000020)
#      define BIT6  (0x00000040)
#      define BIT7  (0x00000080)
#      define BIT8  (0x00000100)
#      define BIT9  (0x00000200)
#      define BIT10 (0x00000400)
#      define BIT11 (0x00000800)
#      define BIT12 (0x00001000)
#      define BIT13 (0x00002000)
#      define BIT14 (0x00004000)
#      define BIT15 (0x00008000)
#      define BIT16 (0x00010000)
#      define BIT17 (0x00020000)
#      define BIT18 (0x00040000)
#      define BIT19 (0x00080000)
#      define BIT20 (0x00100000)
#      define BIT21 (0x00200000)
#      define BIT22 (0x00400000)
#      define BIT23 (0x00800000)
#      define BIT24 (0x01000000)
#      define BIT25 (0x02000000)
#      define BIT26 (0x04000000)
#      define BIT27 (0x08000000)
#      define BIT28 (0x10000000)
#      define BIT29 (0x20000000)
#      define BIT30 (0x40000000)
#      define BIT31 (0x80000000)
#   endif

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

#   define STATIC_VAR   static

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/* -------------------------------------------------------------------
 * Exact Width integer types
 * ---------------------------------------------------------------- */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

/* -------------------------------------------------------------------
 * Minimum width integer types
 * ---------------------------------------------------------------- */

typedef unsigned int uint_least8_t;
typedef unsigned int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long uint_least64_t;

typedef signed int int_least8_t;
typedef signed int int_least16_t;
typedef signed int int_least32_t;
typedef signed long long int_least64_t;

/* -------------------------------------------------------------------
 * Fastest minimum-width integer types: 
 * ---------------------------------------------------------------- */

typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long uint_fast64_t;

typedef signed int int_fast8_t;
typedef signed int int_fast16_t;
typedef signed int int_fast32_t;
typedef signed long long int_fast64_t;

/* -------------------------------------------------------------------
 * Bit-field types:
 * ---------------------------------------------------------------- */

typedef unsigned char bitfield8_t;
typedef unsigned short bitfield16_t;
typedef unsigned int bitfield32_t;
typedef unsigned long long bitfield64_t;

/* -------------------------------------------------------------------
 * Atomic type:
 * ---------------------------------------------------------------- */

typedef int32_t atomic_t;

/* -------------------------------------------------------------------
 * stddef types:
 * ---------------------------------------------------------------- */
#   ifndef SIZE_MAX 
#      define SIZE_MAX  UINT32_MAX
#   endif
#   ifndef SSIZE_MAX
#      define SSIZE_MAX INT32_MAX
#   endif
 
typedef int ssize_t;
 
/* -------------------------------------------------------------------
 * Boolean type:
 * ---------------------------------------------------------------- */

#   ifndef __cplusplus
/* The size of "bool" in C++ for the sh2a compiler is 32-bits.
  So, undef the C bool definition and define as 32-bits so both
  C and C++ are guaranteed to use the same size. */
#      undef bool
typedef uint32_t bool; /* Deprecated, use bool_t instead */
#   else
/* If a compiler error is generated on the next line, then the default
   boolean size for C++ has been changed. This will cause problems
   with the above definition */
typedef bool COMPILE_TIME_BOOL_SIZE_VERIFICATION[sizeof(bool) == 4 ? 1 : -1];
#   endif /* __cplusplus */

/* Use this type for bool going forward. This type is guaranteed to be
  8-bits regardless of the platform. DO NOT update this type to match
  the bool size above */
typedef uint8_t bool_t;
/* This definition is for a type that matches the width of the
   processor. This should only be used for variables in tight loops
   within a function block. This type should NOT be used for data 
   that is shared between processors or platforms because compatibility
   is not guaranteed. */
typedef int bool_fast;

/* -------------------------------------------------------------------
 * Some MISRA types:
 * ---------------------------------------------------------------- */

typedef signed char char_t;
typedef float float32_t;
typedef double float64_t;


/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

/**
 * Compute the number of elements in an array
 * @param array   name of array 
 * @return number of array elements 
 */
/* PRQA S 3410 1 */
#   define Num_Elems(array)    (sizeof(array)/sizeof(array[0]))


/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * - 08-Jul-2010 Larry Piekarski Rev 13
 *   - SCR kok_basa#2323: Created a bool_t type that is guaranteed to be 8-bits
 *                        and ensured the bool type for C and C++ are the same
 *                        width.
 *
 * - 25-Aug-2014 Kirk Bailey Rev 14
 *   - Deleted history prior to 2010.
 *   - Added SIZE_MAX and SSIZE_MAX.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* REUSE_H */
