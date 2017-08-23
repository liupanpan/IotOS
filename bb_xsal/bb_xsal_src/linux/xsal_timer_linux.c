/*===========================================================================*/
/**
 * @file xsal_timer_linux.c
 *
 * This file implements the XSAL timer functions
 *
 * %full_filespec: xsal_timer_linux.c~16:csrc:kok_basa#1 %
 * @version %version: 16 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Thu Jan 28 15:46:03 2016 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007,2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *    Implement XSAL timers
 *
 *    All available timers are allocated dynamically at initialization .
 *
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *         SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - HI7000/4 Series (HI7000/4 V.2.02, HI7700/4 V.2.02, and HI7750/4 V.2.02) User�s Manual
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *       - None
 */
/*===========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/
#include "double_linked_list.h"
#include "pbc_trace.h"
#include "xsal.h"
#include "xsal_cbk.h"
#include "xsal_i_thread.h"
#include "xsal_i_timer.h"
#include "xsal_timer_linux.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <stdlib.h>

/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/

#if !defined(TIMER_ACCESS_TIMEOUT)
/**
 * timeout (mS) used for access to timers
 */
#define TIMER_ACCESS_TIMEOUT (10000)
#endif

#if !defined(TIMER_NUM_ALLOCATED)
/**
 * Number of timers allocated from memory at a time
 */
#define TIMER_NUM_ALLOCATED   (20)
#endif

#if !defined(TIMER_NUM_INIT_ALLOCATED)
/**
 * Number of timers to initially allocate
 */
#define TIMER_NUM_INIT_ALLOCATED   (0)
#endif

#if !defined(SAL_I_TIMER_SIGNAL_ID)
/** If SAL Timer uses signals then user can define which signal is used.
 *  The user cannot use this signal for own purposes.
 *
 *  If this is not defined the the default value is SIGALRM
 */
#define SAL_I_TIMER_SIGNAL_ID SIGVTALRM
#endif /* !SAL_I_TIMER_SIGNAL_ID */

#if !defined(SAL_I_TIMER_SIGNAL_CLOCK_ID)
/** If SAL Timer uses signals or pulses user can choose OS Clock.
 *
 *  If this is not defined the the default value is CLOCK_REALTIME.
 */
#define SAL_I_TIMER_SIGNAL_CLOCK_ID CLOCK_REALTIME
#endif /* SAL_I_TIMER_SIGNAL_CLOCK_ID */

#if !defined(TIMER_THREAD_PRIORITY)
/**
 * Priority level for timer thread
 */
#define TIMER_THREAD_PRIORITY  (SAL_HIGH_PRIORITY - 1)
#endif

/*===========================================================================*\
 * Local Preprocessor #define MACROS
 \*===========================================================================*/

EM_FILENUM(XSAL_TIME_MODULE_ID, 3);
/* define this file for assert handling */

/*===========================================================================*\
 * Local Type Declarations
 \*===========================================================================*/

/**
 * Timer States
 */
typedef enum Time_State_Tag
{
   TIMER_UNUSED = 0x12345678, /**< Timer is not owned */
   TIMER_STOPPED = 0x3C3C3C3C, /**< Timer is owned, but not running */
   TIMER_RUNNING = 0x5A5A5A5A
/**< Timer is owned and active */
} Time_State_T;

/**
 * Data Record for an XSAL timer
 */
typedef struct XSAL_Timer_Tag
{
   DBLL_Entry_T le; /**< link list pointers */
   Time_State_T state; /**< state of timer (determines which list timer is in) */
   SAL_Thread_Id_T thread; /**< owner of timer */
   SAL_Event_Id_T event_id; /**< message id sent at timer expiration */
   struct timespec expiration_time; /**< absolute timer value when timer expires */
   uint32_t period; /**< reload value (mS) at timer expiration. 0 indicates a one-shot */
   bool use_param;
   uintptr_t param;
} XSAL_Timer_T;

typedef struct Timer_Block_Tag
{
   DBLL_Entry_T le; /**< link list pointers */
   XSAL_Timer_T * block; /* pointer to allocated block */
} Timer_Block_T;

/*===========================================================================*\
 * Exported Const Object Definitions
 \*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
 \*===========================================================================*/

/**
 *  Link list of allocated but unassigned timers
 */
static DBLL_List_T Unused_Timers;

/**
 *  Link list of stopped timers
 */
static DBLL_List_T Stopped_Timers;

/**
 *  Link list of running timers
 *    List is sorted by timeout order
 */
static DBLL_List_T Running_Timers;

/**
 *  Link list of allocated blocks
 */
static DBLL_List_T Timer_Blocks;

/**
 * Mutex to control access to timer arrays.
 * This single mutex controls access to all timers. It is locked on all thread
 * calls to the time module and when the timer thread updates the active.
 */
static SAL_Mutex_T Timer_Mutex;

/** Semaphore to synchronize Timer Thread startup and shutdown
 */
static SAL_Semaphore_T Timer_Sem;

/** Marker if Timer Thread is running
 */
static bool_t Is_Timer_Thread_Running = false;

/** Handler to the Timer Thread
 */
static pthread_t Timer_Thread_Id;

/** Handler to the OS timer
 */
static timer_t OS_Timer_Id = NULL;

/*===========================================================================*\
 * Local Function Prototypes
 \*===========================================================================*/

static void Allocate_Initial_Timers(int num_timers);
static int_least8_t Compare_Timeouts(void *arg, DBLL_Entry_T const *e1, DBLL_Entry_T const *e2);
static bool_t Destroy_Thread_Timer(void *arg, struct DBLL_List_T * pLL, DBLL_Entry_T * pEntry);
static void Lock_Timers(void);
static void Restart_Timer(void);
static void Timer_Add_Unused(XSAL_Timer_T * ptimer);
static void Timer_Add_Stop(XSAL_Timer_T * ptimer);
static void Timer_Add_Running(XSAL_Timer_T * ptimer, uint32_t delay_msec);
static void Timers_Allocate_Block(void);
static void Timer_Handler(void);
static void Timer_Remove(XSAL_Timer_T * ptimer);
static void* Timer_Thread(void* param);
static void Unlock_Timers(void);
static void Update_Expiration_Time(struct timespec* t1, uint32_t t2);
static void Timer_Sig_Handler(int sig);

/*===========================================================================*\
 * Local CONST Object Definitions
 \*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
 \*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

/*---------------------------------------------------------------------------*\
 * Local Functions
 \*---------------------------------------------------------------------------*/

static void Update_Expiration_Time(struct timespec* t1, uint32_t t2)
{
   t1->tv_sec += t2 / 1000;
   t1->tv_nsec += (t2 % 1000) * 1000000;
   if (t1->tv_nsec >= 1000000000)
   {
      t1->tv_nsec -= 1000000000;
      t1->tv_sec++;
   }
}

static void Restart_Timer(void)
{
   if(OS_Timer_Id != NULL)
   {
      XSAL_Timer_T * ptimer = (XSAL_Timer_T *) DBLL_Get_First(&Running_Timers);

      if (ptimer != NULL)
      {
         struct itimerspec itimer_spec;

         itimer_spec.it_value = ptimer->expiration_time;
         itimer_spec.it_interval.tv_sec = 0;
         itimer_spec.it_interval.tv_nsec = 0;

         if (timer_settime(OS_Timer_Id, TIMER_ABSTIME, &itimer_spec, NULL) != 0)
         {
            Tr_Fault_1("Timer_Thread/Restart_Timer: timer_settime failed. Error=%s", strerror(errno));
         }
      }
   }
}

/**
 * timer Handler  - called when tiemr signal is generated
 *
 *  Checks head of running timer list until head has not expired
 *    (note expired timers are move to either later in running list or
 *       to the stopped list)
 *     A message is send for each expired timer to owning thread
 */
static void Timer_Handler(void)
{
   bool_t expired;
   struct timespec now;
   XSAL_Timer_T * ptimer;

   do
   {
      expired = false;
      (void) clock_gettime(SAL_I_TIMER_SIGNAL_CLOCK_ID, &now);

      Lock_Timers();

      ptimer = (XSAL_Timer_T *) DBLL_Get_First(&Running_Timers);

      if (ptimer != NULL)
      {
         if ((ptimer->expiration_time.tv_sec < now.tv_sec)
            || ((ptimer->expiration_time.tv_sec == now.tv_sec)
               && (ptimer->expiration_time.tv_nsec <= now.tv_nsec)))
         {
            DBLL_Remove_Entry(&Running_Timers, &ptimer->le); /* remove from running list */

            if (0 != ptimer->period)
            {
               Timer_Add_Running(ptimer, ptimer->period); /* add back in with new time out */
            }
            else
            {
               Timer_Add_Stop(ptimer); /* add to stop list */
            }

            expired = true;
            /* Because expired is set to true, the timer mutex is unlocked, but the do-while loop
             * will loop again, which will relock the timer mutex. Once the code exits the do-
             * while look, the timer mutex is unlocked at the end of the function */
            Unlock_Timers();

            if (ptimer->use_param)
            {
               (void) SAL_Send(SAL_Get_App_Id(), ptimer->thread, ptimer->event_id, &(ptimer->param),
                  sizeof(ptimer->param));
            }
            else
            {
               (void) SAL_Send(SAL_Get_App_Id(), ptimer->thread, ptimer->event_id, NULL, 0);
            }
         }
      }
   } while (expired);

   Restart_Timer();

   Unlock_Timers();
}

/**
 * Default signal handler, should never run
 */
static void Timer_Sig_Handler(int sig)
{
   Tr_Fault("Timer Signal Handler called");
   Restart_Timer();
}

/**
 * Set up timer thread runtime properties
 *    priority
 *    thread name
 *    XSAL thread ID
 */
static void Configure_Thread_Props(void)
{
   char name[SAL_THREAD_NAME_MAX_LENGTH + 8];
   struct sched_param sched_p =
   { 0 };

   sched_p.sched_priority = TIMER_THREAD_PRIORITY - (SAL_REAL_TIME_PRIORITY - 1);
   (void) pthread_setschedparam(pthread_self(), SCHED_RR, &sched_p);

   /** Assign SAL_ROUTER_THREAD_ID to the timer thread.
    *  Timer thread has to have valid Thread ID by reason
    *  of Debug Trace - it prints out debug trace messages.
    */
   if (!SAL_I_TLS_Set_Specific(SAL_I_Thread_Id_Self, &SAL_I_Thread_Table[SAL_ROUTER_THREAD_ID]))
   {
      PBC_Failed("Timer_Thread: couldn't assign Current_SAL_Thread_Id");
   }

   /** Setting Linux thread name */
   snprintf(name, sizeof(name), "%02d.%d-Timer", (int) SAL_Get_App_Id(), (int) SAL_ROUTER_THREAD_ID);
   prctl(PR_SET_NAME, name);
}

/**
 * Thread that waits for timer signal
 */
static void* Timer_Thread(void* param)
{
   sigset_t lsigset;
   struct sigaction action;
   struct sigevent timer_ev_spec =
   { .sigev_value.sival_int = 0, .sigev_signo = SAL_I_TIMER_SIGNAL_ID, .sigev_notify = SIGEV_SIGNAL };

   Configure_Thread_Props();

   Tr_Info_Mid("Timer Thread Starting");

   /* create POSIX timer */
   if (timer_create(SAL_I_TIMER_SIGNAL_CLOCK_ID, &timer_ev_spec, &OS_Timer_Id) == EOK)
   {
      /* Set up action handler */
      memset(&action, 0, sizeof(action));
      action.sa_handler = Timer_Sig_Handler;
      sigemptyset(&action.sa_mask);
      action.sa_flags = SA_RESTART;

      if (sigaction(SAL_I_TIMER_SIGNAL_ID, &action, NULL) < 0)
      {
         Tr_Fault_1("Unable to install timer signal handler %s", strerror(errno));
      }

      /* Although we just installed a signal handler, keep timer signal blocked
       * and rely on sigwait */
      (void) sigemptyset(&lsigset);
      (void) sigaddset(&lsigset, SAL_I_TIMER_SIGNAL_ID);
      pthread_sigmask(SIG_BLOCK, &lsigset, NULL);

      Is_Timer_Thread_Running = true;

      /** Signal SAL_I_Start_Timer_Thread that timer is ready */
      (void) SAL_Signal_Semaphore(&Timer_Sem);

      while (Is_Timer_Thread_Running)
      {
         SAL_Int_T sig;

         /* Wait for timer signal */
         if ((sigwait(&lsigset, &sig) != 0) || (SAL_I_TIMER_SIGNAL_ID != sig))
         {
            Tr_Fault("Invalid sigwait");
         }

         /* Update timers regardless of sigwait status */
         Timer_Handler();
      }

      /* delete timer */
      Lock_Timers();
      timer_delete(OS_Timer_Id);
      OS_Timer_Id = NULL;
      DBLL_Destroy(&Timer_Blocks);
      DBLL_Destroy(&Running_Timers);
      DBLL_Destroy(&Stopped_Timers);
      DBLL_Destroy(&Unused_Timers);

      Unlock_Timers();
   }
   else
   {
      Tr_Fault_1("Failed to create OS timer: %s", strerror(errno));
   }

   /** Signal SAL_I_Stop_Timer_Thread that timer thread is finished */
   (void) SAL_Signal_Semaphore(&Timer_Sem);
   Tr_Info_Mid("Timer Thread terminated");

   return NULL;
}

/**
 * Destroys timer if owned by passed thread_id
 *
 * @param arg thread ID
 * @param pLL pointer to linked list (running or stopped timers)
 * @param pEntry pointer to timer in list
 */
static bool_t Destroy_Thread_Timer(void *arg, struct DBLL_List_T * pLL, DBLL_Entry_T * pEntry)
{
   XSAL_Timer_T * ptimer = (XSAL_Timer_T *) pEntry;
   SAL_Thread_Id_T ID = (SAL_Thread_Id_T) (intptr_t) arg;

   if (ptimer->thread == ID)
   {
      DBLL_Remove_Entry(pLL, &ptimer->le);
      Timer_Add_Unused(ptimer);
   }
   return true;
}

/**
 * Removes a timer from either running or stop list based on it's state
 *
 * @param ptimer pointer to timer
 *
 * @pre Timer must be in running or stopped state
 */
static void Timer_Remove(XSAL_Timer_T * ptimer)
{
   if (TIMER_RUNNING == ptimer->state)
   {
      DBLL_Remove_Entry(&Running_Timers, &ptimer->le);
   }
   else if (TIMER_STOPPED == ptimer->state)
   {
      DBLL_Remove_Entry(&Stopped_Timers, &ptimer->le);
   }
   else
   {
      PBC_Failed_1("Illegal timer state: %d", (int)ptimer->state);
   }
}

/**
 * Add timer to unused list
 *
 * @param ptimer  pointer to timer to mark as unused
 *
 * @pre Timer must have been removed from previous list
 */
static void Timer_Add_Unused(XSAL_Timer_T * ptimer)
{
   ptimer->state = TIMER_UNUSED;
   DBLL_Add_Entry(&Unused_Timers, &ptimer->le);
}

/**
 *  Add timer to stopped list
 *
 * @param ptimer  pointer to timer to stop
 *
 * @pre Timer must have been removed from previous list
 */
static void Timer_Add_Stop(XSAL_Timer_T * ptimer)
{
   ptimer->state = TIMER_STOPPED;
   DBLL_Add_Entry(&Stopped_Timers, &ptimer->le);
}

/**
 *  Add timer to running list
 *
 * @param ptimer  pointer to timer start running
 * @param delay_msec delay from last expiration or set
 *
 * @pre Timer must have been removed from previous list
 */
static void Timer_Add_Running(XSAL_Timer_T * ptimer, uint32_t delay_msec)
{
   ptimer->state = TIMER_RUNNING;
   Update_Expiration_Time(&ptimer->expiration_time, delay_msec);
   DBLL_Add_Entry(&Running_Timers, &ptimer->le);
}

/**
 * Allocate memory for a block of timers
 *    Add new timers to the unused list
 */
static void Timers_Allocate_Block(void)
{
   Timer_Block_T * pBlock;

   uint32_t i;

   Unlock_Timers();
   pBlock = malloc(sizeof(Timer_Block_T));

   if (pBlock)
   {
      pBlock->block = malloc(sizeof(XSAL_Timer_T) * TIMER_NUM_ALLOCATED);
   }
   Lock_Timers();

   if ((pBlock) && (pBlock->block))
   {
      XSAL_Timer_T * ptimers = pBlock->block;

      DBLL_Add_Entry(&Timer_Blocks, (DBLL_Entry_T *) pBlock);

      for (i = 0; i < TIMER_NUM_ALLOCATED; i++)
      {
         Timer_Add_Unused(ptimers++);
      }
   }
   else
   {
      Tr_Fault("Failed to allocate memory for timers");
      free(pBlock);
   }
}

static bool_t Free_Timers(void *arg, struct DBLL_List_T * pLL, DBLL_Entry_T * pEntry)
{
   Timer_Block_T * pBlock = (Timer_Block_T *) pEntry;

   if (pBlock)
   {
      SAL_Free(pBlock->block); /* free timer block */
      SAL_Free(pBlock); /* free double link list entry */
   }
   return true;
}

/**
 * Allocate set of initial timers at module startup
 */
static void Allocate_Initial_Timers(int num_timers)
{
   int i;
   /** This structure defines the callbacks used for the running timers to keep timers in expiration order */
   static const DBLL_Callback_T running_callbacks =
   { NULL, /* arg     */
   NULL, /* calloc  */
   Compare_Timeouts, /* compare */
   NULL /* free    */
   };
   /** This structure defines the callbacks used for the Timer Blocks */
   static const DBLL_Callback_T blocks_callbacks =
      { NULL /* arg     */
      , NULL /* calloc  */
      , NULL /* compare */
      , Free_Timers /* free    */
      };
   /* initialize all the timer lists */
   DBLL_Init(&Unused_Timers, NULL, 0);
   DBLL_Init(&Stopped_Timers, NULL, 0);
   DBLL_Init(&Running_Timers, &running_callbacks, 0);
   DBLL_Init(&Timer_Blocks, &blocks_callbacks, 0);

   Lock_Timers();
   /** do initial allocation of timers */
   for (i = 0; i < ((num_timers + TIMER_NUM_ALLOCATED - 1) / TIMER_NUM_ALLOCATED); i++)
   {
      Timers_Allocate_Block();
   }
   Unlock_Timers();
}

/**
 * Callback function that compares two XSAL_Timer_T entries for sorting into
 * ascending order based on time to expire.
 *
 * @return
 *   <0 if e1<e2, 0 if e1==e2, >0 if e1>e2.
 *
 * @param arg  Not used.
 * @param e1 The XSAL_Timer_T on the left side of the comparison.
 * @param e2 The XSAL_Timer_T on the right side of the comparison.
 */
static int_least8_t Compare_Timeouts(void *arg, DBLL_Entry_T const *e1, DBLL_Entry_T const *e2)
{
   XSAL_Timer_T const *ptimer1 = (XSAL_Timer_T const *) e1;
   XSAL_Timer_T const *ptimer2 = (XSAL_Timer_T const *) e2;
   int_least8_t results = 0;

   (void) arg; /* Prevent unused parameter warning */

   if (ptimer1->expiration_time.tv_sec < ptimer2->expiration_time.tv_sec)
   {
      results = -1;
   }
   else if (ptimer1->expiration_time.tv_sec > ptimer2->expiration_time.tv_sec)
   {
      results = 1;
   }

   if (0 == results) /* seconds equal, then compare nS */
   {
      if (ptimer1->expiration_time.tv_nsec < ptimer2->expiration_time.tv_nsec)
      {
         results = -1;
      }
      else if (ptimer1->expiration_time.tv_nsec > ptimer2->expiration_time.tv_nsec)
      {
         results = 1;
      }
   }

   return results;
}

/**
 * Locks Timers for access by only one threads (including the timer thread)
 *    Used whenever one of the timer list is updated
 *
 * @note mutex should never timeout
 */
static void Lock_Timers(void)
{
   if (!SAL_Lock_Mutex_Timeout(&Timer_Mutex, TIMER_ACCESS_TIMEOUT))
   {
      PBC_Failed("Unable to lock timer mutex");
   }
}

/**
 * Frees Timers to be accessed by other threads
 */
static void Unlock_Timers(void)
{
   if (!SAL_Unlock_Mutex(&Timer_Mutex))
   {
      PBC_Failed("Unable to unlock timer mutex");
   }
}

/*---------------------------------------------------------------------------*\
 * Global API Functions
 \*---------------------------------------------------------------------------*/

/*
 *  Creates a timer sending an event upon expiration.
 *
 *    Locks access to timer lists
 *    Gets timer from unused list
 *    if unused list is empty then
 *       Allocate a new block of timers and use one from that
 *    Assign it to the thread and set the event id
 *    Add to stopped list
 *    unlock access to timer array
 */
bool_t SAL_Create_Timer(SAL_Event_Id_T event_id, SAL_Timer_Id_T * timer_id)
{
   XSAL_Timer_T *ptimer = NULL;
   if(OS_Timer_Id != NULL)
   {
      Lock_Timers();

      ptimer = (XSAL_Timer_T *) DBLL_Get_First(&Unused_Timers);

      if (NULL == ptimer)
      {
         /* allocate block of unused timers */
         Timers_Allocate_Block();

         /* retry get of unused timer */
         ptimer = (XSAL_Timer_T *) DBLL_Get_First(&Unused_Timers);
         PBC_Ensure(NULL != ptimer, "NULL timer pointer");
      }

      DBLL_Remove_Entry(&Unused_Timers, &ptimer->le); /* remove timer from unused */

      /* update timer parameters */
      ptimer->thread = SAL_Get_Thread_Id();
      ptimer->event_id = event_id;

      Timer_Add_Stop(ptimer); /* add timer to stopped list */

      Unlock_Timers();

      *timer_id = (SAL_Timer_Id_T) ptimer;
   }
   return (NULL != ptimer);
}

/*
 *  Destroys the timer identified by timer_id.
 *
 *  Lock access to timer lists
 *  Remove from timer from it's current list
 *  Add back to list of unused timers
 *  Free access to timer lists
 */
void SAL_Destroy_Timer(SAL_Timer_Id_T timer_id)
{
   XSAL_Timer_T *ptimer = (XSAL_Timer_T *) timer_id;
   if(OS_Timer_Id != NULL)
   {
      if (ptimer != NULL)
      {
         Lock_Timers();

         Timer_Remove(ptimer); /* Move timer to unused list */
         Timer_Add_Unused(ptimer);

         Unlock_Timers();
      }
      else
      {
         Tr_Warn("Trying to Destroy NULL timer");
      }
   }
}

/*
 *  Starts the timer identified by timer_id.
 *
 * set timer interval
 * set initial current_value down value
 * if the requested delay is 0, then immediately send the timer expired message
 *    Note, setting the current value to 0 stops the timer
 *  A periodic interval of zero is not allowed.
 */
static void Start_Timer(SAL_Timer_Id_T timer_id, uint32_t interval_msec, bool_t is_periodic, bool_t use_param,
   uintptr_t param)
{
   XSAL_Timer_T *ptimer = (XSAL_Timer_T *) timer_id;

   PBC_Require(NULL != ptimer, "NULL timer pointer");
   PBC_Require_1(((TIMER_RUNNING == ptimer->state) || (TIMER_STOPPED == ptimer->state)),
      "Illegal timer state: %d", (int)ptimer->state);
   PBC_Require_1(interval_msec < 0x80000000, "Too long of a timer interval: %#x", (unsigned)interval_msec);
   PBC_Require_1((!is_periodic) || (interval_msec > 0), "Illegal periodic timer interval: %d",
      (int)interval_msec);
   if(OS_Timer_Id != NULL)
   {
      Lock_Timers();

      Timer_Remove(ptimer);

      ptimer->period = (is_periodic) ? interval_msec : 0;
      ptimer->use_param = use_param;
      ptimer->param = param;

      /* set initial time to stgart of next mS */
      clock_gettime(SAL_I_TIMER_SIGNAL_CLOCK_ID, &ptimer->expiration_time);
      /* round start time up to next mS */
      ptimer->expiration_time.tv_nsec = ((ptimer->expiration_time.tv_nsec / 1000000) + 1) * 1000000;
      if (ptimer->expiration_time.tv_nsec >= 1000000000)
      {
         ptimer->expiration_time.tv_nsec -= 1000000000;
         ptimer->expiration_time.tv_sec++;
      }
      Timer_Add_Running(ptimer, interval_msec); /* add to running list waiting to expire */

      Restart_Timer();

      Unlock_Timers();
   }
}

/*
 *  Starts the timer identified by timer_id.
 *
 * set timer interval
 * set initial current_value down value
 * if the requested delay is 0, then immediately send the timer expired message
 *    Note, setting the current value to 0 stops the timer
 *  A periodic interval of zero is not allowed.
 */
void SAL_Start_Timer(SAL_Timer_Id_T timer_id, uint32_t interval_msec, bool_t is_periodic)
{
   if(OS_Timer_Id != NULL)
   {
      Start_Timer(timer_id, interval_msec, is_periodic, false, 0);
   }
}

/*
 *  Starts the timer with extra parameter
 */
void SAL_Start_Timer_Ex(SAL_Timer_Id_T timer_id, uint32_t interval_msec, bool is_periodic, uintptr_t param)
{
   if(OS_Timer_Id != NULL)
   {
      Start_Timer(timer_id, interval_msec, is_periodic, true, param);
   }
}

/*
 *  Stops the timer identified by timer_id.
 *
 *    if running moves the time to the stopped list
 */
void SAL_Stop_Timer(SAL_Timer_Id_T timer_id)
{
   XSAL_Timer_T *ptimer = (XSAL_Timer_T *) timer_id;
   if(OS_Timer_Id != NULL)
   {
      if (ptimer != NULL)
      {
         Lock_Timers();

         if (TIMER_RUNNING == ptimer->state)
         {
            DBLL_Remove_Entry(&Running_Timers, &ptimer->le);
            Timer_Add_Stop(ptimer);
            Restart_Timer();
         }

         Unlock_Timers();
      }
      else
      {
         Tr_Warn("Trying to stop NULL timer");
      }
   }
}

/*---------------------------------------------------------------------------*\
 * XSAL Localized Global Functions
 \*---------------------------------------------------------------------------*/

/*
 * Initializes timer module
 *
 *   Create mutex for control access to timer array for creation and deletion
 *   Initialize timer lists
 *   Start 1 ms uItron cyclic handler
 *   maximum timers is unused as implementation will do as many timers as requested
 */
bool_t SAL_I_Init_Timer_Module(void)
{
   bool_t status = false;
   sigset_t lsigset;

   /* set main thread (and all children) to block timer signal by default */
   (void) sigemptyset(&lsigset);
   (void) sigaddset(&lsigset, SAL_I_TIMER_SIGNAL_ID);
   (void) pthread_sigmask(SIG_BLOCK, &lsigset, NULL);

   if (SAL_Create_Semaphore(&Timer_Sem, NULL))
   {
      SAL_Mutex_Attr_T timer_mutex_attr;

      SAL_Init_Mutex_Attr(&timer_mutex_attr);

      if (SAL_Create_Mutex(&Timer_Mutex, &timer_mutex_attr))
      {
         Allocate_Initial_Timers(TIMER_NUM_INIT_ALLOCATED);

         {
            status = true;
         }
      }
      else
      {
         PBC_Failed("Creation of timer mutex failed");
      }
      if (!status)
      {
         (void) SAL_Destroy_Semaphore(&Timer_Sem);
      }
   }

   if (!status)
   {
      Tr_Fault("SAL_I_Init_Timer_Module failed");
   }

   return status;
}

void SAL_I_Start_Timer_Module(void)
{
   pthread_attr_t attr;
   struct sched_param param;

   (void) pthread_attr_init(&attr);
   (void) pthread_attr_setschedpolicy(&attr, SCHED_RR);
   param.sched_priority = TIMER_THREAD_PRIORITY - (SAL_REAL_TIME_PRIORITY - 1);
   (void) pthread_attr_setschedparam(&attr, &param);
   (void) pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
   if (pthread_create(&Timer_Thread_Id, &attr, Timer_Thread, (void*) 0) != EOK)
   {
      PBC_Failed("SAL_I_Start_Timer_Thread failed");
   }
   (void) pthread_attr_destroy(&attr);

   /** Wait until timer thread will initialize itself
    */
   (void) SAL_Wait_Semaphore_Timeout(&Timer_Sem, TIMER_ACCESS_TIMEOUT);

   Tr_Info_Mid("SAL_I_Start_Timer_Module: has been started");
}

void SAL_I_Stop_Timer_Module(void)
{
   Tr_Info_Mid("SAL_I_Stop_Timer_Module");

   Is_Timer_Thread_Running = false;
   /* wake up timer thread */
   (void) pthread_kill(Timer_Thread_Id, SAL_I_TIMER_SIGNAL_ID);

   if (SAL_Wait_Semaphore_Timeout(&Timer_Sem, TIMER_ACCESS_TIMEOUT))
   {
      (void) SAL_Destroy_Semaphore(&Timer_Sem);
   }
}

/** Function destroys all timers created by given thread.
 */
void SAL_I_Destroy_And_Unbind_Timers_From_Thread(SAL_I_Thread_Attr_T* thread_attr)
{
   Lock_Timers();
   if(OS_Timer_Id != NULL)
   {
      DBLL_Visit_Each(&Running_Timers, Destroy_Thread_Timer, (void *) (intptr_t) (thread_attr->thread_id));
      Restart_Timer();
      DBLL_Visit_Each(&Stopped_Timers, Destroy_Thread_Timer, (void *)  (intptr_t) (thread_attr->thread_id));
   }
   Unlock_Timers();
}

/*---------------------------------------------------------------------------*\
 * XSAL Timer Diagnostics
 \*---------------------------------------------------------------------------*/

/*
 * Access function to get current timer counts
 */
void SAL_Get_Timer_Status(SAL_Timer_Status_T * timer_status)
{
   Lock_Timers();

   timer_status->num_running = DBLL_Get_Count(&Running_Timers);
   timer_status->num_stopped = DBLL_Get_Count(&Stopped_Timers);
   timer_status->num_unused = DBLL_Get_Count(&Unused_Timers);

   Unlock_Timers();
}

/*===========================================================================*/
/*!
 * @file xsal_timer_linux.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 10-Apr-2014 Paul Casto
 *   - Task kok_inf#6635 Fix "implicit declaration of function ‘malloc’" build warning
 *
 * - 20-Nov-2013 Joel Kiger
 *   - Task kok_inf#3040 Fix valgrind read error when exiting
 *
 * 19-Jul-2013 Larry Piekarski
 * - Task 161388: Removed deprecated util functions.
 *
 * 29-Nov-2012 Dan Carman
 *     Use of signal handler cause some blocked calls to resume early (even with
 *     SA_RESTART set) with an EINTR error.
 *     Solution is to go back to sigwait for primary operation, while leaving a default
 *     signal handler defined to catch any signal if the timer expires while the signal
 *     is unblocked 9which it shouldn't ever be).
 *
 * - 16-Nov-2012 Larry Piekarski Rev 7
 *   - Task 128790: Fixed issue where timer handler would only run if sem_wait
 *     returned an error. Thankfully, since SA_RESTART was not set in sa_flags,
 *     this was most of the time. However, if sem_wait did not return an error,
 *     then the timer handler would run and a new timer tick was not scheduled,
 *     causing the system to stop until another process caused sem_wait to error.
 *
 * - 13-Nov-2012 Dan Carman
 *    -Task 128310 Change from sigwait to signal handler.
 *       In certain situations on SBX, the default signal handler (which exits the
 *       program) was executing for the Timer Signal. The change is to use a very short
 *       signal handler to post to semaphore to the timer thread.  Therefore, the actual
 *       signal process is decoupled from sending the XSAL timer messages.
 *
 * - 20-Sep-2012 Larry Piekarski Rev 4
 *   - Task 120116: Fixed an issue where if a timer was set with a timeout
 *     greater than 2 minutes 7 seconds, the new timer would be added to the
 *     linked list as the next expiring timer. This effectively prevented faster
 *     timers from expiring until after the long timer expired.
 *
 *  24-Aug-2012 Dan Carman
 *    Changed from periodic signal to dynamically scheduled.
 *
 *  26Sep11  David Origer (hz1941)  Rev 2
 * - Adjusted lock mutex timeout to allow for periods of heavy CPU load.
 *
 * - 16-Aug-2011 Dan Carman
 *    - Generalized timers from uITRON implementation.
 *    - Removed all OS Specific code for periodic function
 *
 * 20May14  David Origer
 * - Task kok_inf#7893 : Fix Klocwork issues.
 *   - Make sure allocated memory can be freed or properly tracked by
 *     Klocwork.
 *
 * 16-Mar-2015 Dan Carman
 * + Task 21696 Add intptr_t to casts between ints and pointers (thread IDs)
 *
 */
/*===========================================================================*/
