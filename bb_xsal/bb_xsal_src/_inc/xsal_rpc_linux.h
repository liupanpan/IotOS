/* -------------------------------------------------------------------------
 *
 * File:        xsal_rpc_linux_c.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Dariusz Kubiczek (dariusz.kubiczek@delphi.com)
 *
 * Description: Implementation of the synchronous communication on
 *              Linux platform
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_RPC_LINUX_H
#define XSAL_RPC_LINUX_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal_data_types.h"

typedef enum Message_Tag
{
   Pulse,
   Message
} Message_T;


typedef struct Message_Send_Header_Tag
{
   Message_T message_type;
   size_t    message_size;
} Message_Send_Header_T;


typedef struct Message_Reply_Header_Tag
{
   size_t  reply_size;
   int32_t reply_status;
} Message_Reply_Header_T;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_RPC_LINUX_H */

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
