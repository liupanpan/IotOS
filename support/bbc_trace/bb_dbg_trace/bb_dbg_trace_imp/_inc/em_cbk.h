#ifndef EM_CBK_H
#   define EM_CBK_H
/*===========================================================================*/
/**
 * @file em_cbk.h
 *
 * Defines the callout API for the EM  Asserts
 *
 * %full_filespec:em_cbk.h~kok_inf#5:incl:kok_aud#4 %
 * @version %version:kok_inf#5 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 09:34:02 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - EM = Error Management
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Error_Management.doc (version 0.7)
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
 * @addtogroup em
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define Constants for Callouts
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS for Callouts
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations for Callouts
 *===========================================================================*/

#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */

/*===========================================================================*
 * Exported Function Prototypes for Callouts
 *===========================================================================*/

/**
 * This callout is made when an EM assert occurs; it should implement
 * whatever system action is appropriate for a system fault. Keep in mind that
 * a basic assumption of assert-based logic is that this call NEVER returns.
 */
void EM_Assert(void);

#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */

/*===========================================================================*/
/*!
 * @file em_cbk.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 12-Jun-2011 Kirk Bailey Rev. 4
 *   - Eliminated configurable assert types; replaced with a single callout.
 *
 * - 03-Oct-2013 Kirk Bailey REV 5
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* EM_ICBK_H */
