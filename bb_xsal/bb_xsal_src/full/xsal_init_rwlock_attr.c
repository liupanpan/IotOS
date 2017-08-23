/* -------------------------------------------------------------------------
 *
 * File:        xsal_init_rwlock_attr.c
 *
 * Copyright:   2007 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Init_RWLock_Attr() function.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

EM_FILENUM(XSAL_RWLOCK_MODULE_ID, 0); /* Identify file for trace/assert purposes */


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


void SAL_Init_RWLock_Attr(SAL_RWLock_Attr_T* attr)
{
   PBC_Require(attr != NULL, "NULL RWLock attribute pointer");
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Feb 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
