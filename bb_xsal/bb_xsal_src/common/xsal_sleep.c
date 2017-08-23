/* -------------------------------------------------------------------------
 *
 * File:        xsal_sleep.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Sleep() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"

#if defined (XSAL_POSIX)
#include <unistd.h>
#endif /* XSAL_POSIX */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


void SAL_Sleep(uint32_t milliseconds)
{
#if defined (XSAL_POSIX)

   /** Resolution of QNX timers is microseconds
    */
   (void)usleep(milliseconds*1000u);

#elif defined (WIN32)

    Sleep(milliseconds);

#else
#error "Not supported platform"
#endif
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
