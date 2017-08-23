/* ----------------------------------------------------------------------------
 * File:          pi_filesystem_win32.c
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   WIN32 definitions of filesystem functionality.
 * 
 * --------------------------------------------------------------------------*/

#include "pi_filesystem.h"
#include <windows.h>
#include <io.h>

#ifdef USE_CYGWIN
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif /* USE_CYGWIN */


/*///////////////////////////////////////////////////////////////////////////*/

bool_t pi_mkdir( const char* path )
{
	return (bool_t)CreateDirectoryA( path, 0 );
}

/*///////////////////////////////////////////////////////////////////////////*/

bool_t pi_rmdir( const char* path )
{
   return (bool_t)RemoveDirectoryA( path );
}

/*///////////////////////////////////////////////////////////////////////////*/

bool_t pi_file_exist( const char* path )
{
#ifdef USE_CYGWIN
	return access( path, F_OK ) == 0;
#else
	return (bool_t)(_access( path, 0 ) == 0);
#endif /* USE_CYGWIN */
}

/*///////////////////////////////////////////////////////////////////////////*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/

