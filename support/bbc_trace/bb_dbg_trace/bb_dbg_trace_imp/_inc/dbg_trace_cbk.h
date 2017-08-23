#ifndef DBG_TRACE_CBK_H
#   define DBG_TRACE_CBK_H
/*===========================================================================*/
/**
 * @file dbg_trace_cbk.h
 *
 * Defines the interface that provices system-specific behavior to the debug
 * trace module to have
 *
 * %full_filespec: dbg_trace_cbk.h~kok_inf#13:incl:kok_aud#1 %
 * @version %version: kok_inf#13 % 
 * @author  %derived_by: gzt3xl %
 * @date    %date_modified: Mon Feb 24 16:14:31 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010-2013 Delphi Technologies, Inc., All Rights Reserved.
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
 * @defgroup dbg_trace_callouts Debug Trace Callout Interface
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include <stdio.h>
#   include "dbg_trace_cfg.h"
#   include "reuse.h"
#   include <stdarg.h>

/*===========================================================================*
 * Exported Preprocessor #define Constants for Callouts
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS for Callouts
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */
/*===========================================================================*
 * Exported Type Declarations for Callouts
 *===========================================================================*/

/**
 * Structure to hold timestamps
 *    Can be from reset or epoch depending on system
 *    if epoch good until 2/7/2106 6:28:15 GMT
 */
typedef struct Tr_Clock_T
{
   uint32_t sec;
   uint32_t ns;
} Tr_Clock_T;

/**
 * Structure holding trace event information to log. 
 */
typedef struct Tr_Compact_Data_Tag
{
  uint16_t trace_level; /**< The level of the trace (TR_LVL_FAULT, etc.) */
  uint16_t file_id;     /**< A numeric id for the file with the trace statement. */
  uint16_t line_num;    /**< The source code line number of the trace statement. */
  uint16_t num_args;    /**< The actual # of arguments in the args array(compact mode only). */
  uint32_t args[4];     /**< Trace-specific numeric data (compact mode only). */
  Tr_Clock_T timestamp;  /**< Time of occurrence (see TR_STAND_ALONE). */
  uint32_t app_id;      /**< Application causing trace event (see TR_STAND_ALONE). */
  uint32_t thread_id;   /**< Thread causing trace event (see TR_STAND_ALONE). */
  Tr_Clock_T thr_cpu_time; /**< Thread CPU time (if available). */
}
Tr_Compact_Data_T;

/*===========================================================================*
 * Exported Function Prototypes for Callouts
 *===========================================================================*/

/**
 * Logs a pre-formatted message to all of the outputs of Debug Trace.
 *
 * @note Do NOT call directly.
 *
 * @param [in] fp Pointer to the output stream.
 *
 * @param [in] trace_output Character string to output to all of the Debug Trace sinks
 *
 */
void Tr_Log(FILE* fp, char_t * trace_output);

/**
 * Outputs a Call Stack backtrace from the location where this function was
 * executed. This will list all of the functions that have been called to get
 * to this point of execution.
 *
 * @note This Call Stack backtrace will be output regardless of the Trace Level
 * for the executing module. The caller of this function is responsible for
 * only calling this function when the Trace Level is appropriate (if desired).
 *
 * @note Currently only supported on a Linux C99 platform.
 */
void Tr_Print_Stack_Backtrace(void);

/**
 * This callout allows the system to customize its treatment of compact trace
 * data; it can simply be "printed out" or it can be logged to a file system.
 *
 * @param [in] fp Pointer to the output stream.
 * @param tr_data [in] The compact data captured by the trace statement.
 */
void Tr_Store_Compact_Data(FILE* fp, Tr_Compact_Data_T const *tr_data);

/**
 * This callout allows the system to customize its treatment of verbose trace
 * data; it can simply be "printed out" or it can be logged to a file system.
 *
 * @param [in] fp Pointer to the output stream.
 * @param tr_data [in] The compact data captured by the trace statement.
 * @param f_name [in] The name of the file containing the trace statement.
 * @param msg [in] The printf-like format string for the trace statement.
 * @param arg [in] Any variable-argument parameters needed by the format string.
 */
void Tr_Store_Verbose_Data(FILE* fp, Tr_Compact_Data_T const *tr_data,
                           char_t const *f_name, char_t const *msg, va_list arg);

#   if (TR_STAND_ALONE) /* No use of OS-related callouts, substitute defaults. */

#      define Tr_Clock()               ((Tr_Clock_T) {0, 0})
#      define Tr_Get_App_Id()          (0)
#      define Tr_Get_Thread_Id()       (1)
#      define Tr_Get_Thread_CPU_Time() ((Tr_Clock_T) {0, 0})

#   else

/**
 * Returns a timestamp structure (sec and ns) used for a logged message. The resolution can be
 * up to the system but is typically at least 1 mS.
 *
 * @return current time to use for a logged message.
 */
Tr_Clock_T Tr_Clock(void);

/**
 * Returns a 32-bit id for the application which is used for a logged message.
 * For XSAL systems this is the XSAL application id, but it can be some other
 * value that is meaningful for the system.
 *
 * @return application id for caller.
 */
uint32_t Tr_Get_App_Id(void);

/**
* Returns a 32-bit id for the calling thread which is used for a logged message.
* For XSAL systems this is the XSAL thread id, but it can be some other
* value that is meaningful for the system.
*
* @return application id for caller.
*/
uint32_t Tr_Get_Thread_Id(void);

/**
 * Returns a representation of the time used by the current thread (for
 * systems that support that); this information is used for a logged message
 * if the DBG_USE_THREAD_CPU_TIME option is enabled. The resolution can be
 * up to the system but is typically at least 1 mS. This can be useful for monitoring
 * which threads in a system are utilizing the most CPU (and when).
 *
 * @return time used by the calling thread.
 */
Tr_Clock_T Tr_Get_Thread_CPU_Time(void);

#   endif /* if (TR_STAND_ALONE) */

#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */
#
/*===========================================================================*/
/*!
 * @file dbg_trace_cbk.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 07-may-2011 Kris Boultbee
 *   Task kok_basa#30663 Revision 4
 *   - Added function Tr_Log() used to log a pre-formatted string to Debug Trace.
 *
 * - 10-May-2012 kirk bailey Rev. 5
 *   - Added thread CPU time.
 *
 * - 28-Feb-2013 Kirk Bailey Rev. 6
 *   - Decoupled trace from XSAL, moving that information to callouts.
 *
 * - 12-Mar-2013 Kirk Bailey Rev. 7
 *   - Added DBG_ENABLE_64BIT_TIMESTAMP.
 *
 * - 1-July-2013 Dan Carman
 *      Standardize timestamp format for all systems
 *
 * - 04-Jul-2013 Larry Piekarski Rev 10
 *   - Task 159854: Fixed typo in stand alone mode.
 *
 * - 03-Oct-2013 Kirk Bailey REV 11
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 *
 *  - 04-Oct-2013 Kirk Bailey REV 12
 *   -  Fixed function-like MACROs Tr_Get_App_Id()/Tr_Get_Thread_Id().
 *
 * - 24-Feb-2014 Paul Casto REV 13
 *   - Added support for different output streams.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* DBG_TRACE_CBK_H */
