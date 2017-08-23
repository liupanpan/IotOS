#ifndef DBG_TRACE_ACFG_H
#   define DBG_TRACE_ACFG_H
/*===========================================================================*/
/**
 * @file dbg_trace_acfg.h
 *
 *   Defines default configurations for the debug trace module.
 *
 * %full_filespec:dbg_trace_acfg.h~kok_inf#12:incl:kok_aud#5 %
 * @version %version:kok_inf#12 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Sat Aug 30 15:25:46 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010 - 2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   The contents of this file are project-specific and must be configured by
 *   a project using the debug trace module.
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR - TRace
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_10016705_BASA_dbg_trace.doc
 *
 *   - Requirements Document(s):
 *     - BASA_SRS_Error_Management_1.2.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - none
 *
 * @addtogroup dbg_trace_client
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/


/* ! The inclusion of header files should NOT be inside the extern "C" block */
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */
/*===========================================================================*
 * Exported Preprocessor #define X-MACROS
 *===========================================================================*/

/**
 * Only define TR_DISABLE_EM_FILE_ID if the system is going to use an
 * alternate method for identifying files for trace than by sharing the
 * EM module's EM_F_ID.
 */
/*#   define TR_DISABLE_EM_FILE_ID 1 */

/**
 * Defines the default compile-time verbosity of the debug trace logic.
 *
 * @note
 *
 *   - Can be overridden on a per-file basis by defining TR_COMPILE_VERBOSE
 *     in the file prior to including dbg_trace.h.
 *
 *   - The value must be one of: TR_COMPILE_DISABLED, TR_COMPILE_COMPACT, or
 *     TR_COMPILE_VERBOSE.
 *
 *   - Because this is a compile-time option, nothing can be done at run time
 *     that is not compatible with the verbosity a file is compiled with; e.g.,
 *     if tracing is compiled out (TR_COMPILE_DISABLE), for a file, then there
 *     is no way to get trace information from the file without re-compiling it
 *     with a different setting.
 */
#   ifndef TR_DEFAULT_COMPILE_VERBOSITY
#      define TR_DEFAULT_COMPILE_VERBOSITY TR_COMPILE_VERBOSE
#   endif

/**
 * Defines the default minimum level of informational trace messages that are
 * enabled at compile-time for a file.
 *
 * @note
 *
 *   - This can be overridden on a per-file basis by defining
 *     TR_COMPILE_INFO_LEVEL in the file prior to including dbg_trace.h.
 *
 *   - The value must be set to one of the trace levels: TR_LVL_FAULT,
 *     TR_LVL_WARN, TR_LVL_NOTIFY, TR_LVL_INFO_HI, TR_LVL_INFO_MID, or
 *     TR_LVL_INFO_LO.
 *
 *   - The fault and warning trace messages are not affected by this setting -
 *     they cannot be compiled out; however, setting the level to either
 *     TR_LVL_FAULT or TR_LVL_WARN will cause all other levels to be compiled
 *     out.
 *
 *   - Because this is a compile-time option, nothing can be done at run time
 *     that is not compatible with the INFO level selection a file is compiled
 *     with; e.g., if TR_LVL_INFO_HI is the setting a file is compiled with,
 *     there is no way to get TR_LVL_INFO_MID or TR_LVL_INFO_LO trace
 *     information from the file without re-compiling it with a different
 *     setting.
 */
#   ifndef TR_DEFAULT_COMPILE_INFO_LEVEL
#      define TR_DEFAULT_COMPILE_INFO_LEVEL TR_LVL_INFO_LO
#   endif

#   undef Tr_Module_ID    /* Remove any stray definitions from other modules */
#   undef Tr_Module_ID_EQ /* Remove any stray definitions from other modules */

/**
 * X-Macro to defining enumeration for a module ID
 */
#   define Tr_Module_ID(ID, name) ID,

/**
 * X-Macro to defining enumeration for a module ID, and assign it a specific value.
 */
#   define Tr_Module_ID_EQ(ID, id_value, name) ID=id_value,

/**
 * If desired, customize the file pointer for the stream to send Tr_Fault messages.
 */
/*#   define TR_FAULT_FP stderr*/

/**
 * If desired, customize the file pointer for the stream to send Tr_Warn messages.
 */
/*#   define TR_WARN_FP stderr*/

/**
 * This X MACRO declares the project-specific list of modules ids in the system.
 * There is a corresponding X MACRO in the implementation configuration that
 * MUST have the same list of modules (a compile-time check is made for this).
 * Whenever you change this X MACRO, you MUST make an equivalent change to the
 * other one. The implementation X MACRO is kept separate so that a re-compile
 * of the entire program is not triggered by an adjustment to the initial trace
 * level settings. If these settings were part of this X MACRO (in the API),
 * all modules would need to be recompiled due to such a change even though none
 * of them would be affected (only the dbg_trace implementation changes for the
 * initial level settings). All this X MACRO does is declare the symbols used
 * for the modules' IDs and associates a text description with those module ids.
 */
#   define Tr_Module_IDs\
   Tr_Module_ID(APPLICATION_MANAGER_MODULE_ID     , "Application Manager"          ) \
   Tr_Module_ID(BASE_DRIVERS_MODULE_ID            , "Base Drivers"                 ) \
   Tr_Module_ID(CALLOUTS_MODULE_ID                , "Callouts Modules"             ) \
   Tr_Module_ID(DBG_TRACE_MODULE_ID               , "Debug Trace"                  ) \
   Tr_Module_ID(FBL_LINKER_MODULE_ID              , "FBL Linker"                   ) \
   Tr_Module_ID(HSM_ENGINE_MODULE_ID              , "State Machine Engine"         ) \
   Tr_Module_ID(ICR_INTEGRATED_APPS_TEST_MODULE_ID, "ICR Integrated Apps Test  "   ) \
   Tr_Module_ID(INTERRUPTS_MODULE_ID              , "Interrupts"                   ) \
   Tr_Module_ID(IO_MODULE_ID                      , "I/O callouts"                 ) \
   Tr_Module_ID(PERSISTENT_STORAGE_MODULE_ID      , "Persistent Storage"           ) \
   Tr_Module_ID(PITS_MODULE_ID                    , "PITS"                         ) \
   Tr_Module_ID(PITS_MODULE_ID_2                  , "PITS DESIP"                   ) \
   Tr_Module_ID(PITS_MODULE_ID_5                  , "PITS Services"                ) \
   Tr_Module_ID(PITS_MODULE_ID_XSAL               , "PITS XSAL"                    ) \
   Tr_Module_ID(PROCESS_HANDLER_MODULE_ID         , "Process Handler"              ) \
   Tr_Module_ID(PROCMAN_MODULE_ID                 , "Process Manager"              ) \
   Tr_Module_ID(SOH_MODULE_ID                     , "State of Health Manager"      ) \
   Tr_Module_ID(TEST_MODULE_ID                    , "TEST"                         ) \
   Tr_Module_ID(TEST_MODULE2_ID                   , "TEST2"                        ) \
   Tr_Module_ID(UTILITIES_MODULE_ID               , "Utilities"                    ) \
   Tr_Module_ID(VIP_DESIP_MODULE_ID               , "VIP DESIP Interface"          ) \
   Tr_Module_ID(VIP_PS_INTERFACE_MODULE_ID        , "VIP PS Interface"             ) \
   Tr_Module_ID(VO_MODULE_ID                      , "Virtual Operator"             ) \
   Tr_Module_ID(XENA_GATEWAY_MODULE_ID            , "XENA Gateway"                 ) \
   Tr_Module_ID(XSAL_BUFFER_MODULE_ID             , "XSAL buffer"                  ) \
   Tr_Module_ID(XSAL_CONDVAR_MODULE_ID            , "XSAL Cond Var"                ) \
   Tr_Module_ID(XSAL_IPC_MODULE_ID                , "XSAL IPC"                     ) \
   Tr_Module_ID(XSAL_MSG_MODULE_ID                , "XSAL Messaging"               ) \
   Tr_Module_ID(XSAL_MSG2_MODULE_ID               , "XSAL Messaging"               ) \
   Tr_Module_ID(XSAL_MUTEX_MODULE_ID              , "XSAL Mutex"                   ) \
   Tr_Module_ID(XSAL_NS_MODULE_ID                 , "XSAL Name Server"             ) \
   Tr_Module_ID(XSAL_Q_MODULE_ID                  , "XSAL Message Queue"           ) \
   Tr_Module_ID(XSAL_RWLOCK_MODULE_ID             , "XSAL RW Lock"                 ) \
   Tr_Module_ID(XSAL_SEM_MODULE_ID                , "XSAL Semaphore"               ) \
   Tr_Module_ID(XSAL_SHM_MODULE_ID                , "XSAL Shared Memory"           ) \
   Tr_Module_ID(XSAL_THREAD_MODULE_ID             , "XSAL Thread"                  ) \
   Tr_Module_ID(XSAL_TIME_MODULE_ID               , "XSAL Time"                    ) \
   Tr_Module_ID(XSAL_UTILITIES_MODULE_ID          , "XSAL Helper Functions"        ) \
   Tr_Module_ID(XSALCPP_MODULE_ID                 , "XSAL CPP"                     ) \
   Tr_Module_ID(XSALMT_MODULE_ID                  , "XSAL Message Thread"          ) \
   /* end of X-macro (dont remove this line) */

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/**
 * Project specific list of modules ID used by Debug / Trace module for asserts
 * and tracing.
 */
typedef enum Tr_Module_ID_Tag
{
   TR_MODULE_UNKNOWN, /* 0 = "unknown" module       */
   Tr_Module_IDs      /* Declare list of module ids */
   TR_NUM_MODULE_IDS
}
Tr_Module_ID_T;


#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file dbg_trace_acfg.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 11-jun-2008 kirk bailey
 *   - Created initial file.
 *
 * - 10-sep-2008 kirk bailey
 *   - Fixed include directive.
 *
 * - 11-jan-2011 kirk bailey Rev. 7
 *   - Task 19536 - Tip-off for mdf_xsal.
 *
 * - 30-Aug-2014 Kirk Bailey Rev 12
 *   - Brought to tip.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* DBG_TRACE_ACFG_H */

