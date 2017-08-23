/* -------------------------------------------------------------------------
 *
 * File:        xsal_try_wait_semaphore.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Try_Wait_Semaphore() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_SEM_MODULE_ID, 6); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Try_Wait_Semaphore(SAL_Semaphore_T* sem)
{
#if defined (XSAL_POSIX)

   SAL_Int_T err;
   SAL_Int_T status;

   PBC_Require(sem != NULL, "NULL semaphore");

   status = sem_trywait(sem);
   err = errno;

   /** Do not print debug when function returns false
    */
   PBC_Ensure_1((status == 0) || (err == EAGAIN), "SAL_Try_Wait_Semaphore error: %s", strerror(err));
   
   return (bool_t)(status == 0);

#elif defined (WIN32)

   PBC_Require(sem != NULL, "NULL semaphore");

   /** Do not print debug when function returns false
    */
   return WaitForSingleObject(*sem, 0) == WAIT_OBJECT_0;

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
