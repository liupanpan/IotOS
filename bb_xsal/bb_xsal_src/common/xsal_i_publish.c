/* -------------------------------------------------------------------------
 *
 * File:        xsal_publish.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL Publish functionality
 *
 * -----------------------------------------------------------------------*/

#include "xsal_i_assert.h"
#include "xsal_i_publish.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "xsal_i_engine.h"
#include "xsal_i_event_property.h"

#if defined (XSAL)
#   include "xsal_i_ns_proxy.h"
#else
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
#endif /* XSAL */

#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

EM_FILENUM(XSAL_MSG_MODULE_ID, 8); /* Identify file for trace/assert purposes */

/* ---------------------------
 *  Local Object Definitions
 * ---------------------------*/

#if defined (XSAL)

/**
 *  Pointer to the Global Routing Table (GRT).
 *  The GRT is created by Name Server in shared memory.
 *  Table contains list of ids of application subscribed for events.
 *  Access to this table doesn't have to be synchronized
 */
uint8_t (*SAL_I_GRT)[SAL_MAX_EVENTS][SAL_MAX_APPS + 1];

#endif /* XSAL */

/** Mutex used to synchronize access to SAL_Msg_Count_Stats.
 */
static SAL_Mutex_T SAL_Msg_Count_Stats_Mutex;

/** Message count statistics for this XSAL app.
 */
static SAL_App_Msg_Count_Stats_T SAL_Msg_Count_Stats;

bool_t SAL_I_Init_Publish_Module(void)
{
   bool_t status;
#if defined(XSAL)
   SAL_NS_Data_T* ns_data;
#endif
   status = SAL_Create_Mutex(&SAL_Msg_Count_Stats_Mutex, NULL);

   if (!status)
   {
      PBC_Failed("Failed to create Mutex for message counts.");
   }
#if defined(XSAL)
   status &= SAL_NS_Init_Proxy(&ns_data);
   if (status)
   {
      if (SAL_NS_Config_Ok())
      {
         SAL_I_GRT = &(ns_data->routing_table);
      }
      else
      {
         status = false;
         PBC_Failed("Application has different number of the Channels, App IDs or Event IDs then the Name Server.\n"
            "Re-compile your application or Name Server.");
      }
   }
   else
   {
      Tr_Fault("SAL_I_Init_Publish_Module: SAL_NS_Init_Proxy failed");
   }
#endif /* XSAL */

   return status;
}

void SAL_I_Deinit_Publish_Module(void)
{
   SAL_Destroy_Mutex(&SAL_Msg_Count_Stats_Mutex);
#if defined (XSAL)
   SAL_NS_Deinit_Proxy();
   SAL_I_GRT = NULL;
#endif /* XSAL */
}

/** Function is called when given thread has finished its job 
 *  or has been terminated.
 *  It unsubscribes from LRT events subscribed by given thread.
 *  If any event was subscribed only by the given thread (it isn't
 *  subscribed by any other thread), then it is also unsubscribed 
 *  from Naming Service.
 */
void SAL_I_Unsubscribe_Thread_Events(SAL_Thread_Id_T thread_id)
{
   SAL_Event_Id_T ev_id;
#if defined (XSAL)
   size_t unsubscribe_event_number = 0;
   SAL_Event_Id_T unsubscribe_event_id_list[SAL_MAX_EVENTS] =
   { 0 };
#endif /* XSAL */
   uint8_t* event_properties = SAL_I_Thread_Event_Property(thread_id);

   for (ev_id = SAL_EVG_FIRST_PUBLISHED_EVENT_ID; ev_id < SAL_EVG_LAST_PUBLISHED_EVENT_ID; ev_id++)
   {
      if (event_properties[ev_id] & SAL_I_Subscribe_Bit_Mask)
      {
#if defined (XSAL)
         bool_t search;
         size_t thread_index;
#endif /* XSAL */

         event_properties[ev_id] &= ~SAL_I_Subscribe_Bit_Mask;

#if defined (XSAL)

         /** Check if there is any thread subscribed to given event
          *
          *  Variable "thread_index" is not theread ID.
          *  It starts from 0 (not from 1) ;
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
            /** It was the last one thread in calling application 
             *  subscribed for given event. Unsubscribe event from 
             *  Naming Service
             */
            unsubscribe_event_id_list[unsubscribe_event_number++] = ev_id;
         }
#endif /* XSAL */
      }
   }

#if defined (XSAL)

   if (unsubscribe_event_number > 0u)
   {
      (void) SAL_NS_Unsubscribe(SAL_I_App_Id, unsubscribe_event_id_list, unsubscribe_event_number);
   }
#endif /* XSAL */
}

/**
 *  Function sends given message to all subscribed threads.
 */
void SAL_I_Local_Publish(const SAL_Message_T* msg)
{
   SAL_Event_Id_T ev_id = msg->event_id;

   size_t tid;
   for (tid = 1; tid <= SAL_I_Max_Number_Of_Threads; tid++)
   {
      if (SAL_I_Is_Subscribed(tid, ev_id))
      {
         bool_t send_status;
         SAL_Message_Dropped_Reason_T err;
         SAL_I_Thread_Attr_T* thread_attr = &SAL_I_Thread_Table[tid];

         Tr_Info_Lo_1("SAL_I_Local_Publish: to thread %d", (int)tid);

         SAL_I_Inc_Int_Msg_Count();
#if defined (XSAL)
         send_status = SAL_I_Post_Message(&thread_attr->message_queue, msg, SAL_I_Is_Urgent(tid, ev_id), &err);
#else
         send_status = SAL_I_Post_Message(&thread_attr->message_queue,
            msg, false, &err);
#endif /* XSAL */

         if (!send_status)
         {
            Tr_Fault_3("Dropped Local Publish TID: %d Event: %d Size: %d",
               (int)tid, (int)msg->event_id, (int)msg->data_size);

            if (SAL_I_Message_Dropped_Callback != NULL)
            {
               SAL_I_Message_Dropped_Callback((SAL_Thread_Id_T) tid, err, msg);
            }
         }
      }
   }
}

void SAL_I_Get_App_Msg_Count_Stats(SAL_App_Msg_Count_Stats_T *stats, bool_t reset_counts)
{
   if (NULL == stats)
   {
      Tr_Fault("NULL ptr for message count stats");
   }
   else
   {
      (void) SAL_Lock_Mutex(&SAL_Msg_Count_Stats_Mutex);
      *stats = SAL_Msg_Count_Stats;
      if (reset_counts)
      {
         memset(&SAL_Msg_Count_Stats, 0, sizeof(SAL_Msg_Count_Stats));
      }
      SAL_Unlock_Mutex(&SAL_Msg_Count_Stats_Mutex);
   }
}

void SAL_I_Inc_Ext_Msg_Count(SAL_App_Id_T app_id)
{
#if defined (XSAL)
   if (app_id > SAL_MAX_APPS)
   {
      Tr_Fault_1("Illegal XSAL app id: %d", (int)app_id);
   }
   else
   {
      (void) SAL_Lock_Mutex(&SAL_Msg_Count_Stats_Mutex);
      SAL_Msg_Count_Stats.total_num_sent_externally++;
      SAL_Msg_Count_Stats.num_sent_to_app[app_id]++;
      SAL_Unlock_Mutex(&SAL_Msg_Count_Stats_Mutex);
   }
#endif
}

void SAL_I_Inc_Int_Msg_Count(void)
{
   (void) SAL_Lock_Mutex(&SAL_Msg_Count_Stats_Mutex);
   SAL_Msg_Count_Stats.total_num_sent_internally++;
   SAL_Unlock_Mutex(&SAL_Msg_Count_Stats_Mutex);
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 30-Jan-2015 Kirk Bailey
 * + Fixed Valgrind issue with trace message.
 * 
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the
 *    enumerated type of SAL_Event_Id_T for compatibility with ISA code.
 *
 * - 18-Aug-2015 Kirk Bailey
 *   - Fixed Coverity issues.
 * -----------------------------------------------------------------------*/
