/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_receive_only.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the internal interface of 
 *              the SAL_Receive_Only
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_receive.h"


/* -----------------------
 *  Function Definitions
 * ----------------------- */


SAL_Queue_Iterator_T SAL_I_Search_Message_Queue(
   const SAL_Message_Queue_T* queue, 
   const SAL_Event_Id_T event_list[],
   size_t number_of_events)
{
   bool_t found = false;
   SAL_Message_Queue_Node_T* q_end = queue->end_marker;
   SAL_Queue_Iterator_T iterator = q_end->next_node;

   while((iterator != q_end) && !found)
   {
      size_t i;
      SAL_Event_Id_T ev_id = iterator->message.event_id;

      for(i=0; (i < number_of_events) && !found; i++)
      {
         found = (bool_t)(event_list[i] == ev_id);
      }
      if (!found)
      {
         iterator = iterator->next_node;
      }
   }
   return iterator;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 25 April 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * -----------------------------------------------------------------------*/
