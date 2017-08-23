/* -------------------------------------------------------------------------
 *
 * File:        xsal_try_receive.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Receive() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 7); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


const SAL_Message_T* SAL_Try_Receive(void)
{
   SAL_I_Thread_Attr_T* queue_owner_thread = SAL_I_Get_Thread_Attr();

   PBC_Require(queue_owner_thread != NULL, "NULL owner thread attribute");
   PBC_Require_1(queue_owner_thread->thread_id > 0,
                 "Invalid thread id: %d", (int)queue_owner_thread->thread_id);
   PBC_Require_2((size_t)queue_owner_thread->thread_id <= SAL_I_Max_Number_Of_Threads,
                 "Thread id (%d) > max (%d)", (int)queue_owner_thread->thread_id, (int)SAL_I_Max_Number_Of_Threads);

   return SAL_I_Try_Receive_From(queue_owner_thread);
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 12 March 2008, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
