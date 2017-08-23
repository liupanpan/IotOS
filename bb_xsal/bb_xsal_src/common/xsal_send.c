/* -------------------------------------------------------------------------
 *
 * File:        xsal_send.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Send() function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_publish.h"
#include "xsal_i_event_property.h"
#if defined (XSAL)
#include "xsal_i_connections.h"
#endif /* XSAL */ 

EM_FILENUM(XSAL_MSG_MODULE_ID, 8); /* Identify file for trace/assert purposes */

#include <sys/timeb.h>

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

bool_t SAL_Send(SAL_App_Id_T app_id, SAL_Thread_Id_T thread_id, SAL_Event_Id_T event_id, const void* data,
   size_t data_size)
{
   bool_t send_status = false;
   SAL_Message_T msg = {0};
   struct timeb current_time;

   (void) ftime(&current_time);

   msg.event_id = (SAL_Event_Id_T) ((uint32_t) event_id & 0xffffu);
   msg.object_id = (int16_t) (((uint32_t) event_id >> 16) & 0xffffu);
   msg.sender_app_id = SAL_I_App_Id;
   msg.sender_thread_id = SAL_I_Get_Thread_Id();
   msg.receiver_thread_id = thread_id;
   msg.timestamp_sec = current_time.time;
   msg.timestamp_msec = current_time.millitm;
   msg.data_size = data_size;
   msg.data = (void*) data;

   /** Is it correct message?
    */
   PBC_Require_1(msg.event_id > SAL_FIRST_EVENT_ID, "Invalid event id %d", msg.event_id);
#ifdef XSAL_ENABLE_MSG_TRACE
   Tr_Info_Lo_4("SAL_Send from (%d,%d) to (%d,%d)",
      (int)msg.sender_app_id, (int)msg.sender_thread_id, (int)app_id, (int)thread_id);
   Tr_Info_Lo_4("EV %d OBJ %d data 0x%p size %d",
      (int)msg.event_id, (int)msg.object_id, (void*)data, (int)data_size);
#endif /* XSAL_ENABLE_MSG_TRACE */

   /** Check if it is a local message
    */
#if defined (XSAL)
   if ((app_id == 0) || (app_id == SAL_I_App_Id))
#endif
   {
      if (thread_id == 0)
      {
         /** thread_id == 0 means to send message to all subscribed
          *  threads in calling application.
          */
         SAL_I_Local_Publish(&msg);
         send_status = true;
      }
      else
      {
         SAL_Message_Dropped_Reason_T err;

         /** This is a "local message" only for specified thread.
          */
#if defined (XSAL)

         bool_t is_urgent = (bool_t) ((msg.event_id < SAL_LAST_EVENT_ID) ? SAL_I_Is_Urgent(thread_id, msg.event_id)
            : false);

#else /* defined (XSAL_LIGHT) */
         bool_t is_urgent = false;
#endif /* XSAL && XSAL_LIGHT */
         SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[thread_id];

         SAL_I_Inc_Int_Msg_Count();

         send_status = SAL_I_Post_Message(&thread_attr->message_queue, &msg, is_urgent, &err);

         if (!send_status)
         {
            Tr_Fault_4("Dropped Local Send TID: %d Event: %d Data: %X Size: %d",
               (int)thread_id, (int)event_id, (data_size ? *((uint8_t *) data) : 0), (int)data_size);

            if (SAL_I_Message_Dropped_Callback != NULL)
            {
               SAL_I_Message_Dropped_Callback(thread_id, err, &msg);
            }
         }
      }
   }
#if defined (XSAL)
   else
   {
      SAL_Int_T trial;
      const SAL_Int_T max_number_of_trials = 2;

      PBC_Require_1(app_id < SAL_LAST_APP_ID, "Illegal XSAL app id: %d", (int)app_id);

      SAL_I_Inc_Ext_Msg_Count(app_id);

      /** It is a message to remote application.
       *  Send it to the Router Thread of given application.
       */

      /** When application A sends/publishes a message to application B it
       *  creates and stores a connection to application B.
       *  Every next send/publish operation uses the previously stored
       *  connection. If Application B terminates and is started again,
       *  then it creates a new channel. To prevent application A from
       *  sending  messages via the old connection, if the first send
       *  event fails, then we try to reconnect to the application B and
       *  send once again.
       */
      send_status = 0;
      for (trial = 0; trial < max_number_of_trials; trial++)
      {
         bool_t connected = false;
         SAL_Connection_T connection;

         if (SAL_Lock_Mutex(&SAL_I_Connection_Id_Mutex))
         {
            connected = SAL_Is_Connected(&SAL_I_Connection_Id[app_id]);
            if (!connected)
            {
               connected = SAL_Connect(app_id, SAL_ROUTER_THREAD_ID, &SAL_I_Connection_Id[app_id]);
            }
            if (connected)
            {
               connection = SAL_I_Connection_Id[app_id];
            }
            (void) SAL_Unlock_Mutex(&SAL_I_Connection_Id_Mutex);
         }

         if (connected)
         {
            int32_t return_status;
            SAL_IOV_T iov[2];

            iov[0].iov_base = (void*) &msg;
            iov[0].iov_len = sizeof(msg);

            iov[1].iov_base = (void*) data;
            iov[1].iov_len = data_size;

            if (SAL_Sync_Send_VS(&connection, iov, sizeof(iov) / sizeof(SAL_IOV_T), NULL, 0, &return_status))
            {
               /** Remote Router Thread received a message.
                *  'return_status' means whether the message was delivered
                *  to the destination queue or not
                */
               send_status = (bool_t) (return_status != 0);
               break;
            }
            /** SAL_Sync_Send_V has failed. Try to connect once again.
             */
            Tr_Info_Hi_1("SAL_Send fail, reopen connection the application %d", (int)app_id);
            if (SAL_Lock_Mutex(&SAL_I_Connection_Id_Mutex))
            {
               SAL_Invalidate_Connection(&SAL_I_Connection_Id[app_id]);
               (void) SAL_Unlock_Mutex(&SAL_I_Connection_Id_Mutex);
            }
            if (trial == max_number_of_trials - 1)
            {
               Tr_Fault_1("SAL_Send: Couldn't send to app %d", (int)app_id);
            }
         }
         else
         {
            if (SAL_Lock_Mutex(&SAL_I_Connection_Id_Mutex))
            {
               SAL_Invalidate_Connection(&SAL_I_Connection_Id[app_id]);
               (void) SAL_Unlock_Mutex(&SAL_I_Connection_Id_Mutex);
            }
            if (trial == max_number_of_trials - 1)
            {
               Tr_Fault_1("SAL_Send: Couldn't connect to app %d", (int)app_id);
            }
         }
      }

      if (!send_status)
      {
         Tr_Fault_4("Dropped IPC Send App: %d TID: %d Event: %d Size: %d",
            (int)app_id, (int)thread_id, (int)msg.event_id, (int)msg.data_size);

         if (SAL_I_Message_Dropped_Callback != NULL)
         {
            SAL_I_Message_Dropped_Callback(thread_id, SAL_Communication_Error, &msg);
         }
      }
   }
#endif /* XSAL */

   return send_status;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 15
 *   - Task 1334: Fixed compiler warnings
 *
 * 20May14  David Origer
 * - Task kok_inf#7893 : Fix Klocwork issues.
 *   - Remove changes for "no reply" messaging made under task
 *     kok_basa#115827.
 *
 * 9Sep14  David Origer
 * - Task kok_inf#11831 : Fix Valgrind errors in bb_xsal.
 *   - Make sure SAL_Send() msg data is fully initialized, including pad
 *     bytes.
 *   - Do not report more data in a fault message than may be available.
 *
 * 30-Jan-2015 Kirk Bailey
 * - Fixed Valgrind issue with trace message.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * -----------------------------------------------------------------------*/
