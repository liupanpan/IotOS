#ifndef CHAR_UTILS_HPP
#   define CHAR_UTILS_HPP
/*===========================================================================*/
/**
 * @file char_utils.hpp
 *
 * Collection of general purpose C++ utilities for strings.
 *
 * %full_filespec: char_utils.hpp~4:incl:kok_basa#1 %
 * @version %version: 4 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Wed Aug  1 11:08:21 2012 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Collection of general purpose C++ utilities for strings.
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
 * @section DEV DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @ingroup utilities_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/
#   include <string>

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

/**
 * Converts the UTF8 character sequence in the std::string src into wchar_t
 * values that are appended to the current contents of the dst wstring. Any
 * incomplete UTF8 sequence at the end of the string is skipped.
 *
 * This functions is identical to UTIL_Convert_UTF8_To_Wstring  with the
 * exception that this function does NOT clear the dst string before the
 * conversion is performed.
 *
 * @param dst Reference to wstring to which converstion is appended.
 * @param src Reference to string to be converted.
 *
 * @return Number of characters appended to src (excluding NUL
 *         terminator)
 */
size_t UTIL_Append_UTF8_To_Wstring(std::wstring &dst, const std::string &src);

/**
 * Converts src std::wstring to UTF8 characters that are appended to the dst
 * std::string.
 *
 * This functions is identical to UTIL_Convert_Wchar_To_UTF8 with the exception
 * that this function does NOT clear the dst string before the conversion is
 * performed.
 *
 * @param dst Reference to string to which converstion is appended.
 * @param src Reference to wstring to be converted.
 *
 * @return Number of characters appended to destination (excluding NUL
 *         terminator)
 */
size_t UTIL_Append_Wchar_To_UTF8(std::string &dst, const std::wstring &src);

/**
 * Calculates the length of the UTF8 conversion of a wide string (not including
 * the NUL terminator).
 *
 * @param src Reference to wstring to be converted to UTF8.
 *
 * @return The length of the UTF8 string that would result from converting
 *         src (not including the NUL terminator).
 */
size_t UTIL_Calculate_UTF8_Length(const std::wstring &src);

/**
 * Converts the UTF8 character sequence in a std::string into wchar_t values
 * in the dst wstring. The dst wstring is first cleared meaning that the only
 * contents of the dst wstring will be the conversion results. Any incomplete
 * UTF8 multi-byte sequence at the end of the string is skipped.
 *
 * This functions is identical to UTIL_Append_UTF8_To_Wstring  with the
 * exception that this function clears the dst string before the conversion is
 * performed.
 *
 * @param dst Reference to wstring into which converstion is placed.
 * @param src Reference to string to be converted.
 *
 * @return Number of characters appended to src (excluding NUL
 *         terminator)
 */
size_t UTIL_Convert_UTF8_To_Wstring(std::wstring &dst, const std::string &src);

/**
 * Converts the src std::wstring to UTF8 characters that are placed in the
 * dst std::string. The dst string is first cleared meaning that the only
 * contents of the dst string will be the UTF8 conversion results.
 *
 * This functions is identical to UTIL_Append_Wchar_To_UTF8 with the exception
 * that this function clears the dst string before the conversion is performed.
 *
 * @param dst Reference to string into which converstion is placed.
 * @param src Reference to wstring to be converted.
 *
 * @return Number of characters appended to destination (excluding NUL
 *         terminator)
 */
size_t UTIL_Convert_Wchar_To_UTF8(std::string &dst, const std::wstring &src);

/**
 * Converts a std::wstring to a char buffer containing UTF8 characters.
 *
 * Only if dst_length is zero, is a NULL dst pointer allowed; otherwise, for
 * dst_length > 0 a valid pointer is required. The conversion will write to
 * this dst buffer and will guarantee that no more than dst_length bytes are
 * written (including the NUL terminator). The dst result is guaranteed to
 * be NUL terminated even in the case where the conversion stops in order
 * to avoid going beyond dst_length.
 *
 * As soon as a wide character is encountered that cannot be fit into the
 * remaining dst buffer (leaving room for a NUL terminator), then dst buffer
 * is NUL terminated and the function returns. The value returned by this
 * function is the actual number of characters converted and placed in the
 * dst buffer (NOT counting the guaranteed NUL terminator). This means the
 * return value can be slightly less that dst_length, which would occur if
 * truncation occurred because a multi-byte wide character's UTF8
 * representation would not fit in the remaining space.
 *
 * A NUL terminator is always appended to dst (even when src is empty and no
 * characters are converted). This guarantees the dst result is always NUL
 * terminated and the number of bytes written is always one more than the
 * value returned (the extra byte being the NUL terminator).
 *
 * @param dst Pointer to buffer into which translated characters are to be
 *            placed.
 * @param src Reference to wstring to be converted.
 * @param dst_length Amount of space available starting at dst.
 *
 * @return Number of characters appended to destination (excluding NUL
 *         terminator); the actual number of bytes written is one more
 *         than this value (because a NUL terminator is always written -
 *         unless dst_length is zero).
 */
size_t UTIL_Convert_Wchar_To_UTF8(char *dst, const std::wstring &src, size_t dst_length);

/**
 * Converts a null terminated char string to a standard wstring
 *     char string is assumed to be UTF-8 data,
 *         however, if ill-formed (i.e., non-valid UTF-8 encoding) then
 *         string be will treated as 8859-1 (Latin 1)
 *     There are no size restriction
 *
 * @param str pointer to string to convert
 *
 * @return wstring equivalent
 */
std::wstring UTIL_C_String_To_Wstring(char const * str);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
 \*===========================================================================*/

/*===========================================================================*/
/*!
 * @file char_utils.hpp
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 01-Aug-2012 Dan Carman
 *    - Added function UTIL_C_String_To_Wstring
 *
 * - 05-Mar-2012 Kirk Bailey
 *   - Improved API, added Doxygen comments.
 *
 * - 01-Mar-2012 Kirk Bailey
 *   - Created initial file.
 *
 \*===========================================================================*/
/** @} doxygen end group */
#endif                          /* CHAR_UTILS_HPP */
