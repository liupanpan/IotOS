/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_time.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of SAL Time functions.
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_time.h"


/* -----------------------
 *  Function Definitions
 * -----------------------*/

 
void SAL_I_Get_Time(SAL_I_Time_Spec_T* time_spec)
{
#if defined (XSAL_POSIX)

   (void)clock_gettime(CLOCK_REALTIME, time_spec);

#elif defined (WIN32)

   (void)ftime(time_spec);

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}

void SAL_I_Get_Time_Monotonic(SAL_I_Time_Spec_T* time_spec)
{
#if defined (XSAL_POSIX)

   (void)clock_gettime(CLOCK_MONOTONIC, time_spec);

#elif defined (WIN32)

   (void)ftime(time_spec);

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}

uint32_t SAL_I_Time_Diff(
   const SAL_I_Time_Spec_T* time1_spec,
   const SAL_I_Time_Spec_T* time2_spec)
{
#if defined (XSAL_POSIX)

   uint32_t sec = (uint32_t)(time2_spec->tv_sec - time1_spec->tv_sec)*1000U;
   int32_t msec = (int32_t)(time2_spec->tv_nsec - time1_spec->tv_nsec)/1000000;

   return (msec >= 0) ? (sec + (uint32_t)msec) : (sec-(uint32_t)(-msec));

#elif defined (WIN32)

   uint32_t diff = 
      (uint32_t)(time2_spec->time - time1_spec->time)*1000U +
      (uint32_t)(time2_spec->millitm - time1_spec->millitm);

   return diff;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


void SAL_I_Time_Add_Offset(
   SAL_I_Time_Spec_T* time_spec,
   uint32_t time_offset)
{
#if defined (XSAL_POSIX)

   time_t  sec  = (time_t)(time_offset / 1000U);
   int32_t nsec = (int32_t)(time_offset % 1000u)*1000000;

   if ((time_spec->tv_nsec + nsec) >= 1000000000L)
   {
      nsec -= 1000000000;
      sec++;     
   }
   time_spec->tv_sec  += sec;
   time_spec->tv_nsec += nsec;

#elif defined (WIN32)

   time_t  sec  = (time_t)(time_offset / 1000U);
   int32_t msec = (int32_t)(time_offset % 1000U);

   if ((time_spec->millitm + msec) >= 1000)
   {
      msec -= 1000;
      sec++;     
   }
   time_spec->time  += sec;
   time_spec->millitm += msec;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 16-Dec-2014 Dan Carman
 *    Added Get function for Monotonic Time
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
