/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_time.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of SAL Time functions.
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_TIME_H
#define XSAL_I_TIME_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "reuse.h"
#include "xsal_settings.h"

#if defined (XSAL_POSIX)

#include <time.h>

typedef struct timespec SAL_I_Time_Spec_T;

#elif defined (WIN32)

#include <sys/types.h>
#include <sys/timeb.h>

typedef struct timeb SAL_I_Time_Spec_T;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */


void SAL_I_Get_Time(SAL_I_Time_Spec_T* time_spec);
void SAL_I_Get_Time_Monotonic(SAL_I_Time_Spec_T* time_spec);

uint32_t SAL_I_Time_Diff(
   const SAL_I_Time_Spec_T* time1_spec,
   const SAL_I_Time_Spec_T* time2_spec);

void SAL_I_Time_Add_Offset(
   SAL_I_Time_Spec_T* time_spec,
   uint32_t time_offset);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_TIME_H */

 
/* -------------------------------------------------------------------------
 * Revision history:
 *
 *
 * 16-Dec-2014 Dan Carman
 *    Added Get Monotonic Time
 * 
 * 15 Feb 2008, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
