/*--------------------------------------------------------------------------
 * File:           pm_process.c
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Michal Gebolys / Lukasz Ptak
 *
 * Description:    Implementation of the PMProcess. 
 *
 *--------------------------------------------------------------------------*/

#define DISABLE_DEPRECATED_STRNCPY

#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "xsal.h"

#if defined LINUX
#include <pthread.h>
#include <signal.h>
#endif

#if defined (WIN32)
#  include <windows.h>
#  include <malloc.h>
#  include <process.h>
#  if !defined(USE_CYGWIN)
#     include <direct.h>
#  endif /* !USE_CYGWIN */

#elif defined (LINUX)
#  include <alloca.h>
#  include <unistd.h>
#  include <sys/types.h>
#  include <signal.h>
#elif defined (QNX_NTO)
#  include <unistd.h>
#  include <process.h>
#else
#  error "Not supported platform"
#endif
#include "pi_direct.h"
#include "pi_process.h"
#include "pm_process.h"
#include "process_observer.h"
#include "pm_os_native_thread.h"
#include "pbc_trace.h"

EM_FILENUM(PROCMAN_MODULE_ID, 3);

#if defined LINUX
extern PM_Process_T* spawning_process;
extern SAL_Semaphore_T spawn_ready;
extern pthread_t thread_id;
extern char **pe_arg;
#endif

#if !defined(PM_DEFAULT_PRIORITY)
#define PM_DEFAULT_PRIORITY SAL_DEFAULT_THREAD_PRIORITY
#endif /* PM_DEFAULT_PRIORITY */

static int system_shutdown = 0;

static void send_spawn_status( bool success, int appId )
{
   Ev_Start_App_T msg;
   msg.app_id = appId;
   SAL_Local_Publish( success ? EV_APP_SPAWN_SUCCESS : EV_APP_SPAWN_FAIL, &msg, sizeof msg );   
}

/* In order to avoid blocking the main thread during spawning a process a new thread is created.
 * This thread is created with priority according to process configuration 
 * and the spawned process inherits this priority.
 */
static void spawn_thread( void *param )
{
   int st_index = 0;
   int i = 0;
   char **p_arg;
   char buf[(sizeof(int))*8 + 3 ];
   PM_Process_T *process = (PM_Process_T*)param;

   Tr_Info_Mid("startProcess");

   p_arg = (char**)alloca( (process->arg_counter + 3) * sizeof(char*) );
   memset( p_arg, '\0', (process->arg_counter + 3) * sizeof(char*) );
   p_arg[ st_index++ ] = process->path;    

   for (i = 0; i < process->arg_counter; i++)
   {
      p_arg[st_index++] = process->argv[i];
   }

   /* If application is restarted -X command line arguments should be added. */
   if (process->ret_code != 0) 
   {
      snprintf( buf, sizeof(buf) -1, "-X%d", process->ret_code );
      p_arg[st_index++] = buf; 
   }
   p_arg[ st_index ] = 0;    

   /* Change working directory */
   if (process->dir)
   {
      if (pi_chdir( process->dir ) != 0)
      {
         Tr_Fault_1( "Change directory to: <%s> failed", process->dir );
      }
   }
#ifdef WIN32
   process->pid = pi_spawnvp( PI_P_NOWAIT, process->path, (const char * const*)p_arg );
#else
   process->pid = pi_spawnvp( PI_P_NOWAIT, process->path, p_arg );
#endif
   if (process->pid != -1)
   {
      Tr_Info_Mid_2("App %s - spawned, %d", process->proc_name, process->pid );
   } 
   else
   {
      /* SCR 21719 In the RMC project it was requested to trigger system restart in case
         when spawn fail.
      */
      POL_Proc_Termination_Msg proc_term_msg;
      Tr_Fault_2( "App %s - spawn fail: %s ", process->proc_name, strerror( errno ) );

      strncpy(proc_term_msg.pathbuffer, process->proc_name, PATH_MAX);
      proc_term_msg.pathbuffer[PATH_MAX] = '\0';
      proc_term_msg.pid   = -1;
      proc_term_msg.tid   = -1;
      proc_term_msg.signo = -1;
      proc_term_msg.fltip = -1;
      proc_term_msg.appid  = process->app_id;
      SAL_Publish( EV_APP_TERMINATED, &proc_term_msg, sizeof(proc_term_msg) );      
   }
   send_spawn_status( process->pid != -1, process->app_id );
}

static void startProcess( PM_Process_T* process )
{
   if (!Create_OS_Native_Thread( spawn_thread, process->priority, process )) 
   {
      send_spawn_status( false, process->app_id );
      Tr_Fault( "Fail to create a thread" );
   }
}



static void clear_deferred_events( PM_Process_T *process )
{
   int i;
   for (i = 0; i < PM_DEFERRED_QUEUE_SIZE; i++)
   {
      process->deferred_events[i] = -1;
   }
}

static void defer_event( PM_Process_T *process, int event )
{
   int i;
   for (i = 0; i < PM_DEFERRED_QUEUE_SIZE; i++)
   {
      if (process->deferred_events[i] == -1 && process->deferred_events[i] != event)
      {
         process->deferred_events[i] = event;
         return;
      }
   }
   Tr_Fault("Deferred events queue full");
}

void PM_Recall_Deferred_Events( PM_Process_T *process )
{
   int i;
   for (i = 0; i < PM_DEFERRED_QUEUE_SIZE; i++)
   {
      if (process->deferred_events[i] != -1)
      {
         PM_Process_Event( process, process->deferred_events[i] );
         process->deferred_events[i] = -1;
      }
   }
}

void PM_Process_Event(PM_Process_T* process, int event)
{
   Tr_Info_Hi_3(" App %s [%d] process Event: %d", process->proc_name, (int)process->state, (int)event );

   switch (process->state)
   {
      case Proc_Uninitialized:
	     if (event == EV_START_APP) 
         {           
            process->ret_code = 0;
            startProcess( process );
			
			/* start timer if load_timeout > 0 */
            process->timer = process->load_timeout;
            process->state = (process->load_timeout > 0) ? Proc_Spawning : Proc_Running; 
         }    

         break;
         
      case Proc_Spawning:
         if (event == EV_APP_SPAWN_SUCCESS)
         {
            /* start timer if load_timeout > 0 */
            process->timer = process->load_timeout;
            process->state = (process->load_timeout > 0) ? Proc_Loading : Proc_Running;
         }
         else if (event == EV_APP_SPAWN_FAIL)
         {
            clear_deferred_events( process );
            process->state = Proc_Uninitialized;
         }
         else if (event == EV_APP_INITIALIZED || event == EV_APP_EXIT || event == EV_SHUTDOWN_APP)
         {
            defer_event( process, event );
         }
         break;

      case Proc_Loading:
         switch (event)
         {
            case EV_APP_INITIALIZED:
               process->state = Proc_Running;
               process->timer = 0;
               break;

            case EV_APP_TIMEOUT:
               Tr_Warn_1(" App %s doesn't send APP_INITIALIZED and will be killed.", process->proc_name);
               PM_Kill_Process(process);
               if (process->auto_restart)
               {
                  startProcess(process);
                  process->timer = process->load_timeout;
                  process->state = Proc_Loading;
               }
               else
               {
                  process->state = Proc_Uninitialized;
               }
               break;

            case EV_SHUTDOWN_APP:
               PM_Kill_Process(process);
               process->timer = 0;
               process->state = Proc_Uninitialized;
               break;

            case EV_APP_EXIT:
               if (process->ret_code != 0)
               {
                  Tr_Warn_2(" App %s exited with code %d.", process->proc_name, (int)process->ret_code);
               }
               process->state = Proc_Uninitialized;
               process->timer = 0;
               break;

            default:
               break;
         } /* end switch(event) */
         break;

      case Proc_Running:
         if (event == EV_SHUTDOWN_APP)
         {
            if (process->terminate_timeout > 0)
            {
               SAL_Send(process->app_id, 0, EV_SHUTDOWN_APP, NULL, 0);
               process->timer = process->terminate_timeout; /* start timer */
               process->state = Proc_Terminating;
            }
            else
            {
               PM_Kill_Process(process);
               process->state = Proc_Uninitialized;
            }
         }
         else if (event == EV_APP_EXIT)
         {
                  if (process->ret_code != 0)
                  {
                     Tr_Warn_2(" App %s exited with code %d.", process->proc_name, (int)process->ret_code);
                  }
                  if (process->auto_restart && process->ret_code != 0 && !system_shutdown)
                  {
                     startProcess(process);
                     if (process->load_timeout > 0)
               {
                  process->timer = process->load_timeout;
                  process->state = Proc_Loading;
               }
            }
            else
            {
               process->state = Proc_Uninitialized;
            }
         }
         break;

      case Proc_Terminating:
         if (event == EV_APP_EXIT)
         {
            process->state = Proc_Uninitialized;
         }
         else if (event == EV_APP_TIMEOUT)
         {
            PM_Kill_Process(process);
            process->state = Proc_Uninitialized;
         }
         break;

      default:
         break;
   } /* end switch (state) */

}

void PM_Kill_Process(PM_Process_T* process)
{
   /* When application is killed it should be removed 
    * from connection and routing tables.
    */
   SAL_NS_Unsubscribe_App(process->app_id);
   SAL_NS_Unbind_App(process->app_id);

#ifdef WIN32   
   TerminateProcess( (HANDLE)process->pid, 1 );
   CloseHandle( (HANDLE)process->pid );
#else 
   kill(process->pid, SIGINT);
#endif
   Tr_Info_Hi_1("App %s - killed", process->proc_name );
}

void PM_Set_Shutdown_Flag(void)
{
   system_shutdown = 1;
}

void PM_Timer_Tick(PM_Process_T *process)
{
   if (--process->timer == 0)
   {
      PM_Process_Event(process, EV_APP_TIMEOUT);
   }
}

int PM_Get_Proc_Id(PM_Process_T *process)
{
   return process->pid;
}

int PM_Get_Proc_App_Id(PM_Process_T *process)
{
   return process->app_id;
}

Process_State_T PM_Get_State(PM_Process_T *process)
{
   return process->state;
}

int PM_Is_In_Configuration(int config, PM_Process_T *process)
{
   return (process->config & (1UL << config)) != 0;
}

void PM_Constructor(PM_Process_T *process)
{
   PBC_Require( process != NULL, "NULL process" );

   process->proc_name = NULL;
   process->config = 0;
   process->argv = NULL;
   process->arg_counter = 0;
   process->app_id = 0;
   process->priority = PM_DEFAULT_PRIORITY;
   process->dir = NULL;
   process->path = NULL;
   process->pid = -1;
   process->load_timeout = 0;
   process->terminate_timeout = 0;
   process->auto_restart = 0;
   process->timer = 0;
   process->ret_code = 0;
   process->state = Proc_Uninitialized;
   clear_deferred_events(process);
}

void PM_Destructor(PM_Process_T *process)
{
   int i;
   if (process != NULL)
   {
      free(process->proc_name);
      for (i = 0; i < process->arg_counter; i++)
      {
         free(process->argv[i]);
      }
      free(process->argv);
      free(process->dir);
      free(process->path);
   }
}

/*-----------------------------------------------------------------------------
 * History
 *
 *
 * 14-May-2015 Kirk Bailey: Fixed Cygwin build warnings.
 * 19-Feb-2015, Vijayalakshmi: Fixed NULL dereference found by klockwork
 * 25 Sep. 2006, Lukasz Ptak:     Port to C.
 * 27 Aug. 2004, Michal Gebolys:  Initial revision.
 *-----------------------------------------------------------------------------*/
