/* -------------------------------------------------------------------------
 *
 * File:        xsal_wait_cond_timeout.c
 *
 * Copyright:   2007 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description:Implementation of the SAL_Wait_Cond_Timeout() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_time.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_CONDVAR_MODULE_ID, 4); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Wait_Cond_Timeout(
   SAL_Cond_T* cond,
   SAL_Mutex_T* mutex,
   uint32_t timeout_msec)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;
   SAL_I_Time_Spec_T timeout;

   PBC_Require(cond != NULL, "NULL condvar pointer");
   PBC_Require(mutex != NULL, "NULL mutex pointer");

   SAL_I_Get_Time(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);
  
   status = pthread_cond_timedwait(cond, mutex, &timeout);

   /** Do not print debug when function returns timeout value - this 
    *  isn't invalid behavior.
    */
   PBC_Ensure_1((0 == status) || (ETIMEDOUT == status), 
                "SAL_Wait_Cond_Timeout error: %s", strerror(status));

   return (bool_t)(status == 0);

#elif defined (WIN32)

   bool_t status;
   DWORD wait_status;

   PBC_Require(cond != NULL, "NULL condvar pointer");
   PBC_Require(mutex != NULL, "NULL mutex pointer");

   wait_status = SignalObjectAndWait(*mutex, *cond, (DWORD)timeout_msec, FALSE);
   if (wait_status == WAIT_OBJECT_0)
   {
      status = true;
   }
   else
   {
      status = false;

      PBC_Ensure_1(WAIT_TIMEOUT == wait_status, 
                   "SAL_Wait_Cond_Timeout error: %d", (int)GetLastError());
   }

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
