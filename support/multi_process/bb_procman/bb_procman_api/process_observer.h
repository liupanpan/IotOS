#ifndef PROCESS_OBSERVER_API_H
#   define PROCESS_OBSERVER_API_H
/*===========================================================================*/
/**
 * @file process_observer.h
 *
 * @brief Header file for Process Observation Library (POL) interface
 *
 * %full_filespec:process_observer.h~2:incl:kok_basa#1 %
 * @version %version:2 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed May 27 14:51:00 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009-2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 * This file conatin the APIs and public types of Process Observation Library (POL)
 *
 * ABBREVIATIONS:
 *   - None
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @ingroup XSAL
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#ifndef WIN32
#include <unistd.h>
#endif
#include "xsal.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/
/*
 * The QNX dumper Enable/Disable switch is used as POL switch too. When QNX
 * dumper is enabled (/home/root/dumper_yes) then POL will be disabled.
 */
#define POL_SWITCH  "/home/root/POL_yes"

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * (h) Exported object declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/
/*Data struct for sending information of crashed App, through SAL message*/
typedef struct
{
   int32_t pid;
   int32_t fltip;
   int32_t signo;
   int32_t tid;
   int32_t appid;
   char    pathbuffer[PATH_MAX + 1];
}
POL_Proc_Termination_Msg;
/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/**
 *  API to start the Process Observer thread. Called by Process manager.
 *
 *  @return
 *     - none
 *
 */
void POL_Start_Process_Observer(void);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/
/**
 *  Checks if the Process Oberver needs to be started or not. This option helps
 *  users to enable/disable the POL. When QNX dumper needs be to run then POL
 *  will be disabled.
 *
 *  @param none
 */

#define Is_POL_Enabled() ( 0 == access(POL_SWITCH, F_OK) )

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 08-Oct-2009 Prince-TCI
 * + Created initial file.
 *
 * 27-May-2015 Kirk Bailey
 * + Filled in unresolved comments.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* PROCESS_OBSERVER_API_H */
