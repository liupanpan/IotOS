/* -------------------------------------------------------------------------
 *
 * File:        xsal_ns_proxy.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the Name Server proxy.
 *
 *------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_ns_proxy.h"

#include <string.h>

EM_FILENUM(XSAL_NS_MODULE_ID, 0); /* Identify file for trace/assert purposes */

/* ---------------------------
 *  Local Object Definitions
 * --------------------------- */

static SAL_Shared_Memory_Handle_T SAL_I_NS_Sh_Mem_Handle;

/** Pointer to Name Server data (channel to the Name Server, 
 *  channels of the running application and routing table)
 */
static SAL_NS_Data_T* SAL_I_NS_Proxy_Data;

/** Connection to the Name Server
 */
static SAL_Connection_T SAL_I_NS_Connection;

/* -----------------------
 *  Function Definitions
 * ----------------------- */

#if defined (WIN32)

/** This is a global function, defined in "xsal_rpc_c_win32.c" file,
 *  but not consciously published in any "sal" headers files.
 *  It shouldn't be called directly.
 */
void SAL_Initialize_TCP_IP(void);

#endif

/* -----------------------------
 *  Public function definition
 * ----------------------------- */

bool_t SAL_NS_Init_Proxy(SAL_NS_Data_T** ns_data)
{
   bool_t status;

   PBC_Require(SAL_I_NS_Proxy_Data == NULL, "NULL proxy data pointer");
   PBC_Require(ns_data != NULL, "NULL NS data");

   status = SAL_Open_Shared_Memory(0, /* Key and subkey equal 0 are  */
                                   0, /* reserved for Naming Service */
                                   SAL_SHMEM_RDONLY, &SAL_I_NS_Sh_Mem_Handle);
   if (status)
   {
      SAL_NS_Data_T* tmp_ns_data = (SAL_NS_Data_T*) SAL_I_NS_Sh_Mem_Handle.data;
#if defined (WIN32)
      SAL_Initialize_TCP_IP();
#endif
      /** Create connection to the Name Server
       */
      if (SAL_Create_Connection(&tmp_ns_data->name_server_channel, &SAL_I_NS_Connection))
      {
         SAL_I_NS_Proxy_Data = tmp_ns_data;
         *ns_data = SAL_I_NS_Proxy_Data;
         status = true;
      }
      else
      {
         (void) SAL_Close_Shared_Memory(&SAL_I_NS_Sh_Mem_Handle);
         status = false;
      }
   }
   if (!status)
   {
      Tr_Fault("Failed to initialize NS Proxy");
   }
   return status;
}

void SAL_NS_Deinit_Proxy(void)
{
   PBC_Require(SAL_I_NS_Sh_Mem_Handle.data != NULL, "NULL data handle");

   (void) SAL_Close_Shared_Memory(&SAL_I_NS_Sh_Mem_Handle);
   SAL_I_NS_Sh_Mem_Handle.data = NULL;
   SAL_I_NS_Proxy_Data = NULL;
}

bool_t SAL_NS_Config_Ok(void)
{
   PBC_Require(SAL_I_NS_Proxy_Data != NULL, "NULL proxy data");

   return (bool_t) ((SAL_I_NS_Proxy_Data->max_number_of_channels == SAL_MAX_CHANNELS)
      && (SAL_I_NS_Proxy_Data->app_number == SAL_MAX_APPS)
      && (SAL_I_NS_Proxy_Data->event_number == SAL_MAX_EVENTS));
}

bool_t SAL_NS_Bind_Channel(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, const SAL_Channel_T* channel)
{
   int32_t status = 1;
   SAL_NS_Bind_Channel_T cmd = { 0 };

   /** Do not check if the app_id < SAL_NO_APP_ID. */
   PBC_Require(channel != NULL, "NULL channel");

   cmd.command = SAL_NS_BIND_CHANNEL;
   cmd.app_id = app_id;
   cmd.port_id = port_id;

   /** Copy channel data by memcpy function not by assign operator. 
    *  In the Name Server the channels are comparing with themselves 
    *  by memcmp function. Because data alignment, channel data may
    *  have some padding data between structure fields. The assign 
    *  operator may copy only structure fields, without padding data.
    *  In this case data comparison by memcmp will not work.
    */
   (void) memcpy(&cmd.channel, channel, sizeof(SAL_Channel_T));

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(SAL_NS_Bind_Channel_T), NULL, 0, &status))
   {
      Tr_Fault("NS_Bind_Channel error");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Unbind_Channel(const SAL_Channel_T* channel)
{
   int32_t status = 1;
   SAL_NS_Unbind_Channel_T cmd = { 0 };

   PBC_Require(channel != NULL, "NULL channel");

   cmd.command = SAL_NS_UNBIND_CHANNEL;

   /* memcpy - see comments in SAL_NS_Bind_Channel */
   (void) memcpy(&cmd.channel, channel, sizeof(SAL_Channel_T));

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(SAL_NS_Unbind_Channel_T), NULL, 0, &status))
   {
      Tr_Fault("NS_Unbind_Channel error");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Resolve_Channel(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, SAL_Channel_T* channel)
{
   int32_t status = 1;
   SAL_NS_Resolve_Channel_T cmd = { 0 };

   /** Do not check if the app_id < SAL_NO_APP_ID. */
   PBC_Require(channel != NULL, "NULL channel");

   cmd.command = SAL_NS_RESOLVE_CHANNEL;
   cmd.app_id = app_id;
   cmd.port_id = port_id;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(SAL_NS_Resolve_Channel_T),
                      channel, sizeof(SAL_Channel_T), &status))
   {
      Tr_Fault("NS_Resolve_Channel");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Unbind_App(SAL_App_Id_T app_id)
{
   int32_t status = 1;
   SAL_NS_Unbind_App_T cmd = { 0 };

   /** Do not check if the app_id < SAL_NO_APP_ID. */
   PBC_Require(app_id >= SAL_FIRST_APP_ID, "Illegal app_id");

   cmd.command = SAL_NS_UNBIND_APP;
   cmd.app_id = app_id;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(SAL_NS_Unbind_App_T), NULL, 0, &status))
   {
      Tr_Fault("NS_Unbind_App error");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Get_Running_App_Ids(SAL_App_Id_T reply_app_id_list[])
{
   int32_t status = 1;
   SAL_NS_Get_Running_Apps_T cmd = { 0 };

   PBC_Require(reply_app_id_list != NULL, "NULL reply list");

   cmd.command = SAL_NS_GET_RUNNING_APP_IDS;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(SAL_NS_Get_Running_Apps_T),
                      reply_app_id_list, (SAL_MAX_APPS + 1) * sizeof(SAL_App_Id_T), &status))
   {
      Tr_Fault("Name_Server_Get_Running_Apps");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Get_Last_App_Id(uint16_t *last_app_id)
{
   int32_t status = 1;
   SAL_NS_Get_Last_App_Id_T cmd = { 0 };

   PBC_Require(last_app_id != NULL, "NULL last_app_id");

   cmd.command = SAL_NS_GET_LAST_APP_ID;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(cmd), last_app_id, sizeof(*last_app_id), &status))
   {
      Tr_Fault("SAL_NS_Get_Last_App_Id");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Get_App_Data(uint16_t app_id, SAL_App_Data_T *app_data)
{
   int32_t status = 1;
   SAL_NS_Get_App_Data_T cmd = { 0 };

   PBC_Require(app_data != NULL, "NULL app_data");

   cmd.command = SAL_NS_GET_APP_DATA;
   cmd.app_id = app_id;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(cmd), app_data, sizeof(*app_data), &status))
   {
      Tr_Fault("SAL_NS_Get_App_Data");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Get_Last_Dbg_Module_Id(uint16_t *last_module_id)
{
   int32_t status = 1;
   SAL_NS_Get_Last_Dbg_Module_Id_T cmd = { 0 };

   PBC_Require(last_module_id != NULL, "NULL last_module_id");

   cmd.command = SAL_NS_GET_LAST_DBG_MODULE_ID;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(cmd), last_module_id, sizeof(*last_module_id),
      &status))
   {
      Tr_Fault("SAL_NS_Get_Last_Dbg_Module_Id");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Get_Dbg_Module_Data(uint16_t module_id, SAL_Dbg_Module_Data_T *module_data)
{
   int32_t status = 1;
   SAL_NS_Get_Dbg_Module_Data_T cmd = { 0 };

   PBC_Require(module_data != NULL, "NULL module_data");

   cmd.command = SAL_NS_GET_DBG_MODULE_DATA;
   cmd.module_id = module_id;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(cmd), module_data, sizeof(*module_data), &status))
   {
      Tr_Fault("SAL_NS_Get_App_Data");
      status = 0;
   }
   return (bool_t) (status != 0);
}

bool_t SAL_NS_Subscribe(SAL_App_Id_T app_id, const SAL_Event_Id_T event_id_list[], uint32_t number_of_events)
{
   int32_t status = 1;

   if (number_of_events > 0)
   {
      SAL_IOV_T send_buf[2];
      SAL_NS_Subscribtion_T cmd = { 0 };

      /** In Subscribe operaion app_id is an index to the Routing Table.
       *  Check if the app_id < SAL_LAST_APP_ID.
       */
      PBC_Require(app_id >= SAL_FIRST_APP_ID, "Invalid app_id");
      PBC_Require(app_id < SAL_LAST_APP_ID, "Invalid app_id");
      PBC_Require(event_id_list != NULL, "NULL event list");

      cmd.command = SAL_NS_SUBSCRIBE;
      cmd.app_id = app_id;
      cmd.number_of_events = number_of_events;

      send_buf[0].iov_base = &cmd;
      send_buf[0].iov_len = sizeof(SAL_NS_Subscribtion_T);

      send_buf[1].iov_base = (void*) event_id_list;
      send_buf[1].iov_len = number_of_events * sizeof(event_id_list[0]);

      if (!SAL_Sync_Send_VS(&SAL_I_NS_Connection, send_buf,
                            sizeof(send_buf) / sizeof(SAL_IOV_T), NULL, 0, &status))
      {
         Tr_Fault("NS_Subscribe");
         status = 0;
      }
   }
   return (bool_t) (status != 0);
}


bool_t SAL_NS_Unsubscribe(SAL_App_Id_T app_id, const SAL_Event_Id_T event_id_list[], uint32_t number_of_events)
{
   int32_t status = 1;

   if (number_of_events > 0)
   {
      SAL_IOV_T send_buf[2];
      SAL_NS_Subscribtion_T cmd = { 0 };

      /** In Unsubscribe operaion app_id is an index to the Routing Table.
       *  Check if the app_id < SAL_LAST_APP_ID.
       */
      PBC_Require(app_id >= SAL_FIRST_APP_ID, "Invalid app_id");
      PBC_Require(app_id < SAL_LAST_APP_ID, "Invalid app_id");
      PBC_Require(event_id_list != NULL, "NULL event list");

      cmd.command = SAL_NS_UNSUBSCRIBE;
      cmd.app_id = app_id;
      cmd.number_of_events = number_of_events;

      send_buf[0].iov_base = &cmd;
      send_buf[0].iov_len = sizeof(SAL_NS_Subscribtion_T);

      send_buf[1].iov_base = (void*) event_id_list;
      send_buf[1].iov_len = number_of_events * sizeof(event_id_list[0]);

      if (!SAL_Sync_Send_VS(&SAL_I_NS_Connection, send_buf,
                            sizeof(send_buf) / sizeof(SAL_IOV_T), NULL, 0, &status))
      {
         Tr_Fault("NS_Unsubscribe");
         status = 0;
      }
   }
   return (bool_t) (status != 0);
}


void SAL_NS_Unsubscribe_App(SAL_App_Id_T app_id)
{
   int32_t dummy_val = 0;
   SAL_NS_Unsubscribe_App_T cmd = { 0 };

   /** In Unsubscribe_App operaion app_id is an index to the Routing Table.
    *  Check if the app_id < SAL_LAST_APP_ID. 
    */
   PBC_Require(app_id >= SAL_FIRST_APP_ID, "Invalid app_id");
   PBC_Require(app_id < SAL_LAST_APP_ID, "Invalid app_id");

   cmd.command = SAL_NS_UNSUBSCRIBE_APP;
   cmd.app_id = app_id;

   if (!SAL_Sync_Send(&SAL_I_NS_Connection, &cmd, sizeof(SAL_NS_Unsubscribe_App_T), NULL, 0, &dummy_val))
   {
      Tr_Fault("NS_Unsubscribe_App failed");
   }
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 31-Oct-2013 Larry Piekarski Rev 12
 *   - Task 2589: Fixed uninitialized local variables (Valgrind will issue a 
 *     warning for use of uninitialized pad bytes)
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the
 *    enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 12Jun15  David Origer
 * - Task kok_basa#213204 : Fix Valgrind issues.
 *   - Use correct type in sizeof().
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
