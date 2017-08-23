#ifndef XSAL_APPLICATION_ID_H
#   define XSAL_APPLICATION_ID_H
/*===========================================================================*/
/**
 * @file xsal_application_id.h
 *
 * This file defines the system wide application ids. Its contents are
 * unique to a given system.
 *
 * %full_filespec:xsal_application_id.h~kok_basa#12:incl:kok_aud#2 %
 * @version %version:kok_basa#12 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Fri Apr 17 14:55:26 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @ingroup xsal_data_types_grp
 * @{
 */
/*==========================================================================*/

/**
 * Uses X MACROs to define the enumeration tags and names for the system's processes.
 */
#   define SAL_APPLICATION_IDS\
    X(APP_PROCMAN,       "procman"      ) /*  1 */\
    X(APP_SYSTEM,        "Main_software") /*  2 */\
    X(APP_COUNT_SERVICE, "count_service") /*  3 */\
    X(APP_COUNT_CLIENT,  "count_client" ) /*  4 */\
    X(APP_COUNT_CLIENT2, "count_client2") /*  5 */\
    X(APP_COUNT_CLIENT3, "count_client3") /*  6 */\
    X(APP_COUNT_CLIENT4, "count_client4") /*  7 */\
    X(APP_COUNT_CLIENT5, "count_client5") /*  8 */\
    X(APP_COUNT_CLIENT6, "count_client6") /*  9 */\
    X(APP_COUNT_CLIENT7, "count_client7") /* 10 */\
    X(APP_COUNT_CLIENT8, "count_client8") /* 11 */\
    X(APP_COUNT_CLIENT9, "count_client9") /* 12 */\
    X(XENA_PROCESS_ID  , "XENA_Gateway" ) /* 13 */\

#   undef X
#   define X(a,b) a, /* Create list of enumeration tags */
/**
 * Each application (process) that needs to use XSAL messaging must have
 * a unique "application id". This "app id" is part of the "addressing
 * mechanism" for sending messages between applications. The list of
 * app ids is unique to a system. This enumeration assigns values to the
 * application ids.
 *
 * @note Application IDs must be greater than 0.
 */
typedef enum
{
   SAL_NO_APP_ID = 0,

   SAL_APPLICATION_IDS

   SAL_LAST_APP_ID
}
SAL_Application_Id_T;

/**
 * Map Logical Application IDs to specific process 
 */

#   undef X

/**
 * Application ID associated with first application.
 */
#   define SAL_FIRST_APP_ID 1

/**
 * Used by XSAL to size arrays based on number of applications.
 */
#   define SAL_MAX_APPS (SAL_LAST_APP_ID-SAL_FIRST_APP_ID)

/*===========================================================================*/
/*!
 * @file xsal_application_id.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 04-mar-2008 Kirk Bailey
 *   - Removed DB48x-specific defintions.
 *   - Implemented X MACRO technique for assigning application ids and names.
 *
 * - 22-jul-2008 Kirk Bailey
 *   - Changed tags to follow naming convention by using SAL_ prefix.
 *
 * - 21-apr-2010 Kirk Bailey
 *   - Removed extern C block.
 *
 * - 31-Aug-2014 Kirk Bailey
 *   - Fixed comment.
 *
 *===========================================================================*/
/** @} doxygen end group */
#endif /* XSAL_APPLICATION_ID_H */
