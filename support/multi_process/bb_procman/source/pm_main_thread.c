/*--------------------------------------------------------------------------
 * File:           pm_main_thread.c
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Michal Gebolys / Lukasz Ptak
 *
 * Description:    Implementation of the Process Manager Main Thread. 
 *
 *--------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>

#include "xsal.h"
#include "pbc_trace.h"
#include "pm_cfg.h"
#include "pm_process.h"
#include "pm_main_thread.h"
#include "pm_exit_monitor.h"
#include "pm_parser.h"
#include "pm_queue.h"
#ifdef USE_SYSTIME_FROM_START
#include "sysTime.h"
#endif /* #ifdef USE_SYSTIME_FROM_START */

EM_FILENUM(PROCMAN_MODULE_ID, 1);

/* maximum length of config file line */
#define PM_BUFFER_SIZE 512
#define PM_QUEUE_SIZE 32
#define PM_MESSAGE_SIZE 8
#define PM_MAX_CONFIG_NUMBER 32

typedef enum PM_State_Tag
{
   Running,
   Shutdown
} PM_State_T;

typedef enum PM_RunningState_Tag
{
   Startup,
   Terminating,
   Idle
} PM_RunningState_T;

static int pm_kill = 0;
static PM_Process_T *proc_registry[SAL_MAX_APPS]; /* Process registry */
static unsigned int proc_counter = 0; /* Number of entries in the process registry */
static SAL_Timer_Id_T timer;
static int current_idx = -1; /* Index of process beeing started up */
static int current_config = 0; /* current system configuration */
static PM_State_T state = Running;
static PM_RunningState_T running_state = Startup;

static void Send_Config_Reached(int config)
{
   Ev_System_Config_Reached_T config_msg =
      { config };

   PM_Print_Performance_Info("Configuration reached", config);
   SAL_Publish(EV_SYSTEM_CONFIG_REACHED, (void*) &config_msg, sizeof(config_msg));
}

int PM_Parse_Config_File(const char *file_name)
{
   int line_too_long = 0;
   int ret_val = 1;
   int line_status;
   int line_no = 0;
   PM_Process_T *p_entry = NULL;
   char buffer[PM_BUFFER_SIZE];
   unsigned int i = 0;

   /* Open the file */
   FILE * fp = fopen(file_name, "rt");

   /* set all values of Registry to 0 */
   for (i = 0; i < sizeof(proc_registry) / sizeof(PM_Process_T*); i++)
   {
      proc_registry[i] = 0;
   }

   /* file could not be read */
   if (NULL == fp)
   {
      Tr_Fault_1("cannot open: %s", file_name );
      ret_val = 0;
   }
   /* Read DB items from the file */
   else
   {
      memset(buffer, '\0', PM_BUFFER_SIZE);
      while (fgets(buffer, PM_BUFFER_SIZE, fp))
      {
         if (buffer[PM_BUFFER_SIZE - 2] != '\0')
         {
            Tr_Fault_2( "Too long %d line in %s file", (int)(line_no + 1), file_name );
            ret_val = 0;
            line_too_long = 1;
            break;
         }

         line_status = 1;
         p_entry = PM_Parse_Line(buffer, ++line_no, &line_status);
         if (p_entry != NULL)
         {
            /* If in configuration file process' config number was not mentioned,
             * then it should be automaticaly set to 1 (p_entry->config = 1 means
             * that p_entry is in "0" configuration)
             */
            if (p_entry->config == 0)
            {
               p_entry->config = 1;
            }

            if (proc_counter >= sizeof(proc_registry) / sizeof(PM_Process_T*))
            {
               Tr_Fault_1( "too many apps in configuration file: %d", (int)(proc_counter + 1) );
               PM_Destructor(p_entry);
               free(p_entry);
               p_entry = NULL;
               ret_val = 0;
               break;
            }
            else
            {
               proc_registry[proc_counter++] = p_entry;
            }
         }
         else if (line_status == 0)
         {
            Tr_Fault( "Configuration file not valid" );
            ret_val = 0;
            break;
         }
         memset(buffer, '\0', PM_BUFFER_SIZE);
      } /* while */
      fclose(fp);

      if (proc_counter == 0 && line_too_long == 0)
      {
         Tr_Fault_1( "There are not any valid entries in %s", file_name );
         ret_val = 0;
      }
   } /* end if */
   return ret_val;
}

/** 
 * Searches for processes that are not running
 * and are in specified configuration and puts
 * it into queue
 *
 * @param config new configuration value
 */
void Add_Processes_To_Queue(int config)
{
   unsigned int i;
   int proc_state;
   for (i = 0; i < proc_counter; i++)
   {
      proc_state = PM_Get_State(proc_registry[i]);
      if (PM_Is_In_Configuration(config, proc_registry[i]) && proc_state != Proc_Running && proc_state != Proc_Loading
         && proc_state != Proc_Spawning)
      {
         PM_Queue_Push(i);
      }
   }
}

PM_Process_T* Find_Process_By_Pid(int pid)
{
   unsigned int i = 0;
   for (i = 0; i < proc_counter; ++i)
   {
      if (PM_Get_Proc_Id(proc_registry[i]) == pid)
      {
         return proc_registry[i];
      }
   }

   return 0;
}

void Start_Next_Process(void)
{
   Process_State_T proc_state;

   PBC_Require(current_idx >= 0 && (unsigned int )current_idx < proc_counter, "Invalid process index");

   proc_state = PM_Get_State(proc_registry[current_idx]);
   if (proc_state == Proc_Running || proc_state == Proc_Uninitialized)
   {
      current_idx = PM_Queue_Pop();
      if (current_idx != -1)
      {
         PM_Process_Event(proc_registry[current_idx], EV_START_APP);
         Start_Next_Process();
      }
   }
   /* else wait for the EV_PROCESS_INITIALIZED event */
}

static PM_Process_T* Find_Process_By_App_Id(int app_id)
{
   unsigned int i = 0;
   for (i = 0; i < proc_counter; ++i)
   {
      if (PM_Get_Proc_App_Id(proc_registry[i]) == app_id)
      {
         return proc_registry[i];
      }
   }

   return 0;
}

void Process_App_Exit(PM_Process_T *app, int ret_code)
{
   Ev_App_Exit_T msg;

   /* If application dies it should be removed from connection and routing tables. */
   if (ret_code != 0 && PM_Get_State(app) != Proc_Uninitialized)
   {
      SAL_NS_Unsubscribe_App(PM_Get_Proc_App_Id(app));
      SAL_NS_Unbind_App(PM_Get_Proc_App_Id(app));
   }

   msg.app_id = app->app_id;
   msg.return_code = ret_code;
   SAL_Publish(EV_APP_EXIT, (void*) &msg, sizeof(msg));
   app->ret_code = ret_code;
   PM_Process_Event(app, EV_APP_EXIT);
}

void Message_Loop()
{
   PM_Process_T *process;

   while (1)
   {
      const SAL_Message_T* msg = SAL_Receive();

      /* check if PM is to be killed */
      if (pm_kill)
      {
         int i;
         for (i = proc_counter - 1; i >= 0; i--)
         {
            PM_Kill_Process(proc_registry[i]);
            proc_registry[i]->state = Proc_Uninitialized;
         }
         break;
      }

#ifdef WIN32
      /* Implementation of ExitMonitor for Windows
       * Process monitoring is implemented as pooling on windows
       * each timer message arrived processes are checked.
       */
      {
         unsigned int i;

         for (i = 0; i < proc_counter; i++)
         {
            if (PM_Get_State(proc_registry[i]) != Proc_Uninitialized && PM_Get_State(proc_registry[i]) != Proc_Spawning)
            {
               unsigned long status = STILL_ACTIVE;

               GetExitCodeProcess( (HANDLE)proc_registry[i]->pid, &status );
               if (status != STILL_ACTIVE)
               {
                  CloseHandle( (HANDLE)proc_registry[i]->pid );
                  Process_App_Exit( proc_registry[i], status );
               }
            }
         }
      }
#endif

      switch (msg->event_id)
      {
         case EV_TIMER_1:
         {
            unsigned int i;
            for (i = 0; i < proc_counter; i++)
            {
               PM_Timer_Tick(proc_registry[i]);
            }
            break;
         }
         case EV_SYSTEM_SHUTDOWN: /* graceful shutdown */
         {
            Tr_Info_Hi_1("Received event EV_SYSTEM_SHUTDOWN [%d]", (int)msg->event_id );
            if (state != Shutdown)
            {
               PM_Set_Shutdown_Flag();
               current_idx = proc_counter - 1;
               state = Shutdown;
            }
            break;
         }
         case EV_SET_SYSTEM_CONFIG:
         {
            PM_Print_Performance_Info("Set system config", Get_System_Config( msg ) );
            if (state != Shutdown && Get_System_Config( msg ) >= 0 && Get_System_Config( msg ) < PM_MAX_CONFIG_NUMBER)
            {
               current_config = Get_System_Config( msg );
               PM_Queue_Clear();
               Add_Processes_To_Queue(current_config);
               running_state = Terminating;
               current_idx = proc_counter - 1;
            }
            else
            {
               Tr_Warn( "Attempt to change system config during shutdown or config not in range <0-31>" );
            }
            break;
         }
         case EV_ADD_SYSTEM_CONFIG:
         {
            PM_Print_Performance_Info("Add system config", Get_System_Config( msg ) );
            if (state != Shutdown && Get_System_Config( msg ) >= 0 && Get_System_Config( msg ) < PM_MAX_CONFIG_NUMBER)
            {
               Add_Processes_To_Queue(Get_System_Config( msg ) );
               if (running_state != Terminating)
               {
                  running_state = Terminating;
                  current_idx = -1;
               }
            }
            else
            {
               Tr_Warn( "Attempt to change system config during shutdown or config not in range <0-31>" );
            }
            break;
         }
         case EV_APP_INITIALIZED:
            /* route event to process' state machine */
         {
            Tr_Info_Hi_1("Received event EV_APP_INITIALIZED [%d]", (int)msg->event_id );
            process = Find_Process_By_App_Id(msg->sender_app_id);
            if (process != 0)
            {
               PM_Process_Event(process, msg->event_id);
            }
            else
            {
               Tr_Warn_1( "Receive msg from unknown app: %d", (int)msg->sender_app_id );
            }
            break;
         }

         case EV_START_APP:
         {
            Tr_Info_Hi_1("Rceived event EV_START_APP [%d]", (int)msg->event_id );
            process = Find_Process_By_App_Id(Get_App_Id( msg ) );
            if (process != 0)
            {
               PM_Process_Event(process, EV_START_APP);
            }
            else
            {
               Tr_Warn_1( "Request to start unknown app id: %d", (int)Get_App_Id( msg ) );
            }
            break;
         }

         case EV_PROCESS_EXIT:
         {
            Tr_Info_Hi_1("Rceived event EV_PROCESS_EXIT [%d]", (int)msg->event_id );
            process = Find_Process_By_Pid(Get_PID( msg ) );
            if (process != 0)
            {
               Process_App_Exit(process, Get_Process_Return_Code( msg ) );
            }
            break;
         }

         case EV_GET_APP_STATE:
         {
            Tr_Info_Hi_1("Rceived event EV_GET_APP_STATE [%d]", (int)msg->event_id );
            process = Find_Process_By_App_Id(Get_App_Id( msg ) );
            if (process != 0)
            {
               Ev_App_State_T state_msg =
                  { process->app_id, process->state };
               SAL_Send(msg->sender_app_id, 0, EV_APP_STATE, &state_msg, sizeof(state_msg));
            }
            else
            {
               Tr_Fault_1( "Request state of an unknown app id: %d", (int)Get_App_Id( msg ) );
            }
            break;
         }

         case EV_APP_SPAWN_SUCCESS:
            /* route event to process' state machine */
         {
            Tr_Info_Mid_1("Received event EV_APP_SPAWN_SUCCESS [%d]", msg->event_id);
            process = Find_Process_By_App_Id(Get_App_Id( msg ) );
            if (process != 0)
            {
               PM_Process_Event(process, msg->event_id);
               PM_Recall_Deferred_Events(process);
            }
            else
            {
               Tr_Warn_1("Receive msg from unknown app: %d", msg->sender_app_id);
            }
            break;
         }

         case EV_APP_SPAWN_FAIL:
            /* route event to process' state machine */
         {
            Tr_Info_Hi_1("Received event EV_APP_SPAWN_FAIL [%d]", msg->event_id);
            process = Find_Process_By_App_Id(Get_App_Id( msg ) );
            if (process != 0)
            {
               PM_Process_Event(process, msg->event_id);
            }
            else
            {
               Tr_Warn_1("Receive msg from unknown app: %d", msg->sender_app_id);
            }
            break;
         }

         default:
         {
            break;
         }
      } /* switch event */

      if (state == Shutdown)
      {
         while (current_idx != -1)
         {
            if (PM_Get_State(proc_registry[current_idx]) == Proc_Uninitialized)
            {
               current_idx--;
               continue;
            }
            /* wait for process to terminate */
            if (PM_Get_State(proc_registry[current_idx]) == Proc_Terminating
               || PM_Get_State(proc_registry[current_idx]) == Proc_Spawning)
            {
               break;
            }
            /* send 'shutdown' requets to process */
            PM_Process_Event(proc_registry[current_idx], EV_SHUTDOWN_APP);
         }
         if (current_idx == -1)
         {
            /* all processes stopped */
            Tr_Info_Hi("All processes stopped, exit PM" );
            break;
         }
      }
      else
      {
         switch (running_state)
         {
            case Startup:
               Start_Next_Process();

               if (current_idx == -1)
               {
                  Send_Config_Reached(current_config);
                  running_state = Idle;
                  Tr_Info_Hi("Entering state Idle" );
               }
               break;

            case Terminating:
               while (current_idx != -1)
               {
                  if (PM_Get_State(proc_registry[current_idx]) == Proc_Uninitialized
                     || PM_Is_In_Configuration(current_config, proc_registry[current_idx]))
                  {
                     current_idx--;
                     continue;
                  }
                  /* wait for process to terminate */
                  if (PM_Get_State(proc_registry[current_idx]) == Proc_Terminating
                     || PM_Get_State(proc_registry[current_idx]) == Proc_Spawning)
                  {
                     break;
                  }
                  /* send 'shutdown' requests to process */
                  PM_Process_Event(proc_registry[current_idx], EV_SHUTDOWN_APP);
               }

               if (current_idx == -1)
               {
                  /* all processes stopped, start first process from the new config */
                  running_state = Startup;
                  Tr_Info_Hi("Entering state Startup" );

                  current_idx = PM_Queue_Pop();
                  if (current_idx != -1)
                  {
                     PM_Process_Event(proc_registry[current_idx], EV_START_APP);
                     Start_Next_Process();
                  }

                  if (current_idx == -1)
                  {
                     Send_Config_Reached(current_config);
                     running_state = Idle;
                     Tr_Info_Hi("Entering state Idle" );
                  }
               }

               break;

            case Idle:
            default:
               /* do nothing */
               break;
         } /* switch */
      }
   }
}

void PM_Main_Thread(void *param)
{
   unsigned int i = 0;
   static SAL_Event_Id_T subscribe_list[] =
      { EV_SYSTEM_SHUTDOWN,
         EV_SET_SYSTEM_CONFIG,
         EV_ADD_SYSTEM_CONFIG,
         EV_START_APP,
         EV_APP_INITIALIZED,
         EV_PROCESS_EXIT,
         EV_GET_APP_STATE,
         EV_APP_SPAWN_SUCCESS,
         EV_APP_SPAWN_FAIL };

#if defined LINUX
   static SAL_Thread_Id_T wait_list[] =
      {NAME_SERVER_THREAD, PM_EXIT_MONITOR_THREAD};
#else
   static SAL_Thread_Id_T wait_list[] =
      { NAME_SERVER_THREAD };
#endif

   if (!SAL_Subscribe(subscribe_list, sizeof(subscribe_list) / sizeof(int)))
   {
      Tr_Fault("Call to SAL_Subscribe() failed.  Exiting.");
      SAL_Exit(1);
   }

   /* Wait for NS and ExitMonitor(only under Montavista OS) thread to initialize */
   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(wait_list[0]));

   if (!SAL_Create_Queue(PM_QUEUE_SIZE, PM_MESSAGE_SIZE, malloc, free))
   {
      Tr_Fault("Call to SAL_Create_Queue() failed.  Exiting.");
      SAL_Exit(1);
   }
   PM_Queue_Init();
   SAL_Signal_Ready();

   /* start first process */
   Add_Processes_To_Queue(current_config);
   current_idx = PM_Queue_Pop();
   if (current_idx != -1)
   {
      PM_Process_Event(proc_registry[current_idx], EV_START_APP);

      /* all remaining processes in current configuration are started */
      Start_Next_Process();
   }

   if (current_idx == -1)
   {
      Send_Config_Reached(current_config);
      running_state = Idle;
      Tr_Info_Hi("Entering state Idle");
   }

   if (proc_counter > 0)
   {
      if (!SAL_Create_Timer(EV_TIMER_1, &timer))
      {
         Tr_Fault("Call to SAL_Create_Timer() failed.  Exiting.");
         SAL_Exit(1);
      }
      SAL_Start_Timer(timer, PM_TIMER_PERIOD_MS, true);
   }

   /* enter the msg loop */
   Message_Loop();

   /*Destroy proc_registry*/
   for (i = 0; i < proc_counter; i++)
   {
      PM_Destructor(proc_registry[i]);
      free(proc_registry[i]);
   }

   /* terminate the application */
   Tr_Fault("**** Exiting App ****");
   SAL_Exit(0);
}

void PM_Stop_Main_Thread()
{
   pm_kill = 1;
}

void PM_Set_Configuration(int config)
{
   PBC_Require(config >= 0 && config < PM_MAX_CONFIG_NUMBER, "Illegal config index");
   current_config = config;
}

void PM_Print_Performance_Info(const char *info_msg, int config_num)
{
#ifdef USE_SYSTIME_FROM_START
   uint64_t dNSec = systime_get_time_from_start();
   uint32_t nHr, nMin, nSec, nMSec;

   nHr = (uint32_t)(dNSec / 3600000000000ULL);
   dNSec %= 3600000000000ULL;
   /* NSECPERHR   */
   nMin = (uint32_t)(dNSec / 60000000000ULL);
   dNSec %= 60000000000ULL;
   /* NSECPERMIN  */
   nSec = (uint32_t)(dNSec / 1000000000ULL);
   dNSec %= 1000000000ULL;
   /* NSECPERSEC  */
   nMSec = (uint32_t)(dNSec / 1000000ULL);
   Tr_Info_Hi_2( "%s, %d", info_msg, (int)config_num);
   Tr_Info_Hi_4( "%2d:%02d:%02d.%03d", (int)nHr, (int)nMin, (int)nSec, (int)nMSec);
#else
   Tr_Info_Hi_2( "%s, %d", info_msg, (int)config_num);
#endif /* #ifdef USE_SYSTIME_FROM_START */
}
/*-----------------------------------------------------------------------------
 * History
 *
 * 27 Aug. 2004, Michal Gebolys:  Initial revision.
 * 25 Sep. 2006, Lukasz Ptak:     Port to C.
 * 13-aug-2008, Kirk Bailey: Made period of timer configurable.
 * 05-Jun-2012, Kirk Bailey: Fixed leak found by Klockwork.
 * 26-Sep-2013, Larry Piekarski: Fixed compiler warnings.
 * 11-Apr-2016, Kirk Bailey: Fixed CHECKED_RETURN Coverity defect.
 * 11-Apr-2016, Kirk Bailey: Improved log messages.
 *-----------------------------------------------------------------------------*/
