/**
 * @file char_utils.cpp
 *
 * Implements C++ character utilities
 *
 * %full_filespec: char_utils.cpp~kok_inf#6:ascii:kok_basa#1 %
 * @version %version: kok_inf#6 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Mon Nov 25 14:00:48 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2012-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Implements utilities for basic C++ string conversions.
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRA TRACEABILITY INFO:
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
 * @section DESC DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/
#include "char_utils.hpp"
#include "pbc_trace.h"
#include <string.h>

/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 12); /**< IDs file for EM assert handling */

/** RFC 3629 limits range of UTF8 to be compatible with UTF16 */
#define MAX_UTF8_CHAR 0x10FFFF

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

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
 \*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

/* ==========================================================================
 *
 * Please refer to description in char_utils.hpp
 *
 * ========================================================================== */
size_t UTIL_Append_UTF8_To_Wstring(std::wstring &dst, const std::string &src)
{
   unsigned char const *usrc = (unsigned char const *) src.c_str();
   size_t src_length = src.length();
   size_t num_read = 0;
   size_t start_sz = dst.length();

   while (num_read < src_length)
   {
      unsigned char uch1 = usrc[num_read++];

      if ('\0' == uch1)
      {
         break;
      }
      else if (uch1 <= 0x7f) /* this is a one byte character */
      {
         dst.push_back(uch1);
      }
      else if (uch1 <= 0xdf) /* this is a 2 byte character */
      {
         unsigned char uch2;
         if (num_read >= src_length)
         {
            Tr_Fault("Incomplete 2 byte UTF8 sequence - character dropped");
            break;
         }
         uch2 = usrc[num_read++];
         dst.push_back(((uch1 & 0x1f) << 6) | (uch2 & 0x3f));
      }
      else if (uch1 <= 0xef) /* this is a 3 byte character */
      {
         unsigned char uch2, uch3;
         if (num_read + 1 >= src_length)
         {
            Tr_Fault("Incomplete 3 byte UTF8 sequence - character dropped");
            break;
         }
         uch2 = usrc[num_read++];
         uch3 = usrc[num_read++];
         dst.push_back(((uch1 & 0x0f) << 12) | ((uch2 & 0x3f) << 6) | (uch3 & 0x3f));
      }
      else /* This is a 4 byte character */
      {
         unsigned char uch2, uch3, uch4;
         if (num_read + 2 >= src_length)
         {
            Tr_Fault("Incomplete 4 byte UTF8 sequence - character dropped");
            break;
         }
         uch2 = usrc[num_read++];
         uch3 = usrc[num_read++];
         uch4 = usrc[num_read++];
         dst.push_back(((uch1 & 0x07) << 18) | ((uch2 & 0x3f) << 12) | ((uch3 & 0x3f) << 6) | (uch4 & 0x3f));
      }
   }

   return (dst.length() - start_sz);
}

/* ==========================================================================
 *
 * Please refer to description in char_utils.hpp
 *
 * ========================================================================== */
size_t UTIL_Append_Wchar_To_UTF8(std::string &dst, const std::wstring &src)
{
   size_t num_appended = 0;
   size_t src_length;
   wchar_t const *src_str;

   src_str = src.c_str();
   src_length = src.length();

   for (size_t i = 0; i < src_length; i++)
   {
      wchar_t wc = src_str[i];

      if (wc <= 0x7F)
      { /* 7 sig bits */
         dst.push_back(wc);
         num_appended++;
      }
      else if (wc <= 0x7FF)
      { /* 11 sig bits */
         dst.push_back(0xC0 | (unsigned char) (wc >> 6)); /* upper 5 bits */
         dst.push_back(0x80 | (unsigned char) (wc & 0x3F)); /* lower 6 bits */
         num_appended += 2;
      }
#if WCHAR_MAX - WCHAR_MIN < 0x10000
      else
      { /* 16 sig bits */
         dst.push_back(0xE0 | (unsigned char) (wc >> 12)); /* upper 4 bits */
         dst.push_back(0x80 | (unsigned char) ((wc >> 6) & 0x3F)); /* next 6 bits */
         dst.push_back(0x80 | (unsigned char) (wc & 0x3F)); /* lowest 6 bits */
         num_appended += 3;
      }
#else
      else if (wc <= 0xFFFF)
      { /* 16 sig bits */
         dst.push_back(0xE0 | (unsigned char) (wc >> 12)); /* upper 4 bits */
         dst.push_back(0x80 | (unsigned char) ((wc >> 6) & 0x3F)); /* next 6 bits */
         dst.push_back(0x80 | (unsigned char) (wc & 0x3F)); /* lowest 6 bits */
         num_appended += 3;
      }
      else if (wc <= MAX_UTF8_CHAR)
      { /* 21 sig bits */
         dst.push_back(0xF0 | (unsigned char) ((wc >> 18) & 0x07)); /* upper 3 bits */
         dst.push_back(0x80 | (unsigned char) ((wc >> 12) & 0x3F)); /* next 6 bits */
         dst.push_back(0x80 | (unsigned char) ((wc >> 6) & 0x3F)); /* next 6 bits */
         dst.push_back(0x80 | (unsigned char) (wc & 0x3F)); /* lowest 6 bits */
         num_appended += 4;
      }
      else
      {
         /* RFC 3629 limits range of UTF8 to be compatible with UTF16 */
         Tr_Fault_1("Dropped illegal wchar_t value: %lx", (long unsigned)wc);
      }
#endif
   } // for (size_t i = ...

   return num_appended;
}

/* ==========================================================================
 *
 * Please refer to description in char_utils.hpp
 *
 * ========================================================================== */
size_t UTIL_Calculate_UTF8_Length(const std::wstring &src)
{
   size_t utf8_length = 0;
   size_t src_length;
   wchar_t const *src_str;

   src_str = src.c_str();
   src_length = src.length();

   for (size_t i = 0; i < src_length; i++)
   {
      wchar_t wc = src_str[i];

      if (wc <= 0x7F)
      { /* 7 sig bits */
         utf8_length++;
      }
      else if (wc <= 0x7FF)
      { /* 11 sig bits */
         utf8_length += 2;
      }
#if WCHAR_MAX - WCHAR_MIN < 0x10000
      else
      { /* 16 sig bits */
         utf8_length += 3;
      }
#else
      else if (wc <= 0xFFFF)
      { /* 16 sig bits */
         utf8_length += 3;
      }
      else if (wc <= MAX_UTF8_CHAR)
      { /* 21 sig bits */
         utf8_length += 4;
      }
      else
      {
         /* RFC 3629 limits range of UTF8 to be compatible with UTF16 */
         Tr_Fault_1("Illegal wchar_t value: %lx", (long unsigned)wc);
      }
#endif
   } // for (size_t i =...
   return utf8_length;
}

/* ==========================================================================
 *
 * Please refer to description in char_utils.hpp
 *
 * ========================================================================== */
size_t UTIL_Convert_UTF8_To_Wstring(std::wstring &dst, const std::string &src)
{
   dst.clear();
   return UTIL_Append_UTF8_To_Wstring(dst, src);
}

/* ==========================================================================
 *
 * Please refer to description in char_utils.hpp
 *
 * ========================================================================== */
size_t UTIL_Convert_Wchar_To_UTF8(std::string &dst, const std::wstring &src)
{
   dst.clear();
   return UTIL_Append_Wchar_To_UTF8(dst, src);
}

/* ==========================================================================
 *
 * Please refer to description in char_utils.hpp
 *
 * ========================================================================== */
size_t UTIL_Convert_Wchar_To_UTF8(char *dst, const std::wstring &src, size_t dst_length)
{
   size_t num_appended = 0;

   PBC_Require((0 == dst_length) || (dst != NULL), "NULL destination for UTF8 conversion");

   if ((dst_length > 0) && (dst != NULL))
   {
      size_t num_remaining = dst_length;
      size_t src_length;
      wchar_t const *src_str;
      unsigned char *udst = (unsigned char *) dst;

      src_str = src.c_str();
      src_length = src.length();
      /*
       * Leave room for NUL terminator, which means unless there's room for
       * at least 2 more bytes, then we're done.
       */
      for (size_t i = 0; (i < src_length) && (num_remaining > 1); i++)
      {
         wchar_t wc = src_str[i];

         if (wc <= 0x7F)
         { /* 7 sig bits */
            *udst++ = wc;
            num_appended++;
            num_remaining--;
         }
         else if (num_remaining < 3)
         {
            break; // don't have room for 2 bytes plus NUL terminator
         }
        else if (wc <= 0x7FF)
         { /* 11 sig bits */
            *udst++ = (0xC0 | (unsigned char) (wc >> 6)); /* upper 5 bits */
            *udst++ = (0x80 | (unsigned char) (wc & 0x3F)); /* lower 6 bits */
            num_appended += 2;
            num_remaining -= 2;
         }
         else if (num_remaining < 4)
         {
            break; // don't have room for 3 bytes plus NUL terminator
         }
#if WCHAR_MAX - WCHAR_MIN < 0x10000
         else
         { /* 16 sig bits */
            *udst++ = (0xE0 | (unsigned char) (wc >> 12)); /* upper 4 bits */
            *udst++ = (0x80 | (unsigned char) ((wc >> 6) & 0x3F)); /* next 6 bits */
            *udst++ = (0x80 | (unsigned char) (wc & 0x3F)); /* lowest 6 bits */
            num_appended += 3;
            num_remaining -= 3;
         }
#else
         else if (wc <= 0xFFFF)
         { /* 16 sig bits */
            *udst++ = (0xE0 | (unsigned char) (wc >> 12)); /* upper 4 bits */
            *udst++ = (0x80 | (unsigned char) ((wc >> 6) & 0x3F)); /* next 6 bits */
            *udst++ = (0x80 | (unsigned char) (wc & 0x3F)); /* lowest 6 bits */
            num_appended += 3;
            num_remaining -= 3;
         }
         else if (num_remaining < 5)
         {
            break; // don't have room for 4 bytes plus NUL terminator
         }
         else if (wc <= MAX_UTF8_CHAR)
         { /* 21 sig bits */
            *udst++ = (0xF0 | (unsigned char) ((wc >> 18) & 0x07)); /* upper 3 bits */
            *udst++ = (0x80 | (unsigned char) ((wc >> 12) & 0x3F)); /* next 6 bits */
            *udst++ = (0x80 | (unsigned char) ((wc >> 6) & 0x3F)); /* next 6 bits */
            *udst++ = (0x80 | (unsigned char) (wc & 0x3F)); /* lowest 6 bits */
            num_appended += 4;
            num_remaining -= 4;
         }
         else
         {
            /* RFC 3629 limits range of UTF8 to be compatible with UTF16 */
            Tr_Fault_1("Dropped illegal wchar_t value: %lx", (long unsigned)wc);
         }
#endif
      } // for (size_t i =...

      *udst++ = ('\0'); // Always append a NUL when dst_length > 0
   }
   return num_appended;
}


/**
 * Convert UTF8 to Wide Character array
 *    if ill formed then assume Latin 1
 *
 * @param dstStr Address of wchar_t output array
 * @param srcStr null terminated input char string
 */
static void Char_To_WChar(wchar_t *dstStr, unsigned char const * srcStr)
{
   unsigned char const * s = srcStr;
   wchar_t * d = dstStr;
   bool_t invalid_utf8 = false;

   while ((*s != 0) && !invalid_utf8)
   {
      unsigned char ch = *s++;

      if (ch < 0x80) // this is a one byte character
      {
         *d++ = ch;
      }
      else if (0xC0 == (ch & 0xE0)) // this is a two byte character
      {
         unsigned char ch1 = *s++;

         if (ch1)
         {
            invalid_utf8 = (ch1 & 0xC0) != 0x80;

            *d++ = ((ch & 0x1F) << 6) + (ch1 & 0x3F);
         }
         else
         {
            s--; /* return pointer to null character */
         }
      }
      else if (0xE0 == (ch & 0xF0)) // this is a three byte character
      {
         unsigned char ch1 = *s++;

         if (ch1)
         {
            unsigned char ch2 = *s++;

            if (ch2)
            {
               invalid_utf8 = ((ch1 & 0xC0) != 0x80) || ((ch2 & 0xC0) != 0x80);

               *d++ = ((ch & 0x0F) << 12) + ((ch1 & 0x3F) << 6) + (ch2 & 0x3F);
            }
            else
            {
               s--; /* return pointer to null character */
            }
         }
         else
         {
            s--; /* return pointer to null character */
         }
      }
#if WCHAR_MAX - WCHAR_MIN >= 0x10000
      else if (0xF0 == (ch & 0xF8)) // this is a four byte character
      {
         unsigned char ch1 = *s++;
         if (ch1)
         {
            unsigned char ch2 = *s++;
            if (ch2)
            {
               unsigned char ch3 = *s++;

               if (ch3)
               {
                  invalid_utf8 = ((ch1 & 0xC0) != 0x80) || ((ch2 & 0xC0) != 0x80) || ((ch3 & 0xC0) != 0x80);

                  *d++ = ((ch & 0x07) << 18) + ((ch1 & 0x03F) << 12) + ((ch2 & 0x3F) << 6) + (ch3 & 0x3F);
               }
               else
               {
                  s--; /* return pointer to null character */
               }
            }
            else
            {
               s--; /* return pointer to null character */
            }
         }
         else
         {
            s--; /* return pointer to null character */
         }
      }
#endif
      else
      {
         invalid_utf8 = true;
      }
   }

   /*
    * If it is invalid UTF8 then re-copy the entire string as Latin 1
    */
   if (invalid_utf8)
   {
      s = srcStr;
      d = dstStr;

      while (*s != 0)
      {
         *d++ = *s++;
      }
   }

   *d = 0; /* place terminator */
}

/**
 * Determines worst case size of wchar array for str
 *    Inclues space for null terminator.
 *
 *    If str contains multi-byte UTF-8 codes, the actual 
 *    wchar size will be less than the return value. 
 *
 * @param str null terminated input char string to size
 * @return worst case size of wchar array for input str
 */
static size_t Wsize(char const * str)
{
   size_t size = 1;

   if (str != NULL)
   {
      size = strlen(str) + 1;
   }

   return size;
}

/*
 * Converts UTF-8 or 8859-1 (Latin-1) null terminated C string to a 
 *  c++ wide string
 */
std::wstring UTIL_C_String_To_Wstring(char const * str)
{
   wchar_t dest_buf[Wsize(str)];

   if (str != NULL)
   {
      Char_To_WChar(dest_buf, (unsigned char const *) str);
   }
   else
   {
      Tr_Warn("NULL string passed to UTIL_C_String_To_Wstring");
      dest_buf[0] = 0;
   }

   return ((std::wstring) dest_buf);
 }

/*===========================================================================*/
/*!
 * @file char_utils.cpp
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 01-Mar-2012 Kirk Bailey
 *   - Created initial file.
 *
 * - 02-Mar-2012 Kirk Bailey
 *   - Fixed code to automatically detect and work for systems with 2 byte
 *     wchar_t.
 *
 * - 01-Aug-2012 Dan Carman
 *    - Added function UTIL_C_String_To_Wstring
 *
 * - 17-Jul-2013 Brian Kopeschka Rev 5
 *    - task 161037 - modify Char_To_WChar() to properly cutoff a string to a character boundary
 *      which was cut off at a byte boindary.  Implementation by Dan Carman.
 *
 * - 25-Nov-2013 Kirk Bailey Rev 6
 *   - Fixed Doxygen comments.
 */
/*===========================================================================*/
