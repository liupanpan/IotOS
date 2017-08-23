#ifndef XSAL_EVENT_ID_H
#   define XSAL_EVENT_ID_H
/*===========================================================================*/
/**
 * @file xsal_event_id.h
 *
 * System wide events IDs.
 *
 * %full_filespec:xsal_event_id.h~kok_basa#17:incl:kok_aud#24 %
 * @version %version:kok_basa#17 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Sat May  2 09:25:39 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *   All system event IDs. This file defines all event IDs for the XSAL MDF.
 *
 * @section ABBR ABBREVIATIONS:
 *   - XSAL = eXtensible System Abstraction Layer.
 *
 * @section TINFO TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *     BASA_SRS_Error_Management_0 4.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup xsal_events_grp Public event IDs
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "pm_event_id.h"
#   include "test_event_id.h"

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/**
 * System events that are private to the modules that define them, which means
 * they are intended only for local use by that module. These events are
 * "exposed" only to assign them unique values within the SAL_Event_Id_T enum.
 */
#   define  SAL_PRIVATE_EVENTS  \
            PM_PRIVATE_EVENTS   \
            TEST_PRIVATE_EVENTS \
/**
 * System events that are public, which means the event ids can be used by
 * modules other than the one declaring them.
 *
 * @note "Public" events cannot be subscribed to
 * @see SAL_Send()
 */
#   define  SAL_PUBLIC_EVENTS  \
            PM_PUBLIC_EVENTS   \
            TEST_PUBLIC_EVENTS \
/**
 * System events that are available for use with the XSAL Publish/Subscribe
 * feature.
 *
 * @see SAL_Publish(), SAL_Subscribe()
 */
#   define  SAL_PUBLISHED_EVENTS  \
            PM_PUBLISHED_EVENTS   \
            TEST_PUBLISHED_EVENTS \

/* Expand event declarations to be the id tag followed by a comma. */
#  undef  SAL_EVENT
#  define SAL_EVENT(id,descr) id,
#  define SAL_PRIVATE_EVENT(id,descr)   SAL_EVENT(id, descr)
#  define SAL_PUBLIC_EVENT(id,descr)    SAL_EVENT(id, descr)
#  define SAL_PUBLISHED_EVENT(id,descr) SAL_EVENT(id, descr)

/**
 * System wide event identifiers.
 *
 * All published event identifiers need to be listed in this typedef between
 * SAL_EVG_FIRST_PUBLISHED_EVENT_ID and SAL_EVG_LAST_PUBLISHED_EVENT_ID.
 *
 * Public events are those that are shared between modules but are not
 * available for use with SAL_Publish() and SAL_Subscribe(). These are assigned
 * values between SAL_EVG_LAST_PUBLISHED_EVENT_ID and SAL_EV_FIRST_PRIVATE_ID.
 *
 * Tasks can use private event IDs starting at SAL_EV_FIRST_PRIVATE_ID. Private
 * events include timer IDs, internal message sent via SAL_Send(), etc. 
 */
enum SAL_Event_Id_Tag
{
   /** RESERVED for XSAL, must be first; do not change! */
   SAL_FIRST_EVENT_ID = 0,
   SAL_EVG_FIRST_PUBLISHED_EVENT_ID = SAL_FIRST_EVENT_ID,

   SAL_PUBLISHED_EVENTS

   /** RESERVED for XSAL, must be last published event id; do not change! */
   SAL_EVG_LAST_PUBLISHED_EVENT_ID,

   SAL_PUBLIC_EVENTS

   /** First private event identifier value. */
   SAL_EV_FIRST_PRIVATE_ID,

   SAL_PRIVATE_EVENTS

   SAL_LAST_EVENT_ID /* Must be last - do not move! */
};

/** 
 * Needed by XSAL to allocate space events. Do not change!
 */
#   define SAL_MAX_EVENTS (SAL_LAST_EVENT_ID-SAL_FIRST_EVENT_ID)

/*===========================================================================*/
/*!
 * @file xsal_event_id.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 28-dec-2010 kirk bailey
 *    - Changed SAL_MAX_EVENTS to count all events, not just published ones.
 *
 * - 14-sep-2009 kirk bailey
 *    - Removed flags for deprecated request/receive logic.
 *
 * - 21-jul-2008 kirk bailey
 *    - Refactored event defn. to use X MACROs.
 *
 * - 27-Sept-2007 Dan Carman
 *    - MISRA changes - Added parentheses around macro parameters
 *
 * -14-may-2007 kirk bailey
 *    - Fixed SAL_Form_Data_Request() MACRO.
 *    - Created file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif /* XSAL_EVENT_ID_H */
