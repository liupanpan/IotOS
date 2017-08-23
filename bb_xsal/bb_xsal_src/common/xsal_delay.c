/* -------------------------------------------------------------------------
 *
 * File:        xsal_delay.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Delay() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"

#if defined (XSAL_POSIX)
#include "unistd.h"
#endif /* XSAL_POSIX */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


void SAL_Delay(uint32_t microseconds)
{
#if defined (XSAL_POSIX)

   /** Resolution of QNX timers is microseconds
    */
   (void)usleep(microseconds);

#elif defined (WIN32)

    Sleep(microseconds/1000);

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
