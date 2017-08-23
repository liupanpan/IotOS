/* -------------------------------------------------------------------------
 *
 * File:        xsal_rpc_win32.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the synchronous communication for 
 *              Win32 platform.
 *
 *------------------------------------------------------------------------*/

#include "xsal.h"

#if defined (XSAL)

#include "xsal_i_assert.h"

#include <stdio.h>

EM_FILENUM(XSAL_IPC_MODULE_ID, 4); /* Identify file for trace/assert purposes */


/* ---------------------------------------
 *  Local Preprocessor #define Constants
 * --------------------------------------- */


#define PULSE_REPLY_DATA 15


/* --------------------------
 *  Local Type Declarations
 * --------------------------*/


enum EMessage_Type
{
   EMessage,
   EPulse
};


typedef struct Message_Send_Header_Tag
{
   /** EMessage or EPulse
    */
   int mesage_type;

   /** Message size (without header)
    */
   size_t mesage_size;
} Message_Send_Header_T;


typedef struct Message_Reply_Header_Tag
{
   /** Reply size (without header)
    */
   int reply_size;

   /** Reply status
    */
   int reply_status;
} Message_Reply_Header_T;


/* ---------------------------
 *  Local Object Definitions
 * ---------------------------*/


static bool_t TCP_IP_Initialized = false;


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


/** This is a global function, but not published in any "xsal" 
 *  headers files. It is used internally by functions from 
 *  this file and from "name_server_proxy.c".
 *  It shouldn't be called explicitly.
 */
void SAL_Initialize_TCP_IP(void)
{
   WSADATA wsaData;
   WORD wVersionRequested;

   wVersionRequested = MAKEWORD(2, 2);

   if (WSAStartup(wVersionRequested, &wsaData) != 0)
   {
      Tr_Fault_1("SAL_Initialize_TCP_IP/WSAStartup error: err=%d", (int)GetLastError());
      exit(1);
   }
   TCP_IP_Initialized = true;
}


bool_t SAL_Create_Channel(SAL_Channel_T* channel)
{
   int tcp_address_size;
   struct sockaddr_in tcp_address;

   PBC_Require(channel != 0, "NULL channel");

   if (!TCP_IP_Initialized)
   {
      SAL_Initialize_TCP_IP();
   }

   memset(channel, 0, sizeof(SAL_Channel_T));

   if ((channel->socket_fd = socket(AF_INET, SOCK_STREAM, 0)) ==
      INVALID_SOCKET)
   {
      Tr_Fault_1("SAL_Create_Channel: socket() %d", (int)WSAGetLastError());
      return false;
   }

   /** Assign local address and (any) port
    */
   tcp_address.sin_family           = AF_INET;
   tcp_address.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
   tcp_address.sin_port             = 0;   /* Any port */

   if (bind(
      channel->socket_fd,
      (struct sockaddr *)&tcp_address,
      sizeof(tcp_address)) < 0)
   {
      Tr_Fault_1("SAL_Create_Channel: bind() %d", (int)WSAGetLastError());
      closesocket(channel->socket_fd);
      return false;
   }

   /** Read assigned port number
    */
   tcp_address_size = sizeof(struct sockaddr_in);
   if (getsockname(channel->socket_fd,
      (struct sockaddr *)&tcp_address,
      &tcp_address_size) < 0)
   {
      Tr_Fault_1("SAL_Create_Channel: getsockname() %d", (int)WSAGetLastError());
      closesocket(channel->socket_fd);
      return false;
   }

   channel->tcp_port = tcp_address.sin_port;
   if (listen(channel->socket_fd, SOMAXCONN) == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Create_Channel: listen() %d", (int)WSAGetLastError());
      closesocket(channel->socket_fd);
      return false;
   }

   return true;
}


bool_t SAL_Create_Connection(
   const SAL_Channel_T* channel, 
   SAL_Connection_T* connection)
{
   PBC_Require(channel != NULL && connection != NULL, "Invalid connection");
   *connection = channel->tcp_port;
   return *connection >= 0;
}


bool_t SAL_Bind(
   SAL_App_Id_T app_id, 
   SAL_Port_Id_T port_id,
   const SAL_Channel_T* channel)
{
   PBC_Require(channel != NULL, "NULL channel");

   return SAL_NS_Bind_Channel(app_id, port_id, channel);
}


bool_t SAL_Unbind(const SAL_Channel_T* channel)
{
   PBC_Require(channel != NULL, "NULL channel");
   return SAL_NS_Unbind_Channel(channel);
}


bool_t SAL_Destroy_Channel(SAL_Channel_T* channel)
{
   PBC_Require(TCP_IP_Initialized && channel != NULL, "TCP/IP not ready");

   if (closesocket(channel->socket_fd) == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Destroy_Channel: closesocket() %d", (int)WSAGetLastError());
   }
   channel->socket_fd = (SOCKET)-1;

   return true;
}


bool_t SAL_Sync_Receive(
   SAL_Channel_T* channel,
   void*      buffer,
   size_t    buffer_size,
   size_t*   message_size,
   uint8_t*  is_pulse,
   SAL_Receive_T* receive_id)
{
   int data_to_read;
   int buffer_offset;
   int read_packet_size;
   int return_status = 1;
   Message_Send_Header_T msg_hdr;
   int client_address_length;
   struct sockaddr_in client_address;
   char_t rdata = PULSE_REPLY_DATA;

   PBC_Require(TCP_IP_Initialized, "TCP/IP not initialized");

   client_address_length = sizeof(struct sockaddr_in);

   /** Wait for connection
    */
   receive_id->socket_fd = accept(
      channel->socket_fd,
      (struct sockaddr *)&client_address,
      &client_address_length);

   if (receive_id->socket_fd == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Sync_Receive: accept() %d", (int)WSAGetLastError());
      return false;
   }

   /** Read message header
    */
   if (recv(
         receive_id->socket_fd,
         (char_t*)&msg_hdr,
         sizeof(Message_Send_Header_T),
         0)
      != sizeof(Message_Send_Header_T))
   {
      Tr_Fault_1("SAL_Sync_Receive: recv(header) %d", (int)WSAGetLastError());
      closesocket(receive_id->socket_fd);
      return false;
   }

   *is_pulse = (msg_hdr.mesage_type == EPulse)?(1):(0);
   *message_size = msg_hdr.mesage_size;
   data_to_read = (msg_hdr.mesage_size > buffer_size)? 
      (buffer_size):(msg_hdr.mesage_size);
   buffer_offset = 0;

   while(data_to_read > 0)
   {
      read_packet_size = recv(receive_id->socket_fd,
         (char_t*)buffer + buffer_offset,
         data_to_read,
         0);

      if (read_packet_size < 0)
      {
         Tr_Fault_1("SAL_Sync_Receive: recv(data) %d", (int)WSAGetLastError());
         closesocket(receive_id->socket_fd);
         return false;
      }
 
      buffer_offset += read_packet_size;
      data_to_read -= read_packet_size;
   }
   receive_id->data_to_read = msg_hdr.mesage_size - buffer_offset;

   if (*is_pulse)
   {
      if (send(receive_id->socket_fd, &rdata, sizeof (rdata), 0) != sizeof (rdata))
      {
         Tr_Fault_1("SAL_Sync_Receive: is_pulse, send() %d", (int)WSAGetLastError());
         closesocket(receive_id->socket_fd);
         return false;
      }

      if (closesocket(receive_id->socket_fd) != 0)
      {
         Tr_Fault_1("SAL_Sync_Receive: is_pulse, closesocket() %d", (int)WSAGetLastError());
      }
      receive_id->socket_fd = 0;
   }
   return return_status;
}


bool_t SAL_Sync_Read(
   SAL_Receive_T* receive_id,
   void*   buffer,
   size_t  buffer_size,
   size_t* received_message_size)
{
   int read_packet_size;
   int data_to_read;
   int buffer_offset;

   PBC_Require(TCP_IP_Initialized, "TCP/IP not ready");

   buffer_offset = 0;
   data_to_read = (receive_id->data_to_read > (int)buffer_size)?
      (buffer_size):(receive_id->data_to_read);

   while(data_to_read > 0)
   {
      read_packet_size = recv(
         receive_id->socket_fd,
         (char_t*)buffer + buffer_offset, 
         data_to_read,
         0);

      if (read_packet_size < 0)
      {
         Tr_Fault_1("SAL_Sync_Read: recv() %d", (int)WSAGetLastError());
         closesocket(receive_id->socket_fd);
         return false;
      }
      buffer_offset += read_packet_size;
      data_to_read -= read_packet_size;
   }
   receive_id->data_to_read -= buffer_offset;
   *received_message_size = buffer_offset;
   return true;
}


bool_t SAL_Sync_Reply_V(
   const SAL_Receive_T* receive_id,
   int32_t reply_status,
   const SAL_IOV_T* reply_buffers,
   uint32_t number_of_buffers)
{
   uint32_t buffer_index;
   int sent_packet_size;
   int return_status = 1;
   Message_Reply_Header_T reply_header;
  
   PBC_Require(TCP_IP_Initialized, "TCP/IP not initialized");

   reply_header.reply_size   = 0;
   reply_header.reply_status = reply_status;

   for(buffer_index=0; buffer_index < number_of_buffers; buffer_index++)
   {
      reply_header.reply_size += reply_buffers[buffer_index].iov_len;
   }

   /** Send header
    */
   if (send(
         receive_id->socket_fd, 
         (char_t*)&reply_header, sizeof(Message_Reply_Header_T),
         0)
      == sizeof(Message_Reply_Header_T))
   {
      /** Next send the reply data
       */
      for(
         buffer_index=0;
         buffer_index < number_of_buffers && return_status != 0 && reply_header.reply_size > 0;
         buffer_index++)
      {
         size_t sent_data = 0;
         do 
         {
            sent_packet_size = send(
               receive_id->socket_fd,
               (const char_t*)reply_buffers[buffer_index].iov_base + sent_data,
               reply_buffers[buffer_index].iov_len - sent_data,
               0);

            if (sent_packet_size == SOCKET_ERROR)
            {
               Tr_Fault_1("SAL_Sync_Reply_V: send(data) %d", (int)WSAGetLastError());
               return_status = 0;
               break;
            }
            sent_data += sent_packet_size;
         } while(sent_data < reply_buffers[buffer_index].iov_len);
      }
   }
   else
   {
      Tr_Fault_1("SAL_Sync_Reply_V: send(header) %d", (int)WSAGetLastError());
      return_status = 0;
   }

   if (closesocket(receive_id->socket_fd) != 0)
   {
      Tr_Fault_1("SAL_Sync_Reply_V: closesocket() %d", (int)WSAGetLastError());
   }
   return return_status;
}


bool_t SAL_Sync_Reply(
   const SAL_Receive_T* receive_id,
   int32_t reply_status,
   const void* reply_message,
   size_t reply_message_size)
{
   SAL_IOV_T reply_buf[1];

   reply_buf[0].iov_base = (void*)reply_message;
   reply_buf[0].iov_len  = reply_message_size;

   return SAL_Sync_Reply_V(
      receive_id,
      reply_status,
      reply_buf,
      sizeof(reply_buf)/sizeof(SAL_IOV_T));
}


bool_t SAL_Connect(
   SAL_App_Id_T app_id, 
   SAL_Port_Id_T port_id, 
   SAL_Connection_T* connection)
{
   int status = 0;
   SAL_Channel_T channel = {0};

   PBC_Require(connection != NULL, "NULL connection pointer");

   if (!TCP_IP_Initialized)
   {
      SAL_Initialize_TCP_IP();
   }

   if (SAL_NS_Resolve_Channel(app_id, port_id, &channel))
   {
      status = SAL_Create_Connection(&channel, connection);
   }
   return status;
}


bool_t SAL_Disconnect(SAL_Connection_T* connection)
{
   PBC_Require(connection != NULL, "NULL connection pointer");

   *connection = 0;
   return true;
}


bool_t SAL_Sync_Send_V(
   SAL_Connection_T* connection,
   const SAL_IOV_T* message_buffers,
   uint32_t number_of_buffers,
   const SAL_IOV_T* reply_buffers,
   uint32_t number_of_reply_buffers,
   int32_t*   reply_status)
{
   SOCKET socket_fd;
   uint32_t buffer_index;

   int send_message_size;
   int receive_buffers_size;
   int reply_message_size;
   int received_data_size; 
   struct sockaddr_in server_address;
   Message_Send_Header_T msg_hdr;
   Message_Reply_Header_T reply_header;

   int sent_packet_size;
   struct linger li = { 1, 0 };
   
   PBC_Require(TCP_IP_Initialized, "TCP/IP not initialize");

   if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Sync_Send_V: create socket %d", (int)WSAGetLastError());
      return false;
   }

   if (setsockopt(socket_fd, SOL_SOCKET, SO_LINGER, (const char_t*)&li, sizeof(struct linger)) != 0)
   {
      Tr_Fault_1("SAL_Sync_Send_V: setsockopt() %d", (int)WSAGetLastError());
      closesocket (socket_fd);
      return false;
   }

   memset(&server_address, 0, sizeof(server_address));

   server_address.sin_family           = AF_INET;
   server_address.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
   server_address.sin_port             = (u_short)*connection;

   if (connect(
      socket_fd, 
      (struct sockaddr *)&server_address,
      sizeof(struct sockaddr_in)) == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Sync_Send_V: connect() %d", (int)WSAGetLastError());
      closesocket(socket_fd);
      return false;
   }

   send_message_size = 0;
   for(buffer_index=0; buffer_index < number_of_buffers; buffer_index++)
   {
      send_message_size += message_buffers[buffer_index].iov_len;
   }

   msg_hdr.mesage_type = EMessage;
   msg_hdr.mesage_size = send_message_size;

   /** First send the message header
    */
   if (send(socket_fd, (char_t*)&msg_hdr, sizeof(Message_Send_Header_T), 0)
      == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Sync_Send_V: send(message header) %d", (int)WSAGetLastError());
      closesocket(socket_fd);
      return 0;
    }

   /** Next send the message data
    */
   for(
      buffer_index=0;
      buffer_index < number_of_buffers;
      buffer_index++)
   {
      size_t sent_data = 0;
      do 
      {
         sent_packet_size = send(
            socket_fd,
            (const char_t*)message_buffers[buffer_index].iov_base + sent_data,
            message_buffers[buffer_index].iov_len - sent_data,
            0);

         if (sent_packet_size == SOCKET_ERROR)
         {
            Tr_Fault_1("SAL_Sync_Send_V: send(data) %d", (int)WSAGetLastError());
            closesocket(socket_fd);
            return false;
         }
         sent_data += sent_packet_size;
      } while(sent_data < message_buffers[buffer_index].iov_len);
   }

   /** Wait for replay. First read replay message length,
    *  next reply message data.
    */
   if (recv(
         socket_fd,
         (char_t*)&reply_header,
         sizeof(Message_Reply_Header_T),
         0) 
      != sizeof(Message_Reply_Header_T))
   {
      Tr_Fault_1("SAL_Sync_Send_V: recv(reply_header) %d", (int)WSAGetLastError());
      closesocket(socket_fd);
      return false;
   }

   receive_buffers_size = 0;
   for(buffer_index=0; buffer_index < number_of_reply_buffers; buffer_index++)
   {
      receive_buffers_size += reply_buffers[buffer_index].iov_len;
   }

   reply_message_size = (receive_buffers_size < reply_header.reply_size)?
      (receive_buffers_size):(reply_header.reply_size);
   *reply_status = reply_header.reply_status;

   received_data_size = 0;
   buffer_index = 0;
   while(received_data_size < reply_message_size)
   {
      while(buffer_index < number_of_reply_buffers)
      {
         int packet_size;
         size_t buf_offset = 0;

         while(buf_offset < reply_buffers[buffer_index].iov_len)
         {
            packet_size = recv(
               socket_fd, 
               (char_t*)reply_buffers[buffer_index].iov_base + buf_offset,
               reply_buffers[buffer_index].iov_len - buf_offset, 
               0);
            if (packet_size < 0)
            {
               Tr_Fault_1("SAL_Sync_Send_V: recv(data) %d", (int)WSAGetLastError());
               closesocket(socket_fd);
               return false;
            }
            buf_offset += packet_size;
            received_data_size += packet_size;
            if (received_data_size >= reply_message_size)
            {
               break;
            }
         }
         buffer_index++;
      }
   }

   if (closesocket(socket_fd) != 0)
   {
      Tr_Fault_1("SAL_Sync_Send_V: closesocket %d", (int)WSAGetLastError());
      return false;
   }

   return true;
}


bool_t SAL_Sync_Send(
   SAL_Connection_T* connection,
   const void* message,
   size_t message_size,
   void*  reply,
   size_t reply_size,
   int32_t* reply_status)
{
   SAL_IOV_T message_iov[1];
   SAL_IOV_T reply_iov[1];

   message_iov[0].iov_base = (void*)message;
   message_iov[0].iov_len  = message_size;

   reply_iov[0].iov_base = (void*)reply;
   reply_iov[0].iov_len  = reply_size;

   return SAL_Sync_Send_V(
      connection, 
      message_iov, 
      sizeof(message_iov)/sizeof(message_iov[0]), 
      reply_iov, 
      sizeof(reply_iov)/sizeof(reply_iov[0]), 
      reply_status);
}


bool_t SAL_Sync_Send_VS(
   SAL_Connection_T* connection,
   const SAL_IOV_T* message_buffers,
   uint32_t number_of_buffers,
   void*  reply,
   size_t reply_size,
   int32_t*   reply_status)
{
   SAL_IOV_T reply_iov[1];
   reply_iov[0].iov_base = (void*)reply;
   reply_iov[0].iov_len  = reply_size;

   return SAL_Sync_Send_V(
      connection, 
      message_buffers, 
      number_of_buffers, 
      reply_iov, 
      sizeof(reply_iov)/sizeof(SAL_IOV_T), 
      reply_status);
}


bool_t SAL_Sync_Send_SV(
   SAL_Connection_T* connection,
   const void* message,
   size_t message_size,
   const SAL_IOV_T* reply_buffers,
   uint32_t number_of_reply_buffers,
   int32_t*   reply_status)
{
   SAL_IOV_T message_iov[1];
   message_iov[0].iov_base = (void*)message;
   message_iov[0].iov_len  = message_size;

   return SAL_Sync_Send_V(
      connection, 
      message_iov, 
      sizeof(message_iov)/sizeof(message_iov[0]), 
      reply_buffers, 
      number_of_reply_buffers, 
      reply_status);
}


bool_t SAL_Send_Pulse(
   SAL_Connection_T* connection,
   int8_t  code, 
   int32_t value)
{
   SOCKET socket_fd;
   SAL_Pulse_T pulse;
   Message_Send_Header_T msg_hdr;
   struct sockaddr_in server_address;
   struct linger li = { 1, 0 };
   char_t rdata = 0;

   PBC_Require(TCP_IP_Initialized, "TCP/IP not initialized");

   if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
   {
      Tr_Fault("SAL_Send_Pulse: socket()");
      return false;
   }

   if (setsockopt(socket_fd, SOL_SOCKET, SO_LINGER, (const char_t*)&li, sizeof(struct linger)) != 0 )
   {
      Tr_Fault_1("SAL_Sync_Send_V: setsockopt() %d", (int)WSAGetLastError());
      closesocket(socket_fd);
      return false;
   }

   memset(&server_address, 0, sizeof(struct sockaddr_in));

   server_address.sin_family           = AF_INET;
   server_address.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
   server_address.sin_port             = (u_short)*connection;

   if (connect(socket_fd, (struct sockaddr *)&server_address, 
      sizeof(struct sockaddr_in)) == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Send_Pulse: connect() %d", (int)WSAGetLastError());
      closesocket(socket_fd);
      return false;
   }

   msg_hdr.mesage_type = EPulse;
   msg_hdr.mesage_size = sizeof(SAL_Pulse_T);

   pulse.code  = code;
   pulse.value.sival_int = value;

   /** First send the message header
    */
   if (send(socket_fd, (char_t*)&msg_hdr, sizeof(Message_Send_Header_T), 0)
      == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Send_Pulse: send(message header) %d", (int)WSAGetLastError());
      closesocket(socket_fd);
      return false;
   }

   /** Next send the pulse data
    */
   if (send(socket_fd, (char_t*)&pulse, sizeof(SAL_Pulse_T), 0) 
      == SOCKET_ERROR)
   {
      Tr_Fault_1("SAL_Send_Pulse: send(pulse data) %d", (int)WSAGetLastError());
      closesocket(socket_fd);
      return false;
   }

   if ( recv ( socket_fd, &rdata, sizeof (rdata), 0 ) != sizeof (rdata) || rdata != PULSE_REPLY_DATA )
   {
      Tr_Fault_1("SAL_Send_Pulse: recv() %d", (int)WSAGetLastError());
      closesocket ( socket_fd );
      return false;
   }

   if (closesocket(socket_fd) != 0)
   {
      Tr_Fault_1("SAL_Send_Pulse: closesocket() %d", (int)WSAGetLastError());
      return false;
   }

   return true;
}

#endif /* XSAL */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 05-Jun-2012, Kirk Bailey: Fixed NULL dereference found by Klockwork.
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
