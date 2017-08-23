/* -------------------------------------------------------------------------
 *
 * File:        test_unknown_thread.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of unknown thread management.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 10);

#define APP_ID 2

#define NUM_UNKNOWN_THREADS 5
#define FIRST_UNKNOWN_THREAD_ID (KNOWN_THREAD+4)
#define MAX_NUMB_OF_THREADS (FIRST_UNKNOWN_THREAD_ID+NUM_UNKNOWN_THREADS-1)

#define EV_E1 1

typedef enum Thread_Id_Tag
{
   MAIN_THREAD = PM_LAST_THREAD_ID, KNOWN_THREAD
} Thread_Id_T;

char const * const Thread_Name[6] =
{ "Un6", "Un7", "Un8", "Un9", "Un10", "Un11" };

SAL_Thread_Id_T Thread_Id_1, Known_Thread_Id, Unknown_Thread_Id;
SAL_Thread_Id_T Thread_Id[6];
SAL_Int_T Next_Start = 0;

SAL_Mutex_T Mutex;

SAL_Int_T Timeout = 3000;

/**  \file
 *
 *   Test of unknown threads (<B><I>XSAL only</I></B>).
 * 
 *   \section UTT Unknown_Thread_Test
 *
 *   At first only MAIN_THREAD is created. Then MAIN_THREAD creates unknown threads.
 *
 *   \subsection TP Test procedure:
 *
 *   <OL>
 *   <LI> MAIN_THREAD creates the queue. </LI>
 *   <LI> MAIN_THREAD creates five unknown threads. </LI>
 *   <LI> MAIN_THREAD try to create one thread more when all unknown thread ids are in use. </LI>
 *   <LI> Five unknown threads check if their thread id is correct. </LI>
 *   <LI> Five unknown threads create queues. </LI>
 *   <LI> Five unknown threads sends the messages with their thread id to MAIN_THREAD. </LI>
 *   <LI> MAIN_THREAD receives the messages from five unknown threads. </LI>
 *   <LI> MAIN_THREAD checks if unknown thread ids received in the messages are valid. </LI>
 *   <LI> MAIN_THREAD sends messages with its thread id to unknown threads . </LI>
 *   <LI> Five unknown threads receive messages from MAIN_THREAD. </LI>
 *   <LI> Five unknown threads check if thread id are valid. </LI>
 *   <LI> All unknown thread terminates apart from threads with thread id
 *        FIRST_UNKNOWN_THREAD_ID+2 and FIRST_UNKNOWN_THREAD_ID+3. </LI>
 *   <LI> MAIN_THREAD creates one known thread. </LI>
 *   <LI> MAIN_THREAD creates next unknown thread. </LI>
 *   <LI> Created unknown thread checks its thread id, sends message to MAIN_THREAD. </LI>
 *   <LI> MAIN_THREAD receives message from new created unknown thread, checks its 
 *       thread id and sends message to this thread. </LI>
 *   <LI> Unknown thread receives message and checks thread id. </LI>
 *   </OL>
 */

const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, SAL_Thread_Id_T id, SAL_Priority_T prior,
   SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}

/**  Function of known thread.
 */

void Known_Thread_Fun(void * param)
{
   Tr_Info_Lo("Known_Thread_Fun: Start");

   Tr_Info_Lo("Known_Thread_Fun: Stop");
}

/**  Function of unknown thread.
 *
 *   - checks if Thread_Id is correct
 *   - sends the message with its thread id to the main thread 
 *   - receives the message from the main thread with the thread id of the receiving (unknown)
 *     thread
 *   - checks if the received thread id is correct
 */

void Unknown_Thread_Fun(void* param)
{
   const SAL_Message_T* msg;
   SAL_Thread_Id_T data;
   SAL_Thread_Id_T j = (SAL_Thread_Id_T) (intptr_t) param;
   static SAL_Thread_Id_T destroyed_list[] =
   { MAIN_THREAD };
   SAL_Int_T i, k;
   SAL_Thread_Id_T cur_tid;

   Tr_Info_Lo("Unknown_Thread_Fun: Start");

   SAL_Signal_Ready();

   if (!SAL_Lock_Mutex_Timeout(&Mutex, Timeout))
   {
      PBC_Failed("Mutex lock time out");
   }
   /* unknown thread is started for the second time */
   if (Next_Start)
   {
      i = Unknown_Thread_Id - FIRST_UNKNOWN_THREAD_ID;
      Thread_Id[i] = Unknown_Thread_Id;
   }
   /* for the first time */
   else
      i = j - FIRST_UNKNOWN_THREAD_ID;

   cur_tid = SAL_Get_Thread_Id();
   /* check if thread id is correct */
   if (Thread_Id[i] != cur_tid)
   {
      Tr_Fault_2("Invalid thread id (%d, not %d)", (int)cur_tid, (int)Thread_Id[i]);
      Tr_Info_Hi_2("Thread_Id[i]: %d, i: %d", (int)Thread_Id[i], (int)i);
      for (k = 0; k < NUM_UNKNOWN_THREADS; k++)
      {
         Tr_Info_Hi_2("Thread_Id[%d]: %d", (int)k, (int)Thread_Id[k]);
      }
      SAL_Exit(1);
   }
   SAL_Unlock_Mutex(&Mutex);

   Tr_Info_Hi("Thread number");

   /* send the message to the main thread */
   SAL_Create_Queue(1, 0, malloc, free);

   j = SAL_Get_Thread_Id();

   SAL_Send(APP_ID, MAIN_THREAD, EV_E1, &j, sizeof(SAL_Thread_Id_T));

   /* wait for the message from the main thread */
#if defined (XSAL)
   msg = SAL_Receive();
#else
   msg = SAL_Receive_Timeout(Timeout);
#endif

   data = *(SAL_Thread_Id_T*) msg->data;

   if (data != SAL_Get_Thread_Id())
   {
      PBC_Failed("Invalid thread id");
   }

   /* all threads apart from thread_id = FIRST_UNKNOWN_THREAD_ID+2 and
    * FIRST_UNKNOWN_THREAD_ID+3 are waiting untill the main thread finishes */
   if ((j != FIRST_UNKNOWN_THREAD_ID + 2) && (j != FIRST_UNKNOWN_THREAD_ID + 3))
   {
#if defined (XSAL)
      SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));
#else
      if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
            sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
            Timeout))
      {
         PBC_Failed("Timeout has been reached");
      }
#endif
   }

   Thread_Id[i] = 0;

   Tr_Info_Lo("Unknown_Thread_Fun: Stop");
}

/** Function of MAIN_THREAD.
 *
 *  - creates NUM_UNKNOWN_THREADS unknown threads
 *  - when all unknown thread ids are in use, tries to create one more (which shouldn't been 
 *    created)
 *  - then waits for the message from each of the NUM_UNKNOWN_THREADS unknown treads including their thread id
 *  - sends to each of NUM_UNKNOWN_THREADS threads the message with unknown thread id
 *  - after some of the threads finish, create the next unknown thread
 */
void Main_Thread_Fun(void* param)
{
   const SAL_Message_T* msg;
   SAL_Int_T i, k;
   static SAL_Thread_Id_T destroyed_list[] =
   { FIRST_UNKNOWN_THREAD_ID + 2, FIRST_UNKNOWN_THREAD_ID + 3 };
   SAL_Thread_Id_T data;
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;

   Tr_Info_Lo("Main_Thread_Fun: Start");

   SAL_Create_Queue(NUM_UNKNOWN_THREADS, 0, malloc, free);

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   /* create NUM_UNKNOWN_THREADS unknown threads */
   for (i = 0; i < NUM_UNKNOWN_THREADS; i++)
   {
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Timeout))
      {
         PBC_Failed("Mutex lock time out");
      }
      if ((Thread_Id[i] = SAL_Create_Thread(Unknown_Thread_Fun, (void*) (intptr_t) (FIRST_UNKNOWN_THREAD_ID + i),
         Init_Thread_Attr(Thread_Name[i], SAL_UNKNOWN_THREAD_ID, priority, &thread_attr)))
         == SAL_UNKNOWN_THREAD_ID)
      {
         Tr_Fault("Unknown thread has not been created");
         for (k = 0; k < NUM_UNKNOWN_THREADS; k++)
         {
            Tr_Info_Hi_2("Thread_Id[%d]: %d", (int)k, (int)Thread_Id[k]);
         }
         SAL_Exit(1);
      }

      if (Thread_Id[i] != FIRST_UNKNOWN_THREAD_ID + i)
      {
         Tr_Fault_2("Unknown thread id is not the first unused one: i = %d, Thread_Id[i] = %d", (int)i,
            (int)Thread_Id[i]);
         for (k = 0; k < NUM_UNKNOWN_THREADS; k++)
         {
            Tr_Info_Hi_2("Thread_Id[%d]: %d", (int)k, (int)Thread_Id[k]);
         }
         SAL_Exit(1);
      }

      if (!SAL_Wait_Ready_Timeout(&Thread_Id[i], 1, Timeout))
      {
         Tr_Fault_1("Timed out waiting for unknown thread %d to start.", (int)Thread_Id[i]);
         SAL_Exit(1);
      }

      SAL_Unlock_Mutex(&Mutex);

   }

   /* try to create one thread above the max admitted number */
   if ((Thread_Id[i] = SAL_Create_Thread(Unknown_Thread_Fun, (void*) (intptr_t) (FIRST_UNKNOWN_THREAD_ID + i),
      Init_Thread_Attr(Thread_Name[i], SAL_UNKNOWN_THREAD_ID, priority, &thread_attr)))
      != SAL_UNKNOWN_THREAD_ID)
   {
      PBC_Failed("SAL_Create_Thread: Too many threads have been created (unknown thread)");
   }

   /* wait for the messages from unknown threads */
   for (i = 0; i < NUM_UNKNOWN_THREADS; i++)
   {
#if defined (XSAL)
      msg = SAL_Receive();
#else
      msg = SAL_Receive_Timeout(Timeout);
#endif

      data = *(SAL_Thread_Id_T*) msg->data;

      if (data != msg->sender_thread_id)
      {
         PBC_Failed("Invalid thread id");
      }
   }

   /* send messages to the unknown threads */
   for (i = 0; i < NUM_UNKNOWN_THREADS; i++)
   {
      SAL_Thread_Id_T tid = FIRST_UNKNOWN_THREAD_ID + i;
      SAL_Send(APP_ID, tid, EV_E1, &tid, sizeof(SAL_Thread_Id_T));
   }

#if defined (XSAL)
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));
#else
   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }
#endif

   Next_Start = 1;

   if (!SAL_Lock_Mutex_Timeout(&Mutex, Timeout))
   {
      PBC_Failed("Mutex lock time out");
   }
   /* create known thread */
   if ((Known_Thread_Id = SAL_Create_Thread(Known_Thread_Fun, (void*) KNOWN_THREAD,
      Init_Thread_Attr("Known", KNOWN_THREAD, priority, &thread_attr))) == SAL_UNKNOWN_THREAD_ID)
   {
      PBC_Failed("Known thread has not been created");
   }

   if (Known_Thread_Id != KNOWN_THREAD)
   {
      PBC_Failed("Known thread has invalid thread id");
   }
   SAL_Unlock_Mutex(&Mutex);

   /* create next unknown thread */

   if (!SAL_Lock_Mutex_Timeout(&Mutex, Timeout))
   {
      PBC_Failed("Mutex lock time out");
   }
   if ((Unknown_Thread_Id = SAL_Create_Thread(Unknown_Thread_Fun, NULL,
      Init_Thread_Attr("Unknown", SAL_UNKNOWN_THREAD_ID, priority, &thread_attr))) == SAL_UNKNOWN_THREAD_ID)
   {
      PBC_Failed("Unknown thread has not been created");
   }

   if (!SAL_Wait_Ready_Timeout(&Unknown_Thread_Id, 1, Timeout))
   {
      Tr_Fault_1("Timed out waiting for unknown thread %d to start.", (int)Thread_Id[i]);
      SAL_Exit(1);
   }
   SAL_Unlock_Mutex(&Mutex);

#if defined (XSAL)
   msg = SAL_Receive();
#else
   msg = SAL_Receive_Timeout(Timeout);
#endif

   data = *(SAL_Thread_Id_T*) msg->data;

   if (data != msg->sender_thread_id)
   {
      PBC_Failed("Invalid thread id");
   }

   SAL_Send(APP_ID, Unknown_Thread_Id, EV_E1, &Unknown_Thread_Id, sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Main_Thread_Fun: Stop");
}

/**  Test of unknown threads.
 *
 *   Creates MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Unknown_Thread_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T result;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Thread_Id_1 = SAL_Create_Thread(Main_Thread_Fun, NULL,
      Init_Thread_Attr("Main", MAIN_THREAD, priority, &thread_attr));

   result = SAL_Run();

   return result;
}

SAL_Int_T Set_Up()
{
   SAL_Config_T config;
   SAL_Mutex_Attr_T mutex_attr;

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;
   config.first_unknown_thread_id = FIRST_UNKNOWN_THREAD_ID;

   if (!SAL_Init(&config))
   {
      return 1;
   }

   SAL_Init_Mutex_Attr(&mutex_attr);
   SAL_Create_Mutex(&Mutex, &mutex_attr);

   return 0;
}

void Tear_Down()
{
   SAL_Destroy_Mutex(&Mutex);
}

int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up())
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   ret = ret || Unknown_Thread_Test();

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
