/**
 * @file block_list.h
 *
 * Interface for creating, maintaining, and destroying a block list.
 *
 * %full_filespec:block_list.h~kok_inf#5:incl:kok_basa#1 %
 * @version %version:kok_inf#5 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Mon Nov 25 13:58:18 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Interface for creating, maintaining, and destroying a block list. A block list
 * is a special C implementation of a dynamic array.
 *
 * @section ABBR ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 * @defgroup block_list Interface for handling a block list (C dynamic array)
 * @{
 */

#ifndef BLOCLIST_H_
#define BLOCLIST_H_ 1

#include <stddef.h>

/**********************************************************************
 *
 *  external defines for bloclist.c...
 *
 *********************************************************************/

#define NO_OBJECTS_JUST_POINTERS (-1L)

#define DEFAULT_ITEMS_PER_BLOCK (256)

/*
#define alloc_mem malloc
#define free_mem  free
*/

#ifdef __cplusplus
extern "C"
{
#endif

extern void* alloc_mem(size_t s);
extern void free_mem(void* p);

#ifdef __cplusplus
}
#endif

/**********************************************************************
 *
 *  external structure definitions for bloclist.c...
 *
 *********************************************************************/

typedef struct s_link_block_struct
{
  void **item;
  struct s_link_block_struct *next_block;
} s_link_block;

typedef struct blocklist_struct_tag
{
  s_link_block *d_cur_block;
  s_link_block *d_first_block;
  unsigned long int d_num_items;
  long int d_cur_block_index;
  unsigned long int d_items_per_block;
  long int d_item_size;
  short int d_ok_to_kill_contents;
} blocklist;

/**********************************************************************
 *
 *  external function prototypes for bloclist.c...
 *
 *********************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

extern void create_blocklist(blocklist **list,
                             const long int items_per_block,
                             const long int item_size,
                             const short int ok_to_kill_contents);

extern void dispose_blocklist(blocklist *list);

extern void blocklist_add(blocklist *list,
                          void *const what);

extern void blocklist_remove(blocklist *list,
                             const long unsigned int index);

extern void *blocklist_get(blocklist *list,
                           const long unsigned int index);

extern void blocklist_set(blocklist *list,
                          void *const what,
                          const long unsigned int index);

extern void blocklist_move(blocklist *list,
                           const long unsigned int from,
                           const long unsigned int to);

extern void blocklist_kill_contents(blocklist *list);

#ifdef __cplusplus
}
#endif

#endif

/**********************************************************************
*                                                                     *
*   Modification Record:                                              *
*                                                                     *
**********************************************************************/
/*===========================================================================*/
/*!
 * @file block_list.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 01-dec-2011 Kris Boultbee
 *   - Created initial file.
 *
 * - 21-dec-2011 Kris Boultbee
 *   Task kok_basa#65531 Revision 2
 *   - Fixed improper End of Line issue.
 *
 * - 10-Oct-2013 Larry Piekarski Rev 3
 *   - Task 1502: Added bb_block_list to bbc_util. Made slight changes from the 
 *     original code to patch holes found in unit testing.
 *
 * - 25-Nov-2013 Kirk Bailey Rev 5
 *   - Fixed Doxygen comments.
 */
/*===========================================================================*/
/** @} doxygen end group */
