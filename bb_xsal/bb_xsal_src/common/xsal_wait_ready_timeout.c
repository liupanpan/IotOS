/* -------------------------------------------------------------------------
 *
 * File:        xsal_wait_ready_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Wait_Ready_Timeout() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_THREAD_MODULE_ID, 8); /* Identify file for trace/assert purposes */

/* -----------------------------
 *  Public function definition
 * -----------------------------*/

bool_t SAL_Wait_Ready_Timeout(const SAL_Thread_Id_T thread_id_list[], uint32_t number_of_items, uint32_t timeout_msec)
{
   uint32_t i;
   bool_t status = true;
   /*
    * NOTE: The entries in thread_id_list are waited on in series until either
    *       a timeout occurs or until all are ready. The same timeout value is
    *       used for each of these; so, the "total" time waited has an upper bound
    *       of (number_of_items * timeout_msec). Since timeouts are not supposed to
    *       happen (and there is an upper bound here), adding complexity to adjust
    *       the timeout to account for each successful wait is not considered
    *       important at this time.
    */
   for (i = 0; status && (i < number_of_items); i++)
   {
      SAL_Thread_Id_T tid = thread_id_list[i];

      PBC_Require(tid >= 0, "Negative thread id");
      PBC_Require((size_t )tid <= SAL_I_Max_Number_Of_Threads, "Thread id > max");
      /*
       * 'Decrease' and 'increase' semaphore.
       *  After this operation semaphore will always have 
       *  value 0 or 1 and never exceed the allowable value.
       */
      if (SAL_Wait_Semaphore_Timeout(&SAL_I_Thread_Table[tid].thread_ready_sem, timeout_msec))
      {
         if (!SAL_Signal_Semaphore(&SAL_I_Thread_Table[tid].thread_ready_sem))
         {
            status = false;
         }
      }
      else
      {
         status = false;
      }
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
