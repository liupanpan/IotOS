/* ----------------------------------------------------------------------------
 * File:          pi_filesystem.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent filesystem functionality.
 * 
 * --------------------------------------------------------------------------*/

#ifndef PI_FILESYSTEM_H
#define PI_FILESYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"
#include "reuse.h"

#ifdef PI_WINDOWS

/*///////////////////////////////////////////////////////////////////////////*/

#define PI_PATH_SEP '\\'
#define PI_PATH_SEP_STR "\\"

/*///////////////////////////////////////////////////////////////////////////*/

#else

/*///////////////////////////////////////////////////////////////////////////*/

#define PI_PATH_SEP '/'
#define PI_PATH_SEP_STR "/"

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

bool_t pi_mkdir( const char* path );
bool_t pi_rmdir( const char* path );
bool_t pi_file_exist( const char* path );

#ifdef __cplusplus
}
#endif

#endif /*PI_FILESYSTEM_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
