/* -------------------------------------------------------------------------
 *
 * File:        xsal_rpc_qnx.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the synchronous communication for 
 *              QNX platform.
 * 
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <process.h>

EM_FILENUM(XSAL_IPC_MODULE_ID, 6); /* Identify file for trace/assert purposes */

/* -----------------------------
 *  Public Function Definitions
 * ----------------------------- */


bool_t SAL_Create_Channel(SAL_Channel_T* channel)
{
   SAL_Int_T channel_id;

   PBC_Require(channel != NULL, "NULL channel");

   channel_id = ChannelCreate_r(_NTO_CHF_SENDER_LEN);
   if (channel_id >= 0)
   {
      channel->app_pid    = getpid();
      channel->channel_id = channel_id;
   }

   if (channel_id < 0) { Tr_Fault_1("SAL_Create_Channel error: %s", strerror(-channel_id)); }

   return (bool_t)(channel_id >= 0);
}


bool_t SAL_Destroy_Channel(SAL_Channel_T* channel)
{
   SAL_Int_T status;
   
   PBC_Require(channel != NULL, "NULL channel");

   status = ChannelDestroy_r(channel->channel_id);
   if (status != EOK) { Tr_Fault_1("SAL_Destroy_Channel error: %s", strerror(status)); }

   return (bool_t)(status == EOK);
}


bool_t SAL_Create_Connection(
   const SAL_Channel_T* channel, 
   SAL_Connection_T* connection)
{
   PBC_Require(channel != NULL, "NULL channel");
   PBC_Require(connection != NULL, "NULL connection");

   *connection = ConnectAttach_r(
      0, 
      channel->app_pid,
      channel->channel_id,
      _NTO_SIDE_CHANNEL,
      _NTO_COF_CLOEXEC);

   if (*connection < 0)
   {
      /* Generate fault message at higher level */
      Tr_Info_Hi_1("SAL_Create_Connection error: %s", strerror(-(*connection)));
   }

   return (bool_t)(*connection >= 0);
}

#if defined (XSAL)

bool_t SAL_Bind(
   SAL_App_Id_T app_id, 
   SAL_Port_Id_T port_id, 
   const SAL_Channel_T* channel)
{

   return SAL_NS_Bind_Channel(app_id, port_id, channel);
}


bool_t SAL_Unbind(const SAL_Channel_T* channel)
{
   PBC_Require(channel != NULL, "NULL channel");

   return SAL_NS_Unbind_Channel(channel);
}


bool_t SAL_Connect(
   SAL_App_Id_T app_id,
   SAL_Port_Id_T port_id,
   SAL_Connection_T* connection)
{
   bool_t status = 0;
   SAL_Channel_T channel; 

   PBC_Require(connection != NULL, "NULL connection");

   if (SAL_NS_Resolve_Channel(app_id, port_id, &channel))
   {
      status = SAL_Create_Connection(&channel, connection);
   }

   return status;
}

#endif /* XSAL */


bool_t SAL_Disconnect(SAL_Connection_T* connection)
{
   SAL_Int_T status;

   PBC_Require(connection != NULL, "NULL connection");

   status = ConnectDetach_r(*connection);
   if (status != EOK) { Tr_Fault_1("SAL_Disconnect error: %s", strerror(status)); }

   return (bool_t)(status == EOK);
}


bool_t SAL_Send_Pulse(
   SAL_Connection_T* connection,
   int8_t  code, 
   int32_t value)
{
   SAL_Int_T status;

   PBC_Require(connection != NULL, "NULL connection");

   status = MsgSendPulse_r(*connection, 50, code, value);
   if (status != EOK) { Tr_Fault_1("SAL_Send_Pulse error: %s", strerror(status)); }

   return (bool_t)(status == EOK);
}


#if defined (XSAL)

bool_t SAL_Sync_Send_V(
   SAL_Connection_T* connection, 
   const SAL_IOV_T* message_buffers, 
   uint32_t number_of_buffers,
   const SAL_IOV_T* reply_buffers,
   uint32_t number_of_reply_buffers,
   int32_t* reply_status)
{
   PBC_Require(connection != NULL, "NULL connection");
   PBC_Require(message_buffers != NULL, "NULL message buffers");
   PBC_Require(reply_buffers != NULL, "NULL reply buffers");

   *reply_status = MsgSendv_r(
      *connection, 
      message_buffers, 
      number_of_buffers, 
      reply_buffers, 
      number_of_reply_buffers);

   if (*reply_status < 0) { Tr_Fault_1("SAL_Sync_Send_VS error: %s", strerror(-(*reply_status))); }

   return (bool_t)(*reply_status >= 0);
}


bool_t SAL_Sync_Send(
   SAL_Connection_T* connection,
   const void* message, 
   size_t message_size,
   void* reply,
   size_t reply_size,
   int32_t* reply_status)
{
   PBC_Require(connection != NULL, "NULL connection");
   PBC_Require(message != NULL, "NULL message");
   PBC_Require(message_size > 0, "Zero message size");
   PBC_Require(reply_status != NULL, "NULL reply status");

   *reply_status = MsgSend_r(
      *connection, 
      message, 
      message_size, 
      reply, 
      reply_size);

   if (*reply_status < 0) { Tr_Fault_1("SAL_Sync_Send error: %s", strerror(-(*reply_status))); }

   return (bool_t)(*reply_status  >= 0);
}

 
bool_t SAL_Sync_Send_VS(
   SAL_Connection_T* connection, 
   const SAL_IOV_T* message_buffers, 
   uint32_t number_of_buffers,
   void* reply, 
   size_t  reply_size, 
   int32_t* reply_status)
{
   PBC_Require(connection != NULL, "NULL connection");
   PBC_Require(message_buffers != NULL, "NULL message buffers");
   PBC_Require(number_of_buffers > 0,  "Zero buffers");
   PBC_Require(reply_status != NULL, "NULL reply status");

   *reply_status = MsgSendvs_r(
      *connection, 
      message_buffers, 
      number_of_buffers, 
      reply, 
      reply_size);

   if (*reply_status < 0) { Tr_Fault_1("SAL_Sync_Send_VS error: %s", strerror(-(*reply_status))); }

   return (bool_t)(*reply_status >= 0);
}


bool_t SAL_Sync_Send_SV(
   SAL_Connection_T* connection,
   const void* message,
   size_t message_size,
   const SAL_IOV_T* reply_buffers,
   uint32_t number_of_reply_buffers,
   int32_t*   reply_status)
{
   PBC_Require(connection != NULL, "NULL connection");
   PBC_Require(message != NULL, "NULL message");
   PBC_Require(reply_buffers != NULL, "NULL reply buffers");

   *reply_status = MsgSendsv_r(
      *connection, 
      message, 
      message_size, 
      reply_buffers, 
      number_of_reply_buffers);

   if (*reply_status < 0) { Tr_Fault_1("SAL_Sync_Send_SV error: %s", strerror(-(*reply_status))); }

   return (bool_t)(*reply_status >= 0);
}

#endif /* XSAL */


/* SAL_Sync_Receive() function is used internally in XSAL_LIGHT.
 */
bool_t SAL_Sync_Receive(
   SAL_Channel_T* channel,
   void*    buffer,
   size_t   buffer_size,
   size_t*  message_size,
   uint8_t* is_pulse,
   SAL_Receive_T* receive_id)
{
   SAL_Int_T rcv_id;
   struct _msg_info msg_info;

   PBC_Require(channel != NULL, "NULL channel");
   PBC_Require(buffer != NULL, "NULL buffer");
   PBC_Require(buffer_size > 0, "Non-positive buffer size");
   PBC_Require(message_size != NULL, "NULL message size");
   PBC_Require(is_pulse != NULL, "NULL is_pulse ptr");
   PBC_Require(receive_id != NULL, "NULL receive id");

   rcv_id = MsgReceive_r(
      channel->channel_id, 
      buffer, 
      buffer_size, 
      &msg_info);

   if (rcv_id > 0) /* Message */
   {
      *is_pulse = 0;
      *message_size = msg_info.srcmsglen;
      receive_id->receive_id = rcv_id;
      receive_id->offset = buffer_size;
   }
   else if (rcv_id == 0) /* Pulse */
   {
      *is_pulse = 1;
   }

   if (rcv_id < 0) { Tr_Fault_1("SAL_Sync_Receive error: %s", strerror(-rcv_id)); }

   return (bool_t)(rcv_id >= 0);
}

#if defined (XSAL)

bool_t SAL_Sync_Read(
   SAL_Receive_T* receive_id,
   void* buffer,
   size_t buffer_size,
   size_t* received_message_size)
{
   SAL_Int_T msg_size;

   PBC_Require(receive_id != NULL, "NULL receive id");
   PBC_Require(buffer != NULL, "NULL buffer");
   PBC_Require(buffer_size > 0, "Non-positive buffer size");
   PBC_Require(received_message_size != NULL, "NULL received message size");

   msg_size = MsgRead_r(
      receive_id->receive_id, 
      buffer, 
      buffer_size, 
      receive_id->offset);

   if (msg_size > 0)
   {
      *received_message_size = msg_size;
      receive_id->offset += msg_size;
   }

   if (msg_size < 0) { Tr_Fault_1("SAL_Sync_Read error: %s", strerror(-msg_size)); }

   return (bool_t)(msg_size >= 0);
}


bool_t SAL_Sync_Reply_V(
   const SAL_Receive_T* receive_id,
   int32_t reply_status,
   const SAL_IOV_T* reply_buffers,
   uint32_t number_of_buffers)
{
   SAL_Int_T ret_status;
   
   PBC_Require(receive_id != NULL, "NULL receive id");
   PBC_Require(reply_buffers != NULL, "NULL reply buffers");

   ret_status = MsgReplyv_r(
      receive_id->receive_id, 
      reply_status, 
      reply_buffers, 
      number_of_buffers);

   if (ret_status < 0) { Tr_Fault_1("SAL_Sync_Reply_V error: %s", strerror(-ret_status)); }

   return (bool_t)(ret_status >= 0);
}
#endif /* XSAL */


bool_t SAL_Sync_Reply(
   const SAL_Receive_T* receive_id,
   int32_t reply_status,
   const void* reply_message,
   size_t reply_message_size)
{
   SAL_Int_T ret_status;

   PBC_Require(receive_id != NULL, "NULL receive id");
   /* reply_message can be NULL */

   ret_status = MsgReply_r(
      receive_id->receive_id,
      reply_status, 
      reply_message, 
      reply_message_size);

   if (ret_status < 0) { Tr_Fault_1("SAL_Sync_Reply error: %s", strerror(-ret_status)); }

   return (bool_t)(ret_status >= 0);
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
