/*===========================================================================*/
/**
 * @file xsal_event_names.c
 *
 *   Provide names of XSAL events for dbg_trace purposes.
 *
 * %full_filespec:xsal_event_names.c~6:csrc:kok_basa#1 %
 * @version %version:6 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Feb 10 09:31:16 2016 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "xsal.h"
#include <stdlib.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/
#ifndef XSAL_DISABLE_EVENT_NAMES

#undef SAL_EVENT
#undef SAL_START_RANGE
#undef SAL_END_RANGE

#define SAL_EVENT(id,descr) {id, #id},
#define SAL_START_RANGE(range)
#define SAL_END_RANGE(range, reserve)

typedef struct XSAL_NAMES_TAG
{
      SAL_Event_Id_T id;
      char_t * name;
} XSAL_NAMES_T;

/**
 * List of event names.
 */
static const XSAL_NAMES_T SAL_Event_Names[] =
{
   SAL_PUBLISHED_EVENTS
   SAL_PUBLIC_EVENTS
   SAL_PRIVATE_EVENTS
};

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *==========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

static int cmpfunc(const void * a, const void * b)
{
   return ( ((XSAL_NAMES_T const *)a)->id - ((XSAL_NAMES_T *)b)->id );
}

/*
 * Please refer to comment in xsal.h
 */
char_t const *SAL_Get_Event_Name(SAL_Event_Id_T event_id)
{
   char_t const *name = "UNKNOWN_EVENT";
   XSAL_NAMES_T key = {event_id, ""};
   XSAL_NAMES_T * match;

   match = (XSAL_NAMES_T *) bsearch(&key, SAL_Event_Names, Num_Elems(SAL_Event_Names), sizeof(XSAL_NAMES_T), cmpfunc);

   if (match)
   {
      name = match->name;
   }

   return name;
}

#else
/*
 * Please refer to comment in xsal.h
 */
char_t const *SAL_Get_Event_Name(SAL_Event_Id_T event_id)
{
   return "XSAL_EVENT";
}
#endif


/*===========================================================================*/
/*!
 * @file xsal_event_names.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 07-May-2012 Kirk Bailey
 *   - Created initial file.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * 12-Jun-2015 Dan Carman
 *  - Task kok_basa#213156 - Don't assume events IDs are contiguous (i.e., that index = ID value)
 *       Add support for optional SAL RANGE macros 
 * 
 * 10-Feb-2016 Dan Carman
 *    Place table into constant memory.
 */
/*===========================================================================*/
