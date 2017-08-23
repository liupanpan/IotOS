/* -------------------------------------------------------------------------
 *
 * File:        xsal_wait_semaphore_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Wait_Semaphore_Timeout() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_time.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_SEM_MODULE_ID, 5);
/* Identify file for trace/assert purposes */

/* -----------------------------
 *  Public function definition
 * -----------------------------*/

bool_t SAL_Wait_Semaphore_Timeout(SAL_Semaphore_T* sem, uint32_t timeout_msec)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;
   SAL_I_Time_Spec_T timeout;

   PBC_Require(sem != NULL, "NULL semaphore");

#if defined(QNX_NTO)
   SAL_I_Get_Time_Monotonic(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   do
   {
      status = sem_timedwait_monotonic(sem, &timeout);
      /* Restart if interrupted by signal handler */
   } while ((-1 == status) && (EINTR == errno));
#else
   SAL_I_Get_Time(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   do
   {
      status = sem_timedwait(sem, &timeout);
      /* Restart if interrupted by signal handler */
   } while ((-1 == status) && (EINTR == errno));
#endif

   /** Do not print debug when function returns timeout value - this 
    *  isn't invalid behavior.
    */
   if (status && (ETIMEDOUT != errno))
   {
      Tr_Fault_1("SAL_Wait_Semaphore_Timeout error: %s", strerror(errno));
   }

   return (bool_t)(status == 0);

#elif defined (WIN32)

   bool_t status;
   DWORD wait_status;

   PBC_Require(sem != NULL, "NULL semaphore");

   wait_status = WaitForSingleObject(*sem, (DWORD)timeout_msec);
   if (wait_status == WAIT_OBJECT_0)
   {
      status = true;
   }
   else
   {
      status = false;

      /** Do not print debug when function returns timeout value - this 
       *  isn't invalid behavior.
       */
      if (wait_status != WAIT_TIMEOUT)
      {
         PBC_Failed_1("SAL_Wait_Semaphore_Timeout error: %d", (int)GetLastError());
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
 * 29-Nov-2012 Dan Carman
 *     Modified to handled when semaphore is unblocked early due to signal (EINTR)
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
