/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_select.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the XSAL selective receive
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_event_property.h"
#include "xsal_i_thread.h"
#include "xsal_i_select.h"


/* -----------------------
 *  Function Definitions
 * ----------------------- */


SAL_Queue_Iterator_T SAL_I_Search_Selected_Message(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property)
{
   bool_t found = false;
   SAL_Queue_Iterator_T it = queue->end_marker->next_node;

   while((it != it->queue->end_marker) && !found)
   {
      SAL_Event_Id_T ev_id = it->message.event_id;
      if (
         (ev_id > SAL_FIRST_EVENT_ID) && (ev_id < SAL_LAST_EVENT_ID) &&
         (event_property[ev_id] & SAL_I_Select_Bit_Mask) != 0)
      {
         found = true;
      }
      else
      {
         it = it->next_node;
      }
   }
   return it;
}


void SAL_I_Unselect(uint8_t* event_property)
{
   size_t ev_id;

   for(ev_id = 0; ev_id < SAL_LAST_EVENT_ID; ev_id++)
   {
      event_property[ev_id] &= ~SAL_I_Select_Bit_Mask;
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 15 Feb 2008, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 4
 *   - Task 1334: Fixed compiler warnings
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * -----------------------------------------------------------------------*/
