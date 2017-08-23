/* -------------------------------------------------------------------------
 *
 * File:        xsal_unsubscribe.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Unsubscribe() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"
#include "xsal_i_thread.h"
#include "xsal_i_publish.h"

EM_FILENUM(XSAL_MSG_MODULE_ID, 10); /* Identify file for trace/assert purposes */

/* ----------------------------
 *  Local function prototypes
 * ----------------------------*/

/** Function SAL_Lock_Mutex is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

bool_t SAL_Unsubscribe(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events)
{
   bool_t status = true;
   SAL_Thread_Id_T tid = SAL_I_Get_Thread_Id();

   PBC_Require(tid > 0, "Invalid thread id");
   PBC_Require(tid <= (SAL_Thread_Id_T )SAL_I_Max_Number_Of_Threads, "Thread id > max");
   PBC_Require((event_id_list != NULL) || (number_of_events == 0), "Invalid event list data");

   if (SAL_Lock_Mutex(&SAL_I_Event_Property_Mutex))
   {
      uint32_t event_index;
      uint8_t* event_properties = SAL_I_Thread_Event_Property(tid);

#if defined (XSAL)

      size_t thread_index;
      size_t unsubscribe_event_number = 0;
      SAL_Event_Id_T unsubscribe_event_id_list[SAL_MAX_EVENTS] =
         { 0 };

#endif /* XSAL */

      /** Local unsubscribe
       */
      for (event_index = 0; event_index < number_of_events; event_index++)
      {
#if defined (XSAL)
         bool_t search;
#endif /* XSAL */

         SAL_Event_Id_T ev_id = event_id_list[event_index];

         /** Is the event correct?
          */
         if ((ev_id > SAL_EVG_FIRST_PUBLISHED_EVENT_ID) && (ev_id < SAL_EVG_LAST_PUBLISHED_EVENT_ID))
         {
            event_properties[ev_id] &= ~SAL_I_Subscribe_Bit_Mask;

#if defined (XSAL)

            /** Check if the event is subscribed by another threads.
             *  If not, then unsubscribe event from Naming Service
             *
             *  Variable "thread_index" is not theread ID.
             *  It starts from 0 (not from 1);
             */
            for (thread_index = 0, search = true; search && (thread_index < SAL_I_Max_Number_Of_Threads); thread_index++)
            {
               if (SAL_I_Event_Property[thread_index][ev_id] & SAL_I_Subscribe_Bit_Mask)
               {
                  search = false;
               }
            }

            if (search)
            {
               /** It was the last thread in calling application subscribed for
                *  given event. Unsubscribe event from Naming Service
                */
               unsubscribe_event_id_list[unsubscribe_event_number++] = ev_id;
            }
#endif /* XSAL */
         }
         else
         {
            Tr_Fault_1("Trying to unsubscribe to a non-Published Event Id: 0x%x", ev_id);
         }
      }

#if defined (XSAL)
      if (unsubscribe_event_number > 0u)
      {
         /** The following function must be called in scope of 
          *  the SAL_I_LRT_Mutex lock.
          */
         status = SAL_NS_Unsubscribe(SAL_I_App_Id, unsubscribe_event_id_list, unsubscribe_event_number);
      }
#endif /* XSAL */

      (void) SAL_Unlock_Mutex(&SAL_I_Event_Property_Mutex);
   }
   return status;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 8
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
