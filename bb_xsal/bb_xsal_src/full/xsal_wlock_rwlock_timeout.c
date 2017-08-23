/* -------------------------------------------------------------------------
 *
 * File:        xsal_wlock_rwlock_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *              Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Implementation of the SAL_WLock_RWLock_Timeout() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_RWLOCK_MODULE_ID, 7); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_WLock_RWLock_Timeout(
   SAL_RWLock_T* rw_lock,
   uint32_t timeout_msec)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;
   struct timespec timeout;
   time_t  sec = (time_t)(timeout_msec/1000u);
   int32_t nsec = (int32_t)(timeout_msec%1000u)*1000000;

   PBC_Require(rw_lock != NULL, "NULL rw_lock pointer");

   (void)clock_gettime(CLOCK_REALTIME, &timeout);
   if ((timeout.tv_nsec + nsec) >= 1000000000L)
   {
      nsec -= 1000000000;
      sec++;
   }
   timeout.tv_sec  += sec;
   timeout.tv_nsec += nsec;

   status = pthread_rwlock_timedwrlock(rw_lock, &timeout);

   PBC_Ensure_1((EOK == status) || (ETIMEDOUT == status), 
                "SAL_WLock_RWLock_Timeout error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;

   PBC_Require(rw_lock != NULL, "NULL rw_lock pointer");

   status = SAL_Wait_Semaphore_Timeout(&rw_lock->write_sem, timeout_msec);

   if (status)
   {
      rw_lock->write = 1;
   }

   return status;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 8 March 2007, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Added implementation for Windows.
 *
 * 12 Feb 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
