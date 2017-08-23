/* ----------------------------------------------------------------------------
 * File:          pi_unistd.h
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent 'unistd.h' functionality.
 * 
 * --------------------------------------------------------------------------*/

#ifndef PI_UNISTD_H
#define PI_UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pi_config.h"

#if defined (_MSC_VER)

/*///////////////////////////////////////////////////////////////////////////*/

#include "pi_direct.h"

extern char* pi_optarg;
extern int pi_optind;
extern int pi_opterr;
extern int pi_optopt;

int pi_getopt ( int argc, char *argv[], char *optstring );

/*///////////////////////////////////////////////////////////////////////////*/

#else

/*///////////////////////////////////////////////////////////////////////////*/

#include <unistd.h>

#define pi_optarg optarg
#define pi_optind optind
#define pi_opterr opterr
#define pi_optopt optopt

#define pi_getopt getopt
#define pi_chdir chdir

/*///////////////////////////////////////////////////////////////////////////*/

#endif /*PI_WINDOWS*/

#ifdef __cplusplus
}
#endif

#endif /*PI_UNISTD_H*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/
