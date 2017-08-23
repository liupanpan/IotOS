/* -------------------------------------------------------------------------
 *
 * File:        test_callback.c
 *
 * Copyright:   2006-2014 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of callback for send.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include "xsal.h"
#include "pi_unistd.h"

EM_FILENUM(TEST_MODULE_ID, 8);

#define APP_LOCAL    2
#define APP_SENDER   2
#define APP_RECEIVER 3

#define MAX_NUMB_OF_THREADS 6

typedef enum Event_Id_Tag {
   EV_E1 = 1,
   EV_E2,
   EV_E3, 
   REMOTE_CONTROLLER_READY,
   REMOTE_RECEIVER_READY,
   REMOTE_SENDER_READY
} Event_Id_T;

typedef enum Thread_Id_Tag
{
   SENDER_THREAD = 1,
   RECEIVER_THREAD,
   SENDER_CONTROL,
   RECEIVER_CONTROL
} Thread_Id_T;

typedef enum E_Test_Type_Tag
{
   E_STAND_ALONE, 
   E_SENDER, 
   E_RECEIVER 
} E_Test_Type_T;

E_Test_Type_T Test_Type = E_STAND_ALONE;

SAL_Semaphore_T Sem, Destroyed_Sem;

SAL_Int_T Timeout = 3000;

/**  \file
 *
 *   Test of callback for send.
 *
 *   \section T1 Tests:
 *   <OL>
 *   <LI> \ref TL in one application. </LI>
 *   <LI> \ref TITA </LI>
 *   </OL>
 *
 *   \section T2 Tests:
 *   <OL>
 *   <LI> \ref NQT - test of no queue callback. </LI>
 *   <LI> \ref QFT - test of queue full callback. </LI>
 *   <LI> \ref NMT - test of no memory callback. </LI>
 *   <LI> \ref TSBT - test of too small buffer callback. </LI>
 *   </OL>
 *
 *   \section NQT No_Queue_Test 
 *
 *   Two threads are created: SENDER_THREAD and RECEIVER_THREAD. \n
 *   SENDER_THREAD tries to send message to RECEIVER_THREAD, which has 
 *   not created the queue.
 * 
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> SENDER_THREAD waits until RECEIVER_THREAD is ready. </LI>
 *   <LI> RECEIVER_THREAD signals that is ready. </LI>
 *   <LI> RECEIVER_THREAD waits until SENDER_THREAD terminates. </LI>
 *   <LI> SENDER_THREAD sends the message. </LI>
 *   </OL>
 *
 *   \section QFT Queue_Full_Test 
 *
 *   Two threads are created: SENDER_THREAD and RECEIVER_THREAD. \n
 *   SENDER_THREAD tries to send more messages than queue size of RECEIVER_THREAD.
 *
 *   \subsection TP2 Test procedure:
 *
 *   <OL>
 *   <LI> SENDER_THREAD waits until RECEIVER_THREAD is ready. </LI>
 *   <LI> RECEIVER_THREAD creates the queue of size 1. </LI>
 *   <LI> RECEIVER_THREAD signals that is ready. </LI>
 *   <LI> RECEIVER_THREAD waits until SENDER_THREAD terminates. </LI>
 *   <LI> SENDER_THREAD sends 2 messages. </LI>
 *   </OL>
 *
 *   \section NMT No_Memory_Test 
 *
 *   Two threads are created: SENDER_THREAD and RECEIVER_THREAD. \n
 *   SENDER_THREAD tries to send message of greater size that is allocated
 *   and there is no memory for allocation. As the third argument of 
 *   SAL_Create_Queue function there is Null_Fun which only returns 0 (this
 *   is simulation of situation when there is no memory to allocate).
 *
 *   \subsection TP3 Test procedure:
 *
 *   <OL>
 *   <LI> SENDER_THREAD waits until RECEIVER_THREAD is ready. </LI>
 *   <LI> RECEIVER_THREAD creates the queue with buffer_size = 0 and
 *        allocation function Null_Fun. </LI>
 *   <LI> RECEIVER_THREAD signals that is ready. </LI>
 *   <LI> RECEIVER_THREAD waits until SENDER_THREAD terminates. </LI>
 *   <LI> SENDER_THREAD sends message of size SAL_Int_T type. </LI>
 *   </OL>
 *
 *   \section TSBT Too_Small_Buffer_Test 
 *
 *   Two threads are created: SENDER_THREAD and RECEIVER_THREAD. \n
 *   RECEIVER_THREAD tries to send message of greater size that is allocated.
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> SENDER_THREAD waits until RECEIVER_THREAD is ready. </LI>
 *   <LI> RECEIVER_THREAD creates the queue with buffer_size = 0. </LI>
 *   <LI> RECEIVER_THREAD signals that is ready. </LI>
 *   <LI> RECEIVER_THREAD waits until SENDER_THREAD terminates. </LI>
 *   <LI> SENDER_THREAD sends message of size SAL_Int_T type. </LI>
 *   </OL>
 *
 *   \section TL Tested Locally
 *
 *   SENDER_THREAD and RECEIVER_THREAD are in one application.
 *
 *   \section TITA Tested In Two Applications
 *
 *   SENDER_THREAD and RECEIVER_THREAD are in two different applications.
 *   Comunication between these two threads is remote and there are
 *   additionally two control threads for synchronization, one for sender
 *   application and one for receiver application.
 *
 */


const SAL_Thread_Attr_T* Init_Thread_Attr(
         const char* name, 
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

void Callback_Fun(
   SAL_Thread_Id_T thread_id, 
   SAL_Message_Dropped_Reason_T reason,
   const SAL_Message_T* msg)
{
   Tr_Fault_2("Message dropped in thread: %d, reason: %d", (int)thread_id, (int)reason);
}


/**  \name Functions of Local_No_Queue_Test
 *
 *   \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Local_Receiver_No_Queue_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD };

   Tr_Info_Lo("Local_Receiver_No_Queue_Fun: Start");

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Local_Receiver_No_Queue_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the message
 */

void Local_Sender_No_Queue_Fun(void* param)
{
   SAL_Thread_Id_T wait_list[] = { RECEIVER_THREAD };

   Tr_Info_Lo("Local_Sender_No_Queue_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, 
        sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
        Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   SAL_Send(APP_LOCAL, RECEIVER_THREAD, EV_E1, NULL, 0);

   Tr_Info_Lo("Local_Sender_No_Queue_Fun: Stop");
}


/**  Test of SAL_No_Queue callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_No_Queue_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, SENDER_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Local_Receiver_No_Queue_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSRec1", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Local_Sender_No_Queue_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSend1", SENDER_THREAD, priority, &thread_attr));
   
   SAL_Set_Message_Dropped_Callback(Callback_Fun);
 
   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   return ret;
}

/**  \}
 */


/**  \name Functions of Local_Queue_Full_Test
 *
 *   \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates the queue of length 1
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Local_Receiver_Queue_Full_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD };

   Tr_Info_Lo("Local_Receiver_Queue_Full_Fun: Start");

   if (!SAL_Create_Queue(1, sizeof(SAL_Int_T), malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Local_Receiver_Queue_Full_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the messages
 */

void Local_Sender_Queue_Full_Fun(void* param)
{
   SAL_Thread_Id_T wait_list[] = { RECEIVER_THREAD };

   Tr_Info_Lo("Local_Sender_Queue_Full_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, 
        sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
        Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   SAL_Send(APP_LOCAL, RECEIVER_THREAD, EV_E1, NULL, 0);
   SAL_Send(APP_LOCAL, RECEIVER_THREAD, EV_E2, NULL, 0);
   SAL_Send(APP_LOCAL, RECEIVER_THREAD, EV_E3, NULL, 0);

   Tr_Info_Lo("Local_Sender_Queue_Full_Fun: Stop");
}


/**  Test of SAL_Queue_Full callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_Queue_Full_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, SENDER_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Local_Receiver_Queue_Full_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSRec2", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Local_Sender_Queue_Full_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSend2", SENDER_THREAD, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   return ret;
}

/**  \}
 */


/**  \name Functions of Local_No_Memory_Test
 *
 *   \{
 */

/**  Function of allocation memory.
 *
 *   - returns NULL
 */

void* Null_Fun(size_t size)
{
   return NULL;
}


/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue of buffer_size = 0 and with allocation 
 *     function = Null_Fun
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Local_Receiver_No_Memory_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD };

   Tr_Info_Lo("Local_Receiver_No_Memory_Fun: Start");

   if (!SAL_Create_Queue(1, 0, Null_Fun, NULL))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Timeout)) 
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Local_Receiver_No_Memory_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the message
 */

void Local_Sender_No_Memory_Fun(void* param)
{
   SAL_Thread_Id_T wait_list[] = { RECEIVER_THREAD };
   SAL_Int_T data;

   Tr_Info_Lo("Local_Sender_No_Memory_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, 
        sizeof(wait_list)/sizeof(SAL_Thread_Id_T),  
        Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   data = 5;

   SAL_Send(APP_LOCAL, RECEIVER_THREAD, EV_E1, &data, sizeof(SAL_Int_T));

   Tr_Info_Lo("Local_Sender_No_Memory_Fun: Stop");
}


/**  Test of SAL_No_Memory callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_No_Memory_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, SENDER_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Local_Receiver_No_Memory_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSRec3", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Local_Sender_No_Memory_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSend3", SENDER_THREAD, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   return ret;
}

/** \}
 */

/** \name Functions of Local_Too_Small_Buffer_Test
 *
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue of buffer_size = 0 and with no allocation 
 *     function
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Local_Receiver_Too_Small_Buffer_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD };

   Tr_Info_Lo("Local_Receiver_Too_Small_Buffer_Fun: Start");

   if (!SAL_Create_Queue(1, 0, NULL, NULL))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Local_Receiver_Too_Small_Buffer_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the message
 */

void Local_Sender_Too_Small_Buffer_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { RECEIVER_THREAD };
   SAL_Int_T* data;

   Tr_Info_Lo("Local_Sender_Too_Small_Buffer_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   data = (SAL_Int_T*) malloc(sizeof(SAL_Int_T));
   if (data != NULL)
   {
      *data = 5;
      SAL_Send(APP_LOCAL, RECEIVER_THREAD, EV_E1, data, sizeof(SAL_Int_T));
      free(data);
   }

   Tr_Info_Lo("Local_Sender_Too_Small_Buffer_Fun: Stop");
}


/**  Test of SAL_Too_Small_Buffer callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_Too_Small_Buffer_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, SENDER_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Local_Receiver_Too_Small_Buffer_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSRec4", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Local_Sender_Too_Small_Buffer_Fun, 
                     NULL, 
                     Init_Thread_Attr("LSend4", SENDER_THREAD, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
              sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
              Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   return ret;
}

/** \}
 */

/* ---------------------------------------------------------------------------- */
/*                         REMOTE COMMUNICATION                                 */
/* ---------------------------------------------------------------------------- */

/* CONTROL THREADS */

#if defined (XSAL)

/** \name Functions of remote control threads
 *  \{
 */

/** Function of receiver application for receiver controller thread.
 *
 *   - creates the queue
 *   - receives message that sender controller is ready
 *   - sends message that receiver is ready
 *   - receives message that publisher has finished
 */

void Remote_Receiver_Controller(void* param)
{
   const SAL_Message_T* msg;

   Tr_Info_Lo("Remote_Receiver_Controller: Start");

   if (!SAL_Create_Queue(2, 0, malloc, free))
   {
      PBC_Failed("Queue creation error");
   }

   msg = SAL_Receive();

   if (msg->event_id != (SAL_Event_Id_T)REMOTE_CONTROLLER_READY)
   {
      PBC_Failed("Received invalid message");
   }

   SAL_Wait_Semaphore(&Sem);

   SAL_Send(APP_SENDER, SENDER_CONTROL, REMOTE_RECEIVER_READY, NULL, 0);

   msg = SAL_Receive();

   if (msg->event_id != (SAL_Event_Id_T)REMOTE_SENDER_READY)
   {
      PBC_Failed_1("Received invalid message: %d", (int)msg->event_id);
   }

   SAL_Signal_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Receiver_Controller: Stop");

}

/** Function of sender application for sender controller thread.
 *
 *   - creates the queue
 *   - sends message that sender controller is ready
 *   - receives message that receiver is ready
 *   - sends message that sender has sent messages and has finished
 */

void Remote_Sender_Controller(void* param)
{
   const SAL_Message_T* msg;

   Tr_Info_Lo("Remote_Sender_Controller: Start");

   if (!SAL_Create_Queue(1, 0, malloc, free))
   {
      PBC_Failed("Queue creation error");
   }

   SAL_Send(APP_RECEIVER, RECEIVER_CONTROL, REMOTE_CONTROLLER_READY, NULL, 0);

   msg = SAL_Receive();

   if (msg->event_id != (SAL_Event_Id_T)REMOTE_RECEIVER_READY)
   {
      PBC_Failed_1("Received invalid message: %d", (int)msg->event_id);
   }

   SAL_Signal_Semaphore(&Sem);

   SAL_Wait_Semaphore(&Destroyed_Sem);

   SAL_Send(APP_RECEIVER, RECEIVER_CONTROL, REMOTE_SENDER_READY, NULL, 0);

   Tr_Info_Lo("Remote_Sender_Controller: Stop");
}

/** \}
 */


/* ---------------------------------------------------------- */
/*                          TEST 1                            */
/* ---------------------------------------------------------- */


/** \name Functions of remote No_Queue_Test
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Remote_Receiver_No_Queue_Fun(void* param)
{
   Tr_Info_Lo("Remote_Receiver_No_Queue_Fun: Start");

   SAL_Signal_Semaphore(&Sem);

   SAL_Wait_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Receiver_No_Queue_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the message
 *   - signals that has finished
 */

void Remote_Sender_No_Queue_Fun(void* param)
{

   Tr_Info_Lo("Remote_Sender_No_Queue_Fun: Start");

   SAL_Wait_Semaphore(&Sem);

   SAL_Send(APP_RECEIVER, RECEIVER_THREAD, EV_E1, NULL, 0);

   SAL_Signal_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Sender_No_Queue_Fun: Stop");
}


/**  Test of SAL_No_Queue callback for receiver application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Rec_No_Queue_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret = 0;
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, RECEIVER_CONTROL };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Receiver_No_Queue_Fun,   
      NULL, 
      Init_Thread_Attr("RRec1", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Receiver_Controller,   
      NULL, 
      Init_Thread_Attr("RRecCtr1", RECEIVER_CONTROL, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}


/**  Test of SAL_No_Queue callback for sender application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Send_No_Queue_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, SENDER_CONTROL };
   SAL_Int_T ret = 0;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Sender_No_Queue_Fun,   
      NULL, 
      Init_Thread_Attr("RSnd1", SENDER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Sender_Controller,   
      NULL, 
      Init_Thread_Attr("RSndCtrl", SENDER_CONTROL, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

/* ------------------------------------------------------------------------------- */
/*                               REMOTE TEST 2                                     */
/* ------------------------------------------------------------------------------- */

/** \name Functions of remote Queue_Full_Test
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates the queue of length 1
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Remote_Receiver_Queue_Full_Fun(void* param)
{
   Tr_Info_Lo("Remote_Receiver_Queue_Full_Fun: Start");

   SAL_Sleep(1000);

   if (!SAL_Create_Queue(1, sizeof(SAL_Int_T), malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Signal_Semaphore(&Sem);

   SAL_Wait_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Receiver_Queue_Full_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the messages
 *   - signals that has finished
 */

void Remote_Sender_Queue_Full_Fun(void* param)
{

   Tr_Info_Lo("Remote_Sender_Queue_Full_Fun: Start");

   SAL_Sleep(1000);

   SAL_Wait_Semaphore(&Sem);

   SAL_Send(APP_RECEIVER, RECEIVER_THREAD, EV_E1, NULL, 0);
   SAL_Send(APP_RECEIVER, RECEIVER_THREAD, EV_E2, NULL, 0);
   SAL_Send(APP_RECEIVER, RECEIVER_THREAD, EV_E3, NULL, 0);

   SAL_Signal_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Sender_Queue_Full_Fun: Stop");
}


/**  Test of SAL_Queue_Full callback for receiver application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Rec_Queue_Full_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret;
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, RECEIVER_CONTROL };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Receiver_Queue_Full_Fun,   
      NULL, 
      Init_Thread_Attr("RRec2", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Receiver_Controller,   
      NULL, 
      Init_Thread_Attr("RRecCtr2", RECEIVER_CONTROL, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/**  Test of SAL_Queue_Full callback for sender application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Send_Queue_Full_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, SENDER_CONTROL };
   SAL_Int_T ret;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Sender_Queue_Full_Fun,   
      NULL, 
      Init_Thread_Attr("RSnd2", SENDER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Sender_Controller,   
      NULL, 
      Init_Thread_Attr("RSndCtr2", SENDER_CONTROL, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

/* --------------------------------------------------------------- */
/*                         REMOTE TEST 3                           */
/* --------------------------------------------------------------- */

/** \name Functions of remote No_Memory_Test
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue of buffer_size = 0 and with allocation 
 *     function = Null_Fun
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Remote_Receiver_No_Memory_Fun(void* param)
{
   Tr_Info_Lo("Remote_Receiver_No_Memory_Fun: Start");

   if (!SAL_Create_Queue(1, 0, Null_Fun, free))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Signal_Semaphore(&Sem);

   SAL_Wait_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Receiver_No_Memory_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the message
 *   - signals that has finished
 */

void Remote_Sender_No_Memory_Fun(void* param)
{
   SAL_Int_T data;

   Tr_Info_Lo("Remote_Sender_No_Memory_Fun: Start");

   SAL_Wait_Semaphore(&Sem);

   data = 5;

   SAL_Send(APP_RECEIVER, RECEIVER_THREAD, EV_E1, &data, sizeof(SAL_Int_T));

   SAL_Signal_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Sender_No_Memory_Fun: Stop");
}


/**  Test of SAL_No_Memory callback for receiver application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Rec_No_Memory_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret;
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, RECEIVER_CONTROL };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Receiver_No_Memory_Fun,   
      NULL, 
      Init_Thread_Attr("RRec3", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Receiver_Controller,   
      NULL, 
      Init_Thread_Attr("RRecCtr3", RECEIVER_CONTROL, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}


/**  Test of SAL_No_Memory callback for sender application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Send_No_Memory_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, SENDER_CONTROL };
   SAL_Int_T ret;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Sender_No_Memory_Fun,   
      NULL, 
      Init_Thread_Attr("RSnd3", SENDER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Sender_Controller,   
      NULL, 
      Init_Thread_Attr("RSndCtr3", SENDER_CONTROL, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

/* --------------------------------------------------------------------- */
/*                         REMOTE TEST 4                                 */
/* --------------------------------------------------------------------- */

/** \name Functions of remote Too_Small_Buffer_Test
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue of buffer_size = 0 and with no allocation 
 *     function
 *   - signals that is ready
 *   - waits until SENDER_THREAD terminates
 */

void Remote_Receiver_Too_Small_Buffer_Fun(void* param)
{
   Tr_Info_Lo("Remote_Receiver_Too_Small_Buffer_Fun: Start");

   if (!SAL_Create_Queue(1, 0, NULL, NULL))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Signal_Semaphore(&Sem);

   SAL_Wait_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Receiver_Too_Small_Buffer_Fun: Stop");

}


/**  Function of SENDER_THREAD.
 *
 *   - waits until RECEIVER_THREAD is ready
 *   - sends the message
 *   - signals that has finished
 */

void Remote_Sender_Too_Small_Buffer_Fun(void* param)
{
   SAL_Int_T* data;

   Tr_Info_Lo("Remote_Sender_Too_Small_Buffer_Fun: Start");

   SAL_Wait_Semaphore(&Sem);

   data = (SAL_Int_T*)malloc(sizeof(SAL_Int_T));

   if (data != NULL)
   {
      *data = 5;
      SAL_Send(APP_RECEIVER, RECEIVER_THREAD, EV_E1, &data, sizeof(SAL_Int_T));
      free(data);
  }

   SAL_Signal_Semaphore(&Destroyed_Sem);

   Tr_Info_Lo("Remote_Sender_Too_Small_Buffer_Fun: Stop");
}


/**  Test of SAL_Too_Small_Buffer callback for receiver application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Rec_Too_Small_Buffer_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret;
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD, RECEIVER_CONTROL };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Receiver_Too_Small_Buffer_Fun,   
      NULL, 
      Init_Thread_Attr("RRec4", RECEIVER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Receiver_Controller,   
      NULL, 
      Init_Thread_Attr("RRecCtr4", RECEIVER_CONTROL, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}


/**  Test of SAL_Too_Small_Buffer callback for sender application.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Remote_Send_Too_Small_Buffer_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, SENDER_CONTROL };
   SAL_Int_T ret;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Remote_Sender_Too_Small_Buffer_Fun,   
      NULL, 
      Init_Thread_Attr("RSnd4", SENDER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Remote_Sender_Controller,   
      NULL, 
      Init_Thread_Attr("RSndCtr4", SENDER_CONTROL, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

#endif

/** \}
 */

/** \name Other Functions
 *  \{
 */

/**  Test of callback in local or remote communication.
 *   
 *   Four test are started:
 *   - for SAL_No_Queue
 *   - for SAL_Queue_Full
 *   - for SAL_No_Memory
 *   - for SAL_Too_Small_Buffer
 */

SAL_Int_T Callback_Test_Suite()
{
   SAL_Int_T ret = 0;

   switch(Test_Type)
   {
#if defined (XSAL)
   case E_SENDER:
   
      ret = ret || Remote_Send_No_Queue_Test();
      ret = ret || Remote_Send_Queue_Full_Test();
      ret = ret || Remote_Send_No_Memory_Test();
      ret = ret || Remote_Send_Too_Small_Buffer_Test();

      break;

   case E_RECEIVER:

      ret = ret || Remote_Rec_No_Queue_Test();
      ret = ret || Remote_Rec_Queue_Full_Test();
      ret = ret || Remote_Rec_No_Memory_Test();
      ret = ret || Remote_Rec_Too_Small_Buffer_Test();

      break;

#endif
   case E_STAND_ALONE:

      ret = ret || Local_No_Queue_Test();
      ret = ret || Local_Queue_Full_Test();
      ret = ret || Local_No_Memory_Test();
      ret = ret || Local_Too_Small_Buffer_Test();

      break;

   default:

      Tr_Fault("Invalid Test_Type");
      return 1;
   }

   return ret;
}

/** \}
 */


SAL_Int_T Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_Int_T option;
   SAL_Config_T config;
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

      config.app_id = APP_SENDER;
      config.max_number_of_threads = 3;

      break;

   case E_RECEIVER:

      config.app_id = APP_RECEIVER;
      config.max_number_of_threads = 4;

      break;
#endif

   default:

      config.app_id = APP_LOCAL;
      config.max_number_of_threads = 2;
   }

   if (!SAL_Init(&config))
   {
      Tr_Fault("SAL_Init error");
      return 1;
   }

   SAL_Init_Semaphore_Attr(&sem_attr);

   if (!SAL_Create_Semaphore(&Sem, &sem_attr) || 
       !SAL_Create_Semaphore(&Destroyed_Sem, &sem_attr))
   {
      return 1;
   }

   return 0;
}


SAL_Int_T Tear_Down()
{
   if (!SAL_Destroy_Semaphore(&Sem) || 
       !SAL_Destroy_Semaphore(&Destroyed_Sem))
   {
      return 1;
   }   

   return 0;
}


int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up(argc, argv))
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   Callback_Test_Suite();

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
