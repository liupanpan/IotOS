/* ----------------------------------------------------------------------------
 * File:          pi_process_linux.c
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent definitions of process functionality.
 * 
 * --------------------------------------------------------------------------*/

#include "pi_process.h"
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

/*///////////////////////////////////////////////////////////////////////////*/

pid_t pi_spawnvp ( int mode, const char *cmdname, char *const *argv )
{
   pid_t pid = fork ();

   if ( pid == -1 )
   {
      return -1;
   }

   if ( pid == 0 )
   {
      execv ( cmdname, argv );
      exit ( errno );
   }

   return pid;
} /* pi_spawnvp () */

/*///////////////////////////////////////////////////////////////////////////*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/

