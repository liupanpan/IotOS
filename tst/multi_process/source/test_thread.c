/* -------------------------------------------------------------------------
 *
 * File:        test_thread.c
 *
 * Copyright:   2006-2014 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of thread management.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xsal.h"
#include "xsal_thread_priority.h"
#include "pi_unistd.h"

extern const char *SAL_Get_Thread_Name_Self(void);

EM_FILENUM(TEST_MODULE_ID, 9);

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 14

#define EV_STOP 1

#define EV_E1 1

#define LESSER_TIME_WINDOW_MS    (0)
#define GREATER_TIME_WINDOW_MS   (25)

typedef enum Thread_Id_Tag
{
   CREATE_THREAD = 1,
   PARAM1_THREAD,
   PARAM2_THREAD,
   THREAD,
   DESTROY_THREAD,
   FUN_THREAD,
   VHIGH_P_THREAD,
   HIGH_P_THREAD,
   NORMAL_P_THREAD,
   LOW_P_THREAD,
   VLOW_P_THREAD,
   MAIN_THREAD,
   WAIT_THREAD,
   SIGNAL_THREAD
} Thread_Id_T;

typedef struct Data_Tag
{
   const char* thread_name;
   SAL_Thread_Id_T thread_id;
   SAL_Priority_T priority;
} Data_T;

Data_T Data_1, Data_2;

short Thread_Running_1 = 0;
short Thread_Running_2 = 0;
short Thread_Running_3 = 0;

SAL_Int_T Long_Timeout = 3000;

/**  \file
 *
 *   Test of all functions connected with threads.
 *
 *   \section T Tests:
 *   <OL>
 *   <LI> \ref TCD   - tests of SAL_Create_Thread, SAL_Wait_Destroyed function and 
 *                     SAL_Wait_Destroyed_Timeout functions, long timeout 
 *                     (<B><I>Create_Thread_Test for XSAL only </I></B>). </LI>
 *   <LI> \ref WDTT  - test of SAL_Wait_Destroyed_Timeout function, short timeout. </LI>
 *   <LI> \ref TFT   - test of other functions connected with threads. </LI>
 *   <LI> \ref PT    - test of priorities. </LI>
 *   <LI> \ref WRTT1 - test of SAL_Wait_Ready_Timeout and SAL_Signal_Ready functions 
 *                     (long timeout). </LI>
 *   <LI> \ref WRTT2 - test of SAL_Wait_Ready_Timeout and SAL_Signal_Ready functions 
 *                     (short timeout). </LI>
 *   <LI> \ref WRT   - test of SAL_Wait_Ready and SAL_Signal_Ready functions (<B><I> XSAL only 
 *                     </I></B>). </LI>
 *   </OL>
 *  
 *   \section CBS Can be started:
 *   - start without options - means that all test are started apart from priority test
 *   - start with option -p  - means that all test are started including priority test.
 *
 *   \section TCD Create_Thread_Test and Create_Thread_Light_Test
 *
 *   - Three threads are created: CREATE_THREAD, PARAM1_THREAD, PARAM2_THREAD.
 *   - The only difference is that in \b Create_Thread_Test is used SAL_Wait_Destroyed and in \b
 *     Create_Thread_Light_Test is used SAL_Wait_Destroyed_Timeout. Threads PARAM1_THREAD and 
 *     PARAM2_THREAD terminate earlier than timeout is reached (timeout is 3000 ms).
 *
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> CREATE_THREAD sets variable Thread_Running_1 on 1. </LI>
 *   <LI> CREATE_THREAD waits until two other threads start running. </LI>
 *   <LI> CREATE_THREAD waits until two other threads finishes. </LI>
 *   <LI> PARAM1_THREAD sets variable Thread_Running_2 on 1. </LI>
 *   <LI> PARAM1_THREAD checks if data passed as the last parameter are valid. </LI>
 *   <LI> PARAM1_THREAD waits until CREATE_THREAD starts running or waits 1 sec. </LI>
 *   <LI> PARAM1_THREAD checks if CREATE_THREAD function has started. </LI>
 *   <LI> PARAM2_THREAD sets variable Thread_Running_3 on 1. </LI>
 *   <LI> PARAM2_THREAD checks if the first three passed parameters are valid. </LI>
 *   <LI> CREATE_THREAD checks if two other threads are alive. </LI>
 *   </OL>
 *   
 *   \section WDTT Wait_Destroyed_Timeout_Test
 *
 *   - Two threads are created: THREAD and DESTROY_THREAD.
 *
 *   \subsection TP2 Test procedure:
 *
 *   <OL>
 *   <LI> DESTROY_THREAD waits until THREAD is running. </LI>
 *   <LI> THREAD sets the variable Thread_Running_2 on 1. </LI>
 *   <LI> THREAD sleeps for 1000 sec. </LI>
 *   <LI> DESTROY_THREAD waits until THREAD has terminated. Timeout is reached earlier than THREAD 
 *        terminates (timeout is 100 ms). </LI>
 *   <LI> DESTROY_THREAD checks if timeout value is proper. </LI>
 *   </OL>
 *
 *   \section TFT Thread_Fun_Test
 *
 *   - One thread FUN_THREAD is created.
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> FUN_THREAD creates thread THREAD. </LI>
 *   <LI> FUN_THREAD waits until THREAD function starts. </LI>
 *   <LI> THREAD creates the queue. </LI>
 *   <LI> THREAD signals that is ready. </LI>
 *   <LI> THREAD sets variable RunningThread on 1. </LI>
 *   <LI> THREAD waits for receiving the message. </LI>
 *   <LI> FUN_THREAD checks if THREAD is alive (SAL_Is_Thread_Alive function). </LI>
 *   <LI> FUN_THREAD checks the name of THREAD (SAL_Get_Thread_Name function). </LI>
 *   <LI> FUN_THREAD checks the name of itself (SAL_Get_Thread_Name_Self function). </LI>
 *   <LI> FUN_THREAD gets its thread id (SAL_Get_Thread_Id function). </LI>
 *   <LI> FUN_THREAD gets its priority (SAL_Get_Thread_Priority function). </LI>
 *   <LI> FUN_THREAD sets priority of THREAD (SAL_Set_Thread_Priority function). </LI>
 *   <LI> FUN_THREAD checks if priority of THREAD is valid. </LI>
 *   <LI> FUN_THREAD checks if the sleeping time of SAL_Sleep is valid. </LI>
 *   <LI> FUN_THREAD checks if delay time of SAL_Delay is valid. </LI>
 *   <LI> FUN_THREAD sends the message to THREAD. </LI>
 *   <LI> FUN_THREAD sleeps 500 msec. </LI>
 *   <LI> FUN_THREAD checks if THREAD is alive (SAL_Is_Thread_Alive function). </LI>
 *   </OL>
 *
 *   \section PT Priority_Test
 *
 *   - At the beginning one thread MAIN_THREAD is created. 
 *   - Then MAIN_THREAD creates five threads with different priorities: VHIGH_P_THREAD, 
 *     HIGH_P_THREAD, NORMAL_P_THREAD, LOW_P_THREAD and VLOW_P_THREAD.
 *
 *   \subsection TP5 Test procedure:
 *
 *   <OL>
 *   <LI> MAIN_THREAD creates five threads, each with different level priority: \n
 *        - V_HIGH_P_THREAD (with VERY_HIGH_PRIORITY)
 *        - HIGH_P_THREAD   (with HIGH_PRIORITY)
 *        - NORMAL_P_THREAD (with NORMAL_PRIORITY)
 *        - LOW_P_THREAD    (with LOW_PRIORITY)
 *        - V_LOW_P_THREAD  (with (VERY_LOW_PRIORITY)
 *   <LI> Functions of created threads check if their priority is correct. </LI>
 *   </OL>
 *  
 *   \section WRTT1 Wait_Ready_Timeout_Test_1
 *
 *   - At first only MAIN_THREAD is created. 
 *   - Then MAIN_THREAD creates two threads: WAIT_THREAD and SIGNAL_THREAD.
 *   
 *   WAIT_THREAD suspends with long timeout, SAL_Signal_Ready from SIGNAL_THREAD should 
 *   resume WAIT_THREAD before timeout is reached.
 *
 *   \subsection TP6 Test procedure:
 *
 *   <OL>
 *   <LI> MAIN_THREAD creates WAIT_THREAD. </LI>
 *   <LI> After 200 ms MAIN_THREAD creates SIGNAL_THREAD. </LI>
 *   <LI> WAIT_THREAD suspends itself with timeout 3000, finally SAL_Signal_Ready 
 *        from SIGNAL_THREAD should resume WAIT_THREAD. </LI>
 *   <LI> SIGNAL_THREAD tries to send message to WAIT_THREAD which has not created
 *        the queue yet. </LI>
 *   <LI> SIGNAL_THREAD signals that WAIT_THREAD can resume. </LI>
 *   <LI> WAIT_THREAD creates the queue. </LI>
 *   <LI> WAIT_THREAD set variable Thread_Running_1 on 1. </LI>
 *   <LI> SIGNAL_THREAD sends message to WAIT_THREAD. </LI>
 *   <LI> WAIT_THREAD try to pass SAL_Wait_Ready for the second time. </LI>
 *   <LI> WAIT_THREAD receives message. </LI>
 *   <LI> After 500 ms SIGNAL_THREAD checks the number of messages in the queue of WAIT_THREAD. </LI>
 *   </OL>
 *
 *   \section WRTT2 Wait_Ready_Timeout_Test_2
 *
 *   - At first only MAIN_THREAD is created. 
 *   - Then MAIN_THREAD creates two threads: WAIT_THREAD and SIGNAL_THREAD.
 *   
 *   WAIT_THREAD suspends with short timeout, timeout is reached before SAL_Signal_Ready 
 *   takes place.
 *
 *   \subsection TP7 Test procedure:
 *
 *   <OL>
 *   <LI> MAIN_THREAD creates WAIT_THREAD. </LI>
 *   <LI> WAIT_THREAD suspends itself with timeout 100, finally timeout is reached before 
 *        SAL_Signal_Ready takes place in SIGNAL_THREAD. </LI>
 *   <LI> MAIN_THREAD creates SIGNAL_THREAD 200 ms after creation WAIT_THREAD. </LI>
 *   <LI> SIGNAL_THREAD signals that is ready. </LI>
 *   <LI> WAIT_THREAD creates the queue. </LI>
 *   <LI> WAIT_THREAD set variable Thread_Running_1 on 1. </LI>
 *   <LI> SIGNAL_THREAD sends message to WAIT_THREAD. </LI>
 *   <LI> WAIT_THREAD try to pass SAL_Wait_Ready for the second time. </LI>
 *   <LI> WAIT_THREAD receives message. </LI>
 *   <LI> After 500 ms SIGNAL_THREAD checks the number of messages in the queue of WAIT_THREAD. </LI>
 *   </OL>
 *
 *   \section WRT Wait_Ready_Test
 *
 *   - At first only MAIN_THREAD is created. 
 *   - Then MAIN_THREAD creates two threads: WAIT_THREAD and SIGNAL_THREAD.
 *   
 *   \subsection TP8 Test procedure:
 *
 *   <OL>
 *   <LI> MAIN_THREAD creates WAIT_THREAD. </LI>
 *   <LI> After 200 ms MAIN_THREAD creates SIGNAL_THREAD. </LI>
 *   <LI> WAIT_THREAD suspends itself. </LI>
 *   <LI> SIGNAL_THREAD tries to send message to WAIT_THREAD which has not created
 *        the queue yet. </LI>
 *   <LI> SIGNAL_THREAD signals that is ready. </LI>
 *   <LI> WAIT_THREAD creates the queue. </LI>
 *   <LI> WAIT_THREAD set variable Thread_Running_1 on 1. </LI>
 *   <LI> SIGNAL_THREAD sends message to WAIT_THREAD. </LI>
 *   <LI> WAIT_THREAD try to pass SAL_Wait_Ready for the second time. </LI>
 *   <LI> WAIT_THREAD receives message. </LI>
 *   <LI> WAIT_THREAD sets variable Thread_Running_2 on 1. </LI>
 *   <LI> After 500 ms SIGNAL_THREAD checks the number of messages in the queue of WAIT_THREAD. </LI>
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



#if defined (XSAL)

/**  \name Functions of Create_Thread_Test
 *
 *   \{
 */

/**  Function of CREATE_THREAD.
 *
 *   - sets ThreadRunning variable to 1 (means that function has been started)
 *   - tests SAL_Wait_Destroyed function
 */

void Create_Thread_Fun(void* param)
{
   static SAL_Thread_Id_T destroyed_list[] = { PARAM1_THREAD, PARAM2_THREAD };

   Tr_Info_Lo("Create_Thread_Fun: Start");

   Thread_Running_1 = 1;

   while ((!Thread_Running_2) && (!Thread_Running_3))
   {
      SAL_Sleep(100);
   }

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   if (SAL_Is_Thread_Alive(PARAM1_THREAD) || (SAL_Is_Thread_Alive(PARAM2_THREAD)))
   {
      PBC_Failed("SAL_Wait_Destroyed: Function doesn't suspend thread untill other threads from parameter list terminate");
   }
 
   Tr_Info_Lo("Create_Thread_Fun: End");

}


/**  Function of PARAM1_THREAD.
 *
 *   - tests the last parameter of the function SAL_Create_Thread
 *   - checks if the thread function CreateThreadFun has started
 */

void Create_Param_Fun_1(void* param)
{
   Data_T data_3;
   SAL_Int_T i = 0; 

   Tr_Info_Lo("Create_Param_Fun_1: Start");

   Thread_Running_2 = 1;

   data_3 = *(Data_T*)param;

   /* check if data of the last parameters are valid */
   if ((data_3.thread_name != Data_1.thread_name) || (data_3.thread_id != Data_1.thread_id))
   {
      PBC_Failed("SAL_Create_Thread: Invalid last function parameter");
   }

   while ((!Thread_Running_1) && (i++ < 10))
   {
      SAL_Sleep(100);
   }
   
   /* check if function Create_Thread_Fun has started */
   if (!Thread_Running_1)
   {
      PBC_Failed("SAL_Create_Thread: Thread function has not been started");
   }

   Tr_Info_Lo("Create_Param_Fun_1: End");
}


/**  Function of PARAM2_THREAD.
 *
 *   - tests the first three parameters of the function SAL_Create_Thread
 */

void Create_Param_Fun_2(void* param)
{
   SAL_Thread_Id_T thread_id;
   SAL_Priority_T priority;
   SAL_Int_T i = 0;
   char const *tname = SAL_Get_Thread_Name_Self();

   Tr_Info_Lo("Create_Param_Fun_2: Start");

   Thread_Running_3 = 1;

   thread_id = SAL_Get_Thread_Id();
   SAL_Get_Thread_Priority(thread_id, &priority);

   /* check if the first three parameters are valid */
   if ( (NULL==tname) || (strcmp(Data_2.thread_name, tname)) ||
       (Data_2.thread_id != thread_id ) ||
       (Data_2.priority != priority) )
   {
      PBC_Failed_4("SAL_Create_Thread: Invalid parameters Create TID %d TID %d Create Priority %d Priority %d",
         Data_2.thread_id, thread_id, Data_2.priority, priority);
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   Tr_Info_Lo("Create_Param_Fun_2: End");
}


/**  Test of SAL_Create_Thread and SAL_Wait_Destroyed function.
 *
 *   Function creates three threads and starts SAL_Run.
 */

SAL_Int_T Create_Thread_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { CREATE_THREAD, PARAM1_THREAD, PARAM2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   if (SAL_Create_Thread(
          Create_Thread_Fun, 
          NULL,
          Init_Thread_Attr("CREATE_T", CREATE_THREAD, priority, &thread_attr)) 
        == SAL_UNKNOWN_THREAD_ID)
   {
      Tr_Fault("SAL_Create_Thread: Thread has not been created");
      return 1;
   }

   Data_1.thread_name = "Name";
   Data_1.thread_id = 5;

   SAL_Create_Thread(
      Create_Param_Fun_1, 
      (void*)&Data_1, 
      Init_Thread_Attr("PARAM1_T", PARAM1_THREAD, priority, &thread_attr));

   Data_2.thread_name = "PARAM2_T";
   Data_2.thread_id   = PARAM2_THREAD;
   Data_2.priority    = priority;
  
   SAL_Create_Thread(
      Create_Param_Fun_2, 
      NULL, 
      Init_Thread_Attr(Data_2.thread_name, Data_2.thread_id, Data_2.priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

#endif


/**  \name Functions of Create_Thread_Light_Test
 *
 *   \{
 */

/**  Function of CREATE_THREAD.
 *
 *   - sets ThreadRunning variable to 1 (means that function has been started)
 *   - tests SAL_Wait_Destroyed_Timeout function
 *   - other threads should terminate before timeout is reached (timeout is 3000 ms)
 */

void Create_Thread_Light_Fun(void* param)
{
   static SAL_Thread_Id_T destroyed_list[] = { PARAM1_THREAD, PARAM2_THREAD };
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Create_Thread_Light_Fun: Start");

   Thread_Running_1 = 1;

   while ((!Thread_Running_2) && (!Thread_Running_3))
   {
      SAL_Sleep(100);
   }

   time_1 = SAL_Clock();

   ret_timeout = SAL_Wait_Destroyed_Timeout(destroyed_list, 
                    sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
                    Long_Timeout);

   time_2 = SAL_Clock();

   if (SAL_Is_Thread_Alive(PARAM1_THREAD) || (SAL_Is_Thread_Alive(PARAM2_THREAD)))
   {
      PBC_Failed("SAL_Wait_Destroyed_Timeout: Function doesn't suspend thread untill other threads from parameter list terminate");
   }

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1000 + GREATER_TIME_WINDOW_MS)
      {
         PBC_Failed_1("SAL_Wait_Destroyed_Timeout: thread has been suspended for %d ms, expected time: from 0 to 1000 ms", (int)delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_Wait_Destroyed_Timeout: returns 0");
   }

   Tr_Info_Lo("Create_Thread_Light_Fun: End");

}


/**  Function of PARAM1_THREAD.
 *
 *   - tests the last parameter of the function SAL_Create_Thread
 *   - checks if the thread function Create_Thread_Fun has started
 */

void Create_Param_Light_Fun_1(void* param)
{
   Data_T data_3;
   SAL_Int_T i = 0; 

   Tr_Info_Lo("Create_Param_Light_Fun_1: Start");

   Thread_Running_2 = 1;

   data_3 = *(Data_T*)param;

   /* check if data of the last parameters are valid */
   if ((data_3.thread_name != Data_1.thread_name) || (data_3.thread_id != Data_1.thread_id))
   {
      PBC_Failed("SAL_Create_Thread: Invalid last function parameter");
   }

   while ((!Thread_Running_1) && (i++ < 10))
   {
      SAL_Sleep(100);
   }
   
   /* check if function Create_Thread_Fun has started */
   if (!Thread_Running_1)
   {
      PBC_Failed("SAL_Create_Thread: Thread function has not been started");
   }

   Tr_Info_Lo("Create_Param_Light_Fun_1: End");
}


/**  Function of PARAM2_THREAD.
 *
 *   - tests the first three parameters of the function SAL_Create_Thread
 */

void Create_Param_Light_Fun_2(void* param)
{
   SAL_Thread_Id_T thread_id;
   SAL_Priority_T priority;
   SAL_Int_T i = 0;
   char const *tname = SAL_Get_Thread_Name_Self();

   Tr_Info_Lo("Create_Param_Light_Fun_2: Start");

   Thread_Running_3 = 1;

   thread_id = SAL_Get_Thread_Id();

   SAL_Get_Thread_Priority(thread_id, &priority);

   /* check if the first three parameters are valid */
   if ( (NULL == tname) || (strcmp(Data_2.thread_name, tname) ) ||
       (Data_2.thread_id != thread_id ) ||
       (Data_2.priority != priority) )
   {
      PBC_Failed("SAL_Create_Thread: Invalid parameters");
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   Tr_Info_Lo("Create_Param_Light_Fun_2: End");
}


/**  Test of SAL_Create_Thread and SAL_Wait_Destroyed_Timeout function.
 *
 *   Function creates three threads and starts SAL_Run.
 */

SAL_Int_T Create_Thread_Light_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { CREATE_THREAD, PARAM1_THREAD, PARAM2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   if (SAL_Create_Thread(
           Create_Thread_Light_Fun, 
           NULL, 
           Init_Thread_Attr("CREATE_T", CREATE_THREAD, priority, &thread_attr)) 
        == SAL_UNKNOWN_THREAD_ID)
   {
      Tr_Fault("SAL_Create_Thread: Thread has not been created");
      return 1;
   }

   Data_1.thread_name = "Name";
   Data_1.thread_id = 5;

   SAL_Create_Thread(
      Create_Param_Light_Fun_1, 
      (void*)&Data_1, 
      Init_Thread_Attr("PARAM1_T", PARAM1_THREAD, priority, &thread_attr));

   Data_2.thread_name  = "PARAM2_T";
   Data_2.thread_id    = PARAM2_THREAD;
   Data_2.priority     = priority;

   SAL_Create_Thread(
      Create_Param_Light_Fun_2, 
      NULL, 
      Init_Thread_Attr(Data_2.thread_name, Data_2.thread_id, Data_2.priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;

}


/** \}
 */


/**  \name Functions of Wait_Destroyed_Timeout_Test
 *
 *   \{
 */

/**  Function of THREAD.
 *
 *   - sets ThreadRunning variable to 1
 *   - sleeps for 1000 ms
 */

void Run_Thread_Fun(void * param)
{
   Tr_Info_Lo("Run_Thread_Fun: Start");

   Thread_Running_2 = 1;

   SAL_Sleep(1000);

   Tr_Info_Lo("Run_Thread_Fun: Stop");
}

/**  Function of DESTROY_THREAD.
 *
 *   - waits until THREAD terminates
 *   - timeout is reached before THREAD terminates (timeout is 100 ms)
 */

void Wait_Destroyed_Timeout_Fun(void* param)
{
   static SAL_Thread_Id_T destroyed_list[] = { THREAD };
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Wait_Destroyed_Timeout_Fun: Start");

   while (!Thread_Running_2)
   {
      SAL_Sleep(100);
   }

   time_1 = SAL_Clock();

   ret_timeout = SAL_Wait_Destroyed_Timeout(destroyed_list, 
                     sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
                     100);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (!ret_timeout)
   {
      if ((delta_time < 100 - LESSER_TIME_WINDOW_MS) || (delta_time > 100 + GREATER_TIME_WINDOW_MS))
      {
         PBC_Failed_1("SAL_Wait_Destroyed_Timeout: invalid interval of timeout, expected: 100, achieved: %d", (int)delta_time);
      }
   } 
   else /* ret_timeout */
   {
      PBC_Failed("SAl_Wait_Destroyed_Timeout: returns 1 when timeout should be reached");
   }

   Tr_Info_Lo("Wait_Destroyed_Timeout_Fun: Stop");
}


/**  Test of SAL_Wait_Destroyed_Timeout function.
 *
 *   Function creates two threads and starts SAL_Run.
 */

SAL_Int_T Wait_Destroyed_Timeout_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { THREAD, DESTROY_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   
   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   if (SAL_Create_Thread(      
          Run_Thread_Fun, 
          NULL, 
          Init_Thread_Attr("THREAD", THREAD, priority, &thread_attr)) 
        == SAL_UNKNOWN_THREAD_ID)
   {
      Tr_Fault("SAL_Create_Thread: Thread has not been created");
      return 1;
   }

   SAL_Create_Thread(
      Wait_Destroyed_Timeout_Fun, 
      NULL, 
      Init_Thread_Attr("DESTR_T", DESTROY_THREAD, priority, &thread_attr));

    ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;
}

/**  \}
 */


/**  \name Functions of Thread_Fun_Test
 *
 *   \{
 */


/**  Function of THREAD.
 *
 *   - creates the queue
 *   - sets variable Thread_Running_2 = 1 
 *   - receives the message
 */

void Thread_Fun(void* param)
{
   Tr_Info_Lo("Thread_Fun: Start");

   if (!SAL_Create_Queue(5, 0, malloc, free))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Signal_Ready();

   Thread_Running_2 = 1;

#if defined (XSAL)

   SAL_Receive();

#else

   if (!SAL_Receive_Timeout(Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Thread_Fun: End");
}

/**  Function of FUN_THREAD.
 *
 *   - tests SAL_Is_Thread_Alive, SAL_Get_Thread_Name, SAL_Get_Thread_Name_Self, 
 *     SAL_Get_Thread_Id, SAL_Sleep functions
 */

void Thread_Fun_Test_Fun(void* param)
{
   SAL_Priority_T priority1 = SAL_NORMAL_PRIORITY;
   SAL_Priority_T priority2;
   SAL_Int_T i = 0;
   static SAL_Thread_Id_T wait_list[] = { THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   char const *tname = SAL_Get_Thread_Name_Self();


   Tr_Info_Lo("Thread_Fun_Test_Fun: Start");

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Thread_Fun, 
      NULL, 
      Init_Thread_Attr("THREAD", THREAD, priority, &thread_attr));

   while (!Thread_Running_2)
   {
      SAL_Sleep(100);
   }

   /* test of SAL_Is_Thread_Alive */
   if (!SAL_Is_Thread_Alive(THREAD))
   {
      PBC_Failed("SAL_Is_Thread_Alive: Thread is not alive");
   }

   /* test of SAL_Get_Thread_Name */
   if (strcmp("THREAD", SAL_Get_Thread_Name(THREAD)))
   {
      PBC_Failed("SAL_Get_Thread_Name: Invalid name of thread");
   }

   /* test of SAL_Get_Thread_Name_Self */
   if ((NULL == tname) || strcmp(Data_1.thread_name, tname))
   {
      PBC_Failed("SAL_Get_Thread_Name: Invalid name-self");
   }

   /* test of SAL_Get_Thread_Id */
   if (Data_1.thread_id != SAL_Get_Thread_Id())
   {
      PBC_Failed("SAL_Get_Thread_Id: Invalid thread id");
   }

   if (!SAL_Get_Thread_Priority(THREAD, &priority2))
   {
      PBC_Failed("SAL_Get_Thread_Priority: returns 0");
   }

   /* /if (SAL_NORMAL_PRIORITY != Priority2) !!! */
   if (priority != priority2)
   {
      PBC_Failed("SAL_Get_Thread_Priority: Invalid priority of THREAD");
   }

   priority1 = SAL_HIGH_PRIORITY;

   SAL_Set_Thread_Priority(THREAD, priority1);

   if (!SAL_Get_Thread_Priority(THREAD, &priority2))
   {
      PBC_Failed("SAL_Get_Thread_Priority: returns 0");
   }

   if (priority2 != priority1)
   {
      PBC_Failed_2("SAL_Set_Thread_Priority: Invalid setting a priority of THREAD. Set Value %d Get Value %d", priority1, priority2);
   }

   /* test of SAL_Sleep */

   time_1 = SAL_Clock();

   SAL_Sleep(1000);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);


   if ((delta_time < 1000 - LESSER_TIME_WINDOW_MS) || (delta_time > 1000 + GREATER_TIME_WINDOW_MS))
   {
      Tr_Fault_1("SAL_Sleep: time interval is not valid, expected: 1000 ms, achieved: %d ms", (int)delta_time);
   }

   if ((delta_time < 1000 - (2* LESSER_TIME_WINDOW_MS)) || (delta_time > 1000 + (2 * GREATER_TIME_WINDOW_MS)))
   {
      PBC_Failed_1("SAL_Sleep: time interval is not valid, expected: 1000 ms, achieved: %d ms", (int)delta_time);
   }

   /* test of SAL_Delay */

   time_1 = SAL_Clock();

   SAL_Delay(1000*1000);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if ((delta_time < 1000 - LESSER_TIME_WINDOW_MS) || (delta_time > 1000 + GREATER_TIME_WINDOW_MS))
   {
      PBC_Failed_1("SAL_Delay: time interval is not valid: %d", (int)delta_time);
   }

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, 
          sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   SAL_Send(APP_ID, THREAD, EV_STOP, NULL, 0);

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   /* test of SAL_Is_Thread_Alive */
   if (SAL_Is_Thread_Alive(THREAD))
   {
      PBC_Failed("SAL_Is_Thread_Alive: Thread THREAD is alive");
   }

   Tr_Info_Lo("Thread_Fun_Test_Fun: End");
}


/**  Test of the thread functions.
 *   
 *   - tests the thread functions like SAL_Is_Thread_Alive, SAL_Get_Thread_Name, 
 *     SAL_Get_Thread_Name_Self, SAL_Get_Thread_Id, SAL_Sleep.
 */

SAL_Int_T Thread_Fun_Test()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { FUN_THREAD, THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Data_1.thread_name  = "FUN_T";
   Data_1.thread_id    = FUN_THREAD;

   SAL_Create_Thread(
      Thread_Fun_Test_Fun, 
      NULL, 
      Init_Thread_Attr("FUN_T", FUN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;
}

/**  \}
 */


/**  \name Functions of Priority_Test
 *
 *   \{
 */

/**  Function of VHIGH_P_THREAD, HIGH_P_THREAD, NORMAL_P_THREAD, LOW_P_THREAD, VLOW_P_THREAD.   
 *
 *   - function checks if the priority level is correct
 */

void Prior_Level_Fun(void* param)
{
   SAL_Priority_T priority1 = (SAL_Priority_T) (intptr_t) param;
   SAL_Priority_T priority2;

   Tr_Info_Lo("Prior_Level_Fun: Start");

   if (!SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority2))
   {
      PBC_Failed("SAL_Get_Priorty: returns 0");
   }

   if (priority1 != priority2)
   {
      PBC_Failed("Invalid level priority");
   }

   Tr_Info_Lo("Prior_Level_Fun: End");
}


/**  Function of MAIN_THREAD.
 *
 *   - creates six threads, each with different priority level
 *   - the last thread has an inherited priority
 */

void Priority_Fun(void* param)
{
   SAL_Thread_Attr_T thread_attr;

   Tr_Info_Lo("Priority_Fun: Start ");

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_VERY_HIGH_PRIORITY, 
      Init_Thread_Attr("VHIGH_P", VHIGH_P_THREAD, SAL_VERY_HIGH_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_HIGH_PRIORITY, 
      Init_Thread_Attr("HIGH_P", HIGH_P_THREAD, SAL_HIGH_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_NORMAL_PRIORITY, 
      Init_Thread_Attr("NORM_P", NORMAL_P_THREAD, SAL_NORMAL_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_LOW_PRIORITY, 
      Init_Thread_Attr("LOW_P", LOW_P_THREAD, SAL_LOW_PRIORITY, &thread_attr));

   SAL_Create_Thread(
      Prior_Level_Fun, 
      (void*)SAL_VERY_LOW_PRIORITY, 
      Init_Thread_Attr("VLOW_P", VLOW_P_THREAD, SAL_VERY_LOW_PRIORITY, &thread_attr));

   Tr_Info_Lo("Priority_Fun: End ");
}

/**  Test of thread priorities.
 *
 *   Creates one thread and starts SAL_Run.
 */

SAL_Int_T Priority_Test()
{
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Priority_Fun, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
          sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
          Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;
}

/**  \}
 */


/**  \name Functions of Wait_Ready_Timeout_Test_1
 *
 *   \{
 */

/**  Function of WAIT_THREAD.
 *
 *   - waits until SIGNAL_THREAD signals that is ready
 *   - creates the queue 
 *   - tries to wait for the second time after SIGNAL_THREAD has signalled that is ready
 *   - tries to receive message
 */

void Wait_Timeout_Fun_1(void* param)
{
   static SAL_Thread_Id_T wait_list[] = { SIGNAL_THREAD };
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD };
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   time_1 = SAL_Clock();

   /* should wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 200;

   if (ret_timeout)
   {
      if (abs(res_time) > (2 *GREATER_TIME_WINDOW_MS))
      {
         PBC_Failed_1("SAL_Wait_Ready_Timeout: thread has been suspended for %d ms, expected time: 200 ms", (int)delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_Wait_Ready_Timeout: returns 0");
   }

   SAL_Create_Queue(5, 0, malloc, free);

   Thread_Running_1 = 1;

   time_1 = SAL_Clock();

   /* should not wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         PBC_Failed_1("SAL_Wait_Ready_Timeout: suspends the thread after SAL_Signal_Ready for %d ms", (int)delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_Wait_Ready_Timeout: returns 0 after SAL_Signal_Ready");
   }

   /* should receive message */
   if (SAL_Receive_Timeout(Long_Timeout) == NULL)
   {
      PBC_Failed("Timeout has been reached");
   }

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }
}


/**  Function of SIGNAL_THREAD.
 *
 *   - tries to send message to WAIT_THREAD which hasn't created the queue yet
 *   - signals that is ready
 *   - tries to send the messages to WAIT_THREAD
 *   - checks if message has been received
 */


void Signal_Timeout_Fun_1(void* param)
{
   SAL_Stat_Queue_T queue_stat;
   SAL_Int_T i = 0;

   /* try to send message to the queue which shouldn't exist */
   if (SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      PBC_Failed("SAL_Wait_Ready: doesn't suspend the thread");
   }

   /* after SAL_Signal_Ready the other thread should create the queue */
   SAL_Signal_Ready();

   while (!Thread_Running_1)
   {
      SAL_Sleep(100);
   }

   /* here message should be sent because queue should exist*/
   if (!SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      PBC_Failed("SAL_Signal_Ready: doesn't unblock suspended thread");
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   SAL_Stat_Queue(WAIT_THREAD, &queue_stat);

   /* check if the other thread received message */
   if (queue_stat.message_count == 1)
   {
      PBC_Failed("SAL_Wait_Ready: suspends the thread after SAL_Signal_Ready");
   }
}


/**  Function of MAIN_THREAD.
 *
 *   - creates two threads
 *   - thread WAIT_THREAD which includes SAL_Wait_Ready function starts 
 *     earlier than thread SIGNAL_THREAD with SAL_Signal_Ready function.
 */

void Main_Timeout_Fun_1(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Wait_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("WAIT_T", WAIT_THREAD, priority, &thread_attr));
   
   SAL_Sleep(200);

   SAL_Create_Thread(
      Signal_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("SIGNAL_T", SIGNAL_THREAD, priority, &thread_attr));
}


/**  Test of SAL_Signal_Ready and SAL_Wait_Ready functions.
 *
 *   Creates thread MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Wait_Ready_Timeout_Test_1()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD, WAIT_THREAD, SIGNAL_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Main_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));
   
   ret =  SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;
}

/**  \}
 */


/**  \name Functions of Wait_Ready_Timeout_Test_2
 *
 *   \{
 */

/**  Function of WAIT_THREAD.
 *
 *   - waits until SIGNAL_THREAD signals that is ready
 *   - creates the queue 
 *   - tries to wait for the second time after SIGNAL_THREAD has signalled that is ready
 *   - tries to receive message
 */

void Wait_Timeout_Fun_2(void* param)
{
   static SAL_Thread_Id_T wait_list[] = { SIGNAL_THREAD };
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD };
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   time_1 = SAL_Clock();

   /* should wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     100);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 100;

   if (!ret_timeout)
   {
      if (abs(res_time) > GREATER_TIME_WINDOW_MS)
      {
         PBC_Failed_1("SAL_Wait_Ready_Timeout: invalid interval of timeout, expected: 100, achieved: %d", (int)delta_time);
      }
   }
   else /* ret_timeout */
   {
      PBC_Failed("SAL_Wait_Ready_Timeout: returns 1");
   }

   SAL_Create_Queue(5, 0, malloc, free);

   Thread_Running_1 = 1;

   while (!Thread_Running_2)
   {
      SAL_Sleep(100);
   }

   time_1 = SAL_Clock();

   /* should not wait here */
   ret_timeout = SAL_Wait_Ready_Timeout(wait_list, 
                     sizeof(wait_list)/sizeof(SAL_Thread_Id_T), 
                     Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         PBC_Failed_1("SAL_Wait_Ready_Timeout: suspends the thread after SAL_Signal_Ready for %d ms", (int)delta_time);
      }
   }
   else
   {
      PBC_Failed("SAL_Wait_Ready_Timeout: returns 0");
   }


   /* should receive message */
   if (SAL_Receive_Timeout(Long_Timeout) == NULL)
   {
      PBC_Failed("Timeout has been reached");
   }

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }
}


/**  Function of SIGNAL_THREAD.
 *
 *   - signals that is ready
 *   - tries to send the messages to the WAIT_THREAD
 *   - checks if message has been received
 */


void Signal_Timeout_Fun_2(void* param)
{
   SAL_Stat_Queue_T queue_stat;
   SAL_Int_T i = 0;

   /* after SAL_Signal_Ready the other thread should create the queue */
   SAL_Signal_Ready();

   Thread_Running_2 = 1;

   while (!Thread_Running_1)
   {
      SAL_Sleep(100);
   }

   /* here message should be sent because queue should exist*/
   if (!SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      PBC_Failed("SAL_Signal_Ready: doesn't unblock suspended thread");
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   SAL_Stat_Queue(WAIT_THREAD, &queue_stat);

   /* check if the other thread received message */
   if (queue_stat.message_count == 1)
   {
      PBC_Failed("SAL_Wait_Ready: suspends the thread after SAL_Signal_Ready");
   }
}


/**  Function of MAIN_THREAD.
 *
 *   - creates two threads
 *   - thread WAIT_THREAD which includes SAL_Wait_Ready function starts
 *     earlier than thread SIGNAL_THREAD with SAL_Signal_Ready function.
 */

void Main_Timeout_Fun_2(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Wait_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("WAIT_T", WAIT_THREAD, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Signal_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("SIGNAL_T", SIGNAL_THREAD, priority, &thread_attr));
}


/**  Test of SAL_Signal_Ready and SAL_Wait_Ready functions.
 *
 *   Creates thread MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Wait_Ready_Timeout_Test_2()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD, WAIT_THREAD, SIGNAL_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Main_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));
   
   ret =  SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, 
           sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), 
           Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;
}


#if defined (XSAL)

/**  \name Functions of Wait_Ready_Test
 * 
 *   \{
 */

/**  Function of WAIT_THREAD.
 *
 *   - waits until SIGNAL_THREAD signals that is ready
 *   - creates the queue 
 *   - tries to wait after SIGNAL_THREAD has signalled that is ready
 *   - receives the message
 */

void Wait_Fun(void* param)
{
   static SAL_Thread_Id_T wait_list[] = { SIGNAL_THREAD };
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD };

   /* should wait here */
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_Create_Queue(5, 0, malloc, free);

   Thread_Running_1 = 1;

   /* should not wait here */
   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   /* should receive message */
   SAL_Receive();

   Thread_Running_2 = 1;

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
}


/**  Function of SIGNAL_THREAD.
 *
 *   - tries to send message when the queue hasn't been created yet
 *   - signals that is ready
 *   - tries to send the messages to the WAIT_THREAD
 *   - checks if message has been received
 */

void Signal_Fun(void* param)
{
   SAL_Stat_Queue_T queue_stat;
   SAL_Int_T i = 0;

   /* try to send message to the queue which shouldn't exist */
   if (SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      PBC_Failed("SAL_Wait_Ready: doesn't suspend the thread");
   }

   /* after SAL_Signal_Ready the other thread should create the queue */
   SAL_Signal_Ready();

   while (!Thread_Running_1)
   {
      SAL_Sleep(100);
   }

   /* here message should be sent because queue should exist*/
   if (!SAL_Send(APP_ID, WAIT_THREAD, EV_E1, NULL, 0))
   {
      PBC_Failed("SAL_Signal_Ready: doesn't unblock suspended thread");
   }

   while (i++ < 5)
   {
      SAL_Sleep(100);
   }

   SAL_Stat_Queue(WAIT_THREAD, &queue_stat);

   /* check if the other thread received message */
   if (queue_stat.message_count == 1)
   {
      PBC_Failed("SAL_Wait_Ready: suspends the thread after SAL_Signal_Ready");
   }
}


/**  Function of MAIN_THREAD.
 *
 *   - creates two threads
 *   - thread WAIT_THREAD which includes SAL_Wait_Ready function starts
 *     earlier than thread SIGNAL_THREAD with SAL_Signal_Ready function.
 */

void Main_Fun(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Wait_Fun, 
      NULL, 
      Init_Thread_Attr("WAIT_T", WAIT_THREAD, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Signal_Fun, 
      NULL, 
      Init_Thread_Attr("SIGNAL_T", SIGNAL_THREAD, priority, &thread_attr));
}


/**  Test of SAL_Signal_Ready and SAL_Wait_Ready functions.
 *
 *   Creates thread MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Wait_Ready_Test()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] = { MAIN_THREAD, WAIT_THREAD, SIGNAL_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Main_Fun, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));
   
   ret =  SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

#endif


SAL_Int_T Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_Int_T Option;
   SAL_Config_T Config;
   SAL_Priority_T priority = 2; /* no priority test */

   while ( ( Option = pi_getopt( argc, argv, "pg:v:l:teN:" ) ) != -1 ) 
   {        
      switch(Option) 
      {             
         case 'p':
            priority = 1; /* with priority test */
            break;

         default:
            break;
      }     
   }

   SAL_Get_Config(&Config);

   Config.app_id = APP_ID;
   Config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&Config))
   {
      Tr_Fault("SAL_Init error");
      return 0;
   }

   return priority;
}


void Tear_Down()
{
}

#if defined (XSAL_POSIX)
#include <sched.h>
#endif /* XSAL_POSIX */

int main (int argc, char** argv)
{
   SAL_Int_T ret = 0;
   SAL_Priority_T priority = 0;
   SAL_App_Id_T app_id;
   SAL_Int_T max_numb_of_threads;

   priority = Set_Up(argc, argv);

   if (!priority)
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   if ((app_id = SAL_Get_App_Id()) != APP_ID)
   {
      PBC_Failed("SAL_Get_App_Id: Invalid Application Identifier");
   }

   if ((max_numb_of_threads = SAL_Get_Max_Number_Of_Threads()) != MAX_NUMB_OF_THREADS)
   {
      PBC_Failed("SAL_Get_Max_Number_Of_Threads: Invalid max number of threads");
   }

#if defined (XSAL_POSIX)
   {
      struct timespec ts;

      if (0 == sched_rr_get_interval(0, &ts))
      {
         Tr_Fault_2("Round Robin Interval %ld.%09ld secs", (long int)ts.tv_sec, (long int)ts.tv_nsec);
      }
      else
      {
         Tr_Fault("unable to get round robin interval");
      } 
   }
#endif /* XSAL_POSIX */

#if defined (XSAL)

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Create_Thread_Test();

#endif

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   Create_Thread_Light_Test();

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Wait_Destroyed_Timeout_Test();

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Thread_Fun_Test();

   if (priority == 1)
   {
      ret = ret || Priority_Test();
   }

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Wait_Ready_Timeout_Test_1();

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Wait_Ready_Timeout_Test_2();


#if defined (XSAL)

   Thread_Running_1 = 0;
   Thread_Running_2 = 0;
   Thread_Running_3 = 0;

   ret = ret || Wait_Ready_Test();

#endif

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
