/* ----------------------------------------------------------------------------
 * File:          pi_limits.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent additional 'limits.h' functionality.
 *
 * --------------------------------------------------------------------------*/

#ifndef PI_LIMITS_H
#define PI_LIMITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"
#include <limits.h>

#ifdef PI_WINDOWS

/*///////////////////////////////////////////////////////////////////////////*/

#include <stdlib.h>

#define PI_NAME_MAX _MAX_FNAME
#define PI_PATH_MAX _MAX_PATH

/*///////////////////////////////////////////////////////////////////////////*/

#else

/*///////////////////////////////////////////////////////////////////////////*/

#define PI_NAME_MAX NAME_MAX
#define PI_PATH_MAX PATH_MAX

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

#ifdef __cplusplus
}
#endif

#endif /*PI_LIMITS_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
