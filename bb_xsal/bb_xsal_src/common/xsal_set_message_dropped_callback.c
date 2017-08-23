/* -------------------------------------------------------------------------
 *
 * File:        xsal_set_message_dropped_callback.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Set_Message_Dropped_Callback()
 *              function.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_message_queue.h"


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


SAL_Message_Dropped_Callback_T SAL_Set_Message_Dropped_Callback(
   SAL_Message_Dropped_Callback_T fnc)
{
   SAL_Message_Dropped_Callback_T prev_callback = SAL_I_Message_Dropped_Callback;
   SAL_I_Message_Dropped_Callback = fnc;

   return prev_callback;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 21 May 2007, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
