/* -------------------------------------------------------------------------
 *
 * File:        pm_os_native_thread.h
 *
 * Copyright:   2013 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman(at)delphi.com)
 *
 * Description: Implementation of the Create_OS_Native_Thread() function.
 *
 * -----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

bool Create_OS_Native_Thread(void (*function)(void*), int priority, void* param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 03 December 2013, Jacek Roman (jacek.roman(at)delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
