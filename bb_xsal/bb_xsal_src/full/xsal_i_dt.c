/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_dt.c
 *
 * Copyright:   2015 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the Debug Trace Console functionality.
 *
 * -------------------------------------------------------------------------*/

#include "debugtrace.h"
#include <stdio.h>
#include <string.h>
#include "string_resource.h"
#include "xsal_i_dt.h"
#include "xsal_i_config.h"
#include "xsal_i_thread.h"

EM_FILENUM(XSAL_IPC_MODULE_ID, 14); /* Identify file for trace/assert purposes */

#ifndef PATH_MAX
#   define PATH_MAX 1024
#endif

int SAL_Form_DT_PS_File_Name(char *path, size_t max_path, char const *app_name)
{
   return snprintf(path, max_path, "%s%s", XSAL_DT_PATH, app_name);
}

void SAL_I_Init_DT(void)
{
   char path[PATH_MAX];

   SAL_Form_DT_PS_File_Name(path, PATH_MAX, SAL_Get_App_Name(SAL_I_App_Id));
   Tr_Read_Trace_Levels(path);
}

SAL_Int_T SAL_I_Proccess_DT_Command(const DT_Command_T* command, void* reply_buffer)
{
   SAL_Int_T reply_size = 0;

   if (NULL == command)
   {
      Tr_Fault("NULL DT_Command_T");
   }
   else if (NULL == reply_buffer)
   {
      Tr_Fault("NULL DT command reply buffer");
   }
   else
   {
      SAL_Thread_Id_T tid;
      SAL_I_Thread_Attr_T* thread_attr;

      switch (command->command)
      {
         case DT_SET_OUTPUT_STREAM:
            /*Tr_Open_File(((DT_Set_Output_Stream_T*)command)->output_stream);*/
            break;

         case DT_GET_NUM_THREADS:
            ((DT_Threads_Num_Rep_T*) reply_buffer)->thread_num = SAL_I_Max_Number_Of_Threads;
            reply_size = sizeof(DT_Threads_Num_Rep_T);
            break;

         case DT_GET_QUEUE_STATUS:
         case GET_QUEUE_STAT:
            tid = ((const DT_Get_Queue_Stat_T*) (const void*) command)->thread_id;
            if ((tid >= 0) && (tid <= (SAL_Thread_Id_T) SAL_I_Max_Number_Of_Threads))
            {
               DT_Queue_Stat_Rep_T* rep = (DT_Queue_Stat_Rep_T*) reply_buffer;
               thread_attr = &SAL_I_Thread_Table[tid];

               if (thread_attr->thread_fnc == NULL)
               {
                  rep->queue_status = DT_THREAD_NOT_RUNNING;
               }
               else
               {
#if defined(LINUX)
                  rep->os_tid = thread_attr->os_pid; /** for Linux, this is PID */
#elif defined(XSAL_POSIX)
                  rep->os_tid = thread_attr->os_tid;
#else               
                  rep->os_tid = 0; /** don't report for others */
#endif               
                  (void) Str_Full_Copy(rep->thread_name, thread_attr->thread_name, sizeof(rep->thread_name));

                  if (!SAL_I_Get_Thread_Priority(tid, &rep->thread_priority))
                  {
                     rep->thread_priority = -1;
                  }

                  if (tid > 0)
                  {
                     if (SAL_Stat_Queue(tid, &rep->queue_stat))
                     {
                        rep->queue_status = DT_QUEUE_OK;
                     }
                     else
                     {
                        rep->queue_status = DT_QUEUE_NOT_CREATED;
                     }
                  }
                  else
                  {
                     /** Router thread doesn't have a queue
                      */
                     rep->queue_status = DT_QUEUE_NOT_CREATED;
                  }
               }
               reply_size = sizeof(DT_Queue_Stat_Rep_T);
            }
            else
            {
               reply_size = -1;
            }
            break;

         case DT_GET_DBG_LEVELS:
         {
            DT_Dbg_Levels_T *lvl_reply = (DT_Dbg_Levels_T*) reply_buffer;
            Tr_Module_ID_T module_id;

            lvl_reply->num_levels = TR_NUM_MODULE_IDS;

            for (module_id = TR_MODULE_UNKNOWN; module_id < TR_NUM_MODULE_IDS; module_id++)
            {
               lvl_reply->levels[module_id] = Tr_Get_Module_Info_Trace_Level(module_id);
            }
            reply_size = sizeof(*lvl_reply);
            break;
         }

         case DT_SET_DBG_LEVELS:
         {
            DT_Set_Dbg_Levels_T *set_cmd = (DT_Set_Dbg_Levels_T*) command;
            Tr_Module_ID_T module_id;
            bool_t all_lvls_same = true;
            uint8_t isa_lvl = 0;

            if (TR_NUM_MODULE_IDS == set_cmd->dbg_levels.num_levels)
            {
               char path[PATH_MAX];
               for (module_id = 0; module_id < TR_NUM_MODULE_IDS; module_id++)
               {
                  uint8_t lvl = set_cmd->dbg_levels.levels[module_id];
                  if (lvl <= TR_LVL_INFO_LO)
                  {
                     if (0 == module_id)
                     {
                        isa_lvl = lvl;
                     }
                     else if (lvl != isa_lvl)
                     {
                        all_lvls_same = false;
                        isa_lvl = lvl;
                     }
                     Tr_Set_Module_Info_Trace_Level(module_id, lvl);
                  }
               }
               SAL_Form_DT_PS_File_Name(path, PATH_MAX, SAL_Get_App_Name(SAL_I_App_Id));
               Tr_Save_Trace_Levels(path);
            }
            if (all_lvls_same) /* Allow "default" to control ISA debug levels */
            {
               switch (isa_lvl)
               {
                  case TR_LVL_INFO_HI:
                  case TR_LVL_INFO_LO:
                  case TR_LVL_INFO_MID:
                     /* Map TR_LVL_INFO_HI..LO to 1..3 */
                     isa_lvl -= (TR_LVL_INFO_HI - 1);
                     break;
                  default:
                     isa_lvl = 0;
                     break;
               }
               Tr_Set_Level(isa_lvl);
            }
            break;
         }

         case DT_GET_MSG_COUNTS:
         {
            DT_Get_Msg_Counts_T const *cmd = (const DT_Get_Msg_Counts_T*) command;
            DT_Msg_Counts_Rep_T* rep = (DT_Msg_Counts_Rep_T*) reply_buffer;

            SAL_I_Get_App_Msg_Count_Stats(&rep->msg_count_stats, cmd->reset_counts);
            reply_size = sizeof(*rep);
            break;
         }

         case DT_GET_TIMER_COUNTS:
         {
#if defined(LINUX)
            DT_Timer_Counts_Rep_T* rep = (DT_Timer_Counts_Rep_T*) reply_buffer;

            SAL_Get_Timer_Status(&rep->timer_count_stats);
            reply_size = sizeof(*rep);
#endif
            break;
         }

         case SET_DT_LOGGER:
            if (((const DT_Set_Logger_T*) (const void*) command)->enable == 0)
            {
               Tr_Disable();
            }
            else
            {
               Tr_Enable();
            }
            break;

         case SET_DT_GROUPS:
            Tr_Set_Group(((DT_Set_Groups_T*) command)->groups);
            break;

         case SET_DT_VERBOSITY:
            Tr_Set_Level(((DT_Set_Verbosity_T*) command)->level);
            break;

         case SET_DT_OUTPUT_STREAM:
            Tr_Open_File(((DT_Set_Output_Stream_T*)command)->output_stream);
            break;

         case GET_THREADS_NUM:
            ((DT_Threads_Num_Rep_T*) reply_buffer)->thread_num = SAL_I_Max_Number_Of_Threads;
            reply_size = sizeof(DT_Threads_Num_Rep_T);
            break;

         case GET_DT_SETTINGS:
            /* Clear reply_buffer just in case if DebugTrace is compiled out.
             * In that case macro Tr_Get_Settings does nothing.
             */
            (void) memset(reply_buffer, 0, sizeof(DT_Settings_T));

            Tr_Get_Settings((DT_Settings_T*) reply_buffer);
            reply_size = sizeof(DT_Settings_T);
            break;

         default:
            break;
      }
   }
   return reply_size;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 19-Jul-2013 Larry Piekarski
 * - Task 161388: Removed deprecated util functions.
 *
 * 01-Jul-2015 Kirk Bailey
 * - Added logic to allow ISA module trace levels to be controlled via
 *   a "default" level (a level directed to all module IDs).
 * - Added support for ISA "dt" commands.
 *
 * -----------------------------------------------------------------------*/
