#ifndef PRINTF_GATE_H
#   define PRINTF_GATE_H
/*===========================================================================*/
/**
 * @file printf_gate.h
 *
 * @brief Defines functions to serialize access to stdout (printf)
 *
 * %full_filespec:printf_gate.h~1:incl:kok_inf#12 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Apr 15 17:34:18 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 * ABBREVIATIONS:
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - None.
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/*===========================================================================*\
 * Exported Preprocessor #define Constants for Callouts
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS for Callouts
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations for Callouts
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes for Callouts
\*===========================================================================*/
/**
 * Locks access to printf 
 *    Use in pairs with 
 * @see Printf_Unlock
 */
void Printf_Lock(void);

/**
 * Unlocks access to printf (allows other threads to gain access)
 *    Use in pairs with 
 * @see Printf_Lock
 */
void Printf_Unlock(void);

#   ifdef __cplusplus
}         /* extern "C" */
#   endif /* __cplusplus */
/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 06-oct-2010 Kirk Bailey
 * + Task 15260 - added extern C.
 *
 * 17-Apr-2007 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
#endif /* PRINTF_GATE_H */
