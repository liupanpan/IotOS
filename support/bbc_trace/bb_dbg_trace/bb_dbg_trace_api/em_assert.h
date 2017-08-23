#ifndef EM_ASSERT_H
#   define EM_ASSERT_H
/*===========================================================================*/
/**
 * @file em_assert.h
 *
 * API for Error Management (EM) Assertions
  *
 * %full_filespec:em_assert.h~kok_inf#6:incl:kok_aud#1 %
 * @version %version:kok_inf#6 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 09:33:59 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Defines customizable and memory-efficient assertions applicable to
 * embedded systems. This header file can be used in C, C++, and mixed C/C++
 * programs.
 *
 * THIS VERSION HAS ALL TRACE SUPPORT REMOVED
 *
 * @section ABBR ABBREVIATIONS:
 *   - EM = Error Management
 *   - PbC = Programming by Contract 
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Error_Management.doc (version 0.8)
 *
 *   - Requirements Document(s):
 *     BASA_SRS_Error_Management_1.2.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @addtogroup pbc Error Management Assertions
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{
#   endif       /*__cplusplus*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/**
 * Performs the actual invocation of the configured EM assert. Unlike the
 * EM_Assert_Handler() call, this call does not log any information that
 * can be used to identify the source of the assert. For this reason, it
 * should only be used in rare situations that require it, such as an assert
 * that occurs during the logging of another assert (or an assert fired
 * after other specific action was taken to log the cause).
 */
void EM_Perform_Assert(void);

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * Compile-time assertion produces a compiler error if invalid;  produces no
 * code if valid. For use at function scope. from http://www.panelsoft.com/murphyslaw/apr01.htm 
 */
#   define EM_ASSERT_COMPILE(ex) do {\
typedef uint8_t COMPILE_TIME_ASSERTION_FAILURE[(ex) ? 1 : -1];\
} while(0)

/**
 * Compile-time assertion produces a compiler error if invalid;  produces no
 * code if valid. For use at file scope. From http://www.panelsoft.com/murphyslaw/apr01.htm 
 */
#   define EM_FS_ASSERT_COMPILE(ex) extern uint8_t COMPILE_TIME_ASSERTION_FAILURE[(ex) ? 1 : -1]

#   ifdef __cplusplus
}                               /* close off extern "C" specification */
#   endif  /*__cplusplus*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*/
/*!
 * @file em_assert.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 * 
 *  - 22-Dec-2008 Dan Carman
 *    Gutted EM API to remove duplication with PBC / Trace interface
 * 
 *  - 22-Dec-2008 Dan Carman
 *    - Task kok_aud 56926: Removed deprecated EM_Disable/Restore assert calls.
 *
 * - 03-Oct-2013 Kirk Bailey REV 6
 *   - Fixed order of revision history comments - no functional changes
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* EM_ASSERT_H */
