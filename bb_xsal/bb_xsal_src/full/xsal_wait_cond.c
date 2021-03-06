/* -------------------------------------------------------------------------
 *
 * File:        xsal_wait_cond.c
 *
 * Copyright:   2007 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description:Implementation of the SAL_Wait_Cond() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_CONDVAR_MODULE_ID, 3); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Wait_Cond(SAL_Cond_T* cond, SAL_Mutex_T* mutex)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require(cond != NULL, "NULL condvar pointer");
   PBC_Require(mutex != NULL, "NULL mutex pointr");
  
   status = pthread_cond_wait(cond, mutex);
   PBC_Ensure_1(0 == status, "SAL_Wait_Cond error: %s", strerror(errno));

   return (bool_t)(status == 0);

#elif defined (WIN32)

   bool_t status;

   PBC_Require(cond != NULL, "NULL condvar pointer");
   PBC_Require(mutex != NULL, "NULL mutex pointr");

   status = (bool_t)(SignalObjectAndWait(*mutex, *cond, INFINITE, FALSE) == WAIT_OBJECT_0);
   PBC_Ensure(status, "Failed to signal object");

   SAL_Lock_Mutex(mutex);

   return status;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
