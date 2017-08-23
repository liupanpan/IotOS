#ifndef DOUBLE_LINKED_LIST_H
#   define DOUBLE_LINKED_LIST_H
/*===========================================================================*/
/**
 * @file double_linked_list.h
 *
 * API for functions to support doubly-linked lists.
 *
 * %full_filespec:double_linked_list.h~kok_basa#12:incl:kok_aud#1 %
 * @version %version:kok_basa#12 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Wed Jan  5 11:03:18 2011 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page double_linked_list_page Double Linked List User Manual
 *
 * @section DESC DESCRIPTION:
 *   API to support the managing of double linked lists. Note that these calls
 *   are NOT thread safe for a given structure; the client of this module must guarantee the
 *   thread safety of its double linked lists.
 *
 * @section USAGE USAGE INSTRUCTIONS:
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 *
 * @section ABBR ABBREVIATIONS:
 *   - DBLL = double linked list.
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
 *   - None
 *
 * @defgroup dbl_ll_grp Double Linked List Functions
 * @ingroup utilities_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/
/*
 * The following definition allows other types to have forward references
 * to DBLL_List_T.
 */
      struct DBLL_List_T;

/**
 * DBLL_Entry_T is the structure that can be used to create a doubly-linked
 * list of any other structure. By placing a DBLL_Entry_T element as the
 * first element of a structure, a client can build doubly-linked lists of
 * the containing structure. This takes advantage of the commonly used
 * feature of C compilers: a pointer to a structure is also a pointer
 * to its first element.
 */
   typedef struct DBLL_Entry_T
   {
      struct DBLL_Entry_T *next;
                              /**< Next entry in list */
       
   /** 
    * Previous entry in list
    */
      struct DBLL_Entry_T *prev;
   }
   DBLL_Entry_T;

/**
 * DBLL_Entry_Callback_T defines a callback function that can be used to
 * "visit" each entry in the list (see DBLL_Visit_Each()). The function can
 * terminate the visitation by returning false.
 */
   typedef bool_t(*DBLL_Entry_Callback_T) (void *arg, struct DBLL_List_T * pLL, DBLL_Entry_T * pEntry);

/**
 * DBLL_Match_Callback_T defines a callback function that can be used to
 * "find" a particular entry in the list (see DBLL_Find()).
 *
 * Return true on match 
 */
   typedef bool_t(*DBLL_Match_Callback_T) (void *arg, DBLL_Entry_T * pEntry);

/**
 * DBLL_Callback_T defines a set of callback functions that can be associated
 * with the linked list at the time of its creation. For the simplest type of
 * linked list, this structure is not required (NULL can be used). Only if
 * advanced features of the utility are required (sorting, allocation/
 * deallocation) is the structure required.
 */
   typedef struct DBLL_Callback_T
   {
      void *arg;
              /**< value to pass to other callback functions */
   /** 
    * calloc creates and initializes the client structure, returning a pointer
    * to it. calloc can be NULL if the library is not required to create
    * any entries for the list (see DBLL_Init()). This function is only used
    * during the initialization of the linked list.
    */
      DBLL_Entry_T *(*calloc) (void *arg);
   /**
    * compare, if defined, is used to place the entries into the linked list
    * according to the order defined by the compare function. It follows
    * the standard convention of returned <0, ==0, >0 for the three cases
    * e1 < e2, e1 == e2, and e1 > e2.
    */
        int_least8_t(*compare) (void *arg, DBLL_Entry_T const *e1, DBLL_Entry_T const *e2);
   /**
    * free is used to perform the reverse of calloc() when a DBLL_Destroy()
    * is called and the list contains entries. This function is not required
    * if calloc() is NULL or if the client guarantees that all entries are
    * deleted from the list prior to calling DBLL_Destroy().
    */
      DBLL_Entry_Callback_T free;
   }
   DBLL_Callback_T;

/**
 * DBLL_List_T defines the type used for doubly-linked list objects. Clients
 * must treat this as an anonymous type and not access its fields; only
 * the functions defined in this API should be used to modify its contents
 * (including initialization via DBLL_Init()). The type is published so
 * that client modules can define their own DBLL_List_T objects.
 */
   typedef struct DBLL_List_T
   {
   /** Pointer to first entry in list; NULL if list is empty. */
      DBLL_Entry_T *first;
   /** Pointer to last entry in list; NULL if list is empty. */
      DBLL_Entry_T *last;
      uint16_t count;
                    /**< Number of entries in list. */
   /** Callback functions for this list; NULL if there are no callbacks. */
      DBLL_Callback_T const *callbacks;
   }
   DBLL_List_T;

/*===========================================================================*
 * Exported Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/
#   ifdef __cplusplus
extern "C" 
{
   
#   endif                       /* __cplusplus */
/**
 * Add the specified entry to the linked list.
 *
 * @param [in,out] pLL  Pointer to the linked list object.
 *
 * @param [in,out] pEntry
 *   The entry to be added to the list. If a compare() callback function was
 *   provided, then the entries are placed in "ascending" order (as defined by
 *   the compare function); otherwise, entries are just added to the end of
 *   the list.
 *
 * @pre
 *    - pLL != NULL
 *    - pEntry != NULL
 */
   void DBLL_Add_Entry(DBLL_List_T * pLL, DBLL_Entry_T * pEntry);

/**
 * This function acts like the body of a destructor; it "uninitializes" the
 * doubly linked list. This function does NOT deallocate the DBLL_List_T
 * structure. The client is responsible for the memory allocation/deallocation
 * of the structure, which allows this library to support both statically and
 * dynamically allocated lists. If the callback functions associated with the
 * linked list contains a "free" function, then this function will call the
 * "free" function for each entry that remains in the list. This allows the
 * support of dynamically allocated entries and prevents memory leaks that would
 * occur if a linked list was destroyed without freeing its entries. If there
 * is no "free" callback function, then the call effectively "empties" out
 * the linked list.
 *
 * @param [in,out] pLL
 *    Pointer to the double linked list object to be destroyed.
 *
 * @pre
 *    - pLL != NULL
 */
   void DBLL_Destroy(DBLL_List_T * pLL);

/**
 * Returns entry which returns true by the callback function.
 *    Used to search linked list from the front.
 *
 * @return pointer to matching entry, if none then NULL
 *
 * @param [in,out] pLL  Pointer to the double linked list object.
 *
 * @param [in] pFunc  The match function.
 *
 * @param [in] arg  The argument to pass to the match function.
 *
 * @pre
 *    - pLL != NULL
 *    - pFunc != NULL
 */
   void *DBLL_Find(DBLL_List_T * pLL, DBLL_Match_Callback_T pFunc, void *arg);

/**
 * Returns entry which returns true by the callback function.
 *    Used to search linked list from a specific starting entry.
 *
 * @return pointer to matching entry, if none then NULL
 *
 * @param [in] pEntry  Pointer to the list entry from which search is started.
 *
 * @param [in] pFunc  The match function.
 *
 * @param [in] arg  The argument to pass to the match function.
 *
 * @pre
 *    - pLL != NULL
 *    - pFunc != NULL
 */
   void * DBLL_Find_Next(DBLL_Entry_T * pEntry, DBLL_Match_Callback_T pFunc, void *arg);


/**
 * Get the number of entries in the list.
 *
 * @return  The number of entries in the list.
 *
 * @param [in] pLL  Pointer to the double linked list object.
 *
 * @pre
 *    - pLL != NULL
 */
   uint16_t DBLL_Get_Count(DBLL_List_T const *pLL);

/**
 * Get a pointer to the first entry in the list.
 *
 * @return  Pointer to the first entry, or NULL if the list is empty.
 *
 * @param [in] pLL  Pointer to the double linked list object.
 *
 * @pre
 *    - pLL != NULL
 */
   void *DBLL_Get_First(DBLL_List_T const *pLL);

/**
 * Get pointer to the last entry in the list.
 *
 * @return  Pointer to the last entry, or NULL if the list is empty.
 *
 * @param [in] pLL  Pointer to the double linked list object.
 *
 * @pre
 *    - pLL != NULL
 */
   void *DBLL_Get_Last(DBLL_List_T const *pLL);

/**
 * Get the pointer to the entry that follows the one specified.
 *
 * @return
 *   The pointer to the following entry, or NULL if the specified entry is the
 *   last one.
 *
 * @param [in] pEntry
 *   Pointer to the list entry from which "next" object is to be obtained.
 *
 * @pre
 *    - pEntry != NULL
 */
   void *DBLL_Get_Next(DBLL_Entry_T const *pEntry);

/**
 * Get the pointer to the entry that precedes the one specified.
 *
 * @return
 *   The pointer to the preceding entry, or NULL if the specified entry is the
 *   first.
 *
 * @param [in] pEntry
 *   Pointer to the list entry from which "previous" object is to be obtained.
 *
 * @pre
 *    - pEntry != NULL
 */
   void *DBLL_Get_Prev(DBLL_Entry_T const *pEntry);

/**
 * This function initializes a double linked list object; optionally, the list
 * is populated with entries. This function can be thought of as performing
 * the body of a constructor. This function does NOT allocate the DBLL_List_T
 * structure. The client is responsible for the memory allocation/deallocation
 * of the structure, which allows this library to support both statically and
 * dynamically allocated lists.
 *
 * @param [in,out] pLL  Pointer to the double linked list object to initialize.
 *
 * @param [in] callbacks
 *   A pointer to a structure containing pointers to callback functions that
 *   allow the linked list operations to be customized. This can be NULL if
 *   <b>all</b> of the following are true:
 *     - 1) no initial populating of the list by this function is required
 *          (initialCount==0).
 *     - 2) the entries in the list are either going to be removed by the
 *          client prior to DBLL_Destroy being called or they require no
 *          special dynamic memory operation to be performed by DBLL_Destroy
 *          in order to prevent a memory leak.
 *     - 3) the capability of this module to apply ordering constraints to
 *          the entries will not be used; i.e., the location of entries is
 *          arbitrary, or specifically controlled by the client.
 *
 * @param [in] initialCount
 *   The number of initial entries to populate the list with using the
 *   specified calloc() callback function.
 *
 * @pre
 *    - pLL != NULL
 *    - (initialCount==0) || ((callbacks!=NULL) &&(callbacks->calloc!=NULL))
 */
   void DBLL_Init(DBLL_List_T * pLL, DBLL_Callback_T const *callbacks, uint16_t initialCount);

/**
 * Insert the entry into the linked list after another specified entry.
 *
 * @param [in,out] pLL  Pointer to the double linked list object.
 *
 * @param [in,out] pEntry  Pointer to the entry to be added to the list.
 *
 * @param [in,out] pNeighbor
 *   Pointer to the entry that pEntry is to follow; NULL if entry is to be
 *   placed at the end of the list.
 *
 * This is similar to DBLL_Add_Entry() but can be more efficient for ordered
 * lists if the relative position is already known by the caller. Of course,
 * if the caller gets it wrong, the list will be ordered incorrectly.
 *
 * @pre
 *    - pLL != NULL
 *    - pEntry != NULL
 */
   void DBLL_Insert_After(DBLL_List_T * pLL, DBLL_Entry_T * pEntry, DBLL_Entry_T * pNeighbor);

/**
 * Insert an entry into the linked list in front of another specified entry.
 *
 * @param [in,out] pLL  Pointer to the double linked list object.
 *
 * @param [in,out] pEntry  Pointer to the entry to be added to the list.
 *
 * @param [in,out] pNeighbor
 *   Pointer to the entry that pEntry is to precede; NULL if entry is to be
 *   placed at the beginning of the list.
 *
 * This is similar to DBLL_Add_Entry() but can be more efficient for ordered
 * lists if the relative position is already known by the caller. Of course,
 * if the caller gets it wrong, the list will be ordered incorrectly.
 *
 * @pre
 *    - pLL != NULL
 *    - pEntry != NULL
 */
   void DBLL_Insert_Before(DBLL_List_T * pLL, DBLL_Entry_T * pEntry, DBLL_Entry_T * pNeighbor);

/**
 * Remove the specified entry from the linked list.
 *
 * @return
 *   A pointer to the entry that followed the one removed from the list, NULL
 *   if the one removed was the last.
 *
 * @param [in,out] pLL  Pointer to the double linked list object.
 *
 * @param [in,out] pEntry  Pointer to the entry to be removed from the list.
 *
 * @pre
 *    - pLL != NULL
 *    - pEntry != NULL
 */
   void *DBLL_Remove_Entry(DBLL_List_T * pLL, DBLL_Entry_T * pEntry);

/**
 * Iterate through the list, visiting each entry, and calling the specified
 * function.
 *
 * @param [in,out] pLL  Pointer to the double linked list object.
 *
 * @param [in] pFunc  The function to call for each entry.
 *
 * @param [in,out] arg  The argument to pass to the callback function.
 *
 * @pre
 *    - pLL != NULL
 *    - pFunc != NULL
 */
   void DBLL_Visit_Each(DBLL_List_T * pLL, DBLL_Entry_Callback_T pFunc, void *arg);

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

#   ifdef __cplusplus
} 
#   endif /* __cplusplus */
/*===========================================================================*/
/*!
 * @file double_linked_list.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 11
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 21-apr-2010 Kirk Bailey Rev 10
 *   - Moved extern C block to only wrap function prototypes to match new
 *     template.
 *
 * - 17_Jun-2009 Larry Piekarski Rev 9
 *   - Peer Review Updates
 *
 * - 27-May-2009 Larry Piekarski Rev 8
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 7
 *   - Updated Doxygen Comments
 *
 * - 14-feb-2008 kirk bailey
 *   - Renamed dbl_ll to double_linked_list.
 *   - Changed "dbll_" prefix to "DBLL_" to conform to BASA conventions.
 *
 * - 10-jan-2008 kirk bailey
 *   - Changed types to conform to corporate standards.
 *   - Added "const" where appropriate.
 *   - Updated format of Doxygen comments, improved comments.
 *
 * - 19-apr-2007 kirk bailey
 *   -Updated format of Doxygen comments.
 *
 * - 25-aug-2006 kirk bailey
 *   -Corrected full_filespec, version, author, date fields.
 *
 * - 22-aug-2006 kirk bailey
 *   -Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* DOUBLE_LINKED_LIST_H */
