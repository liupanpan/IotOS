/* -------------------------------------------------------------------------
 *
 * File:        xsal_select.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Select() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"
#include "xsal_i_thread.h"
#include "xsal_i_select.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 12); /* Identify file for trace/assert purposes */

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


bool_t SAL_Select(
   const SAL_Event_Id_T event_id_list[],
   uint32_t number_of_events)
{
   bool_t status;
   SAL_Thread_Id_T tid = SAL_I_Get_Thread_Id();

   PBC_Require(tid > 0, "Invalid thread id");
   PBC_Require((size_t)tid <= SAL_I_Max_Number_Of_Threads, "Invalid thread id");
   PBC_Require((event_id_list != NULL) || (number_of_events == 0), "Invalid event list data");

   if (SAL_Lock_Mutex(&SAL_I_Event_Property_Mutex))
   {
      uint32_t event_index;
      uint8_t* event_property = SAL_I_Thread_Event_Property(tid);
      SAL_I_Thread_Attr_T* thread_attr;

      /** First unselect all events
       */
      SAL_I_Unselect(event_property);

      /** Next select events from given list
       */
      for(event_index = 0; event_index < number_of_events; event_index++)
      {
         SAL_Event_Id_T ev_id = event_id_list[event_index];

         /** Is it correct message?
          */
         PBC_Require_1(ev_id > SAL_FIRST_EVENT_ID, "Invalid Event id (%d)", (int)ev_id);
         PBC_Require_1(ev_id <  SAL_LAST_EVENT_ID,  "Invalid Event id (%d)", (int)ev_id);

         event_property[ev_id] |= SAL_I_Select_Bit_Mask;
      }
      (void)SAL_Unlock_Mutex(&SAL_I_Event_Property_Mutex);

      thread_attr = SAL_I_Get_Thread_Attr();
      PBC_Require(thread_attr != NULL, "NULL thread attribute");
      thread_attr->selected_count = number_of_events;
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
 * - 26-Sep-2013 Larry Piekarski Rev 6
 *   - Task 1334: Fixed compiler warnings
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the
 *    enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
