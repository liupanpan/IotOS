/* ----------------------------------------------------------------------------
 * File:          pi_stdio.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent 'stdio.h' functionality.
 *
 * --------------------------------------------------------------------------*/

#ifndef PI_STDIO_H
#define PI_STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"
#include <stdio.h>

/*///////////////////////////////////////////////////////////////////////////*/

#ifdef PI_WINDOWS

/*///////////////////////////////////////////////////////////////////////////*/

#define pi_snprintf _snprintf

/*///////////////////////////////////////////////////////////////////////////*/

#else

/*///////////////////////////////////////////////////////////////////////////*/

#define pi_snprintf snprintf

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

#ifdef __cplusplus
}
#endif

#endif /*PI_STDIO_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
