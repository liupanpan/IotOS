/* -------------------------------------------------------------------------
 *
 * File:        xsal_receive_from_timeout.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Receive_From_Timeout() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_receive.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 15); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


const SAL_Message_T* SAL_Receive_From_Timeout(
   SAL_Thread_Id_T thread_id,
   uint32_t timeout_ms)
{
   PBC_Require(thread_id > 0, "Invalid thread id");
   PBC_Require((size_t)thread_id <= SAL_I_Max_Number_Of_Threads, "Invalid thread id");

   return SAL_I_Receive_From_Timeout(&SAL_I_Thread_Table[thread_id], timeout_ms);
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
