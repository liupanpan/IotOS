/* -------------------------------------------------------------------------
 *
 * File:        xsal_unselect.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Unselect() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"
#include "xsal_i_thread.h"
#include "xsal_i_select.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 13); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Unselect(void)
{
   bool_t status;
   uint8_t* event_property;
   SAL_Thread_Id_T tid = SAL_I_Get_Thread_Id();

   PBC_Require(tid > 0, "Invalid thread id");
   PBC_Require((size_t)tid <= SAL_I_Max_Number_Of_Threads, "Invalid thread id");

   event_property = SAL_I_Thread_Event_Property(tid);

   if (SAL_Lock_Mutex(&SAL_I_Event_Property_Mutex))
   {
      SAL_I_Thread_Attr_T* thread_attr;
      SAL_I_Unselect(event_property);

      (void)SAL_Unlock_Mutex(&SAL_I_Event_Property_Mutex);

      thread_attr = SAL_I_Get_Thread_Attr();
      PBC_Require(thread_attr != NULL, "NULL thread attribute");
      thread_attr->selected_count = 0;
      status = true;
   }
   else
   {
      status = false;
      PBC_Failed("Failed to lock event property mutex");
   }

   return status;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 15 Feb 2008, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
