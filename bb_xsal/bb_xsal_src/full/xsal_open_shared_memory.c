/* -------------------------------------------------------------------------
 *
 * File:        xsal_open_shared_memory.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Open_Shared_Memory() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_shared_memory.h"

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

EM_FILENUM(XSAL_SHM_MODULE_ID, 2); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


bool_t SAL_Open_Shared_Memory(
   SAL_Shared_Memory_Key_T key, 
   SAL_Shared_Memory_Key_T subkey, 
   SAL_Shared_Memory_Open_Mode_T mode, 
   SAL_Shared_Memory_Handle_T* handle)
{
#if defined (XSAL_POSIX)

   bool_t status = false;
   SAL_Int_T  shm_fd;
   char_t file_path[PATH_MAX];
   SAL_Int_T open_rights = (mode == SAL_SHMEM_RDONLY)?O_RDONLY:O_RDWR;

   PBC_Require(handle != NULL, "NULL handle");
   PBC_Require(key >= 0, "Negative key");
   PBC_Require(subkey >= 0, "Negative subkey");

   SAL_I_Create_Shared_Memory_Path(file_path, PATH_MAX, key, subkey);

   shm_fd = shm_open(file_path, open_rights, 0);
   if (shm_fd >= 0)
   {
      struct stat file_stat;
      if (fstat(shm_fd, &file_stat) == 0)
      {
         SAL_Int_T mmap_rights = 
            (mode == SAL_SHMEM_RDONLY)?(PROT_READ):(PROT_READ | PROT_WRITE);

         void* shm = mmap(
            NULL, 
            (size_t)file_stat.st_size, 
            mmap_rights, 
            MAP_SHARED, 
            shm_fd, 
            0);
         if (shm != MAP_FAILED)
         {
            handle->fd     = shm_fd;
            handle->key    = key;
            handle->subkey = subkey;
            handle->size   = (size_t)file_stat.st_size;
            handle->data   = shm;
            handle->owner  = false;

            status = true;
         }
         else
         {
            Tr_Fault_1("SAL_Open_Shared_Memory/mmap error: %s", strerror(errno));
            (void)close(shm_fd);
         }
      }
      else
      {
         Tr_Fault_1("SAL_Open_Shared_Memory/fstat error: %s", strerror(errno));
         (void)close(shm_fd);
      }
   }
   else
   {
      Tr_Fault_2("SAL_Open_Shared_Memory/shm_open (%s) error: %s", 
                 file_path, strerror(errno));
   }
   return status;

#elif defined (WIN32)

   bool_t status = false;
   char_t file_path[PATH_MAX];
   DWORD access_rights = mode == SAL_SHMEM_RDWR?FILE_MAP_WRITE:FILE_MAP_READ;

   PBC_Require(handle != NULL, "NULL handle");
   PBC_Require(key >= 0, "Negative key");
   PBC_Require(subkey >= 0, "Negative subkey");

   SAL_I_Create_Shared_Memory_Path(file_path, PATH_MAX, key, subkey);

   handle->handle = OpenFileMapping(access_rights, FALSE, file_path);
   if (handle->handle != NULL)
   {
      handle->data = MapViewOfFile(handle->handle, access_rights, 0, 0, 0);
      if (handle->data != NULL)
      {
         status = true;
      }
      else
      { 
         Tr_Fault_3("SAL_Open_Shared_Memory/MapViewOfFile name=[%d %d], %d", 
                    (int)key, (int)subkey, (int)GetLastError());
         CloseHandle(handle->data);
      }
   }
   else
   { 
      Tr_Fault_3("SAL_Open_Shared_Memory/OpenFileMapping name[%d %d], %d",
                 (int)key, (int)subkey, (int)GetLastError());
   }
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
