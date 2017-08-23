#ifndef XSAL_H
#   define XSAL_H
/*===========================================================================*/
/**
 * @file xsal.h
 *
 * XSAL main header file.
 *
 * %full_filespec: xsal.h~kok_basa#25:incl:kok_aud#9 %
 * @version %version: kok_basa#25 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Fri Dec 18 12:48:12 2015 %
 *
 *---------------------------------------------------------------------------
 *
 * Copyright 2007 -2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *---------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *  There are two versions of XSAL: XSAL Light and XSAL.
 *  In order to select version XSAL_LIGHT or XSAL symbol must be defined.
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * @section ABBR ABBREVIATIONS:
 *    XSAL  - eXtended System Abstraction Layer
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *         SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *
 */
/*===========================================================================*/
#   ifdef __cplusplus
extern "C"
{
#   endif                       /* __cplusplus */

/** Define XSAL_PRESENT for Debug Trace
 */
#define XSAL_PRESENT
#   include "xsal_data_types.h"
#   include "xsal_application_id.h"
#   include "xsal_thread_id.h"
#   include "xsal_event_id.h"

#   if !defined(UITRON)
#      include "time.h"
#   endif /* !UITRON */

#   if defined(XSAL)

/** XSAL Only Forward declarations:
 */
struct SAL_NS_Data_Tag;
#   endif                       /* XSAL */

/** Forward declarations:
 */
struct SAL_Message_Tag;

/* PRQA S 3313 1 */
typedef struct SAL_Message_Queue_Node_Tag *SAL_Queue_Iterator_T;

/* -------------------------------------------------------------------------
 *
 *                              SAL Mutex
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Mutex_Grp XSAL Mutex
 *  @{
 */

/**
 *  Initializes mutex attributes.
 *
 *  @param [out]  attr mutex attributes to be initialized
 *
 *  @see SAL_Create_Mutex()
 */
void SAL_Init_Mutex_Attr(SAL_Mutex_Attr_T* attr);

/**
 *  Creates a mutex.
 *
 *  @param [out] mutex  pointer to the created mutex
 *  @param [in]  attr   mutex attributes
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Destroy_Mutex(), SAL_Lock_Mutex(), SAL_Unlock_Mutex()
 */
bool_t SAL_Create_Mutex(SAL_Mutex_T* mutex, const SAL_Mutex_Attr_T* attr);

/**
 *  Destroys a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be destroyed
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Mutex()
 */
bool_t SAL_Destroy_Mutex(SAL_Mutex_T* mutex);

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Locks a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be locked
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Mutex(), SAL_Try_Lock_Mutex(), 
 *       SAL_Lock_Mutex_Timeout(), SAL_Unlock_Mutex()
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);
#endif /* XSAL && !XSAL_LIGHT_API */

/**
 *  Attempts to lock a mutex.
 *
 *  If the mutex is already locked, the calling thread blocks until 
 *  the mutex becomes available or specified timeout expires.
 *
 *  @param [in] mutex         pointer to the mutex to be locked
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @see SAL_Create_Mutex(), SAL_Lock_Mutex(), 
 *       SAL_Try_Lock_Mutex(),SAL_Unlock_Mutex()
 */
bool_t SAL_Lock_Mutex_Timeout(SAL_Mutex_T* mutex, uint32_t timeout_msec);

/**
 *  Attempts to lock a mutex.
 *
 *  If the mutex is already locked then it returns immediately.
 *
 *  @param [in] mutex  pointer to the mutex to be locked
 *
 *  @return  true on success, false when the mutex is already locked 
 *           or an error occurs
 *
 *  @see SAL_Create_Mutex(), SAL_Lock_Mutex(), 
 *       SAL_Lock_Mutex_Timeout, SAL_Unlock_Mutex()
 */
bool_t SAL_Try_Lock_Mutex(SAL_Mutex_T* mutex);

/**
 *  Unlocks a mutex.
 *
 *  @param [in] mutex  pointer to the mutex to be unlocked
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Mutex(), SAL_Lock_Mutex(), SAL_Try_Lock_Mutex()
 */
bool_t SAL_Unlock_Mutex(SAL_Mutex_T* mutex);

/** @} *//* end of SAL_Mutex_Grp */

/* -------------------------------------------------------------------------
 *
 *                          SAL Read-Write Lock
 *
 * -------------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)

/** @defgroup SAL_RWLock_Grp XSAL Read-Write Lock
 *  @{
 */

/**
 *  Initializes Read-Write lock attributes.
 *
 *  @param [out] attr  Read-Write lock attributes to be initialized.
 *
 *  @note Currently XSAL doesn't support any Read-Write attributes.
 *
 *  @see SAL_Create_RWLock()
 */
void SAL_Init_RWLock_Attr(SAL_RWLock_Attr_T* attr);

/**
 *  Creates a Read-Write lock.
 *
 *  @param [out] rw_lock  pointer to the created Read-Write lock.
 *  @param [in]  attr     Read-Write lock attributes. If the attr pointer is
 *                        NULL, then the default attributes are used.
 *
 *  @return  true on success, false on failure
 *
 *  @note Currently XSAL doesn't support any Read-Write attributes, 
 *        so the attr param has to be set to NULL.
 *
 *  @see SAL_Destroy_RWLock(), SAL_RLock_RWLock(), SAL_WLock_RWLock(),
 *       SAL_Unlock_RWLock().
 */
bool_t SAL_Create_RWLock(SAL_RWLock_T* rw_lock, const SAL_RWLock_Attr_T* attr);

/**
 *  Destroys a Read-Write lock.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be destroyed.
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_RWLock().
 */
bool_t SAL_Destroy_RWLock(SAL_RWLock_T* rw_lock);

/**
 *  Locks a Read-Write lock on Read operation.
 *
 *  @param [in] rw_lock  pointer to the Read_Write lock.
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_RWLock(), 
 *       SAL_RLock_RWLock_Timeout(), SAL_Try_RLock_RWLock(),
 *       SAL_Unlock_RWLock().
 */
bool_t SAL_RLock_RWLock(SAL_RWLock_T* rw_lock);

/**
 *  Attempts to lock a Read-Write lock on Read operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  the calling thread blocks until the Read-Write lock becomes available 
 *  or specified timeout expires.
 *
 *  @param [in] rw_lock       pointer to the Read-Write lock to be locked
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @see SAL_Create_RWLock(),
 *       SAL_RLock_RWLock(), SAL_Try_RLock_RWLock(),
 *       SAL_Unlock_RWLock().
 */
bool_t SAL_RLock_RWLock_Timeout(SAL_RWLock_T* rw_lock, uint32_t timeout_msec);

/**
 *  Attempts to lock a Read-Write lock on Read operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  then it returns immediately.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be locked
 *
 *  @return  true on success, false when the rw_lock is already locked 
 *           or an error occurs
 *
 *  @see SAL_Create_RWLock(),
 *       SAL_RLock_RWLock(), SAL_RLock_RWLock_Timeout(),
 *       SAL_Unlock_RWLock().
 */
bool_t SAL_Try_RLock_RWLock(SAL_RWLock_T* rw_lock);

/**
 *  Locks a Read-Write lock on Write operation.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be locked.
 *
 *  @return  true on success, false on failure.
 *
 *  @see SAL_Create_RWLock(),
 *       SAL_WLock_RWLock_Timeout(), SAL_Try_WLock_RWLock(), 
 *       SAL_Unlock_RWLock().
 */
bool_t SAL_WLock_RWLock(SAL_RWLock_T* rw_lock);

/**
 *  Attempts to lock a Read-Write on Write operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  the the calling thread blocks until the Read-Write lock becomes available 
 *  or specified timeout expires.
 *
 *  @param [in] rw_lock         pointer to the Read-Write lock to be locked
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @see SAL_Create_RWLock(),
 *       SAL_WLock_RWLock(), SAL_Try_WLock_RWLock(), 
 *       SAL_Unlock_RWLock().
 */
bool_t SAL_WLock_RWLock_Timeout(SAL_RWLock_T* rw_lock, uint32_t timeout_msec);

/**
 *  Attempts to lock a Read-Write on Write operation.
 *
 *  If the Read-Write lock is already exclusively locked by any thread,
 *  then it returns immediately.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be locked.
 *
 *  @return  true on success, false when the mutex is already locked 
 *           or an error occurs
 *
 *  @see SAL_Create_Mutex(), SAL_Lock_Mutex(), 
 *       SAL_Lock_Mutex_Timeout, SAL_Unlock_Mutex()
 */
bool_t SAL_Try_WLock_RWLock(SAL_RWLock_T* rw_lock);

/**
 *  Unlocks a Read-Write lock.
 *
 *  @param [in] rw_lock  pointer to the Read-Write lock to be unlocked
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_RWLock(), SAL_RLock_RWLock(), SAL_WLock_RWLock()
 */
bool_t SAL_Unlock_RWLock(SAL_RWLock_T* rw_lock);

/** @} *//* end of SAL_RWLock_Grp */

/* -------------------------------------------------------------------------
 *
 *                         SAL Conditional Variable
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Cond_Grp XSAL Conditional Variable
 *  @{
 */

/**
 *  Initializes conditional variable attributes.
 *
 *  @param [out] attr  conditional variable attributes to be initialized.
 *
 *  @note Currently XSAL doesn't support any conditional variable attributes.
 *
 *  @see SAL_Create_Cond()
 */
void SAL_Init_Cond_Attr(SAL_Cond_Attr_T* attr);

/**
 *  Creates a conditional variable.
 *
 *  @param [out] cond   pointer to the created conditional variable.
 *  @param [in]  attr   conditional variable attributes. If the attr pointer
 *                      is NULL, then the default attributes are used.
 *
 *  @return  true on success, false on failure
 *
 *  @note Currently XSAL doesn't support any Conditional Variable attributes, 
 *        so the attr param has to be set to NULL.
 *
 *  @see SAL_Destroy_Cond(), SAL_Wait_Cond(), SAL_Signal_Cond()
 */
bool_t SAL_Create_Cond(SAL_Cond_T* cond, const SAL_Cond_Attr_T* attr);

/**
 *  Destroys a conditional variable.
 *
 *  @param [in] cond  pointer to the conditional variable to be destroyed.
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Cond().
 */
bool_t SAL_Destroy_Cond(SAL_Cond_T* cond);

/**
 *  Wait for conditional variable.
 *
 *  @param [in] cond   pointer to the conditional variable to wait for
 *  @param [in] mutex  pointer to the critical section mutex 
 *
 *  @return  true on success, false on failure
 *
 *  @note Some Operating Systems do not allow recursive mutexes with 
 *        condition variables.
 * 
 *  @see SAL_Create_Cond(), SAL_Wait_Cond_Timeout(), SAL_Signal_Cond()
 */
bool_t SAL_Wait_Cond(SAL_Cond_T* cond, SAL_Mutex_T* mutex);

/**
 *  Wait for conditional variable until it will be signalized
 *  or the specified timeout expires.
 *
 *  @param [in] cond          pointer to the conditional variable to wait for
 *  @param [in] mutex         pointer to the critical section mutex 
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 *
 *  @note Some Operating Systems do not allow recursive mutexes 
 *        with condition variables.
 *
 *  @see SAL_Create_Cond(), SAL_Wait_Cond(), SAL_Signal_Cond()
 */
bool_t SAL_Wait_Cond_Timeout(SAL_Cond_T* cond, SAL_Mutex_T* mutex, uint32_t timeout_msec);

/**
 *  Signals a conditional variable.
 *
 *  @param [in] cond   pointer to the conditional variable to be signaled
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Cond(), SAL_Wait_Cond()
 */
bool_t SAL_Signal_Cond(SAL_Cond_T* cond);

/** @} *//* end of SAL_Cond_Grp */

#endif /* XSAL && !XSAL_LIGHT_API */

/* -------------------------------------------------------------------------
 *
 *                              SAL Semaphore
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Semaphore_Grp XSAL Semaphore
 *  @{
 */

/**
 *  Initializes semaphore attributes.
 *
 *  @param [out]  attr  semaphore attributes
 *
 *  @see SAL_Create_Semaphore()
 */
void SAL_Init_Semaphore_Attr(SAL_Semaphore_Attr_T* attr);

/**
 *  Creates a semaphore.
 *
 *  @param [out] sem   pointer to the semaphore to be created
 *  @param [in]  attr  semaphore attributes.
 *
 *  @note Semafore initial value is defined in structure pointed by 
 *        attr pointer. If the pointer is equal NULL then initial values 
 *        is set to 0.
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Destroy_Semaphore(), SAL_Wait_Semaphore(), 
 *       SAL_Signal_Semaphore()
 */
bool_t SAL_Create_Semaphore(SAL_Semaphore_T* sem, const SAL_Semaphore_Attr_T* attr);

/**
 *  Destroys a semaphore.
 *
 *  @param [in] sem  pointer to the semaphore to be destroyed
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Semaphore()
 */
bool_t SAL_Destroy_Semaphore(SAL_Semaphore_T* sem);

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Attempts to wait for a semaphore.
 *
 *  Suspends the calling thread until the semaphore pointed to by 
 *  sem has non-zero count.
 *
 *  @param [in] sem  pointer to the semaphore to wait for
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Semaphore(), SAL_Try_Wait_Semaphore(),
 *       SAL_Wait_Semaphore_Timeout(), SAL_Signal_Semaphore()
 */
bool_t SAL_Wait_Semaphore(SAL_Semaphore_T* sem);
#endif /* XSAL && !XSAL_LIGHT_API */

/**
 *  Attempts to wait for a semaphore with a timeout.
 *
 *  If semaphore count is 0, the calling thread blocks until 
 *  the semaphore is signaled or specified timeout expires.
 *
 *  @param [in] sem           pointer to the semaphore to wait for
 *  @param [in] timeout_msec  timeout value (milliseconds)
 *
 *  @return  true on success, false on timeout or error
 * 
 *  @see SAL_Create_Semaphore(), SAL_Wait_Semaphore(), 
 *       SAL_Try_Wait_Semaphore(), SAL_Signal_Semaphore()
 */
bool_t SAL_Wait_Semaphore_Timeout(SAL_Semaphore_T* sem, uint32_t timeout_msec);

/**
 *  Attempts to wait for a semaphore.
 *
 *  Function does not block the calling thread if semaphore
 *  value is equal to zero.
 *
 *  @param [in] sem  pointer to the semaphore to wait for
 *
 *  @return  true on success, false when semaphore value is zero
 *           or an error occurs
 * 
 *  @see SAL_Create_Semaphore(), SAL_Wait_Semaphore(),
 *       SAL_Wait_Semaphore_Timeout(), SAL_Signal_Semaphore()
 */
bool_t SAL_Try_Wait_Semaphore(SAL_Semaphore_T* sem);

/**
 *  Signals a semaphore.
 *
 *  @param [in] sem  pointer to the semaphore to be signaled
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Semaphore(), SAL_Wait_Semaphore(), 
 */
bool_t SAL_Signal_Semaphore(SAL_Semaphore_T* sem);

/** @} *//* end of SAL_Semaphore_Grp */

/* -------------------------------------------------------------------------
 *
 *                              Buffer Pools
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Buffer_Pool_Grp XSAL Buffer Pools
 *  @{
 */

/**
 *  Creates a buffer pool.
 *
 *  @param [in] number_of_buffers  number of buffers in the pool
 *  @param [in] buffer_size        size of each buffer
 *  @param [out] pool_id           pointer to the returned buffer 
 *                                 pool identifier
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Alloc_Buffer(), SAL_Free_Buffer()
 */
bool_t SAL_Create_Buffer_Pool(uint32_t number_of_buffers, size_t buffer_size, SAL_Buffer_Pool_Id_T* pool_id);

/**
 *  Destroys the specified buffer pool.
 *
 *  @param [in] pool_id  buffer pool to be destroyed
 * 
 *  @see SAL_Create_Buffer_Pool()
 */
void SAL_Destroy_Buffer_Pool(SAL_Buffer_Pool_Id_T pool_id);

/**
 *  Allocates a buffer from the specified buffer pool.
 *
 *  @param [in] pool_id  identifier of the buffer pool to allocate
 *                       the buffer from
 *
 *  @return  pointer to the allocated buffer or NULL on error
 */
void* SAL_Alloc_Buffer(SAL_Buffer_Pool_Id_T pool_id);

/**
 *  Returns the given buffer to its pool.
 *
 *  @param [in] buffer  pointer to the allocated buffer
 *
 */
void SAL_Free_Buffer(void* buffer);

/** @} *//* end of SAL_Buffer_Pool_Grp */

/* -------------------------------------------------------------------------
 *
 *                              Memory Alloc
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Alloc_Grp Memory Alloc 
 *  @{
 */

/** 
 * OS independent memory allocator 
 *
 * @param [in] size number of bytes to allocate
 *
 * @return pointer to allocated memory, NULL if unable to allocate
 *
 *  @see SAL_Free()
 */
void *SAL_Alloc(size_t size);

/** 
 * OS independent memory free 
 *
 * @param [in] ptr pointer to block previously allocated by SAL_Alloc()
 *
 *  @see SAL_Alloc()
 */
void SAL_Free(void *ptr);

/** @} *//* end of SAL_Alloc_Grp */

/* -------------------------------------------------------------------------
 *
 *                         Synchronous communication
 *
 * -------------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)

/** @defgroup SAL_RPC_Grp Synchronous communication
 *  @{
 */

/**
 *  Creates a synchronous communication channel.
 *
 *  @param [out] channel  pointer to channel handle
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Destroy_Channel(), SAL_Bind(), SAL_Sync_Receive()
 */
bool_t SAL_Create_Channel(SAL_Channel_T* channel);

/**
 *  Destroys a synchronous communication channel.
 *
 *  @param [in] channel  pointer to channel handle
 *
 *  @note If the channel was bound (by calling SAL_Bind()), then it is not 
 *        unbound automatically. The user is responsible for unbinding the
 *        channel before destroying it.
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Channel(), SAL_Unbind()
 */
bool_t SAL_Destroy_Channel(SAL_Channel_T* channel);

/**
 *  Creates a connection to a channel.
 *
 *  @param [in]  channel     channel to create a connection to
 *  @param [out] connection  pointer to a connection handle
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Bind(), SAL_Disconnect(), SAL_Send_Pulse(), 
 *       SAL_Sync_Send() 
 */
bool_t SAL_Create_Connection(const SAL_Channel_T* channel, SAL_Connection_T* connection);

/**
 *  Closes a connection.
 *
 *  @param [in] connection  connection to be closed
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Connect(), SAL_Bind()
 */
bool_t SAL_Disconnect(SAL_Connection_T* connection);

/**
 *  Sends a pulse (short asynchronous message) to the receiver specified 
 *  by connection.
 *
 *  Pulse contains two data fields: code (1 byte) and value (4 bytes)
 *
 *  @param [in] connection  connection handle
 *  @param [in] code        pulse code
 *  @param [in] value       pulse value
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Channel(), SAL_Connect(), 
 *       SAL_Sync_Send(), SAL_Sync_Receive()
 */
bool_t SAL_Send_Pulse(SAL_Connection_T* connection, int8_t code, int32_t value);

/**
 *  Sends a message to the receiver specified by connection.
 *  The sending thread becomes blocked waiting for a reply.
 *
 *  @note The size of the reply buffer must be large enough to 
 *        accommodate the reply or the reply will be truncated.
 *
 *  @param [in]  connection    connection to the receiver
 *  @param [in]  message       message data to be sent
 *  @param [in]  message_size  message data size
 *  @param [out] reply         buffer for the reply
 *  @param [in]  reply_size    size of the reply buffer
 *  @param [out] reply_status  status returned by SAL_Sync_Reply() 
 *                             function
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Connect(), SAL_Send_Pulse(), SAL_Sync_Send_V(), 
 *       SAL_Sync_Send_VS(), SAL_Sync_Send_SV(), SAL_Sync_Receive(),
 *       SAL_Sync_Reply()
 */
bool_t SAL_Sync_Send(SAL_Connection_T* connection, const void* message, size_t message_size, void* reply, size_t reply_size,
   int32_t* reply_status);

/**
 *  Sends a multipart message to the receiver specified by connection.
 *  The sending thread becomes blocked waiting for a reply.
 *
 *  @note The size of the reply buffers must be large enough to 
 *        accommodate the reply or the reply will be truncated.
 *
 *  @param [in]  connection         connection to the receiver
 *  @param [in]  message_buffers    structure specifying the message buffers
 *                                  to be sent
 *  @param [in]  number_of_buffers  number of buffers to be sent
 *  @param [out] reply_buffers      structure specifying the message buffers
 *                                  for the reply
 *  @param [in]  number_of_reply_buffers  number of reply buffers
 *  @param [out] reply_status       status returned by SAL_Sync_Reply()
 *                                  function
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Connect(), SAL_Send_Pulse(), SAL_Sync_Send(), 
 *       SAL_Sync_Send_VS(), SAL_Sync_Send_SV(), 
 *       SAL_Sync_Receive(), SAL_Sync_Reply()
 */
bool_t SAL_Sync_Send_V(SAL_Connection_T* connection, const SAL_IOV_T* message_buffers, uint32_t number_of_buffers,
   const SAL_IOV_T* reply_buffers, uint32_t number_of_reply_buffers, int32_t* reply_status);

/**
 *  Sends a multipart message to the receiver specified by connection.
 *  The sending thread becomes blocked waiting for a reply.
 *
 *  @note The size of the reply buffer must be large enough to 
 *        accommodate the reply or the reply will be truncated.
 *
 *  @param [in]  connection         connection to the receiver
 *  @param [in]  message_buffers    structure specifying the message buffers
 *                                  to be sent
 *  @param [in]  number_of_buffers  number of buffers to be sent
 *  @param [out] reply              buffer for the reply
 *  @param [in]  reply_size         size of the reply buffer
 *  @param [out] reply_status       status returned by SAL_Sync_Reply()
 *                                  function
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Connect(), SAL_Send_Pulse(), SAL_Sync_Send(), 
 *       SAL_Sync_Send_VS(), SAL_Sync_Send_SV(), 
 *       SAL_Sync_Receive(), SAL_Sync_Reply()
 */
bool_t SAL_Sync_Send_VS(SAL_Connection_T* connection, const SAL_IOV_T* message_buffers, uint32_t number_of_buffers, void* reply,
   size_t reply_size, int32_t* reply_status);

/**
 *  Sends a message to the receiver specified by connection.
 *  The sending thread becomes blocked waiting for a reply.
 *
 *  @note The size of the reply buffers must be large enough to 
 *        accommodate the reply or the reply will be truncated.
 *
 *  @param [in]  connection      connection to the receiver
 *  @param [in]  message         message data to be sent
 *  @param [in]  message_size    message data size
 *  @param [out] reply_buffers   structure specifying the message buffers
 *                               for the reply
 *  @param [in]  number_of_reply_buffers  number of reply buffers
 *  @param [out] reply_status    status returned by SAL_Sync_Reply()
 *                               function
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Connect(), SAL_Send_Pulse(), SAL_Sync_Send(), 
 *       SAL_Sync_Send_VS(), SAL_Sync_Send_SV(), 
 *       SAL_Sync_Receive(), SAL_Sync_Reply()
 */
bool_t SAL_Sync_Send_SV(SAL_Connection_T* connection, const void* message, size_t message_size, const SAL_IOV_T* reply_buffers,
   uint32_t number_of_reply_buffers, int32_t* reply_status);

/**
 *  Waits for a message or pulse on a channel.
 *
 *  Function waits for a message or a pulse on the specified channel and stores 
 *  the received data in the buffer pointed to by message_buffer.
 *  If message size is greater then message_buffer_size, the function 
 *  SAL_Sync_Read() should be called in order to read subsequent parts of 
 *  the message.
 *
 *  The receive_id parameter is used to read data from the channel by 
 *  SAL_Sync_Read() and send a reply message by SAL_Sync_Reply().
 *
 *  @param [in]  channel       channel on which to wait for a message
 *  @param [out] buffer        buffer for the received message
 *  @param [in]  buffer_size   size of the received message
 *  @param [out] message_size  length of the received message
 *  @param [out] is_pulse      1 if the received message is a pulse, 0 otherwise
 *  @param [out] receive_id    receive channel
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Channel(), SAL_Bind(), SAL_Sync_Send(),
 *       SAL_Sync_Read(), SAL_Sync_Reply()
 */
bool_t SAL_Sync_Receive(SAL_Channel_T* channel, void* buffer, size_t buffer_size, size_t* message_size, uint8_t* is_pulse,
   SAL_Receive_T* receive_id);

/**
 *  Reads message data from the channel identified by receive_id.
 *
 *  Function reads the remaining data from client identified by receive_id.
 *
 *  @param [in]  receive_id             identifier of the client
 *  @param [out] buffer                 buffer for message data
 *  @param [in]  buffer_size            size of the message buffer
 *  @param [out] received_message_size  number of bytes that have been read
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Channel(), SAL_Bind(), SAL_Sync_Send(), 
 *       SAL_Sync_Receive(), SAL_Sync_Reply()
 */
bool_t SAL_Sync_Read(SAL_Receive_T* receive_id, void* buffer, size_t buffer_size, size_t* received_message_size);

/**
 *  Replies with a message to the sender.
 * 
 *  Function sends a status and a reply message to the client 
 *  identified by receive_id.
 *
 *  @param [in] receive_id          identifier of the client
 *  @param [in] reply_status        status of the reply operation
 *  @param [in] reply_message       data to be sent in reply
 *  @param [in] reply_message_size  length of reply data
 *
 *  @return  true on success, false on failure
 *
 *  @pre reply_status >= 0
 * 
 *  @see SAL_Sync_Receive(), SAL_Sync_Read(), SAL_Sync_Reply_V()
 */
bool_t SAL_Sync_Reply(const SAL_Receive_T* receive_id, int32_t reply_status, const void* reply_message, size_t reply_message_size);

/**
 *  Reply with a multipart message to a client.
 *
 *  Sends a status and a reply multipart message to the client 
 *  identified by receive_id parameter.
 *
 *  @param [in] receive_id         identifier of the client
 *  @param [in] reply_status       status of the reply operation
 *  @param [in] reply_buffers      structure specifying the reply buffers
 *  @param [in] number_of_buffers  number of reply buffers
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Sync_Receive(), SAL_Sync_Read(), SAL_Sync_Reply()
 */
bool_t SAL_Sync_Reply_V(const SAL_Receive_T* receive_id, int32_t reply_status, const SAL_IOV_T* reply_buffers,
   uint32_t number_of_buffers);

/** @} *//* end of SAL_RPC_Grp */

#endif /* XSAL  && !XSAL_LIGHT_API */

/* -------------------------------------------------------------------------
 *
 *                      Name Server proxy functions
 *
 * -----------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)

/** @defgroup SAL_NS_Proxy_Grp Name Server proxy
 *  @{
 */

/**
 *  Initializes a Name Server proxy.
 *  
 *  @note Function is not synchronized and shouldn't be called 
 *        from different threads at the same time.
 *
 *  @param [out] ns_data  name server data
 * 
 *  @return true it the Name Server is running.
 */
bool_t SAL_NS_Init_Proxy(struct SAL_NS_Data_Tag** ns_data);

/**
 *  De-initializes a Name Server proxy.
 *  
 *  @note Function is not synchronized and shouldn't be called 
 *        from different threads at the same time.
 */
void SAL_NS_Deinit_Proxy(void);

/**
 *  Check if the calling application and the Name Server 
 *  have the same configuration.
 *  It is done by comparing the following parameters: 
 *     SAL_MAX_CHANNELS, SAL_MAX_APPS and SAL_MAX_EVENTS.
 *
 *  @return true if the calling application and the Name Server
 *          have the same configuration.
 *
 *  @pre The SAL_NS_Init_Proxy has been caled.
 */
bool_t SAL_NS_Config_Ok(void);

/**
 *  Binds a channel with application id and port id.
 *
 *  @param [in] app_id   ID of the application to be bound
 *  @param [in] port_id  ID of the port to be bound
 *  @param [in] channel  channel handle
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_NS_Unbind_Channel(), SAL_NS_Resolve_Channel()
 */
bool_t SAL_NS_Bind_Channel(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, const SAL_Channel_T* channel);

/**
 *  Unbinds a channel from the Name Server.
 *
 *  @param [in] channel  channel data
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_NS_Bind_Channel(), SAL_NS_Resolve_Channel()
 */
bool_t SAL_NS_Unbind_Channel(const SAL_Channel_T* channel);

/**
 *  Finds a channel bound to app_id and port_id.
 *
 *  @param [in]  app_id   ID of the application
 *  @param [in]  port_id  ID of the port
 *  @param [out] channel  channel handle
 *
 *  @return  true on success, false on error, e.g. when no channel is bound
 *          to app_id and port_id.
 *
 *  @see SAL_NS_Bind_Channel(), SAL_NS_Unbind_Channel()
 */
bool_t SAL_NS_Resolve_Channel(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, SAL_Channel_T* channel);

/**
 *  Unbinds the given application from the Name Server.
 *
 *  All channels bound by the application will be removed from
 *  the Name Server.
 *
 *  @param [in] app_id  ID of the application to be unbound
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_NS_Bind_Channel(), SAL_NS_Unbind_Channel(), 
 *       SAL_NS_Resolve_Channel()
 */
bool_t SAL_NS_Unbind_App(SAL_App_Id_T app_id);

/**
 *  Gets a list of IDs of currently running XSAL applications.
 *
 *  Size of the reply_app_id_list table has to be SAL_MAX_APPS+1. 
 *  List of application IDs is terminated by 0.
 *
 *  @param [out] reply_app_id_list  list of running applications
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_NS_Get_Running_App_Ids(SAL_App_Id_T reply_app_id_list[]);

/**
 *  Gets the value of the last App Id from the name server. This will
 *  be SAL_MAX_APPS for programs built from the same SAL_Application_Id_T;
 *  this call allows the value to be obtained independently of that type.
 *
 *  @param [out] last_id Value of last App Id recognized by the Name Server.
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_NS_Get_Last_App_Id(uint16_t *last_id);

/**
 *  Gets the data for the specified XSAL application.
 *
 *  @param [in] app_id App Id of app for which data is to be obtained.
 *  @param [out] app_data Pointer to structure into which app's data is returned.
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_NS_Get_App_Data(uint16_t app_id, SAL_App_Data_T *app_data);

/**
 * Gets the value of the last dbg_trace module Id from the name server. This
 * will be (TR_NUM_MODULE_IDS - 1) for programs built from the same
 * Tr_Module_ID_T; this call allows the value to be obtained independently
 * of that type.
 *
 *  @param [out] last_id Value of last dbg_trace module id recognized by the Name Server.
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_NS_Get_Last_Dbg_Module_Id(uint16_t *last_id);

/**
 *  Gets the data for the specified dbg_trace module id.
 *
 *  @param [in] module_id Module Id for which data is to be obtained.
 *  @param [out] module_data Pointer to structure into which module's data is returned.
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_NS_Get_Dbg_Module_Data(uint16_t module_id, SAL_Dbg_Module_Data_T *module_data);

/**
 *  Subscribes the given application to events.
 *
 *  @param [in] app_id             application subscribing to events
 *  @param [in] number_of_events   number of events
 *  @param [in] event_id_list      event list
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_NS_Subscribe(SAL_App_Id_T app_id, const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);

/**
 *  Unsubscribes the given application from events.
 *
 *  @param [in] app_id             application unsubscribing
 *                                 from events
 *  @param [in] number_of_events   number of events
 *  @param [in] event_id_list      event list
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_NS_Unsubscribe(SAL_App_Id_T app_id, const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);

/**
 *  Unsubscribes the given application from all events
 *
 *  @param [in] app_id  application id
 */
void SAL_NS_Unsubscribe_App(SAL_App_Id_T app_id);

/**
 *  Binds a synchronous communication channel with app_id and port_id.
 * 
 *  The channel, app_id and port_id are stored in the Naming Service. 
 *  If a channel is bound then each application may create a connection 
 *  to it by calling SAL_Connect().
 *
 *  @param [in] app_id   application ID
 *  @param [in] port_id  port ID
 *  @param [in] channel  channel handle
 *
 *  @return  true on success, false on failure
 *
 *  @pre The Naming Service must be running.
 *
 *  @see SAL_Create_Channel(), SAL_Unbind(), SAL_Connect()
 */
bool_t SAL_Bind(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, const SAL_Channel_T* channel);

/**
 *  Unbinds the specified channel from the Naming Service. 
 *
 *  Function removes channel information from the Naming Service.
 *
 *  @param [in] channel  pointer to the channel data
 *
 *  @return  true on success, false on failure
 *
 *  @pre The Naming Service must be running.
 *
 *  @see SAL_Unbind(), SAL_Destroy_Channel()
 */
bool_t SAL_Unbind(const SAL_Channel_T* channel);

/**
 *  Creates a connection to a remote channel.
 *
 *  Function creates a connection to the channel specified by app_id 
 *  and port_id. Channel data are retrieved from the Naming Service.
 *
 *  @param [in] app_id       application ID
 *  @param [in] port_id      port ID
 *  @param [out] connection  pointer to a connection handle
 *
 *  @return  true on success, false on failure
 *
 *  @pre The Naming Service must be running.
 *
 *  @see SAL_Bind(), SAL_Disconnect(), SAL_Send_Pulse(), 
 *       SAL_Sync_Send(), SAL_Sync_Send_V()
 */
bool_t SAL_Connect(SAL_App_Id_T app_id, SAL_Port_Id_T port_id, SAL_Connection_T* connection);

/** @} *//* end of SAL_NS_Proxy_Grp */

#endif /* XSAL  && !XSAL_LIGHT_API */

/* -------------------------------------------------------------------------
 *
 *                          Shared Memory
 *
 * -----------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)

/** @defgroup SAL_Shm_Grp Shared Memory
 *  @{
 */

/**
 *  Creates a shared memory area with the given ID and size.
 *
 *  Shared memory ID is defined by the key and subkey parameters.
 *
 *  The mode parameter specifies whether the shared memory will be created in
 *  the read-only mode (when mode equals SAL_SHMEM_RDONLY) or read-write mode 
 *  (when mode equals SAL_SHMEM_RDWR).
 *
 *  Functions SAL_Create_Shared_Memory() and SAL_Open_Shared_Memory() fill in
 *  a structure of type SAL_Shared_Memory_Handle_T which serves as a handle to 
 *  the shared memory. The structure contains a member data (of type 
 *  void*) which points to the shared memory area (and must be used to 
 *  access it).
 *
 *  @note Note that SAL_Create_Shared_Memory() also opens the shared 
 *        memory area, so SAL_Open_Shared_Memory() doesn�t need to be 
 *        called.
 *
 *  @param [in] key, subkey  ID of the shared memory
 *  @param [in] size         size of the shared memory
 *  @param [in] mode         open mode: SAL_SHMEM_RDONLY or
 *                           SAL_SHMEM_RDWR
 *  @param [out] handle      handle to the shared memory
 *
 *  @return  true on success, false on failure
 *
 *  @pre  key and subkey must be greater than 0. 
 *
 *  @see SAL_Open_Shared_Memory(), SAL_Close_Shared_Memory()
 */
bool_t SAL_Create_Shared_Memory(SAL_Shared_Memory_Key_T key, SAL_Shared_Memory_Key_T subkey, size_t size,
   SAL_Shared_Memory_Open_Mode_T mode, SAL_Shared_Memory_Handle_T* handle);

/**
 *  Opens an already created shared memory area.
 *
 *  Parameters key, subkey and mode have the same meaning
 *  as in SAL_Create_Shared_Memory().
 *
 *  @param [in] key, subkey  ID of the shared memory
 *  @param [in] mode         open mode: SAL_SHMEM_RDONLY or
 *                           SAL_SHMEM_RDWR
 *  @param [out] handle      handle to the shared memory
 *
 *  @return  true on success, false on failure
 * 
 *  @see SAL_Create_Shared_Memory(), SAL_Close_Shared_Memory()
 */
bool_t SAL_Open_Shared_Memory(SAL_Shared_Memory_Key_T key, SAL_Shared_Memory_Key_T subkey, SAL_Shared_Memory_Open_Mode_T mode,
   SAL_Shared_Memory_Handle_T* handle);

/**
 *  Function closes an open shared memory area.
 *
 *  @param [in] handle  handle to the shared memory
 *
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Shared_Memory(), SAL_Open_Shared_Memory()
 */
bool_t SAL_Close_Shared_Memory(SAL_Shared_Memory_Handle_T* handle);

/** @} *//* end of SAL_Shm_Grp */

#endif /* XSAL  && !XSAL_LIGHT_API */

/* -------------------------------------------------------------------------
 *
 *                          XSAL engine
 *
 * -----------------------------------------------------------------------*/

/** @defgroup SAL_Engine_Grp XSAL Application
 *  @{
 */

/**
 *  XSAL configuration parameters
 */
typedef struct SAL_Config_Tag
{
      /** Identifier of XSAL Application.
       *
       *  Application IDs are unique for the whole system -- each XSAL
       *  application has its own ID defined in a globally known file
       *  xsal_application_id.h.
       */
      SAL_App_Id_T app_id;

      /** The maximum number of threads the application can have.
       *
       *  @note max_number_of_threads may be greater than the
       *        actual number of threads in the applications but it shouldn't be
       *        too large because XSAL preallocates some resources for all
       *        declared threads.
       */
      uint32_t max_number_of_threads;

      /** The maximum number of timers the application can have.
       *  If the application doesn't use any timers, then set it to 0.
       */
      uint32_t max_number_of_timers;

      /** The maximum number of buffer pools the application can have.
       *  If the application doesn't use any buffer pools, then set it to 0.
       */
      uint32_t max_number_of_buffer_pools;

      /** First dynamic thread ID. XSAL threads have numerical IDs. The IDs
       *  can be allocated by XSAL or defined by the user. All user-defined
       *  thread IDs must be greater than zero and less than
       *  first_unknown_thread_id. When XSAL allocates a thread ID, it starts
       *  at first_unknown_thread_id.
       */
      SAL_Thread_Id_T first_unknown_thread_id;
} SAL_Config_T;

/**
 * Initializes the sal_config structure with default XSAL configuration
 * parameters.
 *
 * @param [out] sal_config  pointer to a structure with XSAL
 *                          configuration data
 */
void SAL_Get_Config(SAL_Config_T* sal_config);

/**
 *  Initializes the XSAL Application.
 *
 *  @pre Function must be called before other XSAL functions.
 *
 *  @param [in] sal_config  pointer to the configuration parameters.
 *                          If it is NULL, the default configuration 
 *                          parameters are used.
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Run(), SAL_Get_Config()
 *
 */
bool_t SAL_Init(const SAL_Config_T* sal_config);

/**
 *  Starts up the XSAL engine.
 *
 *  SAL_Run() is called after application-specific initialization 
 *  (e.g. thread creation) is finished to activate XSAL. It will not
 *  exit unless all user threads terminate or SAL_Exit() is called 
 *  by one of the threads.
 *
 *  @return  value passed to SAL_Exit() or 0 when all threads terminate
 *
 *  @pre SAL_Init() has been called and at least one XSAL thread has been 
 *       created
 *
 *  @see SAL_Init(), SAL_Exit()
 */
int32_t SAL_Run(void);

/**
 *  Returns the ID of the application
 *
 *  @return  ID of the application
 *
 *  @see SAL_Init(), SAL_Get_Config()
 */
SAL_App_Id_T SAL_Get_App_Id(void);

/**
 *  Returns the name of the application identified by app_id.
 *
 *  @param [in] app_id  application identifier
 *
 *  @return application name
 *
 *  @pre 1 <= app_id < SAL_LAST_APP_ID
 */
const char_t* SAL_Get_App_Name(SAL_App_Id_T app_id);

/**
 *  Returns the maximum number of threads the application can have.
 *
 *  @return  number of threads the application can have
 *
 *  @see SAL_Init(), SAL_Get_Config()
 */
uint32_t SAL_Get_Max_Number_Of_Threads(void);

/**
 *  Terminates the calling thread and causes SAL_Run() to exit. 
 *  The value of the status parameter is returned by SAL_Run().
 *
 *  @note Function terminates the calling thread and causes SAL_Run()
 *        to exit. Other running threads are not terminated.
 *
 *  @param status  value that will be returned by SAL_Run()
 *
 *  @see SAL_Run()
 */
void SAL_Exit(int32_t status);

/** @} *//* end of SAL_Engine_Grp */

/* -------------------------------------------------------------------------
 *
 *                           Thread management
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Thread_Grp Thread
 *  @{
 */

/**
 *  Initializes thread attribute structure.
 *
 *  @param [out]  attr  thread attributes to be initialized
 *
 *  @see SAL_Create_Thread()
 */
void SAL_Init_Thread_Attr(SAL_Thread_Attr_T* attr);

/**
 *  Creates a thread with a given name, identifier, priority, thread
 *  function to execute, and the argument to pass to the thread function.
 *
 *  Thread name, thread identifier and priority are defined in structure 
 *  pointed by attr pointer.
 *
 *  If the attr pointer is set to NULL then default values are used:
 *   o) thread identifier is set to SAL_UNKNOWN_THREAD_ID, 
 *   o) thread name is set to numerical value of thread identifier,  
 *   o) thread priority is set to the SAL_DEFAULT_THREAD_PRIORITY value
 *      (defined during compilation)
 *
 *  If SAL_UNKNOWN_THREAD_ID is passed as thread ID, XSAL will allocate
 *  an ID for the thread (@see SAL_Init()).
 *
 *  @note It is not possible to create more than one thread with 
 *        the same thread id. 
 *
 *  @note In order to synchronize thread startup and shutdown, functions
 *        SAL_Signal_Ready(), SAL_Wait_Ready() and SAL_Wait_Destroyed() 
 *        can be used.
 *
 *  @note Thread is not started until SAL_Run() is called.
 *
 *  @param [in] thread_function thread function
 *  @param [in] param           parameter passed to thread function
 *  @param [in] attr            thread attributes
 *
 *  @return  ID of the created thread or SAL_UNKNOWN_THREAD_ID in case of failure
 *
 *  @pre thread_id has to be in the range 1 to 
 *       max_number_of_threads (as declared by SAL_Init()).
 *
 *  @see SAL_Init(), SAL_Run(), SAL_Destroy_Thread(),
 *       SAL_Signal_Ready(), SAL_Wait_Ready(), SAL_Wait_Destroyed()
 */
SAL_Thread_Id_T SAL_Create_Thread(void (*thread_function)(void*), void* param, const SAL_Thread_Attr_T* attr);

/**
 *  Checks if the thread identified by thread_id is alive
 *  (has been created and hasn't been terminated before
 *  SAL_Is_Thread_Alive() is called).
 *
 *  @param [in] thread_id  thread identifier
 *
 *  @return false if the thread is not alive
 *
 *  @pre thread_id has to be in the range 1 to 
 *       max_number_of_threads (as declared by SAL_Init()).
 *
 *  @see SAL_Init(), SAL_Create_Thread(), SAL_Destroy_Thread()
 */
bool_t SAL_Is_Thread_Alive(SAL_Thread_Id_T thread_id);

/**
 *  Returns the thread ID of the calling thread.
 *
 *  @return thread ID of the caller
 *
 *  @see SAL_Create_Thread()
 */
SAL_Thread_Id_T SAL_Get_Thread_Id(void);

/**
 *  Returns the name of the thread identified by thread_id.
 *
 *  @param [in] thread_id  thread identifier
 *
 *  @return thread name
 *
 *  @pre thread_id has to be in the range 1 to 
 *       max_number_of_threads (as declared by SAL_Init()).
 *
 *  @see SAL_Init(), SAL_Create_Thread()
 */
const char_t* SAL_Get_Thread_Name(SAL_Thread_Id_T thread_id);

/**
 *  Modifies the priority of thread identified by thread_id.
 *
 *  @note Numerical values of thread priorities depend on the target OS.
 *        Projects based on XSAL have to define their priority values.
 *
 *  @note Some OSes require admin rights to set thread priorities.
 *
 *  @param [in] thread_id  thread identifier
 *  @param [in] priority   priority value
 *
 *  @return  true on success, false on failure
 *
 *  @pre thread_id has to be in the range 1 to 
 *       max_number_of_threads (as declared by SAL_Init()).
 *
 *  @see SAL_Init(), SAL_Create_Thread(), SAL_Get_Thread_Priority()
 */
bool_t SAL_Set_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T priority);

/**
 *  Returns priority of thread identified by thread_id.
 *
 *  @param [in] thread_id  thread identifier
 *  @param [out] priority  pointer to the variable to store the priority in
 *
 *  @return  true on success, false on failure
 *
 *  @pre thread_id has to be in the range 1 to 
 *       max_number_of_threads (as declared by SAL_Init()).
 *
 *  @see SAL_Init(), SAL_Set_Thread_Priority()
 */
bool_t SAL_Get_Thread_Priority(SAL_Thread_Id_T thread_id, SAL_Priority_T* priority);

/**
 *  Suspends the caller for the specified number of milliseconds.
 *
 *  @param [in] milliseconds  suspend time
 */
void SAL_Sleep(uint32_t milliseconds);

/**
 *  Suspends the caller for the specified number of microseconds.
 *
 *  @note Might be implemented as busy-waiting.
 *
 *  @param [in] microseconds  suspend time
 */
void SAL_Delay(uint32_t microseconds);

/* -------------------------------------------------------------------------
 *
 *              Thread synchronization at startup/termination
 *
 * -------------------------------------------------------------------------*/

/** 
 * This function has been deemed unnecessary and should not be used
 */
#define SAL_End_Thread() Tr_Warn("SAL_End_Thread has been depracated. Remove from your code");

/**
 *  Notifies other threads in a process that a given thread 
 *  has completed its initialization.  
 *
 *  @note The notification generated by SAL_Signal_Ready() remains 
 *        in effect until the calling thread is terminated.
 *
 *  @see SAL_Wait_Ready(), SAL_Wait_Destroyed()
 */
void SAL_Signal_Ready(void);

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Suspends its caller until all threads in thread_id_list
 *  have called SAL_Signal_Ready().
 *
 *  @param [in] thread_id_list   list of IDs of threads to wait for
 *  @param [in] number_of_items  number of thread IDs on the list
 *
 *  @see SAL_Signal_Ready(), SAL_Wait_Destroyed()
 */
void SAL_Wait_Ready(const SAL_Thread_Id_T thread_id_list[], uint32_t number_of_items);
#endif /* XSAL  && !XSAL_LIGHT_API */

/**
 *  Suspends its caller until all threads in thread_id_list
 *  have called SAL_Signal_Ready() or the specified timeout expires.
 *
 *  @param [in] thread_id_list   list of IDs of threads to wait for
 *  @param [in] number_of_items  number of thread IDs on the list
 *  @param [in] timeout_msec     timeout (milliseconds)
 *
 *  @see SAL_Signal_Ready(), SAL_Wait_Destroyed()
 */

bool_t SAL_Wait_Ready_Timeout(const SAL_Thread_Id_T thread_id_list[], uint32_t number_of_items, uint32_t timeout_msec);

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Suspends its caller until all threads in thread_id_list 
 *  have been terminated.
 *
 *  @param [in] thread_id_list   list of IDs of threads to wait for
 *  @param [in] number_of_items  number of thread IDs on the list
 *
 *  @see SAL_Signal_Ready(), SAL_Wait_Ready()
 */
void SAL_Wait_Destroyed(const SAL_Thread_Id_T thread_id_list[], uint32_t number_of_items);
#endif /* XSAL  && !XSAL_LIGHT_API */

/**
 *  Suspends its caller until all threads in thread_id_list 
 *  have been terminated or the specified timeout expires.
 *
 *  @param [in] thread_id_list   list of IDs of threads to wait for
 *  @param [in] number_of_items  number of thread IDs on the list
 *  @param [in] timeout_msec     timeout (milliseconds)
 *
 *  @see SAL_Signal_Ready(), SAL_Wait_Ready()
 */
bool_t SAL_Wait_Destroyed_Timeout(const SAL_Thread_Id_T thread_id_list[], uint32_t number_of_items, uint32_t timeout_msec);

/** @} *//* end of Thread */

/* -------------------------------------------------------------------------
 *
 *                            Message Header
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Message_Grp Message passing
 *  @{
 */

/**
 *  Structure with the message header.
 */
typedef struct SAL_Message_Tag
{

#   if !defined(UITRON)
      /** Send time (seconds).
       */
      time_t timestamp_sec;
#   endif                       /* !UITRON */

      /** Size of data associated with the message.
       */
      size_t data_size;

      /** Pointer to message data.
       */
      void *data;

      /** Event ID.
       */
      SAL_Event_Id_T event_id;

#   if !defined(UITRON)
      /** Object ID (reserved for XSAL C++ wrapper).
       */
      int16_t object_id;
#   endif                       /* !UITRON */

      /** ID of application sending the message.
       */
      SAL_App_Id_T sender_app_id;

      /** ID of thread sending the message.
       */
      SAL_Thread_Id_T sender_thread_id;

      /** ID of the receiver thread.
       */
      SAL_Thread_Id_T receiver_thread_id;

#   if !defined(UITRON)
      /** Send time (milliseconds).
       */
      uint16_t timestamp_msec;
#   else
      /** Send time (milliseconds).
       */
      uint32_t timestamp_msec;
#   endif                       /* !UITRON */

} SAL_Message_T;

/* -------------------------------------------------------------------------
 *
 *                              Queue management
 *
 * -------------------------------------------------------------------------*/

/**
 *  Creates a message queue for the calling thread. 
 *
 *  Every thread must create its message queue before it can receive 
 *  asynchronous messages.
 *
 *  XSAL pre-allocates queue_size memory buffers of size message_size and 
 *  places the received messages in the buffers.
 *
 *  If the size of the arriving message is greater than message_size then 
 *  XSAL uses alloc_buf to dynamically allocate a memory buffer (and free_buf
 *  to release it).
 *
 *  If message_size equals zero then memory buffers are not pre-allocated 
 *  and alloc_buf and free_buf are always used instead.
 *
 *  If alloc_buf and free_buf are NULL, the message is dropped when its size
 *  is greater than message_size.
 *
 *  @note Pre-allocated buffers are fast (memory allocation/de-allocation
 *        is not required) but more memory may be used because queue_size 
 *        buffers are always needed (even if the queue is partially empty).
 *
 *  @note When a thread is terminated, its message queue is destroyed
 *        automatically.
 *
 *  @param [in] queue_size    maximum number of messages the queue can have
 *  @param [in] message_size  maximum message size (pre-allocated 
 *                            buffer size)
 *  @param [in] alloc_buf     pointer to the function allocating memory 
 *                            for arriving messages
 *  @param [in] free_buf      pointer to the function freeing memory used 
 *                            by messages
 *  @return  true on success, false on failure
 *
 *  @see SAL_Send(), SAL_Publish(),
 *       SAL_Receive(), SAL_Receive_From(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent(),
 *       SAL_Subscribe(), SAL_Unsubscribe()
 */
bool_t SAL_Create_Queue(size_t queue_size, size_t message_size, void* (*alloc_buf)(size_t size), void (*free_buf)(void* buf));

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Creates a message queue for the specified thread.
 *
 *  @param [in] thread_id     thread ID
 *  @param [in] queue_size    maximum number of messages the queue can have
 *  @param [in] message_size  maximum message size (pre-allocated 
 *                            buffer size)
 *  @param [in] alloc_buf     pointer to the function allocating memory 
 *                            for for arriving messages
 *  @param [in] free_buf      pointer to the function freeing memory used 
 *                            by messages
 *  @return  true on success, false on failure
 *
 *  @see SAL_Create_Queue(),
 */
bool_t SAL_Create_Queue_For_Thread(SAL_Thread_Id_T thread_id, size_t queue_size, size_t message_size,
   void* (*alloc_buf)(size_t size), void (*free_buf)(void* buf));

#endif /* XSAL  && !XSAL_LIGHT_API */

typedef struct SAL_Stat_Queue_Tag
{
      /** Queue size (as passed to SAL_Create_Queue)
       */
      size_t queue_size;

      /** Message buffer size (as passed to SAL_Create_Queue)
       */
      size_t buffer_size;

      /** Current number of messages in the queue
       */
      uint32_t message_count;

      /** Current number of limbo messages in the queue
       *    Should equal the difference between message_count and message_available
       */
      uint32_t limbo_message_count;

      /** Peak number of enqueued messages (since queue creation)
       */
      uint32_t peak_message_count;

      /** Size of largest enqueued messages (since queue creation)
       */
      uint32_t peak_message_size;

      /** Similar to total_rcvd_msgs, except that it can be reset to 0
       *  by calling SAL_Stat_Queue_Reset_Delta_Rcvd_Msgs, allowing for 
       *  a delta from the previous report.
       */
      uint32_t delta_rcvd_msgs;

      /** Total number of messages received by this thread.
       */
      uint32_t total_rcvd_msgs;
} SAL_Stat_Queue_T;

/**
 *  Retrieves the status information and statistics related to 
 *  a message queue owned by thread thread_id.
 *
 *  If a given thread does not have queue, function returns false.
 *
 *  @param [in]  thread_id  ID of the thread whose the queue is examined
 *  @param [out] queue_stat pointer to structure in which queue statistics 
 *               are returned
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_Stat_Queue(SAL_Thread_Id_T thread_id, SAL_Stat_Queue_T* queue_stat);

/**
 *  Resets to 0 the queue stat delta_rcvd_msgs for the queue
 *  owned by thread thread_id.
 *
 *  If thread thread_id does not have a queue, then the function returns false.
 *
 *  @param [in]  thread_id  ID of the thread to reset delta_rcvd_msgs for
 *
 *  @return  true on success, false on failure
 */
bool_t SAL_Stat_Queue_Reset_Delta_Rcvd_Msgs(SAL_Thread_Id_T thread_id);

/**
 *  List of reasons why the message has been dropped.
 */
typedef enum SAL_Message_Dropped_Reason_Tag
{
   /** Destination queue doesn't exist.
    */
   SAL_No_Queue,

   /** Destination queue is full.
    */
   SAL_Queue_Full,

   /** Memory for the message couldn't be allocated.
    */
   SAL_No_Memory,

   /** Memory allocation function is not defined 
    *  and pre-allocated buffer is too small.
    */
   SAL_Too_Small_Buffer,

   /** There was a communication error during message transmission.
    */
   SAL_Communication_Error
} SAL_Message_Dropped_Reason_T;

/**
 *  The type of the callback function to be called when
 *  a message is dropped.
 *
 *  @note
 *  The message parameter passed to the callback points to the header of the 
 *  received message. The pointer to message data may not be initialized;
 *  other fields of the header will be initialized correctly.
 *
 *  @param  thread_id  ID of the thread whose queue is full
 *  @param  reason     reason why the message is dropped
 *  @param  message    pointer to the header of the dropped message
 */
typedef void (*SAL_Message_Dropped_Callback_T)(SAL_Thread_Id_T thread_id, SAL_Message_Dropped_Reason_T reason,
   const SAL_Message_T* message);

/**
 *  Registers the callback function to be called when a message is dropped.
 *
 *  @param  [in] fnc pointer to the callback function
 *
 *  @return pointer to previously installed callback function (or NULL if no
 *          function was installed)
 */
SAL_Message_Dropped_Callback_T SAL_Set_Message_Dropped_Callback(SAL_Message_Dropped_Callback_T fnc);

/* -------------------------------------------------------------------------
 *
 *                           Message passing
 *
 * -------------------------------------------------------------------------*/

/**
 *  Puts a message into the queue of the specified thread.
 *
 *  @note Function SAL_Send() also can be used to achieve limited event 
 *        publication. When thread ID equals zero, SAL_Send() publishes 
 *        the event only in the specified application.
 *
 *  @param [in] app_id     ID of the application to send the message to 
 *  @param [in] thread_id  ID of the thread to send the message to 
 *  @param [in] event_id   ID of the message
 *  @param [in] data       pointer to message data
 *  @param [in] data_size  size of message data
 *
 *  @return                true on success, false on failure
 *
 *  @see SAL_Declare_Urgent(), SAL_Undeclare_Urgent(), SAL_Publish()
 */
bool_t SAL_Send(SAL_App_Id_T app_id, SAL_Thread_Id_T thread_id, SAL_Event_Id_T event_id, const void* data, size_t data_size);

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Retrieves a message from the queue of the specified thread.
 *
 *  SAL_Receive_From() is a special version of SAL_Receive().
 *  It is used to receive messages from the queue of another thread.
 *  It blocks if the queue is empty.
 *
 *  With this function, a number of threads can wait on a single queue,
 *  thus creating a thread pool/farm (then only one of the threads 
 *  should create the message queue).
 *
 * @note Thread owning the message queue must not exit or be destroyed 
 *       while threads receiving messages from its queue are running 
 *       (this thread must always be the last one to exit), otherwise 
 *       the behavior is undefined.
 *
 *  @param thread_id  ID of the thread owning the message queue
 *
 *  @return           pointer to the received message
 *
 *  @pre thread_id has to be in the range 1 to 
 *       max_number_of_threads (as declared by SAL_Init()).
 *
 *  @see SAL_Init(), SAL_Receive(), SAL_Send(), SAL_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent(), 
 *       SAL_Subscribe(), SAL_Unsubscribe()
 */
const SAL_Message_T* SAL_Receive_From(SAL_Thread_Id_T thread_id);

/**
 *  Function SAL_Try_Receive_From() retrieve a message from given
 *  thread's message queue if the queue is not empty. If the queue
 *  is empty, they return NULL.
 *
 *  @param thread_id  ID of the thread owning the message queue
 *
 *  @return pointer to the received message or NULL if queue is empty
 */
const SAL_Message_T* SAL_Try_Receive_From(SAL_Thread_Id_T thread_id);

/**
 *  Retrieves a message from the thread's queue.
 *
 *  Function blocks the calling thread until a message becomes available 
 *  in the thread�s message queue and returns a pointer to the message.
 *  The pointer is only valid until the next invocation of SAL_Receive().
 *  If the message must be retained for a longer time, it must be copied to
 *  a local buffer before SAL_Receive() is called again.
 *
 *  @return  pointer to the received message
 *
 *  @see SAL_Receive_From()
 */
const SAL_Message_T* SAL_Receive(void);

/**
 *  Functions SAL_Try_Receive() retrieve a message from the calling
 *  thread's queue if the message queue is not empty. If the queue
 *  is empty, they return NULL.
 *
 *  @return pointer to the received message or NULL if queue is empty
 */
const SAL_Message_T* SAL_Try_Receive(void);

/**
 *  Retrieves a message from the queue of the specified thread with a timeout.
 *
 *  Function blocks the calling thread until a message becomes available 
 *  or the timeout expires.
 *
 *  @param thread_id   ID of the thread owning the message queue
 *  @param timeout_ms  timeout value
 *
 *  @return  pointer to the received message or NULL in case of timeout
 *
 *  @see SAL_Receive_From()
 */
const SAL_Message_T* SAL_Receive_From_Timeout(SAL_Thread_Id_T thread_id, uint32_t timeout_ms);

#endif /* XSAL  && !XSAL_LIGHT_API */

/**
 *  Retrieves a selected message from the thread's queue.
 *
 *  Function blocks the calling thread until a message from the supplied list 
 *  becomes available in the message queue.
 *
 *  @return  pointer to the received message
 *
 *  @see SAL_Receive_From()
 */
const SAL_Message_T* SAL_Receive_Only(const SAL_Event_Id_T event_list[], uint32_t number_of_events);

/**
 *  Retrieves a selected message from the thread's queue it the queue 
 *  is not empty.
 *
 *  @return  pointer to the received message or NULL
 *
 *  @see SAL_Receive_Only()
 */
const SAL_Message_T* SAL_Try_Receive_Only(const SAL_Event_Id_T event_list[], uint32_t number_of_events);

/**
 *  Retrieves a message from the thread's queue with a timeout.
 *
 *  Function blocks the calling thread until a message becomes available 
 *  or the timeout expires.
 *
 *  @param timeout_ms  timeout value
 *
 *  @return  pointer to the received message or NULL in case of timeout
 *
 *  @see SAL_Receive_From()
 */
const SAL_Message_T* SAL_Receive_Timeout(uint32_t timeout_ms);

/**
 *  Retrieves a selected message from the thread's queue with a timeout.
 *
 *  Function blocks the calling thread until a message from the supplied list 
 *  becomes available in the message queue or the timeout expires.
 *
 *  @return  pointer to the received message or NULL in case of timeout
 *
 *  @see SAL_Receive_From()
 */
const SAL_Message_T* SAL_Receive_Only_Timeout(const SAL_Event_Id_T event_list[], uint32_t number_of_events, uint32_t timeout_ms);

/* -------------------------------------------------------------------------
 *
 *                              Event priorities
 *
 * -------------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Sets message priority to urgent. 
 *
 *  Urgent events are delivered before normal ones.
 * 
 *  @note Message priority is determined on thread level, i.e., threads
 *        may declare some or all messages as urgent.
 *        Message priority also applies to timer events.
 *
 *  @param [in] event_id_list     list of events
 *  @param [in] number_of_events  number of events on the list
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Undeclare_Urgent(), SAL_Send(), SAL_Publish()
 */
bool_t SAL_Declare_Urgent(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);

/**
 *  Sets message priority to normal.
 *
 *  @param [in] event_id_list     list of events
 *  @param [in] number_of_events  number of events on the list
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Declare_Urgent(), SAL_Send(), SAL_Publish()
 */
bool_t SAL_Undeclare_Urgent(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);

#endif /* XSAL  && !XSAL_LIGHT_API */

/* -------------------------------------------------------------------------
 *
 *                            Event subscribtion
 *
 * -------------------------------------------------------------------------*/

/**
 *  Subscribes the calling thread to published events.
 *
 *  Published events are delivered only to threads subscribed to them.
 *
 *  It is possible to call SAL_Subscribe() / SAL_Unsubscribe() 
 *  (@see SAL_Unsubscribe()) more than once 
 *  (and with different sets of events). The newly subscribed/unsubscribed 
 *  events will be added to/removed from the set of already subscribed ones.
 *
 *  @note Functions SAL_Declare_Urgent() and SAL_Undeclare_Urgent() 
 *        work with subscribed events too.
 *
 *  @param [in] event_id_list     list of events to subscribe to
 *  @param [in] number_of_events  number of events to subscribe to
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Unsubscribe(), SAL_Publish(), SAL_Local_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent()
 */
bool_t SAL_Subscribe(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);

/**
 *  Unsubscribes the calling thread from events.
 *
 *  @param [in] event_id_list     list of events to unsubscribe from
 *  @param [in] number_of_events  number of events to unsubscribe from
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Subscribe(), SAL_Publish(), SAL_Local_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent()
 */
bool_t SAL_Unsubscribe(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);

/* -------------------------------------------------------------------------
 *
 *                             Selective receive
 *
 * -------------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)

/**
 *  Select messages for receiving.
 *
 *  Only messages form the list will be received by functions: 
 *  SAL_Receive_Timeout(), SAL_Receive(), SAL_Receive_From(), ... .
 *  Other mesages arriving at the receiver thread queue are deffered, 
 *  i.e. saved in the thread's queue.
 *
 *  @note If SAL_Select() is called more than once, only the last call
 *        takes effect (i.e. it is not possible to select more messages
 *        in addition to the already selected ones by calling SAL_Select()
 *        again). It follows that calling SAL_Select() with an empty message
 *        list declares no messages as selected, i.e. unselects all
 *        previously selected messages.
 *
 *  @param [in] event_id_list     list of events
 *  @param [in] number_of_events  number of events on the list
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Unselect(), SAL_Receive()
 */
bool_t SAL_Select(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);

/**
 *  Resume all queued message in the thread's queue for receiving.
 *
 *  @return true on success, false on failure
 *
 *  @see SAL_Select()
 */
bool_t SAL_Unselect(void);

#endif /* XSAL  && !XSAL_LIGHT_API */

/* -------------------------------------------------------------------------
 *
 *                           Message publishing
 *
 * -------------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Deliveres an event to all subscribers in the calling application.
 *
 *  Function SAL_Send() can also be used to achieve "limited" event 
 *  publication:  When thread ID equals zero, SAL_Send() publishes 
 *  an event only in the specified application.
 *
 *  @param [in] event_id   event identifier
 *  @param [in] data       data to be delivered with the event
 *  @param [in] data_size  size of data
 *
 *  @see SAL_Subscribe(), SAL_Unsubscribe(), SAL_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent()
 */
void SAL_Local_Publish(SAL_Event_Id_T event_id, const void* data, size_t data_size);

#endif /* XSAL && !XSAL_LIGHT_API*/

/**
 *  Deliveres an event to all subscribers (in all applications 
 *  in the system) at once.
 *
 *  @param [in] event_id   event identifier
 *  @param [in] data       data to be delivered with the event
 *  @param [in] data_size  size of data
 *
 *  @see SAL_Subscribe(), SAL_Unsubscribe(), SAL_Local_Publish(), 
 *       SAL_Declare_Urgent(), SAL_Undeclare_Urgent()
 */
void SAL_Publish(SAL_Event_Id_T event_id, const void* data, size_t data_size);

/**
 * Returns the name of the specified event which can be useful for dbg_trace
 * purposes.
 *
 * @param [in] event_id The event id whose name is to be returned.
 *
 * @return Pointer to the const string with the name of the event.
 */
char_t const *SAL_Get_Event_Name(SAL_Event_Id_T event_id);

/* -------------------------------------------------------------------------
 *
 *                            Queue management
 *
 * -------------------------------------------------------------------------*/

#if defined (XSAL) && !defined(XSAL_LIGHT_API)
/**
 *  Returns an iterator to the first (i.e. oldest in terms of 
 *  arrival time) message in the queue of the calling thread.
 *
 *  @return  iterator to the first message
 */
void SAL_First_Message(SAL_Queue_Iterator_T* iterator);

/**
 *  Returns an iterator to the last message in the queue of 
 *  the calling thread
 *
 *  @return  iterator to the last message
 */
void SAL_Last_Message(SAL_Queue_Iterator_T* iterator);

/**
 *  Returns an iterator pointing to the end of the queue 
 *  (i.e. past the last message).
 *
 *  @return  iterator points to end of the queue
 */
void SAL_Queue_End(SAL_Queue_Iterator_T* iterator);

/**
 *  Takes an iterator as its argument and moves it to 
 *  the next message in the queue. 
 *
 *  @param [in, out] iterator  iterator to the message queue on input;
 *                             iterator pointing to the next message on output
 *
 *  @pre Iterator must not point to the last message in the queue
 */
void SAL_Next_Message(SAL_Queue_Iterator_T* iterator);

/**
 *  Takes an iterator as its argument and moves it to 
 *  the previous message in the queue. 
 *
 *  @param [in, out] iterator  iterator pointing to a message
 *
 *  @return  queue iterator pointing to the previous message
 *
 *  @pre Iterator must not point to the first message in the queue
 */
void SAL_Prev_Message(SAL_Queue_Iterator_T* iterator);

/**
 *  Removes the message pointed to by the iterator and moves 
 *  the iterator to the next message. 
 *
 *  @param [in,out] iterator  iterator to the message to be removed.
 *                            When function exits, the iterator points
 *                            to the next message
 *
 *  @pre Iterator must point to a message (not to queue end)
 */
void SAL_Remove_Message(SAL_Queue_Iterator_T* iterator);

/**
 *  Gets a pointer to the actual message pointed to by the iterator.
 *
 *  @param [in] iterator  iterator to a message
 *
 *  @return pointer to the message
 */
const SAL_Message_T* SAL_Get_Message(const SAL_Queue_Iterator_T* iterator);

/**
 *  Checks if its argument points to the first message in the queue.
 *
 *  @param [in] iterator  queue iterator
 *
 *  @return  true if iterator points to the first message in the queue
 */
bool_t SAL_Is_Queue_Begin(const SAL_Queue_Iterator_T* iterator);

/**
 *  Check if its argument points to the end of
 *  the queue (i.e. past the last message).
 *
 *  @param [in] iterator  queue iterator
 *
 *  @return  true if iterator points to the end of the queue
 */
bool_t SAL_Is_Queue_End(const SAL_Queue_Iterator_T* iterator);

/**
 * Function removes all messages from a thread�s message queue.
 *
 *  @see SAL_Purge_Only()
 */
void SAL_Purge(void);

/**
 * Function removes specified messages from a thread�s message queue.
 *
 *  @param [in] event_id_list     list of events to remove
 *  @param [in] number_of_events  number of events on the list
 *
 *  @see SAL_Purge()
 */
void SAL_Purge_Only(const SAL_Event_Id_T event_id_list[], uint32_t number_of_events);
#endif /* XSAL && !XSAL_LIGHT_API */

/** @} *//* end of SAL_Message_Grp */

/* -------------------------------------------------------------------------
 *
 *                           Timer management
 *
 * -------------------------------------------------------------------------*/

/** @defgroup SAL_Timer_Grp Timer
 *  @{
 */

/**
 *  Creates a timer sending an event upon expiration.
 *  After creation, the timer is not running yet. 
 *  It is started with a call to SAL_Start_Timer().
 *
 *  @note XSAL timer events are sent only to the thread that has created 
 *        the timer. Threads do not have to subscribe to timer events to 
 *        receive them. Timer events do not carry any data, only event ID. 
 *        Timers can be single-shot or periodic. 
 *
 *  @note Only the thread that created the timer may start 
 *        (by calling SAL_Start_Timer()), stop (by calling SAL_Stop_Timer()) 
 *        or destroy it (by calling SAL_Destroy_Timer()).
 *        In other words, timers are local to threads creating them.
 *
 *  @param [in]  event_id  ID of event which is sent upon timer expiration
 *  @param [out] timer_id  ID of the timer
 *
 *  @return true on succes, false on failure
 *
 *  @see SAL_Start_Timer(), SAL_Stop_Timer(), SAL_Destroy_Timer(), 
 *       SAL_Destroy_Thread()
 */
bool_t SAL_Create_Timer(SAL_Event_Id_T event_id, SAL_Timer_Id_T* timer_id);

/**
 *  Destroys the timer identified by timer_id.
 *
 *  @note Events generated by a timer are not removed from 
 *        the thread's message queue when the timer is destroyed. 
 *
 *  @param [in] timer_id  ID of the timer to be destroyed
 *
 *  @see SAL_Create_Timer(), SAL_Stop_Timer(), SAL_Destroy_Thread()
 */
void SAL_Destroy_Timer(SAL_Timer_Id_T timer_id);

/**
 *  Starts the timer identified by timer_id.
 *  Timer interval is given in milliseconds but timer resolution 
 *  depends on the underlying operating system. 
 *
 *  Parameter is_periodic defines the timer as periodic or single-shot.
 *
 *  @param [in] timer_id       ID of the timer to be started
 *  @param [in] interval_msec  timer interval
 *  @param [in] is_periodic    timer mode: single-shot if false, 
 *                             periodic otherwise
 *
 *  @see SAL_Create_Timer(), SAL_Stop_Timer(), SAL_Destroy_Timer()
 */
void SAL_Start_Timer(SAL_Timer_Id_T timer_id, uint32_t interval_msec, bool_t is_periodic);

/**
 *  Starts the timer identified by timer_id.
 *  Timer interval is given in milliseconds but timer resolution
 *  depends on the underlying operating system.
 *
 *  Parameter is_periodic defines the timer as periodic or single-shot.
 *
 *  Parameter param is included in the message generated by the timer and
 *  delivered to the receiver as the messages data (so it can be accessed
 *  by the recipient of the message in a standard way, i.e. through
 *  SAL_Message_Ts data field).
 *
 *  @param [in] timer_id       ID of the timer to be started
 *  @param [in] interval_msec  timer interval
 *  @param [in] is_periodic    timer mode: single-shot if false,
 *                             periodic otherwise
 *  @param [in] param          parameter passed to SAL_Message_T when timer
 *                             expired
 *
 *  @see SAL_Create_Timer(), SAL_Start_Timer(), SAL_Stop_Timer(),
 *       SAL_Destroy_Timer()
 */
void SAL_Start_Timer_Ex(SAL_Timer_Id_T timer_id, uint32_t interval_msec, bool_t is_periodic, uintptr_t param);

/**
 *  Stops the timer identified by timer_id.
 *
 *  @param [in] timer_id  ID of the timer to be stopped
 *
 *  @note Events generated by a timer are not removed from 
 *        the thread's message queue when the timer is stopped. 
 *
 *  @see SAL_Create_Timer(), SAL_Start_Timer(), SAL_Destroy_Timer()
 */
void SAL_Stop_Timer(SAL_Timer_Id_T timer_id);

/**
 * Returns program execution time in mS
 *    Works similar to C standard clock function but always return mS 
 *
 * @return Milliseconds since program startup
 */
SAL_Clock_T SAL_Clock(void);

/**
 * Returns elapsed time since / until timestamp 
 *
 * @param timestamp - Reference point for the delta time
 *
 * @return delta milliseconds since timestamp (answer is negative if  timestamp
 *               is in future)
 *
 * @note 
 *    - The elapsed time will return the correct delta even if SAL_Clock has
 *      wraparound
 *    - Actual delta will be +/- 1 mS of reported value since timestamps are
 *      asychronous to actual clock.
 *
 * @see SAL_Clock
 */
SAL_Int_T SAL_Elapsed_Time(SAL_Clock_T timestamp);

/**
 * Returns delta time between two timestamps
 *
 * @param start_time - Reference point for start of delta time
 * @param end_time - Reference point end of delta time
 *
 * @return delta milliseconds between start and end times (end_time - start_time)
 *
 * @note 
 *    - The elapsed time will return the correct delta even if SAL_Clock has
 *      wraparound
 *    - Actual delta will be +/- 1 mS of reported value since timestamps are
 *      asychronous to actual clock.
 *
 * @see SAL_Clock
 */
SAL_Int_T SAL_Delta_Time(SAL_Clock_T start_time, SAL_Clock_T end_time);

/** @} *//* end of SAL_Timer_Grp */

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file xsal.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
 *    - Initial revision.
 *
 * - 23-Mar-2007 Dan Carman
 *    - Made SAL_Destroy_Buffer_Pool valid for both XSAL and XSAL Light
 *
 * - 18-Apr-2007 Dan Carman
 *    - Added SAL_Clock function to remove widespread dependencies on time.h
 *    - Added Limbo count Queue statistics for receive only
 *    - SAL_Message_T - Removed unused fields (app id, etc.) from XSAL Light
 *    - version. Change XSAL Light timestamp to single mS field compatible with
 *    - SAL_Clock
 *
 * - 01-May-2007 Dan Carman
 *    Added SAL_Elasped_Time for standard time delay measurements.
 *
 * - 24-May-2007 kirk bailey
 *    - Made compatible with version 11 for XSAL from TCK.
 *    - Fixed spelling of "Elasped".
 *
 * - 27-Sept-2007 Dan Carman
 *    - Changed to SAL_Clock_T type as return from SAL_Clock()
 *    - Update with changes from xsal.h version tck_mmm2#10 in DB48x.
 *    - MISRA change (char -> char_t)
 *
 * - 19-mar-2008 kirk bailey
 *    - Added conditional compilation since callbacks not yet supported by
 *      full XSAL.
 *
 * - 27-mar-2008 kirk bailey
 *    - Merged latest DB48x changes with uItron version.
 *
 * - 02-jun-2008 kirk bailey
 *    - Added XSAL_LIGHT_API conditional compilation logic that allows clients
 *      of XSAL to be restricted to using the XSAL Light API.
 *    - Added SAL_Alloc, SAL_Free to all XSAL implementations.
 *    - Added SAL_Clock, SAL_Delta_Time to all XSAL implementations.
 *
 * - 10-Mar-2010 Dan Carman
 *    - Update  to TCK XSAL version 18
 *
 * - 24-Jul-2010 Kirk Bailey
 *    - Change "bool" to "bool_t".
 *
 * - 24-Jan-2011 kirk bailey
 *   - Added new name server functionality for app and dbg_trace module data.
 *
 * - 03-Jun-2011 kirk bailey
 *   - Added SAL_Get_App_Name().
 *
 * - 30-Aug-2012 Dan Carman
 *    -Added SAL_Start_Timer_Ex
 *
 * - 17-Apr-2014 Coppock, Jonathan (dzhk56) Rev kok_basa#20
 *   - Task kok_basa#181044: "SBX MY15 - dbg_ctrl & xsal: Add the queue stat 
 *     DeltaRcv to dbg_ctrl and xsal"
 *     + Add the queue stat DeltaRcv to dbg_ctrl and xsal.
 *
 * 14-Apr-2015 Dan Carman
 *    - Task kok_basa#207158
 *    Defined separate type, SAL_Event_Id_T, for passing Event IDs from the
 *    enumerated type of SAL_Event_Id_T for compatibility with ISA code.
 *
 * 27-May-2015 Kirk Bailey
 *    - Filled in unresolved comments.
 *
 * 20-Jul-2015 Kirk Bailey
 *    - Added SAL_Purge() calls.
 *
 * 18-Dec-2015 Kirk Bailey
 *    - Changed size_t to uint32_t for arguments that are truly "counts", not
 *      sizes. This is to address Coverity SIZEOF_MISMATCH issues.
 */
/*===========================================================================*/
#endif                          /* XSAL_H */
