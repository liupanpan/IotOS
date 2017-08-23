/* ----------------------------------------------------------------------------
 * File:          pi_stdlib.c
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent additional 'stdlib.h' functionality.
 * 
 * --------------------------------------------------------------------------*/

#include "pi_stdlib.h"
#include <assert.h>
#include <limits.h>
#include <errno.h>

/*///////////////////////////////////////////////////////////////////////////*/

int pi_atoi ( const char* nptr, int* value )
{
   int status = 0;
   long lvalue = 0;
   char* endptr = NULL;

   assert ( nptr != NULL );
   assert ( value != NULL );

   lvalue = strtol ( nptr, &endptr, 10 );
   if ( lvalue > INT_MAX || lvalue < INT_MIN ||
      *nptr == '\0' || endptr == nptr )
   {
      status = 0;
   }
   else
   {
      status = 1;
      *value = (int) (lvalue);
   }

   return status;
} /* pi_atoi () */

/*///////////////////////////////////////////////////////////////////////////*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
