/* -------------------------------------------------------------------------
 *
 * File:        test_send.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of send communication.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "pbc_trace.h"
#include "xsal.h"
#include "pi_unistd.h"

EM_FILENUM(TEST_MODULE_ID, 7);

#define APP_TEST_LOCAL    2
#define APP_TEST_SENDER   2
#define APP_TEST_RECEIVER 3

#define EV_E6 6
#define EV_ERROR 7


/* Thread ids */
typedef enum Thread_Id_Tag
{
   RECEIVER_1 = 1,
   SENDER_1, 

   RECEIVER_2,
   SENDER_2, 

   RECEIVER_3,
   SENDER_3, 

   RECEIVER_4,
   SENDER_4, 

   LAST_THREAD_ID = SENDER_4
} Thread_Id_T;


typedef enum Thread_Ctrl_Id_Tag
{
   S_CTRL_1 = 1,
   R_CTRL_1, 

   S_CTRL_2,
   R_CTRL_2, 

   S_CTRL_3,
   R_CTRL_3, 

   S_CTRL_4,
   R_CTRL_4
} Thread_Ctrl_Id_T;


/* Control messages */
typedef enum E_Ready_Tag
{
   EV_REMOTE_SENDER_READY = 1,
   EV_REMOTE_RECEIVER_READY 
} E_Ready_T;


typedef enum E_Test_Type_Tag
{
   E_STAND_ALONE, 
   E_SENDER, 
   E_RECEIVER 
} E_Test_Type_T;

typedef struct Ev_Test_Case_Data_Tag
{
   SAL_Event_Id_T event_id;
   size_t data_size;
   void* data;
} Ev_Test_Case_Data_T;

typedef struct Sample_Msg_Tag
{
   SAL_Int_T field1;
   SAL_Int_T field2;
} Sample_Msg_T;

struct timeb Time;

SAL_Semaphore_T Sem_Rec  [LAST_THREAD_ID + 1];
SAL_Semaphore_T Sem_Send [LAST_THREAD_ID + 1];

E_Test_Type_T Test_Type = E_STAND_ALONE;

SAL_Mutex_T Queue_Dump_Mutex;

SAL_Int_T Timeout = 3000;

/*
 *  Test Case 1:
 *
 *  Only normal (not urgent) events without data
 */
const Ev_Test_Case_Data_T Ev_Send_TC_1[] =
{
   { 1, 0, "1234" },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL }
};


const Ev_Test_Case_Data_T Ev_Receive_TC_1[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL }
};

#if defined (XSAL)

/*
 *  Test Case 2:
 *
 *  Normal and urgent events without data
 */
const Ev_Test_Case_Data_T Ev_Send_TC_2[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL }
};

const SAL_Event_Id_T Ev_Urgent_List_TC_2[] = { 2, 4 };

const Ev_Test_Case_Data_T Ev_Receive_Urgent_TC_2[] =
{
   { 2, 0, NULL },
   { 4, 0, NULL },
   { 1, 0, NULL },
   { 3, 0, NULL }
};

#endif

/*
 *  Test Case 3:
 *
 *  Only normal (not urgent) events with data
 */
const Ev_Test_Case_Data_T Ev_Send_TC_3[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" }
};

const Ev_Test_Case_Data_T Ev_Receive_TC_3[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" }
};


#if defined (XSAL)

/*
 *  Test Case 4:
 *
 *  Normal and urgent events with data
 */
const Ev_Test_Case_Data_T Ev_Send_TC_4[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" }
};

const SAL_Event_Id_T Ev_Urgent_List_TC_4[] = { 2, 4 };

const Ev_Test_Case_Data_T Ev_Receive_Urgent_TC_4[] =
{
   { 2, 5,  "1234" },
   { 4, 17, "1234567890123456" },
   { 1, 1,  "" },
   { 3, 9,  "12345678" }
};

#endif

/*
 *  Test Case 5:
 *
 *  Only normal (not urgent) events without data.
 *  Send more events then size of message queue
 */
const Ev_Test_Case_Data_T Ev_Send_TC_5[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL },
   { 5, 0, NULL }
};


const Ev_Test_Case_Data_T Ev_Receive_TC_5[] =
{
   { 1, 0, NULL },
   { 2, 0, NULL },
   { 3, 0, NULL },
   { 4, 0, NULL }
};

#if defined (XSAL)

/*
 *  Test Case 6:
 *
 *  Normal and urgent events with data.
 *  Send more events then size of message queue
 */

const Ev_Test_Case_Data_T Ev_Send_TC_6[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 5, 17, "1234567890123456789012345" }
};

const SAL_Event_Id_T Ev_Urgent_List_TC_6[] = { 2, 4, 5 };

const Ev_Test_Case_Data_T Ev_Receive_Urgent_TC_6[] =
{
   { 2, 5,  "1234" },
   { 4, 17, "1234567890123456" },
   { 5, 17, "1234567890123456789012345" },
   { 1, 1,  "" }
};

#endif

/**
 *   \file
 *
 *   Test of local and remote communication using SAL_Send function and urgent messages.
 *
 *   \section T Tests: \n
 *   <OL>
 *   <LI> \link STS Send_Test_Suite \endlink - test of local and remote communication using 
 *        SAL_Send function and urgent messages. </LI>
 *   <LI> \link TST Time_Send_Test \endlink - test of timestamps. </LI>
 *   </OL>
 *
 *   \page STS Send_Test_Suite
 *
 *   \section CBS Can be started:
 *   - as one local process, without entry parameters
 *   - as two remote processes: sender and receiver, with entry parameters suitably -s and -r,
 *     receiver thread must be started as the first.
 *
 *   \section FBP For both (local and remote) processes:
 *   \subsection T1 Threads:
 *   There are four sender threads (SENDER_\a n where \a n is equal 2, 4, 6 or 8) 
 *   and four receiver threads (RECEIVER_\a n where \a n is equal 1, 3, 5 or 7). \n
 *   Each sender sends messages to the receiver which thread id is less by one than sender
 *   thread id.
 *
 *   \subsection TC0 Test cases:
 *   Senders send lists of messages (test cases) to the receivers. There are six lists 
 *   of messages (test cases), with different event_ids, data_sizes and data.
 *   All of the messages should be received besides the test cases where number of 
 *   sent messages is greater than receiver queue size.
 *
 *   \subsection S Semaphores:
 *   There are also eight semaphores used in this test. 
 *   - Sem_Rec[\a n] (where \a n is equal receiver threads ids (1, 3, 5, 7) ) - control 
 *     if senders send the messages to the receivers when receivers are ready
 *   - Sem_Send[\a n] (where \a n is equal sender threads ids (2, 4, 6, 8) - control if
 *     receivers take the messages out of the queues after senders has sent all messages 
 *     from the list.
 *   
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> RECEIVER_\a m creates the queue. </LI>
 *   <LI> SENDER_\a n waits until RECEIVER_\a m is ready (semaphore Sem_Rec[\a n]). </LI>
 *   <LI> If test set includes urgent messages, RECEIVER_\a m declares \b URG urgent messages. </LI>
 *   <LI> RECEIVER_\a m signals that is ready (Sem_Rec[\a m]). </LI>
 *   <LI> RECEIVER_\a m waits until sender sends \b SEND_MSG messages (Sem_Send[\a m+1]). </LI>
 *   <LI> SENDER_\a n sends \b SEND_MSG messages. </LI>
 *   <LI> SENDER_\a n signals that \b SEND_MSG messages has been sent (Sem_Send[\a n]). </LI>
 *   <LI> RECEIVER_\a m checks if the number of messages in the queue is valid. </LI>
 *   <LI> RECEIVER_\a m receives \b REC_MSG messages and checks if all header components (apart from
 *        timestamp) and data are valid. </LI>
 *   <LI> RECEIVER_\a m checks if message queue is empty after taking out all \b REC_MSG messages. \n </LI>
 *   </OL>
 *   where \a n = \a m + 1.
 *
 *   \section LP Local_Process
 *
 *   There are four senders and four receivers as described above.
 *
 *
 *   \section RP Remote_Process
 *
 *   \subsection RPRP - in Receiver Process:
 *   There are four receiver threads (RECEIVER_\a n, where \a n is equal 1, 3, 5 or 7)
 *   and four control threads (R_CTRL_\a n, where \a n is equal 2, 4, 6 and 8).
 *
 *   \subsection RPPP - in Sender Process:
 *
 *   There are four sender threads (SENDER_\a n, where \a is equal 2, 4, 6 and 8) and four
 *   control threads (S_CTRL_\a n, where \a n is equal 1, 3, 5 or 7).
 * 
 *   Control threads of receiver process are used to communicate with control threads of sender
 *   process. This communication enables semaphore synchronization between threads of different
 *   processes. Receiver control threads communicate with sender control threads with ids 
 *   less by one than receiver's id.
 *
 *
 *  \section TC Test Cases
 *
 *  <TABLE>
 *  <TR><TD><B><CENTER>NO</TD><TD>Desc</TD><TD>SEND_MSG</TD>
 *      <TD>URG</TD><TD>REC_MSG</TD></TD><TD>XSAL only</CENTER></B></TD></TR>
 *  <TR><TD>1</TD><TD>Only normal (not urgent) events without data</TD>
 *      <TD>{ 1, 0, NULL }\n{ 2, 0, NULL }\n{ 3, 0, NULL }\n{ 4, 0, NULL }</TD>
 *      <TD>-</TD>
        <TD>{ 1, 0, NULL }\n{ 2, 0, NULL }\n{ 3, 0, NULL }\n{ 4, 0, NULL }</TD><TD>-</TD></TR>
 *  <TR><TD>2</TD><TD>Normal and urgent events without data</TD>
 *      <TD>{ 1, 0, NULL }\n{ 2, 0, NULL }\n{ 3, 0, NULL }\n{ 4, 0, NULL }</TD>
 *      <TD> 2, 4 </TD>
        <TD>{ 2, 0, NULL }\n{ 4, 0, NULL }\n{ 1, 0, NULL }\n{ 3, 0, NULL }</TD><TD>yes</TD></TR>
 *  <TR><TD>3</TD><TD>Only normal (not urgent) events with data</TD>
 *      <TD>{ 1, 1, "" }\n{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }</TD>
 *      <TD>-</TD>
 *      <TD>{ 1, 1, "" }\n{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }</TD>
 *      <TD>-</TD></TR>
 *  <TR><TD>4</TD><TD>Normal and urgent events with data</TD>
 *      <TD>{ 1, 1, "" }\n{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }</TD>
 *      <TD>2, 4</TD>
 *      <TD>{ 2, 5, "1234" }\n{ 4, 17, "1234567890123456" }\n{ 1, 1, "" }\n{ 3, 9, "12345678" }</TD>
 *      <TD>yes</TD></TR>
 *  <TR><TD>5</TD><TD>Only normal (not urgent) events without data.\n
 *                    Sent more events then size of message queue</TD>
 *      <TD>{ 1, 0, NULL }\n{ 2, 0, NULL }\n{ 3, 0, NULL }\n{ 4, 0, NULL }\n{ 5, 0, NULL }</TD>
 *      <TD>-</TD>
 *      <TD>{ 1, 0, NULL }\n{ 2, 0, NULL }\n{ 3, 0, NULL }\n{ 4, 0, NULL }</TD><TD>-</TD></TR>
 *  <TR><TD>6</TD><TD> Normal and urgent events with data.\n
 *                     Send more events then size of message queue</TD>
 *      <TD>{ 1, 1, "" }\n{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }\n
 *      { 5, 17, "1234567890123456789012345" }</TD>
 *      <TD>2, 4, 5</TD>
 *      <TD>{ 2, 5, "1234" }\n{ 4, 17, "1234567890123456" }\n{ 5, 17, "1234567890123456789012345" }\n
 *      { 1, 1, "" }</TD><TD>yes</TD></TR>
 *  </TABLE>
 *
 *  where:
 *
 *  - \b NO       - test case number
 *  - \b SEND_MSG - list of messages sended by sender in test case
 *  - \b URG      - events defined as urgent
 *  - \b REC_MSG  - expected received messages
 */
/**   \page TST Time_Send_Test
 *
 *   Two threads are created: SENDER_1 and RECEIVER_1.
 *
 *   \section TP2 Test procedure:
 *
 *   <OL>
 *   <LI> RECEIVER_1 creates the queue. </LI>
 *   <LI> SENDER_1 gets system time. </LI>
 *   <LI> SENDER_1 sends the message. </LI>
 *   <LI> RECEIVER_1 receives the message. </LI>
 *   <LI> RECEIVER_1 checks if timestamp is correct. </LI>
 *   </OL>
 */


const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, 
                                          SAL_Thread_Id_T id, 
                                          SAL_Priority_T prior, 
                                          SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}


/**  \name Send_Test_Suite
 *
 *   \{
 */

/**  Auxiliary function of receiver, writes out SAL_Stat_Queue_T contents.
 */

void Dump_Queue_Stat(SAL_Thread_Id_T queue_id, SAL_Stat_Queue_T* stat)
{
#define STAT_BUF_SZ 64
   char qstat[STAT_BUF_SZ];

   snprintf(qstat, STAT_BUF_SZ, "Queue %d Stat: queue_size: %d buffer_size: %d",
            (int)queue_id, (int)stat->queue_size, (int)stat->buffer_size);

   Tr_Fault_4("%s message_count: %d peak_message_count: %d peak_message_size: %d",
              qstat, (int)stat->message_count, (int)stat->peak_message_count,
              (int)stat->peak_message_size);
}


#if defined (XSAL)

/**  Auxiliary function of receiver, writes out event_ids and data_size of all messages 
 *   in the queue (<B><I>XSAL only</I></B>).
 */

void Dump_Queue(const char* pref)
{
   SAL_Queue_Iterator_T iter;

   SAL_First_Message(&iter);

   SAL_Lock_Mutex(&Queue_Dump_Mutex);

   Tr_Fault_1("Messages in the queue: %s", pref);
   while(!SAL_Is_Queue_End(&iter))
   {
      const SAL_Message_T* msg = SAL_Get_Message(&iter);
      Tr_Fault_2("[event_id %d, data_size=%d]", (int)msg->event_id, (int)msg->data_size);
      SAL_Next_Message(&iter);
   }

   SAL_Unlock_Mutex(&Queue_Dump_Mutex);
}

#endif


/**  Function of receiver, receives and checks the list of events.
 *
 *   - checks if number of messages in the queue is valid
 *   - receives messages and checks all data of messages
 *   - after receiving messages checks if the queue is empty
 */

SAL_Int_T Check_Events(const Ev_Test_Case_Data_T* event_list, size_t event_list_size)
{
   size_t i;
   SAL_Stat_Queue_T stat_queue;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat_queue);

   /* check if the number of messages in the queue is valid */
   if (stat_queue.message_count != event_list_size)
   {
      Tr_Fault_2("Check_Events: Invalid message count in the queue %d %d", 
               (int)stat_queue.message_count, (int)event_list_size);
      Dump_Queue_Stat(SAL_Get_Thread_Id(), &stat_queue);
#if defined (XSAL)
      Dump_Queue("Check_Events"); /* !!! */
#endif

      return 1;
   }

   /* receive message and check if all data are valid */
   for(i = 0; i < event_list_size; i++)
   {
      const SAL_Message_T* msg;

#if defined (XSAL)
      msg = SAL_Receive();
#else
      msg = SAL_Receive_Timeout(Timeout);

      if (msg == NULL)
      {
         PBC_Failed("SAL_Receive_Timeout: timeout has been reached");
      }
#endif

      Tr_Info_Mid_4("Ev %d, Size %d, From(%d, %d)",
                  (int)msg->event_id, (int)msg->data_size, (int)msg->sender_app_id, (int)msg->sender_thread_id);

      if (msg->event_id != event_list[i].event_id  ||
         msg->data_size != event_list[i].data_size ||
         memcmp(msg->data, event_list[i].data, msg->data_size) != 0 ||
         msg->sender_app_id != APP_TEST_SENDER ||
         msg->sender_thread_id != thread_id + 1 ||
         msg->receiver_thread_id != thread_id)
      {
         SAL_Stat_Queue_T stat;

         Tr_Fault_2("Check_Events: expected %d, received %d", (int)event_list[i].event_id, (int)msg->event_id);

         SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);
         Dump_Queue_Stat(SAL_Get_Thread_Id(), &stat);
#if defined (XSAL)
         Dump_Queue("Error"); /* !!! */
#endif
         return 1;
      }
   }

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat_queue);

   /* check if queue is empty */
   if (stat_queue.message_count != 0)
   {
      Tr_Fault("Check_Events: queue is not empty");
      return 1;
   }
   return 0;
}



/** Function of receiver, test case.
 *  
 *  - uses semaphores for communication synchronization
 *  - receives and checks events
 */

SAL_Int_T Receive_Test(SAL_Int_T i, 
#if defined (XSAL)
                       const SAL_Event_Id_T* urgent_list, 
                       size_t urgent_list_size,
#endif
                       const Ev_Test_Case_Data_T* event_list, 
                       size_t event_list_size)
{
   SAL_Int_T ret;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

#if defined (XSAL)
   if (urgent_list_size > 0)
   {
      SAL_Declare_Urgent(urgent_list, urgent_list_size);
   }
#endif

   /* receiver is ready */
   SAL_Signal_Semaphore(&Sem_Rec[thread_id]);

   /* wait until sender sends messages */
#if defined (XSAL)
   SAL_Wait_Semaphore(&Sem_Send[thread_id+1]);
#else
   if (!SAL_Wait_Semaphore_Timeout(&Sem_Send[thread_id+1], Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }
#endif

   ret = Check_Events(event_list, event_list_size);

#if defined (XSAL)
   if (urgent_list_size > 0)
   {
      SAL_Undeclare_Urgent(urgent_list, urgent_list_size);
   }
#endif

   Tr_Info_Hi_2("Receive_Test %d: %d", (int)i, (int)ret);

   return ret;
}


/**  Function of sender.
 *
 *   - sends list of messages given as an entry parameter
 */

SAL_Int_T Send_Events(SAL_App_Id_T app_id, 
                      SAL_Thread_Id_T thread_id, 
                      const Ev_Test_Case_Data_T* event_list, 
                      size_t event_list_size)
{
   size_t i;

   for(i = 0; i < event_list_size; i++)
   {
      if (!SAL_Send(app_id, thread_id, event_list[i].event_id, event_list[i].data, event_list[i].data_size))
      {
         PBC_Failed("Send_Events FAILED");
      }
   }
   return 0;
}


/**  Function of sender, test case.
 *
 *   - uses semaphores for communication synchronization
 *   - sends the list of messages
 */

SAL_Int_T Send_Test(SAL_Int_T i, 
                    SAL_App_Id_T app_id, 
                    SAL_Thread_Id_T receiver_thread_id, 
                    const Ev_Test_Case_Data_T* event_list, 
                    size_t event_list_size)
{
   SAL_Int_T ret;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

   /* wait until receiver thread will be ready */
#if defined (XSAL)
   SAL_Wait_Semaphore(&Sem_Rec[receiver_thread_id]);
#else
   if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[receiver_thread_id], Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }
#endif

   /* send test messages */
   ret = Send_Events(app_id, receiver_thread_id, event_list, event_list_size);

   /* signal that all test messages were send */
   SAL_Signal_Semaphore(&Sem_Send[thread_id]);

   Tr_Info_Hi_2("Send_Test %d: %d", (int)i, (int)ret);

   return ret;
}


/**  Function of sender, test case 5.
 *
 *   - uses semaphores for communication synchronization
 *   - sends the list of messages
 */

SAL_Int_T Send_Test_5(SAL_App_Id_T app_id, SAL_Thread_Id_T receiver_thread_id)
{
   SAL_Int_T i;
   SAL_Int_T ret = 0;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();
   SAL_Int_T event_list_size = sizeof(Ev_Send_TC_5)/sizeof(Ev_Test_Case_Data_T);

   /* wait until receiver thread will be ready */
#if defined (XSAL)
   SAL_Wait_Semaphore(&Sem_Rec[receiver_thread_id]);
#else
   if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[receiver_thread_id], Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }
#endif

   /* send test messages */
   /* sends 5 messages (more then receiver queue size) */
   for(i = 0; i < event_list_size-1; i++)
   {
      if (!SAL_Send(app_id, receiver_thread_id, Ev_Send_TC_5[i].event_id, Ev_Send_TC_5[i].data, Ev_Send_TC_5[i].data_size))
      {
         Tr_Fault("Send_Test_5 FAILED");
         ret = 1;
      }
   }
   if (SAL_Send(app_id, receiver_thread_id, Ev_Send_TC_5[i].event_id, Ev_Send_TC_5[i].data, Ev_Send_TC_5[i].data_size))
   {
      Tr_Fault("Send_Test_5 FAILED");
      ret = 1;
   }

   /* signal that all test messages were send */
   SAL_Signal_Semaphore(&Sem_Send[thread_id]);

   Tr_Info_Hi_1("Send_Test_5 %d", (int)ret);

   return ret;
}


/** ************************************************************
 *
 *  Test communications between threads inside the same process
 *
 *  ***********************************************************/


/**  Function of RECEIVER_\a n for local receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Local_Receiver_Test(void* p)
{
   SAL_Int_T ret = 0;
   size_t message_size = (size_t)p;

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

#if defined (XSAL)
   ret = ret || Receive_Test(1, NULL, 0, 
                Ev_Receive_TC_1, sizeof(Ev_Receive_TC_1)/sizeof(Ev_Test_Case_Data_T));
#else
   ret = ret || Receive_Test(1, Ev_Receive_TC_1, 
                sizeof(Ev_Receive_TC_1)/sizeof(Ev_Test_Case_Data_T));
#endif


#if defined (XSAL)
   ret = ret || Receive_Test(2, Ev_Urgent_List_TC_2, 
                sizeof(Ev_Urgent_List_TC_2)/sizeof(SAL_Event_Id_T),
                Ev_Receive_Urgent_TC_2, 
                sizeof(Ev_Receive_Urgent_TC_2)/sizeof(Ev_Test_Case_Data_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test(3, NULL, 0, 
                Ev_Receive_TC_3, sizeof(Ev_Receive_TC_3)/sizeof(Ev_Test_Case_Data_T));
#else
   ret = ret || Receive_Test(3, Ev_Receive_TC_3, 
                sizeof(Ev_Receive_TC_3)/sizeof(Ev_Test_Case_Data_T));
#endif


#if defined (XSAL)   
   ret = ret || Receive_Test(4, Ev_Urgent_List_TC_4, 
                sizeof(Ev_Urgent_List_TC_4)/sizeof(SAL_Event_Id_T), 
                Ev_Receive_Urgent_TC_4, 
                sizeof(Ev_Receive_Urgent_TC_4)/sizeof(Ev_Test_Case_Data_T));
                             
#endif

#if defined (XSAL)
   ret = ret || Receive_Test(5, NULL, 0, 
                Ev_Receive_TC_5, sizeof(Ev_Receive_TC_5)/sizeof(Ev_Test_Case_Data_T));
#else
   ret = ret || Receive_Test(5, Ev_Receive_TC_5, 
                sizeof(Ev_Receive_TC_5)/sizeof(Ev_Test_Case_Data_T));
#endif


#if defined (XSAL)
   ret = ret || Receive_Test(6, Ev_Urgent_List_TC_6, 
                sizeof(Ev_Urgent_List_TC_6)/sizeof(SAL_Event_Id_T), 
                Ev_Receive_Urgent_TC_6, 
                sizeof(Ev_Receive_Urgent_TC_6)/sizeof(Ev_Test_Case_Data_T));
#endif

   if (ret)
   {
      SAL_Exit(1);
   }
}


/**  Function of SENDER_\a n for local sender thread.
 *
 *   - sends messages from the list of all test cases
 */

void Local_Sender_Test(void* param)
{
   SAL_Thread_Id_T receiver_id = (intptr_t) param;
   SAL_Int_T ret = 0;
   SAL_App_Id_T app_id = SAL_Get_App_Id();

   ret = ret || Send_Test(1, app_id, receiver_id, Ev_Send_TC_1, sizeof(Ev_Send_TC_1)/sizeof(Ev_Test_Case_Data_T));

#if defined (XSAL)
   ret = ret || Send_Test(2, app_id, receiver_id, Ev_Send_TC_2, sizeof(Ev_Send_TC_2)/sizeof(Ev_Test_Case_Data_T));
#endif

   ret = ret || Send_Test(3, app_id, receiver_id, Ev_Send_TC_3, sizeof(Ev_Send_TC_3)/sizeof(Ev_Test_Case_Data_T));

#if defined (XSAL)
   ret = ret || Send_Test(4, app_id, receiver_id, Ev_Send_TC_4, sizeof(Ev_Send_TC_4)/sizeof(Ev_Test_Case_Data_T));
#endif

   ret = ret || Send_Test_5(app_id, receiver_id);

#if defined (XSAL)
   ret = ret || Send_Test(6, app_id, receiver_id, Ev_Send_TC_6, sizeof(Ev_Send_TC_6)/sizeof(Ev_Test_Case_Data_T));
#endif

   if (ret)
   {
      SAL_Exit(1);
   }
}


#if defined (XSAL)

/** ********************************************
 *
 *  Tests communications between two processes
 *
 *  *******************************************/


/** Function of RECEIVER_\a n for remote receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Remote_Receiver_Test(void* p)
{
   SAL_Int_T ret = 0;
   size_t message_size = (size_t)p;

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

   ret = ret || Receive_Test(1, NULL, 0, 
                Ev_Receive_TC_1, sizeof(Ev_Receive_TC_1)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Receive_Test(2, Ev_Urgent_List_TC_2, 
                sizeof(Ev_Urgent_List_TC_2)/sizeof(SAL_Event_Id_T),
                Ev_Receive_Urgent_TC_2,
                sizeof(Ev_Receive_Urgent_TC_2)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Receive_Test(3, NULL, 0, 
                Ev_Receive_TC_3, sizeof(Ev_Receive_TC_3)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Receive_Test(4, Ev_Urgent_List_TC_4, 
                sizeof(Ev_Urgent_List_TC_4)/sizeof(SAL_Event_Id_T),
                Ev_Receive_Urgent_TC_4, 
                sizeof(Ev_Receive_Urgent_TC_4)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Receive_Test(5, NULL, 0, 
                Ev_Receive_TC_5, sizeof(Ev_Receive_TC_5)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Receive_Test(6, Ev_Urgent_List_TC_6, 
                sizeof(Ev_Urgent_List_TC_6)/sizeof(SAL_Event_Id_T),
                Ev_Receive_Urgent_TC_6, 
                sizeof(Ev_Receive_Urgent_TC_6)/sizeof(Ev_Test_Case_Data_T));

   if (ret)
   {
      SAL_Exit(1);
   }
}


/**  Function of remote receiver control thread.
 *   
 *   - is called by Remote_Receiver_Driver function for each test case
 *   - communicates with remote sender control thread, sets the semaphore values
 */

void Wait_For_Sender(SAL_Thread_Id_T thread_id)
{
   const SAL_Message_T* msg;

   /* wait for receiver thread will be ready */
   SAL_Wait_Semaphore(&Sem_Rec[thread_id-1]);

   /* inform sender thar receiver is ready */
   SAL_Send(APP_TEST_SENDER,thread_id-1, EV_REMOTE_RECEIVER_READY, NULL, 0);
   
   /* wait until sender sent all messages */
   msg = SAL_Receive();

   if (msg->event_id != (SAL_Event_Id_T)EV_REMOTE_SENDER_READY)
   {
      Tr_Fault_1("Received invalid message: %d", (int)msg->event_id);

      SAL_Send(APP_TEST_SENDER, thread_id-1, EV_ERROR, NULL, 0);

      SAL_Exit(1);
   }

   if (msg->event_id == EV_ERROR)
   {
      PBC_Failed("Receiver control received an information that sender failed");      
   }
   
   SAL_Signal_Semaphore(&Sem_Send[thread_id]);
}


/**  Function of R_CTRL_\a n for remote receiver control thread.
 *
 *   - communicates with remote sender control thread, sets the semaphore values
 */

void Remote_Receiver_Driver(void* param)
{
   const SAL_Message_T* msg;
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

   if (!SAL_Create_Queue(2, 0, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

   /* wait for sender initialization */
   msg = SAL_Receive();

   if (msg->event_id != (SAL_Event_Id_T)EV_REMOTE_SENDER_READY)
   {
      Tr_Fault_1("Received invalid message: %d", (int)msg->event_id);

      SAL_Send(APP_TEST_SENDER, thread_id-1, EV_ERROR, NULL, 0);

      SAL_Exit(1);
   }

   /* test case 1 */
   Wait_For_Sender(thread_id);

   /* test case 2 */
   Wait_For_Sender(thread_id);

   /* test case 3 */
   Wait_For_Sender(thread_id);

   /* test case 4 */
   Wait_For_Sender(thread_id);

   /* test case 5 */
   Wait_For_Sender(thread_id);

   /* test case 6 */
   Wait_For_Sender(thread_id);
}


/**  Function of SENDER\a n for remote sender thread.
 *
 *   - sends messages from the list of all test cases   
 */

void Remote_Sender_Test(void* param)
{
   SAL_Int_T ret = 0;
   SAL_Thread_Id_T receiver_id = (intptr_t) param;

   ret = ret || Send_Test(1, APP_TEST_RECEIVER, receiver_id, Ev_Send_TC_1, sizeof(Ev_Send_TC_1)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Send_Test(2, APP_TEST_RECEIVER, receiver_id, Ev_Send_TC_2, sizeof(Ev_Send_TC_2)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Send_Test(3, APP_TEST_RECEIVER, receiver_id, Ev_Send_TC_3, sizeof(Ev_Send_TC_3)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Send_Test(4, APP_TEST_RECEIVER, receiver_id, Ev_Send_TC_4, sizeof(Ev_Send_TC_4)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Send_Test_5(APP_TEST_RECEIVER, receiver_id);

   ret = ret || Send_Test(6, APP_TEST_RECEIVER, receiver_id, Ev_Send_TC_6, sizeof(Ev_Send_TC_6)/sizeof(Ev_Test_Case_Data_T));

   if (ret)
   {
      SAL_Exit(1);
   }
}


/**  Function of remote sender control thread.
 * 
 *   - is called by Remote_Sender_Driver for each test case
 *   - communicates with remote sender control thread, sets the semaphore values
 */

void Wait_For_Receiver(SAL_Thread_Id_T thread_id)
{
   const SAL_Message_T* msg;

   /* wait until receiver will be ready */
   msg = SAL_Receive();

   if (msg->event_id == EV_ERROR)
   {
      PBC_Failed("Sender control received an information that receiver failed");
   }

   if (msg->event_id != (SAL_Event_Id_T)EV_REMOTE_RECEIVER_READY ||
      msg->sender_thread_id != thread_id + 1)
   {
      Tr_Fault("Received invalid message");

      SAL_Send(APP_TEST_RECEIVER, thread_id+1, EV_ERROR, NULL, 0);

      SAL_Exit(1);
   }

   SAL_Signal_Semaphore(&Sem_Rec[thread_id]);

   SAL_Wait_Semaphore(&Sem_Send[thread_id+1]);

   /* sender sent messages */
   SAL_Send(APP_TEST_RECEIVER, thread_id + 1, EV_REMOTE_SENDER_READY, NULL, 0);
}


/**  Function of S_CTRL_\a n for remote sender control thread.
 *
 *   - communicates with remote receiver control thread, sets the semaphore values
 */

void Remote_Sender_Driver(void* param)
{
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();

   if (!SAL_Create_Queue(2, 0, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

   /* sender is ready */
   if (!SAL_Send(APP_TEST_RECEIVER, thread_id+1, EV_REMOTE_SENDER_READY, NULL, 0))
   {
      PBC_Failed("Remote receiver is not ready yet.");
   }


   /* test case 1 */
   Wait_For_Receiver(thread_id);

   /* test case 2 */
   Wait_For_Receiver(thread_id);

   /* test case 3 */
   Wait_For_Receiver(thread_id);

   /* test case 4 */
   Wait_For_Receiver(thread_id);

   /* test case 5 */
   Wait_For_Receiver(thread_id);

   /* test case 6 */
   Wait_For_Receiver(thread_id);
}

#endif

/**  Test of SAL_Send function in local or remote communication. 
 *   
 *   Urgent messages are also tested.
 *   Creates eight threads and starts SAL_Run.
 */

SAL_Int_T Send_Test_Suite()
{
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_1, SENDER_1 };
   SAL_Int_T retcode;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   switch(Test_Type)
   {

#if defined (XSAL)
      /* remote communication - sender */
   case E_SENDER:

      /* Sender1 thread */
      SAL_Create_Thread(
         Remote_Sender_Test,   
         (void*)RECEIVER_1, 
         Init_Thread_Attr("RSndT1", SENDER_1, priority, &thread_attr));

      /* Control thread of sender1 */
      SAL_Create_Thread(
         Remote_Sender_Driver, 
         NULL, 
         Init_Thread_Attr("RSndD1", S_CTRL_1, priority, &thread_attr));

      /* Sender2 thread */
      SAL_Create_Thread(
         Remote_Sender_Test,   
         (void*)RECEIVER_2, 
         Init_Thread_Attr("RSndT2", SENDER_2, priority, &thread_attr));

      /* Control thread of sender2 */
      SAL_Create_Thread(
         Remote_Sender_Driver, 
         NULL, 
         Init_Thread_Attr("RSndD2", S_CTRL_2, priority, &thread_attr));

      /* Sender3 thread */
      SAL_Create_Thread(
         Remote_Sender_Test,   
         (void*)RECEIVER_3, 
         Init_Thread_Attr("RSndT3", SENDER_3, priority, &thread_attr));

      /* Control thread of sender3 */
      SAL_Create_Thread(
         Remote_Sender_Driver, 
         NULL, 
         Init_Thread_Attr("RSndD3", S_CTRL_3, priority, &thread_attr));

      /* Sender4 thread */
      SAL_Create_Thread(
         Remote_Sender_Test,   
         (void*)RECEIVER_4, 
         Init_Thread_Attr("RSndT4", SENDER_4, priority, &thread_attr));

      /* Control thread of sender4 */
      SAL_Create_Thread(
         Remote_Sender_Driver, 
         NULL, 
         Init_Thread_Attr("RSndD4", S_CTRL_4, priority, &thread_attr));

      break;

      /* remote communication - receiver */
   case E_RECEIVER:

      /* Receiver1 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test, 
         NULL, 
         Init_Thread_Attr("RRecT1", RECEIVER_1, priority, &thread_attr));

      /* Control thread of receiver1 */
      SAL_Create_Thread(
         Remote_Receiver_Driver, 
         NULL, 
         Init_Thread_Attr("RRecD1", R_CTRL_1, priority, &thread_attr));

      /* Receiver2 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test,   
         (void*)4, 
         Init_Thread_Attr("RRecT2", RECEIVER_2, priority, &thread_attr));

      /* Control thread of receiver2 */
      SAL_Create_Thread(
         Remote_Receiver_Driver, 
         NULL, 
         Init_Thread_Attr("RRecD2", R_CTRL_2, priority, &thread_attr));

      /* Receiver3 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test,   
         (void*)5, 
         Init_Thread_Attr("RRecT3", RECEIVER_3, priority, &thread_attr));

      /* Control thread of receiver3 */
      SAL_Create_Thread(
         Remote_Receiver_Driver, 
         NULL, 
         Init_Thread_Attr("RRecD3", R_CTRL_3, priority, &thread_attr));

      /* Receiver4 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test,   
         (void*)512, 
         Init_Thread_Attr("RRecT4", RECEIVER_4, priority, &thread_attr));

      /* Control thread of receiver4 */
      SAL_Create_Thread(
         Remote_Receiver_Driver, 
         NULL, 
         Init_Thread_Attr("RRecD4", R_CTRL_4, priority, &thread_attr));

      break;
#endif

   default:
      /* local communication */

      /* Receiver1 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         NULL, 
         Init_Thread_Attr("LRecD1", RECEIVER_1, priority, &thread_attr));

      /* Sender1 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_1, 
         Init_Thread_Attr("LSndD1", SENDER_1, priority, &thread_attr));

      /* Receiver2 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         (void*)4, 
         Init_Thread_Attr("LRecD2", RECEIVER_2, priority, &thread_attr));

      /* Sender2 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_2, 
         Init_Thread_Attr("LSndD2", SENDER_2, priority, &thread_attr));
      
      /* Receiver3 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         (void*)5, 
         Init_Thread_Attr("LRecD3", RECEIVER_3, priority, &thread_attr));

      /* Sender3 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_3, 
         Init_Thread_Attr("LSndD3", SENDER_3, priority, &thread_attr));

      /* Receiver4 thread */
      SAL_Create_Thread(
         Local_Receiver_Test,   
         (void*)512, 
         Init_Thread_Attr("LRecD4", RECEIVER_4, priority, &thread_attr));

      /* Sender4 thread */
      SAL_Create_Thread(
         Local_Sender_Test,   
         (void*)RECEIVER_4, 
         Init_Thread_Attr("LSndD4", SENDER_4, priority, &thread_attr));

   }

   retcode = SAL_Run();

#if defined (XSAL)
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
#else
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Timeout))
   {
      Tr_Fault("Timeout has been reached");
      retcode = 1;
   }
#endif

   if (retcode != 0)
   {
      Tr_Fault_1("Send_Test: SAL_Run() return %d", (int)retcode);
      return 1;
   }

   return 0;
}

/** \} 
 */


/**  \name Time_Send_Test
 *
 *   \{
 */

/**  Function of SENDER_1.
 *
 *   - sends message with timestamp
 */

void Sender_Fun(void* param)
{
   Sample_Msg_T data;
   SAL_Thread_Id_T wait_list[] = { RECEIVER_1 };

   Tr_Info_Lo("Sender_Fun: Start");

#if defined (XSAL)
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));
#else
   
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   data.field1 = 123;
   data.field2 = 456;

   ftime(&Time);
 
   if (!SAL_Send(APP_TEST_LOCAL, RECEIVER_1, EV_E6, &data, sizeof(Sample_Msg_T)))
   {
      PBC_Failed("SAL_Send: The message has not been sent");
   }

   Tr_Info_Lo("Sender_Fun: Stop");

}



/**  Function of RECEIVER_1.
 *
 *   - receives message with timestamp and checks if the time is correct
 */

void Receiver_Fun(void* param)
{
   const SAL_Message_T* msg;

   Tr_Info_Lo("Receiver_Fun: Start");

   SAL_Create_Queue(1, sizeof(Sample_Msg_T), malloc, free);

   SAL_Signal_Ready();

#if defined (XSAL)
   msg = SAL_Receive();
#else
   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      PBC_Failed("SAL_Receive_Timeout: timeout has been reached");
   }
#endif

   if (((msg->timestamp_sec - Time.time)*1000 + msg->timestamp_msec - Time.millitm) > 10)
   {
      PBC_Failed("SAL_Send - SAL_Receive: Invalid time in header components");
   }

   Tr_Info_Lo("Receiver_Fun: Stop");

}

/**  Test of timestamp.
 *
 *   Function creates two threads: SENDER_1 and RECEIVER_1 and starts SAL_Run.
 */

SAL_Int_T Time_Send_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Sender_Fun, 
      NULL, 
      Init_Thread_Attr("SENDER", SENDER_1, priority, &thread_attr));

   SAL_Create_Thread(
      Receiver_Fun, 
      NULL, 
      Init_Thread_Attr("REC_T", RECEIVER_1, priority, &thread_attr));

   return SAL_Run();
}

/** \}
 */



SAL_Int_T Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_Int_T i;
   SAL_Int_T option;
   SAL_Config_T config;
   SAL_Mutex_Attr_T mutex_attr;
   SAL_Semaphore_Attr_T sem_attr;

   while((option = pi_getopt(argc, argv, "srv:g:l:teN:")) != -1)
   {        
      switch(option) 
      {             

#if defined (XSAL)
         case 's':
            Test_Type = E_SENDER;
            break;

         case 'r':
            Test_Type = E_RECEIVER;
            break;
#endif

         default:
            break;
      }     
   }

   SAL_Get_Config(&config);

   switch(Test_Type)
   {

#if defined (XSAL)
   case E_SENDER:

      config.app_id = APP_TEST_SENDER;
      config.max_number_of_threads = LAST_THREAD_ID;

      if (!SAL_Init(&config))
      {
         Tr_Fault("SAL_Init error");
         return 1;
      }

      break;

   case E_RECEIVER:

      config.app_id = APP_TEST_RECEIVER;
      config.max_number_of_threads = LAST_THREAD_ID;

      if (!SAL_Init(&config))
      {
         Tr_Fault("SAL_Init error");
         return 1;
      }

      break;
#endif

   default:

      config.app_id = APP_TEST_LOCAL;
      config.max_number_of_threads = LAST_THREAD_ID + 2;

      if (!SAL_Init(&config))
      {
         Tr_Fault("SAL_Init error");
         return 1;
      }

   }

   SAL_Init_Mutex_Attr(&mutex_attr);

   if (!SAL_Create_Mutex(&Queue_Dump_Mutex, &mutex_attr))
   {
      return 1;
   }

   SAL_Init_Semaphore_Attr(&sem_attr);

   for(i = 1; i <= LAST_THREAD_ID; i++)
   {
      if (!SAL_Create_Semaphore(&Sem_Rec[i], &sem_attr) || 
         !SAL_Create_Semaphore(&Sem_Send[i], &sem_attr))
      {
         return 1;
      }
   }
   return 0;
}


SAL_Int_T Tear_Down()
{
   SAL_Int_T i;

   if (!SAL_Destroy_Mutex(&Queue_Dump_Mutex))
   {
      return 1;
   }

   for(i = 1; i <= LAST_THREAD_ID; i++)
   {
      if (!SAL_Destroy_Semaphore(&Sem_Rec[i]) || 
         !SAL_Destroy_Semaphore(&Sem_Send[i]))
      {
         return 1;
      }
   }

   return 0;
}


int main (int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up(argc, argv))
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   ret = ret || Send_Test_Suite();

   /* test of timestamp, only locally */
   if (Test_Type == E_STAND_ALONE)
   {
      ret = ret || Time_Send_Test();
   }

   if (Tear_Down())
   {
      Tr_Fault("Tear_Down failed");
      return 1;
   }
   
   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));
   
   return ret;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  01 Sep 2006, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Initial revision
 *
 * -----------------------------------------------------------------------*/
