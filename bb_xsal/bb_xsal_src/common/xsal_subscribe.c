/* -------------------------------------------------------------------------
 *
 * File:        xsal_subscribe.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Subscribe() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"
#include "xsal_i_thread.h"
#include "xsal_i_publish.h"

EM_FILENUM(XSAL_MSG2_MODULE_ID, 3); /* Identify file for trace/assert purposes */

/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/

/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);

/* ------------------------------
 *  Public function definitions
 * ------------------------------ */

bool_t SAL_Subscribe(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events)
{
   bool_t status = true;
   SAL_Thread_Id_T tid = SAL_I_Get_Thread_Id();

   PBC_Require(tid > 0, "Invalid thread id");
   PBC_Require(tid <= (SAL_Thread_Id_T )SAL_I_Max_Number_Of_Threads, "thread id > max");

   if ((event_id_list != NULL) && (number_of_events > 0))
   {
      if (SAL_Lock_Mutex(&SAL_I_Event_Property_Mutex))
      {
         uint32_t event_index;
         uint8_t* event_properties = SAL_I_Thread_Event_Property(tid);

         /** "Local" subscribe
          */
         for (event_index = 0; event_index < number_of_events; event_index++)
         {
            SAL_Event_Id_T ev_id = event_id_list[event_index];

            /** Is the event correct?
             */
            if ((ev_id > SAL_EVG_FIRST_PUBLISHED_EVENT_ID) && (ev_id < SAL_EVG_LAST_PUBLISHED_EVENT_ID))
            {
               event_properties[ev_id] |= SAL_I_Subscribe_Bit_Mask;
            }
            else
            {
               Tr_Fault_1("Trying to subscribe to a non-Published Event Id: 0x%x", ev_id);
            }
         }

#if defined (XSAL)

         /** "Global" subscribe.
          */
         if (status)
         {
            /** The following function must be called in scope of
             *  the SAL_I_Event_Property_Mutex lock.
             */
            status = SAL_NS_Subscribe(SAL_I_App_Id, event_id_list, number_of_events);
         }

#endif /* XSAL */

         (void) SAL_Unlock_Mutex(&SAL_I_Event_Property_Mutex);
      }
      else
      {
         PBC_Failed("Unable to lock event property mutex");
      }
   }
   return status;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 7
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
 * 10-Feb-2016 Dan Carman
 *     - Changed request with null list or no elements from PBC assert to ignore request.
 *
 * -----------------------------------------------------------------------*/
