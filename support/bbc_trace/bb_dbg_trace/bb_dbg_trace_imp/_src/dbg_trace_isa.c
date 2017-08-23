/*===========================================================================*/
/**
 * @file dbg_trace_isa.c
 *
 *   Implements debugtrace functions that map ISA debugtrace to BASA dbg_trace.
 *
 * %full_filespec: dbg_trace_isa.c~2:csrc:kok_inf#1 %
 * @version %version: 2 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Mon May 11 15:56:16 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR = TRace
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
 *   - none
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "dbg_trace_cbk.h"
#include "debugtrace.h"
#include <stdlib.h>
#include <string.h>
#include "string_resource.h"
#include <unistd.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
#define DT_MAX_APP_NAME 64

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

static char App_Name[DT_MAX_APP_NAME];
static uint64_t Group_Mask;
static int Logger_Disable;
static int Verbosity_Level;

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

void Dt_Set_App_Name(const char *name)
{
   if (name != NULL)
   {
      Str_Full_Copy(App_Name, name, DT_MAX_APP_NAME);
   }
}

static void Dt_Getopt(int argc, char const* argv[])
{
   char const* opt_arg = 0;
   int i;
   long long unsigned int llmask;

   for (i = 1; i < argc; ++i)
   {
      if ((argv[i][0] == '-') && (strlen(argv[i]) > 1))
      {
         if (argv[i][2] != '\0')
         {
            opt_arg = argv[i] + 2;
         }
         else if (i < (argc - 1))
         {
            opt_arg = argv[i + 1];
         }
         switch (argv[i][1])
         {
            case 'g':
               if (opt_arg != 0)
               {
#if defined (_MSC_VER)
                  sscanf( opt_arg, "%I64x", &llmask );
#else
                  sscanf(opt_arg, "%llx", &llmask);
#endif
                  Group_Mask = llmask;
               }
               break;

            case 'v':
               if (opt_arg != 0)
               {
                  Verbosity_Level = atoi(opt_arg);
               }
               break;

            case 'N':
               Dt_Set_App_Name(opt_arg);
               break;

            case 'e':
               Logger_Disable = 1;
               break;

            default:
               /* "Unrecognized option" */
               break;
         } /* switch */
         opt_arg = 0;
      }
   }
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Destroy(void)
{
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Disable(void)
{
   Logger_Disable = 1;
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Enable(void)
{
   Logger_Disable = 0;
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Get_Settings(DT_Settings_T *settings)
{
   if (settings != NULL)
   {
      settings->group_mask = Group_Mask;
      settings->verbosity_level = Verbosity_Level;
      settings->logger_enable = Logger_Disable ? 0 : 1;
      settings->time_stamp_enable = 0;
   }
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Init(const char *app_name, uint64_t grp, int level, int time_stamp, int argc, char const* argv[])
{
   Group_Mask = grp;
   Verbosity_Level = level;
   Dt_Set_App_Name(app_name);
   Dt_Getopt(argc, argv);
   Tr_Inf(grp, "%s: [pid: %d] - Logger activated. Group mask = %llx, debug level = %d",
          app_name, getpid(), (unsigned long long) Group_Mask, Verbosity_Level);
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Set_Group(uint64_t grp)
{
   Group_Mask = grp;
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Set_Level(int level)
{
   Verbosity_Level = level;
}

/*===========================================================================*
 *
 * Please refer to the detailed description in debugtrace.h
 *
 *===========================================================================*/
void Tr_Trace(uint64_t group, FILE* fp, Tr_Trace_Level_T lvl, uint16_t f_num, char_t const *f_name, uint16_t line_num,
   char_t const *msg, ...)
{
   bool_t is_enabled = true;
   /*
    * These trace messages are disabled if either: 1) The corresponding bit in
    * the group mask is not set, or 2) The verbosity of the message is greater
    * than that enabled.
    */
   if (lvl >= TR_LVL_NOTIFY)
   {
      if (( (group & Group_Mask) == 0 ) || ( Verbosity_Level < (lvl - TR_LVL_NOTIFY) ))
      {
         is_enabled = false;
      }
   }

   if (is_enabled)
   {
      va_list arg_ptr;
      Tr_Compact_Data_T tr_data;

      tr_data.trace_level = lvl;
      tr_data.file_id     = f_num;
      tr_data.line_num    = line_num;
      tr_data.timestamp   = Tr_Clock();
      tr_data.thread_id   = Tr_Get_Thread_Id();
      tr_data.app_id      = Tr_Get_App_Id();
#if defined(DBG_USE_THREAD_CPU_TIME) && (DBG_USE_THREAD_CPU_TIME==true)
      tr_data.thr_cpu_time = Tr_Get_Thread_CPU_Time();
#else
      tr_data.thr_cpu_time = ((Tr_Clock_T) {0, 0});
#endif
      va_start(arg_ptr, msg);
      Tr_Store_Verbose_Data(fp, &tr_data, f_name, msg, arg_ptr);
      va_end(arg_ptr);
   }
}

/*===========================================================================*/
/*!
 * @file dbg_trace_isa.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 11-May-2015 Kirk Bailey
 *   - Created file from version 120_1428 of AP_DebugTrace
 *
 * - 11-May-2015 Kirk Bailey
 *   - Made argv const.
 */
/*===========================================================================*/
