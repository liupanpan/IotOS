/*===========================================================================*/
/**
 * @file xsal_clock.c
 *
 * Implementation of the XSAL light Clock function for Win32.
 *
 * %full_filespec:xsal_clock.c~kok_inf#7:csrc:kok_aud#2 %
 * @version %version:kok_inf#7 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Dec 15 10:36:14 2014 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *    Implements XSAL Clock function from ftime and subtracting off starting time
 *    to give a relative mS since first clock read.
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
 *         SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - HI7000/4 Series (HI7000/4 V.2.02, HI7700/4 V.2.02, and HI7750/4 V.2.02) User�s Manual
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - Time counts may vary, between process on windows, in a multi-process system, since 
 *     start_time would not be truely global.. 
 *
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "xsal.h"

#if defined (XSAL_POSIX)

#   include <time.h>

#elif defined (WIN32)

#   include <sys/types.h>
#   include <sys/timeb.h>

#else
#   error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/

#define CLOCK_START_NEEDS_SET ((long unsigned int) -1)

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

/**
 * seconds offset into the day 
 *   match clock time, don't use fix size integer
 */
static long unsigned int clock_start_time = CLOCK_START_NEEDS_SET;

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local CONST Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/*
 * Returns time in mS since midnight  (in most cases will return same time across processes) 
 */
SAL_Clock_T SAL_Clock(void)
{
   SAL_Clock_T mSec;

#if defined (XSAL_POSIX)

   struct timespec time_buf;

   (void)clock_gettime(CLOCK_MONOTONIC, &time_buf);
   
   if (CLOCK_START_NEEDS_SET == clock_start_time)
   {
      if (time_buf.tv_sec > (24 * 60 * 60)) /* if system clock is less 24 hours old, use it directly */
      {
         /* set start time to beginning of the day */
         clock_start_time = time_buf.tv_sec - ((time_buf.tv_sec - (6 * 60 * 60)) % (24 * 60 * 60));
      }
      else
      {
         clock_start_time = 0;
      }
    }

   mSec = (SAL_Clock_T) (((time_buf.tv_sec - clock_start_time) * 1000) + (time_buf.tv_nsec / 1000000));

#elif defined (WIN32)

   struct timeb time_buf;

   ftime(&time_buf);            /* time_buf contains time_t time and unsigned short millitm */

   if (CLOCK_START_NEEDS_SET == clock_start_time)
   {
      clock_start_time = time_buf.time - ((time_buf.time - (6 * 60 * 60)) % (24 * 60 * 60));      
   }
     
   mSec = (SAL_Clock_T) (((time_buf.time - clock_start_time) * 1000) + time_buf.millitm);

#else
#   error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */

   return mSec;
}

/*
 * Returns delta time between a timestamp and now
 */
SAL_Int_T SAL_Elapsed_Time(uint32_t timestamp)
{
   return (SAL_Clock() - timestamp);
}

/*
 * Returns delta time between two timestamps
 */
SAL_Int_T SAL_Delta_Time(SAL_Clock_T start_time, SAL_Clock_T end_time)
{
   return (SAL_Int_T) (end_time - start_time);
}

/*===========================================================================*/
/*!
 * @file xsal_clock.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12/15/14 - Dan Carman
 *    Changed SAL_Clock to use CLOCK_MONOTONIC to avoid discontinuities when system time is changed.
 *
 * - 4-Aug-2011 Dan Carman
 *    - Modified rounding of time to detect when clock starts at zero (embedded target versus host)
 *
 * - 27-Sept-2007 Dan Carman
 * 	- MISRA changes
 *    - Added OS specific type for SAL_Clock 
 *
 * - 24-May-2007 kirk bailey
 * 	- Fixed spelling of "Elasped".
 *
 * - 01-May-2007 Dan Carman
 * 	- Added implemenation of SAL_Elasped_Time
 *
 * - 23-Apr-2007 Dan Carman
 * 	- Created initial file.
 *
 */
/*===========================================================================*/
