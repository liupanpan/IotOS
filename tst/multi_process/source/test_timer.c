/* -------------------------------------------------------------------------
 *
 * File:        test_timer.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of timers.
 *
 * -----------------------------------------------------------------------*/

#include <stdio.h>
#include <signal.h>
#include <malloc.h>

#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 9);

#define APP_ID 2

typedef enum
{
   TIMER1 = SAL_EV_FIRST_PRIVATE_ID, EV_1, EV_2, EV_3, EV_STOP,
} Test_Private_Event_Id_T;

/* For tests that check actual times, an error is indicated by being off more than this %. */
#define MAX_PCNT_OVER_ALLOWED 25

/* Used as a divisor to perform integer arithmetic in checking for a time value being over. */
#define MAX_OVER_DIVISOR (100/MAX_PCNT_OVER_ALLOWED)

/* For tests that check actual times, an warning is printed for being off more than this %. */
#define WARN_PCNT_OVER 15

/* Used as a divisor to perform integer arithmetic in checking for a time value being over. */
#define WARN_OVER_DIVISOR (100/WARN_PCNT_OVER)

typedef enum Thread_Id_Tag
{
   MAIN_THREAD = PM_LAST_THREAD_ID, TIMER_THREAD, Single_Shot_Thread, MAX_NUMB_OF_THREADS
} Thread_Id_T;

SAL_Int_T Timeout = 3000;

bool_t Failed = false;

/**  \file
 *
 *   Test of all timer functions.
 *
 *   \section T Test:
 *   <OL>
 *   <LI> \ref PSTT1 - test of SAL_Create_Timer and SAL_Start_Timer functions
 *      for periodic timer. </LI>
 *   <LI> \ref PSTT2 - test of SAL_Stop_Timer function for periodic timer. </LI>
 *   <LI> \ref SSTT1 - test of SAL_Start_Timer function for single timer. </LI>
 *   <LI> \ref SSTT2 - test of single timer. </LI>
 *   <LI> \ref TTT1 - test of timer interval. </LI>
 *   <LI> \ref TTT2 - test of timer interval. </LI>
 *   <LI> \ref TTT3 - test of timer interval. </LI>
 *   <LI> \ref CTTT1 - test of changing time interval. </LI>
 *   <LI> \ref CTTT2 - test of changing time interval. </LI>
 *   </OL>
 *
 *   \section PSTT1 Periodic_Start_Timer_Test 
 *
 *   Two threads are created: TIMER_THREAD and MAIN_THREAD.
 *
 *   \subsection TP1 Test procedure:
 *  
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts periodic timer with interval of 100 ms. </LI>
 *   <LI> MAIN_THREAD sends EV_STOP message to TIMER_THREAD after 200 ms. </LI>
 *   <LI> TIMER_THREAD receives event. </LI>
 *   <LI> If received event is not from timer, SAL_Exit is called. </LI>
 *   <LI> In the other case, TIMER_THREAD receives two more events from timer. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section PSTT2 Periodic_Stop_Timer_Test
 *
 *   Two threads are created: TIMER_THREAD and MAIN_THREAD.
 *
 *   \subsection TP2 Test procedure:
 *      
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts periodic timer with interval of 100 ms. </LI>
 *   <LI> TIMER_THREAD receives three events from timer. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> MAIN_THREAD sends EV_STOP message to TIMER_THREAD. </LI>
 *   <LI> TIMER_THREAD receives the event. </LI>
 *   <LI> If received event is different from EV_STOP, SAL_Exit is called. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section SSTT1 Single_Start_Timer_Test_1
 *
 *   Two threads are created: TIMER_THREAD and MAIN_THREAD.
 *
 *   \subsection TP3 Test procedure:
 *
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts single timer with interval of 100 ms. </LI>
 *   <LI> MAIN_THREAD sends EV_STOP message to TIMER_THREAD after 200 ms. </LI>   
 *   <LI> TIMER_THREAD receives event. </LI>
 *   <LI> If received event is not from timer, SAL_Exit is called. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section SSTT2 Single_Start_Timer_Test_2
 *
 *   Two threads are created: TIMER_THREAD and MAIN_THREAD.
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts single timer with interval of 100 ms. </LI>
 *   <LI> TIMER_THREAD receives timer event. </LI>
 *   <LI> MAIN_THREAD sends EV_STOP message to TIMER_THREAD. </LI>
 *   <LI> TIMER_THREAD receives EV_STOP event. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section TTT1 Tick_Timer_Test_1
 *
 *   One thread is created: TIMER_THREAD.
 *
 *   \subsection TP6 Test procedure:
 *
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts periodic timer with interval of 100 ms. </LI>
 *   <LI> TIMER_THREAD receives ten events from timer and saves receving time
 *      for each received event. </LI>
 *   <LI> TIMER_THREAD checks if time intervals between receiving times
 *      are correct. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section TTT2 Tick_Timer_Test_2
 *
 *   One thread is created: TIMER_THREAD.
 *
 *   \subsection TP7 Test procedure:
 *
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts periodic timer with interval of 100 ms. </LI>
 *   <LI> TIMER_THREAD stores the time before receiving events. </LI>
 *   <LI> TIMER_THREAD receives ten events from timer. </LI>
 *   <LI> TIMER_THREAD stores the time after receiving events. </LI>
 *   <LI> TIMER_THREAD checks if the time interval is correct. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section TTT3 Tick_Timer_Test_3
 *
 *   One thread is created: TIMER_THREAD.
 *
 *   \subsection TP8 Test procedure:
 *
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts periodic timer with interval of 100 ms. </LI>
 *   <LI> TIMER_THREAD receives ten events from timer and saves receving time
 *      for each received event from timestamps. </LI>
 *   <LI> TIMER_THREAD checks if the time interval is correct. </LI>
 *   <LI> TIMER_THREAD checks if time intervals between receiving times
 *      are correct. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section CTTT1 Change_Tick_Timer_Test_1
 *
 *   One thread is created: TIMER_THREAD.
 *
 *   \subsection TP9 Test procedure:
 *
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts periodic timer with interval of 500 ms. </LI>
 *   <LI> TIMER_THREAD receives ten events from timer and saves receving time
 *      for each received event. </LI>
 *   <LI> TIMER_THREAD starts the same timer with interval of 1000 ms. </LI>
 *   <LI> TIMER_THREAD receives twenty events from timer and saves receving time
 *      for each received event. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   </OL>
 *
 *   \section CTTT2 Change_Tick_Timer_Test_2
 *
 *   One thread is created: TIMER_THREAD.
 *
 *   \subsection TP10 Test procedure:
 *
 *   <OL>
 *   <LI> TIMER_THREAD creates the queue. </LI>
 *   <LI> TIMER_THREAD creates the timer. </LI>
 *   <LI> TIMER_THREAD starts periodic timer with interval of 1000 ms. </LI>
 *   <LI> TIMER_THREAD stores the time. </LI>
 *   <LI> After 500 ms TIMER_THREAD starts the same timer with time interval 2000 ms. </LI>
 *   <LI> TIMER_THREAD receives event. </LI>
 *   <LI> TIMER_THREAD stores the time. </LI>
 *   <LI> TIMER_THREAD stops timer. </LI>
 *   <LI> TIMER_THREAD destroys timer. </LI>
 *   <LI> TIMER_THREAD checks if time interval is valid. </LI>
 *   </OL>
 */

void Send_Thread_1(void* param);
void Send_Thread_2(void* param);

static SAL_Message_T const * Wait_Message(void)
{
   SAL_Message_T const * msg;

#if defined (XSAL)

   msg = SAL_Receive();

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   return msg;
}

const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, SAL_Thread_Id_T id, SAL_Priority_T prior, SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}

/** \name Functions of Periodic_Start_Timer_Thread
 *
 *  \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts the timer
 *   - receives timer events
 *   - then stops and destroys the timer
 */

void Period_Start_Timer(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;
   SAL_Int_T i = 0;

   Tr_Info_Lo("Period_Start_Timer: Start");

   SAL_Create_Queue(3, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   SAL_Start_Timer(timer_id, 100, 1);

   SAL_Signal_Ready();

   msg = Wait_Message();

   if ((int) msg->event_id == EV_STOP)
   {
      PBC_Failed("SAL_Start_Timer: Timer doesn't generate events after being started");
   }

   while (i < 2)
   {
      msg = Wait_Message();
      Tr_Info_Mid_1("%d", (int)msg->event_id);
      i++;
   }

   SAL_Stop_Timer(timer_id);

   /* verify timer is destroyed by thread exit SAL_Destroy_Timer(timer_id); */

   Tr_Info_Lo("Period_Start_Timer: Stop");
}

/**  Test of SAL_Create_Timer and SAL_Start_Timer functions.
 *
 *   Tests if periodic timer generates events after being started.
 */

SAL_Int_T Periodic_Start_Timer_Test()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD, MAIN_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Period_Start_Timer, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Send_Thread_1, NULL, Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;
}

/** \}
 */

/**  \name Functions of Periodic_Stop_Timer_Test
 *
 *   \{
 */

/**  Function of TIMER_THREAD.
 *   
 *   - creates and starts the timer
 *   - receives timer events
 *   - stops the timer
 */

void Period_Stop_Timer(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;
   SAL_Int_T i = 0;

   Tr_Info_Lo("Period_Stop_Timer: Start");

   SAL_Create_Queue(4, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   SAL_Start_Timer(timer_id, 100, 1);

   while (i < 3)
   {

      msg = Wait_Message();
      Tr_Info_Mid_1("%d", (int)msg->event_id);
      i++;
   }

   SAL_Stop_Timer(timer_id);

   SAL_Signal_Ready();

   msg = Wait_Message();

   if ((int) msg->event_id != EV_STOP)
   {
      PBC_Failed_1("SAL_Start_Timer: Timer generates events after being stopped, event_id: %d", (int)msg->event_id);
   }

   SAL_Destroy_Timer(timer_id); /* !!! */

   Tr_Info_Lo("Period_Stop_Timer: Stop");

}

/**  Test of SAL_Stop_Timer function for periodic timer.
 * 
 *   Tests if periodic timer generates events after being stopped.
 */

SAL_Int_T Periodic_Stop_Timer_Test()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD, MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Period_Stop_Timer, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Send_Thread_2, NULL, Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }
#endif

   return ret;
}

/**  \}
 */

/**  \name Functions of Single_Start_Timer_Test_1
 *
 *   \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts the timer
 *   - receives timer event
 *   - stops and destroys timer
 */

void Single_Start_Timer_1(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;
   static SAL_Thread_Id_T destroyed_list[] =
   { MAIN_THREAD };

   Tr_Info_Lo("Single_Start_Timer_1: Start");

   SAL_Create_Queue(2, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   SAL_Start_Timer(timer_id, 100, 0);

   SAL_Signal_Ready();

   msg = Wait_Message();

   Tr_Info_Mid_1("%d", (int)msg->event_id);

   if ((int) msg->event_id == EV_STOP)
   {
      PBC_Failed("SAL_Start_Timer: Timer doesn't generate events after starting");
   }

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      PBC_Failed("Timeout has been reached");
   }

   SAL_Destroy_Timer(timer_id);

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

   Tr_Info_Lo("Single_Start_Timer_1: Stop");
}

/** Tests of SAL_Start_Timer function for single timer.
 *
 *  Tests if single timer generates event after being started.
 */

SAL_Int_T Single_Start_Timer_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD, MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Single_Start_Timer_1, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Send_Thread_1, NULL, Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }
#endif

   return ret;
}

/**  \}
 */

/**  \name Functions of Single_Start_Timer_Test_2
 *
 *   \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts the timer
 *   - receives timer event 
 *   - stops and destroys the timer
 */

void Single_Start_Timer_2(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;

   Tr_Info_Lo("Single_Start_Timer_2: Start");

   SAL_Create_Queue(2, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   SAL_Start_Timer(timer_id, 100, 0);

   msg = Wait_Message();

   Tr_Info_Mid_1("%d", (int)msg->event_id);

   SAL_Signal_Ready();

   msg = Wait_Message();

   if ((int) msg->event_id != EV_STOP)
   {
      PBC_Failed_1("SAL_Start_Timer: Single timer generates more than one event, event_id: %d", (int)msg->event_id);
   }

   SAL_Destroy_Timer(timer_id);

   Tr_Info_Lo("Single_Start_Timer_2: Stop");
}

/**  Test of single timer.
 *   
 *   Tests if single timer generates more than one event.
 */

SAL_Int_T Single_Start_Timer_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD, MAIN_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Single_Start_Timer_2, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Send_Thread_2, NULL, Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;

}

/**
 * Test timers with short timeout values.
 */
static void Short_Timer_Test(void* param)
{
   SAL_Timer_Id_T timer_id;
   const SAL_Message_T* msg;
   int timeout_ms;

   Tr_Info_Lo("Short_Timer_Test: Start");

   SAL_Create_Queue(16, 0, SAL_Alloc, SAL_Free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   for (timeout_ms = 0; timeout_ms < 20; timeout_ms++)
   {
      SAL_Clock_T end_time;
      SAL_Clock_T start_time;

      start_time = SAL_Clock();

      SAL_Start_Timer(timer_id, timeout_ms, false);

      msg = SAL_Receive_Timeout(1000);

      end_time = SAL_Clock();

      if (msg == NULL)
      {
         PBC_Failed("Timer did not go off!");
      }
      else
      {
         PBC_Ensure_2((int)msg->event_id == TIMER1, "Expected timer event %d, got %d",
            (int)TIMER1, (int)msg->event_id);
      }
      /*
       * Because SAL_Clock() is not driven by the h/w timer that drive the XSAL timers,
       * there can be a difference of up to 1 mS.
       */
      if ((int)(end_time + 1 - start_time) < timeout_ms)
      {
#ifndef WIN32
         Tr_Warn_2("Timer too short. Expected at least %d mS, got %d mS",
            (int)timeout_ms, (int)(end_time - start_time));
#else
         Failed = true;
         Tr_Fault_2("Timer too short. Expected at least %d mS, got %d mS",
            (int)timeout_ms, (int)(end_time - start_time));
#endif
      }
      Tr_Info_Lo_2("Short_Timer_Test() - Requested: %d mS; Actual: %d mS",
         (int)timeout_ms, (int)(end_time - start_time));
   }
   SAL_Destroy_Timer(timer_id);

   Tr_Info_Lo("Period_Start_Timer: Stop");
}

/**
 *  Test of timers for short timeout values.
 *
 *  Tests if short timers go off and never are less than the specified period.
 */
static SAL_Int_T Run_Short_Timer_Test()
{
   SAL_Int_T ret;
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD };
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Short_Timer_Test, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Timeout))
   {
      Tr_Fault("Timeout waiting for TIMER_THREAD to end");
      ret = 1;
   }

   return ret;
}

/**  \}
 */

/**  \name Functions of Tick_Timer_Test_1
 *
 *   \{
 */

/**  Function of Tick_Timer_Test_1 thread.
 *
 *   - checks the timer interval
 *   - stores the time of receiving all ten events
 *   - checks if the intervals between receiving times are correct
 */

void Tick_Timer_1(void* param)
{
   SAL_Timer_Id_T timer_id;
   SAL_Int_T timer_interval;
   const SAL_Message_T* msg;
   SAL_Clock_T t[11];
   SAL_Int_T dt[10];
   SAL_Int_T i;

   Tr_Info_Lo("Tick_Timer_1: Start");

   SAL_Create_Queue(10, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   timer_interval = 100;

   t[0] = SAL_Clock();

   SAL_Start_Timer(timer_id, timer_interval, 1);

   for (i = 0; i < 10; i++)
   {
      msg = Wait_Message();
      PBC_Ensure(msg != NULL, "NULL XSAL Message");
      t[i + 1] = SAL_Clock();
   }

   for (i = 0; i < 10; i++)
   {
      dt[i] = (SAL_Int_T) (t[i + 1] - t[0]) / (i + 1);

      if (dt[i] < timer_interval - 1)
      {
         Tr_Fault_2("Timer went of in less time (%d mS) than was requested (%d mS)",
            (int)dt[i], (int)timer_interval);
      }

      if (dt[i] > timer_interval + timer_interval / MAX_OVER_DIVISOR)
      {
         Failed = true;
         Tr_Fault_2("SAL_Start_Timer: Real Timer Interval (%d) is different from interval given as a parameter (%d)", (int)dt[i], (int)timer_interval);
      }

      if (dt[i] > timer_interval + timer_interval / WARN_OVER_DIVISOR)
      {
         Tr_Fault_2("SAL_Start_Timer: Real Timer Interval (%d) is different from interval given as a parameter (%d)", (int)dt[i], (int)timer_interval);
      }

      Tr_Info_Mid_2("Requested: %d; Actual: %d", (int)timer_interval, (int)dt[i]);
   }

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   Tr_Info_Lo("Tick_Timer_1: Stop");
}

/**  Test of timer interval.  
 *
 *   Test checks if the periodic timer interval is proper.
 */

SAL_Int_T Tick_Timer_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Tick_Timer_1, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;

}

/** \} 
 */

/**  \name Functions of Tick_Timer_Test_2
 *
 *   \{
 */

/**  Function of Tick_Timer_Test_2 thread.
 *
 *   - checks the timer interval
 *   - stores the time before receiving all ten events and time after receiving all of them
 *   - then checks if the interval before and after receiving events is correct
 */

void Tick_Timer_2(void* param)
{
   SAL_Timer_Id_T timer_id;
   SAL_Int_T timer_interval, tim_int;
   const SAL_Message_T* msg;
   SAL_Clock_T t1, t2;
   SAL_Int_T dt;
   SAL_Int_T i;

   Tr_Info_Lo("Tick_Timer_2: Start");

   SAL_Create_Queue(10, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   timer_interval = 100;

   SAL_Start_Timer(timer_id, timer_interval, 1);

   i = 0;

   t1 = SAL_Clock();

   while (i < 10)
   {
      msg = Wait_Message();
      PBC_Ensure(msg != NULL, "NULL XSAL Message");
      i++;
   }

   t2 = SAL_Clock();

   dt = (SAL_Int_T) (t2 - t1);

   Tr_Info_Mid_1("%d", (int)dt);

   tim_int = 10 * timer_interval;

   if (dt < (tim_int - 1))
   {
      Failed = true;
      Tr_Fault_2("10 Timer intervals took %d mS; should have been %d", (int)dt, (int)tim_int);
   }

   if (dt > tim_int + tim_int / WARN_OVER_DIVISOR)
   {
      Tr_Warn_2("10 Timer intervals took %d mS; should have been %d", (int)dt, (int)tim_int);
   }

   if (dt > tim_int + tim_int / MAX_OVER_DIVISOR)
   {
      Failed = true;
      Tr_Fault_2("10 Timer intervals took %d mS; should have been %d", (int)dt, (int)tim_int);
   }

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   Tr_Info_Lo("Tick_Timer_2: Stop");
}

/**  Test of timer interval.
 *   
 *   Function checks if the timer interval is proper.
 */

SAL_Int_T Tick_Timer_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Tick_Timer_2, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;

}

/** \}
 */

/**  \name Functions of Tick_Timer_Test_3
 *
 *   \{
 */

/**  Function of Tick_Timer_Test_3 thread.
 *
 *   - checks the timer interval
 *   - stores the time of receiving all ten events, the time is stored from timestamps
 *   - checks if the intervals between receiving times are correct
 */

void Tick_Timer_3(void* param)
{
   SAL_Timer_Id_T timer_id;
   SAL_Int_T timer_interval;
   const SAL_Message_T* msg;
   SAL_Clock_T t[11];
   SAL_Int_T dt[10];
   SAL_Int_T i;

   Tr_Info_Lo("Tick_Timer_3: Start");

   SAL_Create_Queue(10, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   timer_interval = 100;
   t[0] = SAL_Clock();

   SAL_Start_Timer(timer_id, timer_interval, 1);

   for (i = 0; i < 10; i++)
   {
      msg = Wait_Message();
      PBC_Ensure(msg != NULL, "NULL XSAL Message");
      t[i + 1] = SAL_Clock();
   }

   for (i = 0; i < 10; i++)
   {
      dt[i] = (SAL_Int_T) (t[i + 1] - t[0]) / (i + 1);

      if (dt[i] < timer_interval - 1)
      {
         Failed = true;
         Tr_Fault_2("Timer went of in less time (%d mS) than was requested (%d mS)",
            (int)dt[i], (int)timer_interval);
      }

      if (dt[i] > timer_interval + timer_interval / WARN_OVER_DIVISOR)
      {
         Tr_Fault_2("SAL_Start_Timer: Real timer interval (%d mS) is different from interval given as a parameter (%d mS)",
            (int)dt[i], (int)timer_interval);
      }

      if (dt[i] > timer_interval + timer_interval / MAX_OVER_DIVISOR)
      {
         Failed = true;
         Tr_Fault_2("SAL_Start_Timer: Real Timer Interval (%d mS) is different from interval given as a parameter (%d mS)",
            (int)dt[i], (int)timer_interval);
      }

      Tr_Info_Mid_2("Interval: %d mS; Requested: %d mS", (int)dt[i], (int)timer_interval);
   }

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   Tr_Info_Lo("Tick_Timer_3: Stop");
}

/**  Test of timer interval.  
 *
 *   Test checks if the periodic timer interval is proper.
 */

SAL_Int_T Tick_Timer_Test_3()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Tick_Timer_3, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;
}

/** \}
 */

/**  \name Functions of Change_Tick_Timer_Test_1
 *
 *   \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts periodic timer with period of 0,5 sec
 *   - after generating 10 events starts the same timer (without stopping it) with period of 1 sec
 *   - after generating 10 events of period 1 sec timer is stopped and destroyed
 */

void Change_Tick_Timer_1(void* param)
{
   const SAL_Message_T* msg;
   SAL_Timer_Id_T timer_id;
   SAL_Int_T i = 0;
   SAL_Clock_T t[21];
   SAL_Int_T dt[20];
   SAL_Int_T timer_interval_1 = 500;
   SAL_Int_T timer_interval_2 = 1000;

   SAL_Create_Queue(20, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }
   SAL_Sleep(1);

   t[0] = SAL_Clock();

   SAL_Start_Timer(timer_id, timer_interval_1, 1);

   while (i < 10)
   {
      msg = Wait_Message();
      i++;

      t[i] = SAL_Clock();
      Tr_Info_Mid_2("%d %d", (int)(i), (int)msg->event_id);
   }

   SAL_Start_Timer(timer_id, timer_interval_2, 1);

   while (i < 20)
   {
      msg = Wait_Message();
      i++;
      t[i] = SAL_Clock();

      Tr_Info_Mid_2("%d %d", (int)(i+1), (int)msg->event_id);
   }

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   for (i = 0; i < 9; i++)
   {
      dt[i] = (SAL_Int_T) (t[i + 1] - t[0]) / (i + 1);

      if (dt[i] < timer_interval_1)
      {
         Failed = true;
         Tr_Fault_3("Timer interval %d was %d mS; should have been %d", i, (int)dt[i], (int)timer_interval_1);
      }

      if (dt[i] > timer_interval_1 + timer_interval_1 / WARN_OVER_DIVISOR)
      {
         Tr_Fault_2("SAL_Start_Timer: Real timer interval (%d mS) is different from interval given as a parameter (%d mS)",
            (int)dt[i], (int)timer_interval_1);
      }

      if (dt[i] > timer_interval_1 + timer_interval_1 / MAX_OVER_DIVISOR)
      {
         Failed = true;
         Tr_Fault_2("SAL_Start_Timer: Real timer interval (%d mS) is different from interval given as a parameter (%d mS)",
            (int)dt[i], (int)timer_interval_1);
      }
   }

   for (i = 10; i < 19; i++)
   {
      dt[i] = (SAL_Int_T) (t[i + 1] - t[10]) / (i - 9);

      if (dt[i] < timer_interval_2)
      {
         Failed = true;
         Tr_Fault_2("Timer interval was %d mS; should have been %d", (int)dt[i], (int)timer_interval_2);
      }

      if (dt[i] > timer_interval_2 + timer_interval_2 / WARN_OVER_DIVISOR)
      {
         Tr_Fault_2("SAL_Start_Timer: Real timer interval (%d mS) is different from interval given as a parameter (%d mS)",
            (int)dt[i], (int)timer_interval_2);
      }

      if (dt[i] > timer_interval_2 + timer_interval_2 / MAX_OVER_DIVISOR)
      {
         PBC_Failed_2("SAL_Start_Timer: Real timer interval (%d mS) is different from interval given as a parameter (%d mS)",
            (int)dt[i], (int)timer_interval_2);
      }
   }
}

/**  Test of changing timer interval.
 */

SAL_Int_T Change_Tick_Timer_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Change_Tick_Timer_1, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;

}

/**  \}
 */

/**  \name Functions of Change_Tick_Timer_Test_2
 *  
 *   \{
 */

/**  Function of TIMER_THREAD.
 *
 *   - creates and starts single timer with period 1 sec
 *   - 0,5 sec after starting starts the same timer with period 2 sec
 *   - the timer event should appear after 2,5 sec after starting timer the first time
 *   - at the end timer is stopped and destroyed
 */

void Change_Tick_Timer_2(void* param)
{
   const SAL_Message_T* msg;
   SAL_Timer_Id_T timer_id;
   SAL_Clock_T t1, t2;
   SAL_Int_T dt;
   SAL_Int_T timer_interval_1 = 1000;
   SAL_Int_T timer_interval_2 = 2000;
   SAL_Int_T time_interval = 2500;

   SAL_Create_Queue(1, 0, malloc, free);

   if (!SAL_Create_Timer(TIMER1, &timer_id))
   {
      PBC_Failed("SAL_Create_Timer: Timer has not been created");
   }

   SAL_Start_Timer(timer_id, timer_interval_1, 0);

   t1 = SAL_Clock();

   SAL_Sleep(500);

   SAL_Start_Timer(timer_id, timer_interval_2, 0);

   msg = Wait_Message();

   t2 = SAL_Clock();

   Tr_Info_Mid_1("%d", (int)msg->event_id);

   SAL_Stop_Timer(timer_id);

   SAL_Destroy_Timer(timer_id);

   dt = (SAL_Int_T) (t2 - t1);

   if (dt < time_interval)
   {
      Failed = true;
      Tr_Fault_1("SAL_Start_Timer: Time interval is below 2.5 sec: %d", (int)dt);
   }

   if (dt > time_interval + time_interval / WARN_OVER_DIVISOR)
   {
      Tr_Fault_1("SAL_Start_Timer: Time interval is too far over 2.5 sec: %d", (int)dt);
   }

   if (dt > time_interval + time_interval / MAX_OVER_DIVISOR)
   {
      Failed = true;
      Tr_Fault_1("SAL_Start_Timer: Time interval is too far over 2.5 sec: %d", (int)dt);
   }
}

/**  Test of changing timer interval.
 */

SAL_Int_T Change_Tick_Timer_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Change_Tick_Timer_2, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list,
         sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;

}

/** \}
 */

/** \name Other Functions
 *
 *  \{
 */

/**  Function of MAIN_THREAD for PeriodicStartTimerThread, SingleStartTimerTest.
 *
 *   - sends EV_STOP event to the TIMER_THREAD
 */

void Send_Thread_1(void* param)
{
   static SAL_Thread_Id_T wait_list[] =
   { TIMER_THREAD };

   Tr_Info_Lo("PeriodStartThread: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list,
         sizeof(wait_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif 

   SAL_Sleep(300);

   if (SAL_Is_Thread_Alive(TIMER_THREAD))
   {
      SAL_Send(APP_ID, TIMER_THREAD, EV_STOP, NULL, 0);
   }
   Tr_Info_Lo("PeriodStartThread: Stop");
}

/**  Function of MAIN_THREAD for Periodic_Stop_Timer_Test, SingleStopTimerTest2, Single_Stop_Timer_Test.
 *
 *   - sends EV_STOP event to TIMER_THREAD
 */

void Send_Thread_2(void* param)
{
   static SAL_Thread_Id_T wait_list[] =
   { TIMER_THREAD };

   Tr_Info_Lo("PeriodStopThread: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list,
         sizeof(wait_list)/sizeof(SAL_Thread_Id_T),
         Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   SAL_Send(APP_ID, TIMER_THREAD, EV_STOP, NULL, 0);

   Tr_Info_Lo("PeriodStopThread: Stop");
}

bool_t Single_Shot_Ex_Test(void)
{
   bool_t tm_ready[3] =
   { false, false, false };
   SAL_Int_T param = 0x5555;
   SAL_Timer_Id_T timer1, timer2, timer3;

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      Tr_Fault("Single_Shot_Ex_Test/SAL_Create_Timer(EV_1) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      Tr_Fault("Single_Shot_Ex_Test/SAL_Create_Timer(EV_2) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      Tr_Fault("Single_Shot_Ex_Test/SAL_Create_Timer(EV_3) err");
      return false;
   }

   SAL_Start_Timer_Ex(timer1, 100, 0, param);
   SAL_Start_Timer_Ex(timer2, 100, 0, (uintptr_t) &param);
   SAL_Start_Timer_Ex(timer3, 100, 0, (uintptr_t) NULL);

   do
   {
      const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
      if (msg == NULL)
      {
         Tr_Fault("Timeout has been reached");
         SAL_Exit(1);
      }
      switch ((int) msg->event_id)
      {
         case EV_1:
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) 0x5555))
            {
               Tr_Fault("Single_Shot_Ex_Test/EV_1: Invalid parameter");
               return false;
            }
            tm_ready[0] = true;
            break;

         case EV_2:
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) &param))
            {
               Tr_Fault("Single_Shot_Ex_Test/EV_2: Invalid parameter");
               return false;
            }
            tm_ready[1] = true;
            break;

         case EV_3:
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) NULL))
            {
               Tr_Fault("Single_Shot_Ex_Test/EV_3: Invalid parameter");
               return false;
            }
            tm_ready[2] = true;
            break;
         default:
            break;
      }
   } while (!tm_ready[0] || !tm_ready[1] || !tm_ready[2]);

   SAL_Stop_Timer(timer1);
   SAL_Destroy_Timer(timer1);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);

   Tr_Info_Mid("Single_Shot_Ex_Test passed");
   return true;
}

bool_t Single_Shot_Restart_Ex_Test(void)
{
   bool_t tm_ready[3] =
   { false, false, false };
   SAL_Int_T param1 = 0xFFFF, param2 = 0x0000;
   SAL_Timer_Id_T timer1, timer2, timer3;

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      Tr_Fault("Single_Shot_Restart_Ex_Test/SAL_Create_Timer(EV_1) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      Tr_Fault("Single_Shot_Restart_Ex_Test/SAL_Create_Timer(EV_2) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      Tr_Fault("Single_Shot_Restart_Ex_Test/SAL_Create_Timer(EV_3) err");
      return false;
   }

   SAL_Start_Timer_Ex(timer1, 500, false, param1);
   SAL_Start_Timer_Ex(timer2, 1000, false, (uintptr_t) &param1);
   SAL_Start_Timer_Ex(timer3, 2000, false, (uintptr_t) NULL);

   do
   {
      const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
      if (msg == NULL)
      {
         Tr_Fault("Timeout has been reached");
         SAL_Exit(1);
      }
      switch ((int) msg->event_id)
      {
         case EV_1:
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) 0xFFFF))
            {
               Tr_Fault("Single_Shot_Restart_Ex_Test/EV_1: Invalid parameter");
               return false;
            }
            SAL_Start_Timer_Ex(timer2, 500, false, param2);
            SAL_Start_Timer_Ex(timer3, 500, false, (uintptr_t) &param2);
            tm_ready[0] = true;
            break;

         case EV_2:
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) 0x0000))
            {
               Tr_Fault("Single_Shot_Restart_Ex_Test/EV_2: Invalid parameter");
               return false;
            }
            tm_ready[1] = true;
            break;

         case EV_3:
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t))
               || (*((uintptr_t*) (msg->data)) != (uintptr_t) &param2))
            {
               Tr_Fault("Single_Shot_Restart_Ex_Test/EV_3: Invalid parameter");
               return false;
            }
            tm_ready[2] = true;
            break;
         default:
            break;
      }
   } while (!tm_ready[0] || !tm_ready[1] || !tm_ready[2]);

   SAL_Destroy_Timer(timer1);
   SAL_Stop_Timer(timer2);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);

   Tr_Info_Mid("Single_Shot_Restart_Ex_Test passed");
   return true;
}

bool_t Single_Shot_Stop_Ex_Test(void)
{
   bool_t tm_ready[3] =
   { false, false, false };
   SAL_Timer_Id_T timer1, timer2, timer3;

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      Tr_Fault("Single_Shot_Stop_Ex_Test/SAL_Create_Timer(EV_1) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      Tr_Fault("Single_Shot_Stop_Ex_Test/SAL_Create_Timer(EV_2) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      Tr_Fault("Single_Shot_Stop_Ex_Test/SAL_Create_Timer(EV_3) err");
      return false;
   }

   SAL_Start_Timer(timer1, 100, false);
   SAL_Start_Timer_Ex(timer2, 200, false, 0);
   SAL_Start_Timer(timer3, 500, false);

   do
   {
      const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
      if (msg == NULL)
      {
         Tr_Fault("Timeout has been reached");
         SAL_Exit(1);
      }
      switch ((int) msg->event_id)
      {
         case EV_1:
            SAL_Stop_Timer(timer2);
            tm_ready[0] = true;
            break;

         case EV_2:
            Tr_Fault("EV_2 received!!!!");
            return false;
            break;

         case EV_3:
            tm_ready[2] = true;
            break;
         default:
            break;
      }
   } while (!tm_ready[0] || !tm_ready[2]);

   SAL_Destroy_Timer(timer1);
   SAL_Stop_Timer(timer2);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);

   Tr_Info_Mid("Single_Shot_Stop_Ex_Test passed");
   return true;
}

bool_t Periodic_Ex_Test(void)
{
   uint32_t tm_ticks[3] =
   { 0, 0, 0 };
   SAL_Int_T param = 0x5555;
   SAL_Timer_Id_T timer1, timer2, timer3;

   if (!SAL_Create_Timer(EV_1, &timer1))
   {
      Tr_Fault("Periodic_Ex_Test/SAL_Create_Timer(EV_1) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_2, &timer2))
   {
      Tr_Fault("Periodic_Ex_Test/SAL_Create_Timer(EV_2) err");
      return false;
   }
   if (!SAL_Create_Timer(EV_3, &timer3))
   {
      Tr_Fault("Periodic_Ex_Test/SAL_Create_Timer(EV_3) err");
      return false;
   }

   SAL_Start_Timer_Ex(timer1, 250, true, param);
   SAL_Start_Timer_Ex(timer2, 500, true, (uintptr_t) &param);
   SAL_Start_Timer_Ex(timer3, 2100, false, (uintptr_t) NULL);

   do
   {
      const SAL_Message_T* msg = SAL_Receive_Timeout(Timeout);
      if (msg == NULL)
      {
         Tr_Fault("Timeout has been reached");
         SAL_Exit(1);
      }
      switch ((int) msg->event_id)
      {
         case EV_1:
            tm_ticks[0]++;
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) 0x5555))
            {
               Tr_Fault("Periodic_Ex_Test/EV_1: Invalid parameter");
               return false;
            }
            break;

         case EV_2:
            tm_ticks[1]++;
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) &param))
            {
               Tr_Fault("Periodic_Ex_Test/EV_2: Invalid parameter");
               return false;
            }
            break;

         case EV_3:
            tm_ticks[2]++;
            if ((msg->data == NULL) || (msg->data_size != sizeof(uintptr_t)) || (*((uintptr_t*) (msg->data)) != (uintptr_t) NULL))
            {
               Tr_Fault("Periodic_Ex_Test/EV_3: Invalid parameter");
               return false;
            }
            break;
         default:
            break;
      }
   } while (tm_ticks[2] < 1);

   if ((tm_ticks[0] != 8) || (tm_ticks[1] != 4))
   {
      Tr_Fault_2("Periodic_Ex_Test/ wrong ticks number: %d/10, %d/5", tm_ticks[0], tm_ticks[1]);
      return false;
   }

   SAL_Destroy_Timer(timer1);
   SAL_Destroy_Timer(timer2);
   SAL_Destroy_Timer(timer3);

   Tr_Info_Mid("Periodic_Ex_Test passed");
   return true;
}

/**  Test of changing timer interval.
 */

static void Ex_Tests(void * param)
{
   Failed = Failed || !SAL_Create_Queue(10, 10, SAL_Alloc, SAL_Free);

   Failed = Failed || !Single_Shot_Ex_Test();
   Failed = Failed || !Single_Shot_Restart_Ex_Test();
   Failed = Failed || !Single_Shot_Stop_Ex_Test();
   Failed = Failed || !Periodic_Ex_Test();
}

SAL_Int_T Timer_Param_Test(void)
{
   static SAL_Thread_Id_T destroyed_list[] =
   { TIMER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Ex_Tests, NULL, Init_Thread_Attr("TIME_T", TIMER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), 10000))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;

}

SAL_Int_T Set_Up()
{
   SAL_Config_T config;

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&config))
      return 1;

   return 0;
}

void Tear_Down()
{
}

int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up())
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   ret = ret || Run_Short_Timer_Test();
   ret = ret || Periodic_Start_Timer_Test();
   ret = ret || Periodic_Stop_Timer_Test();
   ret = ret || Single_Start_Timer_Test_1();
   ret = ret || Single_Start_Timer_Test_2();
   ret = ret || Tick_Timer_Test_1();
   ret = ret || Tick_Timer_Test_2();
   ret = ret || Tick_Timer_Test_3();
   ret = ret || Change_Tick_Timer_Test_1();
   ret = ret || Change_Tick_Timer_Test_2();
   ret = ret || Timer_Param_Test();

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
 * - 26-Sep-2013 Larry Piekarski Rev 19
 *   - Task 1334: Fixed compiler warnings
 * -----------------------------------------------------------------------*/
