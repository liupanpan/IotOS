/* -------------------------------------------------------------------------
 *
 * File:        xsal_exit.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Exit() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_engine.h"
#include "xsal_i_thread.h"


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


void SAL_Exit(int32_t status)
{
   if (SAL_I_Number_Of_Running_User_Threads != 1)
   {
      /*
       * Signal the router thread to terminate unless this thread is the only
       * remaining XSAL thread. If it is the last one, then
       * SAL_I_Free_Thread_Resources will kill the router thread instead.
       */
      SAL_I_Stop_Router_Thread(status);
   }
   SAL_I_Free_Thread_Resources(SAL_I_Get_Thread_Id());
   SAL_I_Exit_Thread();
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
