/* -------------------------------------------------------------------------
 *
 * File:        xsal_local_publish.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Local_Publish() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_publish.h"
#include "xsal_i_thread.h"

#include <stdlib.h>
#include <sys/timeb.h>

EM_FILENUM(XSAL_MSG2_MODULE_ID, 2); /* Identify file for trace/assert purposes */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


void SAL_Local_Publish(
   SAL_Event_Id_T event_id, 
   const void* data, 
   size_t data_size)
{
   SAL_Message_T msg;
   struct timeb current_time;

   (void)ftime(&current_time);

   msg.event_id           = (SAL_Event_Id_T)(uint16_t)event_id;
   msg.object_id          = (int16_t)((uint32_t)event_id >> 16);
   msg.sender_app_id      = SAL_I_App_Id;
   msg.sender_thread_id   = SAL_I_Get_Thread_Id();
   msg.receiver_thread_id = 0;
   msg.timestamp_sec      = current_time.time;
   msg.timestamp_msec     = current_time.millitm;
   msg.data_size          = data_size;
   msg.data               = (void*)data;

   /** Is it correct message?
    */
   PBC_Require_1(msg.event_id > SAL_EVG_FIRST_PUBLISHED_EVENT_ID,
                 "Event id (%d) below first published id", (int)msg.event_id);
   PBC_Require_1(msg.event_id < SAL_EVG_LAST_PUBLISHED_EVENT_ID,
                 "Event id (%d) above last published id", (int)msg.event_id);

#ifdef XSAL_ENABLE_MSG_TRACE
   Tr_Info_Lo_4("SAL_Local_Publish from (%d,%d), event id %d, message size %d",
                (int)msg.sender_app_id, (int)msg.sender_thread_id, (int)msg.event_id, (int)data_size);
#endif /* XSAL_ENABLE_MSG_TRACE */

   SAL_I_Local_Publish(&msg);
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 5
 *   - Task 1334: Fixed compiler warnings
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * -----------------------------------------------------------------------*/
