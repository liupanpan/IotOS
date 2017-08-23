/* ----------------------------------------------------------------------------
 * File:          pi_direct.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent 'direct.h' functionality.
 * 
 * --------------------------------------------------------------------------*/

#ifndef PI_DIRECT_H
#define PI_DIRECT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"

#if defined(PI_WINDOWS) && !defined(USE_CYGWIN)

/*///////////////////////////////////////////////////////////////////////////*/

#include <direct.h>

#define pi_chdir _chdir

/*///////////////////////////////////////////////////////////////////////////*/

#else

/*///////////////////////////////////////////////////////////////////////////*/

#include "pi_unistd.h"

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

#ifdef __cplusplus
}
#endif

#endif /*PI_DIRECT_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
