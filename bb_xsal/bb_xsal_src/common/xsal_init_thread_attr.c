/* -------------------------------------------------------------------------
 *
 * File:        xsal_init_thread_attr.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Init_Thread_Attr() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

EM_FILENUM(XSAL_THREAD_MODULE_ID, 1); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


void SAL_Init_Thread_Attr(SAL_Thread_Attr_T* attr)
{
   PBC_Require(attr != NULL, "NULL thread attribute");

   attr->id         = SAL_UNKNOWN_THREAD_ID;
   attr->priority   = SAL_DEFAULT_THREAD_PRIORITY;
   attr->name       = NULL;
   attr->stack_size = SAL_DEFAULT_THREAD_STACK_SIZE;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Feb 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
