/* -------------------------------------------------------------------------
 *
 * File:        xsal_get_app_id.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Get_App_Id() function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_config.h"


/* -----------------------------
 *  Public function definition
 * -----------------------------*/


SAL_App_Id_T SAL_Get_App_Id(void)
{
   return SAL_I_App_Id;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
