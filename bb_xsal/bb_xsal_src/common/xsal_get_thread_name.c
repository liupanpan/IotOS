/* -------------------------------------------------------------------------
 *
 * File:        xsal_get_thread_name.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Get_Thread_Name() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_THREAD_MODULE_ID, 3); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


const char_t* SAL_Get_Thread_Name(SAL_Thread_Id_T thread_id)
{
   /** It is possible to get name of RouterThread
    */
   PBC_Require(thread_id >= SAL_ROUTER_THREAD_ID, "Thread id negative");
   PBC_Require((size_t)thread_id <= SAL_I_Max_Number_Of_Threads, "Thread id > max");

   return SAL_I_Thread_Table[thread_id].thread_name;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
