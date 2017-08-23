/* -------------------------------------------------------------------------
 *
 * File:        xsal_get_thread_name_self.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the xsal_get_thread_name_self() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_thread.h"


/** This is not a public function. It is used only by the DebugTrace.
 */
const char_t* SAL_Get_Thread_Name_Self(void)
{
   SAL_I_Thread_Attr_T* thread_attr = SAL_I_Get_Thread_Attr();

   /* Function returns NULL when SAL is not initialized or if 
    * it is called from NON-SAL thread    
    */
   return ((SAL_I_Thread_Table == NULL) || (thread_attr == NULL))?
      NULL : thread_attr->thread_name;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
