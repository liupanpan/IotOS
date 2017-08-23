#ifndef RING_BUFFER_H
#   define RING_BUFFER_H
/*===========================================================================*/
/**
 * @file ring_buffer.h
 *
 * API for ring buffer functions
 *
 * %full_filespec:ring_buffer.h~kok_basa#8:incl:tck_aud#1 %
 * @version %version:kok_basa#8 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Mon Jul 12 12:42:50 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page ring_buffer_page Ring Buffer User Manual
 *
 * @section DESC DESCRIPTION:
 * The Ring buffer mechanism is often used for data communication purposes. When 
 * moving through the buffer, the writer moves forward one step each time it 
 * writes, and when it passes the end of the buffer it starts again at the 
 * beginning. The reader moves through the buffer in the same way. As long as the 
 * reader is as fast as or faster than the writer, the buffer acts as a queue of 
 * the next data to be processed. If the writer is faster than the reader, the 
 * buffer can fill up.
 *
 * @image html ring_buffer.jpg
 * 
 * @note The ring buffer implementation is not thread-safe. This is the user 
 *       responsibility for providing whatever synchronization primitives are 
 *       necessary in order to ensure exclusive access to the ring buffer control 
 *       structure and the ring buffer table during adding or removing elements 
 *       from the buffer.
 *
 * @section USAGE USAGE INSTRUCTIONS:
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 * -# Initialize buffer control sturcture
 *     - Ring_Buf_Init(&My_Ring_Buf, Num_Elems(My_Array));
 * -# Add an element
 *     - My_Array[My_Ring_Buf.in] = x;
 *     - Ring_Buf_Add(&My_Ring_Buf);
 * -# Remove an element
 *     - Y = My_Array[My_Ring_Buf.out];
 *     - Ring_Buf_Remove(&My_Ring_Buf);
 * -# See if the buffer full or empty
 *     - Ring_Buf_Is_Empty(&My_Ring_Buf)
 *     - Ring_Buf_Is_Full(&My_Ring_Buf)
 *
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
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
 * @defgroup ring_buffer_grp Ring Buffer handling
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
/**
 *  Control structure for ring buffer.
 */
   typedef struct Ring_Buf_Ctrl_Tag
   {
      uint16_t in;          /**< index of next array element to add to */
      uint16_t out;         /**< index of oldest element to remove */
      uint16_t size;        /**< number of elements in ring buffer */
   } Ring_Buf_Ctrl_T;

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
 * Reset / Initialize ring buffer to empty
 *
 * @param [in] buf         pointer to ring buffer control structure
 * @param [in] size        size of the ring buffer
 *
 * @see Ring_Buf_Is_Empty(), Ring_Buf_Is_Full(), Ring_Buf_Add(), Ring_Buf_Remove()
 *
 * @pre
 *    - buf != NULL
 */
   void Ring_Buf_Init(Ring_Buf_Ctrl_T * buf, uint16_t size);

/**
 * Returns whether ring buffer is empty.
 *
 * @return true if buffer is empty, otherwise false
 *
 * @param [in] buf         pointer to ring buffer control structure
 *
 * @see Ring_Buf_Init(), Ring_Buf_Remove(), Ring_Buf_Add(), Ring_Buf_Is_Full()
 *
 * @pre
 *    - buf != NULL
 */
   bool_t Ring_Buf_Is_Empty(const Ring_Buf_Ctrl_T * buf);

/**
 * Returns whether ring buffer is full
 *
 * @return true if buffer is full, otherwise false
 *
 * @param [in] buf         pointer to ring buffer control structure
 *
 * @see Ring_Buf_Init(), Ring_Buf_Is_Empty(), Ring_Buf_Add(), Ring_Buf_Remove()
 *
 * @pre
 *    - buf != NULL
 */
   bool_t Ring_Buf_Is_Full(const Ring_Buf_Ctrl_T * buf);

/**
 * Logical adds element to ring buffer
 *
 * @param [in] buf         pointer to ring buffer control structure
 *
 * @see Ring_Buf_Init(), Ring_Buf_Is_Empty(), Ring_Buf_Is_Full(), Ring_Buf_Remove(),
 *
 * @pre
 *    - buf != NULL
 */
   void Ring_Buf_Add(Ring_Buf_Ctrl_T * buf);

/**
 * Logical removes element  from ring buffer
 *
 * @param [in] buf         pointer to ring buffer control structure
 *
 * @see Ring_Buf_Init(), Ring_Buf_Is_Empty(), Ring_Buf_Is_Full(), Ring_Buf_Add()
 *
 * @pre
 *    - buf != NULL
 */
   void Ring_Buf_Remove(Ring_Buf_Ctrl_T * buf);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/**
 * @file ring_buffer.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 8
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 21-apr-2010 Kirk Bailey Rev 7
 *   - SCR 1305 - Corrected Doxygen comments to reference existing functions.
 *
 * - 27-May-2009 Larry Piekarski Rev 5-6
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 4
 *   - Updated Doxygen Comments
 *
 * - 06-Oct-2007 Bartosz Jamrozik
 *   - Removed flag indicating ring buffer is full - no longer necessary.
 *
 * - 18-Aug-2007 Dan Carman
 *   - MISRA changes
 *
 * - 02-May-2007 Bartosz Jamrozik
 *   - Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif                          /* RING_BUFFER_H */
