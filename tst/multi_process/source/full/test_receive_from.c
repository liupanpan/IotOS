/* -------------------------------------------------------------------------
 *
 * File:        test_receive_from.c
 *
 * Copyright:   2006-2014 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of receive from.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 15);

#define APP_ID 2

#define MAX_NUMB_OF_THREADS 4

typedef enum Thread_Id_Tag
{
   SENDER_THREAD = 1, RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2, QUEUE_THREAD
} Thread_Id_T;

typedef enum Event_Id_Tag
{
   EV_E1 = 1, EV_E2, EV_E3, EV_E4
} Event_Id_T;

typedef struct Sample_Msg_Tag
{
   SAL_Int_T field1;
   SAL_Int_T field2;
} Sample_Msg_T;

SAL_Mutex_T Mutex;

SAL_Int_T Long_Timeout = 3000;

/** 
 *   \file
 *
 *   Test of receiving messages from the queues of another thread (SAL_Receive_From and 
 *   SAL_Receive_From_Timeout functions).
 *
 *   \section T Tests:
 *
 *   First group:
 *
 *   <OL>
 *   <LI> \ref RFT12 - tests of SAL_Receive_From and SAL_Receive_From_Timeout functions, long
 *                     timeout (<B><I>the first Receive_From_Test for XSAL only</I></B>). </LI>
 *   <LI> \ref RFT3 - test of SAL_Receive_From_Timeout, short timeout. </LI>
 *   </OL>
 *
 *   Second group:
 *
 *   <OL>
 *   <LI> \ref RFT45 - tests of SAL_Receive_From and SAL_Receive_From_Timeout functions, long
 *                     timeout (<B><I>the first Receive_From_Test for XSAL only</I></B>). </LI>
 *   </OL>
 *
 *   \section RFT12 the first and the second Receive_From_Test
 *
 *   Creates four threads:
 *   - QUEUE_THREAD holds the message queue
 *   - SENDER_THREAD sends the messages to the QUEUE_THREAD
 *   - RECEIVE_FROM_THREAD_1 - 
 *   - RECEIVE_FROM_THREAD_2 - both receive messages from the queue of QUEUE_THREAD
 *
 *   At first messages are sent to QUEUE_THREAD, then RECEIVE_FROM_THREAD_1 and 
 *   RECEIVE_FROM_THREAD_2 receives messages from the queue of QUEUE_THREAD.
 *
 *   \subsection TP Test procedure:
 *
 *   <OL>
 *   <LI> QUEUE_THREAD creates the queue. </LI>
 *   <LI> SENDER_THREAD sends six messages to the queue of QUEUE_THREAD. </LI>
 *   <LI> RECEIVE_FROM_THREAD_1 and RECEIVE_FROM_THREAD_2 receives messages from
 *      the queue of QUEUE_THREAD until the queue is empty. </LI>
 *   </OL>
 *
 *  \subsection FRFT the first Receive_From_Test
 *
 *  Messages are received by SAL_Receive_From function.
 *
 *  \subsection CRFT the second Receive_From_Test
 *
 *  Messages are received by SAL_Receive_From_Timeout function with timeout 3000 msec.
 *  Timeout is long therefore messages are received before timeout is reached.
 *
 *   \section RFT3 the third Receive_From_Test
 *
 *   Creates four threads:
 *   - QUEUE_THREAD holds the message queue
 *   - RECEIVE_FROM_THREAD_1 - 
 *   - RECEIVE_FROM_THREAD_2 - both receive messages from the queue of QUEUE_THREAD
 *   - SENDER_THREAD sends messages
 *   Timeout is short (100 ms).
 *   Timeout is reached earlier than messages are sent.
 *   
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> QUEUE_THREAD creates the queue. </LI>
 *   <LI> RECEIVE_FROM_THREAD_1 and RECEIVE_FROM_THREAD_2 try to receive messages from
 *        the queue of QUEUE_THREAD. </LI>
 *   <LI> SENDER_THREAD sends six messages to the queue of QUEUE_THREAD. </LI>
 *   </OL>
 *
 *   \section RFT45 the fourth and the fifth Receive_From_Test
 *
 *   Creates four threads:
 *   - QUEUE_THREAD holds the message queue
 *   - SENDER_THREAD sends the messages to the QUEUE_THREAD
 *   - RECEIVE_FROM_THREAD_1 - 
 *   - RECEIVE_FROM_THREAD_2 - both receive messages from the queue of QUEUE_THREAD
 *
 *   At first RECEIVE_FROM_THREADs wait for receiving messages,
 *   then SENDER_THREAD sends messages, and RECEIVE_FROM_THREADs receive messages.
 *
 *   \subsection TP2 Test procedure:
 *
 *   <OL>
 *   <LI> QUEUE_THREAD creates the queue. </LI>
 *   <LI> RECEIVE_FROM_THREAD_1 and RECEIVE_FROM_THREAD_2 try to receive messages from
 *        the queue of QUEUE_THREAD (they are waiting until messages arrive). </LI>
 *   <LI> SENDER_THREAD sends six messages to the queue of QUEUE_THREAD. </LI>
 *   <LI> RECEIVE_FROM_THREAD_1 and RECEIVE_FROM_THREAD_2 receive reamining messages from
 *        the queue of QUEUE_THREAD . </LI>
 *   </OL>
 *
 *  \subsection RFT4 the fourth Receive_From_Test 
 *
 *  In RECEIVE_FROM_THREAD_1 and RECEIVE_FROM_THREAD_2 messages are received by 
 *  SAL_Receive_From function.
 *
 *  \subsection RFT5 the fifth Receive_From_Test
 *
 *  In RECEIVE_FROM_THREAD_1 and RECEIVE_FROM_THREAD_2 messages are received by 
 *  SAL_Receive_From_Timeout function with timeout 3000 msec. Timeout is long therefore 
 *  messages are received before timeout is reached.
 */

void Sender_Thread(void* param);
void Queue_Thread(void* param);

const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, SAL_Thread_Id_T id, SAL_Priority_T prior, SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}

/** \name Functions of the first Receive_From_Test
 *
 *  \{
 */

/**  Function of RECEIVE_FROM_THREADs.
 *
 *   - takes out the messages from the queue of QUEUE_THREAD
 */

void Receive_From_Thread_1(void* param)
{
   const SAL_Message_T* msg = NULL;
   Sample_Msg_T* data;
   SAL_Stat_Queue_T stat;
   SAL_Int_T Status = 1;
   SAL_Thread_Id_T wait_list[] =
   { SENDER_THREAD };

   Tr_Info_Lo("Receive_From_Thread_1: Start");

   SAL_Signal_Ready();

   /* Wait until the SENDER_THREAD signals that it has sent messages */
   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

   do
   {
      /* lock mutex */
      SAL_Lock_Mutex(&Mutex);

      SAL_Stat_Queue(QUEUE_THREAD, &stat);

      /* receive message from QUEUE_THREAD */
      if (stat.message_count != 0)
      {
         msg = SAL_Receive_From(QUEUE_THREAD);

         if (msg == NULL)
         {
            PBC_Failed("SAL_Receive_From: Message is null");
         }
      }
      else
      {
         Status = 0;
      }

      /* unlock mutex */
      SAL_Unlock_Mutex(&Mutex);

      if (stat.message_count != 0)
      {
         data = (Sample_Msg_T*) msg->data;

         Tr_Info_Mid_2("Receive_From_Thread: field1: %d, field2: %d",
            (int)data->field1, (int)data->field2);
      }

   } while (Status);

   Tr_Info_Lo("Receive_From_Thread_1: Stop");
}

/**  Function of the QUEUE_THREAD.
 *
 *   - creates the queue
 *   - holds the messages in the queue
 */

void Queue_Thread_1(void* param)
{
   SAL_Thread_Id_T destroyed_list[] =
   { RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2 };

   Tr_Info_Lo("Queue_Thread_1: Start");

   SAL_Create_Queue(8, 0, malloc, free);

   /* Signal that the queue now exists */
   SAL_Signal_Ready();

   /* Wait until the receive threads are done with the queue before exiting */
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Queue_Thread_1: Stop");
}

/**  Function of SENDER_THREAD.
 *
 *   - sends the messages to QUEUE_THREAD
 */

void Sender_Thread_1(void* param)
{
   SAL_Thread_Id_T wait_list[] =
   { QUEUE_THREAD };
   SAL_Thread_Id_T destroyed_list[] =
   { QUEUE_THREAD };
   Sample_Msg_T data1, data2, data3;

   Tr_Info_Lo("Sender_Thread_1: Start");

   /* Wait for queue to be created */
   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

   /* set data to send */
   data1.field1 = 123;
   data1.field2 = 456;

   data2.field1 = 234;
   data2.field2 = 567;

   data3.field1 = 345;
   data3.field2 = 678;

   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));

   /* signals that every messages are in the queue */
   SAL_Signal_Ready();

   /*
    * Don't exit right away or receive threads may miss signal; instead, just wait
    * wait for queue thread to terminate after receive threads are done.
    */
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Sender_Thread_1: Stop");
}

/** \}
 */

/** \name Functions of the second Receive_From_Test
 *
 *  \{
 */

/**  Function of RECEIVE_FROM_THREADs.
 *
 *   - takes out the messages from the queue of QUEUE_THREAD
 */

void Receive_From_Thread_2(void* param)
{
   const SAL_Message_T* msg = NULL;
   Sample_Msg_T* data;
   SAL_Stat_Queue_T stat;
   SAL_Int_T Status = 1;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   SAL_Thread_Id_T wait_list[] = { SENDER_THREAD };

   Tr_Info_Lo("Receive_From_Thread_2: Start");

   SAL_Signal_Ready();

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   do
   {
      /* lock mutex */
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {
         PBC_Failed("Timeout has been reached");
      }

      SAL_Stat_Queue(QUEUE_THREAD, &stat);

      /* receive message from QUEUE_THREAD */
      if (stat.message_count != 0)
      {
         time_1 = SAL_Clock();
         msg = SAL_Receive_From_Timeout(QUEUE_THREAD, Long_Timeout);
         time_2 = SAL_Clock();
         delta_time = (SAL_Int_T)(time_2 - time_1);

         /* message has been received and event_id != last message (EV_E4) */
         if (NULL == msg)
         {
            PBC_Failed("SAL_Receive_From_Timeout: timeout has been reached");
         }
         else if (msg->event_id != (SAL_Event_Id_T) EV_E4)
         {
            if (delta_time > 3)
            {
               PBC_Failed_1("SAL_Receive_From_Timeout: suspends the thread, time: %d", (int) delta_time);
            }
         }
         else
         {
         }
      } /* stat.message_count != 0 */
      else
      {
         Status = 0;
      }

      /* unlock mutex */
      SAL_Unlock_Mutex(&Mutex);

      if ((msg != NULL) && (stat.message_count != 0))
      {
         data = (Sample_Msg_T*) msg->data;

         Tr_Info_Mid_2("Receive_From_Thread: field1: %d, field2: %d", (int) data->field1, (int) data->field2);
      }

   } while (Status);

   Tr_Info_Lo("Receive_From_Thread_2: Stop");
}

/**  Function of the QUEUE_THREAD.
 *
 *   - creates the queue
 *   - holds the messages in the queue
 */

void Queue_Thread_2(void* param)
{
   SAL_Thread_Id_T destroyed_list[] =
   { RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2 };

   Tr_Info_Lo("Queue_Thread_2: Start");

   SAL_Create_Queue(8, 0, malloc, free);

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   /* Don't exit until receive threads are done with queue. */
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   Tr_Info_Lo("Queue_Thread_2: Stop");
}

/**  Function of SENDER_THREAD.
 *
 *   - sends the messages to QUEUE_THREAD
 */

void Sender_Thread_2(void* param)
{
   SAL_Thread_Id_T wait_list[] =
   { QUEUE_THREAD };
   SAL_Thread_Id_T destroyed_list[] =
   { QUEUE_THREAD };
   Sample_Msg_T data1, data2, data3;

   Tr_Info_Lo("Sender_Thread_2: Start");

   /* Wait until queue has been created by QUEUE_THREAD. */
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   /* set data to send */
   data1.field1 = 123;
   data1.field2 = 456;

   data2.field1 = 234;
   data2.field2 = 567;

   data3.field1 = 345;
   data3.field2 = 678;

   /* send messages */
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));

   /* signals that every messages are in the queue */
   SAL_Signal_Ready();

   /*
    * Don't exit right away or receive threads may miss signal; instead, just wait
    * wait for queue thread to terminate after receive threads are done.
    */
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   Tr_Info_Lo("Sender_Thread_2: Stop");
}

/** \}
 */

/** \name Functions of the third Receive_From_Test
 *
 *  \{
 */

/**  Function of RECEIVE_FROM_THREADs.
 *
 *   - takes out the messages from the queue of QUEUE_THREAD
 */

void Receive_From_Timeout_Thread_3(void* param)
{
   static SAL_Int_T wait_list[] =
   { QUEUE_THREAD };
   static SAL_Int_T destroyed_list[] =
   { QUEUE_THREAD };
   const SAL_Message_T* msg;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;

   Tr_Info_Lo("Receive_From_Timeout_Thread_3: Start");

   /* wait for queue creation by QUEUE_THREAD */
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Int_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   /* lock mutex */
   if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   time_1 = SAL_Clock();

   /* receive message from QUEUE_THREAD with short timeout */
   msg = SAL_Receive_From_Timeout(QUEUE_THREAD, 100);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T) (time_2 - time_1);

   /* timeout has not been reached */
   if (msg != NULL)
   {
      PBC_Failed_2("SAL_Receive_From_Timeout: timeout has not been reached, delta_time: %d, EventId: %d", (int)delta_time, (int)msg->event_id);
   }

   /* time interval of timeout is not valid */
   if ((delta_time < 100 - 7) && (delta_time > 100 + 15))
   {
      PBC_Failed("SAL_Receive_From_Timeout: timeout interval is not valid");
   }

   /* unlock mutex */
   SAL_Unlock_Mutex(&Mutex);

   SAL_Signal_Ready();

   /*
    * Don't exit right away or send thread may miss signal; instead, just wait
    * wait for queue thread to terminate after the send thread is done.
    */
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   Tr_Info_Lo("Receive_From_Timeout_Thread_3: Stop");
}

/**  Function of the QUEUE_THREAD.
 *
 *   - creates the queue
 *   - holds the messages in the queue
 */

void Queue_Thread_3(void* param)
{
   SAL_Thread_Id_T destroyed_list[] =
   { SENDER_THREAD };

   Tr_Info_Lo("Queue_Thread_3: Start");

   SAL_Create_Queue(8, 0, malloc, free);

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   /* Wait until sender thread is done with queue. */
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   Tr_Info_Lo("Queue_Thread_3: Stop");
}

/**  Function of SENDER_THREAD.
 *
 *   - sends the messages to QUEUE_THREAD
 */

void Sender_Thread_3(void* param)
{
   SAL_Thread_Id_T wait_list[] =
   { RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2 };
   Sample_Msg_T data1, data2, data3;

   Tr_Info_Lo("Sender_Thread_3: Start");

   /*
    * Wait until receive threads signal that they are done trying (and failing)
    * to read from the queue.
    */
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   /* set data to send */
   data1.field1 = 123;
   data1.field2 = 456;

   data2.field1 = 234;
   data2.field2 = 567;

   data3.field1 = 345;
   data3.field2 = 678;

   /* send messages */
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));

   /* signals that every messages are in the queue */
   SAL_Signal_Ready();

   Tr_Info_Lo("Sender_Thread_3: Stop");
}

/** \}
 */

/** \name Functions of the fourth Receive_From_Test
 *
 *  \{
 */

/**  Function of RECEIVE_FROM_THREADs.
 *
 *   - takes out the messages from the queue of QUEUE_THREAD
 */

void Receive_From_Thread_4(void* param)
{
   const SAL_Message_T* msg;
   Sample_Msg_T* data;
   SAL_Stat_Queue_T stat;
   SAL_Thread_Id_T wait_list[] =
   { QUEUE_THREAD };

   Tr_Info_Lo("Receive_From_Thread_4: Start");

   /* Wait for queue to be created by QUEUE_THREAD */
   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

   SAL_Signal_Ready();

   do
   {
      /* lock mutex */
      SAL_Lock_Mutex(&Mutex);

      SAL_Stat_Queue(QUEUE_THREAD, &stat);

      /* receive message from QUEUE_THREAD */
      msg = SAL_Receive_From(QUEUE_THREAD);

      if (msg == NULL)
      {
         PBC_Failed("SAL_Receive_From: Message is null");
      }

      /* unlock mutex */
      SAL_Unlock_Mutex(&Mutex);

      if (msg->event_id != (SAL_Event_Id_T)EV_E4)
      {
         data = (Sample_Msg_T*) msg->data;

         Tr_Info_Mid_2("Receive_From_Thread: field1: %d, field2: %d",
            (int)data->field1, (int)data->field2);
      }

   } while (msg->event_id != (SAL_Event_Id_T)EV_E4);

   Tr_Info_Lo("Receive_From_Thread_4: Stop");
}

/**  Function of the QUEUE_THREAD.
 *
 *   - creates the queue
 *   - holds the messages in the queue
 */

void Queue_Thread_4(void* param)
{
   SAL_Thread_Id_T destroyed_list[] =
   { RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2 };

   Tr_Info_Lo("Queue_Thread_4: Start");

   SAL_Create_Queue(8, 0, malloc, free);

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   /* Wait until receive threads are done with the queue. */
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Queue_Thread_4: Stop");
}

/**  Function of SENDER_THREAD.
 *
 *   - sends the messages to QUEUE_THREAD
 */

void Sender_Thread_4(void* param)
{
   SAL_Thread_Id_T wait_list[] =
   { QUEUE_THREAD };
   Sample_Msg_T data1, data2, data3;

   Tr_Info_Lo("Sender_Thread_4: Start");
   /*
    * Wait until queue has been create by QUEUE_THREAD.
    */
   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

   /* set data to send */
   data1.field1 = 123;
   data1.field2 = 456;

   data2.field1 = 234;
   data2.field2 = 567;

   data3.field1 = 345;
   data3.field2 = 678;

   /* send messages */
   SAL_Sleep(500);

   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));

   SAL_Sleep(500);

   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));

   SAL_Sleep(500);

   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));

   /* EV_E4 */
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E4, NULL, 0);
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E4, NULL, 0);

   Tr_Info_Lo("Sender_Thread_4: Stop");
}

/** \}
 */

/** \name Functions of the fifth Receive_From_Test
 *
 *  \{
 */

/**  Function of RECEIVE_FROM_THREADs.
 *
 *   - takes out the messages from the queue of QUEUE_THREAD
 */

void Receive_From_Thread_5(void* param)
{
   const SAL_Message_T* msg;
   Sample_Msg_T* data;
   SAL_Stat_Queue_T stat;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   SAL_Thread_Id_T wait_list[] =
   { QUEUE_THREAD };

   Tr_Info_Lo("Receive_From_Thread_5: Start");

   /* Wait for queue to be created by QUEUE_THREAD */
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      SAL_Exit(1);
   }

   SAL_Signal_Ready();

   do
   {
      /* lock mutex */
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {
         Tr_Fault("Timeout has been reached");
         SAL_Exit(1);
      }

      SAL_Stat_Queue(QUEUE_THREAD, &stat);

      /* receive message from QUEUE_THREAD */
      time_1 = SAL_Clock();

      msg = SAL_Receive_From_Timeout(QUEUE_THREAD, Long_Timeout);

      time_2 = SAL_Clock();

      delta_time = (SAL_Int_T) (time_2 - time_1);

      /* message has been received and event_id != last message (EV_E4) */
      if ((msg != NULL) && (msg->event_id != (SAL_Event_Id_T)EV_E4))
      {
         /* thread has been suspended */
         /* and this is valid */
         if (delta_time > 3)
         {
            Tr_Info_Hi_1("SAL_Receive_From_Timeout: suspends the thread, time: %d", (int)delta_time);
         }

      }
      /*  */
      else if (NULL == msg)
      {
         Tr_Fault("SAL_Receive_From_Timeout: timeout has been reached");
         SAL_Exit(1);
      }

      /* unlock mutex */
      SAL_Unlock_Mutex(&Mutex);

      if (msg->event_id != (SAL_Event_Id_T)EV_E4)
      {
         data = (Sample_Msg_T*) msg->data;

         Tr_Info_Mid_2("Receive_From_Thread: field1: %d, field2: %d",
            (int)data->field1, (int)data->field2);
      }

   } while (msg->event_id != (SAL_Event_Id_T)EV_E4);

   Tr_Info_Lo("Receive_From_Thread_5: Stop");
}

/**  Function of the QUEUE_THREAD.
 *
 *   - creates the queue
 *   - holds the messages in the queue
 */

void Queue_Thread_5(void* param)
{
   SAL_Thread_Id_T destroyed_list[] =
   { RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2 };

   Tr_Info_Lo("Queue_Thread_5: Start");

   SAL_Create_Queue(8, 0, malloc, free);

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   /* Wait for receive threads to be done with the queue. */
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      SAL_Exit(1);
   }

   Tr_Info_Lo("Queue_Thread_5: Stop");
}

/**  Function of SENDER_THREAD.
 *
 *   - sends the messages to QUEUE_THREAD
 */

void Sender_Thread_5(void* param)
{
   SAL_Thread_Id_T wait_list[] =
   { RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2 };
   Sample_Msg_T data1, data2, data3;

   Tr_Info_Lo("Sender_Thread_5: Start");

   /* wait until QUEUE_THREAD creates the queue for test_nr = 1, 2 */
   /* wait for signal from REC_THREADs */
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      SAL_Exit(1);
   }

   /* set data to send */
   data1.field1 = 123;
   data1.field2 = 456;

   data2.field1 = 234;
   data2.field2 = 567;

   data3.field1 = 345;
   data3.field2 = 678;

   /* send messages */
   SAL_Sleep(500);

   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));

   SAL_Sleep(500);

   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));

   SAL_Sleep(500);

   SAL_Send(APP_ID, QUEUE_THREAD, EV_E1, &data1, sizeof(data1));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E2, &data2, sizeof(data2));
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E3, &data3, sizeof(data3));

   /* EV_E4 */
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E4, NULL, 0);
   SAL_Send(APP_ID, QUEUE_THREAD, EV_E4, NULL, 0);

   Tr_Info_Lo("Sender_Thread_5: Stop");
}

/** \}
 */

/** \name Other Functions
 *
 *  \{
 */

/**  Function of test case.
 *
 *   Function creates four threads: SENDER_THREAD, RECEIVE_FROM_THREAD_1, 
 *   RECEIVE_FROM_THREAD_2, QUEUE_THREAD and starts SAL_Run.
 */

SAL_Int_T Receive_From_Test(void* rec_fun, void* send_fun, void* queue_fun, bool_t xsal_only)
{
   SAL_Int_T ret = 0;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Mutex_Attr_T mutex_attr;
   SAL_Thread_Id_T destroyed_list[] =
   { SENDER_THREAD, RECEIVE_FROM_THREAD_1, RECEIVE_FROM_THREAD_2, QUEUE_THREAD };
   SAL_Init_Mutex_Attr(&mutex_attr);

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      SAL_Exit(1);
   }

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(rec_fun, NULL, Init_Thread_Attr("REC1", RECEIVE_FROM_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(rec_fun, NULL, Init_Thread_Attr("REC2", RECEIVE_FROM_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(queue_fun, NULL, Init_Thread_Attr("QUEUE", QUEUE_THREAD, priority, &thread_attr));

   SAL_Create_Thread(send_fun, NULL, Init_Thread_Attr("SENDER", SENDER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (xsal_only)
   {
      SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Int_T));
   }
   else
   {
      if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Int_T), Long_Timeout))
      {
         Tr_Fault("Timeout has been reached");
         ret = 1;
      }
   }

   SAL_Destroy_Mutex(&Mutex);

   return ret;
}

/** \}
 */

SAL_Int_T Set_Up()
{
   SAL_Config_T config;

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&config))
   {
      return 1;
   }

   return 0;
}

void Tear_Down()
{
}

int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;
   bool_t xsal_only = 0;

   if (Set_Up())
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   /* FIRST SENT, THEN RECEIVED */

#if defined (XSAL)

   xsal_only = true;

   /* without timeout */
   ret = ret || Receive_From_Test(Receive_From_Thread_1, Sender_Thread_1, Queue_Thread_1, xsal_only);

#endif

   xsal_only = false;

   /* without long timeout */
   ret = ret || Receive_From_Test(Receive_From_Thread_2, Sender_Thread_2, Queue_Thread_2, xsal_only);

   /* with short timeout */
   ret = ret || Receive_From_Test(Receive_From_Timeout_Thread_3, Sender_Thread_3, Queue_Thread_3, xsal_only);

   /* FIRST WAITING FOR RECEIVING, THEN SENT */

#if defined (XSAL)

   xsal_only = true;

   /* without timeout */
   ret = ret || Receive_From_Test(Receive_From_Thread_4, Sender_Thread_4, Queue_Thread_4, xsal_only);

#endif

   xsal_only = false;

   /* with long timeout */
   ret = ret || Receive_From_Test(Receive_From_Thread_5, Sender_Thread_5, Queue_Thread_5, xsal_only);

   Tear_Down();

   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));

   return ret;

}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  01 Sep 2006, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Initial revision
 *
 * - 30-Aug-2014 Kirk Bailey
 *   - Fixed Klocwork defects.
 *
 * -----------------------------------------------------------------------*/
