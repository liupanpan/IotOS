/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_run.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_I_Run() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_engine.h"
#include "xsal_i_thread.h"
#include "xsal_i_timer.h"
#include "xsal_i_publish.h"
#include "xsal_i_dt.h"
#include "xsal_i_connections.h"
#include "xsal_i_event_property.h"

#include <stdlib.h>
#include <string.h>

EM_FILENUM(XSAL_IPC_MODULE_ID, 3); /* Identify file for trace/assert purposes */

/** Union with all Router Thread commands.
 *  Size of union determines maximum size of the command.
 */
typedef union SAL_RT_Command_Buffer_Tag
{
   DT_Command_T dt_command;
} SAL_RT_Command_Buffer_T;

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

/** Functions SAL_Lock_Mutex and SAL_Wait_Semaphore are not defined
 *  in XSAL_LIGHT external interface but are used internally. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);

/* -----------------------------
 *  Exported Object Definitions
 * ----------------------------- */

/** Channel to wait for messages from application.
 */
SAL_Channel_T SAL_I_RT_Channel;

/* ----------------------------
 *  Local Function Prototypes
 * ---------------------------- */

static void Dump_Error_Message(const char_t* prefix, SAL_Message_Dropped_Reason_T err, SAL_Thread_Id_T tid);

static bool_t Process_Send(SAL_Receive_T* rec_id, SAL_Message_T* msg_header);

static void Process_Publish(SAL_Receive_T* rec_id, SAL_Message_T* msg_header);

static SAL_Int_T Process_RT_Command(SAL_Receive_T* rec_id, const SAL_Message_T* msg_header, void* reply_data);

/* -----------------------------
 *  Local function definitions
 * ----------------------------- */

static void Dump_Error_Message(const char_t* prefix, SAL_Message_Dropped_Reason_T err, SAL_Thread_Id_T tid)
{
   switch (err)
   {
      case SAL_No_Queue:
         Tr_Fault_2("%s: thread %d doesn't have a queue", prefix, tid);
         break;

      case SAL_Queue_Full:
         Tr_Fault_2("%s: thread's %d queue is full", prefix, tid);
         break;

      case SAL_No_Memory:
         Tr_Fault_1("%s: There is no free memory", prefix);
         break;

      case SAL_Too_Small_Buffer:
         Tr_Fault_2("%s: Too small buffer in thread's %d queue", prefix, tid);
         break;

      case SAL_Communication_Error:
         Tr_Fault_1("%s: Communication error", prefix);
         break;

      default:
         PBC_Failed("Unknown error type")
         ;
         break;
   }
}

static bool_t Process_Send(SAL_Receive_T* rec_id, SAL_Message_T* msg_header)
{
   bool_t status = false;
   SAL_Thread_Id_T receiver_tid = msg_header->receiver_thread_id;
   SAL_Message_Queue_T* q = &SAL_I_Thread_Table[receiver_tid].message_queue;
   SAL_Event_Id_T ev_id = msg_header->event_id;
   bool_t is_urgent =
      (bool_t) ((ev_id > SAL_FIRST_EVENT_ID) && (ev_id < SAL_LAST_EVENT_ID)) ? (SAL_I_Is_Urgent(receiver_tid, ev_id)) : false;

   if (SAL_Lock_Mutex(&q->queue_lock))
   {
      SAL_Message_Dropped_Reason_T err;
      SAL_Message_Queue_Node_T* msg_node = SAL_I_Get_Free_Node(q, is_urgent, msg_header->data_size, &err);

      if (msg_node != NULL)
      {
         /** Save pointer to data in msg_header and next 
          *  copy the whole msg_header to msg_node
          */
         msg_header->data = msg_node->message.data;
         msg_node->message = *msg_header;
         if (msg_header->data_size > 0u)
         {
            size_t receive_data_len;
            if (SAL_Sync_Read(rec_id, msg_node->message.data, msg_header->data_size, &receive_data_len))
            {
               if (msg_header->data_size == receive_data_len)
               {
                  SAL_I_Post_Message_Node(msg_node, is_urgent);
                  status = true;
               }
               else
               {
                  SAL_I_Free_Node(msg_node);
                  err = SAL_Communication_Error;
                  Tr_Fault("Process_Send: Invalid message size");
               }
            }
            else
            {
               SAL_I_Free_Node(msg_node);
               err = SAL_Communication_Error;
               Tr_Fault("Process_Send: SAL_Sync_Read error");
            }
         }
         else
         {
            SAL_I_Post_Message_Node(msg_node, is_urgent);
            status = true;
         }
      }
      (void) SAL_Unlock_Mutex(&q->queue_lock);

      if (!status)
      {
         Dump_Error_Message("Process_Send", err, receiver_tid);
         if (SAL_I_Message_Dropped_Callback != NULL)
         {
            SAL_I_Message_Dropped_Callback(receiver_tid, err, msg_header);
         }
      }
   }
   else
   {
      PBC_Failed("Failed to lock queue's mutex");
   }
   return status;
}

static void Process_Publish(SAL_Receive_T* rec_id, SAL_Message_T* msg_header)
{
   bool_t status = false;
   SAL_Thread_Id_T last_tid;
   SAL_Message_Queue_T* last_queue = NULL;
   SAL_Message_Queue_Node_T* last_msg_node = NULL;
   bool_t is_last_msg_urgent = false;
   SAL_Event_Id_T ev_id = msg_header->event_id;

   /** First finds the last thread (with the highest thread ID) which 
    *  subscribes for the event. Allocates message node entry from their
    *  queue and read message from remote application to allocated buffer.
    *  Next duplicate this message to queues of the all other subscribed
    *  threads.
    */
   for (last_tid = (SAL_Thread_Id_T) SAL_I_Max_Number_Of_Threads; last_tid > 0; last_tid--)
   {
      if (SAL_I_Is_Subscribed(last_tid, ev_id))
      {
         /** Thread is subscribed for event
          */
         last_queue = &SAL_I_Thread_Table[last_tid].message_queue;
         if (SAL_Lock_Mutex(&last_queue->queue_lock))
         {
            SAL_Message_Dropped_Reason_T err;
            is_last_msg_urgent = SAL_I_Is_Urgent(last_tid, ev_id);
            last_msg_node = SAL_I_Get_Free_Node(last_queue, is_last_msg_urgent, msg_header->data_size, &err);
            if (last_msg_node != NULL)
            {
               size_t data_size = msg_header->data_size;

               /** Save pointer to data in msg_header and next 
                *  copy the whole msg_header to last_msg_node
                */
               msg_header->data = last_msg_node->message.data;
               last_msg_node->message = *msg_header;
               if (msg_header->data_size > 0u)
               {
                  size_t rec_data_len;
                  if (SAL_Sync_Read(rec_id, last_msg_node->message.data, data_size, &rec_data_len))
                  {
                     if (data_size == rec_data_len)
                     {
                        status = true;
                     }
                     else
                     {
                        last_msg_node->message.data = NULL;
                        err = SAL_Communication_Error;
                        Dump_Error_Message("Process_Publish", err, last_tid);
                        if (SAL_I_Message_Dropped_Callback != NULL)
                        {
                           SAL_I_Message_Dropped_Callback(last_tid, err, msg_header);
                        }
                        SAL_I_Free_Node(last_msg_node);
                     }
                  }
                  else
                  {
                     last_msg_node->message.data = NULL;
                     err = SAL_Communication_Error;
                     Dump_Error_Message("Process_Publish", err, last_tid);
                     if (SAL_I_Message_Dropped_Callback != NULL)
                     {
                        SAL_I_Message_Dropped_Callback(last_tid, err, msg_header);
                     }
                     SAL_I_Free_Node(last_msg_node);
                  }
               }
               else
               {
                  status = true;
               }
               if (!status)
               {
                  Tr_Fault("Read error. Message was dropped");
                  (void) SAL_Unlock_Mutex(&last_queue->queue_lock);
               }
               break;
            }
            else
            {
               msg_header->data = NULL;
               Dump_Error_Message("Process_Publish", err, last_tid);
               if (SAL_I_Message_Dropped_Callback != NULL)
               {
                  SAL_I_Message_Dropped_Callback(last_tid, err, msg_header);
               }
            }
            (void) SAL_Unlock_Mutex(&last_queue->queue_lock);
         }
         else
         {
            PBC_Failed("Failed to lock queue's mutex");
         }
      }
   }

   if (status)
   {
      SAL_Thread_Id_T tid;

      /** Duplicate message to other message queues
       */
      for (tid = 1; tid < last_tid; tid++)
      {
         if (SAL_I_Is_Subscribed(tid, ev_id))
         {
            SAL_Message_Dropped_Reason_T err;
            bool_t is_urgent = SAL_I_Is_Urgent(tid, ev_id);

            status = SAL_I_Post_Message(&SAL_I_Thread_Table[tid].message_queue, &last_msg_node->message, is_urgent, &err);

            if (!status)
            {
               Dump_Error_Message("Process_Publish", err, tid);
               if (SAL_I_Message_Dropped_Callback != NULL)
               {
                  SAL_I_Message_Dropped_Callback(tid, err, &last_msg_node->message);
               }
            }
         }
      }

      /** Now append message to the last queue
       */
      SAL_I_Post_Message_Node(last_msg_node, is_last_msg_urgent);
      (void) SAL_Unlock_Mutex(&last_queue->queue_lock);
   }
}

/** 
 *  Function is called when a message with event_id < 0 has been received 
 *  by the Router Thread.
 *  If commands returns any data then they are copied to buffer pointed 
 *  by the reply_data parameter and size of data is returned.
 */
static SAL_Int_T Process_RT_Command(SAL_Receive_T* rec_id, const SAL_Message_T* msg_header, void* reply_data)
{
   SAL_Int_T reply_data_length = -1;
   size_t received_data_length;
   SAL_RT_Command_Buffer_T rt_command;

   if (msg_header->data_size <= sizeof(SAL_RT_Command_Buffer_T))
   {
      if (SAL_Sync_Read(rec_id, &rt_command, msg_header->data_size, &received_data_length))
      {
         if ((int) msg_header->event_id == (int) EV_RT_DT_Command)
         {
            reply_data_length = SAL_I_Proccess_DT_Command(&rt_command.dt_command, reply_data);
         }
         else
         {
            Tr_Fault_1("Invalid RouterThread command %d", (int )msg_header->event_id);
         }
      }
   }
   else
   {
      Tr_Fault("RT Message data is too large");
   }

   return reply_data_length;
}

bool_t SAL_I_Before_Run(void)
{
   bool_t status = false;

   if (SAL_Create_Channel(&SAL_I_RT_Channel))
   {
      /** Make connection to the own RT Channel
       */
      if (SAL_Create_Connection(&SAL_I_RT_Channel, &SAL_I_Connection_Id[SAL_I_App_Id]))
      {
         if (SAL_Bind(SAL_I_App_Id, SAL_ROUTER_THREAD_ID, &SAL_I_RT_Channel))
         {
            SAL_I_Start_Timer_Module();
            status = true;
         }
         else
         {
            Tr_Fault("SAL_I_Before_Run/SAL_Bind() failed");
         }

         if (!status)
         {
            (void) SAL_Disconnect(&SAL_I_Connection_Id[SAL_I_App_Id]);
         }
      }
      else
      {
         Tr_Fault("SAL_I_Before_Run/SAL_Create_Connection() failed");
      }

      if (!status)
      {
         (void) SAL_Destroy_Channel(&SAL_I_RT_Channel);
      }
   }
   else
   {
      Tr_Fault("SAL_I_Before_Run/SAL_Create_Channel() failed");
   }

   return status;
}

/** Main loop of the XSAL application
 */
int32_t SAL_I_Run(void)
{
   int32_t exit_status;

   SAL_Signal_Ready();

#if defined (SAL_ROUTER_THREAD_DEFAULT_PRIORITY)
   /*
    * Adjust the initial thread priority for router operations
    */
   (void) SAL_I_Set_Thread_Priority(SAL_ROUTER_THREAD_ID, SAL_ROUTER_THREAD_DEFAULT_PRIORITY);
#endif /* SAL_ROUTER_THREAD_DEFAULT_PRIORITY */

#if defined(QNX_NTO)
/**
 * Set Router thread name in QNX
 */
   {
      char new_name[SAL_THREAD_NAME_MAX_LENGTH + 8];

      snprintf(new_name, sizeof(new_name), "%02d.%d-XSAL-RT", (int)SAL_Get_App_Id(), SAL_ROUTER_THREAD_ID);
      pthread_setname_np(0, new_name);
   }
#endif
   /** Message loop:
    *  Wait for message or pulse
    *   o) Pulse has been received: it is a timer event (QNX) or 
    *      Router Thread command
    *   o) Message with destination thread id = 0: it is published message
    *      or router thread command (when event_id < 0)
    *   o) Message with destination thread > 0: it is a message to given 
    *      thread
    */

   while (1)
   {
      bool_t status;
      uint8_t is_pulse;
      size_t message_size;
      SAL_Int_T reply_data_len;
      SAL_Message_T msg_header;
      SAL_Receive_T receiver_id;
      DT_Reply_Data_T reply_data = { { 0 } };
      SAL_Pulse_T pulse;

      reply_data_len = 0;

      status = SAL_Sync_Receive(&SAL_I_RT_Channel, &msg_header, sizeof(SAL_Message_T), &message_size, &is_pulse, &receiver_id);

      if (status)
      {
         if (is_pulse)
         {
            /** Pulse code specifies destination thread.
             *  Pulse value depends on pulse code. For pulse code = 0, pulse 
             *  value defines Router Thread command. Code value > 0 defines 
             *  timer event (code specifies thread id) and pulse 
             *  value defines event id.
             */
            int8_t pulse_code;
            int32_t pulse_value;
            memcpy(&pulse, &msg_header, sizeof(pulse));
            pulse_code = pulse.code;
            pulse_value = pulse.value.sival_int;

#ifdef XSAL_ENABLE_MSG_TRACE
            Tr_Info_Lo_2("SAL_Run: received pulse: %d, %d", (int)pulse_code, (int)pulse_value);
#endif /* XSAL_ENABLE_MSG_TRACE */
            /** Is it a pulse to the router thread?
             */
            if (pulse_code == SAL_ROUTER_THREAD_ID)
            {
               if (((uint32_t) pulse_value & 0xffu) == SAL_PULSE_RT_EXIT)
               {
                  exit_status = pulse_value / 256;
#if defined (LINUX)
                  close(receiver_id);
#elif defined (WIN32)
                  closesocket(receiver_id.socket_fd);
#endif
                  /** It is a pulse. Do not replay to the sender
                   */
                  break;
               }
            }
#if defined (QNX_NTO)
            else if (pulse_code > 0)
            {
               /** It is a pulse from timer.
                *  pulse_code value defines a thread id
                *  pulse_value points to expired Timer structure
                */
               SAL_I_Post_Timer_Event(
                  ((SAL_I_Timer_T*)pulse_value)->event_id, pulse_code,
                  ((SAL_I_Timer_T*)pulse_value)->use_param, ((SAL_I_Timer_T*)pulse_value)->param);

               /** It is a pulse. Do not replay to the sender
                */
               continue;
            }
#endif  /* QNX_NTO */
         }
         else /* A message has been received */
         {
            int32_t ev_id = msg_header.event_id;
            SAL_Thread_Id_T receiver_tid = msg_header.receiver_thread_id;
#ifdef XSAL_ENABLE_MSG_TRACE
            Tr_Info_Lo_4("SAL_Run: received message: from(%d, %d) to (%d, %d)",
               (int)msg_header.sender_app_id,
               (int)msg_header.sender_thread_id,
               (int)SAL_I_App_Id,
               (int)msg_header.receiver_thread_id);
            Tr_Info_Lo_3("EV %d OBJ %d, message size %d",
               (int)ev_id, (int)msg_header.object_id, (int)msg_header.data_size);
#endif /* XSAL_ENABLE_MSG_TRACE */
            /** Is it a message to the Router Thread?
             */
            if (receiver_tid == SAL_ROUTER_THREAD_ID)
            {
               if ((ev_id >= SAL_FIRST_EVENT_ID) && (ev_id < SAL_LAST_EVENT_ID))
               {
                  /** Broadcast message to all local subscribers
                   */
                  Process_Publish(&receiver_id, &msg_header);
               }
               else if ((ev_id < 0) && (ev_id > EV_RT_Last_Command))
               {
                  reply_data_len = Process_RT_Command(&receiver_id, &msg_header, &reply_data);
                  if (reply_data_len < 0)
                  {
                     reply_data_len = 0;
                     status = false;
                  }
               }
               else
               {
                  Tr_Fault_1("Received unknown event: %d", (int )ev_id);
                  status = false;
               }
            }
            else /* It is a message to the specific thread. */
            {
               if ((receiver_tid > SAL_ROUTER_THREAD_ID) && (receiver_tid <= (SAL_Thread_Id_T) SAL_I_Max_Number_Of_Threads))
               {
                  status = Process_Send(&receiver_id, &msg_header);
               }
               else
               {
                  Tr_Fault_1("Received event with invalid receiver thread Id : %d", (int )receiver_tid);
                  status = false;
               }
            }
         }
      }
      else
      {
         Tr_Fault("ERROR SAL_Receive");
      }

      (void) SAL_Sync_Reply(&receiver_id, (int32_t) status, (void*) &reply_data, (size_t) reply_data_len);
   } /* while (1) */

   return exit_status;
}

void SAL_I_After_Run(void)
{
   SAL_I_Stop_Timer_Module();
   SAL_NS_Unsubscribe_App(SAL_I_App_Id);
   (void) SAL_NS_Unbind_App(SAL_I_App_Id);
   (void) SAL_Disconnect(&SAL_I_Connection_Id[SAL_I_App_Id]);
   (void) SAL_Destroy_Channel(&SAL_I_RT_Channel);

   /** XSAL Engine is not working anymore
    */
   (void) SAL_Wait_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_ready_sem);
   SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].is_ready = false;

   /** "Unlock" threads calling SAL_Wait_Destroyed();
    */
   (void) SAL_Signal_Semaphore(&SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID].thread_destroyed_sem);
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 06 Aug 2012 Dan Carman
 *    - Change Linux to use Pulses for timer notification
 *
 * - 26-Sep-2013 Larry Piekarski Rev 19
 *   - Task 1334: Fixed compiler warnings
 *
 * 20May14  David Origer
 * - Task kok_inf#7893 : Fix Klocwork issues.
 *   - Remove changes for "no reply" messaging made under task
 *     kok_basa#115827.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the
 *    enumerated type of SAL_Event_Id_T for compatibility with ISA code.
 *
 * 18-Aug-2015 Kirk Bailey
 *    - Fixed Coverity defect: receive_id resource lost.
 *
 * 28-Jan-2016 Dan Carman
 *     - Task kok_basa#236038 Add thread name to XSAL Router Thread 
 *           Provides better identification of thread during system analysis
 * -----------------------------------------------------------------------*/
