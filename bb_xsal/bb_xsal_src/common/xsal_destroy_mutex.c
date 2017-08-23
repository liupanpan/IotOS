/* -------------------------------------------------------------------------
 *
 * File:        xsal_destroy_mutex.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Destroy_Mutex() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_MUTEX_MODULE_ID, 2); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Destroy_Mutex(SAL_Mutex_T* mutex)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require((mutex != NULL), "NULL mutex");

   status = pthread_mutex_destroy(mutex);
   
   PBC_Ensure_1((EOK == status), "SAL_Destroy_Mutex error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;

   PBC_Require((mutex != NULL), "NULL mutex");

   status = (bool_t)(CloseHandle(*mutex) != FALSE);

   if (!status)
   {
      PBC_Failed_1("SAL_Destroy_Mutex error: %d", (int)GetLastError());
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
