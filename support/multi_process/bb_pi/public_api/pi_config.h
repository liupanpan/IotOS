/* ----------------------------------------------------------------------------
 * File:          pi_config.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Configuration file for platform independent functionality.
 * 
 * --------------------------------------------------------------------------*/

#ifndef PI_CONFIG_H
#define PI_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

/*///////////////////////////////////////////////////////////////////////////*/

#define PI_WINDOWS (1)

/*///////////////////////////////////////////////////////////////////////////*/

#elif defined(LINUX)

/*///////////////////////////////////////////////////////////////////////////*/

#define PI_LINUX   (1)
#define PI_POSIX   (1)

/*///////////////////////////////////////////////////////////////////////////*/

#elif defined(__QNX__)

/*///////////////////////////////////////////////////////////////////////////*/

#define PI_QNX     (1)
#define PI_POSIX   (1)

/*///////////////////////////////////////////////////////////////////////////*/

#else

/*///////////////////////////////////////////////////////////////////////////*/

#error "Not supported platform yet"

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*WIN32*/

#ifdef __cplusplus
}
#endif

#endif /*PI_CONFIG_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * - 16-Jul-2013 Larry Piekarski Rev 4
 *   - Task 160909: Fixed -Wundef warnings for Linux
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
