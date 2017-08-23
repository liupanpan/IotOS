#ifndef PROCMAN_CBK_H
#   define PROCMAN_CBK_H
/*===========================================================================*/
/**
 * @file procman_cbk.h
 *
 * Timer_Server callouts
 *
 * %full_filespec:procman_cbk.h~1:incl:kok_basa#1 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Thu Oct 13 10:15:28 2011 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Timer_Server callouts - Standard BASA XSAL interfaces and Lookup of file name to play
 *
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup ssnd implementation of Audrey (Timer_Server)
 * @ingroup procman
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "xsal.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/*===========================================================================*
 * Imported constants
 *===========================================================================*/
 
/*===========================================================================*
 * Imported Function Prototypes for Callouts
 *===========================================================================*/

/**
 *  Returns the XSAL thread setting for Name Server
 */
void Name_Server_Get_Thread_Attr(SAL_Thread_Attr_T * thread_attr);

/**
 *  Returns the XSAL thread setting for Main PM
 */
void PM_Main_Get_Thread_Attr(SAL_Thread_Attr_T * thread_attr);

/**
 *  Returns the XSAL thread setting for Exit Monitor
 */
void PM_Exit_Monitor_Get_Thread_Attr(SAL_Thread_Attr_T * thread_attr);


#   ifdef __cplusplus
}         /* extern "C" */
#   endif /* __cplusplus */
/*===========================================================================*/
/*!
 * @file procman_cbk.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 06-Oct-2011 Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* PROCMAN_CBK_H */
