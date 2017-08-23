#ifndef XSAL_THREAD_PRIORITY_H
#   define XSAL_THREAD_PRIORITY_H
/*===========================================================================*/
/**
 * @file xsal_thread_priority.h
 *
 * XSAL standard defines for thread priorities
 *
 * %full_filespec: xsal_thread_priority.h~kok_basa#7:incl:kok_aud#2 %
 * @version %version: kok_basa#7 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Fri May  6 15:21:33 2016 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007-2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * Description: Definition of the thread priority values for various
 *              platform.
 *
 *              Depending on project a new priority items may be added,
 *              both symbolic name and their values.
 
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * @section ABBR ABBREVIATIONS:
 *    XSAL  - eXtended System Abstraction Layer
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
 *         SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - HI7000/4 Series (HI7000/4 V.2.02, HI7700/4 V.2.02, and HI7750/4 V.2.02) User�s Manual
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/


/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/


#ifndef DISABLE_STANDARD_THREAD_PRIORITIES

/**
 *  Definition of the XSAL thread priority values for various platforms.
 */
typedef enum SAL_Thread_Priority_Tag
{
#if defined (QNX_NTO)

   SAL_CRITICAL_PRIORITY   = 21,
   SAL_SUPERVISOR_PRIORITY = 19,
   SAL_SYSTEM_PRIORITY     = 17,
   SAL_VERY_HIGH_PRIORITY  = 15,
   SAL_ABOVE_HIGH_PRIORITY = 14,
   SAL_HIGH_PRIORITY       = 13,
   SAL_BELOW_HIGH_PRIORITY = 12,
   SAL_VERY_ABOVE_NORMAL_PRIORITY = 12,
   SAL_ABOVE_NORMAL_PRIORITY = 11,
   SAL_NORMAL_PRIORITY     = 10,
   SAL_BELOW_NORMAL_PRIORITY = 9,
   SAL_ABOVE_LOW_PRIORITY  = 9,
   SAL_LOW_PRIORITY        = 8,
   SAL_BELOW_LOW_PRIORITY  = 7,
   SAL_VERY_LOW_PRIORITY   = 6,
  
   /** SAL_INHERITED_PRIORITY name is used internally by SAL.
    *  Do not change this name.
    */
    SAL_INHERITED_PRIORITY = -1

#elif defined (LINUX)

/** 
 * LINUX Priority range from 0  (lowest) to 138 (highest)
 *
 *   0 - 39 are Completely Fair Scheduler with Nice value of 20 to -19
 *  40 to 138 are Real-time Round Robin with priority 1-99
 */ 
   SAL_CRITICAL_PRIORITY      = 105,
   SAL_SUPERVISOR_PRIORITY    = 100,
   SAL_SYSTEM_PRIORITY        = 90,
   SAL_VERY_HIGH_PRIORITY     = 80,
   SAL_ABOVE_HIGH_PRIORITY    = 70,
   SAL_HIGH_PRIORITY          = 60,
   SAL_BELOW_HIGH_PRIORITY    = 50,
   
   SAL_VERY_ABOVE_NORMAL_PRIORITY = 35,
   SAL_ABOVE_NORMAL_PRIORITY  = 30,
   SAL_NORMAL_PRIORITY        = 25,
   SAL_BELOW_NORMAL_PRIORITY  = 20,
   SAL_ABOVE_LOW_PRIORITY     = 15,
   SAL_LOW_PRIORITY           = 10,
   SAL_BELOW_LOW_PRIORITY     = 5,
   SAL_VERY_LOW_PRIORITY      = 0,
  
   /** SAL_INHERITED_PRIORITY name is used internally by SAL.
    *  Do not change this name.
    */
   SAL_INHERITED_PRIORITY = -1

#elif defined (WIN32)


   SAL_CRITICAL_PRIORITY      = THREAD_PRIORITY_HIGHEST,
   SAL_SUPERVISOR_PRIORITY    = THREAD_PRIORITY_HIGHEST,
   SAL_SYSTEM_PRIORITY        = THREAD_PRIORITY_HIGHEST,
   SAL_VERY_HIGH_PRIORITY     = THREAD_PRIORITY_HIGHEST,
   SAL_ABOVE_HIGH_PRIORITY    = THREAD_PRIORITY_ABOVE_NORMAL,
   SAL_HIGH_PRIORITY          = THREAD_PRIORITY_ABOVE_NORMAL,
   SAL_BELOW_HIGH_PRIORITY    = THREAD_PRIORITY_ABOVE_NORMAL,
   SAL_VERY_ABOVE_NORMAL_PRIORITY = THREAD_PRIORITY_NORMAL,
   SAL_ABOVE_NORMAL_PRIORITY  = THREAD_PRIORITY_NORMAL,
   SAL_NORMAL_PRIORITY        = THREAD_PRIORITY_NORMAL,
   SAL_BELOW_NORMAL_PRIORITY  = THREAD_PRIORITY_NORMAL,
   SAL_ABOVE_LOW_PRIORITY     = THREAD_PRIORITY_BELOW_NORMAL,
   SAL_LOW_PRIORITY           = THREAD_PRIORITY_BELOW_NORMAL,
   SAL_BELOW_LOW_PRIORITY     = THREAD_PRIORITY_BELOW_NORMAL,
   SAL_VERY_LOW_PRIORITY      = THREAD_PRIORITY_LOWEST,

   /** SAL_INHERITED_PRIORITY name is used internally by SAL.
    *  Do not change this name.
    */
   SAL_INHERITED_PRIORITY = INT_MIN

#   elif defined (UITRON)

   SAL_CRITICAL_PRIORITY   = SAL_MAX_TASK_PRIORITY,
   SAL_SUPERVISOR_PRIORITY = SAL_MAX_TASK_PRIORITY,
   SAL_SYSTEM_PRIORITY     = SAL_MAX_TASK_PRIORITY,
   SAL_VERY_HIGH_PRIORITY  = SAL_MAX_TASK_PRIORITY,
   SAL_ABOVE_HIGH_PRIORITY = SAL_MAX_TASK_PRIORITY,
   SAL_HIGH_PRIORITY       =\
                  ((SAL_MIN_TASK_PRIORITY + (3 * SAL_MAX_TASK_PRIORITY)) / 4),
   SAL_BELOW_HIGH_PRIORITY =\
                  ((SAL_MIN_TASK_PRIORITY + SAL_MAX_TASK_PRIORITY) / 2),
   SAL_NORMAL_PRIORITY     = SAL_BELOW_HIGH_PRIORITY,
   SAL_ABOVE_LOW_PRIORITY  = SAL_NORMAL_PRIORITY,
   SAL_LOW_PRIORITY        =\
                  (((3 * SAL_MIN_TASK_PRIORITY) + SAL_MAX_TASK_PRIORITY) / 4),
   SAL_BELOW_LOW_PRIORITY  = SAL_MIN_TASK_PRIORITY,
   SAL_VERY_LOW_PRIORITY   = SAL_MIN_TASK_PRIORITY,

#   else
#      error "Platform not supported"
#   endif                       /* QNX_NTO, LINUX, WIN32 */

} SAL_Thread_Priority_T;


#endif /* DISABLE_STANDARD_THREAD_PRIORITIES */

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/


/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 01-Sep-2006 JR-TCK	Initial revision.
 *
 * 04-Jul-2007 VNC-BSD
 *    Add CRITICAL, SUPERVISOR, SYSTEM, ABOVE_HIGH, BELOW_HIGH, ABOVE_LOW,
 *    BELOW_LOW priorites
 *
 * 26-mar-2008 kirk bailey
 *    Merged uItron definitions with latest BSD release.
 *
 * 27-May-2015 Kirk Bailey
 *    Filled in unresolved comments.
 *
 * 6-May-2016 Dan Carman
 *     kok_basa#247254 - Updated priorities for QNX to better match names
\*===========================================================================*/
/** @} doxygen end group */
#endif /* XSAL_THREAD_PRIORITY_H */
