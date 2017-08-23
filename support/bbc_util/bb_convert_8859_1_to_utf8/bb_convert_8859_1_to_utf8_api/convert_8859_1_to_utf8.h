#ifndef CONVERT_8859_1_TO_UTF8_H
#   define CONVERT_8859_1_TO_UTF8_H
/*===========================================================================*/
/**
 * @file convert_8859_1_to_utf8.h
 *
 * API for functions used to perform convert_8859_1_to_utf8.
 *
 * %full_filespec:convert_8859_1_to_utf8.h~4:incl:kok_basa#1 %
 * @version %version:4 %
 * @author  %derived_by:xz152s %
 * @date    %date_modified:Sat May 19 07:08:23 2012 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page conversion_page convert_8859_1_to_utf8 User Manual
 *
 * @section DESC DESCRIPTION:
 *
 * The convert_8859_1_to_utf8 sub-module provides set of functions to convert 8859-1
 * character buffer to utf-8 characater buffer.
 *
 *
 * @section USAGE USAGE INSTRUCTIONS:
 *
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 *
 * @section ABBR ABBREVIATIONS:
 *   - 8859-1 also known as Latin-1
 *
 * @section TRA TRACEABILITY INFO:
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
 * @defgroup conversions_grp Data conversions
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
 * The function converts a 8859-1 encoded character array to a utf-8 character array.
 *
 * @param [in] outStr - output buffer
 * @param [in] size   - size of the outStr buffer
 * @param [in] inBuf  - buffer to convert
 *
 */
void Convert_8859_1_To_UTF8(char *outStr,size_t size, const char *inStr);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file conversions.h
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
/** @} doxygen end group */
#endif                          /* CONVERT_8859_1_TO_UTF8_H */
