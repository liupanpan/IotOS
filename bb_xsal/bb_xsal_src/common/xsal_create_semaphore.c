/* -------------------------------------------------------------------------
 *
 * File:        xsal_create_semaphore.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Create_Semaphore() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_SEM_MODULE_ID, 1); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Create_Semaphore(
   SAL_Semaphore_T* sem, 
   const SAL_Semaphore_Attr_T* attr)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;
   uint32_t init_value = (attr == NULL) ? 0u : attr->initial_value;

   PBC_Require(sem != NULL, "NULL semaphore");

   status = sem_init(sem, 0, init_value);
   PBC_Ensure_1((0 == status), "SAL_Create_Semaphore error: %s", strerror(errno));

   return (bool_t)(status == 0);

#elif defined (WIN32)

   LONG init_value = (attr == NULL) ? 0 : attr->initial_value;

   PBC_Require(sem != NULL, "NULL semaphore");

   *sem = CreateSemaphore(NULL, init_value, LONG_MAX, NULL);
   if (*sem == NULL)
   {
      PBC_Failed_1("SAL_Create_Semaphore error: %d", (int)GetLastError());
   }

   return (bool_t)(*sem != NULL);

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
