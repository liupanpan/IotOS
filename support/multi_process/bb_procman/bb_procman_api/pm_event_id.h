#ifndef  PM_EVENT_ID_H
#   define  PM_EVENT_ID_H
/*===========================================================================*/
/**
 * @file pm_event_id.h
 *
 *    Defines XSAL events for the process manager.
 *
 * %full_filespec: pm_event_id.h~kok_basa#14:incl:kok_aud#1 %
 * @version %version: kok_basa#14 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Fri Apr 17 14:55:33 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   These declarations are included into the xsal_event_id module to be
 *   assigned unique values within the system's event space.
 *
 * @section ABBR ABBREVIATIONS:
 *   - PM = Process Manager
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - none
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/** PM System Private Events; for internal use only by the PM module. */
#   define PM_PRIVATE_EVENTS\
   SAL_PRIVATE_EVENT(EV_TIMER_1, "Procman time base event.")\

/**
 * PM System Public Events; these can be used externally but cannot be
 * subscribed to.
 */
#   define PM_PUBLIC_EVENTS\

/**
 * PM System Published Events; published events can be subscribed to via
 * SAL_Subscribe.
 */
#   define PM_PUBLISHED_EVENTS\
   SAL_PUBLISHED_EVENT(EV_START_APP,             "Request startup of application")\
   SAL_PUBLISHED_EVENT(EV_APP_INITIALIZED,       "Application has started")\
   SAL_PUBLISHED_EVENT(EV_SHUTDOWN_APP,          "Request shutdown of application")\
   SAL_PUBLISHED_EVENT(EV_PROCESS_EXIT,          "Process exited")\
   SAL_PUBLISHED_EVENT(EV_APP_EXIT,              "Application is exiting")\
   SAL_PUBLISHED_EVENT(EV_APP_TIMEOUT,           "Procman application timed out")\
   SAL_PUBLISHED_EVENT(EV_SYSTEM_SHUTDOWN,       "Shut down system gracefully")\
   SAL_PUBLISHED_EVENT(EV_SET_SYSTEM_CONFIG,     "Change system configuration")\
   SAL_PUBLISHED_EVENT(EV_SYSTEM_CONFIG_REACHED, "System configuration reached")\
   SAL_PUBLISHED_EVENT(EV_ADD_SYSTEM_CONFIG,     "Add system configuration")\
   SAL_PUBLISHED_EVENT(EV_APP_TERMINATED,        "Event from Proc Mgr to Sys Mgr on abnormal termination of App.")\
   SAL_PUBLISHED_EVENT(EV_APP_SPAWN_SUCCESS,     "Process spawn was successful")\
   SAL_PUBLISHED_EVENT(EV_APP_SPAWN_FAIL,        "Process spawn was not successful")\
   SAL_PUBLISHED_EVENT(EV_APP_STATE,             "Report state of application")\
   SAL_PUBLISHED_EVENT(EV_GET_APP_STATE,         "Get state of application")\

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */
/*===========================================================================*/
/*!
 * @file pm_event_id.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 05-may-2009 kirk bailey
 *   - Separated private and public events.
 *
 * - 05-aug-2008 kirk bailey
 *   - Added EV_GET_APP_STATE and EV_APP_STATE.
 *
 * - 21-jul-2008 kirk bailey
 *   - Introduced use of X MACROs.
 */
/*===========================================================================*/
#endif                          /* PM_EVENT_ID_H */
