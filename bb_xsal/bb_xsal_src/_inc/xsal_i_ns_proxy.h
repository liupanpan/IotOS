/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_ns_proxy.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the internal XSAL naming service
 *              interface.
 *
 *------------------------------------------------------------------------*/

#ifndef XSAL_I_NS_PROXY_H
#define XSAL_I_NS_PROXY_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal.h"


/** Defines maximum number of channels in the Name Server.
 *  By default the max number o channels is equal the number 
 *  of the SAL application (defined in "application_id.h" file) 
 *  multiplied by value 3.
 */
#define SAL_MAX_CHANNELS (SAL_MAX_APPS * SAL_NS_CHANNEL_MULTIPLIER)


/* -------------------------------------------------------------------------
 *
 *                     Name Server global data structures
 *
 * -----------------------------------------------------------------------*/


/** Structure binds application id and port id with channel.
 */
typedef struct SAL_NS_Channel_Tag
{
   /** Application id bound with channel.
    */
   SAL_App_Id_T app_id; 

   /** Port id bound with channel.
    */
   SAL_Port_Id_T port_id;

   /** Channel bound with given application id and port id
    */
   SAL_Channel_T channel;
} SAL_NS_Channel_T;


/** Channels table.
 */
typedef struct SAL_NS_Channels_Tag
{
   /** Current number of registered channels in the Name Server.
    */
   size_t number_of_channels;

   /** Table with channels data
    */
   SAL_NS_Channel_T channel[SAL_MAX_CHANNELS];
} SAL_NS_Channels_T;


/** Name Server data
 */
typedef struct SAL_NS_Data_Tag
{
   /** Name Server Channel. 
    *  Applications create connection to this channel.
    */
   SAL_Channel_T name_server_channel;

   /** Max number of channels that can be stored in the Name Server. 
    *  It must be equal to SAL_MAX_CHANNELS.
    *  It allows to check if the SAL application and the Name Server
    *  were compiled with the same configuration.
    */
   size_t max_number_of_channels;

   /** Number of appplication. 
    *  It must be equal to SAL_MAX_APPS.
    *  It allows to check if the SAL application and the Name Server
    *  were compiled with the same configuration (number of applications)
    */
   size_t app_number;

   /** Number of events.
    *  It must be equal to SAL_MAX_EVENTS.
    *  It allows to check if the SAL application and the Name Server
    *  were compiled with the same configuration (number of events)
    */
   size_t event_number;

   /** Application channels.
    *  This structure keeps all registered channels.
    */
   SAL_NS_Channels_T channels;

   /** Global Routing Table.
    *  This table keeps information which applications are subscribed 
    *  to events.
    *  If the routing_table[ev_id][app_id] != 0 it means that
    *  given application is subscribed to given event.
    */
   uint8_t routing_table[SAL_MAX_EVENTS][SAL_MAX_APPS+1];
} SAL_NS_Data_T;


/* -------------------------------------------------------------------------
 *
 *                      Name Server proxy data structures
 *
 * -----------------------------------------------------------------------*/

/** List of Name Server commands
 */
typedef enum SAL_NS_Command_Tag
{
   /** Bind channel to app_id and port_id
    */
   SAL_NS_BIND_CHANNEL, 

   /** Unbind channel
    */
   SAL_NS_UNBIND_CHANNEL, 

   /** Finds channel bound to given app_id and port_id
    */
   SAL_NS_RESOLVE_CHANNEL, 

   /** Unbind application
    */
   SAL_NS_UNBIND_APP, 

   /** Get list of running application ids
    */
   SAL_NS_GET_RUNNING_APP_IDS, 

   /** Subscribe events
    */
   SAL_NS_SUBSCRIBE, 

   /** Unsubscribe events
    */
   SAL_NS_UNSUBSCRIBE, 

   /** Unsubscribe all events subscribed by given application
    */
   SAL_NS_UNSUBSCRIBE_APP,

   /** Get value of last XSAL application id.
    */
   SAL_NS_GET_LAST_APP_ID,

   /** Get data for the specified XSAL application.
    */
   SAL_NS_GET_APP_DATA,

   /** Get value of last dbg_trace module id.
    */
   SAL_NS_GET_LAST_DBG_MODULE_ID,

   /** Get data for the specified dbg_trace module.
    */
   SAL_NS_GET_DBG_MODULE_DATA,
} SAL_NS_Command_T;


/** 
 *  Data structure for Bind command.
 */
typedef struct SAL_NS_Bind_Channel_Tag
{
   /** Command id for bind channel request (SAL_NS_BIND_CHANNEL)
    *
    *  @note command field has to be the first member of the structure.   
    */
   SAL_NS_Command_T command;

   /** app_id to be bound with channel 
    */
   SAL_App_Id_T app_id;

   /** port_id to be bound with channel 
    */
   SAL_Port_Id_T port_id;

   /** channel to be bound with app_id and port_id in the Name Server
    */
   SAL_Channel_T channel;
} SAL_NS_Bind_Channel_T;


/**
 *  Data structure for Unbind command
 */
typedef struct SAL_NS_Unbind_Channel_Tag
{
   /** Command id for unbind channel request (SAL_NS_UNBIND_CHANNEL)
    *
    *  @note command field has to be the first member of the structure.   
    */
   SAL_NS_Command_T command;

   /** Channel to be unbound from Name Server
    */
   SAL_Channel_T channel;
} SAL_NS_Unbind_Channel_T;


/**
 *  Data structure for Resolve command.
 */
typedef struct SAL_NS_Resolve_Channel_Tag
{
   /** Command id for resolve channel request (SAL_NS_RESOLVE_CHANNEL).
    *
    *  @note command field has to be the first member of the structure. 
    */
   SAL_NS_Command_T command;

   /** app_id of the channel to be looked for
    */
   SAL_App_Id_T app_id;

   /** port_id of the channel to be looked for
    */
   SAL_Port_Id_T port_id;
} SAL_NS_Resolve_Channel_T;


/**
 *  Data structure for UnbindApp command.
 */
typedef struct SAL_NS_Unbind_App_Tag
{
   /** Command id for unbind application channels request (SAL_NS_UNBIND_APP).
    *
    *  @note command field has to be the first member of the structure. 
    */
   SAL_NS_Command_T command;

   /** app_id to be unbound from Name Server
    */
   SAL_App_Id_T app_id;
} SAL_NS_Unbind_App_T;


/**
 *  Data structure for GetRunningApps command.
 */
typedef struct SAL_NS_Get_Running_Apps_Tag
{
   /** Command id for get running applicaion id's list (SAL_NS_GET_RUNNING_APP_IDS).
    *
    *  @note command field has to be the first member of the structure. 
    */
   SAL_NS_Command_T command;
} SAL_NS_Get_Running_Apps_T;


/**
 *  Data structure for Subscribe and Unsubscribe commands.
 */
typedef struct SAL_NS_Subscribtion_Tag
{
   /** Command id for subscribe or unsubscribe events request 
    *  (SAL_NS_SUBSCRIBE or SAL_NS_UNSUBSCRIBE)
    *
    *  @note command field has to be the first member of the structure. 
    */
   SAL_NS_Command_T command;

   /** app_id to subscribe or unsubscribe to/from events
    */
   SAL_App_Id_T app_id;

   /** Number of event to be subscribed/unsubscribe
    */
   size_t number_of_events;

   /** List of subscribed/unsubscribed events is sent just after this 
    *  data structure.
    */
} SAL_NS_Subscribtion_T;


/**
 *  Data structure for UnsubscribeApp command.
 */
typedef struct SAL_NS_Unsubscribe_App_Tag
{
   /** Command id for unsubscribe application events request 
    *  (SAL_NS_UNSUBSCRIBE_APP)
    *
    *  @note command field has to be the first member of the structure. 
    */
   SAL_NS_Command_T command;

   /** app_id which events are unsubscribed from Routing Table
    *  in the Name Server.
    */
   SAL_App_Id_T app_id;
} SAL_NS_Unsubscribe_App_T;

/**
 *  Data structure for SAL_NS_GET_LAST_APP_ID command.
 */
typedef struct SAL_NS_Get_Last_App_Id_Tag
{
   /** Command id for get running applicaion id's list (SAL_NS_GET_LAST_APP_ID).
    *
    *  @note command field has to be the first member of the structure.
    */
   SAL_NS_Command_T command;
}
SAL_NS_Get_Last_App_Id_T;

/**
 *  Data structure for SAL_NS_GET_APP_DATA command.
 */
typedef struct SAL_NS_Get_App_Data_Tag
{
   /** Command id for get running applicaion id's list (SAL_NS_GET_NUM_APPS).
    *
    *  @note command field has to be the first member of the structure.
    */
   SAL_NS_Command_T command;
   uint32_t app_id;
}
SAL_NS_Get_App_Data_T;

/**
 *  Data structure for SAL_NS_GET_LAST_DBG_MODULE_ID command.
 */
typedef struct SAL_NS_Get_Last_Dbg_Module_Id_Tag
{
   /** Command id for get running applicaion id's list (SAL_NS_GET_LAST_DBG_MODULE_ID).
    *
    *  @note command field has to be the first member of the structure.
    */
   SAL_NS_Command_T command;
}
SAL_NS_Get_Last_Dbg_Module_Id_T;

/**
 *  Data structure for SAL_NS_GET_DBG_MODULE_DATA command.
 */
typedef struct SAL_NS_Get_Dbg_Module_Data_Tag
{
   /** Command id for get running applicaion id's list (SAL_NS_GET_DBG_MODULE_DATA).
    *
    *  @note command field has to be the first member of the structure.
    */
   SAL_NS_Command_T command;
   uint32_t module_id;
}
SAL_NS_Get_Dbg_Module_Data_T;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_NS_PROXY_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
