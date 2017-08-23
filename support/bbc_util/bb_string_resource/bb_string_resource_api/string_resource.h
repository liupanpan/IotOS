#ifndef STRING_RESOURCE_H
#   define STRING_RESOURCE_H
/*===========================================================================*/
/**
 * @file string_resource.h
 *
 * API to functions to support string handling
 *
 * %full_filespec:string_resource.h~kok_inf#2:incl:kok_basa#1 %
 * @version %version:kok_inf#2 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Mon Nov 25 13:58:19 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page string_util_page String Utility User Manual
 *
 * @section DESC DESCRIPTION:
 * String and memory manipulation block extends standard libraries with a few 
 * functions specific for radio projects:
 * 
 * #Str_Is_Blank_String - the function checks if given string is blank, i.e. 
 * it contains at least one character which will be displayed on the radio 
 * display. The codes of visible character could be project or font dependent 
 * therefore the function takes as a parameter a call-back function which shall 
 * return if given character is visible or not. The function shall be provided 
 * be user.  
 * 
 * #Str_Get_Sub_String - the function splits given - usually long string into 
 * one shorter sub-string with defined maximum length and rest of given string. 
 * It is necessary to provide a call-back function which returns if given 
 * character is visible on screen or not. Example:
 *
 * Input string:
 * This is long string to be split.
 *
 * Sub-string:
 * This is long string
 *
 * Rest of the input string:
 * to be split.
 *
 * #util_stricmp works the same as the C-library function strcmp, except the 
 * comparison is done in a case insensitive manner.
 *
 * #memexch is similar to the C-library function memcpy, except the memory areas
 * are swapped, retaining data from both.
 *
 * #util_strdup works the same as the C-library function, the caller is responsible
 * for freeing the memory allocated, which is referenced by the return of the
 * function. It is safe to call this function with a NULL string argument
 *
 * #util_strndup works the same as the C-library function, the caller is responsible
 * for freeing the memory allocated, which is referenced by the return of the
 * function. It is safe to call this function with a NULL string argument
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
 * @defgroup string_res_grp String manipulation services
 * @ingroup utilities_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/
#   if !defined(STRICMP_PROVIDED) && !defined(stricmp)
#      define stricmp util_stricmp 
#   endif /* STRICMP_PROVIDED */

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/
#   ifdef __cplusplus
extern "C"
{

#   endif                       /* __cplusplus */

/**
 * "Safely" calculates the size of a string by not looking for the NUL terminator
 * past the "max_size" location.
 *
 * @return The length of the string, limited to max_size. If no NUL terminator is
 *         encountered within max_size characters, them max_size is returned.
 *
 * @param s String whose length is determined (must NOT be NULL).
 * @param max_size Maximum offset from s to examine for the NUL terminator.
 *
 * @pre
 *    - s != NULL
 */
size_t Safe_Strlen(char const *s, size_t max_size);

/**
 * Attempts to concatenate the full "src" string to any string currently in
 * "dest" while guaranteeing that the total length of the result (in dest)
 * does not exceed the specified size. In addition, the resulting string
 * is guaranteed to always be NUL terminated. The "src" string may end
 * up truncated if it (and a NUL terminator) will not fit in its entirety
 * after the current contents of dest.
 *
 * @return the dest pointer passed in
 *
 * @param [in, out] dest pointer to buffer containing string to which the
 *                  src string is to be concatenated.
 * @param [in] src pointer to the NUL-terminated string to be concatenated
 * @param [in] dest_size the total number of bytes available in the dest buffer;
 *             i.e., any concatenated result plus NUL terminator must fit in
 *             this size.
 * @pre
 *    - dest != NULL
 *    - src != NULL
 */
char* Str_Full_Concat(char* dest, char const* src, size_t dest_size);

/**
 * Attempts to copy the full "src" string to the "dest" buffer while
 * guaranteeing that the "dest" buffer size ("dest_size") is not violated.
 * In addition, the resulting string is guaranteed to always be NUL terminated.
 * The copy of the "src" string in the "dest" buffer may end up truncated if it
 * (and a NUL terminator) will not fit into "dest".
 *
 * @return the dest pointer passed in
 *
 * @param [in, out] dest pointer to buffer into which a copy is to be made
 * @param [in] src pointer to the string to be copied
 * @param [in] dest_size the number of bytes available in the dest buffer.
 *
 * @pre
 *    - dest != NULL
 *    - src != NULL
 */
char* Str_Full_Copy(char* dest, char const* src, size_t dest_size);

/**
 * Concatenate up to "max_chars_from_src" characters from the "src" string to
 * any string currently in "dest". It is guaranteed that the total length of
 * the result (in dest) does not exceed the specified size (dest_size). In
 * addition, the resulting string is guaranteed to always be NUL terminated.
 * The "src" string may end up truncated if it (and a NUL terminator) will not
 * fit in its entirety after the current contents of dest. If the src string
 * has a NUL terminator prior to its max_chars_from_src character, then only
 * the characters up to the NUL terminator are used.
 *
 * @return the dest pointer passed in
 *
 * @param [in, out] dest Pointer to buffer containing string to which the
 *                  src string is to be concatenated.
 * @param [in] src Pointer to the string to be concatenated
 * @param [in] max_chars_from_src Maximum number of characters to be copied
 *                                from src; fewer, are copied if a NUL terminator
 *                                is encountered first.
 * @param [in] dest_size the total number of bytes available in the dest buffer;
 *             i.e., any concatenated result plus NUL terminator must fit in
 *             this size.
 * @pre
 *    - dest != NULL
 *    - src != NULL
 */
char* Str_Partial_Concat(char* dest, char const* src, size_t max_chars_from_src, size_t dest_size);

/**
 * Copies up to "max_chars_from_src" characters from the "src" string to
 * the "dest" location. It is guaranteed that "dest" buffer is not overflowed;
 * at most (dest_size-1) characters will be transfered to dest. In addition,
 * the resulting string is guaranteed to always be NUL terminated. The copy
 * of the "src" string in the "dest" buffer may end up truncated if it (and a
 * NUL terminator) will not fit in its entirety. If the src string has a NUL
 * terminator prior to its max_chars_from_src character, then only the
 * characters up to the NUL terminator are used.
 *
 * @return the dest pointer passed in
 *
 * @param [in, out] dest pointer to buffer into which a copy is to be made
 * @param [in] src pointer to the string to be copied
 * @param [in] max_chars_from_src Maximum number of characters to be copied
 *                                from src; fewer, are copied if a NUL terminator
 *                                is encountered first.
 * @param [in] dest_size the number of bytes available in the dest buffer.
 *
 * @pre
 *    - dest != NULL
 *    - src != NULL
 */
char* Str_Partial_Copy(char* dest, char const* src, size_t max_chars_from_src, size_t dest_size);

/**
 * Copy a portion (broken on word boundary) of a large string into a smaller string
 *
 * @return pointer to (end of sub-string)+1 in input string
 *
 * @param [out] dest_str      pointer to where to put the sub string
 * @param [in] src_str        pointer to a large string
 * @param [in] size           maximum size of sub-string
 * @param [in] break_on_word  if false copies a substring without looking for words
 * @param [in] str_char_is_blank pointer to callback function. The function shall return true
 *             if given character is visible on display
 *
 * @pre
 *    - dest_str != NULL
 *    - src_str != NULL
 *    - str_char_is_blank != NULL
 */
char_t *Str_Get_Sub_String(char_t * dest_str, const char_t * src_str, int16_t size, bool_t break_on_word,
   bool_t(*str_char_is_blank)(const char_t char_to_test));

/**
 * Determines if the string will be visible
 *
 * @return true if no displayable characters in string
 *
 * @param [in] buf pointer to a string
 * @param [in] str_char_is_blank pointer to callback function. The function shall return true
 *             if given character is visible on display
 *
 * @pre
 *    - buf != NULL
 *    - str_char_is_blank != NULL
 */
bool_t Str_Is_Blank_String(const char_t * buf, bool_t(*str_char_is_blank)(const char_t char_to_test));

/**
 * Tests whether the string pointed to by "str" is empty.
 *
 * @return true if the string pointed to by "str" is empty; false otherwise.
 *
 * @param str The pointer to be tested.
 *
 * @pre
 *    - str != NULL
 */
bool_t Str_Is_Empty(const char *str);

/**
 * Exchanges n bytes between an object indicated by src1 and an object indicated by src2
 *
 * @param [in] src1  pointer to beginning of first area to exchange
 * @param [in] src2  pointer to beginning of second area to exchange
 * @param [in] n     number of bytes to exchange
 *
 * @pre
 *    - src1 != NULL
 *    - src2 != NULL
 */
void memexch(void *src1, void *src2, size_t n);

/** 
 * Compare two strings, ignoring case 
 * 
 * @return: result of comparison ignoring case: 
 *          < 0 --> s1 is less than s2.  
 *            0 --> s1 is equal to s2.  
 *          > 0 --> s1 is greater than s2. 
 * 
 * @param [in] s1  pointer to string to be compared 
 * @param [in] s2  pointer to string to be compared 
 * 
 * @pre
 *    - s1 != NULL
 *    - s2 != NULL
 */
int util_stricmp(const char *s1, const char *s2);

/** 
 * Duplicate string by allocating enough memory and
 * copying string there
 *
 * IMPORTANT NOTE.- The caller is responsible for freeing 
 * the memory allocated by this function, the pointer
 * returned by this function shall be used to do so
 * 
 * @return: char pointer to new (duplicated) string
 *          NULL if not successful 
 * 
 * @param [in] str  pointer to string to be duplicated 
 */
char *util_strdup(const char * str);

/** 
 * Duplicate string by allocating enough memory and
 * copying string there, the new string will be restricted
 * to the number of characters specified in size.
 * The size of the duplicated string can be at 
 * most the size parameter plus 1 (considering the end of 
 * string character will always be added to the duplicated
 * string)
 * 
 * IMPORTANT NOTE.- The caller is responsible for freeing 
 * the memory allocated by this function, the pointer
 * returned by this function shall be used to do so
 *
 
 * @return: char pointer to new (duplicated) string
 *          NULL if not successful 
 * 
 * @param [in] str  pointer to string to be duplicated 
 * @param [in] size  max size of duplicated string
 */
char *util_strndup(const char * str, size_t size);

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Test for a NULL string pointer.
 *
 * @return true if the string has a NULL pointer; false otherwise.
 *
 * @param str The pointer to be tested.
 */
#define Str_Is_NULL(str) ((str) == NULL)

/**
 * Test for a NULL string pointer or empty string.
 *
 * @return true if the string has a NULL pointer or is empty; false if a
 *              non-empty string is pointed to by "str".
 *
 * @param str The pointer to be tested.
 */
#define Str_Is_NULL_Or_Empty(str) (((str) == NULL) || ((str)[0] == '\0'))

#   ifdef __cplusplus
}
#   endif /* __cplusplus */
/*===========================================================================*/
/**
 * @file string_resource.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 17-Jul-2013 Larry Piekarski Rev 1
 *   - Task 160933: This file was created from string_res.h~tck_aud#1 rev 12.
 *     The name was changed from string_res.h to string_resource.h and the
 *     API's for deprecated functions were removed. This is in support of
 *     converting bb_util from a block to a building block cluster.
 *
 * - 25-Nov-2013 Kirk Bailey Rev 2
 *   - Fixed Doxygen comments.
 *
 *===========================================================================*/
/** @} doxygen end group */
#endif                          /* STRING_RESOURCE_H */
