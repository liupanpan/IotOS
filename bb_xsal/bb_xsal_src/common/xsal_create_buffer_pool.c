/* -------------------------------------------------------------------------
 *
 * File:        xsal_create_buffer_pool.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Create_Buffer_Pool() function.
 *
 * -----------------------------------------------------------------------*/

#include <stdlib.h>
#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_buffer_pools.h"

EM_FILENUM(XSAL_BUFFER_MODULE_ID, 1); /* Identify file for trace/assert purposes */


/**
 * Buffer pool is organized as table with defined number of buffers. 
 * Each buffer in given buffer pool has the same size, specified by 
 * the user. Free buffers (not allocated yet by the user) are linked 
 * together in free buffer list. Index of the first free buffer is kept 
 * in structure Buffer_Pool_Header_T.
 *
 * If buffer isn't allocated by the user, then its memory is used by the 
 * free buffer list. The first four bytes of the data visible for the user
 * keeps index of the next free buffer. The last free buffer has value -1 
 * as the index of next free buffer. 
 *
 * Each buffer has 4 bytes header (not visible by user), where its pool id
 * and buffer id is kept. The header is used during conversion from pointer
 * to the buffer id (see function SAL_Get_Buffer_Id).
 *
 * In debug version (with defined macro BUFFER_POOL_DEBUG) buffer size is 
 * incremented by 4 bytes and some magic pattern is stored at the tail
 * of buffer. The pattern is used to check if the buffer is corrupted. It is 
 * checked every time when the buffer is allocated and released.
 */


/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */


/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);


static SAL_Buffer_Pool_Id_T SAL_I_Create_Buffer_Pool(
   SAL_Buffer_Pool_Id_T buffer_pool_id, 
   size_t number_of_buffers, 
   size_t buffer_size);


/* -----------------------------
 *  Local Function Definitions
 * ----------------------------- */


static SAL_Buffer_Pool_Id_T SAL_I_Create_Buffer_Pool(
   SAL_Buffer_Pool_Id_T buffer_pool_id, 
   size_t number_of_buffers, 
   size_t buffer_size)
{
   uint8_t* buff;
   size_t alignment_buffer_size;
   SAL_I_Buffer_Pool_Header_T* buffer_pool = &SAL_I_Buffer_Pools_Tab[buffer_pool_id];

   /** Set the 'alignment_buffer_size' value to the nearest value 
    *  divided by sizeof(size_t) and at least equal 'buffer_size' value.
    *  Next buffer size is increased by 4 bytes to store parent
    *  buffer pool id and buffer_id.
    *  In debug version each buffer has also 4 bytes suffix for 
    *  debug pattern.
    */
#if defined(BUFFER_POOL_DEBUG)

   alignment_buffer_size = 
      (((buffer_size+sizeof(size_t)-1u)/sizeof(size_t))*sizeof(size_t)) /* aligment buffer size */
      + sizeof(uint32_t)  /* space to store pool id and buffer id */
      + sizeof(uint32_t); /* space to store debug pattern         */

#else

   alignment_buffer_size = 
      (((buffer_size+sizeof(size_t)-1u)/sizeof(size_t))*sizeof(size_t)) /* aligment buffer size */
      + sizeof(uint32_t);                               /* space to store pool id and buffer id */

#endif

   buff = malloc(number_of_buffers*alignment_buffer_size);
   if (buff != NULL)
   {
      size_t i;

      buffer_pool->buffer = buff;
      buffer_pool->buffer_size = alignment_buffer_size;
      buffer_pool->max_number_of_buffers = number_of_buffers;
      buffer_pool->first_free_buffer_id = 0;

      /** Initialize list of free buffers 
       *  and buffer id and pool id in buffer header.
       */
      for(i=0; i < (number_of_buffers-1u); i++)
      {
         /** Initialize buffer header.
          */
         ((uint32_t*)(void*)buff)[0] = Get_Buffer_Id((uint32_t)buffer_pool_id, i);

         /** Initialize list of free buffers.
          *  In the first 4 bytes of buffer data 
          *  store index to the next free buffer.
          */
         ((int32_t*)(void*)buff)[1] = (int32_t)i + 1;

#if defined(BUFFER_POOL_DEBUG)
         ((uint32_t*)(void*)&buff[alignment_buffer_size-sizeof(uint32_t)])[0] = DBG_PATTERN;
#endif /* BUFFER_POOL_DEBUG */
         buff = &buff[alignment_buffer_size];
      }

      /** Initialize the last buffer data
       */
      ((uint32_t*)(void*)buff)[0] = Get_Buffer_Id((uint32_t)buffer_pool_id, i);
      ((int32_t*)(void*)buff)[1] = -1;

#if defined(BUFFER_POOL_DEBUG)
      ((uint32_t*)(void*)&buff[alignment_buffer_size-sizeof(uint32_t)])[0] = DBG_PATTERN;
#endif
   }
   else
   {
      Tr_Fault("SAL_I_Create_Buffer_Pool/malloc() failed");
      buffer_pool_id = -1;
   }
   return buffer_pool_id;
}


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Create_Buffer_Pool(
   uint32_t number_of_buffers,
   size_t buffer_size,
   SAL_Buffer_Pool_Id_T* pool_id)
{
   bool_t status = false;

   PBC_Require(number_of_buffers > 0, "No buffers available");
   PBC_Require(number_of_buffers <= 65535u, "Too many buffers");
   PBC_Require(buffer_size > 0, "Invalid buffer size");
   PBC_Require(pool_id != NULL, "NULL pool id");
   PBC_Require(SAL_I_Max_Number_Of_Buffer_Pools > 0u, "No pools available");

   if (SAL_I_Max_Number_Of_Buffer_Pools > 0u)
   {
      if (SAL_Lock_Mutex(&SAL_I_Buffer_Pools_Mutex))
      {
         size_t i;
         bool_t searching = true;

         /** buffer_size must be >= sizeof(int32_t) 
          *  to store list of free buffers.
          */
         buffer_size = (buffer_size < sizeof(int32_t))?sizeof(int32_t):buffer_size;

         /** Find first free buffer pool
          */
         for(
            i = 0; 
            searching && (i < SAL_I_Max_Number_Of_Buffer_Pools);
            i++)
         {
            if (SAL_I_Buffer_Pools_Tab[i].buffer == NULL)
            {
               *pool_id = SAL_I_Create_Buffer_Pool(
                  (SAL_Buffer_Pool_Id_T)i,
                  number_of_buffers, 
                  buffer_size);
               if (*pool_id != -1)
               {
                  status = true;
               }
               searching = false;
            }
         }
         (void)SAL_Unlock_Mutex(&SAL_I_Buffer_Pools_Mutex);

         if (i >= SAL_I_Max_Number_Of_Buffer_Pools)
         {
             Tr_Fault("SAL_Create_Buffer_Pool: no free buffer pool");
         }
      }
   }
   else
   {
      Tr_Info_Lo("SAL_Create_Buffer_Pool: max number of buffer pools == 0");
   }

   return status;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
  *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
