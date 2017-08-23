/* -------------------------------------------------------------------------
 *
 * File:        test_publish.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of publish communications.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#include "pbc_trace.h"
#include "xsal.h"
#include "pi_unistd.h"

EM_FILENUM(TEST_MODULE_ID, 3);

/* application ids */
#define APP_TEST_LOCAL    2
#define APP_TEST_PUBLISHER   2
#define APP_TEST_RECEIVER 3

/* control thread numbers */
#define REC_CONTROL 1
#define PUB_CONTROL 6
#define RECEIVER_THREAD 1
#define PUBLISHER_THREAD 2

#define NUMBER_OF_RECEIVERS 4

extern const char *SAL_Get_Thread_Name_Self(void);

/* event's numbers */
typedef enum Event_Id_Tag
{
   EV_E9 = 9,
   EV_REC_PASS,
   EV_ERROR
} Event_Id_T;

typedef enum Thread_Id_Tag {
   PUBLISHER = 1,
   RECEIVER_2, 
   RECEIVER_3, 
   RECEIVER_4,
   RECEIVER_5,
} Thread_Id_T;

typedef enum E_Test_Type_Tag
{ 
   E_STAND_ALONE, 
   E_PUBLISHER, 
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

SAL_Semaphore_T Sem_Rec     [2 + 2*NUMBER_OF_RECEIVERS];
SAL_Semaphore_T Sem_Publish [2 + 2*NUMBER_OF_RECEIVERS];

E_Test_Type_T Test_Type = E_STAND_ALONE;

SAL_Mutex_T Queue_Dump_Mutex;

SAL_Int_T Timeout = 3000;


/*
 *  Test 1:
 *  
 *  Events with events id from 1 to 8 are published.
 *
 *  Only normal (not urgent) events without data
 */

const Ev_Test_Case_Data_T Ev_Publish_TC_1[] =
{
   { 1, 0, NULL }, 
   { 2, 0, NULL }, 

   { 3, 0, NULL }, 
   { 4, 0, NULL }, 
   { 5, 0, NULL }, 
   { 6, 0, NULL }, 
   { 7, 0, NULL }, 
   { 8, 0, NULL },
};

const SAL_Event_Id_T Ev_Subscribe_TC_1_1[] = { 1, 3, 5, 7 };
const Ev_Test_Case_Data_T Ev_Receive_TC_1_1[] =
{
   { 1, 0, NULL },

   { 3, 0, NULL },
   { 5, 0, NULL },
   { 7, 0, NULL },
};

const SAL_Event_Id_T Ev_Subscribe_TC_1_2[] = { 2, 4, 6, 8 };
const Ev_Test_Case_Data_T Ev_Receive_TC_1_2[] =
{
   { 2, 0, NULL },

   { 4, 0, NULL },
   { 6, 0, NULL },
   { 8, 0, NULL },
};

#if defined (XSAL)

/*
 *
 *  Test Case 2:
 *
 *  Normal and urgent events without data
 */
const Ev_Test_Case_Data_T Ev_Publish_TC_2[] =
{
   { 1, 0, NULL }, 
   { 2, 0, NULL }, 
   { 3, 0, NULL }, 
   { 4, 0, NULL }, 
   { 5, 0, NULL }, 
   { 6, 0, NULL }, 
   { 7, 0, NULL }, 
   { 8, 0, NULL }
};

const SAL_Event_Id_T Ev_Subscribe_TC_2_1[] = { 2, 4, 6, 8 };
const SAL_Event_Id_T Ev_Urgent_List_TC_2_1[] = { 8, 6 };
const Ev_Test_Case_Data_T Ev_Receive_TC_2_1[] =
{
   { 6, 0, NULL }, 
   { 8, 0, NULL },
   { 2, 0, NULL }, 
   { 4, 0, NULL } 
};


const SAL_Event_Id_T Ev_Subscribe_TC_2_2[] = { 1, 3, 5, 7 };
const SAL_Event_Id_T Ev_Urgent_List_TC_2_2[] = { 7, 5 };
const Ev_Test_Case_Data_T Ev_Receive_TC_2_2[] =
{
   { 5, 0, NULL }, 
   { 7, 0, NULL },
   { 1, 0, NULL }, 
   { 3, 0, NULL } 
};

#endif

/*
 *  Test Case 3:
 *
 *  Only normal (not urgent) events with data
 */
const Ev_Test_Case_Data_T Ev_Publish_TC_3[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 5, 1,  "" },
   { 6, 5,  "1234" },
   { 7, 9,  "12345678" },
   { 8, 17, "1234567890123456" }
};

const SAL_Event_Id_T Ev_Subscribe_TC_3_1[] = { 7, 5, 3, 1 };
const Ev_Test_Case_Data_T Ev_Receive_TC_3_1[] =
{
   { 1, 1,  "" },
   { 3, 9,  "12345678" },
   { 5, 1,  "" },
   { 7, 9,  "12345678" }
};


const SAL_Event_Id_T Ev_Subscribe_TC_3_2[] = { 8, 6, 4, 2 };
const Ev_Test_Case_Data_T Ev_Receive_TC_3_2[] =
{
   { 2, 5,  "1234" },
   { 4, 17, "1234567890123456" },
   { 6, 5,  "1234" },
   { 8, 17, "1234567890123456" }
};


#if defined (XSAL)

/*
 *  Test Case 4:
 *
 *  Normal and urgent events with data
 */
const Ev_Test_Case_Data_T Ev_Publish_TC_4[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 5, 1,  "" },
   { 6, 5,  "1234" },
   { 7, 9,  "12345678" },
   { 8, 17, "1234567890123456" }
};

const SAL_Event_Id_T Ev_Subscribe_TC_4_1[] = { 1, 5, 6, 7 };
const SAL_Event_Id_T Ev_Urgent_List_TC_4_1[] = { 1, 7 };
const Ev_Test_Case_Data_T Ev_Receive_TC_4_1[] =
{
   { 1, 1,  "" }, 
   { 7, 9,  "12345678" }, 
   { 5, 1,  "" }, 
   { 6, 5,  "1234" }
};


const SAL_Event_Id_T Ev_Subscribe_TC_4_2[] = { 2, 3, 4, 5 };
const SAL_Event_Id_T Ev_Urgent_List_TC_4_2[] = { 2, 3 };
const Ev_Test_Case_Data_T Ev_Receive_TC_4_2[] =
{
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 5, 1,  "" },
};

#endif

/*
 *  Test Case 5:
 *
 *  Only normal (not urgent) events without data.
 *  Subscribes on more events then size of message queue.
 */
const Ev_Test_Case_Data_T Ev_Publish_TC_5[] =
{
   { 1, 1,  "" },
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 5, 1,  "" },
   { 6, 5,  "1234" },
   { 7, 9,  "12345678" },
   { 8, 17, "1234567890123456" }
};

const SAL_Event_Id_T Ev_Subscribe_TC_5_1[] = { 1, 5, 6, 7, 8 };
const Ev_Test_Case_Data_T Ev_Receive_TC_5_1[] =
{
   { 1, 1,  "" },
   { 5, 1,  "" },
   { 6, 5,  "1234" },
   { 7, 9,  "12345678" }
};

const SAL_Event_Id_T Ev_Subscribe_TC_5_2[] = { 2, 3, 4, 7, 8 };
const Ev_Test_Case_Data_T Ev_Receive_TC_5_2[] =
{
   { 2, 5,  "1234" },
   { 3, 9,  "12345678" },
   { 4, 17, "1234567890123456" },
   { 7, 9,  "12345678" }
};

#if defined (XSAL)

/*
 *  Test Case 6:
 *
 *  Normal and urgent events with data.
 *  Subscribes on more events then size of message queue.
 */
const Ev_Test_Case_Data_T Ev_Publish_TC_6[] =
{
   { 8, 17, "1234567890123456" }, 
   { 7, 9,  "12345678" }, 
   { 6, 5,  "1234" },
   { 5, 1,  "" },
   { 4, 17, "1234567890123456" },
   { 3, 9,  "12345678" },
   { 2, 5,  "1234" },
   { 1, 1,  "" },
};

const SAL_Event_Id_T Ev_Subscribe_TC_6_1[] = { 1, 5, 6, 7, 8 };
const SAL_Event_Id_T Ev_Urgent_List_TC_6_1[] = { 1, 5, 6 };
const Ev_Test_Case_Data_T Ev_Receive_TC_6_1[] =
{
   { 6, 5,  "1234" },
   { 5, 1,  "" },
   { 1, 1,  "" },
   { 8, 17, "1234567890123456" }
};


const SAL_Event_Id_T Ev_Subscribe_TC_6_2[] = { 1, 2, 5, 7, 8 };
const SAL_Event_Id_T Ev_Urgent_List_TC_6_2[] = { 1, 5, 8 };
const Ev_Test_Case_Data_T Ev_Receive_TC_6_2[] =
{

   { 8, 17, "1234567890123456" }, 
   { 5, 1,  "" },
   { 1, 1,  "" },
   { 7, 9,  "12345678" }
};

#endif

/** 
 *   \file
 *
 *   Test of local and remote communication using SAL_Publish, SAL_Subscribe, SAL_Unsubscribe 
 *   functions and urgent messages.   
 *
 *   \section T Tests: \n
 *   <OL>
 *   <LI> \link PTS Publish_Test_Suite \endlink - test of local and remote communication using 
 *      SAL_Publish, SAL_Subscribe, SAL_Unsubscribe functions and urgent messages. </LI>
 *   <LI> \link TPT Time_Publish_Test \endlink - test of timestamps. </LI>
 *   </OL>
 *
 *   \page PTS Publish_Test_Suite
 *
 *   \section CBS Can be started:
 *   - as one local process, without entry parameters
 *   - as two remote processes: publisher and receiver, with entry parameters suitably -s and -r,
 *     receiver thread must be started as the first.
 *
 *   \section FBP For both (local and remote) processes:
 *   \subsection T2 Threads:
 *   There is one publisher thread (PUBLISHER) and four receiver threads (RECEIVER_\a n, 
 *   where \a n is in range from 2 to 5).
 *   \subsection TC0 Test cases:
 *   Publisher publishes the lists of messages. There are list of messages (test cases with
 *   numbers \a p where \a p is in range from 1 to 6), with different event_ids, data_sizes 
 *   and data. \n
 *   For each list of messages, two receiver threads subscribe on the subset of messages set
 *   (test case number \a p_1) and two other receiver threads subscribe on the other subset 
 *   of messages set (test case number \a p_2).
 *   \subsection S Semaphores:
 *   There are sixteen semaphores used in this test. Eight of them are used for local 
 *   communication and eight for remote communication.
 *   - \b Sem_Rec semaphores control if publisher publishes the messages when receivers are ready
 *     (the queue is created, the thread is subscribed for events etc)
 *   - \b Sem_Publish semaphores control if receivers take the messages out of the queues 
 *     after publisher has published all messages from the list. \n
 *
 *   Array indexes of the semaphores for local communication are in range from 2 to 5 
 *   (are equal receiver thread ids). For the remote communication array indexes of the 
 *   semaphore are in range from 6 to 9.
 *
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> PUBLISHER waits until receivers are ready (Sem_Rec[\a n]). </LI>
 *   <LI> Receivers (RECEIVER_\a n, where \a n is in range from 2 to 5) create queues. </LI>
 *   <LI> Receivers subscribe on \b SUB events. </LI>
 *   <LI> If there are urgent messages in test case, receivers declare \b URG urgent messages. </LI>
 *   <LI> Receivers signal that are ready (Sem_Rec[\a n]). </LI>
 *   <LI> Receivers wait until PUBLISHER publishes \b PUB_MSG messages (Sem_Publish[\a n]. </LI>
 *   <LI> PUBLISHER publishes \b PUB_MSG messages. </LI>
 *   <LI> PUBLISHER signals that \b PUB_MSG messages have been published (Sem_Publish[\a n], where
 *      \a n is in range from 2 to 5). </LI>
 *   <LI> Receivers check if the number of \b REC_MSG messages in the queues is valid. </LI>
 *   <LI> Receivers receive \b REC_MSG messages and check if all header components (besides timestamp)
 *      and data are valid. </LI>
 *   <LI> Receivers check if message queue is empty after taking out all \b REC_MSG messages. </LI>
 *   </OL>
 *   
 *   \section LP Local_Process
 *
 *   There are one publisher and four receivers as described above.
 *   
 *  
 *   \section RP Remote_Process
 *
 *   \subsection RPRP - in Receiver Process:
 *
 *   There are four receiver threads (RECEIVER_\a n, where \a n is in range from 2 to 5
 *   and is equal receiver id) and one control thread (REC_CONTROL, its id is equal 1).
 *
 *   \subsection RPPP - in Publisher Process:
 *
 *   There are one publisher thread (PUBLISHER, its id is equal 1), four local receiver 
 *   threads (RECEIVER_\a n, where \a n is in range from 2 to 5 and is equal receiver id) 
 *   and one control thread (PUB_CONTROL, its id is equal 6). \n
 *   Control thread of receiver process is used to communicate with control thread of publisher
 *   process. This communication enables semaphore synchronization between threads of different
 *   processes. \n
 *   Control threads include semaphores only for remote communication.
 * 
 *
 *  \section TC Test Cases
 *
 *  <TABLE>
 *  <TR><TD><B><CENTER>NO</TD><TD>PUB_MSG</TD>
 *      <TD>REC1\n URG</TD><TD>REC1\n SUB  </TD><TD>REC1 - REC_MSG</TD>
 *      <TD>REC2\n URG</TD><TD>REC2\n SUB  </TD><TD>REC2 - REC_MSG</TD>
 *      <TD>XSAL only</CENTER></B></TD></TR>
 *
 *  <TR><TD>1</TD>
 *      <TD>{ 1, 0, NULL }\n{ 2, 0, NULL }\n{ 3, 0, NULL }\n{ 4, 0, NULL }\n
 *          { 5, 0, NULL }\n{ 6, 0, NULL }\n{ 7, 0, NULL }\n{ 8, 0, NULL }</TD>
 *      <TD>-</TD><TD>1\n 3\n 5\n 7</TD>
 *      <TD>{ 1, 0, NULL }\n{ 3, 0, NULL }\n{ 5, 0, NULL }\n{ 7, 0, NULL }</TD>
 *      <TD>-</TD><TD>2\n 4\n 6\n 8</TD>
 *      <TD>{ 2, 0, NULL }\n{ 4, 0, NULL }\n{ 6, 0, NULL }\n{ 8, 0, NULL }</TD>
 *      <TD>-</TD></TR>
 *
 *  <TR><TD>2</TD>
 *      <TD>{ 1, 0, NULL }\n{ 2, 0, NULL }\n{ 3, 0, NULL }\n{ 4, 0, NULL }\n
 *          { 5, 0, NULL }\n{ 6, 0, NULL }\n{ 7, 0, NULL }\n{ 8, 0, NULL }</TD>
 *      <TD>8\n 6</TD><TD>2\n 4\n 6\n 8</TD>
 *      <TD>{ 6, 0, NULL }\n{ 8, 0, NULL }\n{ 2, 0, NULL }\n{ 4, 0, NULL }</TD>
 *      <TD>7\n 5</TD><TD>1\n 3\n 5\n 7</TD>
 *      <TD>{ 5, 0, NULL }\n{ 7, 0, NULL }\n{ 1, 0, NULL }\n{ 3, 0, NULL }</TD>
 *      <TD>yes</TD></TR>
 *
 *  <TR><TD>3</TD>
 *      <TD>{ 1, 1, "" }\n{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }\n
 *          { 5, 1, "" }\n{ 6, 5, "1234" }\n{ 7, 9, "12345678" }\n{ 8, 17, "1234567890123456" }</TD>
 *      <TD>-</TD><TD>7\n 5\n 3\n 1</TD>
 *      <TD>{ 1, 0, NULL }\n{ 3, 0, NULL }\n{ 5, 0, NULL }\n{ 7, 0, NULL }</TD>
 *      <TD>-</TD><TD>8\n 6\n 4\n 2</TD>
 *      <TD>{ 2, 0, NULL }\n{ 4, 0, NULL }\n{ 6, 0, NULL }\n{ 8, 0, NULL }</TD>
 *      <TD>-</TD></TR>
 *
 *  <TR><TD>4</TD>
 *      <TD>{ 1, 1, "" }\n{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }\n
 *          { 5, 1, "" }\n{ 6, 5, "1234" }\n{ 7, 9, "12345678" }\n{ 8, 17, "1234567890123456" }</TD>
 *      <TD>1\n 7</TD><TD>1\n 5\n 6\n 7</TD>
 *      <TD>{ 1, 1, "" }\n{ 7, 9, "12345678" }\n{ 5, 1, "" }\n{ 6, 5, "1234" }</TD>
 *      <TD>2\n 3</TD><TD>2\n 3\n 4\n 5</TD>
 *      <TD>{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }\n{ 5, 1, "" }</TD>
 *      <TD>yes</TD></TR>
 *
 *  <TR><TD>5</TD>
 *      <TD>{ 1, 1, "" }\n{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }\n
 *          { 5, 1, "" }\n{ 6, 5, "1234" }\n{ 7, 9, "12345678" }\n{ 8, 17, "1234567890123456" }</TD>
 *      <TD>-</TD><TD>1\n 5\n 6\n 7\n 8</TD>
 *      <TD>{ 1, 1, "" }\n{ 5, 1, "" }\n{ 6, 5, "1234" }\n{ 7, 9, "12345678" }\n</TD>
 *      <TD>-</TD><TD>2\n 3\n 4\n 7\n 8</TD>
 *      <TD>{ 2, 5, "1234" }\n{ 3, 9, "12345678" }\n{ 4, 17, "1234567890123456" }\n{ 7, 9, "12345678" }</TD>
 *      <TD>-</TD></TR>
 *      
 *  <TR><TD>6</TD>
 *      <TD>{ 8, 17, "1234567890123456" }\n{ 7, 9,  "12345678" }\n{ 6, 5,  "1234" }\n{ 5, 1,  "" }\n
 *          { 4, 17, "1234567890123456" }\n{ 3, 9,  "12345678" }\n{ 2, 5,  "1234" }\n{ 1, 1,  "" }</TD>
 *      <TD>1\n 5\n 6</TD><TD>1\n 5\n 6\n 7\n 8</TD>
 *      <TD>{ 6, 5,  "1234" }\n{ 5, 1,  "" }\n{ 1, 1,  "" }\n{ 8, 17, "1234567890123456" }</TD>
 *      <TD>1\n 5\n 6</TD><TD>1\n 2\n 5\n 7\n 8</TD>
 *      <TD>{ 8, 17, "1234567890123456" }\n{ 5, 1,  "" }\n{ 1, 1,  "" }\n{ 7, 9, "12345678" }</TD>
 *      <TD>yes</TD></TR>
 *
 *  </TABLE>
 *
 *  where:
 *  - \b NO      - test case number
 *  - \b PUB_MSG - list of messages published by publisher in test case
 *  - \b REC1    - group of receivers (RECEIVER_2 and RECEIVER 4) which receives events from 
 *  test case number \a p_1
 *  - \b REC2    - group of receivers (RECEIVER_3 and RECEIVER_5) which receives events from 
 *  test case number \a p_2
 *  - \b URG     - events defined as urgent
 *  - \b SUB     - subscribed events
 *  - \b REC_MSG - expected received messages
 *
 *  \b Test \b cases \b description: \n
 *  \b 1 - Only normal (not urgent) events without data. \n
 *  \b 2 - Normal and urgent events without data. \n
 *  \b 3 - Only normal (not urgent) events with data. \n
 *  \b 4 - Normal and urgent events with data. \n
 *  \b 5 - Only normal (not urgent) events without data. \n
 *      Subscribes on more events then size of message queue. \n
 *  \b 6 - Normal and urgent events with data. \n
 *      Subscribes on more events then size of message queue. 

 */
/**  \page TPT Time_Publish_Test
 *
 *   Test of timestamps for SAL_Publish function.
 *
 *   Two threads are created: PUBLISHER_THREAD and RECEIVER_THREAD.
 *
 *   \section TP Test procedure:
 *
 *   <OL>
 *   <LI> RECEIVER_THREAD creates the queue. </LI>
 *   <LI> RECEIVER_THREAD subscribes on the event. </LI>
 *   <LI> PUBLISHER_THREAD gets system time. </LI>
 *   <LI> PUBLISHER_THREAD publishes the message. </LI>
 *   <LI> RECEIVER_THREAD receives the message. </LI>
 *   <LI> RECEIVER_THREAD checks if timestamp is correct. </LI>
 *   <LI> RECEIVER_THREAD unsubscribes from the event. </LI>
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



/**  \name Publish_Test_Suite
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

/**  Auxiliary function of receiver, writes out event_ids and data_size of all messages in the queue
 *   (<B><I>XSAL only</I></B>).
 */

void Dump_Queue(const char* pref)
{
   SAL_Queue_Iterator_T iter;
   SAL_Thread_Id_T tid = SAL_Get_Thread_Id();

   SAL_First_Message(&iter);

   SAL_Lock_Mutex(&Queue_Dump_Mutex);

   Tr_Fault_1("Messages in the queue: %s", pref);
   while(!SAL_Is_Queue_End(&iter))
   {
      const SAL_Message_T* msg = SAL_Get_Message(&iter);
      Tr_Fault_3(" [tid=%d, event_id %d, data_size=%d]", (int)tid, (int)msg->event_id, (int)msg->data_size);
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
   const SAL_Message_T* msg;
   
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

#if defined (XSAL)
         msg = SAL_Receive();
#else
         msg = SAL_Receive_Timeout(Timeout);

         if (msg == NULL)
         {
            PBC_Failed("SAL_Receive_Timeout: message has not been received");
         }        
#endif

         Tr_Info_Mid_4("Ev %d, Size %d, From(%d, %d)",
                       (int)msg->event_id, (int)msg->data_size, (int)msg->sender_app_id, (int)msg->sender_thread_id);

         if (msg->event_id != event_list[i].event_id  ||
           msg->data_size != event_list[i].data_size ||
           memcmp(msg->data, event_list[i].data, msg->data_size) != 0 ||
           msg->receiver_thread_id != 0 ||
           msg->sender_app_id != APP_TEST_PUBLISHER ||
           msg->sender_thread_id != 1)

         {
            SAL_Stat_Queue_T stat;

            Tr_Fault_2("Check_Events: expected %d, received %d", (int)event_list[i].event_id, (int)msg->event_id);

            SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);
            Dump_Queue_Stat(SAL_Get_Thread_Id(), &stat);

#if defined (XSAL)
            Dump_Queue("Error");
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

SAL_Int_T Receive_Test(char* s, 
                       const Ev_Test_Case_Data_T* event_list, 
                       size_t event_list_size,
                       const SAL_Event_Id_T* subscribe_list, 
                       size_t subscribe_list_size
#if defined (XSAL)
                       ,
                       const SAL_Event_Id_T* urgent_list, 
                       size_t urgent_list_size
#endif
                       )
{
   SAL_Thread_Id_T thread_id = SAL_Get_Thread_Id();
   SAL_Int_T ret;

   Tr_Info_Hi_2("Receive_Test %s[%s]: Start", s, SAL_Get_Thread_Name_Self());

   SAL_Subscribe(subscribe_list, subscribe_list_size);

#if defined (XSAL)
   if (urgent_list_size > 0)
      SAL_Declare_Urgent(urgent_list, urgent_list_size);
#endif

   /* LOCAL RECEIVER */
   if (SAL_Get_App_Id() != APP_TEST_RECEIVER)
   {
      /* receiver is ready */
      SAL_Signal_Semaphore(&Sem_Rec[thread_id]);


      /* wait until publisher publishes messages */
#if defined (XSAL) 
      SAL_Wait_Semaphore(&Sem_Publish[thread_id]);
#else
      if (!SAL_Wait_Semaphore_Timeout(&Sem_Publish[thread_id], Timeout))
      {
         PBC_Failed("Timeout has been reached");
      }
#endif

   }
   /* REMOTE RECEIVER */
   else
   {
      /* receiver is ready */
      SAL_Signal_Semaphore(&Sem_Rec[thread_id + NUMBER_OF_RECEIVERS]);

      /* wait until publisher publishes messages */
#if defined (XSAL)
      SAL_Wait_Semaphore(&Sem_Publish[thread_id + NUMBER_OF_RECEIVERS]);
#else
      if (!SAL_Wait_Semaphore_Timeout(&Sem_Publish[thread_id + NUMBER_OF_RECEIVERS], Timeout))
      {
         PBC_Failed("Timeout has been reached");
      }
#endif
   }

   ret = Check_Events(event_list, event_list_size);

#if defined (XSAL)
   if (urgent_list_size > 0)
   {
      SAL_Undeclare_Urgent(urgent_list, urgent_list_size);
   }
#endif

   SAL_Unsubscribe(subscribe_list, subscribe_list_size);

   /* REMOTE RECEIVER: receiver sends the info about passing the test
      to the control thread */
   if (Test_Type == E_RECEIVER)
   {
      SAL_Send(SAL_Get_App_Id(), REC_CONTROL, EV_REC_PASS, (void*)&ret, sizeof(SAL_Int_T));
   }

   Tr_Info_Hi_3("Receive_Test%s[%s]: Stop, ret = %d", s, SAL_Get_Thread_Name_Self(), (int)ret);

   return ret;
}


/**  Function of publisher.
 *
 *   - publishes list of messages given as an entry parameter
 */

void Publish_Events(const Ev_Test_Case_Data_T* event_list, size_t event_list_size)
{
   size_t i;

   for(i = 0; i < event_list_size; i++)
   {
      SAL_Publish(event_list[i].event_id, event_list[i].data, event_list[i].data_size);
   }

}


/**  Function of publisher, test case.
 *
 *   - uses semaphores for communication synchronization
 *   - publishes the list of messages
 */

SAL_Int_T Publish_Test(SAL_Int_T j, 
                       const Ev_Test_Case_Data_T* event_list, 
                       size_t event_list_size)
{
   SAL_Int_T i;


   /* wait until receivers will be ready */

   /* LOCAL RECEIVERS */
   for(i = 2; i < 2 + NUMBER_OF_RECEIVERS; i++)
   {
#if defined (XSAL)
         SAL_Wait_Semaphore(&Sem_Rec[i]);
#else
         if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[i], Timeout))
         {
            PBC_Failed("Timeout has been reached");
         }
#endif
   }
  
   /* REMOTE RECEIVERS */
   if (Test_Type != E_STAND_ALONE)
   {
      for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)
      {
#if defined (XSAL)
         SAL_Wait_Semaphore(&Sem_Rec[i]);
#else
         if (!SAL_Wait_Semaphore_Timeout(&Sem_Rec[i], Timeout))
         {
            PBC_Failed("Timeout has been reached");
         }
#endif
      }
   }

   /* publishes test messages */
   Publish_Events(event_list, event_list_size);

   /* signal that all messages were published */

   /* LOCAL RECEIVERS */
   for(i = 2; i < 2 + NUMBER_OF_RECEIVERS; i++)
   {
      SAL_Signal_Semaphore(&Sem_Publish[i]);
   }

   /* REMOTE RECEIVERS */
   if (Test_Type != E_STAND_ALONE)
   {
      for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)
      {
         SAL_Signal_Semaphore(&Sem_Publish[i]);         
      }
   }

   Tr_Info_Hi_1("Publish_Test %d Ok", (int)j);

   return 0;
}



/*  ***************************************************************
 *
 *   Test communications between threads inside the same process
 *
 *   
 *
 *  ***************************************************************/


/**  Function of RECEIVER_\a n for local receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Local_Receiver_Test_1(void* param)
{
   size_t message_size = (size_t)param;
   SAL_Int_T ret = 0;

   Tr_Info_Lo("Local_Receiver_Test_1: Start");

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

#if defined (XSAL)
   ret = ret || Receive_Test("1_1", Ev_Receive_TC_1_1, sizeof(Ev_Receive_TC_1_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_1, sizeof(Ev_Subscribe_TC_1_1)/sizeof(SAL_Event_Id_T), NULL, 0);
#else
   ret = ret || Receive_Test("1_1", Ev_Receive_TC_1_1, sizeof(Ev_Receive_TC_1_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_1, sizeof(Ev_Subscribe_TC_1_1)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("2_1", Ev_Receive_TC_2_1, sizeof(Ev_Receive_TC_2_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_2_1, sizeof(Ev_Subscribe_TC_2_1)/sizeof(SAL_Event_Id_T),
         Ev_Urgent_List_TC_2_1, sizeof(Ev_Urgent_List_TC_2_1)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("3_1", Ev_Receive_TC_3_1, sizeof(Ev_Receive_TC_3_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_1, sizeof(Ev_Subscribe_TC_3_1)/sizeof(SAL_Event_Id_T), NULL, 0);
#else
   ret = ret || Receive_Test("3_1", Ev_Receive_TC_3_1, sizeof(Ev_Receive_TC_3_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_1, sizeof(Ev_Subscribe_TC_3_1)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("4_1", Ev_Receive_TC_4_1, sizeof(Ev_Receive_TC_4_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_4_1, sizeof(Ev_Subscribe_TC_4_1)/sizeof(SAL_Event_Id_T), 
         Ev_Urgent_List_TC_4_1, sizeof(Ev_Urgent_List_TC_4_1)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("5_1", Ev_Receive_TC_5_1, sizeof(Ev_Receive_TC_5_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_1, sizeof(Ev_Subscribe_TC_5_1)/sizeof(SAL_Event_Id_T), NULL, 0);
#else
   ret = ret || Receive_Test("5_1", Ev_Receive_TC_5_1, sizeof(Ev_Receive_TC_5_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_1, sizeof(Ev_Subscribe_TC_5_1)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("6_1", Ev_Receive_TC_6_1, sizeof(Ev_Receive_TC_6_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_6_1, sizeof(Ev_Subscribe_TC_6_1)/sizeof(SAL_Event_Id_T), 
         Ev_Urgent_List_TC_6_1, sizeof(Ev_Urgent_List_TC_6_1)/sizeof(SAL_Event_Id_T));
#endif

   if (ret)
   {
       SAL_Exit(1); 
   }

   Tr_Info_Lo("Local_Receiver_Test_1: End");
}


/**   Function of RECEIVER_\a n for local receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Local_Receiver_Test_2(void* param)
{
   size_t message_size = (size_t)param;
   SAL_Int_T ret = 0;

   Tr_Info_Lo("Local_Receiver_Test_2: Start");

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

#if defined (XSAL)
   ret = ret || Receive_Test("1_2", Ev_Receive_TC_1_2, sizeof(Ev_Receive_TC_1_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_2, sizeof(Ev_Subscribe_TC_1_2)/sizeof(SAL_Event_Id_T), NULL, 0);
#else
   ret = ret || Receive_Test("1_2", Ev_Receive_TC_1_2, sizeof(Ev_Receive_TC_1_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_2, sizeof(Ev_Subscribe_TC_1_2)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("2_2", Ev_Receive_TC_2_2, sizeof(Ev_Receive_TC_2_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_2_2, sizeof(Ev_Subscribe_TC_2_2)/sizeof(SAL_Event_Id_T),
         Ev_Urgent_List_TC_2_2, sizeof(Ev_Urgent_List_TC_2_2)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("3_2", Ev_Receive_TC_3_2, sizeof(Ev_Receive_TC_3_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_2, sizeof(Ev_Subscribe_TC_3_2)/sizeof(SAL_Event_Id_T), NULL, 0);
#else
   ret = ret || Receive_Test("3_2", Ev_Receive_TC_3_2, sizeof(Ev_Receive_TC_3_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_2, sizeof(Ev_Subscribe_TC_3_2)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("4_2", Ev_Receive_TC_4_2, sizeof(Ev_Receive_TC_4_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_4_2, sizeof(Ev_Subscribe_TC_4_2)/sizeof(SAL_Event_Id_T),
         Ev_Urgent_List_TC_4_2, sizeof(Ev_Urgent_List_TC_4_2)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("5_2", Ev_Receive_TC_5_2, sizeof(Ev_Receive_TC_5_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_2, sizeof(Ev_Subscribe_TC_5_2)/sizeof(SAL_Event_Id_T), NULL, 0);
#else
   ret = ret || Receive_Test("5_2", Ev_Receive_TC_5_2, sizeof(Ev_Receive_TC_5_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_2, sizeof(Ev_Subscribe_TC_5_2)/sizeof(SAL_Event_Id_T));
#endif

#if defined (XSAL)
   ret = ret || Receive_Test("6_2", Ev_Receive_TC_6_2, sizeof(Ev_Receive_TC_6_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_6_2, sizeof(Ev_Subscribe_TC_6_2)/sizeof(SAL_Event_Id_T), 
         Ev_Urgent_List_TC_6_2, sizeof(Ev_Urgent_List_TC_6_2)/sizeof(SAL_Event_Id_T));
#endif
  
   if (ret)
   {
      SAL_Exit(1);
   }

   Tr_Info_Lo("Local_Receiver_Test_2: Stop");
}


/**  Function of PUBLISHER for publisher thread of local communication.
 *
 *   - publishes messages from the list of all test cases
 */

void Local_Publisher_Test(void* param)
{
   SAL_Int_T ret = 0;

   Tr_Info_Lo("Local_Publish_Test: Start");

   /*
    * Make sure subscribe/unsubscribe of empty lists is ok.
    */
   ret |= (!SAL_Subscribe(NULL, 0));
   ret |= (!SAL_Unsubscribe(NULL, 0));

   if (ret)
   {
      Tr_Fault("Unable to subscribe/unscribe to empty lists.");
   }
   else
   {
      Tr_Info_Hi("Subscribe/Unsubscribe to empty lists worked.");
   }

   ret = ret || Publish_Test(1, Ev_Publish_TC_1, sizeof(Ev_Publish_TC_1)/sizeof(Ev_Test_Case_Data_T));

#if defined (XSAL)
   ret = ret || Publish_Test(2, Ev_Publish_TC_2, sizeof(Ev_Publish_TC_2)/sizeof(Ev_Test_Case_Data_T));
#endif

   ret = ret || Publish_Test(3, Ev_Publish_TC_3, sizeof(Ev_Publish_TC_3)/sizeof(Ev_Test_Case_Data_T));

#if defined (XSAL)
   ret = ret || Publish_Test(4, Ev_Publish_TC_4, sizeof(Ev_Publish_TC_4)/sizeof(Ev_Test_Case_Data_T));
#endif

   ret = ret || Publish_Test(5, Ev_Publish_TC_5, sizeof(Ev_Publish_TC_5)/sizeof(Ev_Test_Case_Data_T));

#if defined (XSAL)
   ret = ret || Publish_Test(6, Ev_Publish_TC_6, sizeof(Ev_Publish_TC_6)/sizeof(Ev_Test_Case_Data_T));
#endif

   if (ret)
   {
      SAL_Exit(1);      
   }

   Tr_Info_Lo("Local_Publish_Test: Stop");
}


#if defined (XSAL)

/*  *********************************************
 * 
 *   Tests communications between two processes 
 *
 *   ********************************************/


/*   ********************************************
 *   Receiver process
 *   ********************************************/

/**  Function of RECEIVER_\a n for remote receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Remote_Receiver_Test_1(void* param)
{
   SAL_Int_T ret = 0;
   size_t message_size = (size_t)param;

   Tr_Info_Lo("Remote_Receiver_Test_1: Start");

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

   ret = ret || Receive_Test("1_1", Ev_Receive_TC_1_1, sizeof(Ev_Receive_TC_1_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_1, sizeof(Ev_Subscribe_TC_1_1)/sizeof(SAL_Event_Id_T), NULL, 0);

   ret = ret || Receive_Test("2_1", Ev_Receive_TC_2_1, sizeof(Ev_Receive_TC_2_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_2_1, sizeof(Ev_Subscribe_TC_2_1)/sizeof(SAL_Event_Id_T),
         Ev_Urgent_List_TC_2_1, sizeof(Ev_Urgent_List_TC_2_1)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("3_1", Ev_Receive_TC_3_1, sizeof(Ev_Receive_TC_3_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_1, sizeof(Ev_Subscribe_TC_3_1)/sizeof(SAL_Event_Id_T), NULL, 0);

   ret = ret || Receive_Test("4_1", Ev_Receive_TC_4_1, sizeof(Ev_Receive_TC_4_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_4_1, sizeof(Ev_Subscribe_TC_4_1)/sizeof(SAL_Event_Id_T), 
         Ev_Urgent_List_TC_4_1, sizeof(Ev_Urgent_List_TC_4_1)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("5_1", Ev_Receive_TC_5_1, sizeof(Ev_Receive_TC_5_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_1, sizeof(Ev_Subscribe_TC_5_1)/sizeof(SAL_Event_Id_T), NULL, 0);

   ret = ret || Receive_Test("6_1", Ev_Receive_TC_6_1, sizeof(Ev_Receive_TC_6_1)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_6_1, sizeof(Ev_Subscribe_TC_6_1)/sizeof(SAL_Event_Id_T), 
         Ev_Urgent_List_TC_6_1, sizeof(Ev_Urgent_List_TC_6_1)/sizeof(SAL_Event_Id_T));

   if (ret)
   {
      return;
   }

   Tr_Info_Lo("Remote_Receiver_Test_1: Stop");
}


/**  Function of RECEIVER_\a n for remote receiver thread.
 *
 *   - creates the queue
 *   - receives messages of list for all test cases
 */

void Remote_Receiver_Test_2(void* param)
{
   SAL_Int_T ret = 0;
   size_t message_size = (size_t)param;

   Tr_Info_Lo("Remote_Receiver_Test_2: Start");

   if (!SAL_Create_Queue(4, message_size, malloc, free))
   {
      PBC_Failed("Receiver_Fnc: SAL_Create_Queue error");
   }

   ret = ret || Receive_Test("1_2", Ev_Receive_TC_1_2, sizeof(Ev_Receive_TC_1_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_1_2, sizeof(Ev_Subscribe_TC_1_2)/sizeof(SAL_Event_Id_T), NULL, 0);

   ret = ret || Receive_Test("2_2", Ev_Receive_TC_2_2, sizeof(Ev_Receive_TC_2_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_2_2, sizeof(Ev_Subscribe_TC_2_2)/sizeof(SAL_Event_Id_T),
         Ev_Urgent_List_TC_2_2, sizeof(Ev_Urgent_List_TC_2_2)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("3_2", Ev_Receive_TC_3_2, sizeof(Ev_Receive_TC_3_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_3_2, sizeof(Ev_Subscribe_TC_3_2)/sizeof(SAL_Event_Id_T), NULL, 0);

   ret = ret || Receive_Test("4_2", Ev_Receive_TC_4_2, sizeof(Ev_Receive_TC_4_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_4_2, sizeof(Ev_Subscribe_TC_4_2)/sizeof(SAL_Event_Id_T),
         Ev_Urgent_List_TC_4_2, sizeof(Ev_Urgent_List_TC_4_2)/sizeof(SAL_Event_Id_T));

   ret = ret || Receive_Test("5_2", Ev_Receive_TC_5_2, sizeof(Ev_Receive_TC_5_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_5_2, sizeof(Ev_Subscribe_TC_5_2)/sizeof(SAL_Event_Id_T), NULL, 0);

   ret = ret || Receive_Test("6_2", Ev_Receive_TC_6_2, sizeof(Ev_Receive_TC_6_2)/sizeof(Ev_Test_Case_Data_T),
         Ev_Subscribe_TC_6_2, sizeof(Ev_Subscribe_TC_6_2)/sizeof(SAL_Event_Id_T), 
         Ev_Urgent_List_TC_6_2, sizeof(Ev_Urgent_List_TC_6_2)/sizeof(SAL_Event_Id_T));

   if (ret)
   {
      return;
   }

   Tr_Info_Lo("Remote_Receiver_Test_2: Stop");
}


/**  Function of remote receiver control thread.
 *
 *   - is called by Wait_For_Publisher function for each test case
 *   - receives messages with info about passing the test from all receivers
 *   - if test has not been passed, sends the info to the control thread 
 *     of the publisher process
 */

SAL_Int_T Rec_Is_Test_Passed()
{
   const SAL_Message_T* msg;
   SAL_Int_T i; 
   SAL_Int_T data;
   SAL_Int_T ret = 0;

    for (i = 0; i < NUMBER_OF_RECEIVERS; i++)
    {
        msg = SAL_Receive();

        if (msg->sender_app_id != APP_TEST_RECEIVER || msg->sender_thread_id < 2 || msg->sender_thread_id > 5 || msg->event_id != (SAL_Event_Id_T)EV_REC_PASS)
        {
           Tr_Fault("Receiver control process has received invalid message");

           SAL_Send(APP_TEST_PUBLISHER, PUB_CONTROL, EV_ERROR, NULL, 0);

           SAL_Exit(1);
        }

        data = *(SAL_Int_T*)msg->data;
        
        /* test has passed */
        if (data == 0)
        {
           ret = 0;
        }
        /* test has not passed */
        else if (data == 1)
        {
           ret = ret || 1;
        }
    }

    /* if test has not passed */
    if (ret)
    {
      /* inform publisher */
      SAL_Send(APP_TEST_PUBLISHER, PUB_CONTROL, EV_REC_PASS, (void*)&ret, sizeof(SAL_Int_T));

      SAL_Exit(1);
    }

    return ret;
}


/**  Function of remote receiver control thread.
 *   
 *   - is called by Remote_Receiver_Driver for each test case
 *   - communicates with remote publisher control thread, sets the semaphore values
 */

SAL_Int_T Wait_For_Publisher()
{
   const SAL_Message_T* msg;
   SAL_Int_T i;

   /* wait until the receiver threads initialize */
   for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)
   {
      SAL_Wait_Semaphore(&Sem_Rec[i]);
   }

   /* send the message to the remote publisher control that receiver threads has initialzed */
   SAL_Send(APP_TEST_PUBLISHER, PUB_CONTROL, 0, NULL, 0);

   /* wait until the remote publisher publishes all the messages */
   msg = SAL_Receive();

   if (msg->sender_app_id != APP_TEST_PUBLISHER || 
      msg->sender_thread_id != PUB_CONTROL || 
      (msg->event_id != 0 && msg->event_id != (SAL_Event_Id_T)EV_ERROR))
   {
      Tr_Fault("Receiver control process received invalid message");

      SAL_Send(APP_TEST_PUBLISHER, PUB_CONTROL, EV_ERROR, NULL, 0);

      SAL_Exit(1);
   }

   if (msg->event_id == (SAL_Event_Id_T)EV_ERROR)
   {
      Tr_Fault("Receiver process received information that publisher process failed");
      SAL_Exit(1);
   }

   /* signal that receivers can receive the messages */
   for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)
      SAL_Signal_Semaphore(&Sem_Publish[i]);

   /* check if test case has passed */
   return Rec_Is_Test_Passed();
}


/**  Function of REC_CONTROL for remote receiver control thread.
 *
 *   - communicates with remote publisher control thread, sets the semaphore values
 */

void Remote_Receiver_Driver(void* param)
{
   const SAL_Message_T* msg;
   SAL_Int_T ret = 0;
   SAL_Int_T i;

   if (!SAL_Create_Queue(NUMBER_OF_RECEIVERS, sizeof(SAL_Int_T), malloc, free))
   {
      Tr_Fault("Receiver_Fnc: SAL_Create_Queue error");
      SAL_Exit(1); 
   }

   /* wait until remote publisher initializes */
   msg = SAL_Receive();
   
   if (msg->sender_app_id != APP_TEST_PUBLISHER || 
       msg->sender_thread_id != PUB_CONTROL || 
      (msg->event_id != 0 && msg->event_id != (SAL_Event_Id_T)EV_ERROR))
   {
      Tr_Fault("Receiver process received invalid message");

      SAL_Send(APP_TEST_PUBLISHER, PUB_CONTROL, EV_ERROR, NULL, 0);

      SAL_Exit(1);
   }

   if (msg->event_id == (SAL_Event_Id_T)EV_ERROR)
   {
      Tr_Fault("Receiver process received information that publisher process failed");
      SAL_Exit(1);
   }

   /* test case 1 */
   ret = Wait_For_Publisher();

   /* test cases 2 to 6 */
   for (i = 1; i < 6; i++)
   {
      ret = ret || Wait_For_Publisher();
   }  

   /* send the info to publisher control if all tests has passed */
   SAL_Send(APP_TEST_PUBLISHER, PUB_CONTROL, EV_REC_PASS, (void*)&ret, sizeof(SAL_Int_T));

}



/*   ************************************************
 *   Publisher process
 *   ***********************************************/


/**  Function of PUBLISHER for publisher thread of remote communication.
 *
 *   - publishes messages from the list of all test cases   
 */

void Remote_Publisher_Test(void* param)
{
   SAL_Int_T ret = 0;

   Tr_Info_Lo("Remote_Publisher_Test: Start");


   ret = ret || Publish_Test(1, Ev_Publish_TC_1, sizeof(Ev_Publish_TC_1)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Publish_Test(2, Ev_Publish_TC_2, sizeof(Ev_Publish_TC_2)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Publish_Test(3, Ev_Publish_TC_3, sizeof(Ev_Publish_TC_3)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Publish_Test(4, Ev_Publish_TC_4, sizeof(Ev_Publish_TC_4)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Publish_Test(5, Ev_Publish_TC_5, sizeof(Ev_Publish_TC_5)/sizeof(Ev_Test_Case_Data_T));

   ret = ret || Publish_Test(6, Ev_Publish_TC_6, sizeof(Ev_Publish_TC_6)/sizeof(Ev_Test_Case_Data_T));


   if (ret)
   {
      return;
   }

   Tr_Info_Lo("Remote_Publisher_Test: Stop");
}


/**  Function of remote publisher control thread.
 *   
 *   - is called by Remote_Publisher_Driver for each test case
 *   - communicates with remote receiver control thread, sets the semaphore values
 */

void Wait_For_Receiver()
{
   const SAL_Message_T* msg;
   SAL_Int_T i;

   /* wait until receiver will be ready */
   msg = SAL_Receive();

   if (msg->sender_app_id != APP_TEST_RECEIVER || 
      msg->sender_thread_id != REC_CONTROL || 
      (msg->event_id != 0 && msg->event_id != (SAL_Event_Id_T)EV_ERROR && msg->event_id != (SAL_Event_Id_T)EV_REC_PASS))
   {
      Tr_Fault("Publisher control process received invalid message");

      SAL_Send(APP_TEST_RECEIVER, REC_CONTROL, EV_ERROR, NULL, 0);

      SAL_Exit(1);
   }

   if (msg->event_id == (SAL_Event_Id_T)EV_ERROR)
   {
      Tr_Fault("Publisher process received information that receiver process failed");
      SAL_Exit(1);
   }

   if (msg->event_id == (SAL_Event_Id_T)EV_REC_PASS)
   {
      Tr_Fault("Publisher process received an information that receiver test failed");
      SAL_Exit(1);
   }


   /* signal that publisher can publish the messsages */
   for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)
   {
      SAL_Signal_Semaphore(&Sem_Rec[i]);
   }

   /* wait until all the messages are published */
   for(i = 2 + NUMBER_OF_RECEIVERS; i < 2 + 2 * NUMBER_OF_RECEIVERS; i++)   
   {
      SAL_Wait_Semaphore(&Sem_Publish[i]);
   }

   /* send the info to receiver control that publisher has published messages */
   SAL_Send(APP_TEST_RECEIVER, REC_CONTROL, 0, NULL, 0);
}


/**  Function of PUB_CONTROL for remote publisher control thread.
 *
 *   - communicates with remote receiver control thread, sets the semaphore values
 */

void Remote_Publisher_Driver(void* param)
{
   const SAL_Message_T* msg;
   SAL_Int_T data;
   SAL_Int_T i;

   if (!SAL_Create_Queue(2, sizeof(SAL_Int_T), malloc, free))
   {
      Tr_Fault("Receiver_Fnc: SAL_Create_Queue error");
      SAL_Exit(1); 
   }

   /* sends to the remote receiver control that publisher is ready */
   SAL_Send(APP_TEST_RECEIVER, REC_CONTROL, 0, NULL, 0);

   Wait_For_Receiver();

   for (i = 1; i < 6; i++)
   {
     Wait_For_Receiver();
   }

   msg = SAL_Receive();

   if (msg->sender_app_id != APP_TEST_RECEIVER || msg->sender_thread_id != REC_CONTROL || msg->event_id != (SAL_Event_Id_T)EV_REC_PASS)
   {
      Tr_Fault("Publisher control received invalid message");
      SAL_Exit(1);
   }

   data = *(SAL_Int_T*)msg->data;
   
   if (data == 1)
   {
      Tr_Fault("Publisher received the information that receiver test failed");
      SAL_Exit(1);
   }
}

#endif

/**  Test of SAL_Publish function in local or remote communication. 
 *   
 *   SAL_Subscribe, SAL_Unsubscribe functions and urgent messages are also tested.
 *   Creates threads and starts SAL_Run.
 */

SAL_Int_T Publish_Test_Suite()
{
   SAL_Thread_Id_T destroyed_list[] = { 1, 2 };
   SAL_Int_T retcode;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   switch(Test_Type)
   {
#if defined (XSAL)
      /* remote communication - publisher */
   case E_PUBLISHER:

      /* Publisher thread */
      SAL_Create_Thread(
         Remote_Publisher_Test, 
         NULL, 
         Init_Thread_Attr("RPublish", PUBLISHER, priority, &thread_attr));

      /* Local receiver1 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_1, 
         (void*)0, 
         Init_Thread_Attr("LRec2", RECEIVER_2, priority, &thread_attr));

      /* Local receiver2 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_2, 
         (void*)4, 
         Init_Thread_Attr("LRec3", RECEIVER_3, priority, &thread_attr));

      /* Local receiver3 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_1, 
         (void*)5, 
         Init_Thread_Attr("LRec4", RECEIVER_4, priority, &thread_attr));

      /* Local receiver4 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_2, 
         (void*)512, 
         Init_Thread_Attr("LRec5", RECEIVER_5, priority, &thread_attr));

      /* Control thread */
      SAL_Create_Thread(
         Remote_Publisher_Driver, 
         NULL, 
         Init_Thread_Attr("RemDrv", PUB_CONTROL, priority, &thread_attr));

      break;

      /* remote communication - receiver */
   case E_RECEIVER:

      /* Control thread */
    SAL_Create_Thread(
       Remote_Receiver_Driver, 
       NULL, 
       Init_Thread_Attr("RemDrv", REC_CONTROL, priority, &thread_attr));

      /* Remote receiver1 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test_1, 
         NULL, 
         Init_Thread_Attr("RRec2", RECEIVER_2, priority, &thread_attr));

      /* Remote receiver2 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test_2, 
         (void*)4, 
         Init_Thread_Attr("RRec3", RECEIVER_3, priority, &thread_attr));

      /* Remote receiver3 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test_1, 
         (void*)5, 
         Init_Thread_Attr("RRec4", RECEIVER_4, priority, &thread_attr));

      /* Remote receiver4 thread */
      SAL_Create_Thread(
         Remote_Receiver_Test_2, 
         (void*)512, 
         Init_Thread_Attr("RRec5", RECEIVER_5, priority, &thread_attr));

      break;
#endif

   default:
      /* local communication */

      /* Publisher thread */
      SAL_Create_Thread(
         Local_Publisher_Test, 
         NULL, 
         Init_Thread_Attr("LPublish", PUBLISHER, priority, &thread_attr));

      /* Receiver1 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_1, 
         NULL, 
         Init_Thread_Attr("LRec2", RECEIVER_2, priority, &thread_attr));

      /* Receiver2 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_2, 
         (void*)4, 
         Init_Thread_Attr("LRec3", RECEIVER_3, priority, &thread_attr));

      /* Receiver3 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_1, 
         (void*)5, 
         Init_Thread_Attr("LRec4", RECEIVER_4, priority, &thread_attr));

      /* Receiver4 thread */
      SAL_Create_Thread(
         Local_Receiver_Test_2, 
         (void*)512, 
         Init_Thread_Attr("LRec5", RECEIVER_5, priority, &thread_attr));

      break;
   }

   retcode = SAL_Run();

#if defined (XSAL)
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      Tr_Fault("Timeout has been reached");
      retcode = 1;
   }

#endif

   if (retcode != 0)
   {
      Tr_Info_Hi_1("Publish_Test: SAL_Run() return %d", (int)retcode);
   }

   return retcode;
}

/** \}
 */

/** \name Time_Publish_Test
 *
 *  \{
 */

/**  Function of PUBLISHER_THREAD.
 *
 *   - publishes message with timestamp
 */

void Publisher_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { RECEIVER_THREAD };
   Sample_Msg_T data1;

   Tr_Info_Lo("Publisher_Fun: Start");

#if defined (XSAL)
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));
#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      Tr_Fault("Timeout has been reached");
      SAL_Exit(1);
   }

#endif

   data1.field1 = 1;
   data1.field2 = 2;

   ftime(&Time);

   SAL_Publish(EV_E9, &data1, sizeof(data1));

   Tr_Info_Lo("Publisher_Fun: Stop");
}


/**  Function of RECEIVER_THREAD.
 *
 *   - receives message with timestamp and checks if the time is correct
 */

void Receiver_Fun(void* param)
{
   SAL_Event_Id_T subscribe_list [] = { EV_E9 };
   SAL_Event_Id_T unsubscribe_list [] = { EV_E9 };
   const SAL_Message_T* msg;

   Tr_Info_Lo("Receiver_Fun: Start");

   SAL_Create_Queue(1, 0, malloc, free);

   SAL_Subscribe(subscribe_list, sizeof(subscribe_list)/sizeof(SAL_Event_Id_T));

   SAL_Signal_Ready();

#if defined (XSAL)
   msg = SAL_Receive();
#else
   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      PBC_Failed("SAL_Receive_Timeout: message has not been received");
   }
#endif

   if (((msg->timestamp_sec - Time.time)*1000 + msg->timestamp_msec - Time.millitm) > 10)   
   {
      PBC_Failed("SAL_Send - SAL_Receive: Invalid time in header components");
   }

   SAL_Unsubscribe(unsubscribe_list, sizeof(unsubscribe_list)/sizeof(SAL_Event_Id_T));

   Tr_Info_Lo("Receiver_Fun: Stop");
}


/**  Test of timestamp.
 *
 *  Function creates two threads: PUBLISHER_THREAD and RECEIVER_THREAD and starts SAL_Run.
 */

SAL_Int_T Time_Publish_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Publisher_Fun, 
      NULL, 

      Init_Thread_Attr("PUB", PUBLISHER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Receiver_Fun, 
      NULL, 
      Init_Thread_Attr("REC", RECEIVER_THREAD, priority, &thread_attr));
   
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
            Test_Type = E_PUBLISHER;
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
   case E_PUBLISHER:

      config.app_id = APP_TEST_PUBLISHER;
      config.max_number_of_threads = 6;

      if (!SAL_Init(&config))
      {
         Tr_Fault("SAL_Init error");
         return 1;
      }

      break;

   case E_RECEIVER:

      config.app_id = APP_TEST_RECEIVER;
      config.max_number_of_threads = 5;

      if (!SAL_Init(&config))
      {
         Tr_Fault("SAL_Init error");
         return 1;
      }

      break;
#endif

   default:

      config.app_id = APP_TEST_LOCAL;
      config.max_number_of_threads = 8;

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

   for(i = 2; i <= 2 + 2 * NUMBER_OF_RECEIVERS - 1; i++)
   {
      if (!SAL_Create_Semaphore(&Sem_Rec[i], &sem_attr) || 
         !SAL_Create_Semaphore(&Sem_Publish[i], &sem_attr))
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


   for(i = 2; i <= 2 + 2 * NUMBER_OF_RECEIVERS - 1; i++)
   {
      if (!SAL_Destroy_Semaphore(&Sem_Rec[i]) || 
         !SAL_Destroy_Semaphore(&Sem_Publish[i]))
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
      Tr_Fault("Set Up failed");
      return 1;
   }

   ret = ret || Publish_Test_Suite();
   
   if (Test_Type == E_STAND_ALONE)
   {
      ret = ret || Time_Publish_Test();
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
 * 01 Sep 2006, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
