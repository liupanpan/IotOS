/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_connections.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of Asynchronous Message Passing and 
 *              Event Management
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_connections.h"
#include "xsal_i_assert.h"

EM_FILENUM(XSAL_IPC_MODULE_ID, 2); /* Identify file for trace/assert purposes */

/* ---------------------
 *  Object Definitions
 * --------------------- */

/** List of connections to other SAL processes.
 */
SAL_Connection_T SAL_I_Connection_Id[SAL_MAX_APPS + 1];

/** Mutex synchronizes access to the SAL_I_Connection_Id table.
 */
SAL_Mutex_T SAL_I_Connection_Id_Mutex;

/* -----------------------
 *  Function Definitions
 * ----------------------- */

bool_t SAL_I_Init_Connection_Module(void)
{
   bool_t status = SAL_Create_Mutex(&SAL_I_Connection_Id_Mutex, NULL);
   if (!status)
   {
      Tr_Fault("SAL_Init: SAL_I_Connection_Id_Mutex");
   }
   return status;
}

void SAL_I_Deinit_Connection_Module(void)
{
   size_t i;

   /** Start from connection with index 1. 
    *  Connection with index 0 is not used.
    */
   for (i = 1; i <= SAL_MAX_APPS; i++)
   {
      if (SAL_Is_Connected(&SAL_I_Connection_Id[i]))
      {
         (void) SAL_Disconnect(&SAL_I_Connection_Id[i]);
      }
   }
   (void) SAL_Destroy_Mutex(&SAL_I_Connection_Id_Mutex);
}

SAL_Connection_T* SAL_Get_Connection(SAL_App_Id_T app_id)
{
   PBC_Require(app_id > 0, "Illegal app id");
   PBC_Require(app_id <= SAL_MAX_APPS, "Illegal app id");

   if (!SAL_Is_Connected(&SAL_I_Connection_Id[app_id]))
   {
      if (SAL_Lock_Mutex(&SAL_I_Connection_Id_Mutex))
      {
         (void) SAL_Connect(app_id, SAL_ROUTER_THREAD_ID, &SAL_I_Connection_Id[app_id]);
         (void) SAL_Unlock_Mutex(&SAL_I_Connection_Id_Mutex);
      }
   }

   return &SAL_I_Connection_Id[app_id];
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * - 18-Aug-2015 Kirk Bailey
 *   - Fixed Coverity issues.
 *
 * 07 Aug 2012 Dan Carman
 *     Improved error handling / connection on Pulses
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
