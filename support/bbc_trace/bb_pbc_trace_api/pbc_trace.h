#ifndef PBC_TRACE_H
#   define PBC_TRACE_H
/*===========================================================================*/
/**
 * @file pbc_trace.h
 *
 *  Combines dbg_trace and EM module MACROs to provide Programming By
 *  Contract (PBC) support.
 *
 * %full_filespec: pbc_trace.h~kok_inf#7:incl:kok_aud#1 %
 * @version %version: kok_inf#7 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Thu Oct  3 09:58:44 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Although the behavior is the same, separate MACROs are provided for
 * invariant conditions, postconditions, and preconditions to provide
 * a documentation mechanism. In addition, an "internal" condition
 * is supported, which can cover all three cases but can be conditionally
 * compiled out. The internal MACROs are only for use in checking intra-
 * module contracts; once a module has been tested, these checks can be
 * disabled. Inter-module contracts must always be check; hence, the
 * invariant, post, and pre condition MACROs can not be disabled.
 *
 * The Tr_Fault(_x) MACROs are used to report the trace message associated
 * with the failure of a condition. The trace verbosity level setting for
 * the dbg_trace module determines whether the trace message is in "verbose" or
 * "compact" mode.
 *
 * @section ABBR ABBREVIATIONS:
 *   - EM  - Error Management
 *   - PBC - Programming By Contract
 *   - TR  - TRace
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
 * @defgroup pbc_trace PBC - Programming By Contract
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "dbg_trace.h"
#   include "em_assert.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/**
 * EM_ASSERT_CHECK_INTERNAL is used in order to check assertions 
 * used inside modules as opposed to at API interfaces. 
 *
 * In order to configure this on a module-by-module basis, the makefile 
 * for a module would include the following -DEM_ASSERT_CHECK_INTERNAL=false
 * (see Makefile_bb_em_tst.mk in the em mdf test directory for an example)
 *
 * Alternative is explictly set value by setting this define 
 * before include of pbc_trace.h 
 * @code 
 * #   undef EM_ASSERT_CHECK_INTERNAL
 * #   define EM_ASSERT_CHECK_INTERNAL false    / **< disable internal checks for this file * /
 *   or 
 * #   undef EM_ASSERT_CHECK_INTERNAL
 * #   define EM_ASSERT_CHECK_INTERNAL true    / **< force internal checks for this file * /
 * @endcode
 */
#   ifndef EM_ASSERT_CHECK_INTERNAL
#      define EM_ASSERT_CHECK_INTERNAL  true    /* default to checks on */
#   elif (EM_ASSERT_CHECK_INTERNAL != true) && (EM_ASSERT_CHECK_INTERNAL != false)
#      error Illegal value for EM_ASSERT_CHECK_INTERNAL
#   endif

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Verifies that the specified postcondition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 */
#   define PBC_Ensure(expr_,msg_)          \
if(expr_){}                                \
else {                                     \
   Tr_Fault(msg_);                         \
   EM_Perform_Assert();                    \
}

/**
 * Verifies that the specified postcondition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 */
#   define PBC_Ensure_1(expr_,msg_,a1_)    \
if(expr_){}                                \
else {                                     \
   Tr_Fault_1(msg_,a1_);                   \
   EM_Perform_Assert();                    \
}

/**
 * Verifies that the specified postcondition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 */
#   define PBC_Ensure_2(expr_,msg_,a1_,a2_)\
if(expr_){}                                \
else {                                     \
   Tr_Fault_2(msg_,a1_,a2_);               \
   EM_Perform_Assert();                    \
}

/**
 * Verifies that the specified postcondition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 */
#   define PBC_Ensure_3(expr_,msg_,a1_,a2_,a3_)\
if(expr_){}                                    \
else {                                         \
   Tr_Fault_3(msg_,a1_,a2_,a3_);               \
   EM_Perform_Assert();                        \
}

/**
 * Verifies that the specified postcondition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted 
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 * @param a4_ [in]
 *   the 4th argument referenced by the trace message format string.
 */
#   define PBC_Ensure_4(expr_,msg_,a1_,a2_,a3_,a4_)\
if(expr_){}                                        \
else {                                             \
   Tr_Fault_4(msg_,a1_,a2_,a3_,a4_);               \
   EM_Perform_Assert();                            \
}

/**
 * Issues a trace message for the detected contract violation and
 * calls the system-dependent handler.
 *
 * @param msg_ [in] the format string for trace message issued.
 */
#   define PBC_Failed(msg_)                \
   Tr_Fault(msg_);                         \
   EM_Perform_Assert();

/**
 * Issues a trace message for the detected contract violation and
 * calls the system-dependent handler.
 *
 * @param msg_ [in] the format string for trace message issued.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 */
#   define PBC_Failed_1(msg_,a1_)          \
   Tr_Fault_1(msg_,a1_);                   \
   EM_Perform_Assert();

/**
 * Issues a trace message for the detected contract violation and
 * calls the system-dependent handler.
 *
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 */
#   define PBC_Failed_2(msg_,a1_,a2_)      \
   Tr_Fault_2(msg_,a1_,a2_);               \
   EM_Perform_Assert();

/**
 * Issues a trace message for the detected contract violation and
 * calls the system-dependent handler.
 *
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 */
#   define PBC_Failed_3(msg_,a1_,a2_,a3_)  \
   Tr_Fault_3(msg_,a1_,a2_,a3_);           \
   EM_Perform_Assert();

/**
 * Issues a trace message for the detected contract violation and
 * calls the system-dependent handler.
 *
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 * @param a4_ [in]
 *   the 4th argument referenced by the trace message format string.
 */
#   define PBC_Failed_4(msg_,a1_,a2_,a3_,a4_) \
   Tr_Fault_4(msg_,a1_,a2_,a3_,a4_);          \
   EM_Perform_Assert();;

/**
 * Verifies that the specified internal condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 */
#   if (EM_ASSERT_CHECK_INTERNAL == true)

#      define PBC_Internal(expr_,msg_)     \
if(expr_){}                                \
else {                                     \
   Tr_Fault(msg_);                         \
   EM_Perform_Assert();                    \
}

#   else

#      define PBC_Internal(expr_,msg_) ((void)0)

#   endif                       /* EM_ASSERT_CHECK_INTERNAL == true */

/**
 * Verifies that the specified internal condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 */
#   if (EM_ASSERT_CHECK_INTERNAL == true)

#      define PBC_Internal_1(expr_,msg_,a1_)\
if(expr_){}                                 \
else {                                      \
   Tr_Fault_1(msg_,a1_);                    \
   EM_Perform_Assert();                     \
}

#   else

#      define PBC_Internal_1(expr_,msg_,a1_) ((void)0)

#   endif                       /* EM_ASSERT_CHECK_INTERNAL == true */

/**
 * Verifies that the specified internal condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 */
#   if (EM_ASSERT_CHECK_INTERNAL == true)

#      define PBC_Internal_2(expr_,msg_,a1_,a2_)\
if(expr_){}                                     \
else {                                          \
   Tr_Fault_2(msg_,a1_,a2_);                    \
   EM_Perform_Assert();                         \
}

#   else

#      define PBC_Internal_2(expr_,msg_,a1_,a2_) ((void)0)

#   endif                       /* EM_ASSERT_CHECK_INTERNAL == true */

/**
 * Verifies that the specified internal condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 */
#   if (EM_ASSERT_CHECK_INTERNAL == true)

#      define PBC_Internal_3(expr_,msg_,a1_,a2_,a3_)\
if(expr_){}                                         \
else {                                              \
   Tr_Fault_3(msg_,a1_,a2_,a3_);                    \
   EM_Perform_Assert();                             \
}

#   else

#      define PBC_Internal_3(expr_,msg_,a1_,a2_,a3_) ((void)0)

#   endif                       /* EM_ASSERT_CHECK_INTERNAL == true */

/**
 * Verifies that the specified internal condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted 
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 * @param a4_ [in]
 *   the 4th argument referenced by the trace message format string.
 */
#   if (EM_ASSERT_CHECK_INTERNAL == true)

#      define PBC_Internal_4(expr_,msg_,a1_,a2_,a3_,a4_)\
if(expr_){}                                             \
else {                                                  \
   Tr_Fault_4(msg_,a1_,a2_,a3_,a4_);                    \
   EM_Perform_Assert();                                 \
}

#   else

#      define PBC_Internal_4(expr_,msg_,a1_,a2_,a3_,a4_) ((void)0)

#   endif                       /* EM_ASSERT_CHECK_INTERNAL == true */

/**
 * Verifies that the specified invariant condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted 
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 */
#   define PBC_Invariant(expr_,msg_)       \
if(expr_){}                                \
else {                                     \
   Tr_Fault(msg_);                         \
   EM_Perform_Assert();                    \
}

/**
 * Verifies that the specified invariant condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 */
#   define PBC_Invariant_1(expr_,msg_,a1_) \
if(expr_){}                                \
else {                                     \
   Tr_Fault_1(msg_,a1_);                   \
   EM_Perform_Assert();                    \
}

/**
 * Verifies that the specified invariant condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 */
#   define PBC_Invariant_2(expr_,msg_,a1_,a2_)\
if(expr_){}                                   \
else {                                        \
   Tr_Fault_2(msg_,a1_,a2_);                  \
   EM_Perform_Assert();                       \
}

/**
 * Verifies that the specified invariant condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 */
#   define PBC_Invariant_3(expr_,msg_,a1_,a2_,a3_)\
if(expr_){}                                       \
else {                                            \
   Tr_Fault_3(msg_,a1_,a2_,a3_);                  \
   EM_Perform_Assert();                           \
}

/**
 * Verifies that the specified invariant condition is true; if it isn't, the
 * specified trace message is printed and the contract violation is noted 
 * (which leads to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 * @param a4_ [in]
 *   the 4th argument referenced by the trace message format string.
 */
#   define PBC_Invariant_4(expr_,msg_,a1_,a2_,a3_,a4_)\
if(expr_){}                                           \
else {                                                \
   Tr_Fault_4(msg_,a1_,a2_,a3_,a4_);                  \
   EM_Perform_Assert();                               \
}

/**
 * Verifies that the specified precondition is true; if it isn't, the specified
 * trace message is printed and the contract violation is noted (which leads
 * to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 */
#   define PBC_Require(expr_,msg_)         \
if(expr_){}                                \
else {                                     \
   Tr_Fault(msg_);                         \
   EM_Perform_Assert();                    \
}

/**
 * Verifies that the specified precondition is true; if it isn't, the specified
 * trace message is printed and the contract violation is noted (which leads
 * to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 */
#   define PBC_Require_1(expr_,msg_,a1_)   \
if(expr_){}                                \
else {                                     \
   Tr_Fault_1(msg_,a1_);                   \
   EM_Perform_Assert();                    \
}

/**
 * Verifies that the specified precondition is true; if it isn't, the specified
 * trace message is printed and the contract violation is noted (which leads
 * to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 */
#   define PBC_Require_2(expr_,msg_,a1_,a2_)\
if(expr_){}                                 \
else {                                      \
   Tr_Fault_2(msg_,a1_,a2_);                \
   EM_Perform_Assert();                     \
}

/**
 * Verifies that the specified precondition is true; if it isn't, the specified
 * trace message is printed and the contract violation is noted (which leads
 * to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 */
#   define PBC_Require_3(expr_,msg_,a1_,a2_,a3_)\
if(expr_){}                                     \
else {                                          \
   Tr_Fault_3(msg_,a1_,a2_,a3_);                \
   EM_Perform_Assert();                         \
}

/**
 * Verifies that the specified precondition is true; if it isn't, the specified
 * trace message is printed and the contract violation is noted (which leads
 * to system-dependent behavior - typically a reset).
 *
 * @param expr_ [in] the expression that is required to be true.
 * @param msg_ [in] the format string for trace message issued if the
 *                  expression is false.
 * @param a1_ [in]
 *   the 1st argument referenced by the trace message format string.
 * @param a2_ [in]
 *   the 2nd argument referenced by the trace message format string.
 * @param a3_ [in]
 *   the 3rd argument referenced by the trace message format string.
 * @param a4_ [in]
 *   the 4th argument referenced by the trace message format string.
 */
#   define PBC_Require_4(expr_,msg_,a1_,a2_,a3_,a4_)\
if(expr_){}                                         \
else {                                              \
   Tr_Fault_4(msg_,a1_,a2_,a3_,a4_);                \
   EM_Perform_Assert();                             \
}

/*===========================================================================*/
/*!
 * @file pbc_trace.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 03-Oct-2013 Kirk Bailey REV 7
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* PBC_TRACE_H */
