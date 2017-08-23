/*===========================================================================*/
/**
 * @file gen_xsal_msg_list.c
 *
 *   Generates a decode csv file of XSAL Message descriptions
 *
 * %full_filespec:gen_xsal_msg_list.c~kok_basa#13:csrc:kok_aud#1 %
 * @version %version:kok_basa#13 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Tue Aug 18 15:28:35 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008-2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - CNTS = Count Service
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
 * @ingroup test
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
 
#include "reuse.h" 
#include "xsal.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
/* EM_FILENUM(gen_xsal_msg_list_ID, 2); */  /* Identifies file for PbC/trace */

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

#define OUTPUT_FILE "xsal_msgs.csv" 

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

#   undef SAL_PRIVATE_EVENT
#   undef SAL_PUBLIC_EVENT
#   undef SAL_PUBLISHED_EVENT
#   undef SAL_START_RANGE
#   undef SAL_END_RANGE

#   define SAL_PRIVATE_EVENT(id,descr)   {id, #id, "Private :", descr},
#   define SAL_PUBLIC_EVENT(id,descr)    {id, #id, "Public  :", descr},
#   define SAL_PUBLISHED_EVENT(id,descr) {id, #id, "Publish :", descr},
#   define SAL_START_RANGE(range)        { SAL_EV_FIRST_##range, "Start " #range, "", ""},
#   define SAL_END_RANGE(range, reserve) { SAL_EV_LAST_##range, "End " #range, "", ""},

typedef struct desc_table_Tag
{
   SAL_Event_Id_T id;
   const char * enum_name;
   const char * event_type;
   const char * desc;
} desc_table_T;  

desc_table_T msg_descriptions[] =
{
   {SAL_EVG_FIRST_PUBLISHED_EVENT_ID, "SAL_EVG_FIRST_PUBLISHED_EVENT_ID", "Reserved", "First Published Event ID"},

   SAL_PUBLISHED_EVENTS
  
   {SAL_EVG_LAST_PUBLISHED_EVENT_ID, "SAL_EVG_LAST_PUBLISHED_EVENT_ID", "Reserved", "Last Published Event ID"},

   SAL_PUBLIC_EVENTS
   
   {SAL_EV_FIRST_PRIVATE_ID, "SAL_EV_FIRST_PRIVATE_ID", "Reserved", "First Private Event ID"},

   SAL_PRIVATE_EVENTS
};

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/*---------------------------------------------------------------------------*
 * Test implementation of VIP interface callouts
 *---------------------------------------------------------------------------*/

int main(void)
{
   SAL_Event_Id_T i;
   FILE * f;

   fprintf(stdout, "Generating XSAL Message List : <%s>...\n", OUTPUT_FILE);
   f = fopen(OUTPUT_FILE, "w+");
   if ( NULL == f )
   {
      (void) chmod(OUTPUT_FILE, 0777);
      (void) remove(OUTPUT_FILE);
      f = fopen(OUTPUT_FILE, "w+");
   }

   if ( NULL != f )
   {
      fprintf(f, "'ID, ID,SAL_Event_Id_T,Type,Event Description\n");

      for (i = 0; i < Num_Elems(msg_descriptions); i++)
      {
         fprintf(f, "0x%X, %d, %s, %s, %s\n", msg_descriptions[i].id, msg_descriptions[i].id, msg_descriptions[i].enum_name, msg_descriptions[i].event_type, msg_descriptions[i].desc);
      }

      fclose(f);
      fprintf(stdout, "...Done!\n");
   }
   else
   {
      fprintf(stderr, "Open of %s failed: %d %s\n", OUTPUT_FILE, errno, strerror(errno));
   }

   return 0;
}

/*===========================================================================*/
/*!
 * @file gen_xsal_msg_list.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 16-Sept-2008 Dan Carman
 *   - Created initial file.
 *
 * - 17-Oct-2011 Dan Carman
 *   - Incorporate recommendations
 *   - Added column for event type instead of adding to description.
 *
 * - 27-May-2015 Kirk Bailey
 *   - Filled in unresolved comments.
 *
 * - 12-Jun-2015 Dan Carman
 *   Modified to handle non-sequential event ID  values
 *
 * - 18-Aug-2015 Kirk Bailey
 *   - Fixed Coverity issues.
 */
/*===========================================================================*/
/** @} doxygen end group */


