/*--------------------------------------------------------------------------
 * File:           pm_exit_monitor.c
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Michal Gebolys / Lukasz Ptak
 *
 * Description:    Implementation of the Exit Monitor Thread.
 *
 *--------------------------------------------------------------------------*/

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#if defined LINUX
#include <string.h>
#include <unistd.h>
#include "pi_direct.h"
#include "pi_process.h"
#include "pm_process.h"
#endif

#include "pm_exit_monitor.h"
#include "pm_events.h"

#if defined LINUX
extern PM_Process_T* spawning_process;
extern SAL_Semaphore_T spawn_ready;
/*extern*/pthread_t thread_id;
extern char **pe_arg;

static void Signal_Handler( int sig_no )
{
   spawning_process->pid = pi_spawnvp( PI_P_NOWAIT, spawning_process->path, pe_arg );
   spawning_process = NULL;
   (void)SAL_Signal_Semaphore(&spawn_ready);
}
#endif

void PM_Exit_Monitor_Thread(void *param)
{
   pid_t pid;
   int ret_val = 0;
   Ev_Process_Exit_T msg;

   static int wait_list[] =
      { PM_MAIN_THREAD };

#if defined LINUX
   /* install signal handler */
   signal( SIGUSR1, &Signal_Handler );

   thread_id = pthread_self();
   SAL_Signal_Ready();
#endif

   /* Wait for PMMain thread to initialize. */
   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(int));

   while (1)
   {
      pid = wait(&ret_val);

      if (pid == -1)
      {
         /* No child processes. In case of long load time the processes may have not been 
          * completly loaded yet. In that case, to prevent PM from unnecessary killing, wait
          * for a while giving processes opportunity to load completly.
          */
         SAL_Sleep(200);
      }
      else
      {
         /* Some child process has done exit. */
         msg.pid = pid;
         msg.return_code = ret_val;
         SAL_Local_Publish(EV_PROCESS_EXIT, &msg, sizeof(msg));
      }
   }
}

/*-----------------------------------------------------------------------------
 * History
 *
 * 25 Sep. 2006, Lukasz Ptak:     Port to C.
 * 27 Aug. 2004, Michal Gebolys:  Initial revision.
 *-----------------------------------------------------------------------------*/
