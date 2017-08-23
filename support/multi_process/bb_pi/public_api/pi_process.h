/* ----------------------------------------------------------------------------
 * File:          pi_process.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent 'process.h' functionality.
 * 
 * --------------------------------------------------------------------------*/

#ifndef PI_PROCESS_H
#define PI_PROCESS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"

#ifdef USE_CYGWIN

/*///////////////////////////////////////////////////////////////////////////*/

#include <process.h>

#define PI_P_NOWAIT _P_NOWAIT

#define pi_spawnvp spawnvp
#define pi_getpid getpid

/*///////////////////////////////////////////////////////////////////////////*/

#elif defined(PI_WINDOWS)

/*///////////////////////////////////////////////////////////////////////////*/

#include <process.h>

#define PI_P_NOWAIT P_NOWAIT

#define pi_spawnvp _spawnvp
#define pi_getpid _getpid

/*///////////////////////////////////////////////////////////////////////////*/

#elif defined(PI_LINUX)

/*///////////////////////////////////////////////////////////////////////////*/

#include <sys/types.h>

#define PI_P_NOWAIT 1

pid_t pi_spawnvp ( int mode, const char *cmdname, char *const *argv );
#define pi_getpid getpid

/*///////////////////////////////////////////////////////////////////////////*/

#elif defined(PI_QNX)

/*///////////////////////////////////////////////////////////////////////////*/

#include <process.h>

#define PI_P_NOWAIT P_NOWAIT

#define pi_spawnvp spawnvp
#define pi_getpid getpid

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

#ifdef __cplusplus
}
#endif

#endif /*PI_PROCESS_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * - 16-Jul-2013 Larry Piekarski Rev 3
 *   - Task 160909: Fixed -Wundef warnings for Linux
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
