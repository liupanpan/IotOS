/* ----------------------------------------------------------------------------
 * File:          pi_filesystem_posix.c
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   POSIX definitions of filesystem functionality.
 * 
 * --------------------------------------------------------------------------*/

#include "pi_filesystem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*///////////////////////////////////////////////////////////////////////////*/

bool_t pi_mkdir( const char* path )
{
#if defined(WIN32) && !defined(USE_CYGWIN)
   return mkdir( path ) == 0;
#else
   return mkdir( path, 0755 ) == 0;
#endif /* WIN3 */
}

/*///////////////////////////////////////////////////////////////////////////*/

bool_t pi_file_exist( const char* path )
{
	return access( path, F_OK ) == 0;
}

/*///////////////////////////////////////////////////////////////////////////*/

bool_t pi_rmdir( const char* path )
{
   return rmdir( path ) == 0;
}

/*///////////////////////////////////////////////////////////////////////////*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
