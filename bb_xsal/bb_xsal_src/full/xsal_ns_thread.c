/* -------------------------------------------------------------------------
 *
 * File:        xsal_ns_thread.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the Name Server thread
 *
 * -------------------------------------------------------------------------*/

#include "pbc_trace.h"
#include "string_resource.h"
#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_ns_thread.h"
#include "xsal_i_ns_proxy.h"
#include "xsal_i_thread.h"

#include <stdlib.h>
#include <string.h>

EM_FILENUM(XSAL_NS_MODULE_ID, 1); /* Identify file for trace/assert purposes */

/* --------------------------
 *  Local Type Declarations
 * --------------------------*/

/** Union with all Name Server commands.
 *  Size of union determines maximum size of the constant 
 *  part of communication buffer.
 */
typedef union SAL_NS_Command_Buffer_Tag
{
   SAL_NS_Command_T command;
   SAL_NS_Bind_Channel_T bind;
   SAL_NS_Unbind_Channel_T unbind;
   SAL_NS_Resolve_Channel_T resolve;
   SAL_NS_Unbind_App_T unbind_app;
   SAL_NS_Get_Running_Apps_T get_running_apps;

   SAL_NS_Subscribtion_T subscribe;
   SAL_NS_Unsubscribe_App_T unsubscribe_all;
} SAL_NS_Command_Buffer_T;

/** Size of union determines maximum size of the variable 
 *  part of the command data.
 */
typedef union SAL_NS_Variable_Length_Part_Tag
{
   SAL_App_Id_T app_id_list[SAL_MAX_APPS];
   SAL_Event_Id_T event_id_list[SAL_MAX_EVENTS];
} SAL_NS_Variable_Length_Part_T;

typedef union SAL_NS_Reply_Buffer_Tag
{
   SAL_Channel_T channel;
   uint16_t last_app_id;
   SAL_App_Id_T app_id_list[SAL_MAX_APPS + 1];
   SAL_App_Data_T app_data;
   uint16_t last_module_id;
   SAL_Dbg_Module_Data_T module_data;
} SAL_NS_Reply_Buffer_T;

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

static bool_t Bind_Channel(const SAL_NS_Bind_Channel_T* channel);
static bool_t Unbind_Channel(const SAL_NS_Unbind_Channel_T* channel);
static bool_t Resolve_Channel(const SAL_NS_Resolve_Channel_T* channel_id, SAL_Channel_T* reply_channel);
static void Unbind_App(const SAL_NS_Unbind_App_T* unbind_app);
static size_t Get_Running_App_Ids(SAL_App_Id_T reply_router_thread_ids[]);
static void Update_Events(const SAL_NS_Subscribtion_T* subscribe_data, uint8_t subscribe);
static void Unsubscribe_App(const SAL_NS_Unsubscribe_App_T* unsubscribe_app);
static void SAL_Name_Server_Thread(void *param);

/* ---------------------------
 *  Local Object Definitions
 * --------------------------- */

/** Pointer to the Channel Data and Routing table data 
 *  shared between applications.
 */
static SAL_NS_Data_T* SAL_I_NS_Thread_Data;
static SAL_Shared_Memory_Handle_T SH_handle;

#undef  X
#define X(name,descr) #name,

/** Names of the XSAL applications */
static const char * App_Names[] =
{ "SAL_NO_APP_ID", SAL_APPLICATION_IDS};

#undef Tr_Module_ID
#define Tr_Module_ID(name,descr) #name,

/** Names of dbg_trace modules. */
static const char * Module_Names[] =
{ "TR_MODULE_UNKNOWN", Tr_Module_IDs};

/* ----------------------------
 *  Local Function Definitions
 * ---------------------------- */

static bool_t Bind_Channel(const SAL_NS_Bind_Channel_T* channel)
{
   size_t i;
   bool_t status = true;
   size_t ch_no = SAL_I_NS_Thread_Data->channels.number_of_channels;
   SAL_NS_Channel_T* ch = SAL_I_NS_Thread_Data->channels.channel;
   SAL_App_Id_T ap_id = channel->app_id;
   SAL_Port_Id_T p_id = channel->port_id;

   for (i = 0; i < ch_no; i++)
   {
      if ((ap_id == ch[i].app_id) && (p_id == ch[i].port_id))
      {
         break;
      }
   }

   if (i < ch_no)
   {
      if ((ap_id == APP_PROCMAN) && (p_id == SAL_ROUTER_THREAD_ID))
      {
         /** APP_PROCMAN and SAL_ROUTER_THREAD_ID can be bound only once.
          *  It is bound as the first channel in the whole system by the
          *  Process Manager application.
          */
         Tr_Fault_1(
            "Bind_Channel: APP_ID %d is reserved for ProccessManager application",
            (int)APP_PROCMAN);
         status = false;
      }
      else
      {
         Tr_Info_Hi_2("Bind_Channel: re-bind app_id=%d, port_id=%d", (int)ap_id, (int)p_id);

         ch[i].channel = channel->channel;
      }
   }
   else if (i < SAL_MAX_CHANNELS)
   {
      Tr_Info_Hi_2("Bind_Channel: app_id=%d, port_id=%d", (int)ap_id, (int)p_id);

      ch[i].app_id = channel->app_id;
      ch[i].port_id = channel->port_id;
      ch[i].channel = channel->channel;
      SAL_I_NS_Thread_Data->channels.number_of_channels++;
   }
   else
   {
      Tr_Fault_3("Bind_Channel: app_id=%d, port_id=%d, too many entries (>%d)",
         (int)ap_id, (int)p_id, (int)SAL_MAX_CHANNELS);

      status = false;
   }

   return status;
}

static bool_t Unbind_Channel(const SAL_NS_Unbind_Channel_T* channel)
{
   bool_t found;
   size_t ch_idx;
   size_t ch_no = SAL_I_NS_Thread_Data->channels.number_of_channels;
   SAL_NS_Channel_T* ch = SAL_I_NS_Thread_Data->channels.channel;

   for (ch_idx = 0, found = false; !found && (ch_idx < ch_no); ch_idx++)
   {
      if (memcmp(&channel->channel, &ch[ch_idx].channel, sizeof(SAL_Channel_T)) == 0)
      {
         Tr_Info_Hi_2("Unbind_Channel app_id=%d, port_id=%d",
            (int)ch[ch_idx].app_id, (int)ch[ch_idx].port_id);

         /** Replace removed channel by the channel from the end of the table
          */
         ch[ch_idx] = ch[ch_no - 1u];
         SAL_I_NS_Thread_Data->channels.number_of_channels--;
         found = true;
      }
   }

   if (!found)
   {
      Tr_Fault("Unbind_Channel: channel doesn't exist");
   }
   return found;
}

static bool_t Resolve_Channel(const SAL_NS_Resolve_Channel_T* channel_id, SAL_Channel_T* reply_channel)
{
   bool_t found;
   size_t ch_idx;
   size_t ch_no = SAL_I_NS_Thread_Data->channels.number_of_channels;
   SAL_NS_Channel_T* ch = SAL_I_NS_Thread_Data->channels.channel;
   SAL_App_Id_T ap_id = channel_id->app_id;
   SAL_Port_Id_T p_id = channel_id->port_id;

   for (ch_idx = 0, found = false; !found && (ch_idx < ch_no); ch_idx++)
   {
      if ((ap_id == ch[ch_idx].app_id) && (p_id == ch[ch_idx].port_id))
      {
         *reply_channel = ch[ch_idx].channel;

         Tr_Info_Hi_2("Resolve_Channel: app_id=%d, port_id=%d", (int)ap_id, (int)p_id);
         found = true;
      }
   }
   if (!found)
   {
      /* Generate fault message at higher level */
      Tr_Info_Hi_2("Resolve_Channel: app_id=%d, port_id=%d not bound", (int)ap_id, (int)p_id);
   }
   return found;
}

static void Unbind_App(const SAL_NS_Unbind_App_T* unbind_app)
{
   size_t i;
   SAL_App_Id_T a_id = unbind_app->app_id;
   size_t ch_no = SAL_I_NS_Thread_Data->channels.number_of_channels;
   SAL_NS_Channel_T* ch = SAL_I_NS_Thread_Data->channels.channel;

   for (i = 0; i < ch_no;)
   {
      if (a_id == ch[i].app_id)
      {
         Tr_Info_Hi_2("Unbind_App app_id=%d, port_id=%d", (int)a_id, (int)ch[i].port_id);

         /* Replace removed channel by the channel from the end of table */
         ch[i] = ch[--ch_no];
         SAL_I_NS_Thread_Data->channels.number_of_channels--;
      }
      else
      {
         i++;
      }
   }
}

static void Get_App_Data(SAL_NS_Get_App_Data_T const *cmd, SAL_App_Data_T *reply)
{
   if (cmd->app_id < SAL_LAST_APP_ID)
   {
      size_t i;
      size_t ch_no = SAL_I_NS_Thread_Data->channels.number_of_channels;
      SAL_NS_Channel_T* ch = SAL_I_NS_Thread_Data->channels.channel;

      reply->app_id = cmd->app_id;
      Str_Full_Copy(reply->app_name, App_Names[cmd->app_id], sizeof(reply->app_name));
      reply->is_running = false;

      for (i = 0; i < ch_no; i++)
      {
         if ((SAL_ROUTER_THREAD_ID == ch[i].port_id) && (ch[i].app_id == cmd->app_id))
         {
            reply->is_running = true;
            break; /* out of for loop */
         }
      }
   }
   else
   {
      reply->app_id = SAL_NO_APP_ID;
      reply->is_running = false;
      Str_Full_Copy(reply->app_name, "ILLEGAL_APP", sizeof(reply->app_name));
   }
}

static void Get_Module_Data(SAL_NS_Get_Dbg_Module_Data_T const *cmd, SAL_Dbg_Module_Data_T *reply)
{
   if (cmd->module_id < TR_NUM_MODULE_IDS)
   {
      reply->module_id = cmd->module_id;
      Str_Full_Copy(reply->module_name, Module_Names[cmd->module_id], sizeof(reply->module_name));
   }
   else
   {
      reply->module_id = TR_MODULE_UNKNOWN;
      Str_Full_Copy(reply->module_name, "ILLEGAL_MODULE_ID", sizeof(reply->module_name));
   }
}

static size_t Get_Running_App_Ids(SAL_App_Id_T reply_router_thread_ids[])
{
   size_t i;
   size_t number_of_router_threads = 0;
   size_t ch_no = SAL_I_NS_Thread_Data->channels.number_of_channels;
   SAL_NS_Channel_T* ch = SAL_I_NS_Thread_Data->channels.channel;

   /** Create list of running applications (Router Threads).
    */
   for (i = 0; i < ch_no; i++)
   {
      if (ch[i].port_id == SAL_ROUTER_THREAD_ID)
      {
         reply_router_thread_ids[number_of_router_threads++] = ch[i].app_id;
      }
   }
   reply_router_thread_ids[number_of_router_threads++] = 0;

   return number_of_router_threads;
}

static void Update_Events(const SAL_NS_Subscribtion_T* subscribe_data, uint8_t subscribe)
{
   SAL_App_Id_T ap_id = subscribe_data->app_id;
   if ((ap_id >= SAL_FIRST_APP_ID) && (ap_id < SAL_LAST_APP_ID))
   {
      size_t ev_id_index;
      SAL_Event_Id_T ev_id;
      size_t ev_no = subscribe_data->number_of_events;

      /* Address of first element in event list */
      const SAL_Event_Id_T* ev_id_list = (const SAL_Event_Id_T*) (const void*) &subscribe_data[1];

      for (ev_id_index = 0; ev_id_index < ev_no; ev_id_index++)
      {
         ev_id = ev_id_list[ev_id_index];
         if ((ev_id > SAL_EVG_FIRST_PUBLISHED_EVENT_ID) && (ev_id < SAL_EVG_LAST_PUBLISHED_EVENT_ID))
         {
            SAL_I_NS_Thread_Data->routing_table[ev_id][ap_id] = subscribe;
         }
         else
         {
            Tr_Fault_1("Invalid event_id %d", (int)ev_id);
         }
      }
   }
   else
   {
      Tr_Fault_1("Invalid app_id %d", (int)ap_id);
   }
}

static void Unsubscribe_App(const SAL_NS_Unsubscribe_App_T* unsubscribe_app)
{
   SAL_App_Id_T ap_id = unsubscribe_app->app_id;

   if ((ap_id >= SAL_FIRST_APP_ID) && (ap_id < SAL_LAST_APP_ID))
   {
      SAL_Event_Id_T ev_id;
      for (ev_id = SAL_EVG_FIRST_PUBLISHED_EVENT_ID; ev_id < SAL_EVG_LAST_PUBLISHED_EVENT_ID; ev_id++)
      {
         SAL_I_NS_Thread_Data->routing_table[ev_id][ap_id] = 0;
      }
   }
   else
   {
      Tr_Fault_1("Invalid app_id %d", (int)ap_id);
   }
}

static void SAL_Name_Server_Thread(void *param)
{
   int32_t status;
   uint8_t is_pulse;
   size_t reply_data_length;
   size_t command_length;
   void* reply_data;
   SAL_NS_Reply_Buffer_T reply_buffer;
   uint8_t cmd_buff[sizeof(SAL_NS_Command_Buffer_T) + sizeof(SAL_NS_Variable_Length_Part_T)];
   SAL_Receive_T receive_id;
   SAL_NS_Command_Buffer_T *cmd_ptr = (SAL_NS_Command_Buffer_T*) cmd_buff;

   SAL_Signal_Ready();

   while (1)
   {
      if (!SAL_Sync_Receive(&SAL_I_NS_Thread_Data->name_server_channel, cmd_buff, sizeof(cmd_buff), &command_length, &is_pulse,
         &receive_id))
      {
         Tr_Fault("SAL_Sync_Receive");
         continue;
      }

      if (is_pulse)
      {
         Tr_Info_Lo("Pulse was received");
         continue;
      }

      status = 1;
      reply_data = NULL;
      reply_data_length = 0;

      switch (cmd_ptr->command)
      {
         case SAL_NS_BIND_CHANNEL:
            status = (int32_t) Bind_Channel((SAL_NS_Bind_Channel_T*) (void*) cmd_buff);
            break;

         case SAL_NS_UNBIND_CHANNEL:
            status = (int32_t) Unbind_Channel((SAL_NS_Unbind_Channel_T*) (void*) cmd_buff);
            break;

         case SAL_NS_RESOLVE_CHANNEL:
            status
               = (int32_t) Resolve_Channel((SAL_NS_Resolve_Channel_T*) (void*) cmd_buff, (SAL_Channel_T*) (void*) &reply_buffer);
            if (status)
            {
               /** Set the reply data buffer to the resolved channle data
                */
               reply_data = &reply_buffer;
               reply_data_length = sizeof(SAL_Channel_T);
            }
            break;

         case SAL_NS_UNBIND_APP:
            Unbind_App((SAL_NS_Unbind_App_T*) (void*) cmd_buff);
            break;

         case SAL_NS_GET_RUNNING_APP_IDS:
            reply_data_length = Get_Running_App_Ids(reply_buffer.app_id_list) * sizeof(SAL_App_Id_T);
            reply_data = reply_buffer.app_id_list;
            break;

         case SAL_NS_SUBSCRIBE:
            Update_Events((SAL_NS_Subscribtion_T*) (void*) cmd_buff, 1);
            break;

         case SAL_NS_UNSUBSCRIBE:
            Update_Events((SAL_NS_Subscribtion_T *) (void*) cmd_buff, 0);
            break;

         case SAL_NS_UNSUBSCRIBE_APP:
            Unsubscribe_App((SAL_NS_Unsubscribe_App_T*) (void*) cmd_buff);
            break;

         case SAL_NS_GET_LAST_APP_ID:
            reply_buffer.last_app_id = SAL_MAX_APPS;
            reply_data = &reply_buffer.last_app_id;
            reply_data_length = sizeof(reply_buffer.last_app_id);
            break;

         case SAL_NS_GET_APP_DATA:
            Get_App_Data((SAL_NS_Get_App_Data_T*) cmd_buff, &reply_buffer.app_data);
            reply_data = &reply_buffer.app_data;
            reply_data_length = sizeof(reply_buffer.app_data);
            break;

         case SAL_NS_GET_LAST_DBG_MODULE_ID:
            reply_buffer.last_module_id = TR_NUM_MODULE_IDS - 1;
            reply_data = &reply_buffer.last_module_id;
            reply_data_length = sizeof(reply_buffer.last_module_id);
            break;

         case SAL_NS_GET_DBG_MODULE_DATA:
            Get_Module_Data((SAL_NS_Get_Dbg_Module_Data_T*) cmd_buff, &reply_buffer.module_data);
            reply_data = &reply_buffer.module_data;
            reply_data_length = sizeof(reply_buffer.module_data);
            break;

         default:
            Tr_Fault_1("Invalid command %d\n", (int)cmd_ptr->command);
            status = 0;
      }

      (void) SAL_Sync_Reply(&receive_id, status, reply_data, reply_data_length);
   }
}

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

bool_t SAL_Name_Server_Init(void)
{
   bool_t status = false;

   if (SAL_Create_Shared_Memory(0, 0, /* Name Server Shared memory name: key ID=0 and sukey ID=0 */
   sizeof(SAL_NS_Data_T), SAL_SHMEM_RDWR, &SH_handle))
   {
      SAL_I_NS_Thread_Data = SH_handle.data;
      (void) memset(SAL_I_NS_Thread_Data, 0, sizeof(SAL_NS_Data_T));

      /** Create NameServer channel and store its parameters
       *  in shared memory to be available to Name_Service_Proxy.
       */
      if (SAL_Create_Channel(&SAL_I_NS_Thread_Data->name_server_channel))
      {
         SAL_I_NS_Thread_Data->max_number_of_channels = SAL_MAX_CHANNELS;
         SAL_I_NS_Thread_Data->app_number = SAL_MAX_APPS;
         SAL_I_NS_Thread_Data->event_number = SAL_MAX_EVENTS;

         status = true;
      }
      else
      {
         Tr_Fault("Name_Server/SAL_Create_Channel error");
      }
   }
   else
   {
      Tr_Fault("Name_Server/Create_Shared_Memory error");
   }
   return status;
}

SAL_Thread_Id_T SAL_Start_Name_Server_Thread(SAL_Thread_Attr_T * thread_attr)
{
   SAL_Thread_Id_T thread_id[1];

   /* start Naming Service */
   thread_id[0] = SAL_Create_Thread(SAL_Name_Server_Thread, NULL, thread_attr);

   if (thread_id[0] > 0)
   {
      /** Wake up created thread. In "normal" scenario all created threads
       *  wait until the Router Thread will be started calling SAL_Run().
       */
      (void) SAL_Signal_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_ready_sem);

      /** Wait for Name Server thread to initialize itself
       */
      SAL_Wait_Ready(thread_id, Num_Elems(thread_id));

      /** Restore thread_ready_sem of the Router Thread.
       */
      (void) SAL_Wait_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_ready_sem);
   }

   return thread_id[0];
}

void SAL_Name_Server_Destroy(void)
{
   (void) SAL_Close_Shared_Memory(&SH_handle);
}

const char_t* SAL_Get_App_Name(SAL_App_Id_T app_id)
{
   const char_t *name = "ILLEGAL";

   if (app_id < SAL_LAST_APP_ID)
   {
      name = App_Names[app_id];
   }

   return name;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 19-Jul-2013 Larry Piekarski
 * - Task 161388: Removed deprecated util functions.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 14
 *   - Task 1334: Fixed compiler warnings
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 27-May-2015 Kirk Bailey
 *    - Filled in unresolved comments.
 *
 * - 18-Aug-2015 Kirk Bailey
 *   - Fixed Coverity issues.
 * -----------------------------------------------------------------------*/
