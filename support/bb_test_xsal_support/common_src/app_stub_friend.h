#ifndef APP_STUB_FRIEND_H
#   define APP_STUB_FRIEND_H
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */
/*===========================================================================*/
/**
 * @file app_stub_friend.h
 *
 * External declaration for message log 
 *
 * %full_filespec:app_stub_friend.h~2:incl:kok_aud#1 %
 * @version %version:2 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Jul 15 17:35:12 2009 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 * Debug access to recent messages
 *
 * ABBREVIATIONS:
 *   - None
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "reuse.h"
#include "xsal.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/
/**
 * Debug buffer to hold the last MESSAGE_LOG_SIZE message sent 
 * Read only public access 
 */
extern SAL_Message_T Msg_Log_Buffer[];

/** Index for next message to go into log buffer 
 *  Read only public access 
 */
extern uint32_t Msg_Log_Index;

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

/*===========================================================================*/
/*!
 * @file app_stub_frient.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * 16-Aug-2007 Dan Carman
 * + Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#   ifdef __cplusplus
}                               /* extern "C" */
#   endif                       /* __cplusplus */
#endif /* APP_STUB_FRIEND_H */
