/*===========================================================================*/
/**
 * @file convert_8859_1_to_utf8.c
 *
 * Implements generic functions for convert_8859_1_to_utf8
 *
 * %full_filespec:  convert_8859_1_to_utf8.c~4:csrc:kok_basa#1 %
 * @version %version:  4 %
 * @author  %derived_by: xz152s %
 * @date    %date_modified: Sat May 19 07:08:24 2012 %
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
 * Generic functions for convert_8859_1_to_utf8
 *
 * @section ABBR ABBREVIATIONS:
 *   - 8859-1 also known as Latin 1
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

#include "convert_8859_1_to_utf8.h"
#include "pbc_trace.h"
/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/

/*===========================================================================*\
 * Local Preprocessor #define MACROS
 \*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 13); /**< IDs file for EM assert handling */

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

/**
 * The function converts a 8859-1 encoded character array to a utf-8 character array.
 *
 *  * @param outStr - output buffer
 *           size   - size of the outStr buffer
 *           inStr  - buffer to convert
 *
 */
void Convert_8859_1_To_UTF8(char *outStr, size_t size, const char *inBuf)
{
   uint32_t i = 0;
   uint32_t pos = 0;
   unsigned char *inStr = (unsigned char*)inBuf;

   if ((outStr != NULL) && (inStr != NULL) && (size > 0))
   {
      while ((inStr[i] != '\0') && (pos + 1 < size))
      {
         if (inStr[i] < 0x80)
         {
            outStr[pos++] = inStr[i];
         }
         else
         {
            if (pos + 2 < size)
            {
               outStr[pos++] = (((inStr[i] & 0xC0) >> 6) | 0xC0);
               outStr[pos++] = ((inStr[i] & 0x3F) | 0x80);
            }
            else
            {
               break;
            }
         }
         i++;
      }
      outStr[pos] = '\0';
   }
   else
   {
      if (outStr == NULL)
      {
         Tr_Warn("Convert_8859_1_To_UTF8 was called with outStr == NULL");
      }
      if (inStr == NULL)
      {
         Tr_Warn("Convert_8859_1_To_UTF8 was called with inStr == NULL");
      }
      if (size == 0)
      {
         Tr_Warn("Convert_8859_1_To_UTF8 was called with Destination Buffer size == 0");
      }
   }

}

/*===========================================================================*/
/*!
 * @file convert_8859_1_to_utf8.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 19-May-2012  Jim Huemann Revision 4
 *   - Fix Task 97753 for Task 97753 - Move include of debug trace to c file due to silverbox warning issue
 *
 * - 18-May-2012  Jim Huemann Revision 3
 *   - Task 97753 - Update Convert_8859_1_To_UTF8 to prevent warnings in earlier compiler
 *
 * - 12-Apr-2012  Jim Huemann Revision 2
 *   - Task 89840 - Added const to input string
 *
 * - 11-Apr-2012  Jim Huemann Revision 1
 *   - Created initial file.
 *
 \*===========================================================================*/
