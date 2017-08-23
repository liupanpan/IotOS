/* -------------------------------------------------------------------------
 *
 * File:        xsal_wait_destroyed.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Wait_Destroyed() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_THREAD_MODULE_ID, 11); /* Identify file for trace/assert purposes */

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


void SAL_Wait_Destroyed(
   const SAL_Thread_Id_T thread_id_list[],
   uint32_t number_of_items)
{
   uint32_t i;

   for(i = 0; i < number_of_items; i++)
   {
      bool_t status;
      SAL_Thread_Id_T tid = thread_id_list[i];
   
      PBC_Require(tid >= 0, "Negative thread id");
      PBC_Require((size_t)tid <= SAL_I_Max_Number_Of_Threads, "Thread id > max");

      /** 'decrease' and 'increase' semaphore.
       *  Semaphore will always have value 0 or 1 and never exceed 
       *  the allowable value.
       */
      status = SAL_Wait_Semaphore(&SAL_I_Thread_Table[tid].thread_destroyed_sem);
      if (status)
      {
         status = SAL_Signal_Semaphore(&SAL_I_Thread_Table[tid].thread_destroyed_sem);
      }
      PBC_Ensure(status, "Unable to signal semaphore");
   }
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
