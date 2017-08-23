/*===========================================================================*/
/**
 * @file dbg_trace_c99.c
 *
 *   Implements utility functions for translating dbg_trace data into
 *   strings using C99 extensions to the C standard library.
 *
 * %full_filespec:dbg_trace_c99.c~kok_inf#17:csrc:kok_aud#1 %
 * @version %version:kok_inf#17 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Thu May 15 11:37:25 2014 %
 *
 *----------------------------------------------------------------------------
 *
 * Copyright 2010-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *----------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
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
 *   - None
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "pbc_trace.h"
#include "dbg_trace_c99.h"
#include "dbg_trace_cfg.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(DBG_USE_PID_TID_NAMES) && (DBG_USE_PID_TID_NAMES != 0)

#if defined(LINUX)
#include <sys/prctl.h>
#endif

extern char const *__progname;
#endif

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

#ifndef DBG_TRACE_PATH_SEPARATOR
#   define DBG_TRACE_PATH_SEPARATOR '/'
#endif

#ifndef LINUX_TID_NAME_SIZE
#   define LINUX_TID_NAME_SIZE 17
#endif

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

#define COMPACT_TIMESTAMP_FORMAT "[%u.%06u] %u:%u; %#x:%u; %s"
#define COMPACT_CPU_FORMAT       "[%u.%06u:%u.%06u] %u:%u; %#x:%u; %s"

#if defined(DBG_USE_PID_TID_NAMES) && (DBG_USE_PID_TID_NAMES != 0)

#   define VERBOSE_TIMESTAMP_FORMAT "[%u.%06u] %s:%s; %s:%u; %s - "
#   define VERBOSE_CPU_FORMAT       "[%u.%06u:%u.%06u] %s:%s; %s:%u; %s - "

#else

#   define VERBOSE_TIMESTAMP_FORMAT "[%u.%06u] %u:%u; %s:%u; %s - "
#   define VERBOSE_CPU_FORMAT       "[%u.%06u:%u.%06u] %u:%u; %s:%u; %s - "

#endif

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/**
 * Table used to translate Tr_Trace_Level_T values to strings.
 */
static char_t const *Trace_Levels[] =
{ "FAULT", "WARN", "NOTIFY", "INFO_HI", "INFO_MID", "INFO_LO" };

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/**
 * Generates the compact "header" for the log message which contains the
 * timestamp, app/thread information, etc. The format of this information is
 * based upon the compile-time option DBG_USE_THREAD_CPU_TIME.
 *
 * @return The number of bytes written to buf, -1 on error.
 *
 * @param buf Pointer to start of output string buffer.
 * @param buf_size Number of bytes available in buffer.
 * @param tr_data  Structure containing compact trace data to output.
 * @param tr_descr Text description of trace level.
 */
static ssize_t Generate_Compact_Header(char_t * buf, size_t buf_size,
                                       Tr_Compact_Data_T const *tr_data,
                                       char_t const *tr_descr)
{
   ssize_t num_written;

#if defined(DBG_USE_THREAD_CPU_TIME) && (DBG_USE_THREAD_CPU_TIME!=0)
   num_written = snprintf(buf, buf_size, COMPACT_CPU_FORMAT,
                          tr_data->timestamp.sec, (tr_data->timestamp.ns/1000),
                          tr_data->thr_cpu_time.sec, (tr_data->thr_cpu_time.ns / 1000),
                          (unsigned)tr_data->app_id, (unsigned)tr_data->thread_id,
                          (unsigned)tr_data->file_id, (unsigned)tr_data->line_num, (char const*)tr_descr);
#else
   num_written = snprintf(buf, buf_size, COMPACT_TIMESTAMP_FORMAT,
                          tr_data->timestamp.sec, (tr_data->timestamp.ns / 1000),
                          (unsigned) tr_data->app_id, (unsigned) tr_data->thread_id,
                          (unsigned) tr_data->file_id, (unsigned) tr_data->line_num, (char const*)tr_descr);
#endif

   return num_written;
}

/**
 * Generates the verbose "header" for the log message which contains the
 * timestamp, app/thread information, etc. The format of this information is
 * based upon the compile-time options DBG_USE_PID_TID_NAMES and
 * DBG_USE_THREAD_CPU_TIME.
 *
 * @return The number of bytes written to buf, -1 on error.
 *
 * @param buf Pointer to start of output string buffer.
 * @param buf_size Number of bytes available in buffer.
 * @param short_f_name Short version of name of file containing trace statement.
 * @param tr_data  Structure containing compact trace data to output.
 * @param tr_descr Text description of trace level.
 */
static ssize_t Generate_Verbose_Header(char_t * buf, size_t buf_size,
                                       char_t const *short_f_name,
                                       Tr_Compact_Data_T const *tr_data,
                                       char_t const *tr_descr)
{
   ssize_t num_written;

#if defined(DBG_USE_PID_TID_NAMES) && (DBG_USE_PID_TID_NAMES != 0)
   char tid_name[LINUX_TID_NAME_SIZE];
#ifdef PR_GET_NAME
   prctl(PR_GET_NAME, tid_name);
   tid_name[sizeof(tid_name)-1] = '\0';
#else
   snprintf(tid_name, sizeof(tid_name),"%d", tr_data->thread_id);
#endif

#   if defined(DBG_USE_THREAD_CPU_TIME) && (DBG_USE_THREAD_CPU_TIME!=0)
   num_written = snprintf(buf, buf_size, VERBOSE_CPU_FORMAT,
                          tr_data->timestamp.sec, (tr_data->timestamp.ns/1000),
                          tr_data->thr_cpu_time.sec, (tr_data->thr_cpu_time.ns/1000),
                          __progname, tid_name, short_f_name, (unsigned)tr_data->line_num, tr_descr);
#   else
   num_written = snprintf(buf, buf_size, VERBOSE_TIMESTAMP_FORMAT,
                          tr_data->timestamp.sec, (tr_data->timestamp.ns / 1000),
                          __progname, tid_name, short_f_name, (unsigned) tr_data->line_num, tr_descr);
#   endif

#else

#   if defined(DBG_USE_THREAD_CPU_TIME) && (DBG_USE_THREAD_CPU_TIME!=0)
   num_written = snprintf(buf, buf_size, VERBOSE_CPU_FORMAT,
                          tr_data->timestamp.sec, (tr_data->timestamp.ns/1000),
                          tr_data->thr_cpu_time.sec, (tr_data->thr_cpu_time.ns/1000),
                          (unsigned)tr_data->app_id, (unsigned)tr_data->thread_id,
                          (char const*)short_f_name, (unsigned)tr_data->line_num, (char const*)tr_descr);
#   else
   num_written = snprintf(buf, buf_size, VERBOSE_TIMESTAMP_FORMAT,
                          tr_data->timestamp.sec, (tr_data->timestamp.ns / 1000),
                          (unsigned) tr_data->app_id, (unsigned) tr_data->thread_id,
                          (char const*)short_f_name, (unsigned) tr_data->line_num, (char const*)tr_descr);
#   endif

#endif

   return num_written;
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_c99.h
 *
 *===========================================================================*/
int32_t Tr_Snprintf_Compact(char_t * buf, size_t buf_size, Tr_Compact_Data_T const *tr_data)
{
   ssize_t num_written;
   ssize_t total_num_written = 0;
   char_t const *tr_descr = "?";
   ssize_t num_left = buf_size;

   if ((NULL == buf) || (NULL == tr_data))
   {
      /*******/
      return 0;
      /*******/
   }

   if (tr_data->trace_level <= TR_LVL_INFO_LO)
   {
      tr_descr = Trace_Levels[tr_data->trace_level];
   }

   num_written = Generate_Compact_Header(buf, num_left, tr_data, tr_descr);

   if (num_written < 0)
   {
      total_num_written = num_written;
   }
   else if (num_written >= num_left)
   {
      total_num_written += num_written;
   }
   else
   {
      total_num_written += num_written;
      num_left -= num_written;

      if (tr_data->num_args > 0)
      {
         num_written = snprintf(buf + total_num_written, num_left, " -");

         if (num_written < 0)
         {
            total_num_written = num_written;
         }
         else if (num_written >= num_left)
         {
            total_num_written += num_written;
         }
         else
         {
            int32_t i;

            total_num_written += num_written;
            num_left -= num_written;

            for (i = 0; i < tr_data->num_args; i++)
            {
               num_written = snprintf(buf + total_num_written, num_left, " %#x", (unsigned) tr_data->args[i]);

               if (num_written < 0)
               {
                  total_num_written = num_written;
                  break;
               }
               else if (num_written >= num_left)
               {
                  total_num_written += num_written;
                  break;
               }
               else
               {
                  total_num_written += num_written;
                  num_left -= num_written;
               }
            }
         }
      }
   }
   if (buf_size > 0)
   {
      buf[buf_size - 1] = '\0'; /* Make sure to NUL terminate a truncated string */
   }
   return total_num_written; /* WARNING: Multiple returns in function body. */
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_c99.h
 *
 *===========================================================================*/
int32_t Tr_Snprintf_Verbose(char_t * buf, size_t buf_size, Tr_Compact_Data_T const *tr_data,
                            char_t const *f_name, char_t const *msg, va_list arg_ptr)
{
   char_t const *short_f_name = "?";
   ssize_t num_written;
   ssize_t total_num_written = 0;
   char_t const *tr_descr = "?";
   ssize_t num_left = buf_size;

   if ((NULL == buf) || (NULL == tr_data))
   {
      /*******/
      return 0;
      /*******/
   }

   if (f_name != NULL )
   {
      short_f_name = strrchr(f_name, DBG_TRACE_PATH_SEPARATOR);
      if (NULL == short_f_name)
      {
         short_f_name = f_name;
      }
      else
      {
         short_f_name++;
      }
   } /* if (f_name != NULL) */

   if (tr_data->trace_level <= TR_LVL_INFO_LO)
   {
      tr_descr = Trace_Levels[tr_data->trace_level];
   }

   num_written = Generate_Verbose_Header(buf, num_left, short_f_name, tr_data, tr_descr);

   if (num_written < 0)
   {
      total_num_written = num_written;
   }
   else if (num_written >= num_left)
   {
      total_num_written += num_written;
   }
   else if (msg != NULL )
   {
      total_num_written += num_written;
      num_left -= num_written;

      num_written = vsnprintf(buf + total_num_written, num_left, msg, arg_ptr);

      if (num_written < 0)
      {
         total_num_written = num_written;
      }
      else
      {
         total_num_written += num_written;
      }
   }
   if (buf_size > 0)
   {
      buf[buf_size - 1] = '\0'; /* Make sure to NUL terminate a truncated string */
   }
   return total_num_written; /* WARNING: Multiple returns in function body. */
}

/*===========================================================================*/
/*!
 * @file dbg_trace_c99.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 04-Nov-2011 Kirk Bailey Rev. 10
 *   - Added NOTIFY level.
 *
 * - 10-May-2012 kirk bailey Rev. 11
 *   - Added option for thread CPU time.
 *
 * - 12-Mar-2013 Kirk Bailey Rev. 12
 *   - Added DBG_ENABLE_64BIT_TIMESTAMP support.
 *
 * - 03-Oct-2013 Kirk Bailey REV 14
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 *
 *  - 03-Oct-2013 Kirk Bailey REV 15
 *   -  Added DBG_USE_PID_TID_NAMES option to use process/thread names instead
 *      of numbers.
  *
 * - 26-Feb-2014 Kirk Bailey REV 16
 *   - Fixed Klocwork defects caused by using char_t instead of char * for
 *     printf-type arguments.
 *
 *    15-May-2014 Dan Carman
 *    - Added conditional compiler around PR_Get_Name for QNX
*/
/*===========================================================================*/
