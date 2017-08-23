/* -------------------------------------------------------------------------
 *
 * File:        xsal_get_message.c 
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Get_Message() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_message_queue.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 10); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


const SAL_Message_T* SAL_Get_Message(const SAL_Queue_Iterator_T* iterator)
{
   PBC_Require(iterator != NULL, "NULL iterator");
   return &(*iterator)->message;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
