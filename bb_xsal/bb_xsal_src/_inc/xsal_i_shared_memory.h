/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_shared_memory.h
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Specification of the internal XSAL shared memory
 *              interface.
 *
 * -----------------------------------------------------------------------*/

#ifndef XSAL_I_SHARED_MEMORY_H
#define XSAL_I_SHARED_MEMORY_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "xsal.h"

void SAL_I_Create_Shared_Memory_Path(char_t* file_path, size_t path_sz,
                                     SAL_Shared_Memory_Key_T key,
                                     SAL_Shared_Memory_Key_T subkey);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XSAL_I_SHARED_MEMORY_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
