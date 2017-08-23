/*===========================================================================*/
/**
 * @file dbg_trace_ps.c
 *
 * This file provides an implementation of the Tr_PS_Read_Trace_Levels and
 * Tr_PS_Save_Trace_Levels functions using POSIX file I/O.
 *
 * %full_filespec:dbg_trace_ps.c~kok_inf#7:csrc:kok_basa#1 %
 * @version %version:kok_inf#7 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 09:34:06 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * It is possible to build this file on systems that do not support POSIX file
 * I/O by setting the configuration DBG_ENABLE_POSIX_LVL_IO to false. Doing
 * this will cause empty functions to be built instead.
 *
 * Because these functions are implemented in a separate compilation unit, it
 * is still possible to link against the dbg_trace library and substitute
 * alternative implementations of these functions. To do this, simply make
 * sure the alternative implementations are found earlier in the link stage
 * than the dbg_trace library.
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR = TRace
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
 *   - none
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "dbg_trace_cfg.h"
#include "dbg_trace_ps.h"
#include "pbc_trace.h"
#if (DBG_ENABLE_POSIX_LVL_IO == true)
#   include <errno.h>
#   include <fcntl.h>
#   include <stdlib.h>
#   include <string.h>
#   include <sys/stat.h>
#   include <unistd.h>
#endif /* if (DBG_ENABLE_POSIX_LVL_IO == true) */

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
EM_FILENUM(DBG_TRACE_MODULE_ID, 0);

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
 * Please refer to the detailed description in dbg_trace_ps.h
 *
 *===========================================================================*/
void Tr_PS_Read_Trace_Levels(char const *path, Tr_Trace_Level_T *levels, size_t num_levels)
{
#if (DBG_ENABLE_POSIX_LVL_IO == true)
   int fd;
   size_t expected_size = num_levels * sizeof(*levels);

   PBC_Require(path != NULL, "NULL path for storing trace levels");
   PBC_Require(levels != NULL, "NULL ptr. to trace level data");
   PBC_Require(TR_NUM_MODULE_IDS == num_levels, "Wrong number of trace levels");

   fd = open(path, O_RDONLY);

   if (fd < 0)
   {
      Tr_Info_Hi_2("Could not open %s (%s). Using default dbg_trace levels.", path, strerror(errno));
   }
   else
   {
      struct stat buf;
      int err;

      err = fstat(fd, &buf);

      if (err != 0)
      {
         Tr_Warn_2("Error fstat %s: %s", path, strerror(errno));
      }
      else if ((unsigned)buf.st_size != expected_size)
      {
         Tr_Warn_2("Incorrect size (%u) for file. Expecting %u.", (unsigned)buf.st_size, (unsigned)expected_size);
      }
      else
      {
         Tr_Trace_Level_T *tmp_levels = malloc(expected_size);

         if (NULL == tmp_levels)
         {
            Tr_Fault("Could not allocate memory required to read dbg trace levels");
         }
         else
         {
            size_t total_num_read = 0;

            while (total_num_read < expected_size)
            {
               ssize_t num_read = read(fd, (char*) tmp_levels + total_num_read, expected_size - total_num_read);

               if (num_read <= 0)
               {
                  Tr_Fault_2("read() from %s failed: %s", path, strerror(errno));
                  break; /* out of while loop */
               }
               total_num_read += num_read;
            }

            if (total_num_read != expected_size)
            {
               Tr_Fault_1("Wrong size of data read from %s. Using default dbg_trace levels.", path);
            }
            else
            {
               size_t mod_index;

               err = 0;

               for (mod_index = 0; mod_index < expected_size; mod_index++)
               {
                  if (tmp_levels[mod_index] > TR_LVL_INFO_LO)
                  {
                     err++;
                     break;
                  }
               }

               if (0 == err)
               {
                  Tr_Info_Hi_1("Using saved dbg_trace levels from %s.", path);
                  memcpy(levels, tmp_levels, expected_size);
               }
               else
               {
                  Tr_Fault_1("Persistent dbg_trace level out of range - ignoring file %s.", path);
               }
            }

            free(tmp_levels);
         }
      } /* if (NULL == tmp_levels) */

      close(fd);
   }
#endif /* if (DBG_ENABLE_POSIX_LVL_IO == true) */
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_ps.h
 *
 *===========================================================================*/
void Tr_PS_Save_Trace_Levels(char const *path, Tr_Trace_Level_T const *levels, size_t num_levels)
{
#if (DBG_ENABLE_POSIX_LVL_IO == true)
   int fd;
   size_t expected_size = num_levels * sizeof(*levels);

   PBC_Require(path != NULL, "NULL path for storing trace levels");
   PBC_Require(levels != NULL, "NULL ptr. to trace level data");
   PBC_Require(TR_NUM_MODULE_IDS == num_levels, "Wrong number of trace levels");

   fd = open(path, (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

   if (fd < 0)
   {
      Tr_Warn_2("Error opening %s: %s", path, strerror(errno));
   }
   else
   {
      size_t total_num_written = 0;

      while (total_num_written < expected_size)
      {
         ssize_t num_written = write(fd, (char*) levels + total_num_written, expected_size - total_num_written);

         if (num_written <= 0)
         {
            Tr_Fault_1("write() failed: %s", strerror(errno));
            break; /* out of while loop */
         }
         total_num_written += num_written;
      }

      if (total_num_written != expected_size)
      {
         if (ftruncate(fd, 0) != 0)
         {
            Tr_Fault_1("ftruncate() failed: %s", strerror(errno));
         }
      }

      if (fsync(fd) != 0)
      {
         Tr_Fault_1("fsync() failed: %s", strerror(errno));
      }
      close(fd);
   }
#endif /* if (DBG_ENABLE_POSIX_LVL_IO == true) */
}

/*===========================================================================*/
/*!
 * @file dbg_trace_ps.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 31-May-2011 kirk bailey
 *   - Created initial file.
 *
 * - 30-Nov-2011 kirk bailey
 *   - Removed inappropriate use of backtrace conditional compilation option.
 *
 * - 25-Jan-2012 kirk bailey
 *   - Fixed format specifier warning in trace message.
 *
 * - 04-Jun-2012 kirk bailey
 *   - Fixed Klockwork error - check result of malloc for NULL.
 *
 * - 20-Sep-2013 kirk bailey
 *   - Fixed signed/unsigned comparison warnings due to -Wextra.
 *
 * - 03-Oct-2013 Kirk Bailey REV 7
 *   - Fixed order of revision history comments - no functional changes
 */
/*===========================================================================*/
