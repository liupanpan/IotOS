/* -------------------------------------------------------------------------
 *
 * File:        dt.c
 *
 * Copyright:   2006-2015 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman(at)delphi.com)
 *
 * Description: Debug Console Tool application
 *
 * -------------------------------------------------------------------------*/

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

#include "xsal.h"
#include "xsal_i_dt.h"
#include "xsal_i_ns_proxy.h"
#include "pi_unistd.h"
#include "debugtrace.h"
#include "string_resource.h"
#if defined(LINUX)
#include <strings.h>
#endif /* LINUX && WIN32 */

#if defined(LINUX) 
#define TID_LENGTH 5
#else
#define TID_LENGTH 2
#endif /* LINUX */

typedef struct Cmd_Opt_Tag
{
   /** -n<num>
    */
   SAL_Int_T app_id;

   /** -q<num>
    */
   SAL_Int_T queue_id;

   /** -v<num>
    */
   SAL_Int_T verbose;

   /** -g<num>
    */
   uint64_t group;
   bool set_group;

   /** -e<0|1>
    */
   SAL_Int_T enable_dt;

   /** -r<0|1>
    */
   bool read_dt_settings;

   /** -f<string>
    */
   char_t log_file_name[PATH_MAX + 1];
} Cmd_Opt_T;

typedef struct
{
   int app_id;
   char * const app_name;
} Application_Name_T;

#undef X
#define X(app_id,app_name) { app_id, #app_id },

static const Application_Name_T App_Names[] =
{
   SAL_APPLICATION_IDS
};

static const DT_Group_T DT_Groups[] = DT_GROUPS;

static void Usage(void);
static bool Get_Group_Bits(const char_t* grp_str, uint64_t* grp_mask);
static void Show_Apps(void);
static void Show_DT_Groups(void);
static SAL_Int_T Get_App_Id(const char_t* app_name);
static const char_t* Get_App_Name(SAL_App_Id_T app_id);
static bool Get_Val(const char_t* s, SAL_Int_T* val);
static void Init_Cmd_Opt(Cmd_Opt_T* cmd_opt);
static bool Parse_Command_Line_Options(SAL_Int_T argc, char_t* argv[], Cmd_Opt_T* cmd_opt);
static bool RT_Synch_Send(SAL_Connection_T* connection, const void* data, size_t data_size, void* reply_data,
   size_t reply_data_size);
static void Print_DT_Settings(const DT_Settings_T* dt_settings);
static void Print_Queue_Stat_Header(void);
static void Print_Queue_Stat(SAL_Thread_Id_T thread_id, const DT_Queue_Stat_Rep_T* queue_stat_rep);
static bool Get_Queue_Stat(SAL_Connection_T* connection, SAL_Thread_Id_T thread_id);
static void Execute_Command_Options(const SAL_App_Id_T* apps_tab, const Cmd_Opt_T* cmd_opt);

static void Usage(void)
{
   (void) printf("dt -h | -a | -b | -n<AppName | 0> [-e<0|1>] [-g<group_name>]\n"
      "    [-v<0..3>] [-q<1..n | 0>] [-r] [-f<file_name>]\n"
      "\n"
      "Description:\n"
      "  -n<AppName>   name of application or 0 for all applications\n"
      "  -e<0|1>       disable/enable logger \n"
      "  -g<GroupName> set group to trace\n"
      "  -v<0...3>     set the level of verbosity\n"
      "  -q<1..n | 0>  display thread & queue parameters for application\n"
      "                0 for all or number n for particular queue\n"
      "  -r            display DebugTrace parameters set in an application\n"
      "  -f<FileName>  set the path for DebugTrace output stream\n"
      "  -h            print this help\n"
      "  -a            add to help information about defined applications\n"
      "  -b            add to help information about defined groups\n"
      "\n"
      "NOTE:\n"
      "   AppName can be given as string or as number (e.g. -n procman or -n 1)\n"
      "   GroupName can be given as comma separated strings (e.g. -g DT_SAL,DT_PM),\n"
      "   as hex number, (e.g. -gf0) or -g-1 to specify all groups.\n"
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
      "   Prio      : thread priority\n"
      "   Sched     : thread scheduling policy\n");
}

/**
 *  Change name or hex representation of groups to Debug in an application,
 *  given as parameter from command line into uint64_t value
 *
 *  @param inNames  string with names of groups
 *
 *  @return groupAsBits  uint64_t value
 */
static bool Get_Group_Bits(const char_t* grp_str, uint64_t* grp_mask)
{
   bool symbolic_name = false;
   char_t* s;

   do
   {
      bool search = true;
      size_t dt_name_idx;

      s = strchr(grp_str, ',');
      if (s != NULL)
      {
         *s = '\0';
      }

      for (dt_name_idx = 0; search && (dt_name_idx < (sizeof(DT_Groups) / sizeof(DT_Groups[0]))); dt_name_idx++)
      {
         if (strcmp(DT_Groups[dt_name_idx].name, grp_str) == 0)
         {
            *grp_mask |= DT_Groups[dt_name_idx].value;
            symbolic_name = true;
            search = false;
         }
      }

      if (s != NULL)
      {
         grp_str = s + 1;
      }

   } while (s != NULL);

   if (!symbolic_name)
   {
      /* check if given string is a valid number in hex */
#if defined (_MSC_VER)
      symbolic_name = (bool)(sscanf(grp_str, "%I64x", grp_mask) == 1);
#else
      symbolic_name = (bool) (sscanf(grp_str, "%" PRIx64, grp_mask) == 1);
#endif
   }
   return symbolic_name;
}

static void Show_Apps(void)
{
   size_t i;
   (void) printf("# AppId\tAppName\n");
   for (i = 0; i < (sizeof(App_Names) / sizeof(Application_Name_T)); i++)
   {
      (void) printf("%-d\t%s\n", App_Names[i].app_id, App_Names[i].app_name);
   }
}

static void Show_DT_Groups(void)
{
   size_t i;

   (void) printf("# Hex Value      Symbolic Name\n");
   for (i = 0; i < (sizeof(DT_Groups) / sizeof(DT_Group_T)); i++)
   {
#if defined (_MSC_VER)
      (void)printf("%-16I64x %s\n", DT_Groups[i].value, DT_Groups[i].name);
#else
      (void) printf("%-16" PRIx64 " %s \n", DT_Groups[i].value, DT_Groups[i].name);
#endif
   }
}

static SAL_Int_T Get_App_Id(const char_t* app_name)
{
   size_t i;
   bool search = true;
   SAL_Int_T a_id = -1;

   for (i = 0; search && (i < (sizeof(App_Names) / sizeof(Application_Name_T))); i++)
   {
      if (stricmp(app_name, App_Names[i].app_name) == 0)
      {
         a_id = App_Names[i].app_id;
         search = false;
      }
   }
   return a_id;
}

static const char_t* Get_App_Name(SAL_App_Id_T app_id)
{
   size_t i;
   bool search = true;
   const char_t* ap_name = "";

   for (i = 0; search && (i < (sizeof(App_Names) / sizeof(Application_Name_T))); i++)
   {
      if (App_Names[i].app_id == app_id)
      {
         ap_name = App_Names[i].app_name;
         search = false;
      }
   }
   return ap_name;
}

static bool Get_Val(const char_t* s, SAL_Int_T* val)
{
   bool status = true;
   if (strcmp(s, "0") == 0)
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
         status = false;
      }
   }
   return status;
}

static void Init_Cmd_Opt(Cmd_Opt_T* cmd_opt)
{
   (void) memset(cmd_opt, 0, sizeof(Cmd_Opt_T));

   cmd_opt->app_id = -1;
   cmd_opt->queue_id = -1;
   cmd_opt->enable_dt = -1;
   cmd_opt->verbose = -1;
   cmd_opt->group = 0;
   cmd_opt->set_group = 0;
   cmd_opt->read_dt_settings = false;

   cmd_opt->log_file_name[0] = '\0';
   cmd_opt->log_file_name[PATH_MAX] = '\0';
}

static bool Parse_Command_Line_Options(SAL_Int_T argc, char_t* argv[], Cmd_Opt_T* cmd_opt)
{
   bool status = true;
   SAL_Int_T option;

   while ((bool) ((option = pi_getopt(argc, argv, "n:q:v:g:e:f:rhab")) != -1))
   {
      switch (option)
      {
         case 'n':
            if (!Get_Val(pi_optarg, &cmd_opt->app_id))
            {
               SAL_Int_T tmp_app_id = Get_App_Id(pi_optarg);
               if (tmp_app_id >= 0)
               {
                  cmd_opt->app_id = tmp_app_id;
               }
               else
               {
                  (void) printf("Application name '%s' not recognized\n", pi_optarg);
                  status = false;
               }
            }
            break;

         case 'q':
            if (!Get_Val(pi_optarg, &cmd_opt->queue_id))
            {
               (void) printf("Invalid Queue Id: %s\n", pi_optarg);
               status = false;
            }
            break;

         case 'v':
            if (!Get_Val(pi_optarg, &cmd_opt->verbose))
            {
               (void) printf("Invalid Verbose Level: %s\n", pi_optarg);
               status = false;
            }
            break;

         case 'g':
            cmd_opt->set_group = Get_Group_Bits(pi_optarg, &cmd_opt->group);
            break;

         case 'e':
            if ((!Get_Val(pi_optarg, &cmd_opt->enable_dt)) || ((cmd_opt->enable_dt != 0) && (cmd_opt->enable_dt != 1)))
            {
               (void) printf("Invalid Enable_DT option: %s\n", pi_optarg);
               status = false;
            }
            break;

         case 'f':
            Str_Full_Copy(cmd_opt->log_file_name, pi_optarg, sizeof(cmd_opt->log_file_name));
            break;

         case 'r':
            cmd_opt->read_dt_settings = true;
            break;

         case 'h':
            Usage();
            break;

         case 'a':
            Show_Apps();
            break;

         case 'b':
            Show_DT_Groups();
            break;

         default:
            break;
      }
   }
   return status;
}

static bool RT_Synch_Send(SAL_Connection_T* connection, const void* data, size_t data_size, void* reply_data,
   size_t reply_data_size)
{
   int32_t status;
   bool send_status = false;
   SAL_IOV_T send_iov[2];
   SAL_Message_T message;
   struct timeb current_time;

   (void) ftime(&current_time);
   (void) memset(&message, 0, sizeof(SAL_Message_T));
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

   if (SAL_Sync_Send_VS(connection, send_iov, sizeof(send_iov) / sizeof(SAL_IOV_T), reply_data, reply_data_size, &status))
   {
      send_status = true;
   }
   return send_status;
}

static void Print_DT_Settings(const DT_Settings_T* dt_settings)
{
   (void) printf("DebugTrace settings:\n");
   (void) printf("Enable  %d\n"
      "TmStamp %d\n"
#if defined (_MSC_VER)
      "Groups  %I64x\n"
#else
      "Groups  %" PRIx64 "\n"
#endif
      "Level   %d\n",

   dt_settings->logger_enable, dt_settings->time_stamp_enable, dt_settings->group_mask, dt_settings->verbosity_level);
}

static void Print_Queue_Stat_Header(void)
{
   char_t format_str[80 + 1];

   (void) snprintf(format_str, 80, "TID/%%-%ds R %%-%ds QSize BSize  MCnt PMCnt PMSize Prio Sched\n", TID_LENGTH,
   SAL_THREAD_NAME_MAX_LENGTH + 2);
   format_str[80] = '\0';

   /* Assuming that TID_LENGTH is 2 and SAL_THREAD_NAME_MAX_LENGTH is 8, 
    *  the format_str should look like this:
    * 
    * "TID/%-2s R %-10s QSize BSize  MCnt PMCnt PMSize Prio Sched\n"
    */
   (void) printf(format_str, "OS", "ThreadName");
}

static const char_t* Get_Sched_Policy(SAL_Sched_Policy_T sched_policy)
{
   const char_t* policy;

   switch (sched_policy)
   {
#if defined(SCHED_RR)
      case SCHED_RR:
      policy = "RR";
      break;
#endif /* SCHED_RR */

#if defined(SCHED_FIFO)
      case SCHED_FIFO:
      policy = "FF";
      break;
#endif /* SCHED_FIFO */

#if defined(SCHED_OTHER)
      case SCHED_OTHER:
      policy = "Other";
      break;
#endif /* SCHED_OTHER */

      default:
         policy = "-";
         break;
   }
   return policy;
}

static void Print_Queue_Stat(SAL_Thread_Id_T thread_id, const DT_Queue_Stat_Rep_T* queue_stat_rep)
{
   char_t t_name[32]; /* SAL_THREAD_NAME_MAX_LENGTH */
   char_t format_str[80];

   (void) snprintf(t_name, sizeof(t_name),"\"%s\"", queue_stat_rep->thread_name);

   switch (queue_stat_rep->queue_status)
   {
      case DT_THREAD_NOT_RUNNING:
         (void) snprintf(format_str, sizeof(format_str),"%%3d/%%-%ds - %%-%ds %%5s %%5s %%5s %%5s %%6s %%4s %%5s\n", TID_LENGTH,
         SAL_THREAD_NAME_MAX_LENGTH + 2);
         /* Assuming that TID_LENGTH is 2 and SAL_THREAD_NAME_MAX_LENGTH is 8,
          *  the format_str should look like this:
          *
          * "%3d/%-2s - %-10s %5s %5s %5s %5s %6s %4s %s\n"
          */
         (void) printf(format_str, thread_id, "-", "", "-", "-", "-", "-", "-", "-", "-");
         break;

      case DT_QUEUE_NOT_CREATED:
         (void) snprintf(format_str, sizeof(format_str),"%%3d/%%-%dd + %%-%ds %%5s %%5s %%5s %%5s %%6s %%4d %%5s\n", TID_LENGTH,
         SAL_THREAD_NAME_MAX_LENGTH + 2);
         /* Assuming that TID_LENGTH is 2 and SAL_THREAD_NAME_MAX_LENGTH is 8,
          *  the format_str should look like this:
          *
          * "%3d/%-2d + %-10s %5s %5s %5s %5s %6s %4d %s\n"
          */
         (void) printf(format_str, thread_id, queue_stat_rep->os_tid, t_name, "-", "-", "-", "-", "-",
            queue_stat_rep->thread_priority, Get_Sched_Policy(queue_stat_rep->sched_policy));
         break;

      case DT_QUEUE_OK:
         (void) snprintf(format_str, sizeof(format_str),"%%3d/%%-%dd + %%-%ds %%5d %%5d %%5d %%5d %%6d %%4d %%5s\n", TID_LENGTH,
         SAL_THREAD_NAME_MAX_LENGTH + 2);
         /* Assuming that TID_LENGTH is 2 and SAL_THREAD_NAME_MAX_LENGTH is 8,
          *  the format_str should look like this:
          *
          * "%3d/%-2d + %-10s %5d %5d %5d %5d %6d %4d %s\n"
          */
         (void) printf(format_str, thread_id, queue_stat_rep->os_tid, t_name, queue_stat_rep->queue_stat.queue_size,
            queue_stat_rep->queue_stat.buffer_size, queue_stat_rep->queue_stat.message_count,
            queue_stat_rep->queue_stat.peak_message_count, queue_stat_rep->queue_stat.peak_message_size,
            queue_stat_rep->thread_priority, Get_Sched_Policy(queue_stat_rep->sched_policy));
         break;

      default:
         (void) printf("Response error\n");
         break;
   }
}

static bool Get_Queue_Stat(SAL_Connection_T* connection, SAL_Thread_Id_T thread_id)
{
   bool status = false;
   DT_Get_Queue_Stat_T queu_stat_req = { 0 };
   DT_Queue_Stat_Rep_T queu_stat_rep = { 0 };;

   queu_stat_req.command = GET_QUEUE_STAT;
   queu_stat_req.thread_id = thread_id;

   if (RT_Synch_Send(connection, &queu_stat_req, sizeof(DT_Get_Queue_Stat_T), &queu_stat_rep, sizeof(DT_Queue_Stat_Rep_T)))
   {
      Print_Queue_Stat(thread_id, &queu_stat_rep);
      status = true;
   }
   return status;
}

static void Execute_Command_Options(const SAL_App_Id_T* apps_tab, const Cmd_Opt_T* cmd_opt)
{
   size_t app_index;
   SAL_Connection_T connection;

   for (app_index = 0u; apps_tab[app_index] != 0; app_index++)
   {
      SAL_App_Id_T ap_id = apps_tab[app_index];
      if (SAL_Connect(ap_id, SAL_ROUTER_THREAD_ID, &connection))
      {
         (void) printf("Application %s [App_id=%d]\n", Get_App_Name(ap_id), ap_id);
         if (cmd_opt->enable_dt != -1)
         {
            DT_Set_Logger_T logger;

            logger.command = SET_DT_LOGGER;
            logger.enable = cmd_opt->enable_dt;

            if (!RT_Synch_Send(&connection, &logger, sizeof(DT_Set_Logger_T), NULL, 0))
            {
               (void) printf("Send message to %s [App_id=%d] failed\n", Get_App_Name(ap_id), ap_id);
               continue;
            }
         }

         if (cmd_opt->set_group != 0)
         {
            DT_Set_Groups_T groups;

            groups.command = SET_DT_GROUPS;
            groups.groups = cmd_opt->group;

            if (!RT_Synch_Send(&connection, &groups, sizeof(DT_Set_Groups_T), NULL, 0))
            {
               (void) printf("Send message to %s [App_id=%d] failed\n", Get_App_Name(ap_id), ap_id);
               continue;
            }
         }

         if (cmd_opt->verbose != -1)
         {
            DT_Set_Verbosity_T v;

            v.command = SET_DT_VERBOSITY;
            v.level = cmd_opt->verbose;

            if (!RT_Synch_Send(&connection, &v, sizeof(DT_Set_Verbosity_T), NULL, 0))
            {
               (void) printf("Send message to %s [App_id=%d] failed\n", Get_App_Name(ap_id), ap_id);
               continue;
            }
         }

         if (cmd_opt->read_dt_settings)
         {
            DT_Get_DT_Settings_T dt_settings = { 0 };
            DT_Settings_T dt_settings_reply = { 0 };

            dt_settings.command = GET_DT_SETTINGS;

            if (RT_Synch_Send(&connection, &dt_settings, sizeof(DT_Get_DT_Settings_T), &dt_settings_reply, sizeof(DT_Settings_T)))
            {
               Print_DT_Settings(&dt_settings_reply);
            }
            else
            {
               (void) printf("Send message to %s [App_id=%d] failed\n", Get_App_Name(ap_id), ap_id);
            }
         }

         if (cmd_opt->queue_id != -1)
         {
            DT_Get_Thread_Number_T dt_threads_num_req = { 0 };
            DT_Threads_Num_Rep_T dt_threads_num_rep = { 0 };

            dt_threads_num_req.command = GET_THREADS_NUM;

            if (RT_Synch_Send(&connection, &dt_threads_num_req, sizeof(DT_Get_Thread_Number_T), &dt_threads_num_rep,
               sizeof(DT_Threads_Num_Rep_T)))
            {
               if (dt_threads_num_rep.thread_num > 0u)
               {
                  if (cmd_opt->queue_id == 0)
                  {
                     SAL_Thread_Id_T t_id;

                     Print_Queue_Stat_Header();
                     for (t_id = 0; (size_t) t_id <= dt_threads_num_rep.thread_num; t_id++)
                     {
                        (void) Get_Queue_Stat(&connection, t_id);
                     }

                  }
                  else if (cmd_opt->queue_id <= (SAL_Int_T) dt_threads_num_rep.thread_num)
                  {
                     Print_Queue_Stat_Header();
                     (void) Get_Queue_Stat(&connection, cmd_opt->queue_id);
                  }
                  else
                  {
                     (void) printf("TID=%d is out of range. Max valid TID is %" PRIu64 "n", cmd_opt->queue_id,
                        dt_threads_num_rep.thread_num);
                  }
               }
            }
            else
            {
               (void) printf("Send message to %s [App_id=%d] failed\n", Get_App_Name(ap_id), ap_id);
            }
         }

         if (cmd_opt->log_file_name[0] != '\0')
         {
            DT_Set_Output_Stream_T output_stream;

            output_stream.command = SET_DT_OUTPUT_STREAM;
            Str_Full_Copy(output_stream.output_stream, cmd_opt->log_file_name, sizeof(output_stream.output_stream));

            if (!RT_Synch_Send(&connection, &output_stream, sizeof(output_stream), NULL, 0))
            {
               (void) printf("Send message to %s [App_id=%d] failed\n", Get_App_Name(ap_id), ap_id);
               continue;
            }
         }
         (void) SAL_Disconnect(&connection);
      }
      else
      {
         (void) printf("Couldn't connect to %s [App_Id=%d]\n", Get_App_Name(ap_id), ap_id);
      }
   }
}

int main(SAL_Int_T argc, char_t* argv[])
{
   Cmd_Opt_T cmd_opt;

   Init_Cmd_Opt(&cmd_opt);

   if (Parse_Command_Line_Options(argc, argv, &cmd_opt))
   {
      SAL_NS_Data_T* ns_data = NULL;
      SAL_NS_Init_Proxy(&ns_data);
      if ((ns_data != NULL) && (cmd_opt.app_id >= 0))
      {
         SAL_App_Id_T Router_Threads_Ids[SAL_MAX_APPS + 1] = { 0 };

         if (cmd_opt.app_id == 0)
         {
            if (!SAL_NS_Get_Running_App_Ids(Router_Threads_Ids))
            {
               (void) printf("Couldn't get list of running applications. Is procman running the name server?\n");
            }
         }
         else
         {
            Router_Threads_Ids[0] = (SAL_App_Id_T) cmd_opt.app_id;
            Router_Threads_Ids[1] = 0;
         }
         Execute_Command_Options(Router_Threads_Ids, &cmd_opt);
      }
   }
   else
   {
      Usage();
   }

   return 0;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman(at)delphi.com)
 *   Initial revision.
 *
 * 02-Jul-2015, Kirk Bailey
 *   Eliminated warnings and Klocwork defects.
 *
 * 21-Aug-2015, Kirk Bailey
 *   Fixed Cygwin build.
 *
 * -----------------------------------------------------------------------*/
