/* -------------------------------------------------------------------------
 *
 * File:        xsal_receive_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Receive_Timeout() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 3); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


const SAL_Message_T* SAL_Receive_Timeout(uint32_t timeout_ms)
{
   SAL_I_Thread_Attr_T* queue_owner_thread = SAL_I_Get_Thread_Attr();

   PBC_Require(queue_owner_thread != NULL, "NULL owner thread attribute");
   PBC_Require_1(queue_owner_thread->thread_id > 0,
                 "Invalid thread id: %d",
                 (int)queue_owner_thread->thread_id);
   PBC_Require_2((size_t)queue_owner_thread->thread_id <= SAL_I_Max_Number_Of_Threads,
                 "Thread id (%d) > max (%d)",
                 (int)queue_owner_thread->thread_id, (int)SAL_I_Max_Number_Of_Threads);

   return SAL_I_Receive_From_Timeout(queue_owner_thread, timeout_ms);
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
