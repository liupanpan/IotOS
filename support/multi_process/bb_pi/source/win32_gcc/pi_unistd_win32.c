/* ----------------------------------------------------------------------------
 * File:          pi_unistd_win32.c
 *
 * Copyright:     2006 Delphi Technologies, Inc., All Rights Reserved
 *                Information Contained Herein Is Proprietary and Confidential
 *
 * Author:        Dariusz Kubiczek
 *
 * Description:   Platform independent 'unistd.h' functionality.
 * 
 * --------------------------------------------------------------------------*/
#ifndef USE_CYGWIN

#include "pi_unistd.h"
#include <stdio.h>
#include <string.h>

/*///////////////////////////////////////////////////////////////////////////*/

/*
 *  Copyright (C) 1994 Arno Schaefer
 *
 *  AU: Auswertung der Kommandozeile, der POSIX-Version von getopt ()
 *	nachempfunden.
 *
 *  PO: ANSI C
 */

/* Globale Variablen */

char* pi_optarg;
int pi_optind = 1;
int pi_opterr = 1;
int pi_optopt;

static char *nextarg = NULL;


/* Funktion  */

int pi_getopt (int argc, char *argv[], char *optstring)

/*
 *  AU: Auswertung der Kommandozeile
 *
 *  VB: argc und argv sind die Parameter, die an main () uebergeben werden.
 *	optstring ist ein String, der die Zeichen enthaelt, die als
 *	Optionen erkannt werden. Wenn ein Zeichen von einem Doppelpunkt
 *	gefolgt wird, hat die Option ein Argument, das direkt auf das Zeichen
 *	folgt oder durch Space davon getrennt ist. Gueltige Optionszeichen
 *	sind alle druckbaren Zeichen ausser '?', ' ' und ':'.
 *
 *	optind ist der Index auf das naechste Element von argv[], das
 *	bearbeitet wird.
 *
 *	opterr ist ein Flag, das festlegt, ob bei Fehlern Fehlermeldungen
 *	ausgegeben werden.
 *
 *	optarg ist ein Zeiger auf das Argument, wenn eine Option ein
 *	Argument hat.
 *
 *	optopt enthaelt bei Fehlern das Optionszeichen, das den Fehler aus-
 *	geloest hat.
 *
 *  NB: Rueckgabewert ist das jeweils naechste Optionszeichen, oder -1 am
 *	Ende der Optionsliste.
 *
 *	Die Optionsliste ist zu Ende, wenn argv[optind] NULL ist, oder
 *	argv[optind] nicht mit '-' (oder '/') beginnt, oder argv[optind]
 *	ein einzelnes "-" ist. In diesem Fall wird optind nicht erhoeht.
 *	Das Ende der Optionsliste kann mit "--" erzwungen werden, dann ist
 *	argv[optind] das erste Argument nach "--".
 *
 *  FB: Ein '?' wird zurueckgegeben, wenn ein Optionszeichen nicht in
 *	optstring enthalten war oder ein ungueltiges Optionszeichen
 *	uebergeben wurde ('?' oder ':'). Ausserdem bei einem fehlenden
 *	Argument, wenn das erste Zeichen von optstring kein ':' ist.
 *
 *	Ein ':' wird zurueckgegeben bei einem fehlenden Argument, wenn
 *	das erste Zeichen von optstring ein ':' ist.
 */

{
	char *search;

	pi_optarg = NULL;
	
	if (nextarg == NULL)
	{
		nextarg = argv[pi_optind];

		if (nextarg == NULL)
		{
			return (-1);
		}

		if (*nextarg != '-')
		{
			return (-1);
		}

		nextarg++;

	} /* if */

	pi_optopt = *nextarg++;

	if (pi_optopt == 0)
	{
		return (-1);
	}

	pi_optind++;

	if (pi_optopt == '-' && *nextarg == 0)
	{
		return (-1);
	}

	if (pi_optopt == ':' || pi_optopt == '?')
	{
		if (pi_opterr)
		{
			fprintf
			(
				stderr,
				"%s: illegal option -- %c\n",
				argv[0],
				pi_optopt
			);
		}

		return ('?');

	} /* if */

	search = strchr (optstring, pi_optopt);

	if (search == NULL)
	{
		if (pi_opterr)
		{
			fprintf
			(
				stderr,
				"%s: illegal option -- %c\n",
				argv[0],
				pi_optopt
			);
		}

		return ('?');

	} /* if */

	if (*nextarg == 0)
	{
		nextarg = NULL;
	}

	if (search[1] != ':')
	{
		if (nextarg != NULL)
		{
			pi_optind--;
		}

		return (pi_optopt);
	}

	if (nextarg != NULL)
	{
		pi_optarg = nextarg;
		nextarg = NULL;
		return (pi_optopt);
	}

	pi_optarg = argv[pi_optind];

	if (pi_optind == argc)
	{
		if (pi_opterr)
		{
			fprintf
			(
				stderr,
				"%s: option requires an argument -- %c\n",
				argv[0],
				pi_optopt
			);

		} /* if */

		if (optstring[0] == ':')
		{
			return (':');
		}
		else
		{
			return ('?');
		}

	} /* if */

	else
	{
		pi_optind++;
	}

	return (pi_optopt);

} /* getopt () */

#endif /* #ifndef USE_CYGWIN */

/*///////////////////////////////////////////////////////////////////////////*/

/* ----------------------------------------------------------------------------
 * Revision history:
 *
 * 27 September 2006, Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *   Initial revision.
 *
 * --------------------------------------------------------------------------*/

