/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_receive.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the internal receive message functions.
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_RECEIVE_H
#define XSAL_I_RECEIVE_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal.h"
#include "xsal_i_thread.h"


const SAL_Message_T* SAL_I_Receive_From_Timeout(
   SAL_I_Thread_Attr_T* queue_owner_thread,
   uint32_t timeout_ms);


SAL_Queue_Iterator_T SAL_I_Search_Message_Queue(
   const SAL_Message_Queue_T* queue, 
   const SAL_Event_Id_T event_list[],
   size_t number_of_events);


#if defined(XSAL)

const SAL_Message_T* SAL_I_Receive_From(SAL_I_Thread_Attr_T* queue_owner_thread);

#endif /* XSAL */

const SAL_Message_T* SAL_I_Try_Receive_From(SAL_I_Thread_Attr_T* queue_owner_thread);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_RECEIVE_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 14-Apr-2015 Dan Carman 
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the enumerated type of SAL_Event_Id_T for compatibility with ISA code. 
 *
 * -----------------------------------------------------------------------*/
