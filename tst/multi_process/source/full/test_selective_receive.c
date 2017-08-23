/* -------------------------------------------------------------------------
 *
 * File:        test_selective_receive.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (Jacek.Roman@delphi.com)
 *
 * Description: Unit tests of Selective Receive.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pbc_trace.h"
#include "xsal.h"
#include "pi_unistd.h"

EM_FILENUM(TEST_MODULE2_ID, 2);

/** \file
 *
 *  Test of Selective receive functionality.
 *
 * \section T Tests:
 * <OL>
 * <LI> \ref TEST_SAL_RECEIVE test
 * <LI> \ref TEST_SAL_TRY_RECEIVE
 * <LI> \ref TEST_SAL_RECEIVE_TIMEOUT
 * </OL>
 *
 *
 *  \section TEST_SAL_RECEIVE SAL_Receive()
 *  Test consist of two test procedures: \ref RECEIVE_TP1 and \ref RECEIVE_TP2.
 *  Before test is started there:
 *  <OL>
 *  <LI> Two threads are created: <B>SENDER</B> and <B>RECEIVER</B>
 *  <LI> <B>SENDER</B> thread waits until <B>RECEIVER</B> creates message queue
 *  <LI> Receiver thread creates queue
 *  </OL>
 
 *  \section RECEIVE_TP1 Test procedure 1
 *  <OL>
 *  <LI> Receiver thread waits until Sender sends messages to its queue
 *  <LI> Sender thread send <B>SEND_MSG</B> and signal receiver
 *  <LI> Receiver thread call SAL_Select with <B>SELECTED_MSG</B>
 *  <LI> Receiver thread call SAL_Receive and receive <B>SELECTED_MSG</B>
 *  <LI> Receiver thread call SAL_Select with empty event list 
 *  <LI> Receiver thread call SAL_Receive and receive <B>RECEIVE_MSG</B>
 *  </OL>
 *
 *  \section RECEIVE_TP2 Test procedure 2
 *  <OL>
 *  <LI> SENDER thread wait until RECEIVER thread is ready to receiving messages
 *  <LI> RECEIVER thread signal that it is ready to receive messages
 *  <LI> SENDER thread sends <B>SEND_MSG</B> to RECEIVER
 *  <LI> RECEIVER thread call SAL_Select with <B>SELECTED_MSG</B>
 *  </OL>
 *
 *   <TABLE>
 *   <TR><TD><CENTER><B>TC</B></CENTER></TD> <TD><CENTER><B>SEND_MSG</B></CENTER></TD>     <TD><CENTER><B>SELECTED_MSG</B></CENTER></TD> <TD><CENTER><B>RECEIVED_MSG</B></CENTER></TD></TR>
 *   <TR><TD><CENTER>1</CENTER></TD> <TD>1, 2, 3, 4, 5, 6, 7, 8, 9, 10</TD> <TD>1, 2, 3, 4</TD>     <TD>1, 2, 3, 4, 5, 6, 7, 8, 9, 10</TD> </TR>
 *   <TR><TD><CENTER>2</CENTER></TD> <TD>1, 2, 3, 4, 5, 6, 7, 8, 9, 10</TD> <TD>2, 4, 6, 8, 10</TD> <TD>2, 4, 6, 8, 10, 1, 3, 5, 7, 9</TD> </TR>
 *   <TR><TD><CENTER>3</CENTER></TD> <TD>1, 2, 3, 4, 5, 6, 7, 8, 9, 10</TD> <TD>10, 9, 8, 7, 6</TD> <TD>6, 7, 8, 9, 10, 1, 2, 3, 4, 5</TD> </TR>
 *   </TABLE>
 *
 *  \section TEST_SAL_TRY_RECEIVE SAL_Try_Receive()
 *  Test procedure
 *
 *  \section TEST_SAL_RECEIVE_TIMEOUT SAL_Receive_Timeout()
 *  Test procedure
 *
 */


#define APP_TEST_LOCAL    2


/* Thread ids */
typedef enum Thread_Id_Tag
{
   SENDER     = 1,
   RECEIVER   = 2,

   QUEUE_OWNER = 1,
   REC_FROM_1  = 2,
   REC_FROM_2  = 3,
   REC_FROM_3  = 4,
 
   LAST_THREAD_ID = 4
} Thread_Id_T;


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

 
/**  Function of RECEIVER.
 *
 *   - receives message with timestamp and checks if the time is correct
 */


typedef struct Test_Case_Tag
{
   SAL_Event_Id_T* send;
   size_t          send_no;

   SAL_Event_Id_T* select;
   size_t          select_no;

   SAL_Event_Id_T* receive;
   size_t          receive_no;
} Test_Case_T;


/**
 * Test Procedure 1
 *
 *  TestCase 1-3
 */
SAL_Event_Id_T TP1_TC1_Send[]    = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

SAL_Event_Id_T TP1_TC1_Select[]  = { 1, 2, 3, 4 };
SAL_Event_Id_T TP1_TC1_Receive[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };


SAL_Event_Id_T TP1_TC2_Select[]  = { 2, 4, 6, 8, 10 };
SAL_Event_Id_T TP1_TC2_Receive[] = { 2, 4, 6, 8, 10, 1, 3, 5, 7, 9 };

SAL_Event_Id_T TP1_TC3_Select[]  = { 10, 9, 8, 7, 6 };
SAL_Event_Id_T TP1_TC3_Receive[] = { 6, 7, 8, 9, 10, 1, 2, 3, 4, 5 };


const Test_Case_T Test_Case[] = 
{
   { 
      TP1_TC1_Send,    sizeof(TP1_TC1_Send)/sizeof(SAL_Event_Id_T),
      TP1_TC1_Select,  sizeof(TP1_TC1_Select)/sizeof(SAL_Event_Id_T),
      TP1_TC1_Receive, sizeof(TP1_TC1_Receive)/sizeof(SAL_Event_Id_T)
   },
   { 
      TP1_TC1_Send,    sizeof(TP1_TC1_Send)/sizeof(SAL_Event_Id_T),
      TP1_TC2_Select,  sizeof(TP1_TC2_Select)/sizeof(SAL_Event_Id_T),
      TP1_TC2_Receive, sizeof(TP1_TC2_Receive)/sizeof(SAL_Event_Id_T)
   },
   { 
      TP1_TC1_Send,    sizeof(TP1_TC1_Send)/sizeof(SAL_Event_Id_T),
      TP1_TC3_Select,  sizeof(TP1_TC3_Select)/sizeof(SAL_Event_Id_T),
      TP1_TC3_Receive, sizeof(TP1_TC3_Receive)/sizeof(SAL_Event_Id_T)
   }
};



/***************************
 *
 *    Test SAL_Receive()
 *
 ***************************/

void TC_Receive(const Test_Case_T* tc)
{
   size_t event_no = 0;
   bool_t receiving = true;

   if (!SAL_Select(tc->select, tc->select_no))
   {
      PBC_Failed("TC_Receive: SAL_Select failed");
   }

   while(receiving)
   {
      const SAL_Message_T* msg = SAL_Receive();
      Tr_Info_Mid_1("TC_Receive event %d", (int)msg->event_id);
      if (msg->event_id != tc->receive[event_no])
      {
         PBC_Failed_2("TC_Receive: Wrong message received: %d instead %d", (int)msg->event_id, (int)tc->receive[event_no]);
      }

      event_no++;

      if (event_no == tc->select_no)
      {
         Tr_Info_Mid("TC_Receive: SAL_Unselect");
         SAL_Unselect();
      }
      else if (event_no == tc->receive_no)
      {
         SAL_Stat_Queue_T queue_stat;

         SAL_Stat_Queue(SAL_Get_Thread_Id(), &queue_stat);
         if (queue_stat.message_count != 0)
         {
            PBC_Failed("TC_Receive: Queue is NOT empty");
         }

         Tr_Info_Mid("TC_Receive: stop receiving messages");
         receiving = false;
      }
   } while(receiving);
}


void Test_Receive_Receiver_Thread(void* param)
{
   size_t tc_idx;
   static SAL_Thread_Id_T wait_list[] = { SENDER };

   Tr_Info_Mid("Test_Receive_Receiver_Thread[] Start");

   if (!SAL_Create_Queue(16, 16, malloc, free))
   {
      SAL_Exit(1);
   }

   (void)SAL_Signal_Ready();
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 1000))
   {
      PBC_Failed("SAL_Wait_Ready_Timeout");
   }

   /** Receiver test procedure 1
    */
   Tr_Info_Mid("Receiver TP1");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      (void)SAL_Signal_Semaphore(&Receiver_Ready);
      if (!SAL_Wait_Semaphore_Timeout(&Sender_Ready, 1000))
      {
         PBC_Failed("SAL_Wait_Ready_Timeout");
      }
      TC_Receive(&Test_Case[tc_idx]);
   }

   /** Receiver test procedure 2
    */
   Tr_Info_Mid("Receiver TP2");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      (void)SAL_Signal_Semaphore(&Receiver_Ready);
      TC_Receive(&Test_Case[tc_idx]);
   }

   Tr_Info_Mid("Test_Receive_Receiver_Thread[] Stop");
}


void Test_Receive_Sender_Thread(void* param)
{
   size_t tc_idx;
   static SAL_Thread_Id_T wait_list[] = { RECEIVER };

   Tr_Info_Mid("Test_Receive_Sender_Thread[] Start");

   SAL_Signal_Ready();
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));


   /** Sender test procedure 1
    */
   Tr_Info_Mid("Sender TP1");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      size_t i;
      const Test_Case_T* tc = &Test_Case[tc_idx];

      (void)SAL_Wait_Semaphore(&Receiver_Ready);

      for(i=0; i < tc->send_no; i++)
      {
         if (!SAL_Send(SAL_Get_App_Id(), RECEIVER, tc->send[i], NULL, 0))
         {
            PBC_Failed("SAL_Send failed");
         }
      }
      SAL_Signal_Semaphore(&Sender_Ready);
   }


   /** Sender test procedure 2
    */
   Tr_Info_Mid("Sender TP2");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      size_t i;
      const Test_Case_T* tc = &Test_Case[tc_idx];

      if (!SAL_Wait_Semaphore_Timeout(&Receiver_Ready, 10000))
      {
         PBC_Failed("SAL_Wait_Semaphore_Timeout");
      }

      for(i=0; i < tc->send_no; i++)
      {
         if (!SAL_Send(SAL_Get_App_Id(), RECEIVER, tc->send[i], NULL, 0))
         {
            PBC_Failed("SAL_Send failed");
         }
      }
   }
   Tr_Info_Mid("Test_Receive_Sender_Thread[] Stop");
}


bool_t Test_Receive(void)
{
   bool_t status;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   Tr_Info_Hi("Start Test_Receive");

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);
 
   SAL_Create_Thread(
      Test_Receive_Receiver_Thread, 
      NULL, 
      Init_Thread_Attr("RecRec", RECEIVER, priority, &thread_attr));

   SAL_Create_Thread(
      Test_Receive_Sender_Thread, 
      NULL, 
      Init_Thread_Attr("RecSnd", SENDER, priority, &thread_attr));

   status = (bool_t)(SAL_Run() == 0);
   Tr_Info_Hi_1( "Test_Receive %s", (status?"PASSED":"FAILED"));
   return status;
}


/****************************
 *
 *   Test SAL_Try_Receive()
 *
 ****************************/


void TC_Try_Receive(const Test_Case_T* tc)
{
   bool_t receiving = true;
   size_t event_no = 0;

   if (!SAL_Select(tc->select, tc->select_no))
   {
      PBC_Failed("TC_Try_Receive: SAL_Select failed");
   }

   while(receiving)
   {
      const SAL_Message_T* msg = SAL_Try_Receive();
      if (msg != NULL)
      {
         Tr_Info_Mid_1("TC_Try_Receive, event %d", (int)msg->event_id);
         if (msg->event_id != tc->receive[event_no])
         {
            PBC_Failed_2("TC_Try_Receive, wrong message received: %d instead %d", (int)msg->event_id, (int)tc->receive[event_no]);
         }
         event_no++;
      }
      else
      {
         Tr_Info_Mid("TC_Try_Receive, returns NULL");
         if (event_no == tc->select_no)
         {
            Tr_Info_Mid("TC_Try_Receive: SAL_Unselect");
            SAL_Unselect();
         }
         else if (event_no == tc->receive_no)
         {
            SAL_Stat_Queue_T queue_stat;

            SAL_Stat_Queue(SAL_Get_Thread_Id(), &queue_stat);
            if (queue_stat.message_count != 0)
            {
               PBC_Failed("TC_Try_Receive: Queue is NOT empty");
            }
         
            Tr_Info_Mid("TC_Try_Receive: stop receiving messages");
            receiving = false;
         }
         else
         {
            SAL_Sleep(0);
         }
      }
   } while(receiving);
}


void Test_Try_Receive_Receiver_Thread(void* param)
{
   size_t tc;
   static SAL_Thread_Id_T wait_list[] = { SENDER };

   Tr_Info_Mid("Test_Try_Receive_Receiver_Thread[] Start");

   if (!SAL_Create_Queue(16, 16, malloc, free))
   {
      SAL_Exit(1);
   }

   (void)SAL_Signal_Ready();
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 1000))
   {
      PBC_Failed("SAL_Wait_Ready_Timeout");
   }


   /** Receiver Timeout test procedure 1
    */
   Tr_Info_Mid("Try_Receive_Receiver TP1");
   for(tc = 0; tc < sizeof(Test_Case)/sizeof(Test_Case[0]); tc++)
   {
      (void)SAL_Signal_Semaphore(&Receiver_Ready);
      if (!SAL_Wait_Semaphore_Timeout(&Sender_Ready, 1000))
      {
         PBC_Failed("SAL_Wait_Ready_Timeout");
      }
      TC_Try_Receive(&Test_Case[tc]);
   }


   /** Receiver Timeout test procedure 2
    */
   Tr_Info_Mid("Try_Receive_Receiver TP2");
   for(tc = 0; tc < sizeof(Test_Case)/sizeof(Test_Case[0]); tc++)
   {
      (void)SAL_Signal_Semaphore(&Receiver_Ready);
      TC_Try_Receive(&Test_Case[tc]);
   }

   Tr_Info_Mid("Test_Try_Receive_Receiver_Thread[] Stop");
}


void Test_Try_Receive_Sender_Thread(void* param)
{
   size_t tc_idx;
   static SAL_Thread_Id_T wait_list[] = { RECEIVER };

   Tr_Info_Mid("Test_Try_Receive_Sender_Thread[] Start");

   SAL_Signal_Ready();
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));


   /** Sender Timeout test procedure 1
    */
   Tr_Info_Mid("Try_Receive_Sender TP1");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      size_t i;
      const Test_Case_T* tc = &Test_Case[tc_idx];

      (void)SAL_Wait_Semaphore(&Receiver_Ready);

      for(i=0; i < tc->send_no; i++)
      {
         if (!SAL_Send(SAL_Get_App_Id(), RECEIVER, tc->send[i], NULL, 0))
         {
            PBC_Failed("SAL_Send failed");
         }
      }

      SAL_Signal_Semaphore(&Sender_Ready);
   }

   /** Sender Timeout test procedure 2
    */
   Tr_Info_Mid("Try_Receive_Sender TP2");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      size_t i;
      const Test_Case_T* tc = &Test_Case[tc_idx];

      if (!SAL_Wait_Semaphore_Timeout(&Receiver_Ready, 10000))
      {
         PBC_Failed("TP1_Sender_Timeout: SAL_Wait_Semaphore_Timeout");
      }

      for(i=0; i < tc->send_no; i++)
      {
         if (!SAL_Send(SAL_Get_App_Id(), RECEIVER, tc->send[i], NULL, 0))
         {
            PBC_Failed("SAL_Send failed");
         }
      }
   }

   Tr_Info_Mid("Test_Try_Receive_Sender_Thread[] Stop");
}


bool_t Test_Try_Receive(void)
{
   bool_t status;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   Tr_Info_Hi("Start Test_Try_Receive");

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);
 
   SAL_Create_Thread(
      Test_Try_Receive_Receiver_Thread, 
      NULL, 
      Init_Thread_Attr("TryRecRe", RECEIVER, priority, &thread_attr));

   SAL_Create_Thread(
      Test_Try_Receive_Sender_Thread, 
      NULL, 
      Init_Thread_Attr("TryRecSn", SENDER, priority, &thread_attr));

   status = (bool_t)(SAL_Run() == 0);
   Tr_Info_Hi_1( "Test_Try_Receive %s", (status?"PASSED":"FAILED"));
   return status;
}


/********************************
 *
 *  Test SAL_Receive_Timeout()
 *
 ********************************/


void TC_Receive_Timeout(const Test_Case_T* tc)
{
   bool_t receiving = true;
   size_t event_no = 0;

   if (!SAL_Select(tc->select, tc->select_no))
   {
      PBC_Failed("TC_Receive_Timeout: SAL_Select failed");
   }

   while(receiving)
   {
      const SAL_Message_T* msg = SAL_Receive_Timeout(500);
      if (msg != NULL)
      {
         Tr_Info_Mid_1("TC_Receive_Timeout, event %d", (int)msg->event_id);
         if (msg->event_id != tc->receive[event_no])
         {
            PBC_Failed_2("TC_Receive_Timeout, wrong message received: %d instead %d", (int)msg->event_id, (int)tc->receive[event_no]);
         }
         event_no++;
      }
      else
      {
         Tr_Info_Mid("TC_Receive_Timeout, timeout(500)");
         if (event_no == tc->select_no)
         {
            Tr_Info_Mid("TC_Receive_Timeout: SAL_Unselect");
            SAL_Unselect();
         }
         else if (event_no == tc->receive_no)
         {
            Tr_Info_Mid("TC_Receive_Timeout: stop receiving messages");
            receiving = false;
         }
         else
         {
            PBC_Failed("TC_Receive_Timeout: expected message");
         }
      }
   } while(receiving);
}


void Test_Receive_Timeout_Receiver_Thread(void* param)
{
   size_t tc;
   static SAL_Thread_Id_T wait_list[] = { SENDER };

   Tr_Info_Mid("Test_Receive_Timeout_Receiver_Thread[] Start");

   if (!SAL_Create_Queue(16, 16, malloc, free))
   {
      SAL_Exit(1);
   }

   (void)SAL_Signal_Ready();
   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 1000))
   {
      PBC_Failed("SAL_Wait_Ready_Timeout");
   }


   /** Receiver Timeout test procedure 1
    */
   Tr_Info_Mid("Receiver_Timeout TP1");
   for(tc = 0; tc < sizeof(Test_Case)/sizeof(Test_Case[0]); tc++)
   {
      (void)SAL_Signal_Semaphore(&Receiver_Ready);
      if (!SAL_Wait_Semaphore_Timeout(&Sender_Ready, 1000))
      {
         PBC_Failed("SAL_Wait_Ready_Timeout");
      }
      TC_Receive_Timeout(&Test_Case[tc]);
   }


   /** Receiver Timeout test procedure 2
    */
   Tr_Info_Mid("Receiver_Timeout TP2");
   for(tc = 0; tc < sizeof(Test_Case)/sizeof(Test_Case[0]); tc++)
   {
      (void)SAL_Signal_Semaphore(&Receiver_Ready);
      TC_Receive_Timeout(&Test_Case[tc]);
   }

   Tr_Info_Mid("Test_Receive_Timeout_Receiver_Thread[] Stop");
}


void Test_Receive_Timeout_Sender_Thread(void* param)
{
   size_t tc_idx;
   static SAL_Thread_Id_T wait_list[] = { RECEIVER };

   Tr_Info_Mid("Test_Receive_Timeout_Sender_Thread[] Start");

   SAL_Signal_Ready();
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));


   /** Sender Timeout test procedure 1
    */
   Tr_Info_Mid("Sender_Timeout TP1");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      size_t i;
      const Test_Case_T* tc = &Test_Case[tc_idx];

      (void)SAL_Wait_Semaphore(&Receiver_Ready);

      for(i=0; i < tc->send_no; i++)
      {
         if (!SAL_Send(SAL_Get_App_Id(), RECEIVER, tc->send[i], NULL, 0))
         {
            PBC_Failed("SAL_Send failed");
         }
      }

      SAL_Signal_Semaphore(&Sender_Ready);
   }

   /** Sender Timeout test procedure 2
    */
   Tr_Info_Mid("Sender_Timeout TP2");
   for(tc_idx = 0; tc_idx < sizeof(Test_Case)/sizeof(Test_Case[0]); tc_idx++)
   {
      size_t i;
      const Test_Case_T* tc = &Test_Case[tc_idx];

      if (!SAL_Wait_Semaphore_Timeout(&Receiver_Ready, 10000))
      {
         PBC_Failed("TP1_Sender_Timeout: SAL_Wait_Semaphore_Timeout");
      }

      for(i=0; i < tc->send_no; i++)
      {
         if (!SAL_Send(SAL_Get_App_Id(), RECEIVER, tc->send[i], NULL, 0))
         {
            PBC_Failed("SAL_Send failed");
         }
      }
   }

   Tr_Info_Mid("Test_Receive_Timeout_Sender_Thread[] Stop");
}


bool_t Test_Receive_Timeout(void)
{
   bool_t status;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   Tr_Info_Hi("Start Test_Receive_Timeout");

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);
 
   SAL_Create_Thread(
      Test_Receive_Timeout_Receiver_Thread, 
      NULL, 
      Init_Thread_Attr("TP1RecTm", RECEIVER, priority, &thread_attr));

   SAL_Create_Thread(
      Test_Receive_Timeout_Sender_Thread, 
      NULL, 
      Init_Thread_Attr("TP1SndTm", SENDER, priority, &thread_attr));

   status = (bool_t)(SAL_Run() == 0);
   Tr_Info_Hi_1( "Test_Receive_Timeout %s", (status?"PASSED":"FAILED"));
   return status;
}



/****************************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
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


int main (int argc, char** argv)
{
   bool_t status = true;

   if (Set_Up(argc, argv))
   {
      if (!Test_Receive())
      {
         status = false;
      }

      if (!Test_Try_Receive())
      {
         status = false;
      }

      if (!Test_Receive_Timeout())
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

   Tr_Info_Hi_1("Test %s", ((status)?("PASSED"):("FAILED")));

   return status ? 0 : 1;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  01 Sep 2006, Jacek Roman (Jacek.Roman@delphi.com)
 *   Initial revision
 *
 * -----------------------------------------------------------------------*/
