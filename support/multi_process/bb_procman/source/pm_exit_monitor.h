/*--------------------------------------------------------------------------
 * File:      pm_exit_monitor.h
 *
 * Copyright: 2004 Delphi Technologies, Inc., All Rights Reserved
 *            Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Michal Gebolys / Lukasz Ptak
 *
 * Description:    Definition of the Exit Monitor Thread.
 *
 *--------------------------------------------------------------------------*/

#ifndef PM_EXIT_MONITOR_H
#define PM_EXIT_MONITOR_H

#include "xsal.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/**
 *  Event sent when an process terminates.
 */
typedef struct Ev_Process_Exit_Tag
{
      int pid;
      int return_code;
} Ev_Process_Exit_T;

#define Get_PID( msg ) (((Ev_Process_Exit_T*)(((SAL_Message_T*)msg)->data))->pid)
#define Get_Process_Return_Code( msg ) (((Ev_Process_Exit_T*)(((SAL_Message_T*)msg)->data))->return_code)

#if defined (QNX_NTO) || defined (LINUX)
/**
 *  Implementation of thread waiting for child processes to exit
 */
void PM_Exit_Monitor_Thread( void *param );
#endif /* QNX_NTO || LINUX */

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */

#endif /* PM_EXIT_MONITOR_H */

/*-----------------------------------------------------------------------------
 * History
 *
 * 25 Sep. 2006, Lukasz Ptak:     Port to C.
 * 27 Aug. 2004, Michal Gebolys:  Initial revision.
 *-----------------------------------------------------------------------------*/
