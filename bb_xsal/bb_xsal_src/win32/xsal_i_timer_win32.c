/* -------------------------------------------------------------------------
 *
 * File:        xsal_timer_win32.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the internal XSAL Timers functions.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"

#if XSAL_TIMER_IMPL == XSAL_TIMER_USE_WIN32_API

#include "xsal_i_timer.h"
#include "xsal_i_assert.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"
#include "pbc_trace.h"

#include <windows.h>
#include <sys/timeb.h>

EM_FILENUM(XSAL_TIME_MODULE_ID, 4); /* Identify file for trace/assert purposes */

/* --------------------------
 *  Local Type Declarations
 * --------------------------*/

/** Declaration of events sent from User Thread (SAL thread)
 *  to Timer Thread (Win32 thread)
 */
typedef enum Timer_Control_Event_Tag
{
   EV_DESTROY_TIMER = WM_USER,
   EV_START_TIMER,
   EV_STOP_TIMER,
   EV_TERMINATE_THREAD
}Timer_Control_Event_T;

/** Start Timer data
 */
typedef struct Start_Time_Data_Tag
{
   /** Timer interval in miliseconds
    */
   uint32_t interval_msec;

   /** Define if the timer is periodic or single shot
    */
   bool_t is_periodic;

   bool_t use_param;
   uintptr_t param;

}Start_Time_Data_T;

/* ----------------------------
 *  Local Function Prototypes
 * ----------------------------*/

/** Function SAL_Wait_Semaphore is not defined in XSAL_LIGHT 
 *  external interface but is used internally. 
 */
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);

/** Main function of the Timer Thread
 */
static LRESULT Timer_Thread(void* param);

/** "Class function" of the Timer Window
 */
static LRESULT CALLBACK TimerClassFnc(
   HWND hWnd,
   UINT message,
   WPARAM wParam,
   LPARAM lParam);

/* ---------------------------
 *  Local Object Definitions
 * ---------------------------*/

/** Semaphore for synchronization startup 
 *  and shutdown of Timer Thread
 */
static SAL_Semaphore_T Timer_Sem;

/** Handle to the window which receives WIN32 timer events
 */
static HWND Timer_Wnd;

#if defined (XSAL)
/** Handle to the timer thread
 */
static HANDLE Timer_Thread_Handle;
#endif /* XSAL */

/** Class name of the "Timer Window"
 */
static const char_t* Timer_Wnd_Class_Name = "XSALTimer";

/* -----------------------------
 *  Local Function Definitions
 * -----------------------------*/

/** Main loop of the Timer Thread.
 *  Function receives and processes messages from WIN2 timers.
 */
static LRESULT Timer_Thread(void* param)
{
   MSG msg;

   /** Assign SAL_ROUTER_THREAD_ID to the timer thread.
    *  Timer thread has to have valid Thread ID by reason
    *  of Debug Trace - it prints debug trace messages.
    */
   if (!SAL_I_TLS_Set_Specific(SAL_I_Thread_Id_Self, &SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID]))
   {
      PBC_Failed("SAL_I_Init_Timer_Module: couldn't assign Current_SAL_Thread_Id");
   }

   Timer_Wnd = CreateWindowEx(0, Timer_Wnd_Class_Name, "", 0, 0, 0, 0, 0, 0, 0, 0, 0);
   if (Timer_Wnd == NULL)
   {
      PBC_Failed_1("Start_User_Thread/CreateWindowEx error=%d", (int) GetLastError());
   }

   /** The side efect of PeekMessage function call it to create message queue
    */
   (void)PeekMessage(&msg, Timer_Wnd, WM_USER, WM_USER, PM_NOREMOVE);

   /** Signal that Timer Thread has been initialized
    */
   (void)SAL_Signal_Semaphore(&Timer_Sem);

#if defined (XSAL_LIGHT)
   (void)SAL_Signal_Ready();
#endif /* XSAL_LIGHT */

   /** Enter message loop and process messages from WIN32 timer
    */
   while (GetMessage(&msg, NULL, 0, 0))
   {
      DispatchMessage(&msg);
   }

   if (!DestroyWindow(Timer_Wnd))
   {
      PBC_Failed_1("Start_User_Thread/DestroyWindow error=%d", (int) GetLastError());
   }

   /** Signal that Timer Thread has been terminated
    */
   (void)SAL_Signal_Semaphore(&Timer_Sem);

   return msg.wParam;
}

static LRESULT CALLBACK TimerClassFnc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch(message)
   {
      case WM_TIMER:
      if ((SAL_I_Timer_T*)wParam != NULL)
      {
         /** wParam points to SAL Timer
          */
         SAL_Event_Id_T event_id = ((SAL_I_Timer_T*)wParam)->event_id;
         SAL_Thread_Id_T thread_id = ((SAL_I_Timer_T*)wParam)->thread_id;
         bool_t use_param = ((SAL_I_Timer_T*)wParam)->use_param;
         uintptr_t param = ((SAL_I_Timer_T*)wParam)->param;

         /** If timer is not periodic, destroy it
          */
         if (!((SAL_I_Timer_T*)wParam)->is_periodic)
         {
            (void)KillTimer(Timer_Wnd, wParam);
         }
         SAL_I_Post_Timer_Event(event_id, thread_id, use_param, param);
      }
      return 0;

      case EV_DESTROY_TIMER:
      /** wParam identifies SAL Timer ID
       */
      (void)KillTimer(Timer_Wnd, wParam);
      return 0;

      case EV_START_TIMER:
      /** wParam is a pointer to SAL Timer to be started. 
       *  lParam is a pointer to timeout value.
       */
      ((SAL_I_Timer_T*)wParam)->is_periodic = ((Start_Time_Data_T*)lParam)->is_periodic;
      ((SAL_I_Timer_T*)wParam)->use_param = ((Start_Time_Data_T*)lParam)->use_param;
      ((SAL_I_Timer_T*)wParam)->param = ((Start_Time_Data_T*)lParam)->param;
      (void)SetTimer(Timer_Wnd, wParam, ((Start_Time_Data_T*)lParam)->interval_msec + 1, 0);
      return 0;

      case EV_STOP_TIMER:
      /** wParam is a pointer to SAL Timer to be stopped.
       */
      (void)KillTimer(Timer_Wnd, wParam);
      return 0;

      case EV_TERMINATE_THREAD:
      PostQuitMessage(wParam);
      break;

      default:
      break;
   }
   return DefWindowProc(hWnd, message, wParam, lParam);
}

/* -----------------------
 *  Function Definitions
 * -----------------------*/

bool_t SAL_I_Init_Timer_Module(void)
{
   bool_t status = false;

   if (SAL_I_Timers == NULL)
   {
      if (SAL_I_Max_Number_Of_Timers > 0)
      {
         if (SAL_Create_Semaphore(&Timer_Sem, NULL))
         {
            if (SAL_Create_Mutex(&SAL_I_Timers_Mutex, NULL))
            {
               WNDCLASSEX wndClass;
               memset(&wndClass, 0, sizeof(WNDCLASSEX));
               wndClass.cbSize = sizeof(WNDCLASSEX);
               wndClass.lpfnWndProc = TimerClassFnc;
               wndClass.lpszClassName = Timer_Wnd_Class_Name;
               if (RegisterClassEx(&wndClass) != 0)
               {
                  SAL_I_Timers = malloc(SAL_I_Max_Number_Of_Timers*sizeof(SAL_I_Timer_T));
                  if (SAL_I_Timers != NULL)
                  {
                     status = true;
                  }
                  else
                  {
                     Tr_Fault("SAL_I_Init_Timer_Module: memory allocation failed");
                  }
                  if (!status)
                  {
                     UnregisterClass(Timer_Wnd_Class_Name, NULL);
                  }
               }
               else
               {
                  Tr_Fault_1("SAL_I_Init_Timers_Module/RegisterClassEx error=%d", (int) GetLastError());
               }
               if (!status)
               {
                  (void)SAL_Destroy_Mutex(&SAL_I_Timers_Mutex);
               }
            }
            if (!status)
            {
               (void)SAL_Destroy_Semaphore(&Timer_Sem);
            }
         }
      }
      else
      {
         Tr_Info_Mid("SAL_I_Init_Timer_Module: max number of timers == 0");
         status = true;
      }
   }
   else
   {
      Tr_Info_Mid("SAL_I_Init_Timer_Module: Timer Module initialized second time without deinitialization");
   }
   PBC_Require(status, "SAL_I_Init_Timer_Module failed");

   return status;
}

void SAL_I_Deinit_Timer_Module(void)
{
   if (SAL_I_Timers != NULL)
   {
      free(SAL_I_Timers);
      SAL_I_Timers = NULL;
      (void)UnregisterClass(Timer_Wnd_Class_Name, NULL);
      (void)SAL_Destroy_Mutex(&SAL_I_Timers_Mutex);
      (void)SAL_Destroy_Semaphore(&Timer_Sem);
   }
}

#if defined (XSAL)

void SAL_I_Start_Timer_Module(void)
{
   if (SAL_I_Timers != NULL)
   {
      size_t tm_id;

      SAL_I_Timers_Free_List = &SAL_I_Timers[0];
      for(tm_id = 0; tm_id < SAL_I_Max_Number_Of_Timers-1; tm_id++)
      {
         SAL_I_Timers[tm_id].event_id = -1;
         SAL_I_Timers[tm_id].next_thread_timer = &SAL_I_Timers[tm_id + 1];
      }
      SAL_I_Timers[SAL_I_Max_Number_Of_Timers-1].event_id = -1;
      SAL_I_Timers[SAL_I_Max_Number_Of_Timers-1].next_thread_timer = NULL;

      Timer_Thread_Handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Timer_Thread, NULL, 0, NULL);
      if (Timer_Thread_Handle != NULL)
      {
         /** Wait until timer thread initialize itself
          */
         (void) SAL_Wait_Semaphore(&Timer_Sem);
      }
   }
}

void SAL_I_Stop_Timer_Module(void)
{
   if (Timer_Wnd != NULL)
   {
      (void)SendMessage(Timer_Wnd, EV_TERMINATE_THREAD, (WPARAM)0, (LPARAM)0);

      /** Wait until timer thread terminates
       */
      (void)SAL_Wait_Semaphore(&Timer_Sem);
   }
}

SAL_OS_TID_T SAL_I_Get_Timer_Thread_Id(void)
{
   return 0;
}

bool_t SAL_I_Get_Timer_Thread_Priority(
   SAL_Priority_T* priority,
   SAL_Sched_Policy_T* sched_policy)
{
   bool_t status;

   if (Timer_Thread_Handle != NULL)
   {
      *priority = GetThreadPriority(Timer_Thread_Handle);
      *sched_policy = 0;
      status = true;
   }
   else
   {
      Tr_Info_Mid("SAL_I_Get_Timer_Thread_Priority/Timer Thread not started");
      status = false;
   }
   return status;
}

#else /* XSAL_LIGHT */

void SAL_I_Start_Timer_Module(void)
{
   size_t tm_id;
   SAL_I_Timers_Free_List = &SAL_I_Timers[0];
   for(tm_id = 0; tm_id < SAL_I_Max_Number_Of_Timers-1; tm_id++)
   {
      SAL_I_Timers[tm_id].event_id = -1;
      SAL_I_Timers[tm_id].next_thread_timer = &SAL_I_Timers[tm_id + 1];
   }
   SAL_I_Timers[SAL_I_Max_Number_Of_Timers-1].event_id = -1;
   SAL_I_Timers[SAL_I_Max_Number_Of_Timers-1].next_thread_timer = NULL;
}

int32_t SAL_I_RT_Light(void)
{
   return (int32_t)Timer_Thread(NULL);
}

void SAL_I_Stop_RT_Light(int32_t status)
{
   (void)SendMessage(Timer_Wnd, EV_TERMINATE_THREAD, (WPARAM)status, (LPARAM)0);

   /** Wait until timer thread terminates
    */
   (void)SAL_Wait_Semaphore(&Timer_Sem);
}

void SAL_I_Stop_Timer_Module(void)
{
}

#endif /* XSAL && XSAL_LIGHT */

bool_t SAL_I_Create_Timer(SAL_I_Timer_T* timer)
{
   return true;
}

void SAL_I_Start_Timer(
   SAL_I_Timer_T* timer,
   uint32_t interval_msec, bool_t is_periodic,
   bool_t use_param, uintptr_t param)
{
   Start_Time_Data_T start_time_data =
   {  interval_msec, is_periodic, use_param, param};

   (void)SendMessage(Timer_Wnd, EV_START_TIMER, (WPARAM)timer, (LPARAM)&start_time_data);
}

void SAL_I_Stop_Timer(SAL_I_Timer_T* timer)
{
   (void)SendMessage(Timer_Wnd, EV_STOP_TIMER, (WPARAM)timer, (LPARAM)0);
}

void SAL_I_Destroy_Timer(SAL_I_Timer_T* timer)
{
   (void)SendMessage(Timer_Wnd, EV_DESTROY_TIMER, (WPARAM)timer, (LPARAM)0);
}

#endif /* XSAL_TIMER_IMPL == XSAL_TIMER_USE_WIN32_API */
/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 14-May-2015 Kirk Bailey
 * - Fixed Cygwin build warnings.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 23-Jul-2013 Larry Piekarski Rev 9
 * - Task 161627: Fixed build warning in XSAL_LIGHT for win32gcc target.
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
