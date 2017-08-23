/* -------------------------------------------------------------------------
 *
 * File:        xsal_create_mutex.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Create_Mutex() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#if defined (XSAL_POSIX)

#include <errno.h>
#include <string.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_MUTEX_MODULE_ID, 1); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Create_Mutex(
   SAL_Mutex_T* mutex,
   const SAL_Mutex_Attr_T* attr)
{
#if defined (XSAL_POSIX)

   SAL_Int_T status;

   PBC_Require((mutex != NULL), "NULL mutex");

   if (attr == NULL)
   {
      /** Create mutex with default (system dependent) settings
       */
      status = pthread_mutex_init(mutex, NULL);
   }
   else
   {
      pthread_mutexattr_t ptmutex_attr;
      status = pthread_mutexattr_init(&ptmutex_attr);
      if (status == EOK)
      {
#if defined(QNX_NTO)

         status = pthread_mutexattr_setrecursive(
            &ptmutex_attr, 
            attr->recursive?
               PTHREAD_RECURSIVE_ENABLE:
               PTHREAD_RECURSIVE_DISABLE);

#elif defined(LINUX)

         if (attr->recursive)
         {
            status = pthread_mutexattr_settype(
               &ptmutex_attr, 
               PTHREAD_MUTEX_RECURSIVE);
         }

#else
#error "Not supported platform"
#endif /* QNX_NTO && LINUX */

         if (status == EOK)
         {
            status = pthread_mutex_init(mutex, &ptmutex_attr);
         }
         (void)pthread_mutexattr_destroy(&ptmutex_attr);
      }
   }

   PBC_Ensure_1((EOK == status), "SAL_Create_Mutex error: %s", strerror(status));

   return (bool_t)(status == EOK);

#elif defined (WIN32)

   PBC_Require((mutex != NULL), "NULL mutex");

   *mutex = CreateMutex(NULL, FALSE, NULL);
   if (*mutex == NULL)
   {
      PBC_Failed_1("SAL_Create_Mutex error: %d", (int)GetLastError());
   }

   return (bool_t)(*mutex != NULL);

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
