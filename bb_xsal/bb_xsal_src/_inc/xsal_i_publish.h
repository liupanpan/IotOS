/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_publish.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of internal interface of SAL Publish.
 * 
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_PUBLISH_H
#define XSAL_I_PUBLISH_H

#include "xsal.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * This structure is used to maintain message count statistics for an XSAL
 * application.
 */
typedef struct
{
   /**
    * Total number of messages sent between threads of this XSAL app.
    */
   uint32_t total_num_sent_internally;
#if defined (XSAL)
   /**
    * Total number of messages sent from this XSAL app to other XSAL apps.
    * This is the sum of the entries in the 'num_sent_to_app' array.
    */
   uint32_t total_num_sent_externally;
   /**
    * Total number of messages sent to each of the other XSAL apps.
    */
   uint32_t num_sent_to_app[SAL_MAX_APPS + 1];
#endif
} SAL_App_Msg_Count_Stats_T;

#if defined (XSAL)

/** 
 *  Global Routing Table
 */
extern uint8_t (*SAL_I_GRT)[SAL_MAX_EVENTS][SAL_MAX_APPS + 1];

#endif /* XSAL */

/**
 *  Internal functions
 */

bool_t SAL_I_Init_Publish_Module(void);
void SAL_I_Deinit_Publish_Module(void);
/**
 * Fills in the provided structure with the current message count stats for
 * this XSAL application.
 *
 * @param [out] stats Pointer to structure to be filled in.
 * @param [in]  reset_counts If true, reset the message counts after reading them.
 */
void SAL_I_Get_App_Msg_Count_Stats(SAL_App_Msg_Count_Stats_T *stats, bool_t reset_counts);
/**
 * Increments the number of messages sent to the specified XSAL app. from this
 * XSAL app and also increments the total number of messages sent to all other
 * XSAL apps by this one.
 *
 * @param [in] app_id The XSAL application to which the message went.
 */
void SAL_I_Inc_Ext_Msg_Count(SAL_App_Id_T app_id);
/**
 * Increments the number of messages sent between threads of this XSAL app.
 */
void SAL_I_Inc_Int_Msg_Count(void);
void SAL_I_Local_Publish(const SAL_Message_T* message);
void SAL_I_Unsubscribe_Thread_Events(SAL_Thread_Id_T thread_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_PUBLISH_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 20May14  David Origer
 * - Task kok_inf#7893 : Fix Klocwork issues.
 *   - Remove changes for "no reply" messaging made under task
 *     kok_basa#115827.
 *
 * -----------------------------------------------------------------------*/
