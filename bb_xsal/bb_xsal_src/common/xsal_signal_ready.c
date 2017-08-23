/* -------------------------------------------------------------------------
 *
 * File:        xsal_signal_ready.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Signal_Ready() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_THREAD_MODULE_ID, 6); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


void SAL_Signal_Ready(void)
{
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();
   PBC_Require(thread_attr != NULL, "NULL thread attribute");

   if (!thread_attr->is_ready)
   {
      if (SAL_Signal_Semaphore(&thread_attr->thread_ready_sem))
      {
         thread_attr->is_ready = true;
      }
      else
      {
         PBC_Failed("SAL_Signal_Semaphore failed");
      }
   }
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
