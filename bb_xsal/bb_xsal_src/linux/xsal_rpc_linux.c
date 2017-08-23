/* -------------------------------------------------------------------------
 *
 * File:        xsal_rpc_linux_c.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *
 * Description: Implementation of the synchronous communication on
 *              Linux platform
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_rpc_linux.h"

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

EM_FILENUM(XSAL_IPC_MODULE_ID, 5); /* Identify file for trace/assert purposes */

/* ---------------------------------------
 *  Local Preprocessor #define Constants
 * --------------------------------------- */

#define SAL_LISTEN_QUEUE 5

/* ---------------------------------------
 *  Local types
 * --------------------------------------- */

typedef struct Pulse_IPC_Tag
{
   Message_Send_Header_T header;
   SAL_Pulse_T pulse;
} Pulse_IPC_T;

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

static void Format_Address(char_t* output, size_t output_sz, pid_t app_pid, SAL_Int_T socket_id, uid_t user_id);

static SAL_Int_T Unlink(const SAL_Channel_T* channel);

static size_t GetIOVSize(const SAL_IOV_T* buffers, size_t num_of_buffers);

static SAL_Int_T Read_Data(SAL_Int_T s, uint8_t buff[], size_t len);

/* ----------------------------
 *  Local Function Definitions
 * ---------------------------- */

static void Format_Address(char_t* output, size_t output_sz, pid_t app_pid, SAL_Int_T socket_id, uid_t user_id)
{
   size_t num_written = 0;
   int ret;

   ret = snprintf(output, output_sz, "/tmp/sal_%d_%d_%d", (int) app_pid, (int) socket_id, (int) user_id);

   if (ret >= 0)
   {
      /* Get number of bytes written if snprintf did not return an error */
      num_written = ret;
   }

   if ((ret < 0) || (num_written > output_sz))
   {
      Tr_Fault_4("Failed to to create path name (/tmp/sal_%d_%d_%d). result = %zu.", (int )app_pid, (int )socket_id, (int )user_id,
         num_written);
   }
}

static SAL_Int_T Unlink(const SAL_Channel_T* channel)
{
   char_t path[PATH_MAX];

   PBC_Require(channel != NULL, "NULL channel");

   Format_Address(path, PATH_MAX, channel->app_pid, channel->socket_d, channel->user_id);
   return unlink(path);
}

static size_t GetIOVSize(const SAL_IOV_T* buffers, size_t num_of_buffers)
{
   size_t i = 0;
   size_t iov_size = 0;

   for (; i < num_of_buffers; ++i)
   {
      iov_size += buffers[i].iov_len;
   }

   return iov_size;
}

static SAL_Int_T Read_Data(SAL_Int_T s, uint8_t buff[], size_t len)
{
   bool_t reading = true;
   SAL_Int_T offset = 0;

   PBC_Require(buff != NULL, "NULL buffer");

   do
   {
      SAL_Int_T rec_data = read(s, &buff[offset], (size_t) ((SAL_Int_T) len - offset));
      if (rec_data > 0)
      {
         offset += rec_data;
         if (offset == (SAL_Int_T) len)
         {
            reading = false;
         }
      }
      else
      {
         reading = false;
         if (rec_data < 0)
         {
            offset = -1;
         }
      }
   } while (reading);

   return offset;
}

/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

bool_t SAL_Create_Channel(SAL_Channel_T* channel)
{
   bool_t status = false;
   struct sockaddr_un address;

   PBC_Require(channel != NULL, "NULL channel");

   channel->app_pid = getpid();
   channel->user_id = getuid();
   channel->socket_d = socket(AF_UNIX, SOCK_STREAM, 0);

   address.sun_family = AF_UNIX;
   Format_Address(address.sun_path, sizeof(address.sun_path), channel->app_pid, channel->socket_d, channel->user_id);

   if (channel->socket_d >= 0)
   {
      (void) Unlink(channel);

      if (bind(channel->socket_d, (struct sockaddr*) (void*) &address, sizeof(address)) == 0)
      {
         if (listen(channel->socket_d, SAL_LISTEN_QUEUE) == 0)
         {
            status = true;
         }
         else
         {
            (void) Unlink(channel);
         }
      }
   }

   if (!status)
   {
      Tr_Fault_2("SAL_Create_Channel(%s): %s", address.sun_path, strerror(errno));
   }

   return status;
}

bool_t SAL_Destroy_Channel(SAL_Channel_T* channel)
{
   bool_t status;
   PBC_Require(channel != NULL, "NULL channel");

   status = (bool_t) (close(channel->socket_d) == 0);
   if (status)
   {
      status = (bool_t) (Unlink(channel) == 0);
   }

   if (!status)
   {
      Tr_Fault_1("SAL_Destroy_Channel, Unlink(), %s", strerror(errno));
   }
   return status;
}

bool_t SAL_Create_Connection(const SAL_Channel_T* channel, SAL_Connection_T* connection)
{
   PBC_Require(channel != NULL, "NULL channel");
   PBC_Require(connection != NULL, "NULL connection");

   connection->app_pid = channel->app_pid;
   connection->socket_d = channel->socket_d;
   connection->user_id = channel->user_id;

   return true;
}

#if defined (XSAL)

bool_t SAL_Bind(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, const SAL_Channel_T* channel)
{
   bool_t status;
   PBC_Require(channel != NULL, "NULL channel");

   status = SAL_NS_Bind_Channel(app_id, port_id, channel);
   if (!status)
   {
      Tr_Fault_2("SAL_Bind failed: app_id=%d port_id=%d", (int )app_id, (int )port_id);
   }

   return status;
}

bool_t SAL_Unbind(const SAL_Channel_T* channel)
{
   bool_t status;
   PBC_Require(channel != NULL, "NULL channel");

   status = SAL_NS_Unbind_Channel(channel);
   if (!status)
   {
      Tr_Fault("SAL_Unbind, SAL_NS_Unbind_Channel() failed");
   }

   return status;
}

bool_t SAL_Connect(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, SAL_Connection_T* connection)
{
   bool_t status;
   SAL_Channel_T channel;

   PBC_Require(app_id > 0, "Invalid app id");
   PBC_Require(port_id >= 0, "Negative port id");
   PBC_Require(connection != NULL, "NULL connection");

   if (SAL_NS_Resolve_Channel(app_id, port_id, &channel))
   {
      status = SAL_Create_Connection(&channel, connection);
   }
   else
   {
      status = false;
   }
   if (!status)
   {
      Tr_Fault("SAL_Connect, SAL_NS_Resolve_Channel() failed");
   }

   return status;
}

#endif /* defined (XSAL) */

bool_t SAL_Disconnect(SAL_Connection_T* connection)
{
   PBC_Require(connection != NULL, "NULL connection");

   return true;
}

bool_t SAL_Send_Pulse(SAL_Connection_T* connection, int8_t code, int32_t value)
{
   bool_t status = false;
   SAL_Int_T s;

   PBC_Require(connection != NULL, "NULL connection");

   s = socket(AF_UNIX, SOCK_STREAM, 0);
   if (s >= 0)
   {
      struct sockaddr_un address;

      address.sun_family = AF_UNIX;
      Format_Address(address.sun_path, sizeof(address.sun_path), connection->app_pid, connection->socket_d, connection->user_id);

      if (connect(s, (struct sockaddr*) (void*) &address, sizeof(address)) >= 0)
      {
         Pulse_IPC_T msg = { { 0 }, { 0 } };

         msg.header.message_type = Pulse;
         msg.header.message_size = sizeof(SAL_Pulse_T);
         msg.pulse.code = code;
         msg.pulse.value.sival_int = value;

         if (write(s, &msg, sizeof(msg)) == (SAL_Int_T) sizeof(msg))
         {
            status = true;
         }
         else
         {
            Tr_Fault_1("SAL_Send_Pulse, write(), %s", strerror(errno));
         }
      }
      else
      {
         Tr_Fault_1("SAL_Send_Pulse, connect(), %s", strerror(errno));
      }
      (void) close(s);
   }
   else
   {
      Tr_Fault_1("SAL_Send_Pulse, socket(), %s", strerror(errno));
   }

   return status;
}

bool_t SAL_Sync_Send_V(SAL_Connection_T* connection, const SAL_IOV_T* message_buffers, uint32_t number_of_buffers,
   const SAL_IOV_T* reply_buffers, uint32_t number_of_reply_buffers, int32_t* reply_status)
{
   SAL_Int_T s;
   bool_t status = false;

   PBC_Require(connection != NULL, "NULL connection");

   s = socket(AF_UNIX, SOCK_STREAM, 0);
   if (s >= 0)
   {
      SAL_IOV_T msg[number_of_buffers + 1];
      struct sockaddr_un address;
      address.sun_family = AF_UNIX;
      Format_Address(address.sun_path, sizeof(address.sun_path), connection->app_pid, connection->socket_d, connection->user_id);

      if (connect(s, (struct sockaddr*) (void*) &address, sizeof(address)) >= 0)
      {
         Message_Send_Header_T header = { 0 };

         header.message_type = Message;
         header.message_size = GetIOVSize(message_buffers, number_of_buffers);

         msg[0].iov_base = &header;
         msg[0].iov_len = sizeof(header);

         if (number_of_buffers > 0)
         {
            memcpy(&msg[1], message_buffers, number_of_buffers * sizeof(SAL_IOV_T));
         }

         if (writev(s, msg, (SAL_Int_T) number_of_buffers + 1) == (SAL_Int_T) (header.message_size + sizeof(header)))
         {
            Message_Reply_Header_T reply_header;
            SAL_IOV_T reply[number_of_reply_buffers + 1];

            reply[0].iov_base = &reply_header;
            reply[0].iov_len = sizeof(reply_header);

            if (number_of_reply_buffers > 0)
            {
               memcpy(&reply[1], reply_buffers, number_of_reply_buffers * sizeof(SAL_IOV_T));
            }

            if ((size_t) readv(s, reply, (SAL_Int_T) (number_of_reply_buffers + 1)) >= sizeof(reply_header))
            {
               *reply_status = reply_header.reply_status;
               status = true;
            }
            else
            {
               Tr_Fault_1("SAL_Sync_Send_V, readv(), %s", strerror(errno));
            }
         }
         else
         {
            Tr_Fault_1("SAL_Sync_Send_V, writev(), %s", strerror(errno));
         }
      }
      else
      {
         Tr_Fault_1("SAL_Sync_Send_V, connect(), %s", strerror(errno));
      }
      (void) close(s);
   }
   else
   {
      Tr_Fault_1("SAL_Sync_Send_V, socket(), %s", strerror(errno));
   }

   return status;
}

bool_t SAL_Sync_Send(SAL_Connection_T* connection, const void* message, size_t message_size, void* reply, size_t reply_size,
   int32_t* reply_status)
{
   SAL_IOV_T message_iov[1] = { { 0 } };
   SAL_IOV_T reply_iov[1] =   { { 0 } };

   PBC_Require(connection != NULL, "NULL connection");
   PBC_Require(message != NULL, "NULL message");

   message_iov[0].iov_base = (void*) message;
   message_iov[0].iov_len = message_size;

   reply_iov[0].iov_base = reply;
   reply_iov[0].iov_len = reply_size;

   return SAL_Sync_Send_V(connection, message_iov, 1, reply_iov, 1, reply_status);
}

bool_t SAL_Sync_Send_VS(SAL_Connection_T* connection, const SAL_IOV_T* message_buffers, uint32_t number_of_buffers, void* reply,
   size_t reply_size, int32_t* reply_status)
{
   SAL_IOV_T reply_iov[1] = { { 0 } };

   PBC_Require(connection != NULL, "NULL connection");
   PBC_Require(message_buffers != NULL, "NULL message buffer");

   reply_iov[0].iov_base = reply;
   reply_iov[0].iov_len = reply_size;

   return SAL_Sync_Send_V(connection, message_buffers, number_of_buffers, reply_iov, 1, reply_status);
}

bool_t SAL_Sync_Send_SV(SAL_Connection_T* connection, const void* message, size_t message_size, const SAL_IOV_T* reply_buffers,
   uint32_t number_of_reply_buffers, int32_t* reply_status)
{
   SAL_IOV_T message_iov[1];

   PBC_Require(connection != NULL, "NULL connection");
   PBC_Require(message != NULL, "NULL message");
   PBC_Require(reply_buffers != NULL, "NULL reply buffer");

   message_iov[0].iov_base = (void*) message;
   message_iov[0].iov_len = message_size;

   return SAL_Sync_Send_V(connection, message_iov, 1, reply_buffers, number_of_reply_buffers, reply_status);
}

bool_t SAL_Sync_Receive(SAL_Channel_T* channel, void* buffer, size_t buffer_size, size_t* message_size, uint8_t* is_pulse,
   SAL_Receive_T* receive_id)
{
   bool_t status = false;
   Message_Send_Header_T header;

   PBC_Require(channel != NULL, "NULL channel");
   PBC_Require(buffer != NULL, "NULL buffer");
   PBC_Require(message_size != NULL, "NULL message size");
   PBC_Require(is_pulse != NULL, "NULL is_pulse ptr");
   PBC_Require(receive_id != NULL, "NULL receive id");

   *receive_id = accept(channel->socket_d, 0, 0);
   if (*receive_id != -1)
   {
      if (Read_Data(*receive_id, (uint8_t*) (void*) &header, sizeof(header)) == (SAL_Int_T) sizeof(header))
      {
         size_t data_size;
         *is_pulse = (uint8_t) (header.message_type == Pulse);
         *message_size = header.message_size;

         data_size = (header.message_size < buffer_size) ? header.message_size : buffer_size;
         if (Read_Data(*receive_id, buffer, data_size) >= 0)
         {
            status = true;
         }
      }

      if (!status || *is_pulse)
      {
         (void) close(*receive_id);
      }

      if (!status)
      {
         Tr_Fault("SAL_Sync_Receive");
      }
   }
   else
   {
      Tr_Fault_1("SAL_Sync_Receive, accept(), %s", strerror(errno));
   }

   return status;
}

bool_t SAL_Sync_Read(SAL_Receive_T* receive_id, void* buffer, size_t buffer_size, size_t* received_message_size)
{
   SAL_Int_T rec_data = Read_Data(*receive_id, buffer, buffer_size);
   if (rec_data >= 0)
   {
      *received_message_size = (size_t) rec_data;
   }

   if (rec_data < 0)
   {
      Tr_Fault_1("SAL_Sync_Read, Read_Data(), %s", strerror(errno));
   }

   return (bool_t) (rec_data >= 0);
}

bool_t SAL_Sync_Reply_V(const SAL_Receive_T* receive_id, int32_t reply_status, const SAL_IOV_T* reply_buffers,
   uint32_t number_of_buffers)
{
   bool_t status = false;
   Message_Reply_Header_T header = { 0 };
   SAL_IOV_T reply[number_of_buffers + 1];

   PBC_Require(receive_id, "NULL receive_id");
   PBC_Require(((reply_buffers != NULL) && (number_of_buffers > 0)) || ((reply_buffers == NULL) && (number_of_buffers == 0)),
      "Invalid reply buffer data");

   header.reply_size = GetIOVSize(reply_buffers, number_of_buffers);
   header.reply_status = reply_status;

   reply[0].iov_base = &header;
   reply[0].iov_len = sizeof(header);

   if (number_of_buffers > 0)
   {
      memcpy(&reply[1], reply_buffers, number_of_buffers * sizeof(SAL_IOV_T));
   }

   /* Write header & reply data */
   if (writev(*receive_id, reply, (SAL_Int_T) (number_of_buffers + 1)) == (SAL_Int_T) (header.reply_size + sizeof(header)))
   {
      status = true;
   }
   else
   {
      Tr_Fault_1("SAL_Sync_Reply_V, writev(), %s", strerror(errno));
   }
   (void) close(*receive_id);

   return status;
}

bool_t SAL_Sync_Reply(const SAL_Receive_T* receive_id, int32_t reply_status, const void* reply_message, size_t reply_message_size)
{
   bool_t is_ok = false;

   if (*receive_id >= 0)
   {
      SAL_IOV_T reply_buf[1];

      PBC_Require(receive_id != NULL, "NULL receive id");
      PBC_Require((reply_message != NULL) || ((reply_message == NULL) && (reply_message_size == 0)), "Invalid reply message data");

      reply_buf[0].iov_base = (void*) reply_message;
      reply_buf[0].iov_len = reply_message_size;

      is_ok = SAL_Sync_Reply_V(receive_id, reply_status, reply_buf, 1);
   }

   return is_ok;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * 22-Aug-2012 Dan Carman
 *    Removed separate read / write of IPC header and combined with iov read / write of message data
 *    Yields ~10% reduction in IPC time (for short messages).
 *
 * 15May13  David Origer
 * SCR kok_basa#28775 : XSAL is reporting confisuing warnings.
 *    Modified code as needed so that the correct errno value will be
 *    reported when there is a problem.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 19
 *   - Task 1334: Fixed compiler warnings
 *
 * 20May14  David Origer
 * - Task kok_inf#7893 : Fix Klocwork issues.
 *   - Remove changes for "no reply" messaging made under task
 *     kok_basa#115827.
 *
 * 16-Mar-2015 Dan Carman
 * + Task 21696 Add z format specificer for size_t which maybe different from native int
 *
 * 31Mar15  David Origer
 * - Task kok_inf#22780 : Fix build warning.  Add z format for size_t, similar to
 *   previous change.
 *
 * 12Jun15  David Origer
 * - Task kok_basa#213204 : Fix Valgrind issues.
 *   - Make sure variables are completely initialized.
  *
 * - 18-Aug-2015 Kirk Bailey
 *   - Made sure that a reply is only made if the file id is still valid.
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
