#ifndef DBG_TRACE_CFG_H
#   define DBG_TRACE_CFG_H
/*===========================================================================*/
/**
 * @file dbg_trace_cfg.h
 *
 * Program configuration for the dbg_trace module implementation.
 *
 * %full_filespec: dbg_trace_cfg.h~kok_inf#17:incl:kok_aud#5 %
 * @version %version: kok_inf#17 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Sat Aug 30 15:25:50 2014 %
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
 *   - TR = trace
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%
20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @addtogroup dbg_trace
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

/*===========================================================================*
 * #define Constants
 *===========================================================================*/

/*===========================================================================*
 * #define MACROS
 *===========================================================================*/

/**
 * Set DBG_USE_PID_TID_NAMES to a non-zero value to use Linux's process/thread
 * names in log messages instead of the PID/TID numbers; set to zero (or
 * leave undefined) to use the numbers instead.
 */
/*#   define DBG_USE_PID_TID_NAMES 0*/

/**
 * Declaring DBG_DISABLE_STDOUT disables logging of trace information
 * to stdout. Leave this declaration commented out unless you wish to disable
 * logging to stdout. Note that default behavior (DBG_DISABLE_STDOUT not
 * declared) is for trace output to go to stdout.
 */
/*#   define DBG_DISABLE_STDOUT true*/

/**
 * Enables the logic that uses POSIX file I/O to read/save the dbg_trace levels.
 * To enable, define this value to be true. To disable, either do not define
 * this symbol or define it to be false. If this feature is disabled, the
 * functions still exist; however, they do nothing. This is for support of
 * systems that do not support POSIX file I/O.
 */
#   define DBG_ENABLE_POSIX_LVL_IO true

/**
 * Enables the printout of the call stack backtrace of an assert. This enables
 * easier debugging of software that results in an assert. To enable the
 * call stack backtrace define this value to be true. To disable, either do
 * not define this symbol or define it to be false.
 */
#   define DBG_ENABLE_STACK_BACKTRACE true

/**
 * Declaring DBG_ENABLE_SYSLOG enables logging of trace information to syslog.
 * Leave this declaration commented out unless you wish to enable syslog. You
 * can have both syslog and stdout enabled simultaneously (the trace output will
 * go to both locations). Note that the default behavior (DBG_ENABLE_SYSLOG
 * not declared) is for trace output to NOT go to sysglog. For QNX, this
 * controls slogger, not syslog.
 */
/*#  define DBG_ENABLE_SYSLOG true*/

/**
 * Maximum trace line length before truncation occurs; affects stack depth.
 */
#   define DBG_TRACE_MAX_LINE_LENGTH 256 

/**
 * Set to true to build dbg_trace without any dependence on the RTOS. If,
 * instead, it is desired that the trace information contain thread and
 * clock information from the RTOS, then set the value to false.
 */
#   define TR_STAND_ALONE   false

/**
 * Define this symbol to true to enable per-thread CPU time (in addition to clock
 * time) on systems that support it.
 */
#   define DBG_USE_THREAD_CPU_TIME false

/**
 * Set this to true to switch from a 32-bit to 64-bit timestamp for both the
 * regulare and CPU times.
 */
#   define DBG_ENABLE_64BIT_TIMESTAMP false

/**
 * Set the default run-time trace level
 */
#   define TR_DEFAULT_RUNTIME_INFO_LEVEL  TR_LVL_INFO_HI

/**
 * Use TR_MODULE_LEVEL to associate the initial trace level with a module ID.
 */
#   define TR_MODULE_LEVEL(ID, init_lvl)

/**
 * Use TR_MODULE_LEVEL_EQ to associate the initial trace level with a module ID
 * that needs to be assigned a specific value.
 */
#   define TR_MODULE_LEVEL_EQ(ID, id_value, init_lvl)

/**
 * This X MACRO assigns initial trace levels to the project-specific list of
 * modules. This list must match (in order and size) the X MACRO declaring the
 * module ids for the system in the API configuration header file
 * (Tr_Module_IDs). If it does not match, the compiler will fail to build
 * the dbg_trace implementation. Because the values for the level settings in
 * this X MACRO only affect the dbg_trace implementation, they can be changed
 * without causing a re-build of all modules in the system. This is the reason
 * for separating these settings from the API modules list. If modules are added
 * or deleted (or the order changed), then both this X MACRO and the
 * corresponding X MACRO in the API will have to change.
 */
#   define TR_MODULE_LEVELS \
   TR_MODULE_LEVEL(APPLICATION_MANAGER_MODULE_ID     , TR_LVL_INFO_HI               ) \
   TR_MODULE_LEVEL(BASE_DRIVERS_MODULE_ID            , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(CALLOUTS_MODULE_ID                , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(DBG_TRACE_MODULE_ID               , TR_LVL_INFO_HI               ) \
   TR_MODULE_LEVEL(FBL_LINKER_MODULE_ID              , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(HSM_ENGINE_MODULE_ID              , TR_LVL_INFO_LO               ) \
   TR_MODULE_LEVEL(ICR_INTEGRATED_APPS_TEST_MODULE_ID, TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(INTERRUPTS_MODULE_ID              , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(IO_MODULE_ID                      , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(PERSISTENT_STORAGE_MODULE_ID      , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(PITS_MODULE_ID                    , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(PITS_MODULE_ID_2                  , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(PITS_MODULE_ID_5                  , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(PITS_MODULE_ID_XSAL               , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(PROCESS_HANDLER_MODULE_ID         , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(PROCMAN_MODULE_ID                 , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(SOH_MODULE_ID                     , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(TEST_MODULE_ID                    , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(TEST_MODULE2_ID                   , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(UTILITIES_MODULE_ID               , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(VIP_DESIP_MODULE_ID               , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(VIP_PS_INTERFACE_MODULE_ID        , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(VO_MODULE_ID                      , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XENA_GATEWAY_MODULE_ID            , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_BUFFER_MODULE_ID             , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_CONDVAR_MODULE_ID            , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_IPC_MODULE_ID                , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_MSG_MODULE_ID                , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_MSG2_MODULE_ID               , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_MUTEX_MODULE_ID              , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_NS_MODULE_ID                 , TR_LVL_FAULT                 ) \
   TR_MODULE_LEVEL(XSAL_Q_MODULE_ID                  , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_RWLOCK_MODULE_ID             , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_SEM_MODULE_ID                , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_SHM_MODULE_ID                , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_THREAD_MODULE_ID             , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_TIME_MODULE_ID               , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSAL_UTILITIES_MODULE_ID          , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSALCPP_MODULE_ID                 , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   TR_MODULE_LEVEL(XSALMT_MODULE_ID                  , TR_DEFAULT_RUNTIME_INFO_LEVEL) \
   /* end of X-macro (do NOT remove this line) */

/*===========================================================================*
 * Custom Type Declarations
 *===========================================================================*/

/*===========================================================================*/
/*!
 * @file dbg_trace_cfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 12-aug-2008 kirk bailey
 *   - Created initial file.
 *
 * - 11-jan-2011 kirk bailey Rev. 10
 *   - Task 19536 - Tip-off for mdf_xsal.
 *
 * - 21-Mar-2013 Kirk Bailey Rev. 16
 *   - Added DBG_ENABLE_64BIT_TIMESTAMP.
 *
 * - 30-Aug-2014 Kirk Bailey Rev 17
 *   - Brought to tip.
 *
 *===========================================================================*/
/** @} doxygen end group */
#endif /* DBG_TRACE_CFG_H */

