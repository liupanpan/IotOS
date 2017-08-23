/* -------------------------------------------------------------------------
 *
 * File:        xsal_destroy_buffer_pool.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Destroy_Buffer_Pool() function.
 *
 * -----------------------------------------------------------------------*/

#include <stdlib.h>
#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

EM_FILENUM(XSAL_BUFFER_MODULE_ID, 2); /* Identify file for trace/assert purposes */

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

void SAL_Destroy_Buffer_Pool(SAL_Buffer_Pool_Id_T pool_id)
{
   SAL_I_Buffer_Pool_Header_T* pool_header;

   PBC_Require(pool_id >= 0, "Invalid pool id");
   PBC_Require((size_t )pool_id < SAL_I_Max_Number_Of_Buffer_Pools, "Invalid pool id");

   pool_header = &SAL_I_Buffer_Pools_Tab[pool_id];
   if (SAL_Lock_Mutex(&pool_header->pool_mutex))
   {
      free(pool_header->buffer);

      if (SAL_Lock_Mutex(&SAL_I_Buffer_Pools_Mutex))
      {
         pool_header->first_free_buffer_id = -1;
         pool_header->buffer = NULL;
         (void) SAL_Unlock_Mutex(&SAL_I_Buffer_Pools_Mutex);
      }
      (void) SAL_Unlock_Mutex(&pool_header->pool_mutex);
   }
   else
   {
      PBC_Failed("Unable to lock mutex for pool");
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 27-May-2015 Kirk Bailey
 *    - Filled in unresolved comments.
 *
 * -----------------------------------------------------------------------*/
