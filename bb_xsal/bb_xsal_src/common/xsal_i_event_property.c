/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_event_property.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Common functions for XSAL Event Property functionality.
 *
 * -----------------------------------------------------------------------*/

#include <stdlib.h>
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 0); /* Identify file for trace/assert purposes */

/* ---------------------
 *  Object Definitions
 * --------------------- */

/** Pointer to table with event properties.
 */
uint8_t (*SAL_I_Event_Property)[SAL_MAX_EVENTS];

/** Mutex to synchronize access to table with event properties.
 */
SAL_Mutex_T SAL_I_Event_Property_Mutex;

/* -----------------------
 *  Function Definitions
 * ----------------------- */

bool_t SAL_I_Init_Event_Property_Module(void)
{
   bool_t status = false;
   if (SAL_Create_Mutex(&SAL_I_Event_Property_Mutex, NULL))
   {
      SAL_I_Event_Property = calloc(SAL_MAX_EVENTS, SAL_I_Max_Number_Of_Threads);
      if (SAL_I_Event_Property != NULL)
      {
         status = true;
      }
      else
      {
         Tr_Fault("SAL_I_Init_Event_Property_Module/calloc() failed");
      }

      if (!status)
      {
         (void) SAL_Destroy_Mutex(&SAL_I_Event_Property_Mutex);
      }
   }
   else
   {
      Tr_Fault("SAL_I_Init_Event_Property_Module/SAL_Create_Mutex() failed");
   }
   PBC_Ensure(status, "SAL_I_Init_Event_Property_Module failed");

   return status;
}

void SAL_I_Deinit_Event_Property_Module(void)
{
   PBC_Require(SAL_I_Event_Property != NULL, "NULL event property pointer");

   free(SAL_I_Event_Property);
   (void) SAL_Destroy_Mutex(&SAL_I_Event_Property_Mutex);
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
