/*===========================================================================*/
/**
 * @file double_linked_list.c
 *
 * Implements the double-linked-list utility functions.
 *
 * %full_filespec: double_linked_list.c~kok_basa#9:csrc:kok_aud#1 %
 * @version %version: kok_basa#9 %
 * @author %derived_by: jz4fpf %
 * @date %date_modified: Wed Jan  5 11:03:21 2011 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - dbll = Double Linked List
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None.
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "double_linked_list.h"
#include "pbc_trace.h"
#include <string.h>

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 2); /**< IDs file for EM assert handling */

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

static bool_t Do_Ordered_Insert(void *arg, DBLL_List_T * pLL, DBLL_Entry_T * pEntry);

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/


/*===========================================================================*
 * FUNCTION: DBLL_Add_Entry
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void DBLL_Add_Entry(DBLL_List_T * pLL, DBLL_Entry_T * pEntry)
{
   PBC_Require(pLL != NULL, "DBLL_Add_Entry cannot be called with a NULL pLL pointer");
   PBC_Require(pEntry != NULL, "DBLL_Add_Entry cannot be called with a NULL pEntry pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Add_Entry cannot be called with a NULL first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Add_Entry cannot be called with a NULL last or next pointer");

   if ((NULL == pLL->first) || (NULL == pLL->callbacks) || (NULL == pLL->callbacks->compare))
   {
      /* insert at the end of the list */
      DBLL_Insert_After(pLL, pEntry, NULL);
   }
   else
   {
      DBLL_Visit_Each(pLL, Do_Ordered_Insert, pEntry);
   }
}

/*===========================================================================*
 * FUNCTION: DBLL_Destroy
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void DBLL_Destroy(DBLL_List_T * pLL)
{
   PBC_Require(pLL != NULL, "DBLL_Destroy cannot be called with a NULL pLL pointer");

   if ((pLL->callbacks != NULL) && (pLL->callbacks->free != NULL))
   {
      DBLL_Visit_Each(pLL, pLL->callbacks->free, pLL->callbacks->arg);
   }
   memset(pLL, 0, sizeof(*pLL));
}

/*===========================================================================*
 * FUNCTION: DBLL_Find_Next
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void * DBLL_Find_Next(DBLL_Entry_T * pEntry, DBLL_Match_Callback_T pFunc, void *arg)
{
   PBC_Require(pFunc != NULL, "DBLL_Find_Next cannot be called with a NULL pFunc pointer");

   /* Terminated the search once a match is found. */
   while ((pEntry != NULL) && !pFunc(arg, pEntry))
   {
      pEntry = pEntry->next;
   }
   
   return pEntry;
}

/*===========================================================================*
 * FUNCTION: DBLL_Find
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void * DBLL_Find(DBLL_List_T * pLL, DBLL_Match_Callback_T pFunc, void *arg)
{
   PBC_Require(pLL != NULL, "DBLL_Find cannot be called with a NULL pLL pointer");
   PBC_Require(pFunc != NULL, "DBLL_Find cannot be called with a NULL pFunc pointer");

   return DBLL_Find_Next(pLL->first, pFunc, arg);
}

/*===========================================================================*
 * FUNCTION: DBLL_Get_Count
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
uint16_t DBLL_Get_Count(DBLL_List_T const *pLL)
{
   uint16_t count;

   PBC_Require(pLL != NULL, "DBLL_Get_Count cannot be called with a NULL pLL pointer");

   count = pLL->count;

   return count;
}

/*===========================================================================*
 * FUNCTION: DBLL_Get_First
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void *DBLL_Get_First(DBLL_List_T const *pLL)
{
   PBC_Require(pLL != NULL, "DBLL_Get_First cannot be called with a NULL pLL pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Get_First cannot be called with a NULL first and prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Get_First cannot be called with a NULL last and next pointer");

   return pLL->first;
}

/*===========================================================================*
 * FUNCTION: DBLL_Get_Last
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void *DBLL_Get_Last(DBLL_List_T const *pLL)
{
   PBC_Require(pLL != NULL, "DBLL_Get_Last cannot be called with a NULL pLL pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Get_Last cannot be called with a NULL first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Get_Last cannot be called with a NULL last or next pointer");

   return pLL->last;
}

/*===========================================================================*
 * FUNCTION: DBLL_Get_Next
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void *DBLL_Get_Next(DBLL_Entry_T const *pEntry)
{
   PBC_Require(pEntry != NULL, "DBLL_Get_Next cannot be called with a NULL pEntry pointer");

   return pEntry->next;
}

/*===========================================================================*
 * FUNCTION: DBLL_Get_Prev
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void *DBLL_Get_Prev(DBLL_Entry_T const *pEntry)
{
   PBC_Require(pEntry != NULL, "DBLL_Get_Prev cannot be called with a NULL pEntry pointer");

   return pEntry->prev;
}

/*===========================================================================*
 * FUNCTION: DBLL_Init
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void DBLL_Init(DBLL_List_T * pLL, DBLL_Callback_T const *callbacks, uint16_t initialCount)
{
   uint16_t i;

   PBC_Require(pLL != NULL, "DBLL_Init cannot be called with a NULL pLL pointer");
   PBC_Require((0 == initialCount) || ((callbacks != NULL) && (callbacks->calloc != NULL)), "DBLL_Init cannot be called with invalid parameters");

   memset(pLL, 0, sizeof(*pLL));
   pLL->callbacks = callbacks;

   for (i = 0; i < initialCount; i++)
   {
      DBLL_Add_Entry(pLL, callbacks->calloc(callbacks->arg));
   }
}

/*===========================================================================*
 * FUNCTION: DBLL_Insert_After
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void DBLL_Insert_After(DBLL_List_T * pLL, DBLL_Entry_T * pEntry, DBLL_Entry_T * pNeighbor)
{
   bool_fast only_one = false;

   PBC_Require(pLL != NULL, "DBLL_Insert_After cannot be called with a NULL pLL pointer");
   PBC_Require(pEntry != NULL, "DBLL_Insert_After cannot be called with a NULL pEntry pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev),"DBLL_Insert_After cannot be called with a NULL first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Insert_After cannot be called with a NULL last or next pointer");

   if (NULL == pNeighbor)       /* place entry at end of list */
   {
      pNeighbor = pLL->last;

      if (NULL == pNeighbor)    /* list is currently empty! */
      {
         pLL->first = pLL->last = pEntry;
         pLL->count++;
         pEntry->next = pEntry->prev = NULL;
         PBC_Internal(NULL == pLL->first->prev, "DBLL_Insert_After cannot have a null prev pointer");
         PBC_Internal(NULL == pLL->last->next, "DBLL_Insert_After cannot have a null next pointer");

         only_one = true;       /* all done (bypass extra logic below) */
      }
   }

   if (!only_one)
   {
      DBLL_Entry_T *oldNext;

      oldNext = pNeighbor->next;
      pNeighbor->next = pEntry;
      pEntry->prev = pNeighbor;
      pEntry->next = oldNext;

      if (oldNext != NULL)      /* not last in list */
      {
         /* if internal checks are enabled, double check the ordering */
         PBC_Internal((NULL == pLL->callbacks) ||
                             (NULL == pLL->callbacks->compare) ||
                             (pLL->callbacks->compare(pLL->callbacks->arg, pEntry, oldNext) <= 0), "DBLL_Insert_After order is corrupt");
         PBC_Internal((NULL == pLL->callbacks) ||
                             (NULL == pLL->callbacks->compare) ||
                             (pLL->callbacks->compare(pLL->callbacks->arg, pEntry, pNeighbor) >= 0), "DBLL_Insert_After order is corrupt");
         oldNext->prev = pEntry;
      }
      else                      /* is the last in the list */
      {
         /* if internal checks are enabled, double check the ordering */
         PBC_Internal((NULL == pLL->callbacks) ||
                             (NULL == pLL->callbacks->compare) ||
                             (pLL->callbacks->compare(pLL->callbacks->arg, pEntry, pNeighbor) >= 0), "DBLL_Insert_After order is corrupt");
         pLL->last = pEntry;
         PBC_Ensure(NULL == pLL->last->next, "DBLL_Insert_After next pointer is not valid");
      }
      pLL->count++;

      PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Insert_After first or prev pointer is not valid");
      PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Insert_After first or prev pointer is not valid");
   }                            /* if (!only_one) */
}

/*===========================================================================*
 * FUNCTION: DBLL_Insert_Before
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void DBLL_Insert_Before(DBLL_List_T * pLL, DBLL_Entry_T * pEntry, DBLL_Entry_T * pNeighbor)
{
   bool_fast only_one = false;

   PBC_Require(pLL != NULL, "DBLL_Insert_Before cannot be called with a NULL pLL pointer");
   PBC_Require(pEntry != NULL, "DBLL_Insert_Before cannot be called with a NULL pEntry pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Insert_Before cannot be called with a NULL first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Insert_Before cannot be called with a NULL last or next pointer");

   if (NULL == pNeighbor)       /* insert at front of list */
   {
      pNeighbor = pLL->first;

      if (NULL == pNeighbor)    /* list is currently empty! */
      {
         pLL->first = pLL->last = pEntry;
         pLL->count++;
         pEntry->next = pEntry->prev = NULL;
         PBC_Internal(NULL == pLL->first->prev, "DBLL_Insert_Before cannot have a null prev pointer");
         PBC_Internal(NULL == pLL->last->next, "DBLL_Insert_Before cannot have a null next pointer");

         only_one = true;
      }
   }

   if (!only_one)
   {
      DBLL_Entry_T *oldPrev;

      oldPrev = pNeighbor->prev;
      pNeighbor->prev = pEntry;
      pEntry->next = pNeighbor;
      pEntry->prev = oldPrev;

      if (oldPrev != NULL)      /* not first in list */
      {
         /* if internal checks are enabled, double check the ordering */
         PBC_Internal((NULL == pLL->callbacks) ||
                             (NULL == pLL->callbacks->compare) ||
                             (pLL->callbacks->compare(pLL->callbacks->arg, pEntry, pNeighbor) <= 0), "DBLL_Insert_Before order is corrupt");
         PBC_Internal((NULL == pLL->callbacks) ||
                             (NULL == pLL->callbacks->compare) ||
                             (pLL->callbacks->compare(pLL->callbacks->arg, pEntry, oldPrev) >= 0), "DBLL_Insert_Before order is corrupt");
         oldPrev->next = pEntry;
      }
      else                      /* first in list */
      {
         /* if internal checks are enabled, double check the ordering */
         PBC_Internal((NULL == pLL->callbacks) ||
                             (NULL == pLL->callbacks->compare) ||
                             (pLL->callbacks->compare(pLL->callbacks->arg, pEntry, pNeighbor) <= 0), "DBLL_Insert_Before order is corrupt");
         pLL->first = pEntry;
         PBC_Ensure(NULL == pLL->first->prev, "DBLL_Insert_Before prev pointer is not valid");
      }
      pLL->count++;

      PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Insert_Before cannot have a null prev pointer");
      PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Insert_Before cannot have a null next pointer");
   }                            /* if (!only_one) */
}

/*===========================================================================*
 * FUNCTION: DBLL_Remove_Entry
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void *DBLL_Remove_Entry(DBLL_List_T * pLL, DBLL_Entry_T * pEntry)
{
   DBLL_Entry_T *next;
   DBLL_Entry_T *prev;

   PBC_Require(pLL != NULL, "DBLL_Remove_Entry cannot be called with a NULL bdb pointer");
   PBC_Require(pEntry != NULL, "DBLL_Remove_Entry cannot be called with a NULL bdb pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Remove_Entry cannot be called with a NULL first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Remove_Entry cannot be called with a NULL last or next pointer");

   prev = pEntry->prev;
   next = pEntry->next;
   pEntry->next = pEntry->prev = NULL;  /* disassociate entry from list */

   if (NULL == prev)            /* removing 1st one from list */
   {
      pLL->first = next;
   }
   else
   {
      prev->next = next;
   }

   if (next != NULL)            /* an entry follows the one being removed */
   {
      next->prev = prev;
   }
   else                         /* the last entry was just removed */
   {
      pLL->last = prev;
   }
   pLL->count--;

   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Remove_Entry cannot have a null first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Remove_Entry cannot have a null last or next pointer");

   return next;
}

/*===========================================================================*
 * FUNCTION: DBLL_Visit_Each
 *===========================================================================
 *
 * Please refer to the detailed description in double_linked_list.h.
 *
 *===========================================================================*/
void DBLL_Visit_Each(DBLL_List_T * pLL, DBLL_Entry_Callback_T pFunc, void *arg)
{
   DBLL_Entry_T *entry;

   PBC_Require(pLL != NULL, "DBLL_Visit_Each cannot be called with a NULL pLL pointer");
   PBC_Require(pFunc != NULL, "DBLL_Visit_Each cannot be called with a NULL pFunc pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "DBLL_Insert_Before cannot be called with a NULL first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "DBLL_Insert_Before cannot be called with a NULL last or next pointer");

   entry = pLL->first;

   while (entry != NULL)
   {
      /* cache the next value prior to the call. this allows the call to
       * do some drastic things (like remove/free the entry).
       */
      DBLL_Entry_T *next = entry->next;

      /* The iteration is terminated if the function returns zero. */
      if (!pFunc(arg, pLL, entry))
      {
         break;
      }

      entry = next;
   }                            /* while (entry != NULL) */
}

/*===========================================================================*/
/**
 * This is a callback function used to perform an ordered insertion by
 * visiting each node in the list until the insertion point is found.
 *
 * @return
 *   false if insertion has been made, true if iteration needs to
 *   continue.
 *
 * @param arg  A pointer to the entry to be inserted.
 *
 * @param pLL  Pointer to the double linked list object.
 *
 * @param pEntry  A pointer to the currently visited entry in the list.
 *
 * @pre
 *   - arg != NULL.
 *   - pLL != NULL.
 *   - pEntry != NULL
 *   - pLL->callbacks != NULL
 *   - pLL->callbacks->compare != NULL
 */
/*===========================================================================*/
static bool_t Do_Ordered_Insert(void *arg, DBLL_List_T * pLL, DBLL_Entry_T * pEntry)
{
   DBLL_Entry_T *newEntry = (DBLL_Entry_T *) arg;
   int_least8_t cmp;
   bool_fast keep_going = false;

   PBC_Internal(newEntry != NULL, "Do_Ordered_Insert cannot be called with a NULL arg pointer");
   PBC_Internal(pLL != NULL, "Do_Ordered_Insert cannot be called with a NULL pLL pointer");
   PBC_Internal(pLL->callbacks != NULL, "Do_Ordered_Insert cannot be called with a NULL callbacks pointer");
   PBC_Internal(pLL->callbacks->compare != NULL, "Do_Ordered_Insert cannot be called with a NULL compare pointer");
   PBC_Internal(pEntry != NULL, "Do_Ordered_Insert cannot be called with a NULL pEntry pointer");
   PBC_Internal((NULL == pLL->first) || (NULL == pLL->first->prev), "Do_Ordered_Insert cannot be called with a NULL first or prev pointer");
   PBC_Internal((NULL == pLL->last) || (NULL == pLL->last->next), "Do_Ordered_Insert cannot be called with a NULL last or next pointer");

   cmp = pLL->callbacks->compare(pLL->callbacks->arg, newEntry, pEntry);

   if (cmp <= 0)                /* newEntry "<=" pEntry */
   {
      DBLL_Insert_Before(pLL, newEntry, pEntry);
   }
   else if (NULL == pEntry->next)       /* new one goes at end of list */
   {
      DBLL_Insert_After(pLL, newEntry, pEntry);
   }
   else
   {
      keep_going = true;        /* continue iteration through list */
   }

   return keep_going;
}

/*===========================================================================*/
/*!
 * @file double_linked_list.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 8
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 17_Jun-2009 Larry Piekarski Rev 7
 *   - Peer Review Updates
 *
 * - 27-May-2009 Larry Piekarski Rev 6
 *   - Cleaned up code from review checklist
 *
 * - 14-feb-2008 kirk bailey
 *   - Renamed dbl_ll to double_linked_list.
 *   - Changed "dbll_" prefix to "DBLL_" to conform to BASA conventions.
 *
 * - 10-jan-2008 kirk bailey
 *   - Switched from "assert" to EM module MACROs.
 *   - Changed types to conform to corporate standards.
 *   - Added "const" where appropriate.
 *   - Updated format of Doxygen comments, improved comments.
 *
 *
 * - 25-aug-2006 kirk bailey
 *   -Corrected full_filespec, version, author, date fields.
 *
 * - 22-aug-2006 kirk bailey
 *   -Created initial file.
 */
/*===========================================================================*/
