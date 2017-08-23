/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_engine.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Declaration of the SAL engine (Router Thread).
 *
 *------------------------------------------------------------------------*/

#ifndef XSAL_I_ENGINE_H
#define XSAL_I_ENGINE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"


/* ----------------------------
 *  Exported Type Declarations
 * ---------------------------- */


#if defined (XSAL) || defined (QNX_NTO) || defined(LINUX)

/** Internal event Ids
 */
typedef enum SAL_Pulse_Code_Tag
{
   /** Terminates Router Thread
    */
   SAL_PULSE_RT_EXIT = 1
} SAL_Pulse_Code_T; 


#endif /* XSAL */


/* ------------------------------
 *  Exported Object Declarations
 * ------------------------------ */


#if defined (XSAL)

/** Channel the SAL Message Loop is waiting for 
 *  message from remote applications.
 */
extern SAL_Channel_T SAL_I_RT_Channel;

#endif /* XSAL */


/* ------------------------------
 *  Exported Function Prototypes
 * ------------------------------ */


/** Function called just before the SAL Message Loop.
 */
bool_t SAL_I_Before_Run(void);


/** XSAL Message Loop function
 */
int32_t SAL_I_Run(void);


/** Function called just after the SAL Message Loop.
 */
void SAL_I_After_Run(void);


/** Function signalizes SAL Message Loop to 
 *  terminates and returns given status.
 */
void SAL_I_Stop_Router_Thread(int32_t status);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_ENGINE_H */


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
