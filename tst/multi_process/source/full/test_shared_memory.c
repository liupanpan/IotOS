/* -------------------------------------------------------------------------
 *
 * File:        test_shared_memory.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of shared memory.
 * 
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include "pm_events.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xsal.h"
#include "pi_unistd.h"

EM_FILENUM(TEST_MODULE2_ID, 3);

#define MAX_NUMB_OF_THREADS 1

#define THREAD 1

#define EV_READY 1
#define EV_INFO 2

typedef enum Process_Id_Tag 
{
   APP_CREATE_MEMORY  = 2,
   APP_READ_MEMORY_1, /* 3 */
   APP_READ_MEMORY_2, /* 4 */
   APP_READ_MEMORY_3  /* 5 */
} Process_Id_T;

typedef enum E_Test_Type_Tag 
{ 
   Create_Mem_Process, 
   Read_Mem_Process 
} E_Test_Type_T;

E_Test_Type_T Test_Type = Read_Mem_Process;

typedef struct Data_Tag
{
   SAL_Int_T field1;
   SAL_Int_T field2;
   SAL_Int_T field3;
} Data_T;

SAL_Int_T Key = 1;
SAL_Int_T Subkey = 1;


/**
 *   \file
 *
 *   Test of shared memory (<B><I>XSAL only</I></B>).
 *
 *   \section SMT SharedMemoryTest
 *
 *   \subsection HTS How To Start the Test
 *
 *   - four applications should be started with parameters -p2, -p3, -p4 and -p5
 *   - first should be started applications with parameters -p3, -p4, -p5. They read from
 *     shared memory
 *   - the application with parameter -p2 must be started as the last one. It writes
 *     to the shared memory.
 *   Number of parameter means the application number.
 *
 *   \subsection TP Test procedure:
 *
 *   <OL>
 *   <LI> THREADs from APP_READ_MEMORY_1, APP_READ_MEMORY_2, APP_READ_MEMORY_3
 *        create the queues. </LI>
 *   <LI> THREADs from APP_READ_MEMORY_1, APP_READ_MEMORY_2, APP_READ_MEMORY_3
 *        subscribe on event EV_READY. </LI>
 *   <LI> THREAD from APP_CREATE_MEMORY creates the queue. </LI>
 *   <LI> THREAD from APP_CREATE_MEMORY creates shared memory. </LI>
 *   <LI> THREAD from APP_CREATE_MEMORY writes data to thread memory. </LI>
 *   <LI> THREAD from APP_CREATE_MEMORY publishes EV_READY event with data which
 *        were written to the shared memory. </LI>
 *   <LI> THREADs from APP_READ_MEMORY_1, APP_READ_MEMORY_2, APP_READ_MEMORY_3 
 *        open shared memory. </LI>
 *   <LI> THREADs from APP_READ_MEMORY_1, APP_READ_MEMORY_2, APP_READ_MEMORY_3 
 *        check if received data are the same as in shared memory (read shared
 *        memory). </LI>
 *   <LI> If for some thread data are invalid, the thread sends EV_INFO message 
 *        to the THREAD of APP_CREATE_MEMORY and SAL_Exit is called. </LI>
 *   <LI> THREADs from APP_READ_MEMORY_1, APP_READ_MEMORY_2, APP_READ_MEMORY_3 
 *        close shared memory. </LI>
 *   <LI> THREADs from APP_READ_MEMORY_1, APP_READ_MEMORY_2, APP_READ_MEMORY_3
 *        send EV_READY message to THREAD of APP_CREATE_MEMORY. </LI>
 *   <LI> THREAD from APP_CREATE_MEMORY receives messages from all three threads. </LI>
 *   <LI> If one of received messages event is EV_INFO, SAL_Exit is called. </LI>
 *   <LI> In other case, THREAD from APP_CREATE_MEMORY close memory. </LI>
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


/**  Thread function of APP_READ_MEMORY_1, APP_READ_MEMORY_2, APP_READ_MEMORY_3 
 *   processes.
 *
 *   - reads shared memory which was entried by another process
 */

void Read_Memory_Fun(void* param)
{
   SAL_Event_Id_T subscribe_list[] = { EV_READY };
   const SAL_Message_T* msg;
   SAL_Shared_Memory_Handle_T handle;
   Data_T *dat1, *dat2;

   Tr_Info_Lo("Read_Memory_Fun: Start");

   SAL_Create_Queue(1, sizeof(Data_T), malloc, free);

   SAL_Subscribe(subscribe_list, sizeof(subscribe_list)/sizeof(SAL_Event_Id_T));

   msg = SAL_Receive();

   if (msg->event_id != EV_READY)
   {
      Tr_Fault_1("AppId: %d, Event id is different from EV_READY", (int)SAL_Get_App_Id());

      SAL_Send(APP_CREATE_MEMORY, THREAD, EV_INFO, NULL, 0);
      
      SAL_Exit(1);
   }

   dat1 = (Data_T*)msg->data;

   if (!SAL_Open_Shared_Memory(Key, Subkey, SAL_SHMEM_RDONLY, &handle))
   {
      Tr_Fault_1("AppId: %d, SAL_Open_Shared_Memory: Shared memory has not been opened", (int)SAL_Get_App_Id());

      SAL_Send(APP_CREATE_MEMORY, THREAD, EV_INFO, NULL, 0);

      SAL_Exit(1);
   }

   dat2 = (Data_T*)handle.data;

   if ((dat1->field1 != dat2->field1) || (dat1->field2 != dat2->field2) || (dat1->field3 != dat2->field3))
   {
      Tr_Fault_1("AppId: %d, Invalid data in shared memory", (int)SAL_Get_App_Id());

      SAL_Send(APP_CREATE_MEMORY, THREAD, EV_INFO, NULL, 0);

      SAL_Exit(1);
   }

   SAL_Close_Shared_Memory(&handle);

   SAL_Send(APP_CREATE_MEMORY, THREAD, EV_READY, NULL, 0);

   Tr_Info_Lo("Read_Memory_Fun: End");   
}


/**  Thread function of APP_CREATE_MEMORY process.
 *
 *   - creates and writes to shared memory
 */

void Create_Memory_Fun(void* param)
{
   const SAL_Message_T* msg;
   SAL_Shared_Memory_Handle_T handle;
   Data_T dat;
   SAL_Int_T i = 0;
   SAL_Int_T error = 0;

   Tr_Info_Lo("Create_Memory_Fun: Start");

   SAL_Create_Queue(4, 0, malloc, free);

   if (!SAL_Create_Shared_Memory(Key, Subkey, sizeof(Data_T), SAL_SHMEM_RDWR, &handle))
   {
      PBC_Failed("SAL_Create_Shared_Memory: Shared memory has not been created"); 
   }

   dat.field1 = 30;
   dat.field2 = 60;
   dat.field3 = 90;

   memcpy(handle.data, &dat, sizeof(Data_T));   

   SAL_Publish(EV_READY, &dat, sizeof(Data_T));

   while (i<3)
   {
      msg = SAL_Receive();      

      if (msg->event_id == EV_INFO)
      {
         Tr_Fault_1("AppId: 2, Received info from app %d that error occured", (int)msg->sender_app_id);
         error = 1;
      }
      else if (msg->event_id != EV_READY)
      {
         PBC_Failed_1("AppId: %d, Event Id is different from EV_READY", (int)SAL_Get_App_Id());
      }
      else
      {
      }

      i++;

   }

   if (error)
   {
      SAL_Exit(1);
   }

   SAL_Close_Shared_Memory(&handle);

   Tr_Info_Lo("Create_Memory_Fun: End");

}


/**  Test of shared memory.
 *
 *   Creates thread for the chosen process (APP_CREATE_MEMORY or APP_READ_MEMORY_\a n)
 *   and starts SAL_Run.
 */

SAL_Int_T Shared_Memory_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   switch (Test_Type)
   {
   case (Create_Mem_Process):

      Tr_Info_Hi( "CreateMemProcess");

      SAL_Create_Thread(
         Create_Memory_Fun, 
         NULL, 
         Init_Thread_Attr("THREAD", THREAD, priority, &thread_attr));

      break;

   case (Read_Mem_Process):

      Tr_Info_Hi( "ReadMemProcess");

      SAL_Create_Thread(
         Read_Memory_Fun, 
         NULL, 
         Init_Thread_Attr("THREAD", THREAD, priority, &thread_attr));

      break;

   default:

      break;
  
   }

   return SAL_Run();
}


SAL_Int_T Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_App_Id_T app_number = 6;
   SAL_Int_T option;
   SAL_Int_T nr;
   SAL_Config_T config;

   while ( ( option = pi_getopt( argc, argv, "p:g:v:l:teN:" ) ) != -1 ) 
   {
      switch (option)
      {
         case ('p'):

            nr = atoi(pi_optarg);
            
            switch (nr)
            {
               case (2):
                  
                  Test_Type = Create_Mem_Process;
                  app_number = APP_CREATE_MEMORY;                    
                  break;

               case (3):

                  Test_Type = Read_Mem_Process;
                  app_number = APP_READ_MEMORY_1;
                  break;
      
               case (4): 
      
                  Test_Type = Read_Mem_Process;
                  app_number = APP_READ_MEMORY_2;                 
                  break;

               case (5):      
                  
                  Test_Type = Read_Mem_Process;
                  app_number = APP_READ_MEMORY_3;      
                  break;

               default:                  

                  Tr_Fault("Invalid parameters\n");

                  return 1;     
             }

            break;

         default:            
            break;

     }
   }

   SAL_Get_Config(&config);

   switch(Test_Type)
   {
   case Create_Mem_Process:
   
      config.app_id = APP_CREATE_MEMORY;
      config.max_number_of_threads = MAX_NUMB_OF_THREADS;

      if (!SAL_Init(&config))
      {
         return 1;
      }

      break;

   case Read_Mem_Process:

      config.app_id = app_number;
      config.max_number_of_threads = MAX_NUMB_OF_THREADS;

      if (!SAL_Init(&config))
      {
         return 1;
      }

      break;

   default:
      break;
   }

   return 0;
}


void Tear_Down()
{
}


int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up(argc, argv))
   {
      Tr_Fault("Set_Up failed\n");
      return 1;
   }

   ret = ret || Shared_Memory_Test();

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
