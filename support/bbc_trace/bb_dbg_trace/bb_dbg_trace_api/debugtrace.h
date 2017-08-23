#ifndef DEBUGTRACE_H
#   define DEBUGTRACE_H
/*===========================================================================*/
/**
 * @file debugtrace.h
 *
 *   Maps the ISA debugtrace API to the BASA implementation.
 *
 * %full_filespec: debugtrace.h~3:incl:kok_inf#1 %
 * @version %version: 3 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Wed May 13 14:00:56 2015 %
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
 *   - EM - Error Management
 *   - TR - TRace
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - none
 *
 * @ingroup dbg_trace_client
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "dt_groups.h"
#   include "pbc_trace.h"
#   include "reuse.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

#   define DT_ERROR_   TR_LVL_FAULT
#   define DT_WARNING_ TR_LVL_WARN
#   define DT_INFO_    TR_LVL_NOTIFY
#   define DT_DEBUG_   TR_LVL_INFO_HI

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/** Structure used by Tr_Get_Settings macro to obtain Debug Trace settings.
 */
typedef struct DT_Settings_Tag
{
      uint64_t group_mask;
      int verbosity_level;
      int logger_enable;     /* 0 - disable, 1 - enable */
      int time_stamp_enable; /* 0 - disable, 1 - enable */

}
DT_Settings_T;

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/**
 * Terminates application if condition is not satisfied.
 *
 *  @param cond condition to be asserted
 */
#   define Tr_Assert(cond)             \
if (!(cond))                           \
{                                      \
   Tr_Err("assertion failed: " #cond); \
   EM_Perform_Assert();                \
}

/**
 * Disables printing the time stamp in debug output.
 */
#   define Tr_Disable_Timestamp()

/**
 * Enables printing the time stamp in debug output.
 */
#   define Tr_Enable_Timestamp()

/**
 * Opens a file to which debug messages will be stored.
 * The file will be closed during destroying DT by Tr_Destroy().
 *
 * @param const char* file name of file to be opened
 */
#   define Tr_Open_File(file)

/**
 * Allows to install user own function for flushing debug messages.
 *
 * @param void (*handler)(const char*) poiner to function used for flushing messages
 */
#   define Tr_Set_Flush_Handler(handler)

/**
 * @return current SAL thread name if possible.
 */
#   define Tr_Thread_Name SAL_Get_Thread_Name_Self()

#   if (TR_COMPILE_VERBOSITY == TR_COMPILE_DISABLED)
/*############################################################################
 *
 * TRACE DISABLED - No trace code is generated
 *
 *############################################################################*/

#      define Tr_Err(msg, ...)
#      define Tr_Err_If(cond, msg, ...)
#      define Tr_Wrn(msg, ...)
#      define Tr_Wrn_If(cond, msg, ...)
#      define Tr_Inf(group, msg, ...)
#      define Tr_Inf_If(cond,group, msg, ...)
#      define Tr_At1(group, msg, ...)
#      define Tr_At1_If(cond,group, msg, ...)
#      define Tr_At2(group, msg, ...)
#      define Tr_At2_If(cond,group, msg, ...)
#      define Tr_At3(group, msg, ...)
#      define Tr_At3_If(cond,group, msg, ...)
#      define Tr_Dbg_Light(group, level, msg, ...)
#      define Tr_Dbg_Light_If(cond, group, level, msg, ...)
#      define Tr_Dbg_If_Ethernet(group, level, msg, ...)
#      define Tr_Dbg_If_Ethernet_If(cond, group, level, msg, ...)

#   else
/*############################################################################
 *
 * TRACE ENABLED - Errors/Warnings always present. Other levels can be
 *                 conditionally compiled out based on configuration setting
 *                 of TR_COMPILE_INFO_LEVEL.
 *
 *############################################################################*/

#      define Tr_Err(msg, ...)\
          Tr_Trace_Verbose(TR_FAULT_FP,TR_LVL_FAULT,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#      define Tr_Err_If(cond, msg, ...)\
          if (cond) Tr_Trace_Verbose(TR_FAULT_FP,TR_LVL_FAULT,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)

#      define Tr_Wrn(msg, ...)\
          Tr_Trace_Verbose(TR_WARN_FP,TR_LVL_WARN,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#      define Tr_Wrn_If(cond, msg, ...)\
          if (cond) Tr_Trace_Verbose(TR_WARN_FP,TR_LVL_WARN,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_NOTIFY)
#         define Tr_Inf(group, msg, ...)
#         define Tr_Inf_If(cond,group, msg, ...)
#      else
#         define Tr_Inf(group, msg, ...)\
             Tr_Trace(group,stdout,TR_LVL_NOTIFY,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#         define Tr_Inf_If(cond,group, msg, ...)\
             if (cond) Tr_Trace(group,stdout,TR_LVL_NOTIFY,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#      endif

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_HI)
#         define Tr_At1(group, msg, ...)
#         define Tr_At1_If(cond,group, msg, ...)
#      else
#         define Tr_At1(group, msg, ...)\
             Tr_Trace(group,stdout,TR_LVL_INFO_HI,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#         define Tr_At1_If(cond,group, msg, ...)\
             if (cond) Tr_Trace(group,stdout,TR_LVL_INFO_HI,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#      endif

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_MID)
#         define Tr_At2(group, msg, ...)
#         define Tr_At2_If(cond,group, msg, ...)
#      else
#         define Tr_At2(group, msg, ...)\
             Tr_Trace(group,stdout,TR_LVL_INFO_MID,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#         define Tr_At2_If(cond,group, msg, ...)\
             if (cond) Tr_Trace(group,stdout,TR_LVL_INFO_MID,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#      endif

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_LO)
#         define Tr_At3(group, msg, ...)
#         define Tr_At3_If(cond,group, msg, ...)
#      else
#         define Tr_At3(group, msg, ...)\
             Tr_Trace(group,stdout,TR_LVL_INFO_LO,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#         define Tr_At3_If(cond,group, msg, ...)\
             if (cond) Tr_Trace(group,stdout,TR_LVL_INFO_LO,0,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg, ## __VA_ARGS__)
#      endif

#      define Tr_Dbg_Light(group, level, msg, ...)\
       Tr_Trace(group,stdout,(TR_LVL_NOTIFY+level),0,"",0,msg, ## __VA_ARGS__)
#      define Tr_Dbg_Light_If(cond, group, level, msg, ...)\
       if (cond) Tr_Trace(group,stdout,(TR_LVL_NOTIFY+level),0,"",0,msg, ## __VA_ARGS__)

#      define Tr_Dbg_If_Ethernet(group, level, msg, ...)\
       Tr_Trace(group,stdout,(TR_LVL_NOTIFY+level),0,"",0,msg, ## __VA_ARGS__)
#      define Tr_Dbg_If_Ethernet_If(cond, group, level, msg, ...)\
       if (cond) Tr_Trace(group,stdout,(TR_LVL_NOTIFY+level),0,"",0,msg, ## __VA_ARGS__)

#   endif

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/**
 * Cleans up Debug/Trace.
 * Before using this call all threads should be joined.
 * It is not posible to use Debug/Trace after this call.
 */
void Tr_Destroy(void);

/**
 * Disables the trace output for the process.
 */
void Tr_Disable(void);

/**
 * Enables the trace output for the process.
 */
void Tr_Enable(void);

/**
 * Reads current Debug Trace settings.
 *
 * @param  settings DT_Settings_T structure to store debug settings
 */
void Tr_Get_Settings(DT_Settings_T *settings);

/**
 * Initializes Debug Trace. This function is not thread safe.
 * It has to be called only once as a first DT macro in thread safe way.
 *
 *  @param app_name default name of the application
 *  @param grp default group could be override by command line arguments
 *  @param level default level could be override by command line arguments
 *  @param time_stamp whether time stamp should be enabled, 1 - enabled
 *                                                          0 - disabled
 *  @param argc  argument count
 *  @param argv  list of arguments
 */
void Tr_Init(const char *app_name, uint64_t grp, int level, int time_stamp, int argc, char const* argv[]);

/**
 * Defines the Tracing Group.
 *
 * @param grp group to be set
 */
void Tr_Set_Group(uint64_t grp);

/**
 * Defines the Tracing Level.
 *
 * @param int level verbosity level to be set (0-3)
 */
void Tr_Set_Level(int level);

/**
 * Logs a verbose trace message; do NOT call directly. This function should
 * only be called through use of the trace MACROs defined above.
 *
 * @param [in] group Debug group mask.
 *
 * @param [in] fp Pointer to the output stream.
 *
 * @param [in] lvl Trace message level (fault, warning, info_high, etc.)
 *
 * @param [in] f_num "TR_FILE_ID" identifier for the file from which the trace
 *                   originates; this value is generated by the EM module's
 *                   EM_FILENUM MACRO.
 *
 * @param [in] f_name the C __FILE__ value that identifies the file from which
 *                    the trace originates.
 *
 * @param [in] line_num the C __LINE__ value that identifies the line at which
 *                      the trace originates.
 *
 * @param [in] msg A printf-like message that provides a description for the
 *                 trace and identifies the types of the variable arguments that
 *                 follow.
 *
 * @pre
 *   - f_name != NULL
 *   - msg != NULL
 */
void Tr_Trace(uint64_t group, FILE* fp, Tr_Trace_Level_T lvl, uint16_t f_num, char_t const *f_name, uint16_t line_num,
   char_t const *msg, ...)
#      if defined(__GNUC__) && !defined(TR_DISABLE_FORMAT_ATTR)
      __attribute__ ((format (printf, 7, 8)))
#      endif
      ;

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

#   ifdef __cplusplus
} /* extern "C" */
#   endif                    /* __cplusplus */
/*===========================================================================*/
/*!
 * @file debugtrace.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 11-May-2015 Kirk Bailey
 *   - Created file from version 120_1428 of AP_DebugTrace
 *
 * - 11-May-2015 Kirk Bailey
 *   - Improved comments. Made argv const.
 *
 * - 13-May-2015 Kirk Bailey
 *   - Added conditional compilation logic.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* DEBUGTRACE_H */
