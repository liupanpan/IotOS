/*===========================================================================*/
/**
 * @file xsal_wait_cond_linux.c
 *
 * Handles wait conditional for Linux
 *
 * %full_filespec:xsal_wait_cond_linux.c~4:csrc:kok_basa#1 %
 * @version %version:4 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Jan  4 07:29:18 2012 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Handles wait_condes for Linux.  
 *   Linux pthread wait_cond do not guarantee fairness 
 *    Releasing wait_cond does not grant immediate to equal priority thread waiting. This 
 *    can lead to starvation of one of the threads.
 *   The   
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
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
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "pbc_trace.h"
#include "reuse.h"
#include "xsal.h"
#include "xsal_i_time.h"
#include <errno.h>
#include <string.h>

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/
#undef EM_F_ID
#define EM_F_ID EM_FILE_ID(XSAL_CONDVAR_MODULE_ID, 1) /**< Identifies file for PbC/trace */

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/*
 * Wait Condition
 */
bool_t SAL_Wait_Cond(SAL_Cond_T* cond, SAL_Mutex_T* mutex)
{
   SAL_Int_T status;

   PBC_Require(cond != NULL, "NULL condvar pointer");
   PBC_Require(mutex != NULL, "NULL mutex pointer");

   status = pthread_cond_wait(cond, &(mutex->mutex));

   PBC_Ensure_1((EOK == status), "SAL_Wait_Cond error: %s", strerror(errno));

   return (bool_t) (EOK == status);
}

/*
 * Wait Condition with timeout
 */
bool_t SAL_Wait_Cond_Timeout(SAL_Cond_T* cond, SAL_Mutex_T* mutex, uint32_t timeout_msec)
{
   SAL_Int_T status;
   SAL_I_Time_Spec_T timeout;

   PBC_Require(cond != NULL, "NULL condvar pointer");
   PBC_Require(mutex != NULL, "NULL mutex pointer");

   SAL_I_Get_Time(&timeout);
   SAL_I_Time_Add_Offset(&timeout, timeout_msec);

   status = pthread_cond_timedwait(cond, &(mutex->mutex), &timeout);

   /** Do not print debug when function returns timeout value - this 
    *  isn't invalid behavior.
    */
   PBC_Ensure_1((EOK == status) || (ETIMEDOUT == status),
      "SAL_Wait_Cond_Timeout error: %s", strerror(status));

   return (bool_t) (EOK == status);
}

/*===========================================================================*/
/*!
 * @file xsal_wait_cond_linux.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 20-Jan-2012 Dan Carman
 *   - Created initial file.
 */
/*===========================================================================*/
