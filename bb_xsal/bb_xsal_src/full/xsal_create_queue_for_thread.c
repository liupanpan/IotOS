/* -------------------------------------------------------------------------
 *
 * File:        xsal_create_thread_for_queue.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Create_Queue_For_Thread()
 *              function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_Q_MODULE_ID, 3); /* Identify file for trace/assert purposes */

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Create_Queue_For_Thread(
   SAL_Thread_Id_T thread_id,
   size_t queue_size,
   size_t message_size,
   void* (*alloc_buf)(size_t size),
   void (*free_buf)(void *buf))
{
   SAL_I_Thread_Attr_T* thread_attr;

   PBC_Require(thread_id > 0, "Illegal thread id");
   PBC_Require((size_t)thread_id <= SAL_I_Max_Number_Of_Threads, "Illegal thread id");
   
   thread_attr = &SAL_I_Thread_Table[thread_id];

   /** Queue should be able to store queue_size new messages when 
    *  thread is processing a current message. Therefore queue_size 
    *  parameter is increased by 1
    */
    return SAL_I_Create_Message_Queue(
      &thread_attr->message_queue, 
      queue_size + 1u,
      message_size,
      alloc_buf,
      free_buf);
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
