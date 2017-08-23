/* -------------------------------------------------------------------------
 *
 * File:        xsal_get_thread_id.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Get_Thread_Id() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_thread.h"


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


SAL_Thread_Id_T SAL_Get_Thread_Id(void)
{
   return SAL_I_Get_Thread_Id();
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
