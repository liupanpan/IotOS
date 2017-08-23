/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_dt.h
 *
 * Copyright:   2006-2015 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the XSAL Debug Trace functionality.
 *
 * -------------------------------------------------------------------------*/

#ifndef XSAL_I_DT_H
#define XSAL_I_DT_H

#include "dbg_trace.h"
#include "xsal.h"
#include "xsal_i_publish.h"

#if defined(LINUX)
#include "xsal_timer_linux.h"
#endif

#ifndef XSAL_DT_PATH
/**
 * Defines the path and common part of the name of the file used to save the
 * dbg_trace levels. This is the default value which can be overridden by
 * instead providing a definition in xsal_settings.h.
 */
#   define XSAL_DT_PATH "/var/.dbg_trace_lvls_"
#endif


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * List of dbg_trace Console commands
 */
typedef enum DT_Command_Ids_Tag
{
   /* BASA "dbg_trace" commands: */
   DT_GET_DBG_LEVELS = 1,
   DT_GET_NUM_THREADS,
   DT_GET_QUEUE_STATUS,
   DT_SET_DBG_LEVELS,
   DT_SET_OUTPUT_STREAM,
   DT_GET_MSG_COUNTS,
   DT_GET_TIMER_COUNTS,
   /* ISA "dt" commands: */
   SET_DT_LOGGER,
   SET_DT_TIMESTAMP,
   SET_DT_GROUPS,
   SET_DT_VERBOSITY,
   SET_DT_OUTPUT_STREAM,
   GET_DT_SETTINGS,
   GET_THREADS_NUM,
   GET_QUEUE_STAT,
} DT_Command_Ids_T;

typedef struct DT_Dbg_Levels_Tag
{
      size_t num_levels;
      uint8_t levels[TR_NUM_MODULE_IDS];
} DT_Dbg_Levels_T;

/* -------------------------------------------------------------------------
 *
 *                  dbg_trace Console "set" commands
 *
 * -----------------------------------------------------------------------*/

/**
 *  Data structure for set dbg_trace verbosity level command
 */
typedef struct DT_Set_Dbg_Levels_Tag
{
      DT_Command_Ids_T command;
      DT_Dbg_Levels_T dbg_levels;
} DT_Set_Dbg_Levels_T;

/**
 *  Data structure for set dbg_trace output stream command
 */
typedef struct DT_Set_Output_Stream_Tag
{
      DT_Command_Ids_T command;
      char_t output_stream[PATH_MAX + 1];
} DT_Set_Output_Stream_T;

/* -------------------------------------------------------------------------
 *
 *                 dbg_trace Console "get" commands
 *
 * -----------------------------------------------------------------------*/

/**
 *  Data structure for read dbg_trace settings command
 */
typedef struct DT_Get_Dbg_Levels_Tag
{
      DT_Command_Ids_T command;
} DT_Get_Dbg_Levels_T;

/**
 *  Data structure for get number of threads command
 */
typedef struct DT_Get_Thread_Number_Tag
{
      DT_Command_Ids_T command;
} DT_Get_Thread_Number_T;

/**
 *  Data structure for get queue statistic command
 */
typedef struct DT_Get_Queue_Stat_Tag
{
      DT_Command_Ids_T command;
      SAL_Thread_Id_T thread_id;
} DT_Get_Queue_Stat_T;

/**
 *  Data structure for get message counts command
 */
typedef struct DT_Get_Msg_Counts_Tag
{
      DT_Command_Ids_T command;
      bool_t           reset_counts;
} DT_Get_Msg_Counts_T;

/**
 *  Data structure for get timer counts command
 */
typedef struct DT_Get_Timer_Counts_Tag
{
      DT_Command_Ids_T command;
} DT_Get_Timer_Counts_T;

/* **********************
 * ISA dt commands
 *
 * **********************/

/**
 *  Data structure for read ISA DebugTrace settings command
 */
typedef struct DT_Get_DT_Settings_Tag
{
   DT_Command_Ids_T command;
} DT_Get_DT_Settings_T;

/**
 *  Data structure for set ISA DebugTrace groups command
 */
typedef struct DT_Set_Groups_Tag
{
   DT_Command_Ids_T command;
   uint64_t groups;
} DT_Set_Groups_T;

/**
 *  Data structure for ISA DebugTrace logging turn on/off command
 */
typedef struct DT_Set_Logger_Tag
{
   DT_Command_Ids_T command;
   bool enable;
} DT_Set_Logger_T;


/**
 *  Data structure for set ISA DebugTrace verbosity level command
 */
typedef struct DT_Set_Verbosity_T
{
   DT_Command_Ids_T command;
   SAL_Int_T level;
} DT_Set_Verbosity_T;


/*  ------------------------------------------------------------------------
 *
 *                dbg_trace Console reply data
 *
 * -----------------------------------------------------------------------*/

/**
 *  Data structure for "get number of threads" reply
 */
typedef struct DT_Thread_Number_Rep_Tag
{
      uint64_t thread_num;
} DT_Threads_Num_Rep_T;

typedef enum DT_Queue_Status_Tag
{
   DT_QUEUE_OK, DT_QUEUE_NOT_CREATED, DT_THREAD_NOT_RUNNING
} DT_Queue_Status_T;

/**
 *  Data structure for "get queue statistic" reply
 */
typedef struct DT_Queue_Stat_Rep_Tag
{
      DT_Queue_Status_T queue_status;
      SAL_OS_TID_T os_tid;
      SAL_Stat_Queue_T queue_stat;
      SAL_Sched_Policy_T sched_policy;
      SAL_Priority_T     thread_priority;
      char_t thread_name[SAL_THREAD_NAME_MAX_LENGTH];
} DT_Queue_Stat_Rep_T;

#if defined(LINUX)
/**
 *  Data structure for "get timer counts" reply
 */
typedef struct DT_Timer_Counts_Rep_Tag
{
      SAL_Timer_Status_T timer_count_stats;
} DT_Timer_Counts_Rep_T;
#endif

/**
 *  Data structure for "get message counts" reply
 */
typedef struct DT_Msg_Counts_Rep_Tag
{
   SAL_App_Msg_Count_Stats_T msg_count_stats;
} DT_Msg_Counts_Rep_T;

/** Union DC_Command_T groups all dbg_trace Console application commands
 */
typedef union DT_Command_Tag
{
      DT_Command_Ids_T command;
      DT_Get_Queue_Stat_T get_queue_statistic;
      DT_Get_Dbg_Levels_T get_levels;
      DT_Get_Thread_Number_T get_threads_number;
      DT_Set_Dbg_Levels_T set_levels;
      DT_Set_Output_Stream_T set_output_stream;
      DT_Get_Msg_Counts_T get_msg_counts;
} DT_Command_T;

/** Union DC_Reply_Data_T groups all dbg_trace Console application replys
 */
typedef union DT_Reply_Data_Tag
{
      DT_Dbg_Levels_T dbg_levels;
      DT_Queue_Stat_Rep_T queue_stat;
      DT_Threads_Num_Rep_T threads_number;
      DT_Msg_Counts_Rep_T msg_counts;
} DT_Reply_Data_T;

/**
 * Create the path to the file used to save dbg_trace levels for this XSAL app.
 *
 * @return The number of characters written to the buffer.
 *
 * @param path The buffer into which the path string is written.
 * @param max_path The length of the buffer.
 * @param app_name The unique name of the XSAL application
 */
int SAL_Form_DT_PS_File_Name(char *path, size_t max_path, char const *app_name);

/**
 * Initialize debug trace facility.
 */
void SAL_I_Init_DT(void);

/**
 * Function to process debug trace commands.
 *
 * @param command Pointer to the DT_Command_T structure containing the command.
 * @param reply_buffer Pointer to buffer into which reply is placed.
 * @return number of bytes in reply message.
 */
SAL_Int_T SAL_I_Proccess_DT_Command(const DT_Command_T* command, void* reply_buffer);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_DT_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * - 01-Jul-2015, Kirk Bailey
 *   - Added support for ISA "dt" commands
 *
 * -----------------------------------------------------------------------*/
