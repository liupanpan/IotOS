/**********************************************************************
 *
 *               (c) Delphi Delco Electronics Systems (unpub.)
 *        All Rights Reserved. Delphi Delco Electronics Confidential
 *
 **********************************************************************
 **********************************************************************
 * %full_filespec:block_list.c~kok_inf#7:csrc:kok_basa#1 %
 * @version %version:kok_inf#7 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Wed Oct 23 08:42:21 2013 %
 *
 * Title               : block_list.c
 *
 * Description         : this file is the source code for an indexed,
 *                       blocked, linked-list class.
 *
 * Configuration ID    :
 *
 *********************************************************************/

/**********************************************************************
 *
 *  standard #includes
 *
 *********************************************************************/
#include "block_list.h"
#include "pbc_trace.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "reuse.h"

#define EM_F_ID EM_FILE_ID(BLOCK_LIST_MODULE_ID, 0) /* Identifies file for PbC/trace */

#define nil NULL
#define NIL NULL

int malloc_count = 0;

/**********************************************************************
 *
 *  private subroutines for block_list.c...
 *
 *********************************************************************/
static void blocklist_replace(blocklist *list, void *const what, const long unsigned int index_i, bool_t free_memory_if_allowed);

static s_link_block* blocklist_allocate_item_block(const long int items_per_block,
                                                   const long int item_size);

static void blocklist_dispose_item_block(const s_link_block* const block);

static void blocklist_index_to_last_block(blocklist *list);

static void blocklist_add_item_to_new_block(blocklist *list,
                                            void *const what);

static void blocklist_add_first_item(blocklist *list,
                                     void *const what);

static void blocklist_add_item(blocklist *list,
                               void *const what);

static void blocklist_delete_item(blocklist *list,
                                  const long int index,
                                  void *const temp_item_object);

static void blocklist_delete_last_block(blocklist *list);

static void blocklist_move_item(blocklist *list,
                                const long int from,
                                const long int to,
                                void *const temp_item_object,
                                void *moved_item,
                                void* const moved_item_object);

static void blocklist_move_downward_in_list(blocklist *list,
                                            const long int from,
                                            const long int to,
                                            void *const temp_item_object);

static void blocklist_move_upward_in_list(blocklist *list,
                                          const long int from,
                                          const long int to,
                                          void *const temp_item_object);

static void blocklist_list_is_disposed(blocklist *list);

/**********************************************************************
 *
 * BLL list constructor...
 *
 * items_per_block is the number of items per blocks.
 * item_size is the size of an "item", or -1 if pointers only.
 * ok_to_kill_contents is true if OK to dispose of the list's contents.
 *
 * This method returns void.
 *
 *********************************************************************/

void create_blocklist(blocklist **list,
                      const long int items_per_block,
                      const long int item_size,
                      const short int ok_to_kill_contents)
{
   PBC_Require(list != NULL, "list may not be NULL");
   PBC_Require(items_per_block > 0, "items_per_block must be greater than 0");
   PBC_Require((item_size > 0) || (item_size == NO_OBJECTS_JUST_POINTERS), "item_size must be greater than 0 or NO_OBJECTS_JUST_POINTERS");

   /* Allocate memory for the list */
   *list = (blocklist*)alloc_mem(sizeof(blocklist));

   /* If allocated memory OK, initialize data members */
   if(*list)
   {
      (*list)->d_first_block = nil;
      (*list)->d_cur_block = nil;
      (*list)->d_num_items = 0;
      (*list)->d_items_per_block = items_per_block;
      (*list)->d_cur_block_index = -1;
      (*list)->d_item_size = item_size;
      (*list)->d_ok_to_kill_contents = ok_to_kill_contents;
   } /* End if create list OK */
} /* End create_blocklist */

/**********************************************************************
 *
 * BLL list destructor...
 *
 * This method returns void.
 *
 *********************************************************************/

void dispose_blocklist(blocklist *list)
{
  s_link_block *next = nil; /* Next block to be deleted */

  PBC_Require(list != NULL, "list pointer cannot be NULL");

  /* Kill the pointer-only contents of the list */
  blocklist_kill_contents(list);

  /* Get the first block */
  list->d_cur_block = list->d_first_block;

  /* Delete all of the blocks */
  while(nil != list->d_cur_block)
  {
    /* Save the next block - it will be the next current block,
       delete the array and the block itself,
       and set up for the next block */
    next = list->d_cur_block->next_block;
    blocklist_dispose_item_block(list->d_cur_block);
    list->d_cur_block = next;
  } /* End while more blocks */

  /* Clear data members */
  blocklist_list_is_disposed(list);

  /* Free the blocklist */
  free_mem(list);
} /* End dispose_blocklist */

/**********************************************************************
 *
 * This method adds an item to the list...
 *
 * what is the item to add.
 *
 * This method returns void.
 *
 *********************************************************************/

void blocklist_add(blocklist *list,
                   void *const what)
{
   PBC_Require(list != NULL, "list pointer cannot be NULL");
   PBC_Require((NO_OBJECTS_JUST_POINTERS == list->d_item_size) || (what != NULL), "what pointer cannot be NULL");

  /* If the first block exists, add the new item to the list */
  if(0 < list->d_num_items)
  {
    /* Move cur_block_ptr to the last block */
    blocklist_index_to_last_block(list);

    /* If the last block is full, need to allocate a new block */
    if(0 == (list->d_num_items % list->d_items_per_block))
      blocklist_add_item_to_new_block(list, what);

    /* Otherwise current block is not full - add to current block */
    else blocklist_add_item(list, what);
  } /* End if first block exists */

  /* Otherwise the list does not exists yet. Add the first item */
  else blocklist_add_first_item(list, what);
} /* End blocklist_add */

/**********************************************************************
 *
 * This method removes an item from the list...
 *
 * index is the item index to remove.
 *
 * This method returns void.
 *
 *********************************************************************/

void blocklist_remove(blocklist *list,
                      const long unsigned int index_i)
{
  void *temp_item_object = nil; /* For copying item objects */
  void *deleted_item = nil; /* For deleted items */

  PBC_Require(list != NULL, "list pointer cannot be NULL");

  /* If the index is valid, it's OK to remove the item at index */
  if(index_i < list->d_num_items)
  {
    /* If actual objects, allocate temporary buffer */
    if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
      temp_item_object = (void*)alloc_mem(list->d_item_size);

    /* Get the item to be deleted */
    deleted_item = blocklist_get(list, index_i);

    /* Delete the item at the given index */
    blocklist_delete_item(list, index_i, temp_item_object);

    /* If item is valid, and pointers are used, dispose of it */
    if(nil != deleted_item)
      if((false != list->d_ok_to_kill_contents) &&
         (NO_OBJECTS_JUST_POINTERS == list->d_item_size))
        free_mem(deleted_item);

    /* Delete the temporary object buffer if necessary */
    if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
      if(nil != temp_item_object) free_mem(temp_item_object);

    /* One less item now */
    list->d_num_items--;
  } /* End if valid index */
} /* End blocklist_remove */

/**********************************************************************
 *
 * This method gets an item from the list...
 *
 * index is the item index to get.
 *
 * This method returns a pointer to the item.
 *
 *********************************************************************/

void *blocklist_get(blocklist *list,
                    const long unsigned int index_i)
{
   PBC_Require(list != NULL, "list pointer cannot be NULL");

   /* If the index is valid, get the contents */
   if(index_i < list->d_num_items)
   {
      /* If the specified index is in a block that comes before the
       current block, need to start at the top to get to the
       "earlier" block */
      if((nil == list->d_cur_block) ||
         (list->d_cur_block_index < 0) ||
         (index_i <= (long unsigned int)(list->d_cur_block_index * list->d_items_per_block)))
      {
         list->d_cur_block = list->d_first_block;
         list->d_cur_block_index = 0;
      } /* End if index before d_cur_block_index */

      /* Search through the blocks to find the specified index */
      while((long unsigned int)((list->d_cur_block_index + 1) * list->d_items_per_block) <= index_i)
      {
         list->d_cur_block = list->d_cur_block->next_block;
         list->d_cur_block_index++;
      } /* End while not at right block */

      /* If pointers only, just return the pointer */
      if(NO_OBJECTS_JUST_POINTERS == list->d_item_size)
         return(list->d_cur_block->item[index_i % list->d_items_per_block]);

      /* Otherwise structures - return pointer to structure */
      else return((char*)(list->d_cur_block->item) +
         (index_i % list->d_items_per_block) * list->d_item_size);
   } /* End if valid index */

   return(nil);
} /* End blocklist_get */

/**********************************************************************
 *
 * This method sets the contents of an item in the list...
 *
 * what is what to set the item to.
 * index is the item index to set.
 *
 * This method returns void.
 *
 *********************************************************************/

void blocklist_set(blocklist *list,
                   void *const what,
                   const long unsigned int index_i)
{
   /* Make the call to replace with the free_memory_if_allowed parameter
      set to true that if the replaced item is a pointer and blocklist
	  is allowed to manage memory, then the replaced item will be freed
	  before it is replaced. This is to prevent memory leaks, and the 
	  caller has no way to know what item was replaced. */
   blocklist_replace(list, what, index_i, true);
} /* End blocklist_set */

/**********************************************************************
 *
 * This method moves an item within the list...
 *
 * from is the index of the item to be moved.
 * to is the index to move the item to.
 *
 * This method returns void.
 *
 *********************************************************************/

void blocklist_move(blocklist *list,
                    const long unsigned int from,
                    const long unsigned int to)
{
  void *moved_item; /* Item to be moved */
  void *moved_item_object = nil; /* For copying the moved item */
  void *temp_item_object = nil; /* For copying item objects */

  PBC_Require(list != NULL, "list pointer cannot be NULL");

  /* If indices are valid, it's OK to move the item in the list */
  if(from < list->d_num_items &&
     to < list->d_num_items &&
     from != to)
  {
    moved_item = blocklist_get(list, from);

    /* If objects, need to copy object, not just pointer to it */
    if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
    {
      temp_item_object = (void*)alloc_mem(list->d_item_size);
      moved_item_object = (void*)alloc_mem(list->d_item_size);

      /* If allocated memory OK, copy the object */
      if(nil != moved_item_object)
        memcpy(moved_item_object, moved_item, list->d_item_size);
    } /* End if actual objects */

    /* Move the item in the list */
    blocklist_move_item(list, from, to,
                        temp_item_object,
                        moved_item,
                        moved_item_object);

    /* If actual objects, release memory we allocated */
    if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
    {
      if(nil != moved_item_object) free_mem(moved_item_object);
      if(nil != temp_item_object) free_mem(temp_item_object);
    } /* End if actual objects */
  } /* End if indices valid */
} /* End blocklist_move */

/**********************************************************************
 *
 * This method disposes of the contents of the list...
 *
 * This method returns void.
 *
 *********************************************************************/

void blocklist_kill_contents(blocklist *list)
{
  unsigned long int i; /* Loop counter */

  PBC_Require(list != NULL, "list pointer cannot be NULL");

  /* If OK to dispose of contents, and using pointers only,
     kill the pointers that were allocated */
  if((false != list->d_ok_to_kill_contents) &&
     (NO_OBJECTS_JUST_POINTERS == list->d_item_size))
  {
    /* Dispose of all blocks in the list */
    list->d_cur_block = list->d_first_block;

    while(nil != list->d_cur_block)
    {
      /* Dispose of all items in the current block */
      for(i=0; i<list->d_items_per_block; i++)
      {
        if(nil != list->d_cur_block->item[i])
        {
          free_mem(list->d_cur_block->item[i]);
          list->d_cur_block->item[i] = nil;
        } /* End if valid item */
      } /* End for all items */

      /* Move to the next block */
      list->d_cur_block = list->d_cur_block->next_block;
    } /* End while more blocks */

    /* Mark current block pointer as invalid */
    list->d_cur_block_index = -1;
  } /* End if OK to kill contents, and using pointers only */
} /* End blocklist_kill_contents */

/**********************************************************************
 *
 * This private method sets the contents of an item in the list...
 *
 * what is what to set the item to.
 * index is the item index to set.
 * free_memory_if_allowed configures this method as to whether to 
 *                        free the replaced members memory (if 
 *                        appropriate) If this function is called
 *                        from and API, then this should be set to 
 *                        true, and if called internally, set to false.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_replace(blocklist *list,
                   void *const what,
                   const long unsigned int index_i, bool_t free_memory_if_allowed)
{
   PBC_Require(list != NULL, "list pointer cannot be NULL");
   PBC_Require((NO_OBJECTS_JUST_POINTERS == list->d_item_size) || (what != NULL), "what pointer cannot be NULL");

  /* If the index is valid, set the new contents */
  if(index_i < list->d_num_items)
  {
    /* If the specified index is in a block that comes before the
       current block, need to start at the top to get to the
       "earlier" block */
    if((nil == list->d_cur_block) ||
       (list->d_cur_block_index < 0) ||
       (index_i <= (long unsigned int)(list->d_cur_block_index * list->d_items_per_block)))
    {
      list->d_cur_block = list->d_first_block;
      list->d_cur_block_index = 0;
    } /* End if index before d_cur_block_index */

    /* Search through the blocks - find the specified index */
    while((long unsigned int)(list->d_cur_block_index + 1) * list->d_items_per_block <= index_i)
    {
      list->d_cur_block = list->d_cur_block->next_block;
      list->d_cur_block_index++;
    } /* End while not at right block */

    /* If pointers only, just set the new pointer */
    if(NO_OBJECTS_JUST_POINTERS == list->d_item_size)
    {
       if(free_memory_if_allowed && (false != list->d_ok_to_kill_contents) && (list->d_cur_block->item[index_i % list->d_items_per_block] != what))
       {
          /* If this is an external call, it is ok to kill contents, and we are not replacing the same thing with what was there,
           * then free the item that was replaced. */
          free_mem(list->d_cur_block->item[index_i % list->d_items_per_block]);
       }
       list->d_cur_block->item[index_i % list->d_items_per_block] = what;
    }
    /* Otherwise structures - copy the structure to the list */
    else
      memcpy((char*)(list->d_cur_block->item) +
                    (index_i % list->d_items_per_block) *
                    list->d_item_size,
             what, list->d_item_size);
  } /* End if valid index */
} /* End blocklist_replace */

/**********************************************************************
 *
 * This method allocates memory for an item block...
 *
 * This method returns a void** to the block.
 *
 *********************************************************************/

static s_link_block* blocklist_allocate_item_block(const long int items_per_block,
                                                   const long int item_size)
{
  /* If allocated memory for the block OK, allocate memory for the link */
  void** items = (void**)alloc_mem(items_per_block *
                                ((NO_OBJECTS_JUST_POINTERS == item_size) ?
                                 sizeof(void*) : (size_t)item_size));

  if(nil != items)
  {
    s_link_block* block = (s_link_block*)alloc_mem(sizeof(s_link_block));

    /* If allocated memory OK, set pointer, and return the block */
    if(nil != block)
    {
      block->item = items;
      return(block);
    } /* End if allocated block memory OK */

    /* If we got here, error occured, dispose of the items */
    free_mem(items);
  } /* End if allocated item memory OK */

  return(nil);
} /* End blocklist_allocate_item_block */

/**********************************************************************
 *
 * This method disposes of memory for an item block...
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_dispose_item_block(const s_link_block* const block)
{
  /* Dispose of the item block and the link */
  if(nil != block)
  {
    if(nil != block->item) free_mem(block->item);

    free_mem((void*)block);
  } /* End if block is valid */
} /* End blocklist_dispose_item_block */

/**********************************************************************
 *
 * This method moves the cur_block_ptr to the last block...
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_index_to_last_block(blocklist *list)
{
  /* If the current block is invalid, set it to the first block */
  if(nil == list->d_cur_block || list->d_cur_block_index < 0)
  {
    list->d_cur_block = list->d_first_block;
    list->d_cur_block_index = 0;
  } /* End if invalid current block */

  /* Find the last block */
  while((nil != list->d_cur_block) &&
        (nil != list->d_cur_block->next_block))
  {
    list->d_cur_block = list->d_cur_block->next_block;
    list->d_cur_block_index++;
  } /* End while not at the last block */
} /* End blocklist_index_to_last_block */

/**********************************************************************
 *
 * This method adds to the list when the current block is full,
 * and a new block must be allocated...
 *
 * what is the item to add.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_add_item_to_new_block(blocklist *list,
                                            void *const what)
{
  /* Allocate the item block */
  list->d_cur_block->next_block =
             blocklist_allocate_item_block(list->d_items_per_block,
                                           list->d_item_size);

  /* If allocated block OK, assign the current block pointers */
  if(nil != list->d_cur_block->next_block)
  {
    list->d_cur_block = list->d_cur_block->next_block;
    list->d_cur_block->next_block = nil;
    list->d_cur_block_index++;

    /* Add the item */
    blocklist_add_item(list, what);

    /* If necessary, set all pointers in new array to NULL */
    if((false != list->d_ok_to_kill_contents) &&
       (NO_OBJECTS_JUST_POINTERS == list->d_item_size))
    {
      unsigned int i = 0;

      for(i=1;i<list->d_items_per_block;i++)
      {
        list->d_cur_block->item[i] = NULL;
      } /* End for all array items */
    } /* End if will call free() on pointer contents */
  } /* End if no memory error */
} /* End blocklist_add_item_to_new_block */

/**********************************************************************
 *
 * This method adds to the list when there is nothing in it yet...
 *
 * what is the item to add.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_add_first_item(blocklist *list,
                                     void *const what)
{
  /* Allocate the item block */
  list->d_first_block =
           blocklist_allocate_item_block(list->d_items_per_block,
                                         list->d_item_size);

  /* If memory allocated OK, initialize current block pointers */
  if(nil != list->d_first_block)
  {
    list->d_cur_block = list->d_first_block;
    list->d_cur_block->next_block = nil;
    list->d_cur_block_index++;

    /* Add the item */
    blocklist_add_item(list, what);

    /* If necessary, set all pointers in new array to NULL */
    if((false != list->d_ok_to_kill_contents) &&
       (NO_OBJECTS_JUST_POINTERS == list->d_item_size))
    {
      unsigned int i = 0;

      for(i=1;i<list->d_items_per_block;i++)
      {
        list->d_cur_block->item[i] = NULL;
      } /* End for all array items */
    } /* End if will call free() on pointer contents */
  } /* End if allocated memory OK */
} /* End blocklist_add_first_item */

/**********************************************************************
 *
 * This method adds an item...
 *
 * what is the item to add.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_add_item(blocklist *list,
                               void *const what)
{
  /* If using only pointers, copy pointer */
  if(NO_OBJECTS_JUST_POINTERS == list->d_item_size)
    list->d_cur_block->item[list->d_num_items %
                            list->d_items_per_block] = what;

  /* Otherwise, copy entire object */
  else
    memcpy((char*)(list->d_cur_block->item) +
                  (list->d_num_items % list->d_items_per_block) *
                  list->d_item_size,
           what, list->d_item_size);

  /* Increment counters */
  list->d_num_items++;
} /* End blocklist_add_item */

/**********************************************************************
 *
 * This method shuffles items up in the list...
 *
 * index is the item index to remove.
 * temp_item_object is a pointer to a copy of the data to remove.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_delete_item(blocklist *list,
                                  const long int index_i,
                                  void *const temp_item_object)
{
   long unsigned int i = 0; /* Loop counter */
   void *item; /* Current item in list */

   if (NULL != list)
   {
      /* Shift everything under the index-th item up one position */
      for(i=index_i; i<list->d_num_items - 1; i++)
      {
         /* Get the "next" item in the list */
         item = blocklist_get(list, i + 1);

         /* If objects, need to copy object, not just pointer */
         if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
         {
            if((nil != temp_item_object) && (nil != item))
            {
               memcpy(temp_item_object, item, list->d_item_size);
            }
         }

         /* If item is valid, set as the "current" item in list */
         if(nil != item)
         {
            if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
            {
               item = temp_item_object;
            }
            if (NULL != item)
            {
               blocklist_replace(list, item, i, false);
            }
         } /* End if found item */
      } /* End for all items */

      /* If last item now in the list is the first item of a block,
        need to delete the block */
      if(0 == (i % list->d_items_per_block))
         blocklist_delete_last_block(list);

      /* Otherwise not the first item of a block,
        just set the item's pointer to nil */
      else if(NO_OBJECTS_JUST_POINTERS == list->d_item_size)
         blocklist_replace(list, nil, i, false);
   }
} /* End blocklist_delete_item */

/**********************************************************************
 *
 * This method deletes the last block in the list...
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_delete_last_block(blocklist *list)
{
  s_link_block *last_block;

  /* If there is a "next block" to advance to,
     advance to the next block */
  if(nil != list->d_cur_block->next_block)
    list->d_cur_block = list->d_cur_block->next_block;

  /* Get the first block */
  last_block = list->d_first_block;

  /* Special case when the very last item is being deleted */
  if(list->d_cur_block == list->d_first_block)
    list->d_first_block = nil;

  /* Otherwise some other block - find previous block */
  else
  {
    while((nil != last_block) &&
          (last_block->next_block != list->d_cur_block))
      last_block = last_block->next_block;

    /* If block that points to block to be deleted found,
       set to nil */
    if(nil != last_block) last_block->next_block = nil;
  } /* End else not first block */

  /* Delete the empty block */
  blocklist_dispose_item_block(list->d_cur_block);

  /* Set block info invalid */
  list->d_cur_block = nil;
  list->d_cur_block_index = -1;
} /* End blocklist_delete_last_block */

/**********************************************************************
 *
 * This method physically moves the item in the list...
 *
 * from is the index of the item to be moved.
 * to is the index to move the item to.
 * temp_item_object is a pointer to the data of the item to be moved.
 * moved_item is a pointer to the item to be moved.
 * moved_item_object is a copy of the raw data if not using pointers.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_move_item(blocklist *list,
                                const long int from,
                                const long int to,
                                void *const temp_item_object,
                                void *moved_item,
                                void* const moved_item_object)
{
  /* Move upward or downward in the list as appropriate */
  if(from < to)
    blocklist_move_downward_in_list(list, from, to, temp_item_object);
  else if(to < from)
    blocklist_move_upward_in_list(list, from, to, temp_item_object);

  /* If actual objects, need to copy the object,
     not just the pointer to it */
  if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
    moved_item = moved_item_object;

  /* Move the item */
  if(nil != moved_item) blocklist_replace(list, moved_item, to, false);
} /* End blocklist_move_item */

/**********************************************************************
 *
 * This method shuffles list items up...
 *
 * from is the index of the item to be moved.
 * to is the index to move the item to.
 * temp_item_object is a pointer to the data of the item to be moved.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_move_downward_in_list(blocklist *list,
                                            const long int from,
                                            const long int to,
                                            void *const temp_item_object)
{
   long int item_index; /* Loop counter */

   if (NULL != list)
   {
      /* Move all items between up one position */
      for(item_index=from; item_index<to; item_index++)
      {
         /* Get the "next" item in the list */
         void *item = blocklist_get(list, item_index + 1);

         /* If objects, need to copy object, not just pointer to it */
         if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
         {
            if((nil != temp_item_object) && (nil != item))
            {
               memcpy(temp_item_object, item, list->d_item_size);
            }
         }

         /* If item is valid, set as the "current" item in list */
         if(nil != item)
         {
            if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
            {
               item = temp_item_object;
            }
            if (NULL != item)
            {
               blocklist_replace(list, item, item_index, false);
            }
         } /* End if found item */
      } /* End for all items */
   }
} /* End blocklist_move_downward_in_list */

/**********************************************************************
 *
 * This method shuffles list items down...
 *
 * from is the index of the item to be moved.
 * to is the index to move the item to.
 * temp_item_object is a pointer to the data of the item to be moved.
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_move_upward_in_list(blocklist *list,
                                          const long int from,
                                          const long int to,
                                          void *const temp_item_object)
{
   long int item_index; /* Loop counter */

   if (NULL != list)
   {
      /* Move all items between down one position */
      for(item_index=from; item_index>to; item_index--)
      {
         /* Get the "next" item in the list */
         void *item = blocklist_get(list, item_index - 1);

         /* If objects, need to copy object, not just pointer to it */
         if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
         {
            if((nil != temp_item_object) && (nil != item))
            {
               memcpy(temp_item_object, item, list->d_item_size);
            }
         }

         /* If item is valid, set as the "current" item in list */
         if(nil != item)
         {
            if(NO_OBJECTS_JUST_POINTERS != list->d_item_size)
            {
               item = temp_item_object;
            }
            if (NULL != item)
            {
               blocklist_replace(list, item, item_index, false);
            }
         } /* End if found item */
      } /* End for all items */
   }
} /* End blocklist_move_upward_in_list */

/**********************************************************************
 *
 * This method "re-initializes" the list...
 *
 * This method returns void.
 *
 *********************************************************************/

static void blocklist_list_is_disposed(blocklist *list)
{
  /* Clear the list */
  list->d_first_block = (s_link_block*)NIL;
  list->d_cur_block = (s_link_block*)NIL;
  list->d_num_items = 0L;
  list->d_cur_block_index = -1L;
} /* End blocklist_list_is_disposed */


extern void* alloc_mem(size_t size)
{
   void* p;
   malloc_count++;
   p = malloc(size);
   return(p);
}

extern void free_mem(void* mem)
{
   malloc_count--;
   free(mem);
}
/**********************************************************************
*                                                                     *
*   Modification Record:                                              *
*                                                                     *
**********************************************************************/
/*===========================================================================*/
/*!
 * @file block_list.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 * ----------- --------
 * - 01-dec-2011 Kris Boultbee
 *   - Created initial file.
 *
 * - 21-dec-2011 Kris Boultbee
 *   Task kok_basa#65531 Revision 2
 *   - Fixed improper End of Line issue.
 *
 * - 12-sep-2012 Miguel Rios
 *   Task kok_basa#118859 Revision 3
 *   - Klocwork issues for bb_block_list.
 *     + Pointer 'item' checked for NULL at line 640 may be passed to function
 *       and may be dereferenced there by passing argument 2 to function
 *       'blocklist_set' at line 648.
 *     + Pointer 'item' checked for NULL at line 770 may be passed to function
 *       and may be dereferenced there by passing argument 2 to function
 *       'blocklist_set' at line 778.
 *     + Pointer 'item' checked for NULL at line 810 may be passed to function
 *       and may be dereferenced there by passing argument 2 to function
 *       'blocklist_set' at line 818.
 *
 * - 21-Feb-2013 Jeronimo R. Navarrete Rev.4
 *   + Updates to fix clockwork issues.
 *   + SCR kok_basa#38475. Task kok_basa#141258
 *
 * - 10-Oct-2013 Larry Piekarski Rev 5
 *   - Task 1502: Added bb_block_list to bbc_util. Made slight changes from the 
 *     original code to patch holes found in unit testing.
 *
 * - 11-Oct-2013 Larry Piekarski Rev 6
 *   - Task 1502: Added a new private function that is what blocklist_set used to 
 *     be, but is configurable on whether it frees the replaced object. blocklist_set
 *     now calls this function with the option to free memory set to true, and 
 *     internal calls have the option set false.
 *
 * - 23-Oct-2013 Larry Piekarski Rev 7
 *   - Task 2154: Made change to blocklist_add to only check for NULL what 
 *     pointer if blocklist is only pointers. If blocklist is actual blocks, 
 *     then what pointer is used in a memcpy, and the NULL check is 
 *     appropriate.
 */
/*===========================================================================*/
