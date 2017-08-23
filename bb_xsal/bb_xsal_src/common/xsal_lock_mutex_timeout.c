/* -------------------------------------------------------------------------
 *
 * File:        xsal_lock_mutex_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Lock_Mutex_Timeout() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_time.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_MUTEX_MODULE_ID, 5); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/
 

bool_t SAL_Lock_Mutex_Timeout(SAL_Mutex_T* mutex, uint32_t timeout_msec)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;
   SAL_I_Time_Spec_T timeout;

   PBC_Require(mutex != NULL, "NULL Mutex");

#if defined(QNX_NTO)
   SAL_I_Get_Time_Monotonic(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   status = pthread_mutex_timedlock_monotonic(mutex, &timeout);
#else
   SAL_I_Get_Time(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   status = pthread_mutex_timedlock(mutex, &timeout);
#endif
   if ((EOK != status) && (ETIMEDOUT != status))
   {
      Tr_Fault_1("SAL_Lock_Mutex_Timeout error: %s", strerror(status));
   }

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;
   DWORD wait_status;

   PBC_Require(mutex != NULL, "NULL Mutex");

   wait_status = WaitForSingleObject(*mutex, (DWORD)timeout_msec);
   if (wait_status == WAIT_OBJECT_0)
   {
      status = true;
   }
   else
   {
      status = false;
      if (wait_status != WAIT_TIMEOUT)
      {
         PBC_Failed_1("SAL_Lock_Mutex_Timeout error: %d", (int)GetLastError());
      }
   }
   return status;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 16-Dec-2014 Dan Carman
 *    Use QNX extension to use Monotonic Time for timeout
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
