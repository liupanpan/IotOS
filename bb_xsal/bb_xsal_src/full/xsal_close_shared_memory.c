/* -------------------------------------------------------------------------
 *
 * File:        xsal_close_shared_memory.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Close_Shared_Memory() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_shared_memory.h"
#include <errno.h>

#if defined (XSAL_POSIX)

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>

#endif /* XSAL_POSIX */

EM_FILENUM(XSAL_SHM_MODULE_ID, 3); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Close_Shared_Memory(SAL_Shared_Memory_Handle_T* handle)
{
#if defined (XSAL_POSIX)

   bool_t status = true;

   PBC_Require(handle != NULL, "NULL handle");
   PBC_Require(handle->data != NULL, "NULL data");
   PBC_Require(handle->size > 0, "Non-positive size");
   PBC_Require(handle->fd >= 0, "Negative file descriptor");
   PBC_Require(handle->key >= 0, "Negative key");
   PBC_Require(handle->subkey >= 0, "Negative subkey");

   if (munmap(handle->data, handle->size) != 0)
   {
      Tr_Fault_3("SAL_Close_Shared_Memory/munmap, name=[%d, %d], error: %s", 
                 (int)handle->key, (int)handle->subkey, strerror(errno));
      status = false;
   }
   if (close(handle->fd) != 0)
   {
      Tr_Fault_3("SAL_Close_Shared_Memory/close, name=[%d, %d], error: %s", 
                 (int)handle->key, (int)handle->subkey, strerror(errno));
      status = false;
   }

   if (handle->owner != 0)
   {
      char_t file_path[PATH_MAX];

      SAL_I_Create_Shared_Memory_Path(file_path, PATH_MAX, handle->key, handle->subkey);
      if (shm_unlink(file_path)!= 0)
      {
         Tr_Fault_2("SAL_Close_Shared_Memory/shm_unlink (%s) error: %s", 
                    file_path, strerror(errno));
         status = false;
      }
   }

   return status;

#elif defined (WIN32)

   bool_t status = true;

   PBC_Require(handle != NULL, "NULL handle");

   if (!UnmapViewOfFile(handle->data) ||
      !CloseHandle(handle->handle))
   {
      Tr_Fault_1("SAL_Close_Shared_Memory error: %s", strerror(errno));
      status = false;
   }
   handle->handle = NULL;

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
