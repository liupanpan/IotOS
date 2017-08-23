/* -------------------------------------------------------------------------
 *
 * File:        xsal_wait_semaphore.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description:Implementation of the SAL_Wait_Semaphore() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_SEM_MODULE_ID, 4); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require(sem != NULL, "NULL semaphore");
  
   do
   {
      status = sem_wait(sem);
      /* Restart if interrupted by signal handler */
  } while ((-1 == status) && (EINTR == errno));

   if (status)
   {
      Tr_Fault_1("SAL_Wait_Semaphore error: %s", strerror(errno));
   }

   return (bool_t)(status == 0);

#elif defined (WIN32)

   bool_t status;

   PBC_Require(sem != NULL, "NULL semaphore");

   status = (bool_t)(WaitForSingleObject(*sem, INFINITE) == WAIT_OBJECT_0);
   PBC_Ensure_1(status, "SAL_Wait_Semaphore error: %d", (int)GetLastError());

   return status;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 29-Nov-2012 Dan Carman
 *     Modified to handled when semaphore is unblocked early due to signal (EINTR)
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
