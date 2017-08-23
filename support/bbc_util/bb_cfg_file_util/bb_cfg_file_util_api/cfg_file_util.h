#ifndef CFG_FILE_UTIL_H
#define CFG_FILE_UTIL_H
/**
 * @file cfg_file_util.h
 *
 * Utility for managing the runtime configuration using the contents of a file
 *
 * %full_filespec:cfg_file_util.h~kok_inf#2:incl:kok_basa#2 %
 * @version %version:kok_inf#2 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Mon Nov 25 13:58:19 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Utility for managing the runtime configuration using the contents of a file
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
 *     - None
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup cfg_file_util Utility for managing configuration files
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

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
 * Checks for the existence of a file and reads the first valid line from it;
 * otherwise returns a default value.
 *
 * @param default_value Value to return if file doesn't exist or an error occurs.
 * @param file_name Name of file to check for value.
 *
 * @return A dynamically allocated string containing either the default value
 *         or the value from the first line of the file.
 *
 * @note It is the callers responsibility to free the pointer returned!
 */
char const *Get_Config_String_From_File(char const *default_value, char const *file_name);

/**
 * Frees memory allocated for a Config String ensuring the pointer is NULL
 * to prevent further deallocation.
 *
 * @param config_string pointer to the configuration string to deallocate
 */
void Free_Config_String(char const ** config_string);

#   ifdef __cplusplus
}         /* extern "C" */
#   endif /* __cplusplus */
/*===========================================================================*/
/*!
 * @file cfg_file_util.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 24-Jul-2013 Larry Piekarski
 *   - Created initial file (from a ADP GIT project from Kirk Bailey)
 *
 * - 25-Nov-2013 Kirk Bailey Rev 2
 *   - Fixed Doxygen comments.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* CFG_FILE_UTIL_H */
