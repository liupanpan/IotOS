#ifndef XSAL_CBK_H
#   define XSAL_CBK_H
/*===========================================================================*/
/**
 * @file xsal_cbk.h
 *
 * Defines the callout API for XSAL Light
 *
 * %full_filespec:xsal_cbk.h~3:incl:kok_aud#3 %
 * @version %version:3 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Wed Mar 19 08:17:23 2008 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *    Defines callouts from XSAL kernel to
 *       Tie into the 1 mS timer thread - Low overhead for high frequency execution 
 *       Log messages 
 *       Handle dropped messages 
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
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "xsal.h"

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
 * Exported Function Prototypes for Callouts
\*===========================================================================*/

/*!!!!!!!!!!!!!!!!!!!*/
#   if !defined(XSAL) /* These are currently only supported in XSAL Light */
/*!!!!!!!!!!!!!!!!!!!*/

/**
 * @addtogroup SAL_Timer_Grp Timer
 * @{
 */

/**
 * Application hook into 1 mS timer task
 *
 * @post This function should never block and must execute in << 1 mS.
 *
 * @note This callout should be used judicially only for functions that must execute 
 * every mS.  Functions that execute at slower rate should use standard XSAL timers.
 */
void SAL_1_mS_Tasks(void);

/**
 * @} doxygen end group
 *
 * @addtogroup SAL_Message_Grp
 * @{
 */

/**
 * Application hook to log messages
 *
 *  @param [in] message    pointer to message being sent 
 *
 * @post Minimum processing is none
 */
void SAL_Log_Message(const SAL_Message_T * message);

/**
 *  Application callout when message is dropped due to non-existent or full queue
 *    of the receiving thread. 
 *
 *  @param  dropped_message   pointer message unable to send
 *
 *  @note - Mininum functionality should be to generate an assert. 
 */
void SAL_Dropped_Message(const SAL_Message_T * dropped_message);

/*!!!!!!!!!!!!!!!!!!!*/
#   endif /* !XSAL */
/*!!!!!!!!!!!!!!!!!!!*/

/*===========================================================================*/
/*!
 * @file xsal_cbk.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 19-mar-2008 kirk bailey
 *    - Added conditional compilation since callbacks not yet supported by
 *      full XSAL.
 *
 * - 08-Apr-2007 Dan Carman
 *    - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* XSAL_CBK_H */
