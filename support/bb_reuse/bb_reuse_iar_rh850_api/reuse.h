#ifndef REUSE_H
#   define REUSE_H
/*===========================================================================*/
/**
 * @file reuse.h
 *
 * @brief Declares base types required by the Delphi C Coding Standards for
 * the NEC tools for the RH850.
 *
 * %full_filespec: reuse.h~2:incl:kok_inf#19 %
 * @version %version: 2 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Thu May  7 07:13:25 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2014 Delphi Technologies, Inc., All Rights Reserved.
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
 *   - Defines the types required by the standard but does not define
 *     the required functions for enabling/disabling interrupts, etc.
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include <stddef.h>          /* get ptrdiff_t, size_t, NULL, offsetof */

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/* -------------------------------------------------------------------
 * Exact Width integer types
 * ---------------------------------------------------------------- */

#define UINT8_MIN    (0)
#define UINT8_MAX    (255)
#define UINT16_MIN   (0)
#define UINT16_MAX   (65535)
#define UINT32_MIN   (0)
#define UINT32_MAX   (4294967295U)

#define INT8_MIN     (-128)
#define INT8_MAX     (127)
#define INT16_MIN    (-32768)
#define INT16_MAX    (32767)
#define INT32_MIN    (-2147483648)
#define INT32_MAX    (2147483647)

/* -------------------------------------------------------------------
 * Minimum width integer types
 * ---------------------------------------------------------------- */

#define UINT_LEAST8_MIN    UINT32_MIN
#define UINT_LEAST8_MAX    UINT32_MAX
#define UINT_LEAST16_MIN   UINT32_MIN
#define UINT_LEAST16_MAX   UINT32_MAX
#define UINT_LEAST32_MIN   UINT32_MIN
#define UINT_LEAST32_MAX   UINT32_MAX

#define INT_LEAST8_MIN     INT32_MIN
#define INT_LEAST8_MAX     INT32_MAX
#define INT_LEAST16_MIN    INT32_MIN
#define INT_LEAST16_MAX    INT32_MAX
#define INT_LEAST32_MIN    INT32_MIN
#define INT_LEAST32_MAX    INT32_MAX

/* -------------------------------------------------------------------
 * Fastest minimum-width integer types:
 * ---------------------------------------------------------------- */

#define UINT_FAST8_MIN     UINT32_MIN
#define UINT_FAST8_MAX     UINT32_MAX
#define UINT_FAST16_MIN    UINT32_MIN
#define UINT_FAST16_MAX    UINT32_MAX
#define UINT_FAST32_MIN    UINT32_MIN
#define UINT_FAST32_MAX    UINT32_MAX

#define INT_FAST8_MIN      INT32_MIN
#define INT_FAST8_MAX      INT32_MAX
#define INT_FAST16_MIN     INT32_MIN
#define INT_FAST16_MAX     INT32_MAX
#define INT_FAST32_MIN     INT32_MIN
#define INT_FAST32_MAX     INT32_MAX

/* -------------------------------------------------------------------
 * Atomic type:
 * ---------------------------------------------------------------- */

#define ATOMIC_MIN INT32_MIN
#define ATOMIC_MAX INT32_MAX

/* -------------------------------------------------------------------
 * Boolean type:
 * ---------------------------------------------------------------- */

#define false  (0)
#define true   (!false)

/* -------------------------------------------------------------------
 * CPU_BIT_ORDER, CPU_BYTE_ORDER
 * ---------------------------------------------------------------- */

/*
 * DESCRIPTION: Little Endian memory storage is LSB 1st then MSB
 */
#define LITTLE_ENDIAN TRUE

/* Value for CPU_BYTE_ORDER if high byte is located in memory before low byte */
#   define HIGH_BYTE_FIRST (0)
/* Value for CPU_BYTE_ORDER if low byte is located in memory before high byte */
#   define LOW_BYTE_FIRST  (1)

#   define CPU_BYTE_ORDER LOW_BYTE_FIRST

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

/* -------------------------------------------------------------------
 * stddef types:
 * ---------------------------------------------------------------- */
#define SIZE_MAX  UINT16_MAX
#define SSIZE_MAX INT16_MAX

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

#define STATIC_VAR   static

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/* -------------------------------------------------------------------
 * Typedef Function
 * ---------------------------------------------------------------- */

/* -------------------------------------------------------------------
 * Exact Width integer types
 * ---------------------------------------------------------------- */

typedef unsigned char   uint8_t;       /* unsigned 8 bits */
typedef unsigned short  uint16_t;      /* unsigned 16 bits */
typedef unsigned int    uint32_t;      /* unsigned 32 bits */

typedef signed char     int8_t;        /* signed 8 bits */
typedef signed short    int16_t;       /* signed 16 bits */
typedef signed int      int32_t;       /* signed 32 bits */

/* -------------------------------------------------------------------
 * Minimum width integer types
 * ---------------------------------------------------------------- */

typedef unsigned int uint_least8_t;    /* unsigned atleast 8 bits */
typedef unsigned int uint_least16_t;   /* unsigned atleast 16 bits */
typedef unsigned int uint_least32_t;   /* unsigned atleast 32 bits */

typedef signed int   int_least8_t;     /* signed atleast 8 bits */
typedef signed int   int_least16_t;    /* signed atleast 16 bits */
typedef signed int   int_least32_t;    /* signed atleast 32 bits */

/* -------------------------------------------------------------------
 * Fastest minimum-width integer types: 
 * ---------------------------------------------------------------- */

typedef unsigned int uint_fast8_t;     /* unsigned fast 8 bits */
typedef unsigned int uint_fast16_t;    /* unsigned fast 16 bits */
typedef unsigned int uint_fast32_t;    /* unsigned fast 32 bits */

typedef signed int   int_fast8_t;      /* signed fast 8 bits */
typedef signed int   int_fast16_t;     /* signed fast 16 bits */
typedef signed int   int_fast32_t;     /* signed fast 32 bits */

/* -------------------------------------------------------------------
 * Bit-field types:
 * ---------------------------------------------------------------- */

typedef unsigned char   bitfield8_t;      /* unsigned 8 bits */
typedef unsigned short  bitfield16_t;     /* unsigned 16 bits */
typedef unsigned int    bitfield32_t;     /* unsigned 32 bits */


/* -------------------------------------------------------------------
 * Atomic type:
 * ---------------------------------------------------------------- */

typedef int32_t atomic_t;

/* -------------------------------------------------------------------
 * Boolean type:
 * ---------------------------------------------------------------- */

typedef unsigned char   bool;             /* logical true/false (0 = false) */
typedef uint8_t         bool_t;
typedef int             bool_fast;        /* fast logical true/false */

/* -------------------------------------------------------------------
 * stddef types:
 * ---------------------------------------------------------------- */

typedef int ssize_t;

/* -------------------------------------------------------------------
 * Some MISRA types:
 * ---------------------------------------------------------------- */
typedef char char_t;
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

#define  Num_Elems(array) (sizeof(array) / sizeof(array[0]))
                                       /* returns the number of records in an array */

#   ifdef __cplusplus
}         /* extern "C" */
#   endif /* __cplusplus */

/*===========================================================================*/
/*!
 * @file reuse.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 30-Apr-2015 Emmanuel Yepez (zzxc1x) Rev. 1
 *   + Created initial file.
 */
/*===========================================================================*/

/** @} doxygen end group */
#endif /* REUSE_H */

