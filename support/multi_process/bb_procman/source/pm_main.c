/*--------------------------------------------------------------------------
 * File:           main.c
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Michal Gebolys / Lukasz Ptak
 *
 * Description:    The ProcessManager application.
 *
 *--------------------------------------------------------------------------
 */
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "xsal.h"
#include "pi_stdlib.h"
#include "pi_filesystem.h"
#include "pi_unistd.h"
#include "xsal_ns_thread.h"
#include "dbg_trace.h"
#include "pm_process.h"
#include "pm_main_thread.h"
#include "pm_exit_monitor.h"
#include "procman_cbk.h"

#if defined(LINUX)
void SAL_Background_Process(void);
#endif /* LINUX */

#if defined (QNX_NTO)
#include "process_observer.h"
#endif

EM_FILENUM(PROCMAN_MODULE_ID, 0);

typedef struct PM_Init_Values_Tag
{
      int f, c, n, h, p;
      const char* file_name;
#if defined (XSAL_NET)
      int P;
      const char* net_config_file_name;
#endif /* XSAL_NET */
} PM_Init_Values_T;

#if defined(LINUX)
PM_Process_T *spawning_process = NULL;
SAL_Semaphore_T spawn_ready;
pthread_t thread_id;
char **pe_arg;
#endif

static PM_Init_Values_T init_values =
   { 0, 0, 0, 0, 0, NULL ,
#if defined (XSAL_NET)
      0, NULL
#endif /* XSAL_NET */
   };

extern SAL_Channel_T SAL_I_RT_Channel;

static void Usage(const char *app_name)
{
   printf("Usage: \n"
      "%s -n | -f <config-file> [-c <configuration>] "
      "[-g <group>] [-l <log-file>] [-t] [-e] [-N <application name>] [-v <level>] [-h]\n"
      "  -n Run the name server thread only.\n"
      "  -f Name of the configuration file. If it is not defined then XSAL_INSTANCE_NAME environment variable is used instead.\n"
#if defined (XSAL_NET)
      "  -F Name of the XSAL network configuration file.\n"
      "  -P <priority> network gateway thread priority\n"
#endif /* XSAL_NET */
      "  -c Configuration id to be started, 0 is default value.\n"
      "  -p <priority> ProcessManager priority\n"
      "  -h Shows this info.\n"
      "  -t, -l, -g, -e, -v and -N are DebugTrace parameters.\n\n"

      "It is possible to run your own instance of the ProcessManger.\n"
      "In order to do it set unique name to the XSAL_INSTANCE_NAME environment variable,\n"
      "e.g.\n"
      "export XSAL_INSTANCE_NAME=my_procces_manager\n\n", app_name);
}

static void Int_Signal_Handler(int sig_no)
{
   signal(SIGINT, SIG_DFL );

   if (!SAL_Is_Thread_Alive(PM_MAIN_THREAD))
   {
      /* Only Name Server thread is running
       * It's not allowed to use any XSAL functions
       * in asynchronous signal handler function e.g. SAL_Exit( 0 );
       */
#if defined (XSAL_NET)
      SAL_I_Net_Send_Power_Off_Notify();
#endif /* XSAL_NET */
      SAL_Name_Server_Destroy();
      SAL_Destroy_Channel(&SAL_I_RT_Channel);
      _exit(0);
   }
   PM_Stop_Main_Thread();
}

static int Parse_Command_Line(int argc, char* argv[])
{
   int status = 1;
   int option;
   /* g:v:l:teN: - DebugTrace options */
   /* Parameters: F:P: are used only in XSAL_NET. In other configurations are ignored */
   while ((option = pi_getopt(argc, argv, "h:f:c:np:" "g:v:l:teN:" "F:P:")) != -1)
   {
      switch (option)
      {
         case 'f': /* config file */
            init_values.f++;
            init_values.file_name = pi_optarg;
            break;

         case 'c': /* default configuration*/
            if (!pi_atoi(pi_optarg, &init_values.c))
            {
               status = 0;
            }
            break;

         case 'n': /* Name Server only */
            init_values.n++;
            break;

         case 'p': /* default configuration*/
            if (!pi_atoi(pi_optarg, &init_values.p))
            {
               status = 0;
            }
            break;

         case 'h':
            init_values.h = 1;
            break;

#if defined (XSAL_NET)
         case 'F': /* Name of the network config file */
            init_values.net_config_file_name = pi_optarg;
            break;

         case 'P': /* Priority of the network gateway thread */
            if (!pi_atoi(pi_optarg, &init_values.P))
            {
               status = 0;
            }
            break;

#endif /* XSAL_NET */

         default:
            break;
      }
   }
#if defined (XSAL_NET)
   if (init_values.net_config_file_name == NULL)
   {
      init_values.net_config_file_name = getenv("XSAL_NET_CONFIG_FILE");
      if (init_values.net_config_file_name == NULL)
      {
         Tr_Err(
            "XSAL network config file not specified.\n"
            "Use -F command line parameter or XSAL_NET_CONFIG_FILE environment variable");
         status = 0;
      }
   }
#endif /* XSAL_NET */
   return status;
}

static void Start_Threads(void)
{
   SAL_Thread_Attr_T thread_attr;

   Name_Server_Get_Thread_Attr(&thread_attr);

   if (SAL_Start_Name_Server_Thread(&thread_attr) < 0)
   {
      Tr_Fault("Couldn't start Name Server thread");
      exit(1);
   }

#if defined (XSAL_NET)
   if (SAL_I_Net_Start(
      NET_GATEWAY_THREAD,
      NAME_SERVER_THREAD,
      (init_values.P == 0) ? thread_attr.priority : init_values.P) < 0)
   {
      Tr_Err("Couldn't start Net Gateway thread");
      exit(1);
   }
#endif /* XSAL_NET */

   if (init_values.f == 1 && PM_Parse_Config_File(init_values.file_name))
   {
      PM_Main_Get_Thread_Attr(&thread_attr);
      SAL_Create_Thread(PM_Main_Thread, NULL, &thread_attr);

#if defined(XSAL_POSIX)

      PM_Exit_Monitor_Get_Thread_Attr(&thread_attr);
      SAL_Create_Thread(PM_Exit_Monitor_Thread, NULL, &thread_attr);

#endif /* XSAL_POSIX */

   }
}

static void Sigpipe_Handler(int signal_id, siginfo_t *info, void *context)
{
   Tr_Warn("SIGPIPE handled");
}

int main(int argc, char* argv[])
{
   int ret = 0;
   int status;
   const char *name;
   struct sigaction sigpipe_action =
      {
         { 0 } };

   sigpipe_action.sa_flags = SA_SIGINFO;
   sigpipe_action.sa_sigaction = Sigpipe_Handler;
   if (0 != sigaction(SIGPIPE, &sigpipe_action, NULL))
   {
      Tr_Fault("sigaction() failed for SIGPIPE");
   }

   PM_Print_Performance_Info("Procman started", 0);

   status = Parse_Command_Line(argc, argv);

   if (status != 0 && init_values.h == 0 && ((init_values.f == 1 && init_values.n == 0) || (init_values.f == 0 && init_values.n == 1)))
   {
      if (SAL_Name_Server_Init())
      {
         SAL_Config_T config;
         SAL_Get_Config(&config);

         /* install signal handler */
         signal(SIGINT, &Int_Signal_Handler);

         config.app_id = APP_PROCMAN;
         config.max_number_of_threads = PM_LAST_THREAD_ID;
         if (!SAL_Init(&config))
         {
            Tr_Fault_1("SAL_Init status = %d", (int)status);
            exit(1);
         }

         if (init_values.p > 0)
         {
            SAL_Set_Thread_Priority(SAL_ROUTER_THREAD_ID, init_values.p);
         }
         if (init_values.c != 0)
         {
            PM_Set_Configuration(init_values.c);
         }
#if defined(LINUX)
         if (SAL_Create_Semaphore(&spawn_ready, 0))
         {
            SAL_Background_Process();
#else
         {
#endif
               Start_Threads();

#if defined (QNX_NTO)
               /* Start the Process Observer Thread, if it is configured/enabled */
               if (Is_POL_Enabled())
               {
                  POL_Start_Process_Observer();
               }
#endif			
               ret = SAL_Run();
#if defined (XSAL_NET)
               SAL_I_Net_Send_Power_Off_Notify();
#endif /* XSAL_NET */
               SAL_Name_Server_Destroy();
            }
         }
         else
         {
            Tr_Fault("Cannot initialize Name Server");
            ret = 1;
         }
      }
      else
      {
         name = strrchr(argv[0], PI_PATH_SEP);
         Usage(name != NULL ? name + 1 : argv[0]);
      }

      return ret;
   }

   /*-----------------------------------------------------------------------------
    * History
    *
    * 20Jan12  David Origer (hz1941)  Rev 8
    * SCR kok_basa#20206 : Handle SIGPIPE so procman does not crash during
    *    shutdown.
    *
    * 25 Sep. 2006, Lukasz Ptak:     Port to C.
    * 27 Aug. 2004, Michal Gebolys:  Initial revision.
    *-----------------------------------------------------------------------------*/

