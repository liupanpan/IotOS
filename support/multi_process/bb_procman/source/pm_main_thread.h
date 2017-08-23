/*--------------------------------------------------------------------------
 * File:           pm_main_thread.h
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Michal Gebolys / Lukasz Ptak
 *
 * Description:    Definition of the Process Manager Main Thread. 
 *
 *--------------------------------------------------------------------------
 */
#ifndef PM_MAIN_THREAD_H
#define PM_MAIN_THREAD_H

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/**
 * Implementation of Process Manager Main thread
 *
 * @param param pointer to object passed to Process Manager Main thread
 */
void PM_Main_Thread(void *param);

/** 
 * Opens the Process Manager configuration file.     
 *
 * @param file_name name of the config file
 * @return 1 if file exists and contains valid entries,
 *         0 otherwise
 */
int PM_Parse_Config_File(const char *file_name);

/**
 * Requests to exit thread loop 
 */
void PM_Stop_Main_Thread(void);

/** 
 * Sets new configuration     
 *
 * @param config new configuration value
 */
void PM_Set_Configuration(int config);

/** 
 * Prints info message with time measured from system startup.     
 *
 * @param info_msg info message text
 * @param config_num current configuration
 */
void PM_Print_Performance_Info(const char *info_msg, int config_num);

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */

#endif /* PM_MAIN_THREAD_H */

/*-----------------------------------------------------------------------------
 * History
 *
 * 25 Sep. 2006, Lukasz Ptak:     Port to C.
 * 27 Aug. 2004, Michal Gebolys:  Initial revision.
 *-----------------------------------------------------------------------------*/
