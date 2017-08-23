/* -------------------------------------------------------------------------
 *
 * File:        xsal_init.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Init function.
 *
 * ------------------------------------------------------------------------- */

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_dt.h"
#include "xsal_i_buffer_pools.h"
#include "xsal_i_engine.h"
#include "xsal_i_thread.h"
#include "xsal_i_timer.h"
#include "xsal_i_publish.h"
#include "xsal_i_event_property.h"
#if defined (XSAL)
#   include "xsal_i_ns_proxy.h"
#   include "xsal_i_connections.h"
#endif /* XSAL */
#include "xsal_i_version_string.h"

#include <stdlib.h>

EM_FILENUM(XSAL_IPC_MODULE_ID, 0); /* Identify file for trace/assert purposes */

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

bool_t SAL_Init(const SAL_Config_T* sal_config)
{
   bool_t status = true;

   Tr_Info_Hi(XSAL_VERSION_STRING);

   /** Check if SAL_I_App_Id is equal 0 - SAL_Init mustn't be called 
    *  more than once
    */
   PBC_Require(SAL_I_App_Id == 0, "Attempt to re-initialize XSAL");
   PBC_Require(sal_config != NULL, "NULL config pointer");
   PBC_Require(sal_config->app_id > SAL_NO_APP_ID, "Invalid app_id");
   PBC_Require(sal_config->app_id < SAL_LAST_APP_ID, "app_id > max");
   PBC_Require(sal_config->max_number_of_threads > 0u, "Invalid max # of threads");

   SAL_I_App_Id = sal_config->app_id;
   SAL_I_Max_Number_Of_Threads = sal_config->max_number_of_threads;
   SAL_I_Max_Number_Of_Timers = sal_config->max_number_of_timers;
   SAL_I_Max_Number_Of_Buffer_Pools = sal_config->max_number_of_buffer_pools;
#if defined (XSAL)
   SAL_I_Init_DT(); /* Init. debug trace data */
#endif /* XSAL */
   if (sal_config->first_unknown_thread_id == SAL_UNKNOWN_THREAD_ID)
   {
      SAL_First_Unknown_Thread_Id
         = (SAL_Thread_Id_T) sal_config->max_number_of_threads + 1;
   }
   else
   {
      PBC_Require(sal_config->first_unknown_thread_id > 0, "Invalid unknown thread id");
      PBC_Require(sal_config->first_unknown_thread_id <= (SAL_Thread_Id_T)sal_config->max_number_of_threads,
         "Invalid first unknown thread id");

      SAL_First_Unknown_Thread_Id = sal_config->first_unknown_thread_id;
   }

   status = status && SAL_I_Init_Thread_Module();
   status = status && SAL_I_Init_Buffer_Pools();
   status = status && SAL_I_Init_Timer_Module();
   status = status && SAL_I_Init_Publish_Module();
   status = status && SAL_I_Init_Event_Property_Module();

#if defined (XSAL)
   status = status && SAL_I_Init_Connection_Module();
#endif /* XSAL */

   return status;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 05-Jun-2012, Kirk Bailey: Fixed Klockwork errors.
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
