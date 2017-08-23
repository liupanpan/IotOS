/* -------------------------------------------------------------------------
 *
 * File:        xsal_publish_c.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Publish() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_publish.h"

#if defined (XSAL)
#include "xsal_i_connections.h"
#endif /* XSAL */

#include <stdlib.h>
#include <sys/timeb.h>

EM_FILENUM(XSAL_MSG_MODULE_ID, 8); /* Identify file for trace/assert purposes */

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

void SAL_Publish(SAL_Event_Id_T event_id, const void* data, size_t data_size)
{
   struct timeb current_time;
   SAL_Message_T msg = {0};
#if defined (XSAL)
   size_t app_index;
   SAL_App_Id_T* p_app_id;
#endif /* XSAL */

   (void) ftime(&current_time);

   msg.event_id = (SAL_Event_Id_T) ((uint32_t) event_id & 0xffffu);
   msg.object_id = (int16_t) (((uint32_t) event_id >> 16) & 0xffffu);
   msg.sender_app_id = SAL_I_App_Id;
   msg.sender_thread_id = SAL_I_Get_Thread_Id();
   msg.receiver_thread_id = 0;
   msg.timestamp_sec = current_time.time;
   msg.timestamp_msec = current_time.millitm;
   msg.data_size = data_size;
   msg.data = (void*) data;

   /** Is it correct message?
    */
   PBC_Require_1(msg.event_id > SAL_EVG_FIRST_PUBLISHED_EVENT_ID,
      "Event id (%d) below first published id", msg.event_id);
   PBC_Require_1(msg.event_id < SAL_EVG_LAST_PUBLISHED_EVENT_ID,
      "Event id (%d) above last published id", (int)msg.event_id);

#ifdef XSAL_ENABLE_MSG_TRACE
   Tr_Info_Lo_2("SAL_Publish from (%d,%d))",
      (int)msg.sender_app_id, (int)msg.sender_thread_id);
   Tr_Info_Lo_4("EV %d OBJ %d data 0x%p size %d",
      (int)msg.event_id, (int)msg.object_id, (void*)data, (int)data_size);
#endif /* XSAL_ENABLE_MSG_TRACE */

   SAL_I_Local_Publish(&msg);

#if defined (XSAL)

   /** Gets pointer to list of AppId's sunscribed to event.
    *  First valid AppId is 1.
    */
   p_app_id = &(*SAL_I_GRT)[event_id][1];
   for (app_index = 1; app_index < SAL_LAST_APP_ID; app_index++, p_app_id++)
   {
      SAL_Int_T trial;
      const SAL_Int_T max_number_of_trials = 2;

      /** Do not send message neither to application which is not 
       *  subscribed to it nor to itself.
       */
      if ((*p_app_id == 0) || (app_index == SAL_I_App_Id))
      {
         continue;
      }

      SAL_I_Inc_Ext_Msg_Count(app_index);
      /** When application A sends/publishes a message to application B it
       *  creates and stores a connection to application B.
       *  Every next send/publish operation uses the previously stored 
       *  connection. If Application B terminates and is started again,
       *  then it creates a new channel. To prevent application A from 
       *  sending  messages via the old connection, if the first send 
       *  event fails, then we try to reconnect to the application B and 
       *  send once again.
       */

      for (trial = 0; trial < max_number_of_trials; trial++)
      {
         bool_t connected = false;
         SAL_Connection_T connection;

         if (SAL_Lock_Mutex(&SAL_I_Connection_Id_Mutex))
         {
            connected = SAL_Is_Connected(&SAL_I_Connection_Id[app_index]);
            if (!connected)
            {
               connected = SAL_Connect((SAL_App_Id_T) app_index, SAL_ROUTER_THREAD_ID, &SAL_I_Connection_Id[app_index]);
            }
            if (connected)
            {
               connection = SAL_I_Connection_Id[app_index];
            }
            (void) SAL_Unlock_Mutex(&SAL_I_Connection_Id_Mutex);
         }

         if (connected)
         {
            int32_t status;
            SAL_IOV_T iov[2];

            iov[0].iov_base = (void*) &msg;
            iov[0].iov_len = sizeof(msg);

            iov[1].iov_base = (void*) data;
            iov[1].iov_len = data_size;

#ifdef XSAL_ENABLE_MSG_TRACE
            Tr_Info_Lo_1("SAL_Publish: to app %d", (int)app_index);
#endif /* XSAL_ENABLE_MSG_TRACE */

            if (SAL_Sync_Send_VS(&connection, iov, sizeof(iov) / sizeof(SAL_IOV_T), NULL, 0, &status))
            {
               break;
            }

            /** SAL_Sync_Send_V has failed. Try to connect once again.
             */
            Tr_Info_Mid_1("SAL_Publish fail, reopen connection the application %d", (int)app_index);
            if (SAL_Lock_Mutex(&SAL_I_Connection_Id_Mutex))
            {
               SAL_Invalidate_Connection(&SAL_I_Connection_Id[app_index]);
               (void) SAL_Unlock_Mutex(&SAL_I_Connection_Id_Mutex);
            }

            if (trial == (max_number_of_trials - 1))
            {
               Tr_Fault_3("Couldn't Publish to App: %d Event: %d Size: %d",
                  (int)app_index, (int)msg.event_id, (int) msg.data_size);
            }
         }
         else
         {
            if (SAL_Lock_Mutex(&SAL_I_Connection_Id_Mutex))
            {
               SAL_Invalidate_Connection(&SAL_I_Connection_Id[app_index]);
               (void) SAL_Unlock_Mutex(&SAL_I_Connection_Id_Mutex);
            }

            if (trial == (max_number_of_trials - 1))
            {
               Tr_Warn_3("Couldn't Connect to App: %d to Publish Event: %d Size: %d",
                  (int)app_index, (int)msg.event_id, (int) msg.data_size);
            }
         }
      }
   }
#endif /* XSAL */
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 11
 *   - Task 1334: Fixed compiler warnings
 *
 * - 31-Oct-2013 Larry Piekarski Rev 12
 *   - Task 2589: Fixed uninitialized local variables (Valgrind will issue a
 *     warning for use of uninitialized pad bytes)
 *
 * 20May14  David Origer
 * - Task kok_inf#7893 : Fix Klocwork issues.
 *   - Remove changes for "no reply" messaging made under task
 *     kok_basa#115827.
 *
 * 30-Jan-2015 Kirk Bailey
 * - Fixed Valgrind issue with trace message.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * -----------------------------------------------------------------------*/
