/* -------------------------------------------------------------------------
 *
 * File:        test_cond.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (Jacek.Roman@delphi.com)
 *
 * Description: Unit tests of conditional variable.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <malloc.h>
#include <sys/timeb.h>

#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 11);

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 4

#define LOOP_NUMB 10

typedef enum Thread_Id_Tag {
   WAIT_THREAD = 1,
   SIGNAL_THREAD,
   PRODUCER,
   CONSUMER
} Thread_Id_T;

/*
typedef enum Test_Nr_Tag {
   LONG_TIMEOUT = 1,
   SHORT_TIMEOUT
} Test_Nr_T;
*/
/*
typedef struct Param_Tag {
   Test_Nr_T test_nr;
   SAL_Thread_Id_T destroyed_list[2];
   size_t list_size;
} Param_T;
*/

SAL_Mutex_T Mutex;
SAL_Cond_T Cond;

SAL_Int_T Number = 0;
SAL_Int_T Product = 0;

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



void Wait_Condvar_Fun(void* param)
{
   Tr_Info_Lo("Wait_Condvar_Fun: Start");

   SAL_Lock_Mutex(&Mutex);

   while (Number != 5)
   {
      SAL_Signal_Ready();

      Tr_Info_Hi("WAIT_THREAD is waiting...");

      SAL_Wait_Cond(&Cond, &Mutex);
   }

   Tr_Info_Hi("Number = 5");

   SAL_Unlock_Mutex(&Mutex);

   Tr_Info_Lo("Wait_Condvar_Fun: Stop");
}

void Signal_Condvar_Fun(void* param)
{
   SAL_Thread_Id_T wait_list[] = { WAIT_THREAD };

   Tr_Info_Lo("Signal_Condvar_Fun: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_Lock_Mutex(&Mutex);

   Number = 5;

   SAL_Signal_Cond(&Cond);

   SAL_Unlock_Mutex(&Mutex);

   Tr_Info_Lo("Signal_Condvar_Fun: Stop");
}

SAL_Int_T Condvar_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] = { WAIT_THREAD, SIGNAL_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Wait_Condvar_Fun, 
      NULL, 
      Init_Thread_Attr("WAIT_THREAD", WAIT_THREAD, priority, &thread_attr));

/*   SAL_Sleep(200); */

   SAL_Create_Thread(
      Signal_Condvar_Fun, 
      NULL, 
      Init_Thread_Attr("SIGNAL_THREAD", SIGNAL_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;

}


void Producer_Fun(void* param)
{
   SAL_Int_T i;

   Tr_Info_Lo("Producer_Fun: Start");

   for (i = 0; i < LOOP_NUMB; i++)
   {
      SAL_Lock_Mutex(&Mutex);

      while (Number == 1)
      {
         SAL_Wait_Cond(&Cond, &Mutex);
      }

      Number = 1;

      Tr_Info_Hi("Producer produces product");

      SAL_Signal_Cond(&Cond);

      SAL_Unlock_Mutex(&Mutex);
   }

   Tr_Info_Lo("Producer_Fun: Stop");
}

void Consumer_Fun(void* param)
{
   SAL_Int_T i;

   Tr_Info_Lo("Consumer_Fun: Start");

   for (i = 0; i < LOOP_NUMB; i++)
   {
      SAL_Lock_Mutex(&Mutex);

      while (Number == 0)
      {
         SAL_Wait_Cond(&Cond, &Mutex);
      }

      Number = 0;

      Tr_Info_Hi("Consumer consumes product");

      SAL_Signal_Cond(&Cond);

      SAL_Unlock_Mutex(&Mutex);
   }

   Tr_Info_Lo("Consumer_Fun: Stop");
}


SAL_Int_T Condvar_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] = { PRODUCER, CONSUMER };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Consumer_Fun, 
      NULL, 
      Init_Thread_Attr("CONSUMER", CONSUMER, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Producer_Fun, 
      NULL, 
      Init_Thread_Attr("PRODUCER", PRODUCER, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;

}


/**  Test of condvar functions.
 *   
 */

SAL_Int_T Cond_Test_Suite()
{
   SAL_Int_T ret = 0;
   SAL_Mutex_Attr_T mutex_attr = SAL_DEFAULT_MUTEX_ATTR;
   SAL_Cond_Attr_T cond_attr = SAL_DEFAULT_COND_ATTR;

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("SAL_Create_Mutex: Mutex has not been created");
      return 1;
   }

   if (!SAL_Create_Cond(&Cond, &cond_attr))
   {
      Tr_Fault("SAL_Create_Cond: Condvar has not been created");
      return 1;
   }

   ret = ret || Condvar_Test_1();

   Number = 0;

   ret = ret || Condvar_Test_2();

   if (!SAL_Destroy_Cond(&Cond))
   {
      Tr_Fault("SAL_Destroy_Cond: Condvar has not been destroyed");
      return 1;
   }

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("SAL_Destroy_Mutex: Mutex has not been destroyed");
      return 1;
   }

   return ret;
}


bool_t Set_Up()
{
   SAL_Config_T config;

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   return SAL_Init(&config);
}


void Tear_Down()
{
}


int main(int argc, char** argv)
{
   SAL_Int_T ret = 1;

   if (Set_Up())
   {
      ret = Cond_Test_Suite();
      Tear_Down();
   }
   else
   {
      Tr_Fault("Set_Up failed");
   }
   
   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));

   return ret;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  01 Sep 2007, Jacek Roman (Jacek.Roman@delphi.com)
 *   Initial revision
 *
 * -----------------------------------------------------------------------*/
