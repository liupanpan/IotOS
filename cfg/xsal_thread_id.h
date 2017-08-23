#ifndef XSAL_THREAD_ID_H
#   define XSAL_THREAD_ID_H
/*===========================================================================*/
/**
 * @file xsal_thread_id.h
 *
 *   Configuration file assigns thread ids for each process in the system.
 *
 * %full_filespec:xsal_thread_id.h~4:incl:kok_basa#1 %
 * @version %version:4 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Fri Apr 17 14:55:28 2015 %
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
 *   Thread IDs must be unique within each process. This file provides the single
 *   location for the assignment of these IDS.
 *
 *    A seperate enumeration should be defined for each process
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

/*===========================================================================*
 * Header Files
 *===========================================================================*/

/*===========================================================================*
 * #define Constants
 *===========================================================================*/

/*===========================================================================*
 * #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Custom Type Declarations
 *===========================================================================*/

/**
 * The following thread ids are for procman and should not be modified.
 */

enum Procman_Thread_Id_Tag
{
   NAME_SERVER_THREAD = XSAL_FIRST_APPLICATION_THREAD_ID,
   PM_MAIN_THREAD,
   PM_EXIT_MONITOR_THREAD,
#if defined (QNX_NTO)
    PM_POL_THREAD,
#endif /* QNX_NTO */
#if defined (XSAL_NET)
    NET_GATEWAY_THREAD,
#endif /* XSAL_NET */
   PM_LAST_THREAD_ID
};


/**
 * Each XSAL application (process) should have a type like this that declares
 * its static thread ids. Use the following as a template and rename it
 * appropriately. Be sure to choose meaningful names for the thread ids that
 * allow the thread to be identified and are unlikely to clash with names
 * from other XSAL applications.
 */
enum XSAL_App1_Id_Tag
{
   XSAL_APP1_NO_THREAD_ID = 0, /* DO NOT MOVE. Must be first in list. */

   XSAL_APP1_LAST_THREAD_ID    /* DO NOT MOVE. Must be last in list. */
};

/*===========================================================================*/
/*!
 * @file xsal_thread_id.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 02-Nov-2011 Kirk Bailey
 *   - Removed circular include of xsal.h.
 *   - Renamed type to prevent confusion and added comments. This will be
 *     the basis for a template file in bb_xsal. 
 *
 * - 06-Oct-2011 Dan Carman
 *   - Create standard header file to define thread IDs 
 */
/*===========================================================================*/
#endif /* XSAL_THREAD_ID_H */
