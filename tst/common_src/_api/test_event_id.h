#ifndef TEST_EVENT_ID_H
#   define TEST_EVENT_ID_H
/*===========================================================================*/
/**
 * @file test_event_id.h
 *
 *    The published events for the XSAL MDF tests.
 *
 * %full_filespec:test_event_id.h~1:incl:kok_basa#12 %
 * @version %version:1 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Tue Apr 14 15:16:56 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *
 * This header file provides the events for the XSAL MDF tests.
 *
 * ABBREVIATIONS:
 *   - XSAL = eXtended System Abstraction Layer
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @addtogroup test_task
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define X-MACROS
\*===========================================================================*/

#   define TEST_PRIVATE_EVENTS\
	SAL_PRIVATE_EVENT(SEND_TO_YL,     "send msg to YL") \

#   define TEST_PUBLIC_EVENTS

#   define TEST_PUBLISHED_EVENTS\
	  SAL_PUBLISHED_EVENT(TEST_EVG_SAL_MESSAGE,            "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_SAL_STOP_ACTIVE_OBJECT, "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_MSG1,                   "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_1,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_2,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_3,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_4,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_5,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_6,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_7,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_8,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_9,                      "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_REC_PASS,               "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_ERROR,                  "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_STOP,                   "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_READY,                  "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_REMOTE_SENDER_READY,    "")\
          SAL_PUBLISHED_EVENT(TEST_EVG_REMOTE_RECEIVER_READY,  "")\
          SAL_PUBLISHED_EVENT(TEST_SEND_EV1,                   "")\
          SAL_PUBLISHED_EVENT(TEST_SEND_EV2,                   "")\
          SAL_PUBLISHED_EVENT(TEST_SEND_EV3,                   "")\
          SAL_PUBLISHED_EVENT(TEST_SEND_EV4,                   "")\
          SAL_PUBLISHED_EVENT(TEST_SEND_EV5,                   "")

/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 21-jul-2008 kirk bailey
 * + Converted event definitions to new X MACRO technique.
 *
 * 14-may-2007 kirk bailey
 * + Created initial file.
 *
\*===========================================================================*/
/** @} doxygen end group */
#endif /* TEST_EVENT_ID_H */
