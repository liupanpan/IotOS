/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_buffer_pools.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the internal XSAL buffer pools
 *              functions.
 *
 * -----------------------------------------------------------------------*/

#include <stdlib.h>
#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_buffer_pools.h"

EM_FILENUM(XSAL_BUFFER_MODULE_ID, 0); /* Identify file for trace/assert purposes */


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


/* ---------------------------
 *  Local Object Definitions
 * --------------------------- */


/** Pointer to the table with buffer pool headers
 */
SAL_I_Buffer_Pool_Header_T* SAL_I_Buffer_Pools_Tab = NULL;


/** Mutes to synchronize access to the SAL_I_Buffer_Pools_Tab
 */
SAL_Mutex_T SAL_I_Buffer_Pools_Mutex;


/* -----------------------
 *  Function Definitions
 * ----------------------- */


bool_t SAL_I_Init_Buffer_Pools(void)
{
   bool_t status = false;

   PBC_Require(SAL_I_Buffer_Pools_Tab == NULL, "Attempt to re-initialize XSAL buffer pools");

   if (SAL_I_Max_Number_Of_Buffer_Pools > 0u)
   {
      SAL_I_Buffer_Pools_Tab = 
         malloc(SAL_I_Max_Number_Of_Buffer_Pools*sizeof(SAL_I_Buffer_Pool_Header_T));
      if (SAL_I_Buffer_Pools_Tab != NULL)
      {
         if (SAL_Create_Mutex(&SAL_I_Buffer_Pools_Mutex, NULL))
         {
            size_t i;
            for(i=0; i < SAL_I_Max_Number_Of_Buffer_Pools; i++)
            {
               (void)SAL_Create_Mutex(&SAL_I_Buffer_Pools_Tab[i].pool_mutex, NULL);
               SAL_I_Buffer_Pools_Tab[i].buffer = NULL;
            }
            status = true;
         }
         else
         {
            Tr_Fault("SAL_I_Init_Buffer_Pools/SAL_Create_Mutex() failed");
         }

         if (!status)
         {
            free(SAL_I_Buffer_Pools_Tab);
            SAL_I_Buffer_Pools_Tab = NULL;
         }
      }
      else
      {
         Tr_Fault("SAL_Init_Buffer_Pools/malloc() failed");
      }
   }
   else
   {
      Tr_Info_Lo("SAL_I_Init_Buffer_Pools: max number of buffer pools == 0");
      status = true; 
   }

   PBC_Ensure(status, "SAL_I_Init_Buffer_Pools failed");

   return status;
}


void SAL_I_Deinit_Buffer_Pools(void)
{
   if (SAL_I_Max_Number_Of_Buffer_Pools > 0u)
   {
      size_t i;
      
      PBC_Require(SAL_I_Buffer_Pools_Tab != NULL, "Attempt to deinit uninitialized buffer pools");

      for(i=0; i < SAL_I_Max_Number_Of_Buffer_Pools; i++)
      {
         (void)SAL_Destroy_Mutex(&SAL_I_Buffer_Pools_Tab[i].pool_mutex);
         free(SAL_I_Buffer_Pools_Tab[i].buffer);
      }
      (void)SAL_Destroy_Mutex(&SAL_I_Buffer_Pools_Mutex);
      free(SAL_I_Buffer_Pools_Tab);
      SAL_I_Buffer_Pools_Tab = NULL;
   }
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
