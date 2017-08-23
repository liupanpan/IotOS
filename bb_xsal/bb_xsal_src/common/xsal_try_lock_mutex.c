/* -------------------------------------------------------------------------
 *
 * File:        xsal_try_lock_mutex.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Try_Lock_Mutex() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_MUTEX_MODULE_ID,  6); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Try_Lock_Mutex(SAL_Mutex_T* mutex)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require(mutex != NULL, "NULL Mutex");
   
   status = pthread_mutex_trylock(mutex);
   PBC_Ensure_1(
      (EOK == status) || (EBUSY == status), 
      "SAL_Try_Lock_Mutex error: %s", strerror(status));

   if (status == EBUSY)
   {
       Tr_Info_Lo("SAL_Try_Lock_Mutex: BUSY");
   }

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   DWORD status;

   PBC_Require(mutex != NULL, "NULL Mutex");

   status = WaitForSingleObject(*mutex, 0);
   if (status == WAIT_FAILED)
   {
      PBC_Failed_1("SAL_Try_Lock_Mutex error: %d", (int)GetLastError());
   }
   if (status == WAIT_TIMEOUT)
   {
       Tr_Info_Lo("SAL_Try_Lock_Mutex: BUSY");
   }
   return (bool_t)(status == WAIT_OBJECT_0);

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
