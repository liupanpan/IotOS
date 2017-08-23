/* -------------------------------------------------------------------------
 *
 * File:        xsal_create_cond.c
 *
 * Copyright:   2007 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Create_Cond() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_CONDVAR_MODULE_ID, 0); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Create_Cond(
   SAL_Cond_T* cond,
   const SAL_Cond_Attr_T* attr)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;
   pthread_condattr_t condattr;

   PBC_Require(cond != NULL, "NULL convar pointer");

   /* set condition attribute to relative timeout for wait condition */
   pthread_condattr_init(&condattr);
   pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC);	

   /** Create conditional variable with default (system dependent) settings
    *  Current implementation doesn't use any attributes.
    */
   status = pthread_cond_init(cond, &condattr);

   PBC_Ensure_1(EOK == status, "SAL_Create_Cond error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   bool_t status;

   PBC_Require(cond != NULL, "NULL convar pointer");

   *cond = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
   status = (bool_t)(*cond != NULL);
   PBC_Ensure_1(status, "SAL_Create_Cond error: %d", (int)GetLastError());

   return status;

#else
#error "Not supported platform"
#endif /* XSAL_POSIX & WIN32 */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * - 20-Feb-2013 Wang Le
 *    - Modified condition variable attribute to relative timeout 
 *    - for waiting condition  
 *
 * 01 Sep 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
