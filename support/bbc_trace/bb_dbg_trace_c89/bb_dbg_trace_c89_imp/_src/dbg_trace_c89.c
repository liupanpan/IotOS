/*===========================================================================*/
/**
 * @file dbg_trace_c89.c
 *
 *   Implements utility functions for translating dbg_trace data into
 *   strings using the C89 C standard library.
 *
 * %full_filespec:dbg_trace_c89.c~kok_inf#18:csrc:kok_aud#1 %
 * @version %version:kok_inf#18 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed Feb 26 13:09:10 2014 %
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

/*
 * This implementation is only for use on systems that don't support C99's
 * snprintf; therefore, the use of sprintf is allowed.
 */
#define DISABLE_DEPRECATED_SPRINTF 1


#include "pbc_trace.h"
#include "dbg_trace_c89.h"
#include "dbg_trace_cfg.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

#ifndef DBG_TRACE_PATH_SEPARATOR
#   define DBG_TRACE_PATH_SEPARATOR '/'
#endif

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

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
static char_t const *Trace_Levels[] = {
   "FAULT",
   "WARN",
   "NOTIFY",
   "INFO_HI",
   "INFO_MID",
   "INFO_LO"
};

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_c89.h
 *
 *===========================================================================*/
int32_t Tr_Sprintf_Compact(char_t * buf, size_t buf_size, Tr_Compact_Data_T const *tr_data)
{
   ssize_t num_written;
   ssize_t total_num_written = 0;
   char const *tr_descr = "?";
   ssize_t num_left = buf_size;

   if ((NULL == buf) || (NULL == tr_data) || (0 == buf_size))
   {
      /*******/
      return 0;
      /*******/
   }

   if (tr_data->trace_level <= TR_LVL_INFO_LO)
   {
      tr_descr = Trace_Levels[tr_data->trace_level];
   }

   num_written =
#if defined(DBG_USE_THREAD_CPU_TIME) && (DBG_USE_THREAD_CPU_TIME==true)
      sprintf(buf + total_num_written, "[%u.%06u:%u.%06u] %u:%u; %#x:%u; %s",
              tr_data->timestamp.sec, (tr_data->timestamp.ns / 1000),
              tr_data->thr_cpu_time.sec, (tr_data->thr_cpu_time.ns / 1000),
              (unsigned)tr_data->app_id,    (unsigned)tr_data->thread_id,
              (unsigned)tr_data->file_id,   (unsigned)tr_data->line_num, tr_descr);
#else
      sprintf(buf + total_num_written, "[%u.%06u] %u:%u; %#x:%u; %s",
              tr_data->timestamp.sec, (tr_data->timestamp.ns / 1000),
              (unsigned)tr_data->app_id,  (unsigned)tr_data->thread_id,
              (unsigned)tr_data->file_id, (unsigned)tr_data->line_num, tr_descr);
#endif
   if ((num_written < 0) || (num_written >= num_left))
   {
      /*
       * sprintf may have written past the allocated buffer and the effect
       * of this on the system are not predictable; so, an assert is thrown.
       * The assert is thrown without trace information since it is likely
       * that this logic is already being used during a trace and so another
       * trace would simply lead to recursive calls.
       */
      EM_Perform_Assert();
   }
   else
   {
      total_num_written += num_written;
      num_left -= num_written;

      if (tr_data->num_args > 0)
      {
         num_written = sprintf(buf + total_num_written, " -");

         if ((num_written < 0) || (num_written >= num_left))
         {
            /*
             * sprintf may have written past the allocated buffer. See detailed
             * description with first EM_Assert() call above.
             */
            EM_Perform_Assert();
         }
         else
         {
            int32_t i;

            total_num_written += num_written;
            num_left -= num_written;

            for (i = 0; i < tr_data->num_args; i++)
            {
               num_written = sprintf(buf + total_num_written, " %#x", (unsigned)tr_data->args[i]);

               if ((num_written < 0) || (num_written >= num_left))
               {
                  /*
                   * sprintf may have written past the allocated buffer. See
                   * detailed description with first EM_Assert() call above.
                   */
                  EM_Perform_Assert();
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
   buf[buf_size-1] = '\0'; /* Make sure to NUL terminate a truncated string */
   return total_num_written;    /* WARNING: Multiple returns in function body. */
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_c89.h
 *
 *===========================================================================*/
int32_t Tr_Sprintf_Verbose(char_t * buf, size_t buf_size,
                           Tr_Compact_Data_T const *tr_data, char_t const *f_name, char_t const *msg, va_list arg_ptr)
{
   char const *short_f_name = "?";
   ssize_t num_written;
   ssize_t total_num_written = 0;
   char const *tr_descr = "?";
   ssize_t num_left = buf_size;

   if ((NULL == buf) || (NULL == tr_data) || (0 == buf_size))
   {
      /*******/
      return 0;
      /*******/
   }

   if (f_name != NULL)
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
   }                            /* if (f_name != NULL) */

   if (tr_data->trace_level <= TR_LVL_INFO_LO)
   {
      tr_descr = Trace_Levels[tr_data->trace_level];
   }
#if defined(DBG_USE_THREAD_CPU_TIME) && (DBG_USE_THREAD_CPU_TIME==true)
   num_written = sprintf(buf + total_num_written, "[%u.%06u:%u.%06u] %u:%u; %s:%u; %s - ",
                         tr_data->timestamp.sec, (tr_data->timestamp.ns / 1000),
                         (unsigned)tr_data->thr_cpu_time.sec, (tr_data->thr_cpu_time.ns / 1000),
                         (unsigned)tr_data->app_id,    (unsigned)tr_data->thread_id,
                         short_f_name, (unsigned)tr_data->line_num, tr_descr);
#else
   num_written = sprintf(buf + total_num_written, "[%u.%06u] %u:%u; %s:%u; %s - ",
                         tr_data->timestamp.sec, (tr_data->timestamp.ns / 1000),
                         (unsigned)tr_data->app_id, (unsigned)tr_data->thread_id,
                         short_f_name, (unsigned)tr_data->line_num, tr_descr);
#endif
   if ((num_written < 0) || (num_written >= num_left))
   {
      /*
       * sprintf may have written past the allocated buffer and the effect
       * of this on the system are not predictable; so, an assert is thrown.
       * The assert is thrown without trace information since it is likely
       * that this logic is already being used during a trace and so another
       * trace would simply lead to recursive calls.
       */
      EM_Perform_Assert();
   }
   else if (msg != NULL)
   {
      total_num_written += num_written;
      num_left -= num_written;

      num_written = vsprintf(buf + total_num_written, msg, arg_ptr);

      if ((num_written < 0) || (num_written >= num_left))
      {
         /*
          * vsprintf may have written past the allocated buffer. See
          * detailed description with first EM_Assert() call above.
          */
         EM_Perform_Assert();
      }
      else
      {
         total_num_written += num_written;
      }
   }
   buf[buf_size-1] = '\0'; /* Make sure to NUL terminate a truncated string */
   return total_num_written;    /* WARNING: Multiple returns in function body. */
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_cbk.h
 *
 *===========================================================================*/
void Tr_Print_Stack_Backtrace(void)
{
   /* There is currently no implementation to output a Call Stack Backtrace for
    * C89 only systems. If you need a Call Stack Backtrace, you must use a C99
    * capable system.
    */
}

/*===========================================================================*/
/*!
 * @file dbg_trace_c89.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 09-may-2011 Kris Boultbee
 *   - Task kok_basa#30633 Revision 11
 *   Added empty implementation of the Tr_Print_Stack_Backtrace() function.
 *
 * - 04-Nov-2011 Kirk Bailey Rev. 12
 *   - Added NOTIFY level.
 *
 * - 10-May-2012 kirk bailey Rev. 13
 *   - Added option for thread CPU time.
 *
 * - 31-May-2012 kirk bailey Rev. 15
 *   - Added DISABLE_DEPRECATED_SPRINTF
 *
 * - 03-Oct-2013 Kirk Bailey REV 17
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 *
 * - 26-Feb-2014 Kirk Bailey REV 18
 *   - Fixed Klocwork defects caused by using char_t instead of char * for
 *     printf-type arguments.
 */
/*===========================================================================*/
