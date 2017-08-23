/* -------------------------------------------------------------------------
 *
 * File:        xsal_alloc_buffer.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Alloc_Buffer() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

EM_FILENUM(XSAL_BUFFER_MODULE_ID, 3); /* Identify file for trace/assert purposes */


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


void* SAL_Alloc_Buffer(SAL_Buffer_Pool_Id_T pool_id)
{
   uint8_t* buff = NULL;
   SAL_I_Buffer_Pool_Header_T* pool_header;

   PBC_Require(pool_id >= 0, "Invalid pool id");
   PBC_Require((size_t)pool_id < SAL_I_Max_Number_Of_Buffer_Pools, "Invalid pool id");

   pool_header = &SAL_I_Buffer_Pools_Tab[pool_id];

   if (SAL_Lock_Mutex(&pool_header->pool_mutex))
   {
      int32_t buffer_id = pool_header->first_free_buffer_id;
      if (buffer_id != -1)
      {
         /** Get pointer to the buffer header.
          */
         buff = &pool_header->buffer[(uint32_t)buffer_id*pool_header->buffer_size];

#if defined(BUFFER_POOL_DEBUG)

         /** Check if the allocated buffer isn't corrupted.
          *  First check buffer header
          */
         if (((uint32_t*)(void*)buff)[0] != 
            Get_Buffer_Id((uint32_t)pool_id, (uint32_t)buffer_id))
         {
            PBC_Failed_2("Buffer %d in pool %d is corrupted (invalid buffer header)", 
                         (int)pool_id, (int)buffer_id);
         }

         /** Next check debug pattern at the end of buffer.
          */
         if (((uint32_t*)(void*)&buff[pool_header->buffer_size-sizeof(uint32_t)])[0] != DBG_PATTERN)
         {
            PBC_Failed_2("Buffer %d in pool %d is corrupted (invalid debug pattern)",
                         (int)pool_id, (int)buffer_id);
         }
#endif
         pool_header->first_free_buffer_id = ((int32_t*)(void*)&(buff[sizeof(uint32_t)]))[0];
      }
      (void)SAL_Unlock_Mutex(&pool_header->pool_mutex);

      if (buffer_id == -1)
      {
          Tr_Fault_1("No free buffer in pool %d", (int)pool_id);
      }
   }
   else
   {
      PBC_Failed("Unable to lock mutex for pool");
   }

   return (buff == NULL) ? NULL: (&buff[sizeof(uint32_t)]);
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
