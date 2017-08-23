/* -------------------------------------------------------------------------
 *
 * File:        xsal_free_buffer.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Free_Buffer() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

EM_FILENUM(XSAL_BUFFER_MODULE_ID, 4); /* Identify file for trace/assert purposes */


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


void SAL_Free_Buffer(void* buffer)
{
   uint8_t* raw_buffer  = &((uint8_t*)buffer)[-(SAL_Int_T)sizeof(uint32_t)];
   uint32_t pool_id     = Get_Pool_Id(((uint32_t*)(void*)raw_buffer)[0]);
   uint32_t buffer_id   = Get_Buffer_Index(((uint32_t*)(void*)raw_buffer)[0]);
   SAL_I_Buffer_Pool_Header_T* pool_header;

   PBC_Require((size_t)pool_id < SAL_I_Max_Number_Of_Buffer_Pools, "Invalid pool id");
   PBC_Require((size_t)buffer_id < SAL_I_Buffer_Pools_Tab[pool_id].max_number_of_buffers, "Invalid buffer id");

   pool_header = &SAL_I_Buffer_Pools_Tab[pool_id];

#if defined(BUFFER_POOL_DEBUG)

   /** Check if the buffer pointer points to 
    *  the memory area owned by the buffer pool.
    */
   if (raw_buffer != &pool_header->buffer[buffer_id*pool_header->buffer_size])
   {
      PBC_Failed("Invalid buffer address");
   }

   /* Check if the released buffer isn't corrupted - check the buffer header.
    */
   if (((uint32_t*)(void*)raw_buffer)[0] != Get_Buffer_Id(pool_id, buffer_id))
   {
      PBC_Failed_2("Buffer %d in pool %d is corrupted (invalid buffer header)",
                   (int)buffer_id, (int)pool_id);
   }

   /** Next check debug pattern at the end of buffer.
    */
   if (((uint32_t*)(void*)&raw_buffer[pool_header->buffer_size-sizeof(uint32_t)])[0] != DBG_PATTERN)
   {
      PBC_Failed_2("Buffer %d in pool %d is corrupted (invalid debug pattern)",
                   (int)buffer_id, (int)pool_id);
   }

#endif

   if (SAL_Lock_Mutex(&pool_header->pool_mutex))
   {
      /** Put the given buffer at the front of the free buffers list
       */
      ((int32_t*)buffer)[0] = pool_header->first_free_buffer_id;
      pool_header->first_free_buffer_id = (int32_t)buffer_id;

      (void)SAL_Unlock_Mutex(&pool_header->pool_mutex);
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
