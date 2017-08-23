/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_select.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the XSAL selective receive.
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_SELECT_H
#define XSAL_I_SELECT_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_i_message_queue.h"


SAL_Queue_Iterator_T SAL_I_Search_Selected_Message(
   SAL_Message_Queue_T* queue,
   const uint8_t* event_property);

void SAL_I_Unselect(uint8_t* event_property);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_SELECT_H */


/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 15 Feb 2008, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
