/* -------------------------------------------------------------------------
 *
 * File:        test_receive_only.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of receive only.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 5);

#define APP_ID 2

typedef enum Thread_Id_Tag
{
   SENDER_THREAD = PM_LAST_THREAD_ID,
   REC_THREAD_1,
   REC_THREAD_2,
   BUFFER_THREAD,
   MAX_NUMB_OF_THREADS
} Thread_Id_T;

typedef enum Event_Id_Tag
{
   EV_E1 = SAL_EV_FIRST_PRIVATE_ID,
   EV_E2,
   EV_E3,
   EV_E4,
   EV_E5,
   EV_E6,
   EV_E7,
   EV_E8,
   EV_E9, 
   EV_ERROR
} Event_Id_T;

typedef struct Sample_Msg_Tag_1
{
   SAL_Int_T field_1;
   SAL_Int_T field_2;
} Sample_Msg_T_1;

typedef struct Sample_Msg_Tag_2
{
   SAL_Int_T field_1;
   SAL_Int_T field_2;
   SAL_Int_T field_3;
} Sample_Msg_T_2;

typedef struct Param_List_Tag
{
   SAL_Event_Id_T* send_event_list;
   size_t send_event_list_size;
   SAL_Event_Id_T* rec_event_list;
   size_t rec_event_list_size;
   SAL_Int_T rec_event_list_number;
   SAL_Int_T test_nr;
   SAL_Int_T test_case_nr;
} Param_List_T;

SAL_Mutex_T Mutex;

size_t Message_Count, Peak_Message_Count;
size_t Peak_Message_Size;

SAL_Int_T Long_Timeout = 3000;

SAL_Event_Id_T Send_Event_List_Test_Case_1[] = { EV_E1, EV_E2, EV_E3, EV_E4, 
                                             EV_E1, EV_E2, EV_E3, EV_E4 };

SAL_Event_Id_T Send_Event_List_Test_Case_2[] = { EV_E1, EV_E2, EV_E3, EV_E4, 
                                             EV_E5, EV_E6, EV_E7, EV_E8 };

SAL_Event_Id_T Rec_Event_List_Test_Case_1[] = { EV_E1, EV_E4 };
SAL_Event_Id_T Rec_Event_List_Test_Case_2[] = { EV_E1, EV_E3, EV_E6, EV_E8 };

/** 
 *   \file
 *
 *   Test of receiving selected messages from the queues (SAL_Receive_Only and 
 *   SAL_Receive_Only_Timeout functions).
 *
 *   \section T Tests:
 *   First group:
 *   <OL>
 *   <LI> \ref ROT12 - test of SAL_Receive_Only and SAL_Receive_Only_Timeout functions
 *        (<B><I>the first Receive_Only_Test for XSAL only</I></B>). </LI> 
 *   <LI> \ref ROT3 - test of SAL_Receive_Only_Timeout function. </LI>
 *   </OL>
 *
 *   Second group:
 *   <OL>
 *   <LI> \ref ROT45 - test of SAL_Receive_Only and SAL_Receive_Only_Timeout function.
 *        (<B><I>the fourth Receive_Only_Test for XSAL only</I></B>).
 *   </OL>
 *
 *   \section ROT12 the first and the second Receive_Only_Test
 *
 *   Creates three threads (for XSAL) or two threads(for XSAL_LIGHT):
 *   - REC_THREAD_1
 *   - REC_THREAD_2 (<B><I>XSAL only</I></B>)
 *   - SENDER_THREAD
 *
 *   At first SEND_CASE messages are sent to REC_THREAD_1, then REC_THREAD_1 receives 
 *   REC_CASE messages which are subset of SEND_CASE and at the end REC_THREAD_2 receives 
 *   remaining messages. 
 *
 *   \subsection TP Test procedure:
 *
 *   <OL>
 *   <LI> REC_THREAD_1 creates the queue. </LI>
 *   <LI> SENDER_THREAD sends SEND_CASE to REC_THREAD_1. </LI>
 *   <LI> SENDER_THREAD waits until REC_THREAD_1 terminates. </LI>
 *   <LI> REC_THREAD_1 receives REC_CASE by REC_ONLY and checks if timeout and data 
 *        are valid. </LI>
 *   <LI> REC_THREAD_1 checks if fields of SAL_Stat_Queue_T are valid. </LI>
 *   <LI> REC_THREAD_1 sends message (meaning that it has finished receiving messages) 
 *        to REC_THREAD_2 (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_1 waits until REC_THREAD_2 terminates (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_2 creates queue (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_2 receives message from REC_THREAD_1 (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_2 receives all remaining messages from the queue of REC_THREAD_1
 *        and checks if messages data are valid (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_1 checks if fields of SAL_Stat_Queue_T are valid 
 *        (<B><I>XSAL only</I></B>). </LI>
 *   </OL>
 *
 *  \subsection FT the first Receive_Only_Test 
 *
 *  In REC_THREAD_1 messages are received by SAL_Receive_Only function.
 *
 *  \subsection CT the second Receive_Only_Test
 *
 *  In REC_THREAD_1 messages are received by SAL_Receive_Only_Timeout function with 
 *  timeout 3000 msec. Timeout is long therefore messages are received before timeout 
 *  is reached.
 *
 *   \section ROT3 the third Receive_Only_Test
 *
 *   Creates two threads:
 *   - SEND_THREAD holds the message queue
 *   - RECEIVE_THREAD_1 - 
 *   Timeout is short (100 ms).
 *   Timeout is reached earlier than messages would be sent.
 *   
 *   \subsection TP Test procedure:
 *
 *   <OL>
 *   <LI> SENDER_THREAD waits for REC_THREAD_1. </LI>
 *   <LI> REC_THREAD_1 creates the queue. </LI>
 *   <LI> REC_THREAD_1 waits for receiving messages with short timeout 100 msec.
 *        Timeout is reached.
 *   <LI> REC_THREAD_1 checks the value of timeout. </LI>
 *   <LI> REC_THREAD_1 signals that is ready. </LI>
 *   <LI> SENDER_THREAD resumes and terminates. </LI>
 *   <LI> REC_THREAD_1 waits until SENDER_THREAD terminates. </LI>
 *   </OL>
 *
 *   \section ROT45 the fourth and the fifth Receive_Only_Test
 *
 *   Creates three threads (for XSAL) or two threads(for XSAL_LIGHT):
 *   - REC_THREAD_1
 *   - REC_THREAD_2 (<B><I>XSAL only</I></B>)
 *   - SENDER_THREAD
 *
 *   At first REC_THREAD_1 waits for receiving messages, then SENDER_THREAD sends 
 *   SEND_CASE messages, REC_THREAD_1 receives REC_CASE and at the end REC_THREAD_2 
 *   receives remaining messages. 
 *
 *   \subsection TP Test procedure:
 *
 *   <OL>
 *   <LI> REC_THREAD_1 creates the queue. </LI>
 *   <LI> REC_THREAD_1 waits for receiving messages. </LI>
 *   <LI> SENDER_THREAD sends SEND_CASE to REC_THREAD_1. </LI>
 *   <LI> REC_THREAD_1 receives REC_CASE by REC_ONLY and checks if timeout and data 
 *        are valid. </LI>
 *   <LI> REC_THREAD_1 checks if fields of SAL_Stat_Queue_T are valid. </LI>
 *   <LI> REC_THREAD_1 sends message (meaning that it has finished receiving messages) 
 *        to REC_THREAD_2 (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_1 waits until REC_THREAD_2 terminates (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_2 creates queue (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_2 receives message from REC_THREAD_1 (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_2 receives all remaining messages from the queue of REC_THREAD_1
 *        and checks if messages data are valid (<B><I>XSAL only</I></B>). </LI>
 *   <LI> REC_THREAD_2 checks if fields of SAL_Stat_Queue_T are valid 
 *        (<B><I>XSAL only</I></B>). </LI>
 *   </OL>
 *
 *  \subsection FRT the fourth Receive_Only_Test 
 *
 *  In REC_THREAD_1 messages are received by SAL_Receive_Only function.
 *
 *  \subsection FFT the fifth Receive_Only_Test
 *
 *  In REC_THREAD_1 messages are received by SAL_Receive_Only_Timeout function with 
 *  timeout 3000 msec. Timeout is long therefore messages are received before timeout 
 *  is reached.
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER> Symbol </TD><TD> the first Receive_Only_Test </TD>
 *                  <TD> the second Receive_Only_Test </CENTER></B></TD></TR>
 *   <TR><TD> REC_ONLY </TD><TD> SAL_Receive_Only </TD><TD> SAL_Receive_Only_Timeout </TD></TR>
 *   </TABLE>
 *
 *  \section TC Test Cases
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER> Test case nr </TD><TD> SEND_CASE </TD>
 *                                          <TD> REC_CASE </CENTER></B></TD></TR>
 *   <TR><TD> 1 </TD><TD> Send_Event_List_Test_Case_1 </TD>
 *                                          <TD> Rec_Event_List_Test_Case_1 </TD></TR>
 *   <TR><TD> 2 </TD><TD> Send_Event_List_Test_Case_2 </TD>
 *                                          <TD> Rec_Event_List_Test_Case_2 </TD></TR>
 *   </TABLE>
 *
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER> Send event list test case </TD><TD> Rec event list test case </TD>
 *                      <TD> Send event list </TD><TD> Sub event list </TD>
 *                      <TD> Rec event list </CENTER></B></TD></TR>
 *   <TR><TD> Send_Event_List_Test_Case_1 </TD><TD> Rec_Event_List_Test_Case_1 </TD>
 *   <TD>EV_E1, EV_E2, EV_E3, EV_E4,\n EV_E1, EV_E2, EV_E3, EV_E4</TD><TD>EV_E1, EV_E4</TD>
 *   <TD>EV_E1, EV_E4, EV_E1, EV_E4</TD></TR>
 *   <TR><TD> Send_Event_List_Test_Case_2 </TD><TD> Rec_Event_List_Test_Case_2 </TD>
 *   <TD>EV_E1, EV_E2, EV_E3, EV_E4,\n EV_E5, EV_E6, EV_E7, EV_E8</TD>
 *   <TD>EV_E1, EV_E3, EV_E6, EV_E8</TD><TD>EV_E1, EV_E3, EV_E6, EV_E8</TD></TR>
 *   </TABLE>
 *
 *  where:
 *  - <B> Send event list </B> - list of events which are sent
 *  - <B> Sub event list </B> - list of events which thread is subcribed on
 *  - <B> Rec event list </B> - list of events which are received
 *
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


#if defined (XSAL)

/**  \name Functions of the first Receive_Only_Test
 *
 *   \{
 */

/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - receives messages
 *   - checks if data of received messages are valid
 *   - checks if fields of SAL_Stat_Queue_T are valid
 *   - sends message to REC_THREAD_2
 */

void Receive_Only_Thread_1(void* param)
{
   SAL_Thread_Id_T wait_list[] = { SENDER_THREAD };
   SAL_Thread_Id_T destroyed_list[] = { REC_THREAD_2 };
   const SAL_Message_T* msg;
   Sample_Msg_T_1* data_1;
   Sample_Msg_T_2* data_2;
   SAL_Stat_Queue_T stat;
   SAL_Int_T counter = 0;
   Param_List_T* param_list = (Param_List_T*)param;

   Tr_Info_Lo("Receive_Only_Thread_1: Start");

   /* create the queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   /* wait until all messages are sent */
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   do
   {
      /* receive selected messages */
      msg = SAL_Receive_Only(param_list->rec_event_list, param_list->rec_event_list_size);

      if (msg != NULL)
      {
         if (msg->data_size == sizeof(Sample_Msg_T_1))
         {
            data_1 = (Sample_Msg_T_1*)msg->data;
      
             Tr_Info_Mid_2("Receive_Only_Thread: field_1: %d, field_2: %d\n", 
                           (int)data_1->field_1, (int)data_1->field_2);
         }
         else /* msg->data_size == sizeof(Sample_Msg_T_2) */
         {
            data_2 = (Sample_Msg_T_2*)msg->data;

            Tr_Info_Mid_3("Receive_Only_Thread: field_1: %d, field_2: %d, field_3:%d\n", 
                        (int)data_2->field_1, (int)data_2->field_2, (int)data_2->field_3);
         }
      }

      counter++;
  
   }
   while (counter < param_list->rec_event_list_number);

   Message_Count -= param_list->rec_event_list_number;

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   /* check message_count */
   if (stat.message_count != Message_Count)
   {
      Tr_Fault_2("Invalid message count in SAL_Stat_Queue_T structure: %d, Message_Count: %d",
                 (int)stat.message_count, (int)Message_Count);
      PBC_Failed_1("rec_event_list_number: %d", (int)param_list->rec_event_list_number);
   }

   /* check peak_message_count */
   if (stat.peak_message_count != Peak_Message_Count)
   {
      PBC_Failed_2("Invalid maximum number of messages in the queue in SAL_Stat_Queue_T structure: %d, Peak_Message_Count: %d",
                 (int)stat.peak_message_count, (int)Peak_Message_Count);
   }

   /* check peak_message_size */
   if (stat.peak_message_size != Peak_Message_Size)
   {
      PBC_Failed("Invalid maximum message size in SAL_Stat_Queue_T structure");
   }

   /* send message to REC_THREAD_2 */
   SAL_Send(APP_ID, REC_THREAD_2, EV_E1, NULL, 0);

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Receive_Only_Thread_1: Stop");

} 

#endif

/** \}
 */


/**  \name Functions of the second Receive_Only_Test
 *
 *   \{
 */

/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - receives messages
 *   - checks if data of received messages are valid
 *   - checks if fields of SAL_Stat_Queue_T are valid
 *   - sends message to REC_THREAD_2 (only for XSAL)
 */

void Receive_Only_Thread_2(void* param)
{
#if defined (XSAL)
   SAL_Thread_Id_T destroyed_list[] = { REC_THREAD_2 };
#endif
   SAL_Thread_Id_T wait_list[] = { SENDER_THREAD };
   const SAL_Message_T* msg;
   Sample_Msg_T_1* data_1;
   Sample_Msg_T_2* data_2;
   SAL_Stat_Queue_T stat;
   SAL_Int_T counter = 0;
   Param_List_T* param_list = (Param_List_T*)param;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;

   Tr_Info_Lo("Receive_Only_Thread_2: Start");

   /* create the queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   /* wait until SENDER_THREAD sends messages */
   /* LONG TIMEOUT */
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
              Long_Timeout))
   {
         PBC_Failed("Timeout has been reached");
   }

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   do
   {
      time_1 = SAL_Clock();
       
      /* receive selected messages */
      msg = SAL_Receive_Only_Timeout(param_list->rec_event_list, 
               param_list->rec_event_list_size, 
               Long_Timeout);

      time_2 = SAL_Clock();

      delta_time = (SAL_Int_T)(time_2 - time_1);

      if (msg != NULL)
      {
         if (delta_time > 3)
         {
            PBC_Failed_1("SAL_Receive_Only_Timeout: suspends the thread, time: %d", (int)delta_time);
         }
      }
      else
      {
         PBC_Failed("SAL_Receive_Only_Timeout: timeout has been reached");
      }

      if (msg != NULL)
      {
         if (msg->data_size == sizeof(Sample_Msg_T_1))
         {
            data_1 = (Sample_Msg_T_1*)msg->data;
      
            Tr_Info_Mid_2("Receive_Only_Thread: field_1: %d, field_2: %d\n", 
                        (int)data_1->field_1, (int)data_1->field_2);
         }
         else /* msg->data_size == sizeof(Sample_Msg_T_2) */
         {
            data_2 = (Sample_Msg_T_2*)msg->data;

            Tr_Info_Mid_3("Receive_Only_Thread: field_1: %d, field_2: %d, field_3:%d\n", 
                          (int)data_2->field_1, (int)data_2->field_2, (int)data_2->field_3);
         }
      }
 
      counter++;
  
   }
   while (counter < param_list->rec_event_list_number);

   Message_Count -= param_list->rec_event_list_number;

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   /* check message_count */
   if (stat.message_count != Message_Count)
   {
      Tr_Fault_2("Invalid message count in SAL_Stat_Queue_T structure: %d, Message_Count: %d",
               (int)stat.message_count, (int)Message_Count);
      PBC_Failed_1("rec_event_list_number: %d", (int)param_list->rec_event_list_number);
   }

   /* check peak_message_count */
   if (stat.peak_message_count != Peak_Message_Count)
   {
      PBC_Failed_2("Invalid maximum number of messages in the queue in SAL_Stat_Queue_T structure: %d, Peak_Message_Count: %d",
                  (int)stat.peak_message_count, (int)Peak_Message_Count);
   }

   /* check peak_message_size */
   if (stat.peak_message_size != Peak_Message_Size)
   {
      PBC_Failed("Invalid maximum message size in SAL_Stat_Queue_T structure");
   }

#if defined (XSAL)

   /* send message to REC_THREAD_2 */
   SAL_Send(APP_ID, REC_THREAD_2, EV_E1, NULL, 0);

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Receive_Only_Thread_2: Stop");
} 

/** \}
 */



/**  \name Functions of the third Receive_Only_Test
 *
 *   \{
 */

/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - try to receive message
 *   - checks if timeout is valid
 */

void Receive_Only_Thread_3(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD };
   const SAL_Message_T* msg;
   Param_List_T* param_list = (Param_List_T*)param;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;

   Tr_Info_Lo("Receive_Only_Thread_3: Start");

   /* create the queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   time_1 = SAL_Clock();

   /* try to receive message */
   msg = SAL_Receive_Only_Timeout(param_list->rec_event_list, 
                  param_list->rec_event_list_size, 
                  100);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (msg != NULL)
   {
      PBC_Failed_2("SAL_Receive_Only_Timeout: timeout has not been reached, delta_time: %d, EventId: %d", 
                                                               delta_time, msg->event_id);
   }

   /* check if timeout is valid */
   if ((delta_time < 100 - 7) && (delta_time > 100 + 15))
   {
      PBC_Failed("SAL_Receive_Only_Timeout: timeout interval is not valid");
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
            sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
            Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   Tr_Info_Lo("Receive_Only_Thread_3: Stop");

}

/**  Function of SENDER_THREAD
 *
 *   - waits for REC_THREAD_1 thread
 */

void Sender_Thread_3(void* param)
{
   SAL_Thread_Id_T wait_list[] = { REC_THREAD_1 };

   Tr_Info_Lo("Sender_Thread_3: Start");

   /* wait for REC_THREAD_1 */
   if (!SAL_Wait_Ready_Timeout(wait_list, 
              sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
              Long_Timeout))
   {
         PBC_Failed("Timeout has been reached");
   }

   Tr_Info_Lo("Sender_Thread_3: Stop");

}

/** \}
 */

#if defined (XSAL)

/**  \name Functions of the fourth Receive_Only_Test
 *
 *   \{
 */

/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - receives messages
 *   - checks if data of received messages are valid
 *   - checks if fields of SAL_Stat_Queue_T are valid
 *   - sends message to REC_THREAD_2 (only for XSAL)
 */

void Receive_Only_Thread_4(void* param)
{
   SAL_Thread_Id_T destroyed_list_1[] = { REC_THREAD_2 };
   SAL_Thread_Id_T destroyed_list_2[] = { SENDER_THREAD };
   const SAL_Message_T* msg;
   Sample_Msg_T_1* data_1;
   Sample_Msg_T_2* data_2;
   SAL_Stat_Queue_T stat;
   SAL_Int_T counter = 0;
   Param_List_T* param_list = (Param_List_T*)param;

   Tr_Info_Lo("Receive_Only_Thread_4: Start");

   /* create the queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   do
   {

      /* receive selected messages */
      msg = SAL_Receive_Only(param_list->rec_event_list, param_list->rec_event_list_size);

      if (msg == NULL)
      {
         Tr_Fault("SAL_Receive_Only: returns NULL");

         /* send info to REC_THREAD_2 that error has occured */
         SAL_Send(APP_ID, REC_THREAD_2, EV_ERROR, NULL, 0);

         SAL_Wait_Destroyed(destroyed_list_2, 
                 sizeof(destroyed_list_2)/sizeof(SAL_Thread_Id_T));

         SAL_Exit(1);
      }

      if (msg != NULL)
      { 
         if (msg->data_size == sizeof(Sample_Msg_T_1))
         {
            data_1 = (Sample_Msg_T_1*)msg->data;
                
            Tr_Info_Mid_2("Receive_Only_Thread: field_1: %d, field_2: %d\n", 
                          (int)data_1->field_1, (int)data_1->field_2);
         }
         else /* msg->data_size == sizeof(Sample_Msg_T_2) */
         {
            data_2 = (Sample_Msg_T_2*)msg->data;

            Tr_Info_Mid_3("Receive_Only_Thread: field_1: %d, field_2: %d, field_3:%d\n", 
                          (int)data_2->field_1, (int)data_2->field_2, (int)data_2->field_3);
         }
      }         

      counter++;
   
   }  
   while (counter < param_list->rec_event_list_number);

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   /* check message_count */
   if (stat.message_count > (Peak_Message_Count - param_list->rec_event_list_number))
   {
      PBC_Failed_2("Invalid message count in SAL_Stat_Queue_T structure: %d, Peak_Message_Count: %d",
                 (int)stat.message_count, (int)Peak_Message_Count);
   }

   /* check peak_message_count */
   if (stat.peak_message_count > Peak_Message_Count)
   {
      PBC_Failed_2("Invalid maximum number of messages in the queue in SAL_Stat_Queue_T structure: %d, Peak_Message_Count: %d",
                 (int)stat.peak_message_count, (int)Peak_Message_Count);
   }

   /* check peak_message_size */
   if (stat.peak_message_size != Peak_Message_Size)
   {
      PBC_Failed("Invalid maximum message size in SAL_Stat_Queue_T structure");
   }

   /* send message to REC_THREAD_2 */
   SAL_Send(APP_ID, REC_THREAD_2, EV_E1, NULL, 0);

   SAL_Wait_Destroyed(destroyed_list_1, sizeof(destroyed_list_1)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Receive_Only_Thread_4: Stop");

}

/** \}
 */

#endif


/**  \name Functions of the fifth Receive_Only_Test
 *
 *   \{
 */

/**  Function of REC_THREAD_1.
 *
 *   - creates queue
 *   - receives messages
 *   - checks if data of received messages are valid
 *   - checks if fields of SAL_Stat_Queue_T are valid
 *   - sends message to REC_THREAD_2 (only for XSAL)
 */

void Receive_Only_Thread_5(void* param)
{
#if defined (XSAL)
   SAL_Thread_Id_T destroyed_list[] = { REC_THREAD_2 };
#endif
   const SAL_Message_T* msg;
   Sample_Msg_T_1* data_1;
   Sample_Msg_T_2* data_2;
   SAL_Stat_Queue_T stat;
   SAL_Int_T counter = 0;
   Param_List_T* param_list = (Param_List_T*)param;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;

   Tr_Info_Lo("Receive_Only_Thread_5: Start");

   /* create queue */
   if (!SAL_Create_Queue(9, sizeof(Sample_Msg_T_1), malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   /* signals that it has created the queue */
   SAL_Signal_Ready();

   do
   {
      time_1 = SAL_Clock();
      
      /* receive selected messages */
      msg = SAL_Receive_Only_Timeout(param_list->rec_event_list, 
                  param_list->rec_event_list_size, 
                  Long_Timeout);

      time_2 = SAL_Clock();

      delta_time = (SAL_Int_T)(time_2 - time_1);

      if (msg != NULL)
      {
         if (delta_time > 3)
         {
            Tr_Info_Hi_2("Receive_Only_Thread_5: SAL_Receive_Only_Timeout suspends the thread, time: %d for event %d",
               (int)delta_time, (int)msg->event_id);
         }
         else
         {
            Tr_Info_Mid_1("Receive_Only_Thread_5: Received event %d", (int)msg->event_id);
         }

      }
      else
      {
         PBC_Failed("SAL_Receive_Only_Timeout: timeout has been reached");
      }
  
      if (msg != NULL)
      { 
         if (msg->data_size == sizeof(Sample_Msg_T_1))
         {
            data_1 = (Sample_Msg_T_1*)msg->data;
                
            Tr_Info_Mid_2("Receive_Only_Thread_5: field_1: %d, field_2: %d\n",
                        (int)data_1->field_1, (int)data_1->field_2);
         }
         else /* msg->data_size == sizeof(Sample_Msg_T_2) */
         {
            data_2 = (Sample_Msg_T_2*)msg->data;

            Tr_Info_Mid_3("Receive_Only_Thread_5: field_1: %d, field_2: %d, field_3:%d\n",
                        (int)data_2->field_1, (int)data_2->field_2, (int)data_2->field_3);
         }
      }         

      counter++;
   
   }  
   while (counter < param_list->rec_event_list_number);

   /* Wait for sender thread to terminate to make sure it sent everything to us. */
   {
      SAL_Thread_Id_T sender = SENDER_THREAD;

      if (!SAL_Wait_Destroyed_Timeout(&sender, 1, Long_Timeout))
      {
         PBC_Failed("Sender thread didn't terminate");
      }
   }

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   /* check message_count */
   if (stat.message_count > (Peak_Message_Count - param_list->rec_event_list_number))
   {
      PBC_Failed_2("Invalid message count in SAL_Stat_Queue_T structure: %d, Peak_Message_Count: %d",
                  (int)stat.message_count, (int)Peak_Message_Count);
   }

   /* check peak_message_count */
   if (stat.peak_message_count > Peak_Message_Count)
   {
      PBC_Failed_2("Invalid maximum number of messages in the queue in SAL_Stat_Queue_T structure: %d, Peak_Message_Count: %d",
                 (int)stat.peak_message_count, (int)Peak_Message_Count);
   }

   /* check peak_message_size */
   if (stat.peak_message_size != Peak_Message_Size)
   {
      PBC_Failed("Invalid maximum message size in SAL_Stat_Queue_T structure");
   }

#if defined (XSAL)

   /* send message to REC_THREAD_2 */
   SAL_Send(APP_ID, REC_THREAD_2, EV_E1, NULL, 0);

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Receive_Only_Thread_5: Stop");

} 

/** \}
 */

/**  \name Other Functions
 *   \{
 */

/**  Function of SENDER_THREAD for the fourth and the fifth Receive_Only_Test
 *
 *   - sends messages
 */

void Sender_Thread_First(void* param)
{
   SAL_Thread_Id_T wait_list[] = { REC_THREAD_1 };
   SAL_Thread_Id_T destroyed_list[] = { REC_THREAD_1 };
   Sample_Msg_T_1 data_1, data_3;
   Sample_Msg_T_2 data_2, data_4;
   Param_List_T* param_list = (Param_List_T*)param;

   Tr_Info_Lo("Sender_Thread_First: Start");

   /* wait until REC_THREAD_1 creates queue */
#if defined (XSAL)

   if (param_list->test_nr == 1)
   {
      SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));
   }

#endif

   if (param_list->test_nr == 2)
   {
      if (!SAL_Wait_Ready_Timeout(wait_list, 
              sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
              Long_Timeout))
      {
         PBC_Failed("Timeout has been reached");
      }
   }

   data_1.field_1 = 123;
   data_1.field_2 = 234;

   data_2.field_1 = 234;
   data_2.field_2 = 345;
   data_2.field_3 = 456;

   data_3.field_1 = 345;
   data_3.field_2 = 456;

   data_4.field_1 = 456;
   data_4.field_2 = 567;
   data_4.field_3 = 678;

   /* send messages */
   SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));

   if (param_list->test_case_nr == 1)
   {
      SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));
   }
   else
   {
      SAL_Send(APP_ID, REC_THREAD_1, EV_E5, &data_1, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E6, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E7, &data_3, sizeof(Sample_Msg_T_1));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E8, &data_4, sizeof(Sample_Msg_T_2));
   }

   SAL_Send(APP_ID, REC_THREAD_1, EV_E9, NULL, 0);

   Peak_Message_Count = Message_Count = 9;
   Peak_Message_Size = sizeof(data_2);

   /* signals that all messages are in the queue */
   SAL_Signal_Ready();

#if defined (XSAL)

   if (param_list->test_nr == 1)
   {
      SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
   }

#endif

   if (param_list->test_nr == 2)
   {
      if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Long_Timeout))
      {
         PBC_Failed("Wait Destroy Timeout has been reached");
      }
   }

   Tr_Info_Lo("Sender_Thread_First: Stop");

}


/**  Function of SENDER_THREAD for the fourth and the fifth Receive_Only_Test
 *
 *   - sends messages
 */

void Sender_Thread_Second(void* param)
{
   SAL_Thread_Id_T wait_list[] = { REC_THREAD_1 };
   Sample_Msg_T_1 data_1, data_3;
   Sample_Msg_T_2 data_2, data_4;
   Param_List_T* param_list = (Param_List_T*)param;

   Tr_Info_Lo("Sender_Thread_Second: Start");

   /* wait until REC_THREAD_1 creates queue */
#if defined (XSAL)

   if (param_list->test_nr == 4)
   {
      SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));
   }

#endif

   if (param_list->test_nr == 5)
   {
      if (!SAL_Wait_Ready_Timeout(wait_list, 
           sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
           Long_Timeout))
      {
         PBC_Failed("Timeout has been reached");
      }
   }

   data_1.field_1 = 123;
   data_1.field_2 = 234;

   data_2.field_1 = 234;
   data_2.field_2 = 345;
   data_2.field_3 = 456;

   data_3.field_1 = 345;
   data_3.field_2 = 456;

   data_4.field_1 = 456;
   data_4.field_2 = 567;
   data_4.field_3 = 678;

   Peak_Message_Count = 9;
   Peak_Message_Size = sizeof(data_2);

   /* send messages */
   SAL_Sleep(500);

   SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
   SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));

   SAL_Sleep(500);

   SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));

   if (param_list->test_case_nr == 1)
   {

      SAL_Send(APP_ID, REC_THREAD_1, EV_E1, &data_1, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E2, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E3, &data_3, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E4, &data_4, sizeof(Sample_Msg_T_2));
   }
   else
   {
      SAL_Send(APP_ID, REC_THREAD_1, EV_E5, &data_1, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E6, &data_2, sizeof(Sample_Msg_T_2));
      SAL_Send(APP_ID, REC_THREAD_1, EV_E7, &data_3, sizeof(Sample_Msg_T_1));

      SAL_Sleep(500);

      SAL_Send(APP_ID, REC_THREAD_1, EV_E8, &data_4, sizeof(Sample_Msg_T_2));
   }

   SAL_Send(APP_ID, REC_THREAD_1, EV_E9, NULL, 0);

   Tr_Info_Lo("Sender_Thread_Second: Stop");

}


#if defined (XSAL)

/**  Function of the RECEIVE_FROM_THREAD (only for XSAL).
 *
 *   - takes out the messages from the queue of QUEUE_THREAD
 */

void Receive_From_Thread(void* param)
{
   Param_List_T* param_list = (Param_List_T*)param;
   const SAL_Message_T* msg;
   Sample_Msg_T_1* data_1;
   Sample_Msg_T_2* data_2;
   SAL_Stat_Queue_T stat;

   Tr_Info_Lo("Receive_From_Thread: Start");

   /* create the queue */
   if (!SAL_Create_Queue(1, 0, malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   /* wait for message from REC_THREAD_1 that it has finished receiving messages */
   msg = SAL_Receive();

   /* error occured in REC_THREAD_1 */
   if (msg->event_id == (SAL_Event_Id_T)EV_ERROR)
   {
      PBC_Failed("Received message that error occured");
   } /* invalid message */
   else if (msg->event_id != (SAL_Event_Id_T)EV_E1)
   {
      PBC_Failed("Invalid control message received");
   }
   else
   {
   }

   /* receive all remaining messages from queue of REC_THREAD_1 */
   do
   {
      SAL_Stat_Queue(REC_THREAD_1, &stat);

      if ((param_list->test_nr == 1) || (param_list->test_nr == 4))
      { 
         msg = SAL_Receive_From(REC_THREAD_1);

         /* received message is NULL */
         if (msg == NULL)
         {
            PBC_Failed("Received message is NULL");
         }
      }

      if ((param_list->test_nr != 1) && (param_list->test_nr != 4))
      {
         msg = SAL_Receive_From_Timeout(REC_THREAD_1, Long_Timeout);

         /* received message is NULL */
         if (msg == NULL)
         {
            PBC_Failed("Timeout has been reached");
         } /* received message that error occured */
         else if (msg->event_id == (SAL_Event_Id_T)EV_ERROR)
         {
            PBC_Failed("Received message that error occured");
         }
           
      }

      /* print received message */  
      if ((msg != NULL) && (msg->event_id != (SAL_Event_Id_T)EV_E9))
      {
         if (msg->event_id % 2 == 1)
         {
            data_1 = (Sample_Msg_T_1*)msg->data;

            Tr_Info_Mid_2("Receive_From_Thread: field_1: %d, field_2: %d\n", 
                        (int)data_1->field_1, (int)data_1->field_2);
         }
         else /* msg->event_id % 2 == 0 */
         {
            data_2 = (Sample_Msg_T_2*)msg->data;

            Tr_Info_Mid_3("Receive_From_Thread: field_1: %d, field_2: %d, field_3: %d\n", 
                        (int)data_2->field_1, (int)data_2->field_2, (int)data_2->field_3);
         }
      }

   }
   /* until message is equal EV_E9 (the last one) */
   while ((int) msg->event_id != EV_E9);

   SAL_Stat_Queue(REC_THREAD_1, &stat);

   /* check if message count is valid */
   if (stat.message_count != 0)
   {
      PBC_Failed_1("Invalid message count in SAL_Stat_Queue_T structure: %d", (int)stat.message_count);
   }

   /* check if peak message count is valid */
   if (stat.peak_message_count > Peak_Message_Count)
   {
      PBC_Failed_2("Invalid maximum number of messages in the queue in SAL_Stat_Queue_T structure: %d, Peak_Message_Count: %d", 
                 (int)stat.peak_message_count, (int)Peak_Message_Count);
   }

   /* check if peak message size is valid */
   if (stat.peak_message_size != Peak_Message_Size)
   {
      PBC_Failed("Invalid maximum message size in SAL_Stat_Queue_T structure");
   }

   Tr_Info_Lo("Receive_From_Thread: Stop");

}

#endif

/**  Function of test case.
 *
 *   Function creates four threads: SENDER_THREAD, RECEIVE_FROM_THREAD1, RECEIVE_FROM_THREAD2, QUEUE_THREAD
 *   and starts SAL_Run.
 */

SAL_Int_T Receive_Only_Test(void* sender_fun, void* receiver_fun, void* param)
{
#if defined (XSAL)
   Param_List_T* param_list = (Param_List_T*)param;
   SAL_Thread_Id_T destroyed_list_1[] = { SENDER_THREAD, REC_THREAD_1, REC_THREAD_2 };
#endif
   SAL_Thread_Id_T destroyed_list_2[] = { SENDER_THREAD, REC_THREAD_1 };
   SAL_Thread_Attr_T thread_attr;
   SAL_Mutex_Attr_T mutex_attr;
   SAL_Int_T ret = 0;
   SAL_Priority_T priority;
   
   SAL_Init_Mutex_Attr(&mutex_attr);

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      PBC_Failed("Mutex has not been created");
   }

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      receiver_fun,
      param,
      Init_Thread_Attr("REC1", REC_THREAD_1, priority, &thread_attr));

#if defined (XSAL)

   if (param_list->test_nr != 3)
   {

      SAL_Create_Thread(
         Receive_From_Thread,
         param,
         Init_Thread_Attr("REC2", REC_THREAD_2, priority, &thread_attr));
   }

#endif

   SAL_Create_Thread(
      sender_fun, 
      param,
      Init_Thread_Attr("SENDER", SENDER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   if (param_list->test_nr != 3)
   {
      SAL_Wait_Destroyed(destroyed_list_1, sizeof(destroyed_list_1)/sizeof(SAL_Thread_Id_T));
   }
   else
   {
      SAL_Wait_Destroyed(destroyed_list_2, sizeof(destroyed_list_2)/sizeof(SAL_Thread_Id_T));
   }

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list_2, 
           sizeof(destroyed_list_2)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

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


int main (int argc, char** argv)
{
   SAL_Int_T ret = 0;
   Param_List_T param_list;

   if (Set_Up())
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   /* FIRST SENT, THEN RECEIVED */

#if defined (XSAL)

   /* without timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 1;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_1, (void*)&param_list);   

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 1;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_1, (void*)&param_list);   

#endif

   /* with long timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 2;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_2, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 2;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_First, Receive_Only_Thread_2, &param_list);

   /* with short timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 3;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_3, Receive_Only_Thread_3, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 3;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_3, Receive_Only_Thread_3, &param_list);

   /* FIRST WAITING FOR RECEIVING, THEN SENT */

#if defined (XSAL)

   /* without timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 4;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_4, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 4;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_4, &param_list);

#endif

   /* with long timeout */

   param_list.send_event_list = Send_Event_List_Test_Case_1;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_1;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_1)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = 2 * param_list.rec_event_list_size;
   param_list.test_nr = 5;
   param_list.test_case_nr = 1;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_5, &param_list);

   param_list.send_event_list = Send_Event_List_Test_Case_2;
   param_list.send_event_list_size = sizeof(Send_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list = Rec_Event_List_Test_Case_2;
   param_list.rec_event_list_size = sizeof(Rec_Event_List_Test_Case_2)/sizeof(SAL_Event_Id_T);
   param_list.rec_event_list_number = param_list.rec_event_list_size;
   param_list.test_nr = 5;
   param_list.test_case_nr = 2;

   ret = ret || Receive_Only_Test(Sender_Thread_Second, Receive_Only_Thread_5, &param_list);

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
 * -----------------------------------------------------------------------*/
