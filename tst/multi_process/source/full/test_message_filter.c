/* -------------------------------------------------------------------------
 *
 * File:        test_message_filter.c
 *
 * Copyright:   2008 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Leszek Plonka (Leszek.Plonka@delphi.com) and Jacek Roman (Jacek.Roman@delphi.com)
 *
 * Description: Unit tests of message filters.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbc_trace.h"
#include "xsal.h"
#include "pi_unistd.h"

EM_FILENUM(TEST_MODULE_ID, 14);

/** \file
 *
 *  Test of Message Filter functionality.
 *
 * \section T Tests:
 * <OL>
 * <LI> \ref TEST_MESSAGE_FILTER test
 * </OL>
 *
 *
 *  \section TEST_MESSAGE_FILTER SAL_Receive()
 *  Test consists of test procedures: \ref RECEIVE_TP1.
 *  Before the test is started:
 *  <OL>
 *  <LI> Two threads are created: <B>SENDER</B> and <B>RECEIVER</B>
 *  <LI> Sender thread waits for the receiver to create its message queue
 *  <LI> Receiver thread creates its message queue
 *  <LI> Receiver thread installs a message filter
 *  </OL>
 *
 *  \section RECEIVE_TP1 Test procedure 1
 *  <OL>
 *  <LI> Receiver thread waits until the Sender sends messages to its queue
 *  <LI> Receiver thread calls SAL_Receive and receives the messages
 *  </OL>
 */


#define APP_TEST_LOCAL    2


/* Thread ids */
typedef enum Thread_Id_Tag
{
   SENDER     = 1,
   RECEIVER   = 2,

   LAST_THREAD_ID = 2
} Thread_Id_T;


/* Help synchronize the sender and the receiver between test cases. */
SAL_Semaphore_T Sender_Ready;
SAL_Semaphore_T Receiver_Ready;


const SAL_Thread_Attr_T* Init_Thread_Attr(
   const char_t* name, 
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


typedef struct Test_Case_Tag
{
   SAL_Event_Id_T* send;
   int*            send_data;   
   size_t          send_no;

   SAL_Event_Id_T* receive;
   int*            receive_data;
   size_t          receive_no;
   
   void (*message_filter)(
      SAL_Thread_Id_T thread_id,
      const SAL_Message_T* message,
      SAL_Queue_Iterator_T* iterator);

} Test_Case_T;


typedef struct Test_Message_Tag
{
   int param1;
}
Test_Message_T;


/* Test Procedure 1 */

/* TestCase 1 */
SAL_Event_Id_T TP1_TC1_Send[]    = {1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
/*SAL_Event_Id_T TP1_TC1_Receive[] = {1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};*/
SAL_Event_Id_T TP1_TC1_Receive[] = {1, 3, 4, 5, 6, 7, 8, 9, 10};

int TP1_TC1_Send_Data[]    = {1, 21, 22, 23, 3, 4, 5, 6, 7, 8, 9, 10};
/*int TP1_TC1_Receive_Data[] = {1, 21, 22, 23, 3, 4, 5, 6, 7, 8, 9, 10};*/
int TP1_TC1_Receive_Data[] = {1, 3, 4, 5, 6, 7, 8, 9, 10};


void TP1_TC1_Filter(
   SAL_Thread_Id_T thread_id,
   const SAL_Message_T* message,
   SAL_Queue_Iterator_T* iterator)
{
   Tr_Info_Mid_1("TP1_TC1_Filter, event_id %d", (int)message->event_id);
   /* Drop all events whose event_id == 2.*/
   if (message->event_id == 2)
   {
      /* The following line of code was commented out because it was
       * causing a warning. I suspect that the prototype for this function
       * should be changed to SAL_Queue_Iterator_T** iterator, and the
       * following line changed to *iterator = NULL. However, this test
       * case is not even being run, and when it is run, it fails differently
       * so it is not clear what the intent of this test even was. Given
       * that this building block has reached end of life, this change is
       * being made to eliminate the warning and no effort is being made
       * to fix the test in the first place, which has never been run.
       */
      /*iterator = NULL;*/
   }
}

/* TestCase 2 */
SAL_Event_Id_T TP1_TC2_Send[]    = {1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};
/*SAL_Event_Id_T TP1_TC2_Receive[] = {1, 2, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10};*/
SAL_Event_Id_T TP1_TC2_Receive[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

int TP1_TC2_Send_Data[]    = {1, 21, 22, 23, 3, 4, 5, 6, 7, 8, 9, 10};
/*int TP1_TC2_Receive_Data[] = {1, 21, 22, 23, 3, 4, 5, 6, 7, 8, 9, 10};*/
int TP1_TC2_Receive_Data[] = {1, 23, 3, 4, 5, 6, 7, 8, 9, 10};


void TP1_TC2_Filter(
   SAL_Thread_Id_T thread_id,
   const SAL_Message_T* message,
   SAL_Queue_Iterator_T* iterator)
{

   Tr_Info_Mid_1("TP1_TC2_Filter, event_id %d", (int)message->event_id);
   
   if (message->event_id == 2)
   {
      /* Scan the message queue and replace an old message whose event_id == 2
       * (if it exists) with this one.
       */
      SAL_Queue_Iterator_T iter;

      for (SAL_First_Message(&iter); !SAL_Is_Queue_End(&iter); SAL_Next_Message(&iter))
      {
         SAL_Event_Id_T event_id = SAL_Get_Message(&iter)->event_id;

         if (event_id == 2)
         {
            SAL_Remove_Message(&iter);
            *iterator = iter;
            break;
         }
      }
   }
}


const Test_Case_T Test_Case[] = 
{
   { 
      TP1_TC1_Send,    TP1_TC1_Send_Data,    sizeof(TP1_TC1_Send)/sizeof(TP1_TC1_Send[0]),
      TP1_TC1_Receive, TP1_TC1_Receive_Data, sizeof(TP1_TC1_Receive)/sizeof(TP1_TC1_Receive[0]),
      TP1_TC1_Filter,
   },
   { 
      TP1_TC2_Send,    TP1_TC2_Send_Data,    sizeof(TP1_TC2_Send)/sizeof(TP1_TC2_Send[0]),
      TP1_TC2_Receive, TP1_TC2_Receive_Data, sizeof(TP1_TC2_Receive)/sizeof(TP1_TC2_Receive[0]),
      TP1_TC2_Filter,
   },
};


void TC_Receive(int tc_idx)
{
   size_t event_no = 0;
   bool_t receiving = true;
   const Test_Case_T* tc = &Test_Case[tc_idx];

   while (receiving)
   {
      const SAL_Message_T* msg = SAL_Receive();
      Tr_Info_Mid_2("TC_Receive tc_idx = %d, event %d", (int)tc_idx, (int)msg->event_id);
      if (msg->event_id != tc->receive[event_no])
      {
         PBC_Failed_3("TC_Receive(tc_idx = %d): Wrong message received: %d instead of %d",
                     (int)tc_idx, (int)msg->event_id, (int)tc->receive[event_no]);
      }

      if (((Test_Message_T*)(msg->data))->param1 != tc->receive_data[event_no])
      {
         PBC_Failed_4("TC_Receive(tc_idx = %d): Wrong data received in message %d: %d instead of %d",
                      (int)tc_idx, (int)msg->event_id,
                      (int)((Test_Message_T*)(msg->data))->param1, (int)tc->receive_data[event_no]);
      }

      event_no++;

      if (event_no == tc->receive_no)
      {
         SAL_Stat_Queue_T queue_stat;

         SAL_Stat_Queue(SAL_Get_Thread_Id(), &queue_stat);
         if (queue_stat.message_count != 0)
         {
            PBC_Failed("TC_Receive: Queue is not empty");
         }

         Tr_Info_Mid("TC_Receive: stop receiving messages");
         receiving = false;
      }
   } while (receiving);
}


void Test_Receiver_Thread(void* param)
{
   size_t tc_idx;
   static SAL_Thread_Id_T wait_list[] = { SENDER };

   Tr_Info_Mid("Test_Receiver_Thread[] Start");

   if (!SAL_Create_Queue(16, 16, malloc, free))
   {
      PBC_Failed("SAL_Create_Queue");
   }

   (void)SAL_Signal_Ready();
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 1000))
   {
      PBC_Failed("SAL_Wait_Ready_Timeout");
   }

   /* Receiver test procedure 1. */
   Tr_Info_Mid("Receiver TP1");
   for (tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      /* Install the message filter. */
      /*SAL_Set_Message_Filter(Test_Case[tc_idx].message_filter);*/
   
      (void)SAL_Signal_Semaphore(&Receiver_Ready);
      if (!SAL_Wait_Semaphore_Timeout(&Sender_Ready, 1000))
      {
         PBC_Failed("SAL_Wait_Semaphore_Timeout");
      }
      TC_Receive(tc_idx);
   }

   Tr_Info_Mid("Test_Receiver_Thread[] Stop");
}


void Test_Sender_Thread(void* param)
{
   size_t tc_idx;
   static SAL_Thread_Id_T wait_list[] = { RECEIVER };

   Tr_Info_Mid("Test_Sender_Thread[] Start");

   SAL_Signal_Ready();
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   /* Sender test procedure 1. */
   Tr_Info_Mid("Sender TP1");
   for (tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      size_t i;
      const Test_Case_T* tc = &Test_Case[tc_idx];

      (void)SAL_Wait_Semaphore(&Receiver_Ready);

      for (i = 0; i < tc->send_no; i++)
      {
         Test_Message_T msg = {tc->send_data[i]};
         if (!SAL_Send(SAL_Get_App_Id(), RECEIVER, tc->send[i], &msg, sizeof(msg)))
         {
            PBC_Failed("SAL_Send failed");
         }
      }
      SAL_Signal_Semaphore(&Sender_Ready);
   }

}


bool_t Test_Message_Filter(void)
{
   bool_t status;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   Tr_Info_Hi("Start Test_Message_Filter");

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);
 
   SAL_Create_Thread(
      Test_Receiver_Thread, 
      NULL, 
      Init_Thread_Attr("Rec", RECEIVER, priority, &thread_attr));

   SAL_Create_Thread(
      Test_Sender_Thread, 
      NULL, 
      Init_Thread_Attr("Snd", SENDER, priority, &thread_attr));

   status = (bool_t)(SAL_Run() == 0);
   Tr_Info_Hi_1("Test_Message_Filter %s", (status?"PASSED":"FAILED"));
   return status;
}



/****************************************************************************
 * Sets up and executes the test.
 ****************************************************************************/


bool_t Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_Config_T config;
   SAL_Get_Config(&config);

   if (!SAL_Create_Semaphore(&Sender_Ready, NULL))
   {
      Tr_Fault("!SAL_Create_Semaphore");
      return false;
   }

   if (!SAL_Create_Semaphore(&Receiver_Ready, NULL))
   {
      Tr_Fault("!SAL_Create_Semaphore");
      return false;
   }

   config.app_id = APP_TEST_LOCAL;
   config.max_number_of_threads = LAST_THREAD_ID;

   if (!SAL_Init(&config))
   {
      Tr_Fault("SAL_Init error");
      return false;
   }

   return true;
}


bool_t Tear_Down(void)
{
   SAL_Destroy_Semaphore(&Sender_Ready);
   SAL_Destroy_Semaphore(&Receiver_Ready);
   
   return true;
}


int main (int argc, char* argv[])
{
   bool_t status = true;

   if (Set_Up(argc, argv))
   {
      if (!Test_Message_Filter())
      {
         status = false;
      }

      if (!Tear_Down())
      {
         Tr_Fault("Tear_Down failed");
         status = false;
      }
   }
   else
   {
      Tr_Fault("Set_Up failed");
      status = false;
   }


   return status ? 0 : 1;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  01 Apr 2008, Leszek Plonka (Leszek.Plonka@delphi.com) and Jacek Roman (Jacek.Roman@delphi.com)
 *   Initial revision
 *
 * -----------------------------------------------------------------------*/
