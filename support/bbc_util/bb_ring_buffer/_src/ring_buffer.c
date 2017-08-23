/*===========================================================================*/
/**
 * @file ring_buffer.c
 *
 * The file contains set of functions for handling ring buffer.
 *
 * %full_filespec:ring_buffer.c~kok_basa#6:csrc:tck_aud#1 %
 * @version %version:kok_basa#6 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Mon Jul 12 12:42:51 2010 %
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
 * The file contains set of functions for handling ring buffer.
 *
 * @section ABBR ABBREVIATIONS:
 *   - None.
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
 *   - None.
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "pbc_trace.h"
#include "ring_buffer.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 10); /**< IDs file for EM assert handling */

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
static uint16_t rb_next(uint16_t cur, uint16_t size);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/**
 * Returns index of the next element in ring buffer.
 *
 * @param [in] cur Index of the current element.
 * @param [in] size Total number of elements in the buffer
 */
static uint16_t rb_next(uint16_t cur, uint16_t size)
{
   cur++;

   if (cur >= size)
   {
      cur = 0;
   }

   return cur;
}

/*===========================================================================*\
 *
 * Reset / Initialize ring buffer to empty
 *
\*===========================================================================*/
void Ring_Buf_Init(Ring_Buf_Ctrl_T * buf, uint16_t size)
{
   PBC_Require(buf != NULL, "Ring_Buf_Init called with NULL pointer");
   
   buf->in = 0;
   buf->out = 0;
   buf->size = size;
}

/*===========================================================================*\
 *
 * Returns whether ring buffer is empty.
 *
\*===========================================================================*/
bool_t Ring_Buf_Is_Empty(const Ring_Buf_Ctrl_T * buf)
{
   PBC_Require(buf != NULL, "Ring_Buf_Is_Empty called with NULL pointer");
   
   return (buf->in == buf->out);
}

/*===========================================================================*\
 *
 * Returns whether ring buffer is full
 *
\*===========================================================================*/
bool_t Ring_Buf_Is_Full(const Ring_Buf_Ctrl_T * buf)
{
   PBC_Require(buf != NULL, "Ring_Buf_Is_Full called with NULL pointer");
   
   return (rb_next(buf->in, buf->size) == buf->out);
}

/*===========================================================================*\
 *
 * Logical adds element to ring buffer
 *
\*===========================================================================*/
void Ring_Buf_Add(Ring_Buf_Ctrl_T * buf)
{
   PBC_Require(buf != NULL, "Ring_Buf_Add called with NULL pointer");
   
   /* Only safe for a single producer thread. */
   if (!Ring_Buf_Is_Full(buf))
   {
      buf->in = rb_next(buf->in, buf->size);
   }
}

/*===========================================================================*\
 *
 * Logical removes element  from ring buffer
 *
\*===========================================================================*/
void Ring_Buf_Remove(Ring_Buf_Ctrl_T * buf)
{
   PBC_Require(buf != NULL, "Ring_Buf_Remove called with NULL pointer");
   
   /* Only save for a single consumer thread. */
   if (!Ring_Buf_Is_Empty(buf))
   {
      buf->out = rb_next(buf->out, buf->size);
   }
}

/*===========================================================================*/
/*!
 * @file ring_buffer.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 6
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 08-Jul-2009 Larry Piekarski Rev 5
 *   - Peer Review Updates
 *
 * - 27-May-2009 Larry Piekarski Rev 4
 *   - Updated Doxygen comments.
 *
 * - 06-Oct-2007 Bartosz Jamrozik
 *   - added local function rb_next. Other functions reworked to be more robust.
 *
 * - 18-Aug-2007 Dan Carman
 *   - MISRA changes
 *
 * - 22-May-2007 Bartosz Jamrozik
 *   - Created initial file.
 *
\*===========================================================================*/
