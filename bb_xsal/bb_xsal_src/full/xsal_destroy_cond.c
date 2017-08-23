/* -------------------------------------------------------------------------
 *
 * File:        xsal_destroy_cond.c
 *
 * Copyright:   2007 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Destroy_Cond() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_CONDVAR_MODULE_ID, 1); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Destroy_Cond(SAL_Cond_T* cond)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require(cond != NULL, "NULL condvar pointer");

   status = pthread_cond_destroy(cond);
   PBC_Ensure_1(EOK == status, "SAL_Destroy_Cond error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;

   PBC_Require(cond != NULL, "NULL condvar pointer");

   status = (bool_t)(CloseHandle(*cond) != FALSE);
   PBC_Ensure_1(status, "SAL_Destroy_Cond error: %d", (int)GetLastError());

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
