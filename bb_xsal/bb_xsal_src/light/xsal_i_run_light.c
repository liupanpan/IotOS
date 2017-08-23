/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_run_light.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_I_Run() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal_i_engine.h"
#include "xsal_i_timer.h"


/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */


/** Function SAL_Wait_Semaphore is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);


/* ----------------------
 *  Function Definitions
 * ---------------------- */


bool_t SAL_I_Before_Run(void)
{
   SAL_I_Start_Timer_Module();
   return true;
}


int32_t SAL_I_Run(void)
{
   return SAL_I_RT_Light();
}


void SAL_I_After_Run(void)
{
   SAL_I_Stop_Timer_Module();

   /** XSAL Engine is not working anymore
    */
   (void)SAL_Wait_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_ready_sem);
   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].is_ready = false;

   /** "Unlock" threads calling SAL_Wait_Destroyed();
    */
   (void)SAL_Signal_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_destroyed_sem);
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek  Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
