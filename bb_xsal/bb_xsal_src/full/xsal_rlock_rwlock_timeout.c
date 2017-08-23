/* -------------------------------------------------------------------------
 *
 * File:        xsal_rlock_rwlock_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *              Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Implementation of the SAL_RLock_RWLock_Timeout() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#elif defined (WIN32)

#include <sys/timeb.h>

#endif /* XSAL_POSIX & WIN32 */

EM_FILENUM(XSAL_RWLOCK_MODULE_ID, 4); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_RLock_RWLock_Timeout(
   SAL_RWLock_T* rw_lock, 
   uint32_t timeout_msec)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;
   struct timespec timeout;
   time_t  sec  = (time_t)(timeout_msec/1000u);
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

   status = pthread_rwlock_timedrdlock(rw_lock, &timeout);

   PBC_Ensure_1((EOK == status) || (ETIMEDOUT == status),
                "SAL_RLock_RWLock_Timeout error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;
   bool_t status_w = 0;
   struct timeb t1, t2;
   SAL_Int_T dt, dt1;

   PBC_Require(rw_lock != NULL, "NULL rw_lock pointer");

   ftime(&t1);

   status = SAL_Wait_Semaphore_Timeout(&rw_lock->read_sem, timeout_msec);

   ftime(&t2);

   dt = (SAL_Int_T)((t2.time - t1.time)*1000 + t2.millitm - t1.millitm);

   dt1 = abs(dt - timeout_msec);

   if (status)
   {
      rw_lock->number_of_readers++;

      if (rw_lock->number_of_readers == 1)
      {
         status_w = SAL_Wait_Semaphore_Timeout(&rw_lock->write_sem, dt1);

         if (!status_w)
         {
            rw_lock->number_of_readers--;
         }
      }
      else
      {
         status_w = 1;
      }  

      status = SAL_Signal_Semaphore(&rw_lock->read_sem);

      PBC_Ensure_1(status, "SAL_RLock_RWLock error: %d", (int)GetLastError());
   }

   return status && status_w;

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
 * -----------------------------------------------------------------------*/
