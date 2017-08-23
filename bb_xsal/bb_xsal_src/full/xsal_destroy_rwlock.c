/* -------------------------------------------------------------------------
 *
 * File:        xsal_destroy_rwlock.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *              Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Implementation of the SAL_Destroy_RWLock() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_RWLOCK_MODULE_ID, 2); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Destroy_RWLock(SAL_RWLock_T* rw_lock)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require(rw_lock != NULL, "NULL rw_lock pointer");

   status = pthread_rwlock_destroy(rw_lock);
   
   PBC_Ensure_1(EOK == status, "SAL_Destroy_RWLock error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;

   PBC_Require(rw_lock != NULL, "NULL rw_lock pointer");

   status = SAL_Destroy_Semaphore(&rw_lock->read_sem);

   PBC_Ensure_1(status, "SAL_Destroy_RWLock error: %d", (int)GetLastError());

   if (status)
   {
      status = SAL_Destroy_Semaphore(&rw_lock->write_sem);

      PBC_Ensure_1(status, "SAL_Destroy_RWLock error: %d", (int)GetLastError());
   }

   return status;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 * 
 * 8 March 2007, Urszula Gessing (Urszula.Gessing@delphi.com
 *   Added implementation for Windows.
 *
 * 12 Feb 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 * -----------------------------------------------------------------------*/
