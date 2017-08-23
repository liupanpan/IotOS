/* -------------------------------------------------------------------------
 *
 * File:        xsal_rlock_rwlock.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *              Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Implementation of the SAL_RLock_RWLock() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_RWLOCK_MODULE_ID, 3); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public function definitions
 * ------------------------------ */


bool_t SAL_RLock_RWLock(SAL_RWLock_T* rw_lock)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require(rw_lock != NULL, "NULL rw_lock pointer");

   status = pthread_rwlock_rdlock(rw_lock);
   
   PBC_Ensure_1(EOK == status, "SAL_RLock_RWLock error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;
   bool_t status_w = 0;

   PBC_Require(rw_lock != NULL, "NULL rw_lock pointer");

   status = SAL_Wait_Semaphore(&rw_lock->read_sem);

   PBC_Ensure_1(status, "SAL_RLock_RWLock error: %d", (int)GetLastError());

   if (status)
   {
      rw_lock->number_of_readers++;

      if (rw_lock->number_of_readers == 1)
      {
         status_w = SAL_Wait_Semaphore(&rw_lock->write_sem);

         PBC_Ensure_1(status_w, "SAL_RLock_RWLock error: %d", (int)GetLastError());
      }
      else
      {
         status_w = 1;
      }

      if (status && status_w)
      {
         status = SAL_Signal_Semaphore(&rw_lock->read_sem);

         PBC_Ensure_1(status, "SAL_RLock_RWLock error: %d", (int)GetLastError());
      }
   }

   return status && status_w;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 12 Feb 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 8 March 2007, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Added implementation for Windows.
 *
 * -----------------------------------------------------------------------*/
