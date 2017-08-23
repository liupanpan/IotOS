/* -------------------------------------------------------------------------
 *
 * File:        xsal_run.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Run() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_engine.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_IPC_MODULE_ID, 5); /* Identify file for trace/assert purposes */

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


/** Main loop of the SAL application.
 */
int32_t SAL_Run(void)
{
   int32_t exit_status;

   if (SAL_I_Number_Of_Running_User_Threads == 0u)
   {
      Tr_Fault("SAL_Run: no user threads were created");
      exit_status = -1;
   }
   else
   {
      if (SAL_I_Before_Run())
      {
         SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_fnc = (SAL_Thread_Fnc_T)SAL_Run;
         exit_status = SAL_I_Run();
         SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_fnc = NULL;
         SAL_I_After_Run();
      }
      else
      {
         exit_status = -1;
      }
   }
   return exit_status;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
