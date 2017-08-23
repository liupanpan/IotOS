/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_engine.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of the SAL engine (Router Thread).
 *
 * -------------------------------------------------------------------------*/

#include "xsal.h"
#include "xsal_i_engine.h"

#if defined (XSAL)

#include "xsal_i_config.h"
#include "xsal_i_connections.h"

#else /* XSAL_LIGHT */

#include "xsal_i_timer.h"

#endif /* XSAL && XSAL_LIGHT*/

#include <stdlib.h>

/* -----------------------
 *  Function Definitions
 * -----------------------*/


void SAL_I_Stop_Router_Thread(int32_t status)
{
#if defined (XSAL)

   /** Send a pulse to the Router Thread of calling application with 
    *  SAL_PULSE_RT_EXIT command and exit_value.
    *  Pulse code is set to 0 (ID of RouterThread) while less significant 
    *  byte of pulse value contains a command and other bytes contains 
    *  retcode value.
    */

   (void)SAL_Send_Pulse(
      SAL_Get_Connection(SAL_I_App_Id),
      SAL_ROUTER_THREAD_ID,
      SAL_PULSE_RT_EXIT | (status << 8));

#else /* XSAL_LIGHT */

   SAL_I_Stop_RT_Light(status);

#endif /* XSAL && XSAL_LIGHT */
}


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 07 Aug 2012 Dan Carman
 *     Improved error handling / connection on Pulses
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
