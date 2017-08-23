#ifndef REUSE_H
#   define REUSE_H
/*===========================================================================*/
/**
 * @file bb_reuse_uitron_arm_rvct_api/reuse.h
 *
 * Declares base types required by the Delphi C Coding Standards for
 * the RVCT tools for ARM.
 *
 * %full_filespec: reuse.h~kok_inf#13:incl:kok_aud#34 %
 * @version %version:kok_inf#13 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Mon Aug 25 13:45:52 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008-2014 Delphi Technologies, Inc., All Rights Reserved.
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
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - Defines the types required by the standard but does not define
 *     the required functions for enabling/disabling interrupts, etc.
 *
 * @defgroup reuse_arm_rvct Base type declarations for  ARM RVCT.
 * @{
 */
/*==========================================================================*/
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */

/*===========================================================================*
 * Header Files
 *===========================================================================*/

/*
 * NOTE: For a compiler that implements the 1999 version of the ANSI C
 *       standard, most of reuse.h is satisfied by the following header files.
 */
#   include <signal.h>
#   include <stdbool.h>
#   include <stddef.h>

#define __STDC_LIMIT_MACROS /* force inclusion of 64-bit definitions from eSol library for C++ code */

#   include <stdint.h>

/* -------------------------------------------------------------------
 * Exact Width integer types
 * ---------------------------------------------------------------- */

#   define UINT8_MIN    (0)
#   define UINT16_MIN   (0)
#   define UINT32_MIN   (0)
#   define UINT_LEAST8_MIN    UINT32_MIN
#   define UINT_LEAST16_MIN   UINT32_MIN
#   define UINT_LEAST32_MIN   UINT32_MIN
#   define UINT_FAST8_MIN     UINT32_MIN
#   define UINT_FAST16_MIN    UINT32_MIN
#   define UINT_FAST32_MIN    UINT32_MIN

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

#   define ATOMIC_MIN SIG_ATOMIC_MIN
#   define ATOMIC_MAX SIG_ATOMIC_MAX

/* -------------------------------------------------------------------
 * Bit-field types:
 * ---------------------------------------------------------------- */

typedef uint8_t bitfield8_t;
typedef uint16_t bitfield16_t;
typedef uint32_t bitfield32_t;
typedef uint64_t bitfield64_t;

/* -------------------------------------------------------------------
 * Boolean type:
 * ---------------------------------------------------------------- */

#   ifndef __cplusplus
/* The size of "bool" in C++ for the arm rvct compiler is 8-bits.
  So, undef the C bool definition and define as 8-bits so both
  C and C++ are guaranteed to use the same size. */
#      undef bool
typedef uint8_t bool; /* Deprecated, use bool_t instead */
#   else
/* If a compiler error is generated on the next line, then the default
   boolean size for C++ has been changed. This will cause problems
   with the above definition */
typedef bool COMPILE_TIME_BOOL_SIZE_VERIFICATION[sizeof(bool) == 1 ? 1 : -1];
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
 * CPU_BIT_ORDER, CPU_BYTE_ORDER
 * ---------------------------------------------------------------- */

/* Value for CPU_BYTE_ORDER if high byte is located in memory before low byte */
#   define HIGH_BYTE_FIRST (0)
/* Value for CPU_BYTE_ORDER if low byte is located in memory before high byte */
#   define LOW_BYTE_FIRST  (1)

/** ARM is little endian */
#   define CPU_BYTE_ORDER LOW_BYTE_FIRST

/* Value for CPU_BIT_ORDER if the most significant bit is the first bit of a
 * bit field */
#   define MSB_FIRST (0)
/* Value for CPU_BIT_ORDER if the least significant bit is the first bit of
 * the bit field */
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

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

#   define STATIC_VAR   static

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/* -------------------------------------------------------------------
 * Atomic type:
 * ---------------------------------------------------------------- */

typedef sig_atomic_t atomic_t;

/* -------------------------------------------------------------------
 * stddef types:
 * ---------------------------------------------------------------- */
#   ifndef SIZE_MAX 
#      define SIZE_MAX  UINT32_MAX
#   endif
#   ifndef SSIZE_MAX
#      define SSIZE_MAX INT32_MAX
#   endif
 
#ifndef	_SSIZE_T
	#define	_SSIZE_T
	typedef	signed int		ssize_t;
#endif
 
/* -------------------------------------------------------------------
 * Some MISRA types:
 * ---------------------------------------------------------------- */

typedef char char_t;

#ifndef _FLOAT32_T
   #define _FLOAT32_T
   typedef float	float32_t;
#endif

#ifndef _FLOAT64_T
   #define _FLOAT64_T
   typedef double float64_t;
#endif


/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Compute the number of elements in an array
 * @param array   name of array 
 * @return number of array elements 
 */
/* PRQA S 3410 1 */ /* Silence QAC warning about no parentheses around "array" */
#   define Num_Elems(array)    (sizeof(array)/sizeof(array[0]))


/* 
 * snprinft is supported by ARM stdio, but is not declared with strict ansi, 
 *  but given that it is so much safer than sprintf, make is available
 */ 
extern int snprintf(char * /*s*/, size_t /*n*/, const char * /*format*/, ...);
   /*
    * is equivalent to fprintf, except that the argument s specifies an array
    * into which the generated output is to be written, rather than to a
    * stream. The argument n specifies the size of the output array, so as to
    * avoid overflowing the buffer.
    * A null character is written at the end of the characters written, even
    * if the formatting was not completed; it is not counted as part of the
    * returned sum. At most n characters of the output buffer are used,
    * _including_ the null character.
    * Returns: the number of characters that would have been written to the
    *          array, not counting the terminating null character, if the
    *          array had been big enough. So if the return is >=0 and <n, then
    *          the entire string was successfully formatted; if the return is
    *          >=n, the string was truncated (but there is still a null char
    *          at the end of what was written); if the return is <0, there was
    *          an error.
    */

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */

/*===========================================================================*/
/*!
 * @file bb_reuse_uitron_arm_rvct_api/reuse.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 08-Jul-2010 Larry Piekarski Rev 12
 *   - SCR kok_basa#2323: Created a bool_t type that is guaranteed to be 8-bits
 *                        and ensured the bool type for C and C++ are the same
 *                        width.
 *
 * - 25-Aug-2014 Kirk Bailey
 *   - Deleted history prior to 2010.
 *   - Added support for SIZE_MAX and SSIZE_MAX.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* REUSE_H */
