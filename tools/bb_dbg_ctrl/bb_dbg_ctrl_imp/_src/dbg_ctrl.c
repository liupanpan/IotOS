/*===========================================================================*/
/**
 * @file dbg_ctrl.c
 *
 * Program to monitor and control XSAL applications' debug settings at run time.
 *
 * %full_filespec:dbg_ctrl.c~26:csrc:kok_basa#1 %
 * @version %version:26 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon May  9 16:47:39 2016 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2014 Delphi Technologies, Inc., All Rights Reserved.
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
 *   - none
 *
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include <assert.h>
#include "dbg_trace.h"
#include "dbg_trace_cfg.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include "xsal.h"
#include "xsal_i_dt.h"
#include "xsal_i_ns_proxy.h"

#if defined (XSAL_POSIX)

#   include <errno.h>
#   include <pthread.h>
#   include <stdlib.h>
#   include <string.h>
#   if defined(LINUX)
#      include <sched.h>
#      include <sys/resource.h>
#   endif
#   include <unistd.h>

#endif /* XSAL_POSIX */

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

/**
 * Maximum length of dynamically-generated format string.
 */
#define FORMAT_STR_LEN_MAX 80

/**
 * Maximum length of dynamically-generated priority string.
 */
#define PRIO_STR_LEN_MAX 5

/**
 * Maximum period (seconds) for -r (repeat) option.
 */
#define MAX_REPEAT_PERIOD_SEC 20

/**
 * Number of string pointers to dynamically allocate whenever more are needed.
 */
#define NUM_STR_TO_ALLOC 16

#ifndef PATH_MAX
/**
 * Maximum length of file path (if not defined by system).
 */
#   define PATH_MAX 1024
#endif

/**
 * Maximum length of thread name placed into output.
 */
#define THREAD_NAME_LEN_MAX 32

/**
 * Base priority of the real-time scheduler
 */
#ifdef SAL_REAL_TIME_PRIORITY
#   define SAL_REAL_TIME_PRIORITY_BASE SAL_REAL_TIME_PRIORITY
#else
#   define SAL_REAL_TIME_PRIORITY_BASE 0
#endif

#if defined(WIN32)
#define PRIORITY_SUPPORTED 0
#else
#define PRIORITY_SUPPORTED 1
#endif

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/**
 * This structure defines all of the data used to run the dbg_ctrl application.
 */
typedef struct Cmd_Opt_Tag
{
   /**
    * Indicates whether the corresponding XSAL App is running. This array is
    * dynamically allocated and initialized by communicating with the name
    * server.
    */
   bool_t *app_is_running;
   /**
    * Names of the corresponding XSAL Apps. This array is dynamically allocated
    * and initialized by communicating with the name server.
    */
   char **app_names;
   /**
    * Used to implement the "-d" option to establish a default Debug Module
    * trace level. Value is -1 unless the -d option is used to set it to the
    * default level.
    */
   int16_t default_module_level;
   /**
    * Used to only print headers, etc. on the first pass (when -r option is used).
    */
   bool_t first_pass;
   /**
    * Names of the corresponding Debug Modules. This array is dynamically
    * allocated and initialized by communicating with the name server.
    */
   char **module_names;
   /**
    * True if a connection to the Name Server has been made. If false, the
    * program runs in a degraded performance mode that supports limited
    * functionality.
    */
   bool_t ns_connection_is_ok;
   /**
    * Number of entries in app_is_running and app_names arrays. Value is
    * determined at run time by querying the name server.
    */
   size_t num_app_ids;
   /**
    * Number of entries in requested_fault_lvl_modules array. This is determined
    * based on the value of the "-f" command line option.
    */
   size_t num_fault_lvl_modules;
   /**
    * Number of entries in requested_hi_lvl_modules array. This is determined
    * based on the value of the "-h" command line option.
    */
   size_t num_hi_lvl_modules;
   /**
    * Number of entries in requested_lo_lvl_modules array. This is determined
    * based on the value of the "-l" command line option.
    */
   size_t num_lo_lvl_modules;
   /**
    * Number of entries in requested_lo_lvl_modules array. This is determined
    * based on the value of the "-m" command line option.
    */
   size_t num_mid_lvl_modules;
   /**
    * Number of entries in module_names array. Value is determined at run time
    * by querying the name server.
    */
   size_t num_module_ids;
   /**
    * Number of entries in requested_notify_lvl_modules array. This is determined
    * based on the value of the "-y" command line option.
    */
   size_t num_notify_lvl_modules;
   /**
    * Number of entries in selected_app_ids array. This value is determined at
    * run time based on the "-n" command line argument.
    */
   size_t num_selected_apps;
   /**
    * Number of entries in requested_warn_lvl_modules array. This is determined
    * based on the value of the "-w" command line option.
    */
   size_t num_warn_lvl_modules;
   /**
    * If "-a" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-a" option was entered.
    */
   char *opt_a_optarg;
   /**
    * If "-b" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-b" option was entered.
    */
   char *opt_b_optarg;
   /**
    * If "-f" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-f" option was entered.
    */
   char *opt_f_optarg;
   /**
    * If "-h" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-h" option was entered.
    */
   char *opt_h_optarg;
   /**
    * If "-l" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-l" option was entered.
    */
   char *opt_l_optarg;
   /**
    * If "-m" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-m" option was entered.
    */
   char *opt_m_optarg;
   /**
    * If "-n" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-n" option was entered.
    */
   char *opt_n_optarg;
   /**
    * If "-w" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-w" option was entered.
    */
   char *opt_w_optarg;
   /**
    * If "-y" option is entered, this is a copy of its sub-options, which is
    * parsed later to execute that command. NULL if no "-y" option was entered.
    */
   char *opt_y_optarg;
   /**
    * True if "-p" option was entered to cause debug module levels to be printed.
    */
   bool_t print_dbg_levels;
   /**
    * If "-p" option was entered, this array indicates which of the debug trace
    * levels are to have their modules printed.
    */
   bool_t print_level[TR_LVL_INFO_LO + 1];
   /**
    * Used to implement the "-q" option, which specifies which XSAL thread queues
    * are to have their status displayed. A value of -1 means no "-q" option was
    * specified, a value of 0 means display all thread queues, and a value 1..n
    * means display only that queue.
    */
   int16_t queue_id;
   /**
    * if "-r" option was entered, this is the delay (sec) between repeating
    * the status printout.
    */
   int32_t repeat_sec;
   /**
    * Used to implement the "-f" option, which specifies the list of debug
    * module ids whose levels are to be set to TR_LVL_FAULT. Each entry in
    * the array is a debug module id that is to have this level.
    */
   uint16_t *requested_fault_lvl_modules;
   /**
    * Used to implement the "-h" option, which specifies the list of debug
    * module ids whose levels are to be set to TR_LVL_INFO_HI. Each entry in
    * the array is a debug module id that is to have this level.
    */
   uint16_t *requested_hi_lvl_modules;
   /**
    * Used to implement the "-l" option, which specifies the list of debug
    * module ids whose levels are to be set to TR_LVL_INFO_LO. Each entry in
    * the array is a debug module id that is to have this level.
    */
   uint16_t *requested_lo_lvl_modules;
   /**
    * Used to implement the "-m" option, which specifies the list of debug
    * module ids whose levels are to be set to TR_LVL_INFO_MID. Each entry in
    * the array is a debug module id that is to have this level.
    */
   uint16_t *requested_mid_lvl_modules;
   /**
    * Used to implement the "-y" option, which specifies the list of debug
    * module ids whose levels are to be set to TR_LVL_NOTIFY. Each entry in
    * the array is a debug module id that is to have this level.
    */
   uint16_t *requested_notify_lvl_modules;
   /**
    * Used to implement the "-w" option, which specifies the list of debug
    * module ids whose levels are to be set to TR_LVL_WARN. Each entry in
    * the array is a debug module id that is to have this level.
    */
   uint16_t *requested_warn_lvl_modules;
   /**
    * If true (-x option), causes the app message counts (-c option) to be
    * reset (cleared) every time they are re-acquired.
    */
   bool_t reset_msg_counts;
   /**
    * Used to implement the "-n" option, which selects the list of XSAL apps
    * which are to be configured/interrogated. Each entry in the list is an
    * XSAL application id.
    */
   uint16_t *selected_app_ids;
   /**
    * When true (-c option), show message count statistics for the selected
    * XSAL applications.
    */
   bool_t show_msg_counts;
   /**
    * When true (-t option), show timer count statistics for the selected
    * XSAL applications.
    */
   bool_t show_timer_counts;
  /**
    * True if the -s option has been specified to allow access to information
    * about an XSAL app that is not currently running.
    */
   bool_t static_run_enabled;
} Cmd_Opt_T;

/**
 * This structure is used in place of DT_Dbg_Levels_T so as to avoid having
 * this program depend on the value of TR_NUM_MODULE_IDS at compile time.
 * By instead using this type as a template for dynamically allocating a
 * structure of the appropriate size, this program can work properly with
 * any XSAL application - even those built with different values of
 * TR_NUM_MODULE_IDS.
 */
typedef struct Dbg_Ctrl_Dbg_Levels_Tag
{
   size_t num_levels;
   uint8_t levels[1];
} Dbg_Ctrl_Dbg_Levels_T;

/**
 * This structure is used in place of DT_Set_Dbg_Levels_T so as to avoid having
 * this program depend on the value of TR_NUM_MODULE_IDS at compile time.
 * By instead using this type as a template for dynamically allocating a
 * structure of the appropriate size, this program can work properly with
 * any XSAL application - even those built with different values of
 * TR_NUM_MODULE_IDS.
 */
typedef struct Dbg_Ctrl_Set_Dbg_Levels_Tag
{
   DT_Command_Ids_T command;
   Dbg_Ctrl_Dbg_Levels_T dbg_levels;
} Dbg_Ctrl_Set_Dbg_Levels_T;

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

#undef TR_MODULE_LEVEL
#undef TR_MODULE_LEVEL_EQ
/* PRQA S 3410 10 */
/**
 * X-Macro to defining enumeration for a module ID
 */
#define TR_MODULE_LEVEL(ID, init_lvl) init_lvl,
#define TR_MODULE_LEVEL_EQ(ID, id_value, init_lvl) init_lvl,

/**
 * Keeps track of the run-time trace level for each of the modules in the
 * system.
 */
static Tr_Trace_Level_T Default_Trace_Levels[TR_NUM_MODULE_IDS] =
{ TR_DEFAULT_RUNTIME_INFO_LEVEL, TR_MODULE_LEVELS /* Assign initial values for trace levels */
};

/**
 * Array used to translate a debug trace level to its name.
 */
static const char *Module_Level_Names[] =
{ "TR_LVL_FAULT", "TR_LVL_WARN", "TR_LVL_NOTIFY", "TR_LVL_INFO_HI", "TR_LVL_INFO_MID", "TR_LVL_INFO_LO" };

#undef Tr_Module_ID
#define Tr_Module_ID(name,descr) #name,

/** Names of dbg_trace modules statically configured for this build. */
static const char * Module_Names[] =
{ "TR_MODULE_UNKNOWN", Tr_Module_IDs };

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/
static void Configure_App_Dbg_Levels(Cmd_Opt_T* cmd_opt, SAL_Connection_T connection, uint16_t app_id);
static void Configure_Application(Cmd_Opt_T* cmd_opt, SAL_Connection_T connection, uint16_t app_id);
static void Configure_Selected_Apps(Cmd_Opt_T *cmd_opt);
static void Extract_Selected_App_Ids(Cmd_Opt_T* cmd_opt);
static void Extract_Selected_Module_Ids(Cmd_Opt_T* cmd_opt);
static void Free_Pattern_Strings(char **pattern_strings, size_t num_strings);
static void Get_App_Thread_Stats(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id);
static void Get_Dbg_Levels(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id);
static int32_t Get_Local_App_List(Cmd_Opt_T *cmd_opt);
static void Get_Local_Module_List(Cmd_Opt_T *cmd_opt);
static size_t Get_Matches(uint16_t **p_match_list, size_t num_str, char **p_list, size_t num_substr,
   char const **substrings);
static size_t Get_Matching_Apps(Cmd_Opt_T* cmd_opt, uint16_t **p_match_list, size_t num_substr,
   char const *substrings[]);
static size_t Get_Matching_Modules(Cmd_Opt_T* cmd_opt, uint16_t **p_match_list, size_t num_substr,
   char const *substrings[]);
static void Get_Msg_Count_Stats(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id);
static void Get_Timer_Count_Stats(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id);
static int32_t Get_NS_App_List(Cmd_Opt_T *cmd_opt);
static void Get_NS_Module_List(Cmd_Opt_T *cmd_opt);
static bool_t Get_Queue_Stat(SAL_Connection_T* connection, SAL_Thread_Id_T thread_id);
static bool_t Get_Val(const char_t* s, SAL_Int_T* val);
static void Init_Cmd_Opt(Cmd_Opt_T* cmd_opt);
static void Out_Of_Memory_Exit(int32_t linenum);
static void Override_Dbg_Levels(Cmd_Opt_T const *cmd_opt, uint8_t *dbg_levels);
static bool_t Parse_Command_Line_Options(int argc, char* argv[], Cmd_Opt_T* cmd_opt);
static size_t Parse_Suboption_String(char ***p_str_list, char const *option_optarg);
static void Print_Apps(Cmd_Opt_T* cmd_opt);
static void Print_Dbg_Levels(Cmd_Opt_T const* cmd_opt, uint8_t const * dbg_levels);
static void Print_Module_Names(Cmd_Opt_T* cmd_opt);
static void Print_Help(Cmd_Opt_T const* cmd_opt);
static void Print_Queue_Stat(SAL_Thread_Id_T thread_id, DT_Queue_Stat_Rep_T const * queue_stat_rep);
static void Print_Queue_Stat_Header(void);
static bool_t Read_App_Dbg_Levels(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id,
   Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels);
static bool_t Read_Dbg_Levels_From_App(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection,
   uint16_t app_id, Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels);
static bool_t Read_Dbg_Levels_From_File(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection,
   uint16_t app_id, Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels);
static bool_t Read_Default_Dbg_Levels(Cmd_Opt_T const * cmd_opt, Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels);
static bool_t RT_Synch_Send(SAL_Connection_T* connection, const void* data, size_t data_size,
   void* reply_data, size_t reply_data_size);
static bool_t Write_App_Dbg_Levels(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id,
   Dbg_Ctrl_Set_Dbg_Levels_T *p_set_cmd, size_t set_cmd_sz);

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/**
 * Determine if any of the configuration options are present
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 *
 * @return true if any options were selected that configure an XSAL app.
 */
static bool_t Any_Configure_Options(Cmd_Opt_T const * cmd_opt)
{
   bool_t options_present = false;

   if ((cmd_opt->default_module_level >= 0) || (cmd_opt->num_fault_lvl_modules > 0)
      || (cmd_opt->num_warn_lvl_modules > 0) || (cmd_opt->num_notify_lvl_modules > 0)
      || (cmd_opt->num_hi_lvl_modules > 0) || (cmd_opt->num_mid_lvl_modules > 0)
      || (cmd_opt->num_lo_lvl_modules > 0) || cmd_opt->show_msg_counts || cmd_opt->show_timer_counts)
   {
      options_present = true;
   }
   if (cmd_opt->print_dbg_levels)
   {
      options_present = true;
   }
   if (cmd_opt->queue_id != -1)
   {
      options_present = true;
   }
   if (cmd_opt->static_run_enabled)
   {
      options_present = true;
   }

   return options_present;
}

/**
 * Used to configure the debug levels of the specified XSAL application.
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 *
 * @param [in] connection The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 *
 * @param [in] app_id The ID of the XSAL application.
 */
static void Configure_App_Dbg_Levels(Cmd_Opt_T* cmd_opt, SAL_Connection_T connection, uint16_t app_id)
{
   /*
    * The reply to a DT_GET_DBG_LEVELS request is a DT_Dbg_Levels_T structure,
    * which is a size_t (num_levels) followed by an array of uint8_t containing
    * the levels. To allow this program to work with XSAL apps that were
    * built with a different configuration value of TR_NUM_MODULE_IDS, this
    * code dynamically creates a data area that matches the size of the
    * DT_Dbg_Levels_T of the name server (and the XSAL apps that run with it)
    * instead of using the type that this application was compiled with.
    */
   size_t set_cmd_sz = sizeof(Dbg_Ctrl_Set_Dbg_Levels_T) + (cmd_opt->num_module_ids - 1) * sizeof(uint8_t);
   Dbg_Ctrl_Set_Dbg_Levels_T *p_set_cmd = calloc(1, set_cmd_sz);

   if (NULL == p_set_cmd)
   {
      Out_Of_Memory_Exit(__LINE__);
   }
   else if (Read_App_Dbg_Levels(cmd_opt, connection, app_id, &p_set_cmd->dbg_levels))
   {
      if (p_set_cmd->dbg_levels.num_levels != cmd_opt->num_module_ids)
      {
         printf("App reported a different number of modules (%d) than expected (%d)\n",
            (int) p_set_cmd->dbg_levels.num_levels, (int) cmd_opt->num_module_ids);
      }
      else
      {
         Override_Dbg_Levels(cmd_opt, p_set_cmd->dbg_levels.levels);

         Write_App_Dbg_Levels(cmd_opt, connection, app_id, p_set_cmd, set_cmd_sz);
      }
   }
   else
   {
      /* nothing */
   }
   free(p_set_cmd);
}

/**
 * Applies the command line options to the specified XSAL application.
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 *
 * @param [in] connection The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 *
 * @param [in] app_id The ID of the XSAL application.
 */
static void Configure_Application(Cmd_Opt_T* cmd_opt, SAL_Connection_T connection, uint16_t app_id)
{
   if (cmd_opt->first_pass)
   {
      printf("\nApplication %s [App_id=%d]\n", cmd_opt->app_names[app_id], app_id);
   }
   /*
    * Configure debug module levels for the application
    */
   if ((cmd_opt->default_module_level >= 0) || (cmd_opt->num_fault_lvl_modules > 0)
      || (cmd_opt->num_warn_lvl_modules > 0) || (cmd_opt->num_notify_lvl_modules > 0)
      || (cmd_opt->num_hi_lvl_modules > 0) || (cmd_opt->num_mid_lvl_modules > 0)
      || (cmd_opt->num_lo_lvl_modules > 0))
   {
      Configure_App_Dbg_Levels(cmd_opt, connection, app_id);
   }
   /*
    * Display dbg_trace levels for the application.
    */
   if (cmd_opt->print_dbg_levels)
   {
      Get_Dbg_Levels(cmd_opt, connection, app_id);
   }
   /*
    * Display XSAL thread queue status
    */
   if (cmd_opt->queue_id != -1)
   {
      Get_App_Thread_Stats(cmd_opt, connection, app_id);
   }
   /*
    * Display message count statistics
    */
   if (cmd_opt->show_msg_counts)
   {
      Get_Msg_Count_Stats(cmd_opt, connection, app_id);
   }
   /*
    * Display timer count statistics
    */
   if (cmd_opt->show_timer_counts)
   {
      Get_Timer_Count_Stats(cmd_opt, connection, app_id);
   }
}

/**
 * Configures the XSAL applications specified by the "-n" option.
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 */
static void Configure_Selected_Apps(Cmd_Opt_T* cmd_opt)
{
   size_t app_index;
   SAL_Connection_T connection;

   for (app_index = 0; app_index < cmd_opt->num_selected_apps; app_index++)
   {
      uint16_t app_id = cmd_opt->selected_app_ids[app_index];

      if ((app_id != SAL_NO_APP_ID) && (cmd_opt->static_run_enabled || cmd_opt->app_is_running[app_id]))
      {
         if (SAL_Connect(app_id, SAL_ROUTER_THREAD_ID, &connection))
         {
            Configure_Application(cmd_opt, connection, app_id);
            SAL_Disconnect(&connection);
         }
         else if (cmd_opt->static_run_enabled)
         {
            SAL_Invalidate_Connection(&connection);
            Configure_Application(cmd_opt, connection, app_id);
         }
         else
         { /*
          * If a connection error occurs to an app after the first pass (-r
          * option active), then remove that application from the selected list
          * to prevent further error messages.
          */
            cmd_opt->selected_app_ids[app_index] = SAL_NO_APP_ID;
         }
      }
      else if ((app_id != SAL_NO_APP_ID) && !cmd_opt->app_is_running[app_id] && cmd_opt->first_pass)
      {
         printf("\nApplication %s (%d) is not running.\n", cmd_opt->app_names[app_id], (int) app_id);
      }
      else
      {
         /* Nothing */
      }
   } /* for for(app_index = 0u... */
}

/**
 * Parses the value of the "-n" option to create the list of XSAL applications
 * that are to be configured/interrogated.
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 */
static void Extract_Selected_App_Ids(Cmd_Opt_T* cmd_opt)
{
   char const *empty_string = "";
   size_t num_pattern_strings; /* number of elements in pattern_strings */
   /* Array of patterns extracted (duplicated) from the command line: */
   char **pattern_strings;

   num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_n_optarg);

   if ('*' == *pattern_strings[0]) /* match everything */
   {
      cmd_opt->num_selected_apps = Get_Matching_Apps(cmd_opt, &cmd_opt->selected_app_ids, 1, &empty_string);
   }
   else /* only match the specified pattern strings */
   {
      cmd_opt->num_selected_apps = Get_Matching_Apps(cmd_opt, &cmd_opt->selected_app_ids, num_pattern_strings,
         (const char**) pattern_strings);
   }
   Free_Pattern_Strings(pattern_strings, num_pattern_strings);
}

/**
 * Parses the values of the "-f", "-h", "-l", "-m", "-w" and "-y" options to create
 * lists of debug module ids that are to be assigned the respective trace levels.
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 */
static void Extract_Selected_Module_Ids(Cmd_Opt_T* cmd_opt)
{
   char const *empty_string = "";
   size_t num_pattern_strings; /* number of elements in pattern_strings */
   /* Array of patterns extracted (duplicated) from the command line options: */
   char **pattern_strings;

   /* Modules for TR_LVL_FAULT (-f option) */
   if (cmd_opt->opt_f_optarg != NULL)
   {
      num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_f_optarg);

      if ('*' == *pattern_strings[0]) /* match everything */
      {
         cmd_opt->num_fault_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_fault_lvl_modules,
            1, &empty_string);
      }
      else /* only match the specified pattern strings */
      {
         cmd_opt->num_fault_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_fault_lvl_modules,
            num_pattern_strings, (const char**) pattern_strings);
      }
      Free_Pattern_Strings(pattern_strings, num_pattern_strings);
   }

   /* Modules for TR_LVL_WARN (-w option) */
   if (cmd_opt->opt_w_optarg != NULL)
   {
      num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_w_optarg);

      if ('*' == *pattern_strings[0]) /* match everything */
      {
         cmd_opt->num_warn_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_warn_lvl_modules,
            1, &empty_string);
      }
      else /* only match the specified pattern strings */
      {
         cmd_opt->num_warn_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_warn_lvl_modules,
            num_pattern_strings, (const char**) pattern_strings);
      }
      Free_Pattern_Strings(pattern_strings, num_pattern_strings);
   }

   /* Modules for TR_LVL_NOTIFY (-y option) */
   if (cmd_opt->opt_y_optarg != NULL)
   {
      num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_y_optarg);

      if ('*' == *pattern_strings[0]) /* match everything */
      {
         cmd_opt->num_notify_lvl_modules = Get_Matching_Modules(cmd_opt,
            &cmd_opt->requested_notify_lvl_modules, 1, &empty_string);
      }
      else /* only match the specified pattern strings */
      {
         cmd_opt->num_notify_lvl_modules = Get_Matching_Modules(cmd_opt,
            &cmd_opt->requested_notify_lvl_modules, num_pattern_strings, (const char**) pattern_strings);
      }
      Free_Pattern_Strings(pattern_strings, num_pattern_strings);
   }

   /* Modules for TR_LVL_INFO_HI (-h option) */
   if (cmd_opt->opt_h_optarg != NULL)
   {
      num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_h_optarg);

      if ('*' == *pattern_strings[0]) /* match everything */
      {
         cmd_opt->num_hi_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_hi_lvl_modules, 1,
            &empty_string);
      }
      else /* only match the specified pattern strings */
      {
         cmd_opt->num_hi_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_hi_lvl_modules,
            num_pattern_strings, (const char**) pattern_strings);
      }
      Free_Pattern_Strings(pattern_strings, num_pattern_strings);
   }

   /* Modules for TR_LVL_INFO_MID (-m option) */
   if (cmd_opt->opt_m_optarg != NULL)
   {
      num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_m_optarg);

      if ('*' == *pattern_strings[0]) /* match everything */
      {
         cmd_opt->num_mid_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_mid_lvl_modules, 1,
            &empty_string);
      }
      else /* only match the specified pattern strings */
      {
         cmd_opt->num_mid_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_mid_lvl_modules,
            num_pattern_strings, (const char**) pattern_strings);
      }
      Free_Pattern_Strings(pattern_strings, num_pattern_strings);
   }

   /* Modules for TR_LVL_INFO_LO (-l option) */
   if (cmd_opt->opt_l_optarg != NULL)
   {
      num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_l_optarg);

      if ('*' == *pattern_strings[0]) /* match everything */
      {
         cmd_opt->num_lo_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_lo_lvl_modules, 1,
            &empty_string);
      }
      else /* only match the specified pattern strings */
      {
         cmd_opt->num_lo_lvl_modules = Get_Matching_Modules(cmd_opt, &cmd_opt->requested_lo_lvl_modules,
            num_pattern_strings, (const char**) pattern_strings);
      }
      Free_Pattern_Strings(pattern_strings, num_pattern_strings);
   }
}

/**
 * Frees the previously allocated array of dynamically allocated strings.
 *
 * @param [in] pattern_strings Pointer to dynamically allocated array.
 *
 * @param [in] num_strings Number of dynamically allocated strings within
 *                         the array.
 */
static void Free_Pattern_Strings(char **pattern_strings, size_t num_strings)
{
   size_t str_index;

   /* Free each element in the array. */
   for (str_index = 0; str_index < num_strings; str_index++)
   {
      free(pattern_strings[str_index]);
   }
   free(pattern_strings); /* And then free the array itself */
}

/**
 * Reads the thread status for an application and prints it out.
 *
 * @param cmd_opt [in] Cmd_Opt_T structure containing the program options
 *                          and any results.
 * @param connection [in] The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 */
static void Get_App_Thread_Stats(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id)
{
   DT_Get_Thread_Number_T dt_threads_num_req;
   DT_Threads_Num_Rep_T dt_threads_num_rep =
   { 0 };

   dt_threads_num_req.command = DT_GET_NUM_THREADS;

   if (RT_Synch_Send(&connection, &dt_threads_num_req, sizeof(dt_threads_num_req), &dt_threads_num_rep,
      sizeof(dt_threads_num_rep)))
   {
      if (dt_threads_num_rep.thread_num > 0u)
      {
         SAL_Thread_Id_T tid;

         if (cmd_opt->first_pass)
         {
            Print_Queue_Stat_Header();
         }
         for (tid = 0; (size_t) tid <= dt_threads_num_rep.thread_num; tid++)
         {
            if ((0 == cmd_opt->queue_id) || (tid == cmd_opt->queue_id))
            {
               Get_Queue_Stat(&connection, tid);
            }
         }
      }
   }
   else
   {
      printf("Send message to %s [App_id=%d] failed\n", cmd_opt->app_names[app_id], app_id);
   }
}

/**
 * Reads the debug trace levels from an XSAL application and prints them out.
 *
 * @param cmd_opt [in] Cmd_Opt_T structure containing the program options
 *                          and any results.
 * @param connection [in] The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 */
static void Get_Dbg_Levels(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id)
{
   /*
    * The reply to a DT_GET_DBG_LEVELS request is a DT_Dbg_Levels_T structure,
    * which is a size_t (num_levels) followed by an array of uint8_t containing
    * the levels. To allow this program to work with XSAL apps that were
    * built with a different configuration value of TR_NUM_MODULE_IDS, this
    * code dynamically creates a data area that matches the size of the
    * DT_Dbg_Levels_T of the name server (and the XSAL apps that run with it)
    * instead of using the type that this application was compiled with.
    */
   size_t dbg_get_level_reply_sz = sizeof(Dbg_Ctrl_Dbg_Levels_T)
      + (cmd_opt->num_module_ids - 1) * sizeof(uint8_t);
   Dbg_Ctrl_Dbg_Levels_T *dbg_lvl_data = calloc(1, dbg_get_level_reply_sz);

   if (Read_App_Dbg_Levels(cmd_opt, connection, app_id, dbg_lvl_data))
   {
      Print_Dbg_Levels(cmd_opt, dbg_lvl_data->levels);
   }
   else
   {
      printf("Unable to read debug levels for %s [App_id=%d] failed\n", cmd_opt->app_names[app_id], app_id);
   }
   free(dbg_lvl_data);
}

/**
 * Obtains the list of XSAL applications from the static XSAL configuration.
 * This will be the same as what the Name Server would supply provided that
 * this program was built with the same XSAL configuration as the Name Server.
 *
 * @return the number of running XSAL applications, or -1 if unable to determine
 *
 * @param [out] cmd_opt Cmd_Opt_T structure whose running_apps list is updated.
 */
static int32_t Get_Local_App_List(Cmd_Opt_T *cmd_opt)
{
   uint16_t app_id;

   cmd_opt->num_app_ids = SAL_MAX_APPS + 1;
   cmd_opt->app_names = calloc(sizeof(*cmd_opt->app_names), cmd_opt->num_app_ids);
   cmd_opt->app_is_running = calloc(sizeof(*cmd_opt->app_is_running), cmd_opt->num_app_ids);

   if ((NULL == cmd_opt->app_names) || (NULL == cmd_opt->app_is_running))
   {
      Out_Of_Memory_Exit(__LINE__);
   }
   else
   {
      for (app_id = 0; app_id < cmd_opt->num_app_ids; app_id++)
      {
         cmd_opt->app_names[app_id] = strdup(SAL_Get_App_Name(app_id));
      }
   }

   return 0;
}

/**
 * Obtains the list of dbg_trace module names from the static dbg_trace
 * configuration. This will be the same as what the Name Server would supply
 * provided that this program was built with the same dbg_trace configuration
 * as the Name Server.
 *
 * @param [out] cmd_opt Cmd_Opt_T structure whose running_apps list is updated.
 */
static void Get_Local_Module_List(Cmd_Opt_T *cmd_opt)
{
   uint16_t module_id;

   cmd_opt->num_module_ids = TR_NUM_MODULE_IDS;
   cmd_opt->module_names = calloc(sizeof(*cmd_opt->module_names), cmd_opt->num_module_ids);

   if (NULL == cmd_opt->module_names)
   {
      Out_Of_Memory_Exit(__LINE__);
   }
   else
   {
      for (module_id = 0; module_id < cmd_opt->num_module_ids; module_id++)
      {
         cmd_opt->module_names[module_id] = strdup(Module_Names[module_id]);
      }
   }
}

/**
 * Searches an array of strings (p_list) for any strings that contain any of
 * the substrings in another array (substrings). The results are returned in
 * a third array that is dynamically allocated by this function and whose
 * pointer is written to *p_match_list; this result is a list of indices into
 * the "p_list" array that match any of the substrings. It is the responsibility
 * of the caller to free this results array when it is finished with it. The
 * number of valid entries in the dynamically created array is returned by the
 * function.
 *
 * @return The number of valid entries in the *p_match_list array.
 *
 * @param [out] p_match_list Pointer to pointer to which this function writes the
 *                           address of the dynamically allocated results array.
 *                           The caller must free this array. The values in this
 *                           array are indices into "p_list" that match any of
 *                           the patterns in the substrings array.
 * @param [in] num_str Number of entries in the list of strings to search (p_list)
 * @param [in] p_list List of strings to search for the substring matches.
 * @param [in] num_substr Number of elements in substrings array.
 * @param [in] substrings Array of substrings to search for.
 */
static size_t Get_Matches(uint16_t **p_match_list, size_t num_str, char **p_list, size_t num_substr,
   char const **substrings)
{
   uint16_t *list;
   size_t list_index;
   size_t num_matches = 0;

   assert(substrings != NULL);
   assert(p_match_list != NULL);

   /* Allocate worst case result - every entry in p_list matches */
   list = calloc(num_str, sizeof(*list));

   if (NULL == list)
   {
      Out_Of_Memory_Exit(__LINE__);
   }
   else
   {
      for (list_index = 0; list_index < num_str; list_index++)
      {
         size_t ss_i;

         for (ss_i = 0; ss_i < num_substr; ss_i++)
         {
            if (strstr(p_list[list_index], substrings[ss_i]))
            {
               list[num_matches++] = list_index;
               break; /* done with this element once a match is found */
            }
         }
      }
   }
   *p_match_list = list;
   return num_matches;
}

/**
 * Searches the list of XSAL application names for any that contain any of
 * the substrings in the specified array. The results are returned in
 * a third array that is dynamically allocated by this function and whose
 * pointer is written to *p_match_list; this result is a list of XSAL App Ids
 * that match any of the substrings. It is the responsibility of the caller
 * to free this results array when it is finished with it. The number of
 * valid entries in the dynamically created array is returned by the function.
 *
 * @return The number of valid entries in the *p_match_list array.
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 * @param [out] p_match_list Pointer to pointer to which this function writes the
 *                           address of the dynamically allocated results array.
 *                           The caller must free this array. The values in this
 *                           array are XSAL App Ids of any apps whose names match
 *                           any of the patterns in the substrings array.
 * @param [in] num_substr Number of elements in substrings array.
 * @param [in] substrings Array of substrings to search for.
 */
static size_t Get_Matching_Apps(Cmd_Opt_T* cmd_opt, uint16_t **p_match_list, size_t num_substr,
   char const *substrings[])
{
   return Get_Matches(p_match_list, cmd_opt->num_app_ids, cmd_opt->app_names, num_substr, substrings);
}

/**
 * Searches the list of dbg_trace module names for any that contain any of
 * the substrings in the specified array. The results are returned in
 * a third array that is dynamically allocated by this function and whose
 * pointer is written to *p_match_list; this result is a list of debug Module
 * Ids that match any of the substrings. It is the responsibility of the caller
 * to free this results array when it is finished with it. The number of
 * valid entries in the dynamically created array is returned by the function.
 *
 * @return The number of valid entries in the *p_match_list array.
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 * @param [out] p_match_list Pointer to pointer to which this function writes the
 *                           address of the dynamically allocated results array.
 *                           The caller must free this array. The values in this
 *                           array are debug Module Ids of any module ids whose
 *                           names match any of the patterns in the substrings
 *                           array.
 * @param [in] num_substr Number of elements in substrings array.
 * @param [in] substrings Array of substrings to search for.
 */
static size_t Get_Matching_Modules(Cmd_Opt_T* cmd_opt, uint16_t **p_match_list, size_t num_substr,
   char const *substrings[])
{
   return Get_Matches(p_match_list, cmd_opt->num_module_ids, cmd_opt->module_names, num_substr, substrings);
}

/**
 * Reads the message count status for an application and prints it out.
 *
 * @param cmd_opt [in] Cmd_Opt_T structure containing the program options
 *                          and any results.
 * @param connection [in] The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 */
static void Get_Msg_Count_Stats(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id)
{
   DT_Get_Msg_Counts_T dt_msg_counts_req;
   DT_Msg_Counts_Rep_T reply =
   {
   { 0 } };

   dt_msg_counts_req.command = DT_GET_MSG_COUNTS;
   dt_msg_counts_req.reset_counts = cmd_opt->reset_msg_counts;

   if (RT_Synch_Send(&connection, &dt_msg_counts_req, sizeof(dt_msg_counts_req), &reply, sizeof(reply)))
   {
      size_t to_app;

      printf("app %3d msg cnt: internal = %lu; external = %lu;", (int) app_id,
         (long unsigned) reply.msg_count_stats.total_num_sent_internally,
         (long unsigned) reply.msg_count_stats.total_num_sent_externally);

      for (to_app = 1;
         to_app
            < (sizeof(SAL_App_Msg_Count_Stats_T) - offsetof(SAL_App_Msg_Count_Stats_T,num_sent_to_app))
               / sizeof(reply.msg_count_stats.num_sent_to_app[0]); to_app++)
      {
         uint32_t count = reply.msg_count_stats.num_sent_to_app[to_app];
         if (count != 0)
         {
            printf(" ->(%d) = %lu;", (int) to_app, (unsigned long) count);
         }
      }
      printf("\n");
   }
   else
   {
      printf("Send message to %s [App_id=%d] failed\n", cmd_opt->app_names[app_id], app_id);
   }
}

/**
 * Reads the timer count status for an application and prints it out.
 *
 * @param cmd_opt [in] Cmd_Opt_T structure containing the program options
 *                          and any results.
 * @param connection [in] The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 */
static void Get_Timer_Count_Stats(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id)
{
#   if defined(LINUX)
   DT_Get_Timer_Counts_T dt_timer_counts_req;
   DT_Timer_Counts_Rep_T reply =
   {
   { 0 } };

   dt_timer_counts_req.command = DT_GET_TIMER_COUNTS;

   if (RT_Synch_Send(&connection, &dt_timer_counts_req, sizeof(dt_timer_counts_req), &reply, sizeof(reply)))
   {
      printf("app %3d Timers - Running: %hu Stopped: %hu Unused: %hu\n", (int) app_id,
         reply.timer_count_stats.num_running, reply.timer_count_stats.num_stopped, reply.timer_count_stats.num_unused);
   }
   else
   {
      printf("Send message to %s [App_id=%d] failed\n", cmd_opt->app_names[app_id], app_id);
   }
#endif   
}

/**
 * Interrogates the name server for the list of XSAL applications and updates
 * cmd_opt structure with their current status.
 *
 * @return the number of running XSAL applications, or -1 if unable to determine
 *
 * @param [out] cmd_opt Cmd_Opt_T structure whose running_apps list is updated.
 */
static int32_t Get_NS_App_List(Cmd_Opt_T *cmd_opt)
{
   int32_t num_running = 0;
   uint16_t app_id = 0;

   if (!SAL_NS_Get_Last_App_Id(&app_id))
   {
      printf("Unable to get number of apps from Name Server\n");
      num_running = -1;
      cmd_opt->ns_connection_is_ok = false;
   }
   else
   {
      cmd_opt->num_app_ids = app_id + 1;
      cmd_opt->app_names = calloc(sizeof(*cmd_opt->app_names), cmd_opt->num_app_ids);
      cmd_opt->app_is_running = calloc(sizeof(*cmd_opt->app_is_running), cmd_opt->num_app_ids);

      if ((NULL == cmd_opt->app_names) || (NULL == cmd_opt->app_is_running))
      {
         Out_Of_Memory_Exit(__LINE__);
      }
      else
      {
         for (app_id = 0; app_id < cmd_opt->num_app_ids; app_id++)
         {
            SAL_App_Data_T app_data =
            { 0 };

            if (SAL_NS_Get_App_Data(app_id, &app_data))
            {
               cmd_opt->app_names[app_id] = strdup(app_data.app_name);
               if (NULL == cmd_opt->app_names[app_id])
               {
                  Out_Of_Memory_Exit(__LINE__);
               }
               else if (app_data.is_running)
               {
                  cmd_opt->app_is_running[app_id] = true;
                  num_running++;
               }
               else
               {
                  /* nothing */
               }
            }
            else
            {
               printf("Could not get app data from Name Server for app %d\n", (int) app_id);
               cmd_opt->ns_connection_is_ok = false;
            }
         }
      }
   }

   return num_running;
}

/**
 * Interrogates the name server for the list of dbg_trace module names and
 * updates cmd_opt structure with the data.
 *
 * @param [out] cmd_opt Cmd_Opt_T structure whose running_apps list is updated.
 */
static void Get_NS_Module_List(Cmd_Opt_T *cmd_opt)
{
   uint16_t module_id = 0;

   if (!SAL_NS_Get_Last_Dbg_Module_Id(&module_id))
   {
      printf("Unable to get number of dbg module ids from Name Server\n");
      cmd_opt->ns_connection_is_ok = false;
   }
   else
   {
      cmd_opt->num_module_ids = module_id + 1;
      cmd_opt->module_names = calloc(sizeof(*cmd_opt->module_names), cmd_opt->num_module_ids);

      if (NULL == cmd_opt->module_names)
      {
         Out_Of_Memory_Exit(__LINE__);
      }
      else
      {
         for (module_id = 0; module_id < cmd_opt->num_module_ids; module_id++)
         {
            SAL_Dbg_Module_Data_T module_data =
            { 0 };

            if (SAL_NS_Get_Dbg_Module_Data(module_id, &module_data))
            {
               cmd_opt->module_names[module_id] = strdup(module_data.module_name);
            }
            else
            {
               printf("Could not get module data for module %d\n", (int) module_id);
               cmd_opt->ns_connection_is_ok = false;
            }
         }
      }
   }
}

/**
 * Uses the specified connection to an XSAL application to obtain the queue
 * status of the specified XSAL thread and prints the results.
 *
 * @return true if successful, false if an error occurred.
 *
 * @param [in,out] connection SAL_Connection_T used to communicate to the XSAL app.
 * @param [in] thread_id The XSAL thread for which queue status is to be obtained.
 */
static bool_t Get_Queue_Stat(SAL_Connection_T* connection, SAL_Thread_Id_T thread_id)
{
   bool_t is_ok;
   DT_Get_Queue_Stat_T queu_stat_req;
   DT_Queue_Stat_Rep_T queu_stat_rep =
   { 0 };

   queu_stat_req.command = DT_GET_QUEUE_STATUS;
   queu_stat_req.thread_id = thread_id;

   is_ok = RT_Synch_Send(connection, &queu_stat_req, sizeof(DT_Get_Queue_Stat_T), &queu_stat_rep,
      sizeof(DT_Queue_Stat_Rep_T));

   if (is_ok)
   {
      Print_Queue_Stat(thread_id, &queu_stat_rep);
   }
   return is_ok;
}

/**
 * Converts the specified string to an integer and writes the results to the
 * specified variable.
 *
 * @return true if successful, false if an error occurred.
 *
 * @param [in] s The string to be converted.
 * @param [in] val Pointer to location to which to write the value.
 */
static bool_t Get_Val(const char_t* s, SAL_Int_T* val)
{
   bool_t is_ok = true;
   if (strncmp(s, "0", 8) == 0)
   {
      *val = 0;
   }
   else
   {
      SAL_Int_T tmp_val = atoi(s);
      if (tmp_val != 0)
      {
         *val = tmp_val;
      }
      else
      {
         is_ok = false;
      }
   }
   return is_ok;
}

/**
 * Establishes the initial default values for the program's command options.
 *
 * @param [in,out] cmd_opt Points to the structure containing all of the program's
 *                         options.
 */
static void Init_Cmd_Opt(Cmd_Opt_T* cmd_opt)
{
   memset(cmd_opt, 0, sizeof(Cmd_Opt_T));

   cmd_opt->default_module_level = -1;
   cmd_opt->first_pass = true;
   cmd_opt->queue_id = -1;
}

/**
 * Overrides the dbg_trace levels within the specified array according to the
 * program's command line arguments.
 *
 * @param [in] cmd_opt Points to the structure containing all of the program's
 *                     options.
 * @param dbg_levels Pointer to the array containing the dbg_trace levels to
 *                   be overridden.
 */
static void Override_Dbg_Levels(Cmd_Opt_T const *cmd_opt, uint8_t *dbg_levels)
{
   uint16_t module_id;
   size_t mod_index;
   /* Apply any default setting to all modules first */
   if ((cmd_opt->default_module_level >= TR_LVL_FAULT) && (cmd_opt->default_module_level <= TR_LVL_INFO_LO))
   {
      for (module_id = 0; module_id < cmd_opt->num_module_ids; module_id++)
      {
         dbg_levels[module_id] = cmd_opt->default_module_level;
      }
   }
   /* Then apply any TR_LVL_FAULT settings */
   for (mod_index = 0; mod_index < cmd_opt->num_fault_lvl_modules; mod_index++)
   {
      module_id = cmd_opt->requested_fault_lvl_modules[mod_index];
      dbg_levels[module_id] = TR_LVL_FAULT;
   }
   /* Then apply any TR_LVL_WARN settings */
   for (mod_index = 0; mod_index < cmd_opt->num_warn_lvl_modules; mod_index++)
   {
      module_id = cmd_opt->requested_warn_lvl_modules[mod_index];
      dbg_levels[module_id] = TR_LVL_WARN;
   }
   /* Then apply any TR_LVL_NOTIFY settings */
   for (mod_index = 0; mod_index < cmd_opt->num_notify_lvl_modules; mod_index++)
   {
      module_id = cmd_opt->requested_notify_lvl_modules[mod_index];
      dbg_levels[module_id] = TR_LVL_NOTIFY;
   }
   /* Then apply any TR_LVL_INFO_HI settings */
   for (mod_index = 0; mod_index < cmd_opt->num_hi_lvl_modules; mod_index++)
   {
      module_id = cmd_opt->requested_hi_lvl_modules[mod_index];
      dbg_levels[module_id] = TR_LVL_INFO_HI;
   }
   /* Then apply any TR_LVL_INFO_MID settings */
   for (mod_index = 0; mod_index < cmd_opt->num_mid_lvl_modules; mod_index++)
   {
      module_id = cmd_opt->requested_mid_lvl_modules[mod_index];
      dbg_levels[module_id] = TR_LVL_INFO_MID;
   }
   /* Then apply any TR_LVL_INFO_LO settings */
   for (mod_index = 0; mod_index < cmd_opt->num_lo_lvl_modules; mod_index++)
   {
      module_id = cmd_opt->requested_lo_lvl_modules[mod_index];
      dbg_levels[module_id] = TR_LVL_INFO_LO;
   }
}

/**
 * Parses the program's command line options and modifies the Cmd_Opt_T structure
 * to reflect the results.
 *
 * @param [in] argc The number of command line arguments (num entries in argv)
 * @param [in] argv Array of command line argument strings.
 * @param [in,out] cmd_opt Points to the structure containing all of the program's options.
 */
static bool_t Parse_Command_Line_Options(int argc, char* argv[], Cmd_Opt_T* cmd_opt)
{
   bool_t proceed = false;
   SAL_Int_T option;
   SAL_Int_T local_int_value;

   if (argc > 1)
   {
      proceed = true;

      while ((bool_t) ((option = getopt(argc, argv, "?a:b:cd:f:h:l:m:n:p:q:r:stw:xy:")) != -1))
      {
         switch (option)
         {
            case '?':
               proceed = false;
               break;

            case 'a':
               if (cmd_opt->opt_a_optarg != NULL)
               {
                  free(cmd_opt->opt_a_optarg); /* Last -a option "wins", discard others. */
               }
               cmd_opt->opt_a_optarg = strdup(optarg);
               break;

            case 'b':
               if (cmd_opt->opt_b_optarg != NULL)
               {
                  free(cmd_opt->opt_b_optarg); /* Last -b option "wins", discard others. */
               }
               cmd_opt->opt_b_optarg = strdup(optarg);
               break;

            case 'c':
               cmd_opt->show_msg_counts = true;
               break;

            case 'd':
               switch (optarg[0])
               {
                  case 'f':
                     cmd_opt->default_module_level = TR_LVL_FAULT;
                     break;
                  case 'w':
                     cmd_opt->default_module_level = TR_LVL_WARN;
                     break;
                  case 'y':
                     cmd_opt->default_module_level = TR_LVL_NOTIFY;
                     break;
                  case 'h':
                     cmd_opt->default_module_level = TR_LVL_INFO_HI;
                     break;
                  case 'm':
                     cmd_opt->default_module_level = TR_LVL_INFO_MID;
                     break;
                  case 'l':
                     cmd_opt->default_module_level = TR_LVL_INFO_LO;
                     break;
                  default:
                     printf("Illegal default module level: %s\n", optarg);
                     proceed = false;
                     break;
               }
               break;

            case 'f':
               if (cmd_opt->opt_f_optarg != NULL)
               {
                  free(cmd_opt->opt_f_optarg); /* Last -f option "wins", discard others. */
               }
               cmd_opt->opt_f_optarg = strdup(optarg);
               break;

            case 'h':
               if (cmd_opt->opt_h_optarg != NULL)
               {
                  free(cmd_opt->opt_h_optarg); /* Last -h option "wins", discard others. */
               }
               cmd_opt->opt_h_optarg = strdup(optarg);
               break;

            case 'l':
               if (cmd_opt->opt_l_optarg != NULL)
               {
                  free(cmd_opt->opt_l_optarg); /* Last -l option "wins", discard others. */
               }
               cmd_opt->opt_l_optarg = strdup(optarg);
               break;

            case 'm':
               if (cmd_opt->opt_m_optarg != NULL)
               {
                  free(cmd_opt->opt_m_optarg); /* Last -m option "wins", discard others. */
               }
               cmd_opt->opt_m_optarg = strdup(optarg);
               break;

            case 'n':
               if (cmd_opt->opt_n_optarg != NULL)
               {
                  free(cmd_opt->opt_n_optarg); /* Last -n option "wins", discard others. */
               }
               cmd_opt->opt_n_optarg = strdup(optarg);
               break;

            case 'q':
               if (Get_Val(optarg, &local_int_value))
               {
                  cmd_opt->queue_id = local_int_value;
               }
               else
               {
                  printf("Invalid Queue Id: %s\n", optarg);
                  proceed = false;
               }
               break;

            case 'p':
               cmd_opt->print_dbg_levels = true;
               if ('*' == optarg[0])
               {
                  cmd_opt->print_level[TR_LVL_FAULT] = cmd_opt->print_level[TR_LVL_WARN] =
                     cmd_opt->print_level[TR_LVL_NOTIFY] = cmd_opt->print_level[TR_LVL_INFO_HI] =
                        cmd_opt->print_level[TR_LVL_INFO_MID] = cmd_opt->print_level[TR_LVL_INFO_LO] = true;
               }
               else
               {
                  char *next_lvl = optarg;
                  while (*next_lvl != '\0')
                  {
                     switch (*next_lvl)
                     {
                        case '0':
                        case 'f':
                           cmd_opt->print_level[TR_LVL_FAULT] = true;
                           break;
                        case '1':
                        case 'w':
                           cmd_opt->print_level[TR_LVL_WARN] = true;
                           break;
                        case '2':
                        case 'y':
                           cmd_opt->print_level[TR_LVL_NOTIFY] = true;
                           break;
                        case '3':
                        case 'h':
                           cmd_opt->print_level[TR_LVL_INFO_HI] = true;
                           break;
                        case '4':
                        case 'm':
                           cmd_opt->print_level[TR_LVL_INFO_MID] = true;
                           break;
                        case '5':
                        case 'l':
                           cmd_opt->print_level[TR_LVL_INFO_LO] = true;
                           break;
                        default:
                           printf("Invalid suboption for -p: %c\n", *next_lvl);
                           break;
                     }
                     do
                     {
                        next_lvl++;
                     } while (',' == *next_lvl); /* skip separating commas */
                  }
               }
               break;

            case 'r':
               cmd_opt->repeat_sec = strtol(optarg, NULL, 10);
               if ((cmd_opt->repeat_sec <= 0) || (cmd_opt->repeat_sec > MAX_REPEAT_PERIOD_SEC))
               {
                  printf("Illegal value for -r (%s) - defaulting to 1\n", optarg);
                  cmd_opt->repeat_sec = 1;
               }
               break;

            case 's':
               cmd_opt->static_run_enabled = true;
               break;

            case 't':
               cmd_opt->show_timer_counts = true;
               break;

            case 'w':
               if (cmd_opt->opt_w_optarg != NULL)
               {
                  free(cmd_opt->opt_w_optarg); /* Last -w option "wins", discard others. */
               }
               cmd_opt->opt_w_optarg = strdup(optarg);
               break;

            case 'x':
               cmd_opt->reset_msg_counts = true;
               break;

            case 'y':
               if (cmd_opt->opt_y_optarg != NULL)
               {
                  free(cmd_opt->opt_y_optarg); /* Last -y option "wins", discard others. */
               }
               cmd_opt->opt_y_optarg = strdup(optarg);
               break;

            default:
               break;
         }
      }
   }
   return proceed;
}

/**
 * Parses a command line argument sub-option (optarg) to extract an array of
 * strings. The array and strings it contains are dynamically created and are
 * the responsibility of the caller to free (see Free_Pattern_Strings).
 *
 * @return The number of valid strings in the dynamically created array; the
 *         array is created (and must be freed) even if the value is zero.
 * 
 * @param [out] p_str_list Pointer to array pointer to which this function
 *                         writes the address of the array it allocates.
 * @param [in] option_optarg The sub-option string to be parsed.
 */
static size_t Parse_Suboption_String(char ***p_str_list, char const *option_optarg)
{
   char *next_string;
   char **pattern_strings;
   size_t num_pattern_strings = 0;
   size_t sz_pattern_strings;
   char *dup_name;
   char *my_optarg = strdup(option_optarg); /* create local copy to modify */
   char const *start_string = my_optarg;

   if (NULL == my_optarg)
   {
      Out_Of_Memory_Exit(__LINE__);
   }

   /* Skip any leading '=' */
   next_string = strchr(start_string, '=');
   if (next_string != NULL)
   {
      start_string = next_string + 1;
   }

   if ('*' == *start_string)
   {
      /* Just one pattern string: "*" */
      pattern_strings = malloc(sizeof(*pattern_strings));
      if (NULL == pattern_strings)
      {
         Out_Of_Memory_Exit(__LINE__);
      }
      /* use strdup so caller can always use free on the returned result */
      pattern_strings[0] = strdup("*");
      num_pattern_strings = 1;
   }
   else
   { /* Extract the comma-separated list of pattern strings and add them to
    * the pattern_strings array. Start with room for NUM_STR_TO_ALLOC, but
    * expand it later if necessary.
    */
      sz_pattern_strings = NUM_STR_TO_ALLOC;
      pattern_strings = malloc(sz_pattern_strings * sizeof(*pattern_strings));
      if (NULL == pattern_strings)
      {
         Out_Of_Memory_Exit(__LINE__);
      }

      while (start_string != NULL) /* repeat for all sub-options */
      {
         next_string = strchr(start_string, ',');
         if (next_string != NULL)
         {
            *next_string++ = '\0'; /* replace ',' with NIL and skip to next subopt */
            dup_name = strdup(start_string);
         }
         else /* last subopt (no trailing ',') */
         {
            dup_name = strdup(start_string);
         }
         if (NULL == dup_name)
         {
            Out_Of_Memory_Exit(__LINE__);
         }
         if (num_pattern_strings >= sz_pattern_strings)
         {
            sz_pattern_strings += NUM_STR_TO_ALLOC; /* make room for NUM_STR_TO_ALLOC more pattern strings */
            pattern_strings = realloc(pattern_strings, sz_pattern_strings * sizeof(*pattern_strings));
            if (NULL == pattern_strings)
            {
               Out_Of_Memory_Exit(__LINE__);
            }
         }
         pattern_strings[num_pattern_strings++] = dup_name;

         start_string = next_string;
      } /* while (start_string != NULL) */
   }
   free(my_optarg);
   *p_str_list = pattern_strings;
   return num_pattern_strings;
}

/**
 * Prints out the list of XSAL application names that make it through the filter
 * specified on the command line ("-a").
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 */
static void Print_Apps(Cmd_Opt_T* cmd_opt)
{
   char const *empty_string = "";
   size_t match_index;
   uint16_t *match_list; /* List of App ids for Apps that match */
   size_t num_matches; /* # of App names that match the specified patterns */
   size_t num_pattern_strings; /* number of elements in pattern_strings */
   /* Array of patterns extracted (duplicated) from the command line: */
   char **pattern_strings;

   num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_a_optarg);

   if ('*' == *pattern_strings[0]) /* match everything */
   {
      num_matches = Get_Matching_Apps(cmd_opt, &match_list, 1, &empty_string);
   }
   else /* only match the specified pattern strings */
   {
      num_matches = Get_Matching_Apps(cmd_opt, &match_list, num_pattern_strings,
         (const char**) pattern_strings);
   }

   printf("#    App Name\n");
   printf("---- --------\n");
   for (match_index = 0; match_index < num_matches; match_index++)
   {
      size_t app_id = match_list[match_index];
      printf("%-4d %s\n", (int) app_id, cmd_opt->app_names[app_id]);
   }

   free(match_list);

   Free_Pattern_Strings(pattern_strings, num_pattern_strings);
}

/**
 * Prints the dbg_trace levels reported by an application that pass through the
 * command line filters.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 * @param [in] dbg_levels array containing the dbg_trace levels reported by an
 *                        XSAL application.
 */
static void Print_Dbg_Levels(Cmd_Opt_T const* cmd_opt, uint8_t const * dbg_levels)
{
   size_t module_id;

   printf("\n  DBG_TRACE LEVEL      Module\n");
   printf("  -------------------  -----------\n");
   for (module_id = 1; module_id < cmd_opt->num_module_ids; module_id++)
   {
      char const *level_name;
      uint8_t level = dbg_levels[module_id];

      if ((level > TR_LVL_INFO_LO) || cmd_opt->print_level[level])
      {
         if (level > TR_LVL_INFO_LO)
         {
            level_name = "ILLEGAL";
         }
         else
         {
            level_name = Module_Level_Names[level];
         }
         printf("  (%d) %-15s  (%d) %s\n", (int) level, level_name, (int) module_id,
            cmd_opt->module_names[module_id]);
      }
   }
}

/**
 * Prints out the list of dbg_trace module names that make it through the filter
 * specified on the command line ("-b").
 *
 * @param [in,out] cmd_opt Cmd_Opt_T structure containing the program options
 *                         and any results.
 */
static void Print_Module_Names(Cmd_Opt_T* cmd_opt)
{
   char const *empty_string = "";
   size_t match_index;
   uint16_t *match_list; /* List of App ids for Apps that match */
   size_t num_matches; /* # of App names that match the specified patterns */
   size_t num_pattern_strings; /* number of elements in pattern_strings */
   /* Array of patterns extracted (duplicated) from the command line: */
   char **pattern_strings;

   num_pattern_strings = Parse_Suboption_String(&pattern_strings, cmd_opt->opt_b_optarg);

   if ('*' == *pattern_strings[0]) /* match everything */
   {
      num_matches = Get_Matching_Modules(cmd_opt, &match_list, 1, &empty_string);
   }
   else /* only match the specified pattern strings */
   {
      num_matches = Get_Matching_Modules(cmd_opt, &match_list, num_pattern_strings,
         (const char**) pattern_strings);
   }

   printf("#    Module Name\n");
   printf("---- -----------\n");
   for (match_index = 0; match_index < num_matches; match_index++)
   {
      size_t mod_id = match_list[match_index];
      printf("%-4d %s\n", (int) mod_id, cmd_opt->module_names[mod_id]);
   }

   free(match_list);

   Free_Pattern_Strings(pattern_strings, num_pattern_strings);
}

/**
 * Prints help information for the program.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 */
static void Print_Help(Cmd_Opt_T const* cmd_opt)
{
   printf("\ndbg_ctrl -? | [-a<\\*|PATTERN>] [-b<\\*|PATTERN>] [CMD]\n"
      "\n"
      "Description:\n"
      "  CMD       -n<\\*|PATTERN> OPTIONS\n"
      "\n"
      "  PATTERN   string[,string] - a comma-separated list of strings to\n"
      "              match (either to App names or Module names). No spaces are\n"
      "              allowed and these are case sensitive. See NOTES below.\n"
      "\n"
      "  OPTIONS   [LEVELS] [-p \\*|LVL_LIST] [-q<\\0|1..n>] -s\n"
      "\n"
      "  LEVELS    [-d f|w|y|h|m|l] [-f|h|l|m|w|y<\\*|PATTERN>]\n"
      "\n"
      "  LVL_LIST  comma-separated level values to print: f,w,y,h,m,l (or 0,1,2,3,4,5)\n"
      "\n"
      "  -?              print this help.\n"
      "  -a<\\*|PATTERN>  list Applications - \\* for all Apps or a list of patterns\n"
      "                     used to form the Module list.\n"
      "  -b<\\*|PATTERN>  list Modules - \\* for all Modules or a list of patterns\n"
      "                     used to form the Module list.\n"
      "  -c              Print message count statistics for selected applications.\n"
      "  -d f|w|y|h|m|l  Default all module levels to the specified value (f=FAULT,\n"
      "                    w=WARN,y=NOTIFY,h=HI,m=MID,l=LO). This can be used by itself or in\n"
      "                    combination with the -f, -h, -l, -m, -w, and -y options. If used\n"
      "                    with the other options, the affects of -d are applied first.\n"
      "  -f<\\*|PATTERN>  Enable fault trace for the specified Modules -\n"
      "                     \\* for all Modules or a list of patterns used to form the\n"
      "                     Module list. Takes precedence over -d.\n"
      "  -h<\\*|PATTERN>  Enable hi info trace for the specified Modules -\n"
      "                     \\* for all Modules or a list of patterns used to form the\n"
      "                     Module list. Takes precedence over -d, -f, -w, -y.\n"
      "  -l<\\*|PATTERN>  Enable low info trace for the specified Modules -\n"
      "                     \\* for all Modules or a list of patterns used to form the\n"
      "                     Module list. Takes precedence over -d, -f, -h, -m, -w, and -y.\n"
      "  -m<\\*|PATTERN>  Enable mid info trace for the specified Modules -\n"
      "                     \\* for all Modules or a list of patterns used to form the\n"
      "                     Module list. Takes precedence over -d, -f, -h, -w, and -y.\n"
      "  -n<\\*|PATTERN>  Names of running Apps to which to apply the CMD - \\* for all\n"
      "                     Apps or a list of patterns used to form the App list.\n"
      "  -p<\\*|LVL_LIST> print resulting trace levels.\n"
      "  -q<0|n>          display thread & queue parameters for the App. '0' for all\n"
      "                     threads/queues or number 'n' for particular thread/q.\n"
      "  -r <time_sec>    Repeat output every <time_sec> seconds.\n"
      "  -s               Enable 'static mode'. Allows viewing/setting trace levels\n"
      "                     of applications that are not running.\n"
      "  -t              Print timer count statistics for selected applications.\n"
      "  -w<\\*|PATTERN>  Enable warn trace for the specified Modules -\n"
      "                     \\* for all Modules or a list of patterns used to form the\n"
      "                     Module list. Takes precedence over -d, and -f.\n"
      "  -x              Reset message counts when reading them (-c option).\n"
      "  -y<\\*|PATTERN>  Enable notify trace for the specified Modules -\n"
      "                     \\* for all Modules or a list of patterns used to form the\n"
      "                     Module list. Takes precedence over -d, -f, and -w.\n"
      "\n"
      "NOTES:\n"
      "   - The '*' character has special meaning to the shell; so, it must be\n"
      "     'escaped' (preceded with a '\\') when this program is launched from a\n"
      "     shell prompt.\n"
      "\n"
      "   - PATTERNs are case-sensitive, comma-separated lists (with no spaces) and are\n"
      "     used to match names the same way the file glob '*PATTERN*' would be used. For\n"
      "     example, the PATTERN 'FOO,BAZ' would match 'FOO_BAR', 'A_FOO_BAR', 'A_BAR_FOO',\n"
      "     'BAZ_BAR', 'B_BAZ_BAR' and 'B_BAR_BAZ'.\n"
      "\n"
      "Thread & queue parameters:\n"
      "   TID       : XSAL Thread ID\n"
      "   OS        : Operating System thread id\n"
      "   R <+|->   : '+' thread is running, '-' thread is not running\n"
      "   ThreadName: Thread name\n"
      "   QSize     : message Queue Size\n"
      "   BSize     : pre-allocated Buffer Size\n"
      "   MCnt      : number of messages in the queue\n"
      "   PMCnt     : peek message number\n"
      "   PMSize    : size of the largest message\n"
      "   RcvCnt    : total # messages receive by thread\n");
}

/**
 * Get the SAL priority for a thread in another process.
 *
 * @param [in] pid The system PID of the thread to find the priority for
 * @param [out] priority Pointer to priority variable to fill
 */
#if defined (LINUX)
/* only works with Linux */
static bool_t Get_SAL_Thread_Priority(SAL_OS_TID_T pid, SAL_Priority_T* priority)
{
   bool_t success = false;

   if (pid > 0)
   {
#if defined (XSAL_POSIX)
      /* ONLY LINUX WAS TESTED */

      int policy;
      struct sched_param sched_p;
      SAL_Int_T status = 0;

      policy = sched_getscheduler(pid);
      if (policy < 0)
      {
         printf("Failed sched_getscheduler(%d) with error %s\n", (int) pid, strerror(errno));
      }
      else
      {
         status = sched_getparam(pid, &sched_p);
         if (status != 0)
         {
            printf("Failed sched_getparam(%d) with error %s\n", (int) pid, strerror(errno));
         }
         else
         {
#if defined (LINUX)
            if (SCHED_OTHER == policy)
            {
               /** using CFS - Get NICE level */
               int nice_level;

               nice_level = getpriority(PRIO_PROCESS, pid);
               *priority = (SAL_REAL_TIME_PRIORITY/2) - nice_level;
            }
            else /* using RR */
            {
               *priority = (SAL_REAL_TIME_PRIORITY - 1) + sched_p.sched_priority;
            }
#else
            *priority = sched_p.sched_priority;
#endif
         }
      }
      success = (bool_t)(status == EOK);

#elif defined (WIN32)
      /* NOT TESTED */

      *priority = GetThreadPriority(pid);
      success = (bool_t)(*priority != THREAD_PRIORITY_ERROR_RETURN);
#endif
   }

   return success;
}

static bool_t Get_Priority_String(SAL_OS_TID_T pid, char * priority_str, size_t dest_size)
{
   bool_t priority_succeeded = false;

   if (pid > 0)
   {
      SAL_Priority_T priority = 0;
      bool_t is_high_prio = false;
      char_t *prio_strs[] =
      { " ", "*" };

      priority_succeeded = Get_SAL_Thread_Priority(pid, &priority);
      if (priority_succeeded)
      {
         is_high_prio = priority >= SAL_REAL_TIME_PRIORITY_BASE;
         snprintf(priority_str, dest_size, "%3d%1s", priority, prio_strs[is_high_prio]);
      }
   }

   return priority_succeeded;
}
#endif

static void Out_Of_Memory_Exit(int32_t linenum)
{
   printf("ERROR: Unabled to allocated needed memory at line #%d. Exiting.\n", (int) linenum);
   exit(-1);
}

/**
 * Prints the queue stats for an XSAL thread.
 *
 * @param [in] thread_id The XSAL thread whose queue's stats are printed.
 * @param [in] qstat Pointer to DT_Queue_Stat_Rep_T structure containing the
 *                   queue stats that were reported.
 */
static void Print_Queue_Stat(SAL_Thread_Id_T thread_id, DT_Queue_Stat_Rep_T const * qstat)
{
   char_t t_name[THREAD_NAME_LEN_MAX];
   char_t t_prio[PRIO_STR_LEN_MAX];
   char_t no_thread_str[] = "%3d/-  -\n";
   char_t format_str[FORMAT_STR_LEN_MAX];
   bool_t has_priority;
   char_t t_os_id[5];

   if (qstat->os_tid > 0)
   {
      snprintf(t_os_id, sizeof(t_os_id), "%-4d", (int) qstat->os_tid);
   }
   else
   {
      snprintf(t_os_id, sizeof(t_os_id), "--  ");
   }

#ifdef __QNXNTO__
   {
      /* QNX does NOT have global tids, so Linux approach won't work */
      snprintf(t_prio, sizeof(t_prio), "%3d", qstat->thread_priority);
      has_priority = (qstat->thread_priority != -1);
   }
#else
   has_priority = Get_Priority_String(qstat->os_tid, t_prio, sizeof(t_prio));
#endif

   if (has_priority)
   {
      switch (qstat->queue_status)
      {
         case DT_THREAD_NOT_RUNNING:
            if (qstat->os_tid > 0)
            {
               printf(no_thread_str, thread_id);
            }
            break;

         case DT_QUEUE_NOT_CREATED:
            snprintf(t_name, sizeof(t_name), "\"%s\"", qstat->thread_name);
            snprintf(format_str, sizeof(format_str), "%%3d/%%4s %%4s + %%-%ds %%5s %%5s %%5s %%5s %%6s %%6s\n",
               SAL_THREAD_NAME_MAX_LENGTH + 2);
            printf(format_str, thread_id, t_os_id, t_prio, t_name, "-", "-", "-", "-", "-", "-");
            break;

         case DT_QUEUE_OK:
            snprintf(t_name, sizeof(t_name), "\"%s\"", qstat->thread_name);
            snprintf(format_str, sizeof(format_str), "%%3d/%%4s %%4s + %%-%ds %%5d %%5d %%5d %%5d %%6d %%6d\n",
               SAL_THREAD_NAME_MAX_LENGTH + 2);
            printf(format_str, thread_id, t_os_id, t_prio, t_name, qstat->queue_stat.queue_size, qstat->queue_stat.buffer_size,
               qstat->queue_stat.message_count, qstat->queue_stat.peak_message_count, qstat->queue_stat.peak_message_size,
               qstat->queue_stat.total_rcvd_msgs);
            break;

         default:
            break;
      }
   }
   else
   {
      switch (qstat->queue_status)
      {
         case DT_QUEUE_NOT_CREATED:
            snprintf(t_name, sizeof(t_name), "\"%s\"", qstat->thread_name);
            snprintf(format_str, sizeof(format_str), "%%3d/%%4s + %%-%ds %%5s %%5s %%5s %%5s %%6s\n",
            SAL_THREAD_NAME_MAX_LENGTH + 2);
            printf(format_str, thread_id, t_os_id, t_name, "-", "-", "-", "-", "-");
            break;

         case DT_QUEUE_OK:
            snprintf(t_name, sizeof(t_name), "\"%s\"", qstat->thread_name);
            snprintf(format_str, sizeof(format_str), "%%3d/%%4s + %%-%ds %%5d %%5d %%5d %%5d %%6d\n",
            SAL_THREAD_NAME_MAX_LENGTH + 2);
            printf(format_str, thread_id, t_os_id, t_name, qstat->queue_stat.queue_size, qstat->queue_stat.buffer_size,
               qstat->queue_stat.message_count, qstat->queue_stat.peak_message_count, qstat->queue_stat.peak_message_size);
            break;

         default:
            break;
      }
   }
}

/**
 * Prints the the header for queue stats of an XSAL thread. Looks like:
 *
 * "TID/OS R ThreadName     QSize BSize  MCnt PMCnt PMSize"
 */
static void Print_Queue_Stat_Header(void)
{
   char_t format_str[FORMAT_STR_LEN_MAX];

#if PRIORITY_SUPPORTED
   snprintf(format_str, sizeof(format_str), "TID/OS   Prio R %%-%ds QSize BSize  MCnt PMCnt PMSize RcvCnt\n",
      SAL_THREAD_NAME_MAX_LENGTH + 2);
#else
   snprintf(format_str, sizeof(format_str), "TID/OS   R %%-%ds QSize BSize  MCnt PMCnt PMSize RcvCnt\n",
      SAL_THREAD_NAME_MAX_LENGTH + 2);
#endif
   printf(format_str, "ThreadName");
}

/**
 * Reads the current dbg_trace levels for an XSAL application. These can come
 * from (in order of precedence): 1) The running XSAL application, 2) The
 * file used to save the persistent dbg_trace levels for the XSAL application,
 * and (if static mode is enabled) 3) The dbg_trace compile-time configurations
 * for this program.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 * @param [in] connection The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 * @param p_dbg_levels Dbg_Ctrl_Dbg_Levels_T structure into which the levels
 *                     are read.
 * @return true upon success; otherwise, false.
 */
static bool_t Read_App_Dbg_Levels(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection, uint16_t app_id,
   Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels)
{
   bool_t read_succeeded = false;
   /*
    * If the Name Service is running, always try to get the dbg levels through it first.
    */
   if (cmd_opt->ns_connection_is_ok)
   {
      read_succeeded = Read_Dbg_Levels_From_App(cmd_opt, connection, app_id, p_dbg_levels);
   }
   /*
    * If we couldn't get the information from the Name Service, then either it
    * isn't running or the XSAL app isn't running. Try to get the data from
    * the persistent file associated with the XSAL app.
    */
   if (!read_succeeded)
   {
      read_succeeded = Read_Dbg_Levels_From_File(cmd_opt, connection, app_id, p_dbg_levels);

      if (!read_succeeded && cmd_opt->static_run_enabled)
      {
         /*
          * At this point, the only option is to use the compile-time config
          * values for dbg_trace. These are valid as long as this program
          * (dbg_ctrl) was compiled with the same dbg_trace config as the
          * XSAL app in question. We only consider this an option if the
          * "static" mode of this program was enabled.
          */
         read_succeeded = Read_Default_Dbg_Levels(cmd_opt, p_dbg_levels);
      }
   }
   return read_succeeded;
}

/**
 * Reads the current dbg_trace levels from a running XSAL application using
 * a debug message to the router thread of that application.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 * @param [in] connection The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 * @param p_dbg_levels Dbg_Ctrl_Dbg_Levels_T structure into which the levels
 *                     are read.
 * @return true upon success; otherwise, false.
 */
static bool_t Read_Dbg_Levels_From_App(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection,
   uint16_t app_id, Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels)
{
   bool_t read_succeeded = true;
   DT_Get_Dbg_Levels_T dt_get_levels;
   size_t get_cmd_sz = sizeof(Dbg_Ctrl_Dbg_Levels_T) + (cmd_opt->num_module_ids - 1) * sizeof(uint8_t);
   dt_get_levels.command = DT_GET_DBG_LEVELS;

   if (!RT_Synch_Send(&connection, &dt_get_levels, sizeof(dt_get_levels), p_dbg_levels, get_cmd_sz))
   {
      printf("Failed to read debug levels from %s [App_id=%d]\n", cmd_opt->app_names[app_id], app_id);
      read_succeeded = false;
   }
   return read_succeeded;
}

/**
 * Reads the current dbg_trace levels for an XSAL application from the
 * file used to save its persistent dbg_trace levels.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 * @param [in] connection The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 * @param p_dbg_levels Dbg_Ctrl_Dbg_Levels_T structure into which the levels
 *                     are read.
 * @return true upon success; otherwise, false.
 */
static bool_t Read_Dbg_Levels_From_File(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection,
   uint16_t app_id, Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels)
{
   bool_t read_succeeded = true;
   size_t expected_size = cmd_opt->num_module_ids * sizeof(uint8_t);
   int fd;
   char path[PATH_MAX];

   SAL_Form_DT_PS_File_Name(path, PATH_MAX, SAL_Get_App_Name(app_id));

   fd = open(path, O_RDONLY);

   if (fd < 0)
   {
      read_succeeded = false;
   }
   else
   {
      struct stat buf;
      int err;

      err = fstat(fd, &buf);

      if (err != 0)
      {
         printf("fstat error of %s: %s\n", path, strerror(errno));
         read_succeeded = false;
      }
      else if ((size_t)buf.st_size != expected_size)
      {
         printf("Incorrect size (%d) for file. Expecting %d.\n", (int) buf.st_size, (int) expected_size);
         read_succeeded = false;
      }
      else
      {
         uint8_t *dbg_levels = p_dbg_levels->levels;
         size_t total_num_read = 0;

         while (total_num_read < expected_size)
         {
            ssize_t num_read = read(fd, (char*) dbg_levels + total_num_read, expected_size - total_num_read);

            if (num_read <= 0)
            {
               printf("read() from %s failed: %s\n", path, strerror(errno));
               break; /* out of while loop */
            }
            total_num_read += num_read;
         }

         if (total_num_read != expected_size)
         {
            printf("Wrong size of data read from %s.\n", path);
         }
         else
         {
            size_t mod_index;

            err = 0;

            for (mod_index = 0; mod_index < expected_size; mod_index++)
            {
               if (dbg_levels[mod_index] > TR_LVL_INFO_LO)
               {
                  err++;
                  break;
               }
            }

            if (0 == err)
            {
               printf("Read saved dbg_trace levels from %s.\n", path);
            }
            else
            {
               printf("Persistent dbg_trace level out of range - ignoring file %s.\n", path);
               read_succeeded = false;
            }
         }
      }

      close(fd);
   }

   p_dbg_levels->num_levels = cmd_opt->num_module_ids;

   return read_succeeded;
}

/**
 * Reads the dbg_trace levels for an XSAL application from the static dbg_trace
 * configuration.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 * @param p_dbg_levels Dbg_Ctrl_Dbg_Levels_T structure into which the levels
 *                     are read.
 * @return true upon success; otherwise, false.
 */
static bool_t Read_Default_Dbg_Levels(Cmd_Opt_T const * cmd_opt, Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels)
{
   size_t expected_size = cmd_opt->num_module_ids * sizeof(uint8_t);
   size_t mod_index;
   uint8_t *dbg_levels = p_dbg_levels->levels;

   for (mod_index = 0; mod_index < expected_size; mod_index++)
   {
      dbg_levels[mod_index] = Default_Trace_Levels[mod_index];
   }
   printf("Using default dbg_trace levels from program configuration.\n");

   p_dbg_levels->num_levels = cmd_opt->num_module_ids;

   return true;
}

/**
 * Sends a special diagnostic message to an XSAL applications router thread
 * and waits for the reply.
 *
 * @return true if the exchange had no errors; otherwise, false.
 *
 * @param [in,out] connection The connection to the XSAL application
 * @param [in] data pointer to the message data to send
 * @param [in] data_size number of bytes in message to send
 * @param [out] reply_data points to buffer into which reply is to be placed
 * @param [in] reply_data_size size of reply buffer
 */
static bool_t RT_Synch_Send(SAL_Connection_T* connection, const void* data, size_t data_size,
   void* reply_data, size_t reply_data_size)
{
   int32_t status;
   bool_t is_ok;
   SAL_IOV_T send_iov[2];
   SAL_Message_T message;
   struct timeb current_time;

   ftime(&current_time);
   memset(&message, 0, sizeof(SAL_Message_T));
   message.event_id = EV_RT_DT_Command;
   message.receiver_thread_id = SAL_ROUTER_THREAD_ID;
   message.timestamp_sec = current_time.time;
   message.timestamp_msec = current_time.millitm;
   message.data_size = data_size;
   message.data = (void*) data;

   send_iov[0].iov_base = &message;
   send_iov[0].iov_len = sizeof(message);

   send_iov[1].iov_base = (void*) data;
   send_iov[1].iov_len = data_size;

   is_ok = SAL_Sync_Send_VS(connection, send_iov, sizeof(send_iov) / sizeof(SAL_IOV_T), reply_data,
      reply_data_size, &status);
   return is_ok;
}

/**
 * Writes the dbg_trace levels for an XSAL application to the file used to
 * save its persistent dbg_trace levels.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 * @param [in] connection The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 * @param p_dbg_levels Dbg_Ctrl_Dbg_Levels_T structure into which the levels
 *                     are read.
 * @return true upon success; otherwise, false.
 */
static bool_t Write_Dbg_Levels_To_File(Cmd_Opt_T const * cmd_opt, SAL_Connection_T connection,
   uint16_t app_id, Dbg_Ctrl_Dbg_Levels_T *p_dbg_levels)
{
   size_t expected_size = cmd_opt->num_module_ids * sizeof(uint8_t);
   int fd;
   uint8_t *levels = p_dbg_levels->levels;
   char path[PATH_MAX];
   bool_t write_succeeded = true;

   SAL_Form_DT_PS_File_Name(path, PATH_MAX, SAL_Get_App_Name(app_id));

   fd = open(path, (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH));

   if (fd < 0)
   {
      printf("Error opening %s: %s\n", path, strerror(errno));
      write_succeeded = false;
   }
   else
   {
      size_t total_num_written = 0;

      while (total_num_written < expected_size)
      {
         ssize_t num_written = write(fd, (char*) levels + total_num_written,
            expected_size - total_num_written);

         if (num_written <= 0)
         {
            printf("write() to %s failed: %s\n", path, strerror(errno));
            break; /* out of while loop */
         }
         total_num_written += num_written;
      }

      if (total_num_written != expected_size)
      {
         if (ftruncate(fd, 0) != 0)
         {
            printf("ftruncate() of %s failed: %s\n", path, strerror(errno));
         }
         write_succeeded = false;
      }

      if (fsync(fd) != 0)
      {
         printf("fsync() of %s failed: %s\n", path, strerror(errno));
      }
      close(fd);
   }

   if (write_succeeded)
   {
      printf("Debug trace levels saved in %s.\n", path);
   }

   return write_succeeded;
}

/**
 * Writes the dbg_trace levels for an XSAL application. This can be done by
 * (in order of precedence): 1) Sending a debug trace message to the router
 * thread of the running XSAL application, or (if static mode is enabled)
 * 2) Writing to the file used to save the persistent dbg_trace levels for
 * the XSAL application.
 *
 * @param [in] cmd_opt Cmd_Opt_T structure containing the program options.
 * @param [in] connection The pre-established connection to the XSAL application
 *                        that is used to exchange data with it.
 * @param [in] app_id The ID of the XSAL application.
 * @param [in,out] p_set_cmd Dbg_Ctrl_Set_Dbg_Levels_T structure from which levels
 *                           are written.
 * @param [in] set_cmd_sz Total size of p_set_cmd structure.
 * @return true upon success; otherwise, false.
 */
static bool_t Write_App_Dbg_Levels(Cmd_Opt_T const *cmd_opt, SAL_Connection_T connection, uint16_t app_id,
   Dbg_Ctrl_Set_Dbg_Levels_T *p_set_cmd, size_t set_cmd_sz)
{
   bool_t write_succeeded = false;
   /*
    * If the Name Service is running, always try to set the dbg levels through it first.
    */
   if (cmd_opt->ns_connection_is_ok)
   {
      p_set_cmd->command = DT_SET_DBG_LEVELS;
      write_succeeded = RT_Synch_Send(&connection, p_set_cmd, set_cmd_sz, NULL, 0);
   }
   /*
    * If we couldn't set the information through the Name Service, then either it
    * isn't running or the XSAL app isn't running. If static mode is enabled, then
    * we can write directly to the persistent file.
    */
   if (!write_succeeded && cmd_opt->static_run_enabled)
   {
      write_succeeded = Write_Dbg_Levels_To_File(cmd_opt, connection, app_id, &p_set_cmd->dbg_levels);
   }
   return write_succeeded;
}

/**
 * Starting point for dbg_ctrl application.
 *
 * @return 0
 *
 * @param argc number of command line arguments (num elements in argv)
 * @param argv array of command line argument strings.
 */
int main(int argc, char* argv[])
{
   Cmd_Opt_T cmd_opt;

   Init_Cmd_Opt(&cmd_opt);

   if (Parse_Command_Line_Options(argc, argv, &cmd_opt))
   {
      SAL_NS_Data_T* ns_data;

      if (SAL_NS_Init_Proxy(&ns_data))
      {
         cmd_opt.ns_connection_is_ok = true;
         Get_NS_App_List(&cmd_opt);
         Get_NS_Module_List(&cmd_opt);
      }

      if (!cmd_opt.ns_connection_is_ok)
      {
         printf("Unable to connect to nameserver - ");

         if (!cmd_opt.static_run_enabled)
         {
            printf("Use -s option (static mode) to enable degraded capability\n\n");
            exit(-1);
         }
         else
         {
            printf("continuing with degraded capability\n\n");
         }
         Get_Local_App_List(&cmd_opt);
         Get_Local_Module_List(&cmd_opt);
      }

      if (cmd_opt.opt_a_optarg != NULL)
      {
         Print_Apps(&cmd_opt);
      }
      if (cmd_opt.opt_b_optarg != NULL)
      {
         Print_Module_Names(&cmd_opt);
      }

      Extract_Selected_Module_Ids(&cmd_opt);

      if ((cmd_opt.opt_n_optarg == NULL) && Any_Configure_Options(&cmd_opt))
      {
         cmd_opt.opt_n_optarg = strdup("*"); /* default to all apps */
      }

      if (cmd_opt.opt_n_optarg != NULL)
      {
         Extract_Selected_App_Ids(&cmd_opt);
         while (1)
         {
            Configure_Selected_Apps(&cmd_opt);
            cmd_opt.first_pass = false;

            if (0 == cmd_opt.repeat_sec)
            {
               break;
            }
            else
            {
               sleep(cmd_opt.repeat_sec);
            }
         }
      }
   }
   else
   {
      Print_Help(&cmd_opt);
   }

   free(cmd_opt.app_is_running);
   free(cmd_opt.app_names);
   free(cmd_opt.module_names);
   free(cmd_opt.opt_a_optarg);
   free(cmd_opt.opt_b_optarg);
   free(cmd_opt.opt_f_optarg);
   free(cmd_opt.opt_h_optarg);
   free(cmd_opt.opt_l_optarg);
   free(cmd_opt.opt_m_optarg);
   free(cmd_opt.opt_n_optarg);
   free(cmd_opt.opt_w_optarg);
   free(cmd_opt.opt_y_optarg);
   free(cmd_opt.requested_fault_lvl_modules);
   free(cmd_opt.requested_hi_lvl_modules);
   free(cmd_opt.requested_lo_lvl_modules);
   free(cmd_opt.requested_mid_lvl_modules);
   free(cmd_opt.requested_notify_lvl_modules);
   free(cmd_opt.requested_warn_lvl_modules);
   free(cmd_opt.selected_app_ids);
   return 0;
}

/*===========================================================================*/
/*!
 * @file dbg_ctrl.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 10-Jan-2011 Kirk Bailey
 *   - Created initial file using "dt" program from DB48x as a starting point.
 *
 * - 20-Feb-2011 Kirk Bailey
 *   - Change -p option to take letters or numbers for debug levels.
 *
 * - 13-Jun-2011 Kirk Bailey
 *   - Replace sprintf with snprintf, strcmp with strncmp.
 *   - Added -f and -w options.
 *
 * - 22-Jun-2011 Kirk Bailey
 *   - Added "static mode" to allow limited reading/writing of dbg_trace levels
 *   for non-running applications.
 *
 * - 29-Sep-2011 Kevin Kaster
 *   - Added "Prio" field, only tested for Linux.
 *   - Only display threads that exist.
 *   - Add return before printing each Application's information.
 *
 * - 08-Nov-2011 Kirk Bailey
 *   - Added support for new dbg_trace level: NOTIFY.
 *
 * - 06-Jan-2012 Dan Carman
 *   - Change to default to all apps for selection
 *
 * - 07-Jan-2012 Kirk Bailey
 *   - Added -c (message Counts) and -r (repeat) options. Added display of
 *     thread receive count.
 *
 * - 09-Jan-2012 Kirk Bailey
 *   - Fixed logic that defaults to "-n \*" to work with all configuration
 *     options.
 *
 * - 05-Jun-2012 Kirk Bailey
 *   - Fixed Klockwork issues.
 *
 * - 15-Aug-2012 Kirk Bailey
 *   - Fixed more Klockwork issues. Changed snprintf size arguments to use
 *     sizeof() instead of constants when possible.
 *
 * - 27-Aug-2012 Dan Carman
 *    - Added -t option to display count of timers
 *
 * - 26-Sep-2013 Larry Piekarski Rev 19
 *   - Task 1334: Reversed version control comments - no functional change
 *
 * - 26-Sep-2013 Larry Piekarski Rev 20
 *   - Task 1334: Fixed compiler warnings.
 *
 * - 30-Aug-2014 Kirk Bailey Rev 21
 *   - Fixed Klocwork defects.
 *
 * - 31-Aug-2014 Kirk Bailey Rev 22
 *   - Fixed remaining Klocwork defects.
 *
 * - 28-May-2015 Kirk Bailey Rev 23, 24
 *   - Resolved outstanding XSAL interface issues.
 *
 * - 18-Aug-2015 Kirk Bailey Rev 25
 *   - Fixed Coverity defect: missing break.
 *
 * - 9-May-2016 Dan Carman
 *   - Task kok_basa#247372 - Fix report of QNX priorities on queue status
 */
/*===========================================================================*/
