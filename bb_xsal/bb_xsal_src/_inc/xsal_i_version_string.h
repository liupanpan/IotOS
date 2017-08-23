/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_version_string.h
 *
 * Copyright:   2006,12 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: String with XSAL lib version.
 *
 * %full_filespecdes: %
 * @version %version: kok_basa#33 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Fri Dec 18 13:35:36 2015 %
 */
/* -------------------------------------------------------------------------*/

#ifndef XSAL_I_VERSION_STRING_H
#define XSAL_I_VERSION_STRING_H

/**
 *  String XSAL_VERSION_STRING defines version of the XSAL lib.
 *  It has the following format:
 *
 *      XSAL XXX YWW Year-Month-Day 
 *
 *  where:
 *
 *   XSAL            lib name
 *   XXX             version number (incremented by 1 on every release)
 *   Y               last digit of the current year
 *   WW              current week number
 *   Year-Month-Day  current date
 *   
 */

#define XSAL_VERSION_STRING "XSAL 2015-12-18"

#endif /* XSAL_I_VERSION_STRING_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * 16-Mar-2015 Dan Carman
 * + Task 21696 Updated version for 64  bit Linux support
 *
 * -----------------------------------------------------------------------*/
