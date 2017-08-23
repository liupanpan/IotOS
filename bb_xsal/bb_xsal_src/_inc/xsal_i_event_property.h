/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_event_property.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the internal interface of the SAL Send.
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_EVENT_PROPERTY_H
#define XSAL_I_EVENT_PROPERTY_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal.h"

/** Pointer to table with event properties (NORMAL,URGENT, SUBSCRIBED, SELECTED, ...).
 *  Logically it is a two dimensinal table, where each thread has its own
 *  column and rows defines event properties in given thread:
 *
 *    SAL_I_Event_Property[MAX_NUMBER_OF_THREADS][SAL_MAX_EVENTS];
 *
 *  Number of events is known during compilation time, while number
 *  of threads is defined calling SAL_Init() function.
 *
 *  Access to table is synchronized by SAL_I_Event_Property_Mutex
 */
extern uint8_t (*SAL_I_Event_Property)[SAL_MAX_EVENTS];


/** The following bitmask maps propertirs to single bits.
 */
#define SAL_I_Subscribe_Bit_Mask  0x01
#define SAL_I_Urgent_Bit_Mask     0x02
#define SAL_I_Select_Bit_Mask     0x04


/** Mutex used to synchronize access to the SAL_I_Event_Property table.
 */
extern SAL_Mutex_T SAL_I_Event_Property_Mutex;


/** Initialize Event Property module
 */
bool_t SAL_I_Init_Event_Property_Module(void);


/** De-initialize Event Property module
 */
void SAL_I_Deinit_Event_Property_Module(void);


/** Returns pointer to the thread properties events for given thread.
 *
 *  @param [in] tid id of examined thread
 *
 *  @return pointer to thread properites
 */
#define SAL_I_Thread_Event_Property(tid) (SAL_I_Event_Property[(tid)-1])


/** Function returns true if given thread is subscribed for given event.
 *
 *  @param [in] tid id of examined thread
 *  @param [in] ev_id id of examined event
 *
 *  @return true when thread is subscribed for event.
 */
#define SAL_I_Is_Subscribed(tid, ev_id) ((SAL_I_Event_Property[(tid)-1][(ev_id)] & SAL_I_Subscribe_Bit_Mask) != 0)


/** Function returns true if given thread has declared given even as urgent.
 *
 *  @param [in] tid id of examined thread
 *  @param [in] ev_id id of examined event
 *
 *  @return true when event is urgent for thread
 */
#define SAL_I_Is_Urgent(tid, ev_id) ((SAL_I_Event_Property[(tid)-1][(ev_id)] & SAL_I_Urgent_Bit_Mask) != 0)


bool_t SAL_I_Init_Event_Property_Module(void);
void SAL_I_Deinit_Event_Property_Module(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_EVENT_PROPERTY_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
