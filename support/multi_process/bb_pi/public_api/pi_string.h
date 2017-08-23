/* ----------------------------------------------------------------------------
 * File:          pi_string.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent 'string.h' functionality.
 *
 * --------------------------------------------------------------------------*/

#ifndef PI_STRING_H
#define PI_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"

/*///////////////////////////////////////////////////////////////////////////*/

#ifdef PI_WINDOWS

#include <string.h>

/*///////////////////////////////////////////////////////////////////////////*/

#define pi_strcmpi _strcmpi

/*///////////////////////////////////////////////////////////////////////////*/

#else

#include <strings.h>

/*///////////////////////////////////////////////////////////////////////////*/

#define pi_strcmpi strcasecmp

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

#ifdef __cplusplus
}
#endif

#endif /*PI_STRING_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
