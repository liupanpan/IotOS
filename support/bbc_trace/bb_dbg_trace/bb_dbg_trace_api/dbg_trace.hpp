#ifndef DBG_TRACE_HPP
#   define DBG_TRACE_HPP
/*===========================================================================*/
/**
 * @file dbg_trace.hpp
 *
 *   Declares MACROs that can be used by C++ code to log expressions that
 *   use the "<<" operator while maintaining the efficiency of not evaluating
 *   the expression unless the result will in fact be logged.
 *
 * %full_filespec: dbg_trace.hpp~3:ascii:kok_inf#1 %
 * @version %version: 3 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Thu May  8 07:33:33 2014 %
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
 *  C++ code often uses the "<<" operator to produce text descriptions of
 *  instances. It is only natural to want to use this information in trace
 *  messages. Non-error trace logic also tries to be efficient by not
 *  spending time generating trace strings that end up not being logged
 *  (due to trace level settings). The MACROs declared in this header file
 *  avoid evaluating an expression containing "<<" unless it is known that
 *  the result will be logged.
 *
 * @section ABBR ABBREVIATIONS:
 *   - EM   - Error Management
 *   - TR   - TRace
 *   - Trpp - TRace for C++
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
 * @addtogroup dbg_trace_client Debug Trace Client Interface
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "dbg_trace.h"
#   include <iostream>
#   include <sstream>

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/**
 * Allows C++ code to log FAULTs using an argument that contains an expression
 * that uses the "<<" operator. This allows logs to easily contain the
 * information from classes that override "<<" to provide a meaningful
 * representation of their instances. Note that tr_Is_Trace_Level_Enabled()
 * is NOT called since FAULTs are always enabled.
 *
 * @param [in] arg Expression that will be evaluated with "<<" operator to
 *                 generate the trace message.
 */
#   define Tr_Cpp_Fault(arg)                                      \
   do {                                                           \
      std::ostringstream _oss;                                    \
      _oss << arg;                                                \
      Tr_Fault_1("%s", _oss.str().c_str());                       \
    } while (0)

/**
 * Allows C++ code to log WARNings using an argument that contains an expression
 * that uses the "<<" operator. This allows logs to easily contain the
 * information from classes that override "<<" to provide a meaningful
 * representation of their instances. Note that tr_Is_Trace_Level_Enabled()
 * is NOT called since WARNingss are always enabled.
 *
 * @param [in] arg Expression that will be evaluated with "<<" operator to
 *                 generate the trace message.
 */
#   define Tr_Cpp_Warn(arg)                                       \
   do {                                                           \
      std::ostringstream _oss;                                    \
      _oss << arg;                                                \
      Tr_Warn_1("%s", _oss.str().c_str());                        \
    } while (0)

/**
 * Allows C++ code to log NOTIFY messages using an argument containing an
 * expression that uses the "<<" operator. This allows logs to easily contain
 * the information from classes that override "<<" to provide a meaningful
 * representation of their instances. Note that tr_Is_Trace_Level_Enabled()
 * is called to verify that NOTIFY messages are enabled for the module
 * prior to evaluating the expression. This prevents the expression from
 * being evaluated unless the result will be logged.
 *
 * @param [in] arg Expression that will be evaluated with "<<" operator to
 *                 generate the trace message.
 */
#   if (TR_COMPILE_INFO_LEVEL < TR_LVL_NOTIFY)
#      define Tr_Cpp_Notify(arg)
#   else
#      define Tr_Cpp_Notify(arg)                                      \
       do {                                                           \
          if (tr_Is_Trace_Level_Enabled(TR_FILE_ID, TR_LVL_NOTIFY))   \
          {                                                           \
             std::ostringstream _oss;                                 \
             _oss << arg;                                             \
             Tr_Notify_1("%s", _oss.str().c_str());                   \
         }                                                            \
        } while (0)
#   endif

/**
 * Allows C++ code to log INFO_HI messages using an argument containing an
 * expression that uses the "<<" operator. This allows logs to easily contain
 * the information from classes that override "<<" to provide a meaningful
 * representation of their instances. Note that tr_Is_Trace_Level_Enabled()
 * is called to verify that INFO_HI messages are enabled for the module
 * prior to evaluating the expression. This prevents the expression from
 * being evaluated unless the result will be logged.
 *
 * @param [in] arg Expression that will be evaluated with "<<" operator to
 *                 generate the trace message.
 */
#   if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_HI)
#      define Tr_Cpp_Info_Hi(arg)
#   else
#      define Tr_Cpp_Info_Hi(arg)                                     \
       do {                                                           \
          if (tr_Is_Trace_Level_Enabled(TR_FILE_ID, TR_LVL_INFO_HI))  \
          {                                                           \
             std::ostringstream _oss;                                 \
             _oss << arg;                                             \
             Tr_Info_Hi_1("%s", _oss.str().c_str());                  \
         }                                                            \
        } while (0)
#   endif

/**
 * Allows C++ code to log INFO_MID messages using an argument containing an
 * expression that uses the "<<" operator. This allows logs to easily contain
 * the information from classes that override "<<" to provide a meaningful
 * representation of their instances. Note that tr_Is_Trace_Level_Enabled()
 * is called to verify that INFO_MID messages are enabled for the module
 * prior to evaluating the expression. This prevents the expression from
 * being evaluated unless the result will be logged.
 *
 * @param [in] arg Expression that will be evaluated with "<<" operator to
 *                 generate the trace message.
 */
#   if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_MID)
#      define Tr_Cpp_Info_Mid(arg)
#   else
#      define Tr_Cpp_Info_Mid(arg)                                    \
       do {                                                           \
          if (tr_Is_Trace_Level_Enabled(TR_FILE_ID, TR_LVL_INFO_MID)) \
          {                                                           \
             std::ostringstream _oss;                                 \
             _oss << arg;                                             \
             Tr_Info_Mid_1("%s", _oss.str().c_str());                 \
         }                                                            \
        } while (0)
#   endif

/**
 * Allows C++ code to log INFO_LO messages using an argument containing an
 * expression that uses the "<<" operator. This allows logs to easily contain
 * the information from classes that override "<<" to provide a meaningful
 * representation of their instances. Note that tr_Is_Trace_Level_Enabled()
 * is called to verify that INFO_LO messages are enabled for the module
 * prior to evaluating the expression. This prevents the expression from
 * being evaluated unless the result will be logged.
 *
 * @param [in] arg Expression that will be evaluated with "<<" operator to
 *                 generate the trace message.
 */
#   if (TR_COMPILE_INFO_LEVEL < TR_LVL_INFO_LO)
#      define Tr_Cpp_Info_Lo(arg)
#   else
#      define Tr_Cpp_Info_Lo(arg)                                     \
       do {                                                           \
          if (tr_Is_Trace_Level_Enabled(TR_FILE_ID, TR_LVL_INFO_LO))  \
          {                                                           \
             std::ostringstream _oss;                                 \
             _oss << arg;                                             \
             Tr_Info_Lo_1("%s", _oss.str().c_str());                  \
         }                                                            \
        } while (0)
#endif

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

extern "C"
{

/**
 * Returns an indication of whether the specified trace level is enabled for
 * the module associated with the specified file id.
 *
 * @return true if the trace level is enabled; false otherwise.
 *
 * @param file_id [in] identifier for file issuing trace statement.
 * @param level [in] trace level associated with trace statement.
 */
bool_t tr_Is_Trace_Level_Enabled(uint16_t file_id, Tr_Trace_Level_T level);

} /* extern "C" */

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/


/*===========================================================================*/
/*!
 * @file dbg_trace.hpp
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 15-Aug-2013 Kirk Bailey
 *   - Created initial file.
 *
 * - 03-Oct-2013 Kirk Bailey
 *   - Fixed revision comment order - no functional changes.
 *
 * - 08-May-2014 Kirk Bailey
 *   - Added logic to allow non-error trace calls to be compiled out.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* DBG_TRACE_HPP */
