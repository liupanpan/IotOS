/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_shared_memory.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the internal XSAL shared memory
 *              functions.
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_shared_memory.h"
#include "xsal_i_assert.h"

#include <stdio.h>
#include <stdlib.h>

EM_FILENUM(XSAL_SHM_MODULE_ID, 0); /* Identify file for trace/assert purposes */


/* -----------------------
 *  Function definitions
 * ----------------------- */


void SAL_I_Create_Shared_Memory_Path(char_t* file_path, size_t path_sz,
                                     SAL_Shared_Memory_Key_T key,
                                     SAL_Shared_Memory_Key_T subkey)
{
   /* Default prefix for shared memory name
    */
#if defined (XSAL_POSIX)

   static const char_t* SAL_I_Shm_Default_Prefix = "/XSAL_Shm_";

#elif defined(WIN32)

   static const char_t* SAL_I_Shm_Default_Prefix = "XSAL_Shm_";

#else
#error "Not supported platform"
#endif /* XSAL_POSIX && WIN32 */

   /* Name of the environment variable with prefix for shared memory.
    *  If this variable is defined, then its value is used as a part of 
    *  shared memory file name. This allows to separate applications started
    *  by different users.
    */
   static const char_t* SAL_I_Shm_Env_Var = "XSAL_INSTANCE_NAME";

   const char_t* user_prefix = getenv(SAL_I_Shm_Env_Var);
   
   const char_t *path_fmt;

   int ret;
   unsigned int length = 0;

   if ((user_prefix == NULL) || (user_prefix[0] == '\0'))
   {
      path_fmt = "%s%d_%d";
      ret = snprintf(file_path, path_sz, path_fmt,
                        SAL_I_Shm_Default_Prefix, (int)key, (int)subkey);
      if (ret >= 0)
      {
         /* Get length if return code indicates no error */
         length = ret;
      }

      if ((ret < 0) || (length > path_sz))
      {
         PBC_Failed_3("Failed to create path %s_%d_%d",
                      SAL_I_Shm_Env_Var, (int)key, (int)subkey);
      }
   }
   else
   {
      path_fmt = "%s%s_%d_%d";
      ret = snprintf(file_path, path_sz, path_fmt,
                        SAL_I_Shm_Default_Prefix, user_prefix,
                        (int)key, (int)subkey);

      if (ret >= 0)
      {
         /* Get length if return code indicates no error */
         length = ret;
      }

      if ((ret < 0) || (length > path_sz))
      {
         PBC_Failed_4("Failed to create path %s%s_%d_%d",
                      SAL_I_Shm_Env_Var, user_prefix, (int)key, (int)subkey);
      }
   }
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 6
 *   - Task 1334: Fixed compiler warnings
 * -----------------------------------------------------------------------*/
