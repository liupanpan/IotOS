#ifndef PM_PROCESS_H
#define PM_PROCESS_H
/*--------------------------------------------------------------------------
 * File:           pm_process.h
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Michal Gebolys / Lukasz Ptak
 *
 * Description:    Definition of the PMProcess. 
 *
 *--------------------------------------------------------------------------
 */

#   include "pm_events.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */

#define PM_DEFERRED_QUEUE_SIZE 4

/** 
 *  A database entry for a process. Contains information necessary to start
 *  a process, like process name, path, etc., as well as some other transitory
 *  information which may change during a processes running lifecycle. 
 */
typedef struct PM_Process_Tag
{
      char *proc_name; /**< Process name. */
      unsigned int config; /**< Process configuration mask. */
      char **argv; /**< Argument list of the process. */
      int arg_counter; /**< Number of arguments */
      int app_id; /**< Application id. */
      int priority; /**< Process priority. */
      char *dir; /**< Working directory. */
      char *path; /**< Executable file path. */
      int pid; /**< Process id. */
      /** Process loading timeout; granularity is configurable. */
      int load_timeout;
      /** Process termination timeout; granularity is configurable. */
      int terminate_timeout;
      int auto_restart; /**< Process auto-restart flag. */
      /** Used to time loading and termination; granularity is configurable.*/
      int timer;
   int ret_code;                /* Code returned by process when terminated, default 0 means first app spawn */
   Process_State_T state;       /* Current state of the process. */
   int deferred_events[PM_DEFERRED_QUEUE_SIZE]; /* queue of deferred events */
} PM_Process_T;

/** 
 * Implements process state machine.
 *
 * @param process  pointer to process which receives an event
 * @param event    event received by process' state machine
 * @param ret_code  status returned by application (EV_APP_EXIT event parameter)
 */
void PM_Process_Event( PM_Process_T* process, int event );

/** 
 * Function called by the timer every clock tick (1s).
 * It decreases internal process counter, if zero is reached
 * EV_TIMEOUT event is generated.
 *
 * @param process  pointer to process which decrements its timer counter
 */
void PM_Timer_Tick(PM_Process_T *process);

/** 
 * Tests if process belongs to given configuration.
 *
 * @param config   configuration to be tested
 * @param process  pointer to process to be tested
 *
 * @return 1 successful parsing.
 *         0 otherwise
 */
int PM_Is_In_Configuration(int config, PM_Process_T *process);

/** 
 * Terminates a process.
 *
 * @param process  pointer to process
 */
void PM_Kill_Process(PM_Process_T *process);

/** 
 * sets system_shutdown flag.
 */
void PM_Set_Shutdown_Flag(void);

/** 
 * Gets current state of the process.
 *
 * @param process  pointer to process
 *
 * @return Process state
 */
Process_State_T PM_Get_State(PM_Process_T *process);

/** 
 * Gets Process id from the process.
 *
 * @param process  pointer to process
 *
 * @return Process id
 */
int PM_Get_Proc_Id(PM_Process_T *process);

/** 
  * Recalls events stored in the deferred_events queue
  * 
  * @param process  pointer to process
  */
void PM_Recall_Deferred_Events( PM_Process_T *process );

/** 
 * Gets Application id from the process.
 *
 * @param process  pointer to process
 *
 * @return Application id
 */
int PM_Get_Proc_App_Id(PM_Process_T *process);

/** 
 * Fills with initial values PM_Process_T data structure.
 *
 * @param process  pointer to data structure to be initialized.
 */
void PM_Constructor(PM_Process_T *process);

/** 
 * Frees *char fields of PM_Process_T data structure.
 *
 * @param process  pointer to data structure to be destroyed.
 */
void PM_Destructor(PM_Process_T *process);

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */
/*-----------------------------------------------------------------------------
 * History
 *
 * 13-aug-2008 Kirk Bailey: Made granularity of timeouts configurable.
 * 25 Sep. 2006, Lukasz Ptak:     Port to C.
 * 27 Aug. 2004, Michal Gebolys:  Initial revision.
 *-----------------------------------------------------------------------------*/
#endif /* PM_PROCESS_H */
