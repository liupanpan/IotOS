/* -------------------------------------------------------------------------
 *
 * File:        xsal_is_thread_alive.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Is_Thread_Alive() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_THREAD_MODULE_ID, 5); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Is_Thread_Alive(SAL_Thread_Id_T thread_id)
{
   PBC_Require(thread_id > 0, "Thread id to small");
   PBC_Require((size_t)thread_id <= SAL_I_Max_Number_Of_Threads, "Thread id > max");

   return (bool_t)(SAL_I_Thread_Table[thread_id].thread_id != SAL_UNKNOWN_THREAD_ID);
} 


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
