/*--------------------------------------------------------------------------
 * File:           pm_queue.c
 *
 * Copyright:      2004 Delphi Technologies, Inc., All Rights Reserved
 *                 Information Contained Herein Is Proprietary and Confidential
 *
 * Author          Dominik Schaefer
 *
 * Description:    FIFO queue implementation.
 *
 *--------------------------------------------------------------------------
 */

#include "pm_queue.h"
#include "xsal_application_id.h"
#include "pbc_trace.h"

EM_FILENUM(PROCMAN_MODULE_ID, 4);

static int pm_queue[SAL_MAX_APPS];

void PM_Queue_Init()
{
   int i;
   for (i = 0; i < SAL_MAX_APPS; i++)
   {
      pm_queue[i] = PM_QUEUE_EMPTY_ELEMENT;
   }
}

int PM_Queue_Pop()
{
   int i, ret;
   ret = pm_queue[0];
   for (i = 0; i < SAL_MAX_APPS - 1; i++)
   {
      if (pm_queue[i + 1] == PM_QUEUE_EMPTY_ELEMENT)
      {
         pm_queue[i] = PM_QUEUE_EMPTY_ELEMENT;
         break;
      }
      pm_queue[i] = pm_queue[i + 1];
   }
   return ret;
}

void PM_Queue_Push(int element)
{
   int i;
   PBC_Require_1(( element >= 0 ) && ( element < SAL_MAX_APPS ), "element out of range: %d", (int)element);
   for (i = 0; i < SAL_MAX_APPS; i++)
   {
      if (pm_queue[i] == element)
      {
         break;
      }
      if (pm_queue[i] == PM_QUEUE_EMPTY_ELEMENT)
      {
         pm_queue[i] = element;
         break;
      }
   }

}

void PM_Queue_Clear()
{
   PM_Queue_Init();
}

/*-----------------------------------------------------------------------------
 * History
 *
 * 20 May. 2009, Dominik Schaefer:  Initial revision.
 *-----------------------------------------------------------------------------*/

