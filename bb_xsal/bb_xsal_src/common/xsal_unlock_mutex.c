/* -------------------------------------------------------------------------
 *
 * File:        xsal_unlock_mutex.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Unlock_Mutex() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_MUTEX_MODULE_ID, 4); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Unlock_Mutex(SAL_Mutex_T* mutex)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require(mutex != NULL, "NULL Mutex");

   status = pthread_mutex_unlock(mutex);
   if (EOK != status)
   {
      Tr_Fault_1("SAL_Unlock_Mutex error: %s", strerror(status));
   }
   
   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;

   PBC_Require(mutex != NULL, "NULL Mutex");

   status = (bool_t)(ReleaseMutex(*mutex) != FALSE);
   if (!status)
   {
      Tr_Fault_1("SAL_Unlock_Mutex error: %d", (int)GetLastError());
   }
   return status;

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
