/* ----------------------------------------------------------------------------
 * File:          pi_stdlib.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent 'stdlib.h' functionality.
 *
 * --------------------------------------------------------------------------*/

#ifndef PI_STDLIB_H
#define PI_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"
#include <stdlib.h>

/*///////////////////////////////////////////////////////////////////////////*/

/**
 * pi_atoi function converts the initial portion of the string pointed
 *    to by nptr to int.
 *
 * @param [in] nptr string to be converted
 *
 * @note The string must begin with an arbitrary amount of white space
 *   followed by a single optional '+' or '-' sign.
 *
 * @param [out] value converted value.
 *
 * @return an error code interpreted as a value of boolean type.
 *
 * @pre nptr != NULL
 * @pre value != NULL
 */

int pi_atoi ( const char* nptr, int* value );

/*///////////////////////////////////////////////////////////////////////////*/

#ifdef PI_WINDOWS

/*///////////////////////////////////////////////////////////////////////////*/

#define PI_MAX_PATH _MAX_PATH

/*///////////////////////////////////////////////////////////////////////////*/

#else

/*///////////////////////////////////////////////////////////////////////////*/

#include <limits.h>

#define PI_MAX_PATH PATH_MAX

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

#ifdef __cplusplus
}
#endif

#endif /*PI_STDLIB_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
