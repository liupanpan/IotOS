/*--------------------------------------------------------------------------
 * File:           pm_parser.h
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Lukasz Ptak
 *
 * Description:    Definition of the Process Manager Parser.
 *
 *--------------------------------------------------------------------------
 */

#   include "pm_process.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/** 
 * Parses a single line of the process configuration file.
 *
 * @param line_str     pointer to line to be parsed
 * @param line_number  configuration file line number
 * @param line_status  when set to 0, it indicates that configuration file line could
 *                     not be parsed and Process Manager should not be started.
 *
 * @return pointer to PM_Process_T data structure.
 */
PM_Process_T *PM_Parse_Line(const char *line_str, int line_number, int *line_status);

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */

/*-----------------------------------------------------------------------------
 * History
 *
 * 22 Sep. 2006, Lukasz Ptak:     Initial revision.
 *-----------------------------------------------------------------------------*/
