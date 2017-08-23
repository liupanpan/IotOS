/* -------------------------------------------------------------------------
 *
 * File:        xsal_get_max_number_of_threads.c 
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL_Get_Max_Number_Of_Threads()
 *              function.
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_config.h"


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */


uint32_t SAL_Get_Max_Number_Of_Threads(void)
{
   return SAL_I_Max_Number_Of_Threads;
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 *
 * -----------------------------------------------------------------------*/
