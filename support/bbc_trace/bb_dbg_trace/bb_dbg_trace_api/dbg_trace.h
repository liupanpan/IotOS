#ifndef DBG_TRACE_H
#   define DBG_TRACE_H
/*===========================================================================*/
/**
 * @file dbg_trace.h
 *
 *   Defines the API used by modules for issuing trace messages to the system.
 *
 * %full_filespec:dbg_trace.h~kok_inf#14:incl:kok_aud#1 %
 * @version %version:kok_inf#14 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Fri Aug  8 12:34:20 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2013-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   The debug trace module provides a mechanism for modules in the system to
 *   log trace information. The details of how the information is logged are
 *   dependent on the implementation of this API chosen for a given system and
 *   various compile-time and run-time configurations.
 *
 *   Modules use the MACROs defined in this API to generate trace information.
 *   A "trace level" determines the importance of the trace information and
 *   can be used to filter trace messages at compile time. Depending on the
 *   implementation, run time filtering may also be possible but will, of
 *   course, always be limited to the compile-time choices.
 *
 *   Three levels of verbosity are supported at compile time. The verbosity
 *   level can be different for different files within the same build. The
 *   verbosity levels are used to perform tradeoffs between the amount of
 *   system resources (code and data memory sizes) used for the trace
 *   information and the readability of that information.
 *
 *   The most verbose setting allows descriptive strings to be used, much like
 *   "printf". The intent is for this mode to be "string based"; not only will
 *   the format string included as the first argument to the MACRO be used, but
 *   additional strings like the name of the file from which the MACRO is
 *   invoked. This provides a very readable set of trace data at the cost of
 *   system resources.
 *
 *   Variadic (variable argument number) MACROs are not used in order to 
 *   maximize the number of C compilers that can build this module. Many C
 *   compilers do not yet conform to the C99 standard, in which variadic MACRO
 *   support was added. Also, the C++ specification does not contain support
 *   for variadic MACROs. To support a range of needs for trace data, MACROs
 *   are provided for 0 to 4 arguments other than the trace message format
 *   string. Multiple MACRO invocations can be used in rare situations where
 *   more trace data is needed. Another alternative is to use snprintf to write
 *   data into a buffer and pass that buffer as one of the trace arguments.
 *
 *   A middle verbosity setting is designed for systems that cannot afford the
 *   resource load of having a large number of strings in the program for debug
 *   trace purposes. This is called "compact" verbosity. For this setting, the
 *   strings for the trace messages and file names are discarded. In their
 *   place  are numeric codes that allow the location of the trace message to
 *   be uniquely identified. The unique identification comes by combining the
 *   "module id" and "file number" into the symbol EM_F_ID. MACROs are provided
 *   to support this. In addition to this numeric "file identifier", the line
 *   number of the MACRO invocation is kept. These values can be "decoded" in
 *   order to identify the source code containing the trace message. This 
 *   sacrifices the readability of the trace information to gain memory
 *   resources. In compact mode, any trace MACRO arguments are cast to unsigned
 *   32-bit values for logging. This, again, favors resource usage over ease of
 *   reading by the developer.
 *
 *   A final verbosity setting is "disabled". This setting turns off code
 *   generation for all trace MACROs (for the files compiled with this setting).
 *
 * @section ABBR ABBREVIATIONS:
 *   - EM - Error Management
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
 * @defgroup dbg_trace_client Debug Trace Client Interface
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include <stdio.h>
#   include "dbg_trace_acfg.h"
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/**
 * EM_F_ID is formed from a combination of the module id and the file index
 * by shifting the module id left this number of times before combining it
 * with the file index.
 */
#   define EM_MOD_SHIFT_CNT 4

/**
 * EM_F_ID must have a unique value for each compilation unit that uses
 * the dbg_trace "Tr_*" (or pbc_trace "PBC_*") MACROs. This unique value is
 * formed by combining the "module id" of the module containing the file
 * (defined in dbg_trace_acfg.h) and the file number of the file within the
 * module. The EM_FILE_ID MACRO (below) can be used to assign a value to 
 * EM_F_ID. This declaration should appear at the beginning of the compilation
 * unit in the MACRO declaration section. Typically, only C source files will
 * declare EM_F_ID since C header files will not tend to have code sequences
 * that invoke the above-mentioned MACROs. For C++, it is quite conceivable
 * that header files will need to declare EM_F_ID since class declarations may
 * also include code. In these situations, EM_F_ID will need to be defined in
 * the header file, but it should also be undefined at the end of the header
 * file. This allows files that include it to easily define different values
 * for EM_F_ID and not "inherit" the value from the header file. The following
 * undefine is intended to help prevent accidental propagation of a value
 * from a header file.
 */
#   define EM_F_ID 0
#   undef  EM_F_ID

/**
 * The following MACRO can be used to define the value of EM_F_ID using its
 * module and file ids.
 *
 * @param mod_id Module ID value (see dbg_trace_acfh.h)
 * @param fidx Index of file within module; range (0-15)
 */
#   define EM_FILE_ID(mod_id, fidx) (((mod_id) << EM_MOD_SHIFT_CNT) + ((fidx)&(0xffff>>(16-EM_MOD_SHIFT_CNT))))

/**
 * The following MACRO is the original method for assigning a value of EM_F_ID
 * within a C compilation unit. It can still be used as long as the compilation
 * unit does not include any header files that also define EM_F_ID. This may
 * occur in C++ code, and in this situation, the code will not be able to use
 * this MACRO. Instead, a definition associating EM_F_ID with an invocation
 * of EM_FILE_ID can be used. The header file should undefine its value of
 * EM_F_ID at the end of the header file; this allows files that include the
 * header file to easily declare their own values for EM_F_ID and prevents them
 * from "accidentally" inheriting the value from the header file. This
 * MACRO is maintained for backward compatibility on files that already exist
 * using this technique without any of the header file conflicts described.
 *
 * @param mod_id Module ID value (see dbg_trace_acfh.h)
 * @param fidx Index of file within module range (0-15)
 */
#   define EM_FILENUM(mod_id, fidx) enum{EM_F_ID = EM_FILE_ID(mod_id, fidx)}

/**
 * Unless TR_DISABLE_EM_FILE_ID is defined (dbg_trace_acfg.h), the default
 * behavior is for the "file id" used for tracing (TR_FILE_ID) be the same
 * as that used by the EM module for reporting faults: EM_F_ID. Although not
 * encouraged, this allows support for other legacy methods of identifying
 * "groups" or categories for debug trace.
 */
#   ifndef TR_DISABLE_EM_FILE_ID
#      define TR_FILE_ID EM_F_ID
#   endif

/**
 * Unless TR_FAULT_FP is defined (dbg_trace_acfg.h), the default
 * behavior is for the Tr_Fault messages to be streamed to stderr.
 */
#   ifndef TR_FAULT_FP
#      define TR_FAULT_FP stderr
#   endif

/**
 * Unless TR_WARN_FP is defined (dbg_trace_acfg.h), the default
 * behavior is for the Tr_Warn messages to be streamed to stderr.
 */
#   ifndef TR_WARN_FP
#      define TR_WARN_FP stderr
#   endif

/**
 * Trace level used to indicate a fault condition for which default recovery
 * action was taken.
 */
#   define TR_LVL_FAULT 0
/**
 * Trace level used to indicate a temporary disruption that was corrected
 * or for which a default action was taken.
 */
#   define TR_LVL_WARN 1
/**
 * Trace level used for significant, infrequent state changes.
 */
#   define TR_LVL_NOTIFY 2
/**
 * Trace level used for important information.
 */
#   define TR_LVL_INFO_HI 3
/**
 * Trace level used for typical information.
 */
#   define TR_LVL_INFO_MID 4
/**
 * Trace level used for low level debug information.
 */
#   define TR_LVL_INFO_LO 5

/**
 * Compile-time verbosity setting to disable all trace code generation.
 */
#   define TR_COMPILE_DISABLED 0

/**
 * Compile-time verbosity setting to generate compact trace date (output
 * requires "decoding").
 */
#   define TR_COMPILE_COMPACT 1

/** 
 * Compile-time verbosity setting that maximizes the readability of the
 * trace output but consumes more target resources.
 */
#   define TR_COMPILE_VERBOSE 2

/**
 * Name of the file, this can be overwritten by a Makefile to only store
 * the name of the file instead of the full path
 */
#   ifndef _BASE_FILE_NAME_
#      define _BASE_FILE_NAME_ __FILE__
#   endif

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/**
 * The Trace Level type is used to filter / categorize trace events.
 * Valid values are: @see TR_LVL_FAULT, TR_LVL_WARN, TR_LVL_NOTIFY,
 * TR_LVL_INFO_HI, TR_LVL_INFO_MID, and TR_LVL_INFO_LO. This type is
 * not defined as an enum of these in order to precisely control its
 * size (8 bits).
 */
typedef uint8_t Tr_Trace_Level_T;

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/**
 * If a compile-time trace verbosity (TR_COMPILE_VERBOSITY) is not specified by
 * the file including this header file, then the default is used.
 */
#   ifndef TR_COMPILE_VERBOSITY
#      ifndef TR_DEFAULT_COMPILE_VERBOSITY
#         error TR_DEFAULT_COMPILE_VERBOSITY was not specified.
#      endif                    /* TR_DEFAULT_COMPILE_VERBOSITY */
#      define TR_COMPILE_VERBOSITY TR_DEFAULT_COMPILE_VERBOSITY
#   endif

/**
 * If a minimum compile-time logged info level for a file
 * (TR_COMPILE_INFO_LEVEL) is not specified by the file including this header
 * file, then the default is used.
 */
#   ifndef TR_COMPILE_INFO_LEVEL
#      ifndef TR_DEFAULT_COMPILE_INFO_LEVEL
#         error TR_DEFAULT_COMPILE_INFO_LEVEL was not specified.
#      endif                    /* TR_DEFAULT_COMPILE_INFO_LEVEL */
#      define TR_COMPILE_INFO_LEVEL TR_DEFAULT_COMPILE_INFO_LEVEL
#   endif

#   if (TR_COMPILE_VERBOSITY == TR_COMPILE_DISABLED)
/*############################################################################
 *
 * TRACE DISABLED - No trace code is generated
 *
 *############################################################################*/

#      define Tr_Fault(msg)
#      define Tr_Fault_1(msg,a1)
#      define Tr_Fault_2(msg,a1,a2)
#      define Tr_Fault_3(msg,a1,a2,a3)
#      define Tr_Fault_4(msg,a1,a2,a3,a4)

#      define Tr_Warn(msg)
#      define Tr_Warn_1(msg,a1)
#      define Tr_Warn_2(msg,a1,a2)
#      define Tr_Warn_3(msg,a1,a2,a3)
#      define Tr_Warn_4(msg,a1,a2,a3,a4)

#      define Tr_Notify(msg)
#      define Tr_Notify_1(msg,a1)
#      define Tr_Notify_2(msg,a1,a2)
#      define Tr_Notify_3(msg,a1,a2,a3)
#      define Tr_Notify_4(msg,a1,a2,a3,a4)

#      define Tr_Info_Hi(msg)
#      define Tr_Info_Hi_1(msg,a1)
#      define Tr_Info_Hi_2(msg,a1,a2)
#      define Tr_Info_Hi_3(msg,a1,a2,a3)
#      define Tr_Info_Hi_4(msg,a1,a2,a3,a4)

#      define Tr_Info_Mid(msg)
#      define Tr_Info_Mid_1(msg,a1)
#      define Tr_Info_Mid_2(msg,a1,a2)
#      define Tr_Info_Mid_3(msg,a1,a2,a3)
#      define Tr_Info_Mid_4(msg,a1,a2,a3,a4)

#      define Tr_Info_Lo(msg)
#      define Tr_Info_Lo_1(msg,a1)
#      define Tr_Info_Lo_2(msg,a1,a2)
#      define Tr_Info_Lo_3(msg,a1,a2,a3)
#      define Tr_Info_Lo_4(msg,a1,a2,a3,a4)

#   elif (TR_COMPILE_VERBOSITY == TR_COMPILE_COMPACT)
/*############################################################################
 *
 * COMPACT TRACE
 *
 *   - The message string is not used (discarded).
 *   - The _BASE_FILE_NAME_ name string is not used (discarded).
 *   - The "TR_FILE_ID" integral value is used to identify the file.
 *   - The __LINE__ value is used to identify the location of the trace call.
 *   - Any arguments are cast to and passed as uint32_t.
 *
 *############################################################################*/

#      define Tr_Fault(msg) Tr_Trace_Compact(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,(uint16_t)__LINE__,0)
#      define Tr_Fault_1(msg,a1) Tr_Trace_Compact(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,(uint16_t)__LINE__,1,((uint32_t)a1))
#      define Tr_Fault_2(msg,a1,a2) Tr_Trace_Compact(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,(uint16_t)__LINE__,2,((uint32_t)a1),((uint32_t)a2))
#      define Tr_Fault_3(msg,a1,a2,a3) Tr_Trace_Compact(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,(uint16_t)__LINE__,3,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3))
#      define Tr_Fault_4(msg,a1,a2,a3,a4) Tr_Trace_Compact(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,(uint16_t)__LINE__,4,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3),((uint32_t)a4))

#      define Tr_Warn(msg) Tr_Trace_Compact(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,(uint16_t)__LINE__,0)
#      define Tr_Warn_1(msg,a1) Tr_Trace_Compact(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,(uint16_t)__LINE__,1,((uint32_t)a1))
#      define Tr_Warn_2(msg,a1,a2) Tr_Trace_Compact(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,(uint16_t)__LINE__,2,((uint32_t)a1),((uint32_t)a2))
#      define Tr_Warn_3(msg,a1,a2,a3) Tr_Trace_Compact(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,(uint16_t)__LINE__,3,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3))
#      define Tr_Warn_4(msg,a1,a2,a3,a4) Tr_Trace_Compact(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,(uint16_t)__LINE__,4,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3),((uint32_t)a4))

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_NOTIFY)

#         define Tr_Notify(msg)
#         define Tr_Notify_1(msg,a1)
#         define Tr_Notify_2(msg,a1,a2)
#         define Tr_Notify_3(msg,a1,a2,a3)
#         define Tr_Notify_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Notify(msg) Tr_Trace_Compact(stdout, TR_LVL_NOTIFY,TR_FILE_ID,(uint16_t)__LINE__,0)
#         define Tr_Notify_1(msg,a1) Tr_Trace_Compact(stdout, TR_LVL_NOTIFY,TR_FILE_ID,(uint16_t)__LINE__,1,((uint32_t)a1))
#         define Tr_Notify_2(msg,a1,a2) Tr_Trace_Compact(stdout, TR_LVL_NOTIFY,TR_FILE_ID,(uint16_t)__LINE__,2,((uint32_t)a1),((uint32_t)a2))
#         define Tr_Notify_3(msg,a1,a2,a3) Tr_Trace_Compact(stdout, TR_LVL_NOTIFY,TR_FILE_ID,(uint16_t)__LINE__,3,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3))
#         define Tr_Notify_4(msg,a1,a2,a3,a4) Tr_Trace_Compact(stdout, TR_LVL_NOTIFY,TR_FILE_ID,(uint16_t)__LINE__,4,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3),((uint32_t)a4))

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_NOTIFY */

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_HI)

#         define Tr_Info_Hi(msg)
#         define Tr_Info_Hi_1(msg,a1)
#         define Tr_Info_Hi_2(msg,a1,a2)
#         define Tr_Info_Hi_3(msg,a1,a2,a3)
#         define Tr_Info_Hi_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Info_Hi(msg) Tr_Trace_Compact(stdout, TR_LVL_INFO_HI,TR_FILE_ID,(uint16_t)__LINE__,0)
#         define Tr_Info_Hi_1(msg,a1) Tr_Trace_Compact(stdout, TR_LVL_INFO_HI,TR_FILE_ID,(uint16_t)__LINE__,1,((uint32_t)a1))
#         define Tr_Info_Hi_2(msg,a1,a2) Tr_Trace_Compact(stdout, TR_LVL_INFO_HI,TR_FILE_ID,(uint16_t)__LINE__,2,((uint32_t)a1),((uint32_t)a2))
#         define Tr_Info_Hi_3(msg,a1,a2,a3) Tr_Trace_Compact(stdout, TR_LVL_INFO_HI,TR_FILE_ID,(uint16_t)__LINE__,3,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3))
#         define Tr_Info_Hi_4(msg,a1,a2,a3,a4) Tr_Trace_Compact(stdout, TR_LVL_INFO_HI,TR_FILE_ID,(uint16_t)__LINE__,4,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3),((uint32_t)a4))

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_HI */

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_MID)

#         define Tr_Info_Mid(msg)
#         define Tr_Info_Mid_1(msg,a1)
#         define Tr_Info_Mid_2(msg,a1,a2)
#         define Tr_Info_Mid_3(msg,a1,a2,a3)
#         define Tr_Info_Mid_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Info_Mid(msg) Tr_Trace_Compact(stdout, TR_LVL_INFO_MID,TR_FILE_ID,(uint16_t)__LINE__,0)
#         define Tr_Info_Mid_1(msg,a1) Tr_Trace_Compact(stdout, TR_LVL_INFO_MID,TR_FILE_ID,(uint16_t)__LINE__,1,((uint32_t)a1))
#         define Tr_Info_Mid_2(msg,a1,a2) Tr_Trace_Compact(stdout, TR_LVL_INFO_MID,TR_FILE_ID,(uint16_t)__LINE__,2,((uint32_t)a1),((uint32_t)a2))
#         define Tr_Info_Mid_3(msg,a1,a2,a3) Tr_Trace_Compact(stdout, TR_LVL_INFO_MID,TR_FILE_ID,(uint16_t)__LINE__,3,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3))
#         define Tr_Info_Mid_4(msg,a1,a2,a3,a4) Tr_Trace_Compact(stdout, TR_LVL_INFO_MID,TR_FILE_ID,(uint16_t)__LINE__,4,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3),((uint32_t)a4))

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_MID */

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_LO)

#         define Tr_Info_Lo(msg)
#         define Tr_Info_Lo_1(msg,a1)
#         define Tr_Info_Lo_2(msg,a1,a2)
#         define Tr_Info_Lo_3(msg,a1,a2,a3)
#         define Tr_Info_Lo_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Info_Lo(msg) Tr_Trace_Compact(stdout, TR_LVL_INFO_LO,TR_FILE_ID,(uint16_t)__LINE__,0)
#         define Tr_Info_Lo_1(msg,a1) Tr_Trace_Compact(stdout, TR_LVL_INFO_LO,TR_FILE_ID,(uint16_t)__LINE__,1,((uint32_t)a1))
#         define Tr_Info_Lo_2(msg,a1,a2) Tr_Trace_Compact(stdout, TR_LVL_INFO_LO,TR_FILE_ID,(uint16_t)__LINE__,2,((uint32_t)a1),((uint32_t)a2))
#         define Tr_Info_Lo_3(msg,a1,a2,a3) Tr_Trace_Compact(stdout, TR_LVL_INFO_LO,TR_FILE_ID,(uint16_t)__LINE__,3,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3))
#         define Tr_Info_Lo_4(msg,a1,a2,a3,a4) Tr_Trace_Compact(stdout, TR_LVL_INFO_LO,TR_FILE_ID,(uint16_t)__LINE__,4,((uint32_t)a1),((uint32_t)a2),((uint32_t)a3),((uint32_t)a4))

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_LO */

#   elif (TR_COMPILE_VERBOSITY == TR_COMPILE_VERBOSE)
/*############################################################################
 *
 * VERBOSE TRACE
 *
 *   - The message string used.
 *   - The _BASE_FILE_NAME_ name string is used.
 *   - The "TR_FILE_ID" integral value is also used (to make sure it was
 *        defined by the file).
 *   - The __LINE__ value is used to identify the location of the trace call.
 *   - Any arguments are passed in a variable argument list and the assumption
 *       is that the message string somehow specifies their real type.
 *
 *############################################################################*/

#      define Tr_Fault(msg) Tr_Trace_Verbose(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg)
#      define Tr_Fault_1(msg,a1) Tr_Trace_Verbose(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1)
#      define Tr_Fault_2(msg,a1,a2) Tr_Trace_Verbose(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2)
#      define Tr_Fault_3(msg,a1,a2,a3) Tr_Trace_Verbose(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3)
#      define Tr_Fault_4(msg,a1,a2,a3,a4) Tr_Trace_Verbose(TR_FAULT_FP, TR_LVL_FAULT,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3,a4)

#      define Tr_Warn(msg) Tr_Trace_Verbose(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg)
#      define Tr_Warn_1(msg,a1) Tr_Trace_Verbose(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1)
#      define Tr_Warn_2(msg,a1,a2) Tr_Trace_Verbose(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2)
#      define Tr_Warn_3(msg,a1,a2,a3) Tr_Trace_Verbose(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3)
#      define Tr_Warn_4(msg,a1,a2,a3,a4) Tr_Trace_Verbose(TR_WARN_FP, TR_LVL_WARN,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3,a4)

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_NOTIFY)

#         define Tr_Notify(msg)
#         define Tr_Notify_1(msg,a1)
#         define Tr_Notify_2(msg,a1,a2)
#         define Tr_Notify_3(msg,a1,a2,a3)
#         define Tr_Notify_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Notify(msg) Tr_Trace_Verbose(stdout, TR_LVL_NOTIFY,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg)
#         define Tr_Notify_1(msg,a1) Tr_Trace_Verbose(stdout, TR_LVL_NOTIFY,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1)
#         define Tr_Notify_2(msg,a1,a2) Tr_Trace_Verbose(stdout, TR_LVL_NOTIFY,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2)
#         define Tr_Notify_3(msg,a1,a2,a3) Tr_Trace_Verbose(stdout, TR_LVL_NOTIFY,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3)
#         define Tr_Notify_4(msg,a1,a2,a3,a4) Tr_Trace_Verbose(stdout, TR_LVL_NOTIFY,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3,a4)

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_NOTIFY */

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_HI)

#         define Tr_Info_Hi(msg)
#         define Tr_Info_Hi_1(msg,a1)
#         define Tr_Info_Hi_2(msg,a1,a2)
#         define Tr_Info_Hi_3(msg,a1,a2,a3)
#         define Tr_Info_Hi_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Info_Hi(msg) Tr_Trace_Verbose(stdout, TR_LVL_INFO_HI,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg)
#         define Tr_Info_Hi_1(msg,a1) Tr_Trace_Verbose(stdout, TR_LVL_INFO_HI,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1)
#         define Tr_Info_Hi_2(msg,a1,a2) Tr_Trace_Verbose(stdout, TR_LVL_INFO_HI,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2)
#         define Tr_Info_Hi_3(msg,a1,a2,a3) Tr_Trace_Verbose(stdout, TR_LVL_INFO_HI,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3)
#         define Tr_Info_Hi_4(msg,a1,a2,a3,a4) Tr_Trace_Verbose(stdout, TR_LVL_INFO_HI,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3,a4)

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_HI */

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_MID)

#         define Tr_Info_Mid(msg)
#         define Tr_Info_Mid_1(msg,a1)
#         define Tr_Info_Mid_2(msg,a1,a2)
#         define Tr_Info_Mid_3(msg,a1,a2,a3)
#         define Tr_Info_Mid_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Info_Mid(msg) Tr_Trace_Verbose(stdout, TR_LVL_INFO_MID,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg)
#         define Tr_Info_Mid_1(msg,a1) Tr_Trace_Verbose(stdout, TR_LVL_INFO_MID,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1)
#         define Tr_Info_Mid_2(msg,a1,a2) Tr_Trace_Verbose(stdout, TR_LVL_INFO_MID,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2)
#         define Tr_Info_Mid_3(msg,a1,a2,a3) Tr_Trace_Verbose(stdout, TR_LVL_INFO_MID,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3)
#         define Tr_Info_Mid_4(msg,a1,a2,a3,a4) Tr_Trace_Verbose(stdout, TR_LVL_INFO_MID,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3,a4)

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_MID */

#      if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_LO)

#         define Tr_Info_Lo(msg)
#         define Tr_Info_Lo_1(msg,a1)
#         define Tr_Info_Lo_2(msg,a1,a2)
#         define Tr_Info_Lo_3(msg,a1,a2,a3)
#         define Tr_Info_Lo_4(msg,a1,a2,a3,a4)

#      else

#         define Tr_Info_Lo(msg) Tr_Trace_Verbose(stdout, TR_LVL_INFO_LO,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg)
#         define Tr_Info_Lo_1(msg,a1) Tr_Trace_Verbose(stdout, TR_LVL_INFO_LO,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1)
#         define Tr_Info_Lo_2(msg,a1,a2) Tr_Trace_Verbose(stdout, TR_LVL_INFO_LO,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2)
#         define Tr_Info_Lo_3(msg,a1,a2,a3) Tr_Trace_Verbose(stdout, TR_LVL_INFO_LO,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3)
#         define Tr_Info_Lo_4(msg,a1,a2,a3,a4) Tr_Trace_Verbose(stdout, TR_LVL_INFO_LO,TR_FILE_ID,_BASE_FILE_NAME_,(uint16_t)__LINE__,msg,a1,a2,a3,a4)

#      endif                    /* TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_LO */

#   else

#      error TR_COMPILE_VERBOSITY is not set to a supported value.

#   endif                       /* if (TR_COMPILE_VERBOSITY == TR_COMPILE_DISABLED) */

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Returns the minimum "INFO" trace level that will be logged at run time for
 * the specified module. @see Tr_Set_Info_Trace_Level for a description of
 * the way the trace level affects the messages logged.
 *
 * @return the minimum INFO trace level logged for the specified module.
 *
 * @param [in] module_id Identifies the module to which the setting applies.
 */
Tr_Trace_Level_T Tr_Get_Module_Info_Trace_Level(Tr_Module_ID_T module_id);

/**
 * Reads the dbg_trace level settings from the specified file.
 *
 * @param path The path to the file from which to read the levels.
 */
void Tr_Read_Trace_Levels(char const *path);

/**
 * Saves the dbg_trace level settings to the specified file.
 *
 * @param path The path to the file into which the levels are written.
 */
void Tr_Save_Trace_Levels(char const *path);

/**
 * Sets the minimum "INFO" trace level that will be logged at run time for
 * all modules. Values TR_LVL_FAULT and TR_LVL_WARN will cause all NOTIFY and
 * INFO messages to be discarded. FAULT and WARN level trace messages are
 * never filtered out. A value of TR_LVL_NOTIFY will only add NOTIFY messages
 * to those enabled. A value TR_LVL_INFO_HI will allow NOTIFY and "high"
 * INFO trace messages to be logged. A value TR_LVL_INFO_MID will allow
 * NOTIFY, "high", and "mid" level INFO messages. Finally, a value of
 * TR_LVL_INFO_LO will allow all messages to be logged.
 *
 * @param [in] lvl Trace message level (fault, warning, info_high, etc.)
 */
void Tr_Set_Info_Trace_Level(Tr_Trace_Level_T lvl);

/**
 * Sets the minimum "INFO" trace level that will be logged at run time for
 * the specified module. @see Tr_Set_Info_Trace_Level for a description of
 * the way the trace level affects the messages logged.
 *
 * @param [in] module_id Identifies the module to which the setting applies.
 *
 * @param [in] lvl Trace message level (fault, warning, info_high, etc.)
 */
void Tr_Set_Module_Info_Trace_Level(Tr_Module_ID_T module_id, Tr_Trace_Level_T lvl);

/**
 * Logs a compact trace message; do NOT call directly. This function should
 * only be called through use of the trace MACROs defined above.
 *
 * @param [in] fp Pointer to the output stream.
 *
 * @param [in] lvl Trace message level (fault, warning, info_high, etc.)
 *
 * @param [in] f_num "TR_FILE_ID" identifier for the file from which the trace
 *                   originates; this value is generated by the EM module's
 *                   EM_FILENUM MACRO.
 *
 * @param [in] line_num the C __LINE__ value that identifies the line at which
 *                      the trace originates.
 *
 * @param [in] num_args Number of arguments in the variable argument list; each
 *                      of these has been cast to uint32_t.
 */
void Tr_Trace_Compact(FILE* fp, Tr_Trace_Level_T lvl, uint16_t f_num, uint16_t line_num, size_t num_args, ...);

/**
 * Logs a verbose trace message; do NOT call directly. This function should
 * only be called through use of the trace MACROs defined above.
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
void Tr_Trace_Verbose(FILE* fp, Tr_Trace_Level_T lvl, uint16_t f_num, char_t const *f_name, uint16_t line_num, char_t const *msg,
   ...)
#      if defined(__GNUC__) && !defined(TR_DISABLE_FORMAT_ATTR)
             __attribute__ ((format (printf, 6, 7)))
#      endif
;

#   ifdef __cplusplus
} /* extern "C" */
#   endif                    /* __cplusplus */
/*===========================================================================*/
/*!
 * @file dbg_trace.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 02-Jun-2011 kirk bailey Rev. 8
 *   - Task 34345 - Added Tr_Read_Trace_Levels and Tr_Save_Trace_Levels.
 *
 * - 04-Nov-2011 kirk bailey Rev. 9
 *   - Added NOTIFY level.
 *
 * - 25-Jan-2012 kirk bailey Rev. 10
 *   - Added use of function attribute (gcc extension) to have the format
 *     specifiers' syntax checked.
 *
 * - 03-Oct-2013 Kirk Bailey REV 11
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 *
 * - 03-Oct-2013 Kirk Bailey REV 12
 *   - Eliminated support of ISA debugtrace.
 *
 * - 24-Feb-2014 Paul Casto REV 13
 *   - Send output of "Tr_Fault_*" and "Tr_Warn_*" macros to TR_FAULT_FP
 *     and TR_WARN_FP file streams, respectively.
 *
 * - 08-Aug-2014 Kirk Bailey REV 14
 *   - Added ability to override __FILE__ with _BASE_FILE_NAME_.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* DBG_TRACE_H */
