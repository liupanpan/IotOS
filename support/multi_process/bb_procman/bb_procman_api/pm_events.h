/*--------------------------------------------------------------------------
 * File:      pm_events.h
 *
 * Copyright: 2004 Delphi Technologies, Inc., All Rights Reserved
 *            Information Contained Herein Is Proprietary and Confidential
 *
 * Description:
 *
 * Definition of Process Manager events and events structures.
 *
 *
 * Revision history:
 *
 * 13 May 2004, Michal Gebolys (michal.gebolys@delphi.com)
 *   Initial revision.
 *--------------------------------------------------------------------------
 */
#ifndef  PM_EVENTS_H
#define  PM_EVENTS_H

#include "xsal_event_id.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/**
 * Enumeration of possible application state. Please refer PM design document
 * for detailed state's description as well as an application statechart. 
 */
typedef enum Process_State_Tag
{
   Proc_Uninitialized,
   Proc_Spawning,
   Proc_Loading,
   Proc_Running,
   Proc_Terminating
} Process_State_T;

/**
 *  Event sent by an application to the Process Manager to start an application.
 */
typedef struct
{
      int app_id;
} Ev_Start_App_T;

/**
 *  Gets an application id from received message. The message can be 
 *  SAL_Message_T type for C or SALMessage for C++, i.e.
 *  int app_id = Get_App_Id( msg );
 *
 *  @param msg     received message 
 */
#define Get_App_Id( msg ) (((Ev_Start_App_T*)(((SAL_Message_T*)msg)->data))->app_id)

/**
 *  Event published by the Process Manager when an application terminates.
 */
typedef struct
{
      int app_id;
      int return_code;
} Ev_App_Exit_T;

/**
 *  Gets id of terminated application from received message. The message can be 
 *  SAL_Message_T type for C or SALMessage for C++, i.e.
 *  int app_id = Get_Terminated_App( msg );
 *
 *  @param msg     received message 
 */
#define Get_Terminated_App( msg ) (((Ev_App_Exit_T*)(((SAL_Message_T*)msg)->data))->app_id)

/**
 *  Gets application's return code from received message. 
 *  The message can be SAL_Message_T type for C or SALMessage for C++, i.e.
 *  int ret_code = Get_Return_Code( msg );
 *
 *  @param msg      received message 
 */
#define Get_Return_Code( msg ) (((Ev_App_Exit_T*)(((SAL_Message_T*)msg)->data))->return_code)

/**
 *  Event sent by an application to the Process Manager to set 
 *  new system configuration.
 */
typedef struct
{
      int system_config;
} Ev_Set_System_Config_T;

/**
 *  Event sent by an application to the Process Manager to extend
 *  current system configuration.
 */
typedef Ev_Set_System_Config_T Ev_Add_System_Config_T;

/**
 *  Gets a new system coniguration from received message. The message can be 
 *  SAL_Message_T type for C or SALMessage for C++, i.e.
 *  int system_config = Get_System_Config( msg ); 
 *
 *  @param msg      received message
 */
#define Get_New_System_Config( msg ) (((Ev_Set_System_Config_T*)(((SAL_Message_T*)msg)->data))->system_config)

/**
 *  Event published by the Process Manager when system configuratio is reached
 */
typedef struct
{
      int system_config;
} Ev_System_Config_Reached_T;

/**
 *  Gets the reached system coniguration from received message. 
 *  The message can be SAL_Message_T type for C or SALMessage for C++, i.e.
 *  int system_config = Get_Reached_System_Config( msg );
 *
 *  @param msg      received message
 */
#define Get_System_Config( msg ) (((Ev_System_Config_Reached_T*)(((SAL_Message_T*)msg)->data))->system_config)

/**
 *  Replay sent by the Process Manager with an application state.
 */
typedef struct Ev_App_State_Tag
{
      int app_id;
      Process_State_T state;
} Ev_App_State_T;

/**
 *  Gets an application state. The message can be 
 *  SAL_Message_T type for C or SALMessage for C++, i.e.
 *  Process_State_T new_state = Get_App_State( msg );
 *
 *  @param msg     received message 
 */
#define Get_App_State( msg ) (((Ev_App_State_T*)(((SAL_Message_T*)msg)->data))->state)

/*
 * Requests curremt application state. PM replayes with event EV_APP_STATE
 *
 * @param app requested application id
 */
#define PM_Request_App_State( app ) \
{ \
  Ev_App_State_T msg = {app, (Process_State_T)0}; \
  SAL_Send( APP_PROCMAN, 0, EV_GET_APP_STATE, &msg, sizeof( msg ) ); \
}

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */

#endif /* PM_EVENTS_H */

/*-----------------------------------------------------------------------------
 * History
 *
 * 27 Aug. 2004, Michal Gebolys:  Initial revision.
 *-----------------------------------------------------------------------------*/
