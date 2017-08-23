/* -------------------------------------------------------------------------
 *
 * File:        test_mutex.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of mutexes.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 2);

#define APP_ID 2

typedef enum Thread_Id_Tag
{
   MAIN_THREAD = PM_LAST_THREAD_ID,
   LOCK1_THREAD,
   LOCK2_THREAD,
   LOCK3_THREAD,
   LOCK4_THREAD,
   LOCK5_THREAD,
   LOCK6_THREAD,
   UNLOCK1_THREAD,
   UNLOCK2_THREAD,
   TRYLOCK1_THREAD,
   TRYLOCK2_THREAD,
   REC_THREAD,
   MAX_NUMB_OF_THREADS
} Thread_Id_T;

typedef enum Test_Nr_Tag
{
   LONG_TIMEOUT = 1, SHORT_TIMEOUT
} Test_Nr_T;

typedef struct Param_Tag
{
   Test_Nr_T test_nr;
   SAL_Thread_Id_T destroyed_list[2];
   size_t list_size;
} Param_T;

Param_T Par;

#ifdef SAL_MUTEX_INITIALIZER
SAL_Mutex_T Mutex = SAL_MUTEX_INITIALIZER;
#else
SAL_Mutex_T Mutex;
#endif

short Flag = 0;
SAL_Int_T Long_Timeout = 3000;

int Share_Lock_Count = 0;

bool_t test_failed = false;


/**  \file
 *  
 *   Test of all mutex functions.
 *  
 *   \section T Tests:
 *   <OL>
 *   <LI> \ref TLMT1 - test of SAL_Lock_Mutex_Timeout and SAL_Unlock_Mutex functions 
 *                     (long timeout). </LI>
 *   <LI> \ref TLMT2 - test of SAL_Lock_Mutex_Timeout and SAL_Unlock_Mutex functions
 *                     (short timeout). </LI>
 *   <LI> \ref LMT - test of SAL_Lock_Mutex and SAL_Unlock_Mutex functions (<B><I>XSAL only</I></B>). </LI>
 *   <LI> \ref TLMT - test of SAL_Try_Lock_Mutex function. </LI>
 *   <LI> \ref FRMT - with parameter 0 - test of recursive mutex using SAL_Lock_Mutex function. </LI>
 *   <LI> \ref SRMT - with parameter LONG_TIMEOUT - test of recursive mutex using SAL_Lock_Mutex_Timeout function. </LI>
 *   <LI> \ref TRMT - with parameter SHORT_TIMEOUT - test of recursive mutex using SAL_Lock_Mutex_Timeout function. </LI>
 *   <LI> \ref RMTT - test of recursive mutex using SAL_Try_Lock_Mutex function. </LI>
 *   <LI> Functions of SAL_Create_Mutex and SAL_Destroy_Mutex are tested in \b Mutex_Test_Suite
 *      function before and after calling tests above. </LI>
 *   </OL>
 *
 *   \section TLMT1 Lock_Mutex_Timeout_Test_1
 *
 *   Two threads are created: LOCK1_THREAD and LOCK2_THREAD. \n
 *   Unlocking the mutex by LOCK1_THREAD takes place earlier than timeout of
 *   SAL_Lock_Mutex_Timeout is reached in LOCK2_THREAD (timeout is 3000).
 *
 *   \subsection TP1 Test procedure:
 *     
 *   <OL>
 *   <LI> LOCK1_THREAD locks the mutex. </LI>
 *   <LI> LOCK2_THREAD waits for signal that LOCK1_THREAD is ready. </LI>
 *   <LI> LOCK1_THREAD signals that is ready. </LI>
 *   <LI> LOCK2_THREAD tries to lock the locked mutex with timeout 2000, finally should lock it. </LI>
 *   <LI> LOCK1_THREAD sleeps for 1000 ms. </LI>
 *   <LI> LOCK1_THREAD checks if LOCK2_THREAD is suspended. </LI>
 *   <LI> LOCK1_THREAD unlocks the mutex. </LI>
 *   <LI> LOCK1_THREAD sleeps for 1000 ms. </LI>
 *   <LI> LOCK2_THREAD unlocks the mutex. </LI>
 *   <LI> LOCK1_THREAD checks if LOCK2_TRHREAD has unlocked the mutex. </LI>
 *   </OL>
 *
 *   \section TLMT2 Lock_Mutex_Timeout_Test_2
 *
 *   Two threads are created: LOCK1_THREAD and LOCK2_THREAD. \n
 *   Timeout of SAL_Lock_Mutex_Timeout is short (100 ms) therefore timeout in LOCK2_THREAD is reached
 *   earlier than unlocking the mutex by LOCK1_THREAD.
 *   
 *   \subsection TP2 Test procedure:
 *   
 *   <OL>
 *   <LI> LOCK1_THREAD locks the mutex. </LI>
 *   <LI> LOCK2_THREAD tries to lock the locked mutex with timeout 100 ms, finally should reach 
 *        the timeout. </LI>
 *   <LI> LOCK1_THREAD checks if LOCK2_THREAD is alive. </LI>
 *   <LI> LOCK1_THREAD unlocks the mutex. </LI>
 *   </OL>
 *
 *   \section LMT Lock_Mutex_Test
 *
 *   Two threads are created: LOCK1_THREAD and LOCK2_THREAD.
 *
 *   \subsection TP3 Test procedure:
 *     
 *   <OL>
 *   <LI> LOCK1_THREAD locks the mutex. </LI>
 *   <LI> LOCK2_THREAD tries to lock the locked mutex. </LI>
 *   <LI> LOCK1_THREAD checks if LOCK2_THREAD is suspended. </LI>
 *   <LI> LOCK1_THREAD unlocks the mutex. </LI>
 *   <LI> LOCK2_THREAD unlocks the mutex. </LI>
 *   <LI> LOCK1_THREAD checks if LOCK2_TRHREAD has unlocked the mutex. </LI>
 *   </OL>
 *
 *   \section TLMT Try_Lock_Mutex_Test
 *
 *   Two threads are created: LOCK1_THREAD and LOCK2_THREAD.
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> LOCK1_THREAD tries to lock the mutex. </LI>
 *   <LI> LOCK2_THREAD tries to lock the mutex which has been already locked by LOCK1_THREAD. </LI>
 *   <LI> LOCK1_THREAD unlocks the mutex. </LI>
 *   <LI> LOCK2_THREAD tries to lock the mutex and terminates. </LI>
 *   </OL>
 *
 *   \section FRMT the first Recursive_Mutex_Test
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates four threads: 
 *   - REC_THREAD
 *   - after 100 msec creates LOCK1_THREAD
 *   - after 100 msec creates LOCK2_THREAD
 *   - after 100 msec creates LOCK3_THREAD
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> REC_THREAD locks mutex. </LI>
 *   <LI> REC_THREAD sleeps for 150 msec. </LI>
 *   <LI> LOCK1_THREAD tries to lock mutex. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD is suspended. </LI>
 *   <LI> REC_THREAD locks mutex for the second time. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> LOCK2_THREAD tries to lock mutex. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD and LOCK2_THREAD are suspended. </LI>
 *   <LI> REC_THREAD locks mutex for the third time. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> LOCK3_THREAD tries to lock mutex. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD, LOCK2_THREAD and LOCK3_THREAD are suspended. </LI>
 *   <LI> REC_THREAD unlocks mutex for the first time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD, LOCK2_THREAD and LOCK3_THREAD are suspended. </LI>
 *   <LI> REC_THREAD unlocks mutex for the second time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD, LOCK2_THREAD and LOCK3_THREAD are suspended. </LI>
 *   <LI> REC_THREAD unlocks mutex for the third time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> LOCK1_THREAD resumes, locks and unlocks mutex. </LI>
 *   <LI> LOCK2_THREAD resumes, locks and unlocks mutex. </LI>
 *   <LI> LOCK3_THREAD resumes, locks and unlocks mutex. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD, LOCK2_THREAD and LOCK3_THREAD have resumed and terminated. </LI>
 *   </OL>
 *
 *   \section SRMT the second Recursive_Mutex_Test
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates two threads: 
 *   - REC_THREAD
 *   - after 100 msec creates LOCK1_THREAD
 *
 *   LOCK1_THREAD tries to lock mutex with long timeout.\n
 *   Mutex is locked before timeout is reached.
 *
 *   \subsection TP5 Test procedure:
 *
 *   <OL>
 *   <LI> REC_THREAD locks mutex with timeout 3000 msec. </LI>
 *   <LI> REC_THREAD sleeps for 150 msec. </LI>
 *   <LI> LOCK1_THREAD tries to lock mutex with timeout 3000 msec. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD is suspended. </LI>
 *   <LI> REC_THREAD locks mutex for the second time with timeout 3000 msec. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD is suspended. </LI>
 *   <LI> REC_THREAD locks mutex for the third time with timeout 3000 msec. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD is suspended. </LI>
 *   <LI> REC_THREAD unlocks mutex for the first time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD is suspended. </LI>
 *   <LI> REC_THREAD unlocks mutex for the second time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD is suspended. </LI>
 *   <LI> REC_THREAD unlocks mutex for the third time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> LOCK1_THREAD resumes, locks, unlocks mutex and terminates. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD has been terminated. </LI>
 *   </OL>
 *
 *   \section TRMT the third Recursive_Mutex_Test
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates two threads: 
 *   - REC_THREAD
 *   - after 100 msec creates LOCK1_THREAD
 *
 *   LOCK1_THREAD tries to lock mutex with short timeout.\n
 *   Timeout is reached before mutex is unlocked.
 *
 *   \subsection TP6 Test procedure:
 *
 *   <OL>
 *   <LI> REC_THREAD locks mutex with timeout 3000 msec. </LI>
 *   <LI> REC_THREAD sleeps for 150 msec. </LI>
 *   <LI> LOCK1_THREAD tries to lock mutex with timeout 100 msec and terminates. </LI>  
 *   <LI> REC_THREAD locks mutex for the second time with timeout 3000 msec. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> REC_THREAD locks mutex for the third time with timeout 3000 msec. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> REC_THREAD unlocks mutex for the first time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> REC_THREAD unlocks mutex for the second time. </LI>
 *   <LI> REC_THREAD sleeps for 50 msec. </LI>
 *   <LI> REC_THREAD unlocks mutex for the third time. </LI>
 *   </OL>
 *
 *   \section RMTT Recursive_Mutex_Try_Test
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates seven threads: 
 *   - REC_THREAD
 *   - after 100 msec creates LOCK1_THREAD
 *   - after 100 msec creates LOCK2_THREAD
 *   - after 100 msec creates LOCK3_THREAD
 *   - after 100 msec creates LOCK4_THREAD
 *   - after 100 msec creates LOCK5_THREAD
 *   - after 100 msec creates LOCK6_THREAD
 *
 *   \subsection TP7 Test procedure:
 *
 *   <OL>
 *   <LI> REC_THREAD locks mutex. </LI>
 *   <LI> REC_THREAD sleeps for 150 msec. </LI>
 *   <LI> LOCK1_THREAD tries to lock mutex which has been already locked by REC_THREAD. </LI>
 *   <LI> REC_THREAD checks if LOCK1_THREAD has been terminated. </LI>
 *   <LI> REC_THREAD locks mutex for the second time. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> LOCK2_THREAD tries to lock mutex locked by REC_THREAD. </LI>
 *   <LI> REC_THREAD checks if LOCK2_THREAD has been terminated. </LI>
 *   <LI> REC_THREAD locks mutex for the third time. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> LOCK3_THREAD tries to lock mutex locked by REC_THREAD. </LI>
 *   <LI> REC_THREAD checks if LOCK3_THREAD has been terminated. </LI>
 *   <LI> REC_THREAD unlocks mutex for the first time. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> LOCK4_THREAD tries to lock mutex locked by REC_THREAD. </LI>
 *   <LI> REC_THREAD checks if LOCK4_THREAD has been terminated. </LI>
 *   <LI> REC_THREAD unlocks mutex for the second time. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> LOCK5_THREAD tries to lock mutex locked by REC_THREAD. </LI>
 *   <LI> REC_THREAD checks if LOCK5_THREAD has been terminated. </LI>
 *   <LI> REC_THREAD unlocks mutex for the third time. </LI>
 *   <LI> REC_THREAD sleeps for 100 msec. </LI>
 *   <LI> LOCK6_THREAD locks mutex. </LI>
 *   <LI> LOCK6_THREAD unlocks mutex. </LI>
 *   <LI> REC_THREAD checks if LOCK6_THREAD has been terminated. </LI>
 *   </OL>
 *
 *
 */

const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, SAL_Thread_Id_T id, SAL_Priority_T prior, SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}

/**  \name Functions of Lock_Mutex_Timeout_Test_1
 *
 *   \{
 */

/**  Function of LOCK1_THREAD.
 *
 *   - locks the mutex
 *   - unlocks the mutex
 */

void Lock_Timeout_Fun_11(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Lock_Timeout_Fun_11: Start");

   time_1 = SAL_Clock();

   /* lock the mutex */
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T) (time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         Tr_Fault_1("SAL_Lock_Mutex_Timeout: suspends the thread for %d mS", delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed_1("SAL_Lock_Mutex_Timeout: thread doesn't lock the mutex Delta Time %d", delta_time);
   }

   SAL_Signal_Ready();

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD thread is suspended */
   if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      PBC_Failed("SAL_Lock_Mutex_Timeout: LOCK2_THREAD has not been suspended");
   }

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD unlocked the mutex */
   if (SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      PBC_Failed("SAL_Unlock_Mutex: Doesn't unlock the mutex");
   }

   Tr_Info_Lo("Lock_Timeout_Fun_11: Stop");
}

/**  Function of LOCK2_THREAD.
 *
 *   - tries to lock the mutex which is already locked
 *   - finally locks the mutex because timeout is long enough (first mutex in LOCK1_THREAD
 *     is unlocked)
 *   - unlocks the mutex
 */

void Lock_Timeout_Fun_12(void* param)
{
   static SAL_Thread_Id_T wait_list[] =
   { LOCK1_THREAD };
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Lock_Timeout_Fun_12: Start");

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   time_1 = SAL_Clock();

   /* try to lock the mutex which is locked by LOCK1_THREAD */
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, 2000);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T) (time_2 - time_1);

   res_time = delta_time - 1000;

   if (ret_timeout)
   {
      if (abs(res_time) > 25)
      {
         PBC_Failed_1("SAL_Lock_Mutex_Timeout: error: thread has been suspended for %d ms, expected time: 1000 ms", (int)delta_time);
      }
      if (abs(res_time) > 10)
      {
         Tr_Fault_1("SAL_Lock_Mutex_Timeout: warning: thread has been suspended for %d ms, expected time: 1000 ms", (int)delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex");
   }

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   Tr_Info_Lo("Lock_Timeout_Fun_12: Stop");
}

/**  Test of SAL_Lock_Mutex_Timeout and SAL_Unlock_Mutex functions.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Lock_Mutex_Timeout_Test_1()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { LOCK1_THREAD, LOCK2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Lock_Timeout_Fun_11, NULL, Init_Thread_Attr("LOCK1", LOCK1_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Lock_Timeout_Fun_12, NULL, Init_Thread_Attr("LOCK2", LOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;

}

/**  \}
 */

/**  \name Functions of Lock_Mutex_Timeout_Test_2
 *
 *   \{
 */

/**  Function of LOCK1_THREAD.
 *
 *   - locks the mutex
 *   - unlocks the mutex
 */

void Lock_Timeout_Fun_21(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Lock_Timeout_Fun_21: Start");

   time_1 = SAL_Clock();

   /* lock the mutex */
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T) (time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         PBC_Failed("SAL_Lock_Mutex_Timeout: suspends the thread");
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_Lock_Mutex_Timeout: thread doesn't lock the mutex");
   }

   SAL_Signal_Ready();

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD has terminated */
   if (SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      PBC_Failed("SAL_Lock_Mutex_Timeout: Thread has not been suspended");
   }

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   Tr_Info_Lo("Lock_Timeout_Fun_21: Stop");
}

/**  Function of LOCK2_THREAD.
 *
 *   - tries to lock the mutex which is already locked
 *   - finally doesn't lock the mutex because timeout is short (timeout is reached
 *     before mutex in LOCK1_THREAD is unlocked)
 *   - unlock the mutex
 */

void Lock_Timeout_Fun_22(void* param)
{
   static SAL_Thread_Id_T wait_list[] =
   { LOCK1_THREAD };
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Lock_Timeout_Fun_22: Start");

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   time_1 = SAL_Clock();

   /* try to lock the mutex which is locked by LOCK1_THREAD */
   ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, 100);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T) (time_2 - time_1);

   res_time = delta_time - 100;

   if (!ret_timeout)
   {
      if (abs(res_time) > 20)
      {
         PBC_Failed_1("SAL_Lock_Mutex_Timeout: error: invalid interval of timeout, expected: 100, achieved: %d", (int)delta_time);
      }
      if (abs(res_time) > 10)
      {
         Tr_Fault_1("SAL_Lock_Mutex_Timeout: warning: invalid interval of timeout, expected: 100, achieved: %d", (int)delta_time);
      }
   }
   else /* ret_timeout */
   {
      PBC_Failed("SAL_Lock_Mutex_Timeout: returns 1");
   }

   Tr_Info_Lo("Lock_Timeout_Fun_22: Stop");
}

/**  Test of SAL_Lock_Mutex and SAL_Unlock_Mutex functions.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Lock_Mutex_Timeout_Test_2()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { LOCK1_THREAD, LOCK2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Lock_Timeout_Fun_21, NULL, Init_Thread_Attr("LOCK1", LOCK1_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Lock_Timeout_Fun_22, NULL, Init_Thread_Attr("LOCK2", LOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;

}

/** \}
 */

#if defined (XSAL)

/**  \name Functions of Lock_Mutex_Test
 *
 *   \{
 */

/**  Function of LOCK1_THREAD.
 *
 *   - locks the mutex
 *   - unlocks the mutex
 */

void Lock_Fun_1(void* param)
{

   Tr_Info_Lo("Lock_Fun_1: Start");

   /* lock the mutex */
   if (!SAL_Lock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Lock_Mutex: Thread doesn't lock the mutex");
   }

   SAL_Signal_Ready();

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD is suspended */
   if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      PBC_Failed("SAL_Lock_Mutex: Thread has not been suspended");
   }

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   SAL_Sleep(1000);

   /* check if LOCK2_THREAD unlocked the mutex */
   if (SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      PBC_Failed("SAL_Unlock_Mutex: Doesn't unlock the mutex");
   }

   Tr_Info_Lo("Lock_Fun_1: Stop");
}

/**  Function of LOCK2_THREAD.
 *
 *   - tries to lock the mutex which is already locked
 *   - unlocks the mutex
 */

void Lock_Fun_2(void* param)
{
   static SAL_Thread_Id_T wait_list[] =
   { LOCK1_THREAD };

   Tr_Info_Lo("Lock_Fun_2: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

   /* try to lock the mutex which is locked by LOCK1_THREAD */
   SAL_Lock_Mutex(&Mutex);

   /* unlock the mutex */
   SAL_Unlock_Mutex(&Mutex);

   Tr_Info_Lo("Lock_Fun_2: Stop");
}

/**  Test of SAL_Lock_Mutex and SAL_Unlock_Mutex functions.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Lock_Mutex_Test()
{
   static SAL_Thread_Id_T destroyed_list[] =
   { LOCK1_THREAD, LOCK2_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Lock_Fun_1, NULL, Init_Thread_Attr("LOCK1", LOCK1_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Lock_Fun_2, NULL, Init_Thread_Attr("LOCK2", LOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

#endif /* XSAL */

/**  \name Functions of Try_Lock_Mutex_Test
 *
 *   \{
 */

/**  Function of TRYLOCK1_MUTEX.
 *
 *   - locks the mutex
 *   - unlocks the mutex
 */

void Try_Lock_Fun_1(void* param)
{
   static SAL_Thread_Id_T wait_list[] =
   { TRYLOCK2_THREAD };

   Tr_Info_Lo("Try_Lock_Fun_1: Start");

   /* try to lock the mutex */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Try_Lock_Mutex: Thread doesn't lock the mutex");
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   /* unlock the mutex */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   Flag = 1;

   Tr_Info_Lo("Try_Lock_Fun_1: Stop");
}

/**  Function of TRYLOCK2_THREAD.
 *   
 *   - tries to lock the mutex which is already locked
 *   - tries to lock the mutex which has been unlocked
 */

void Try_Lock_Fun_2(void* param)
{
   static SAL_Thread_Id_T wait_list[] =
   { TRYLOCK1_THREAD };

   Tr_Info_Lo("Try_Lock_Fun_2: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   /* try to lock the mutex which is already locked by TRYLOCK1_THREAD */
   if (SAL_Try_Lock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Try_Lock_Mutex: returns 1 when mutex is locked");
   }

   SAL_Signal_Ready();

   while (!Flag)
   {
      SAL_Sleep(10);
   }

   /* try to lock the mutex which has been unlocked by TRYLOCK1_THREAD */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Try_Lock_Mutex: doesn't lock the mutex after it has been unlocked");
   }

   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: can't unlock mutex");
   }

   Tr_Info_Lo("Try_Lock_Fun_2: Stop");
}

/**  Tests the SAL_Try_Lock_Mutex function.
 *
 *   Creates the threads and starts SAL_Run.
 */

SAL_Int_T Try_Lock_Mutex_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret;
   SAL_Thread_Id_T destroyed_list[] =
   { TRYLOCK1_THREAD, TRYLOCK2_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Try_Lock_Fun_1, NULL, Init_Thread_Attr("TRY1_T", TRYLOCK1_THREAD, priority, &thread_attr));

   SAL_Create_Thread(Try_Lock_Fun_2, NULL, Init_Thread_Attr("TRY2_T", TRYLOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;

}

/**  \}
 */

/**  \name Functions of Recursive_Mutex_Test
 *
 *   \{
 */

/**  Function of REC_THREAD.
 *
 *   - locks mutex three times
 *   - checks if another threads are suspended (waiting for unlocking mutex)
 *   - unlocks the mutex three times
 *   - checks if another threads are suspended until recursive mutex is unlocked the last time
 */

void Recursive_Lock_Fun(void* param)
{
   Test_Nr_T test_nr = (Test_Nr_T) param;

   Tr_Info_Lo_1("Recursive_Lock_Fun, Timeout: %d  Start", (int)test_nr);

   /* lock mutex for the first time */

#if defined (XSAL)
   if (test_nr == 0)
   {
      if (!SAL_Lock_Mutex(&Mutex))
      {
         PBC_Failed("SAL_Lock_Mutex: Thread doesn't lock the mutex");
      }
   }
#endif

   if (test_nr != 0)
   {
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {
         PBC_Failed("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex");
      }
   }

   SAL_Sleep(150);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         PBC_Failed("LOCK1_THREAD is not suspended");
      }
   }

   /* lock mutex for the second time */
#if defined (XSAL)
   if (test_nr == 0)
   {
      if (!SAL_Lock_Mutex(&Mutex))
      {
         PBC_Failed("SAL_Lock_Mutex: Thread doesn't lock the mutex for the second time");
      }
   }
#endif

   if (test_nr != 0)
   {
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {
         PBC_Failed("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex for the second time");
      }
   }

   SAL_Sleep(100);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         PBC_Failed("LOCK1_THREAD is not suspended");
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         PBC_Failed("LOCK2_THREAD is not suspended");
      }
   }

#if defined (XSAL)

   /* lock mutex for the third time */

   if (test_nr == 0)
   {
      if (!SAL_Lock_Mutex(&Mutex))
      {
         PBC_Failed("SAL_Lock_Mutex: Thread doesn't lock the mutex for the third time");
      }
   }
#endif

   if (test_nr != 0)
   {
      if (!SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout))
      {
         PBC_Failed("SAL_Lock_Mutex_Timeout: Thread doesn't lock the mutex for the third time");
      }
   }

   SAL_Sleep(100);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         PBC_Failed("LOCK1_THREAD is not suspended");
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         PBC_Failed("LOCK2_THREAD is not suspended");
      }

      if (!SAL_Is_Thread_Alive(LOCK3_THREAD))
      {
         PBC_Failed("LOCK3_THREAD is not suspended");
      }
   }

   /* unlock mutex for the first time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   SAL_Sleep(50);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         PBC_Failed("LOCK1_THREAD is not suspended");
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         PBC_Failed("LOCK2_THREAD is not suspended");
      }

      if (!SAL_Is_Thread_Alive(LOCK3_THREAD))
      {
         PBC_Failed("LOCK3_THREAD is not suspended");
      }
   }

   /* unlock mutex for the second time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   SAL_Sleep(50);

   if (test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         PBC_Failed("LOCK1_THREAD is not suspended");
      }
   }

   if (test_nr == 0)
   {
      if (!SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         PBC_Failed("LOCK2_THREAD is not suspended");
      }

      if (!SAL_Is_Thread_Alive(LOCK3_THREAD))
      {
         PBC_Failed("LOCK3_THREAD is not suspended");
      }
   }

   /* unlock mutex for the third time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   if (test_nr != SHORT_TIMEOUT)
   {
      SAL_Sleep(50);

      if (SAL_Is_Thread_Alive(LOCK1_THREAD))
      {
         PBC_Failed("LOCK1_THREAD has not been terminated");
      }
   }

   if (test_nr == 0)
   {
      if (SAL_Is_Thread_Alive(LOCK2_THREAD))
      {
         PBC_Failed("LOCK2_THREAD has not been terminated");
      }

      if (SAL_Is_Thread_Alive(LOCK3_THREAD))
      {
         PBC_Failed("LOCK3_THREAD has not been terminated");
      }
   }

   Tr_Info_Lo_1("Recursive_Lock_Fun, Timeout: %d  Stop", (int)test_nr);
}

/**  Function of LOCK1_THREAD, LOCK2_THREAD and LOCK3_THREAD.
 *
 *   - locks mutex
 *   - unlocks mutex
 */

void Lock_Fun(void* param)
{
   SAL_Clock_T in_time_1, in_time_2;
   SAL_Int_T in_delta_time, in_theor_time, in_res_time;
   Param_T par = *(Param_T*) param;
   bool_t ret_timeout;

   Tr_Info_Lo_1("Lock_Fun, Timeout: %d Start", (int)par.test_nr);

#if defined (XSAL)
   if (par.test_nr == 0)
   {
      if (!SAL_Lock_Mutex(&Mutex))
      {
         PBC_Failed("SAL_Lock_Mutex: Thread locks mutex which has been already locked");
      }
   }
#endif

   if (par.test_nr == LONG_TIMEOUT)
   {
      in_time_1 = SAL_Clock();

      /* try to lock mutex which is locked by REC_THREAD */
      ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, Long_Timeout);

      in_time_2 = SAL_Clock();

      in_delta_time = (SAL_Int_T) (in_time_2 - in_time_1);

      in_theor_time = 350;

      in_res_time = in_delta_time - in_theor_time;

      if (ret_timeout)
      {
         if (abs(in_res_time) > 60)
         {
            PBC_Failed_2("SAL_Lock_Mutex_Timeout: error: thread has been suspended for %d ms, expected time: %d ms", (int)in_res_time, (int)in_theor_time);
         }
         else if (abs(in_res_time) > 50)
         {
            Tr_Fault_2("SAL_Lock_Mutex_Timeout: warning: thread has been suspended for %d ms, expected time: %d ms", (int)in_res_time, (int)in_theor_time);
         }
         else
         {
         }
      }
      else /* !ret_timeout */
      {
         PBC_Failed("SAL_Lock_Mutex_Timeout: Thread doesn't lock mutex");
      }

   } /* if test_nr == LONG_TIMEOUT */
   else if (par.test_nr == SHORT_TIMEOUT)
   {
      in_time_1 = SAL_Clock();

      /* try to lock mutex which is locked by REC_THREAD */
      ret_timeout = SAL_Lock_Mutex_Timeout(&Mutex, 100);

      in_time_2 = SAL_Clock();

      in_delta_time = (SAL_Int_T) (in_time_2 - in_time_1);

      in_res_time = in_delta_time - 100;

      if (!ret_timeout)
      {
         if (abs(in_res_time) > 20)
         {
            PBC_Failed_1("SAL_Lock_Mutex_Timeout: error: thread has been suspended for %d ms, expected time: 100 ms", (int)in_res_time);
         }
         else if (abs(in_res_time) > 10)
         {
            Tr_Fault_1("SAL_Lock_Mutex_Timeout: warning: thread has been suspended for %d ms, expected time: 100 ms", (int)in_res_time);
         }
         else
         {
         }
      }
      else /* ret_timeout */
      {
         PBC_Failed_1("SAL_Lock_Mutex_Timeout: returns 1, timeout: %d", (int)in_delta_time);
      }
   } /* if SHORT_TIMEOUT */
   else
   {
   }

   /* unlock the mutex */
   if (par.test_nr != SHORT_TIMEOUT)
   {
      if (!SAL_Unlock_Mutex(&Mutex))
      {
         PBC_Failed("SAL_Unlock_Mutex: returns 0");
      }
   }

   Tr_Info_Lo_1("Lock_Fun, Timeout: %d  Stop", (int)par.test_nr);
}

/**
 * The purpose of this test is to see which thread will acquire mutex between two equal priority threads
 * The running thread which just unlocked the mutex or the thread waiting on mutex.
 */
void Share_Lock_Fun(void* param)
{
   SAL_Thread_Id_T const wait_list[] =
   { LOCK1_THREAD, LOCK2_THREAD };
   int local_count = 0;
   int times_in_arow = 0;
   int i;

   Tr_Info_Lo("Shared Lock Fun: Start");

   SAL_Signal_Ready();

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Thread startup timeout");
   }

   for (i = 0; i < 200; i++)
   {
      /* try to lock shared mutex */
      if (SAL_Lock_Mutex_Timeout(&Mutex, 50))
      {
         int delta;

         SAL_Sleep(5);

         delta = Share_Lock_Count - local_count;
         switch (delta)
         {
            case 0:
               times_in_arow++;
               break;
            case 1:
               if (times_in_arow > 3)
               {
                  Tr_Fault_2("Got shared Mutex %d times in a row. Count %d", times_in_arow, Share_Lock_Count );
               }
               times_in_arow = 0;
               break;
            default:
               if (times_in_arow > 3)
               {
                  Tr_Fault_2("Got shared %d times in a row. Count %d", times_in_arow, Share_Lock_Count );
               }
               times_in_arow = 0;
               if (delta > 3)
               {
                  Tr_Fault_2("Shared count %d incremented %d extra times", Share_Lock_Count, (delta - 1) );
               }
               break;
         }
         Share_Lock_Count++;
         local_count = Share_Lock_Count;
         SAL_Unlock_Mutex(&Mutex);
      }
      else
      {
         Tr_Fault("Timed out - Failed to get Shared Mutex");
         test_failed = true;
      }
   }
}


/**
 *  Test that two equal priority threads will share a mutex in a balanced / fair way
 */
SAL_Int_T Share_Mutex_Test(SAL_Priority_T priority)
{
   static SAL_Thread_Id_T destroyed_list[] =
   { LOCK1_THREAD, LOCK2_THREAD };
   SAL_Int_T ret = 0;
   SAL_Thread_Attr_T thread_attr;

   Tr_Info_Hi_1("Start Share Mutex Test at Priority %d", priority);

   Share_Lock_Count = 0;
   test_failed = false;

   SAL_Create_Thread(Share_Lock_Fun, NULL, Init_Thread_Attr("LOCK1", LOCK1_THREAD, priority, &thread_attr));
   SAL_Create_Thread(Share_Lock_Fun, NULL, Init_Thread_Attr("LOCK2", LOCK2_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return (ret || test_failed);
}

/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Thread_Fun(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   Test_Nr_T test_nr = (Test_Nr_T) param;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Recursive_Lock_Fun, param, Init_Thread_Attr("REC_T", REC_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   Par.test_nr = (Test_Nr_T) param;

   if (test_nr == 0)
   {
      Par.destroyed_list[0] = LOCK2_THREAD;
      Par.destroyed_list[1] = LOCK3_THREAD;
      Par.list_size = 2;
   }
   else
   {
      Par.destroyed_list[0] = 0;
      Par.list_size = 0;
   }

   SAL_Create_Thread(Lock_Fun, &Par, Init_Thread_Attr("LOCK1_T", LOCK1_THREAD, priority, &thread_attr));

   if (test_nr == 0)
   {
      SAL_Sleep(100);

      Par.test_nr = (Test_Nr_T) param;
      Par.destroyed_list[0] = LOCK1_THREAD;
      Par.destroyed_list[1] = LOCK3_THREAD;
      Par.list_size = 2;

      SAL_Create_Thread(Lock_Fun, &Par, Init_Thread_Attr("LOCK2_T", LOCK2_THREAD, priority, &thread_attr));

      SAL_Sleep(100);

      Par.test_nr = (Test_Nr_T) param;
      Par.destroyed_list[0] = LOCK1_THREAD;
      Par.destroyed_list[1] = LOCK2_THREAD;
      Par.list_size = 2;

      SAL_Create_Thread(Lock_Fun, &Par, Init_Thread_Attr("LOCK3_T", LOCK3_THREAD, priority, &thread_attr));

   }
}

/**  Tests recursive mutex.
 *
 *   Creates MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Recursive_Mutex_Test(Test_Nr_T test_nr)
{
#if defined (XSAL)
   SAL_Thread_Id_T destroyed_list[] =
   { REC_THREAD, LOCK1_THREAD, LOCK2_THREAD, LOCK3_THREAD, MAIN_THREAD };
#endif
   SAL_Thread_Id_T destroyed_list_t[] =
   { REC_THREAD, LOCK1_THREAD, MAIN_THREAD };
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Main_Thread_Fun, (void*) test_nr, Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)
   if (test_nr == 0)
   {
      SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T));
   }
#endif

   if (test_nr != 0) /* LONG_TIMEOUT or SHORT_TIMEOUT */
   {
      if (!SAL_Wait_Destroyed_Timeout(destroyed_list_t, sizeof(destroyed_list_t) / sizeof(SAL_Thread_Id_T), Long_Timeout))
      {
         Tr_Fault("Timeout has been reached");
         ret = 1;
      }
   }

   return ret;
}

/** \}
 */

/**  \name Functions of Recursive_Mutex_Try_Test
 *
 *   \{
 */

/**  Function of REC_THREAD.
 *
 *   - locks mutex three times
 *   - checks if another threads are not suspended (terminated after try_lock)
 *   - unlocks the mutex three times
 *   - checks if LOCK6_THREAD is not suspended (terminated after locking and unlocking mutex)
 */

void Recursive_Try_Lock_Fun(void* param)
{
   Tr_Info_Lo("Recursive_Try_Lock_Fun: Start");

   /* lock mutex for the first time */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Try_Lock_Mutex: Thread doesn't lock the mutex");
   }

   SAL_Sleep(150);

   if (SAL_Is_Thread_Alive(LOCK1_THREAD))
   {
      PBC_Failed("LOCK1_THREAD is suspended");
   }

   /* lock mutex for the second time */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Try_Lock_Mutex: Thread doesn't lock the mutex for the second time");
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK2_THREAD))
   {
      PBC_Failed("LOCK1_THREAD is suspended");
   }

   /* lock mutex for the third time */
   if (!SAL_Try_Lock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Try_Lock_Mutex: Thread doesn't lock the mutex for the third time");
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK3_THREAD))
   {
      PBC_Failed("LOCK1_THREAD is suspended");
   }

   /* unlock mutex for the first time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK4_THREAD))
   {
      PBC_Failed("LOCK1_THREAD is suspended");
   }

   /* unlock mutex for the second time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK5_THREAD))
   {
      PBC_Failed("LOCK1_THREAD is suspended");
   }

   /* unlock mutex for the third time */
   if (!SAL_Unlock_Mutex(&Mutex))
   {
      PBC_Failed("SAL_Unlock_Mutex: returns 0");
   }

   SAL_Sleep(100);

   if (SAL_Is_Thread_Alive(LOCK6_THREAD))
   {
      PBC_Failed("LOCK1_THREAD has not been terminated");
   }

   Tr_Info_Lo("Recursive_Try_Lock_Fun: Stop");
}

/**  Function of LOCKn_THREAD, where n is in range from 1 to 6
 *
 *   - tries to lock mutex
 *   - mutex should not be locked if n is in range from 1 to 5
 *   - mutex should be locked if n is equal 6
 *   - unlocks mutex for n equal 6
 */

void Try_Lock_Fun(void* param)
{
   SAL_Int_T lock = (intptr_t) param;

   Tr_Info_Lo("Try_Lock_Fun: Start");

   /* mutex should not be locked */
   if (!lock)
   {
      if (SAL_Try_Lock_Mutex(&Mutex))
      {
         PBC_Failed("SAL_Try_Lock_Mutex: Thread locks mutex which has been already locked");
      }
   }
   /* mutex should be locked */
   else /* lock */
   {
      if (!SAL_Try_Lock_Mutex(&Mutex))
      {
         PBC_Failed("SAL_Try_Lock_Mutex: Mutex has not beem locked");
      }

      if (!SAL_Unlock_Mutex(&Mutex))
      {
         PBC_Failed("Mutex has not been unlocked");
      }
   }

   Tr_Info_Lo("Try_Lock_Fun: Stop");
}

/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Thread_Try_Fun(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T lock;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Recursive_Try_Lock_Fun, NULL, Init_Thread_Attr("REC_T", REC_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* mutex should not be locked */
   lock = false;

   /* after first try lock in REC_THREAD */
   SAL_Create_Thread(Try_Lock_Fun, (void *)(intptr_t) lock, Init_Thread_Attr("LOCK1_T", LOCK1_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after second try lock in REC_THREAD */
   SAL_Create_Thread(Try_Lock_Fun, (void *)(intptr_t) lock, Init_Thread_Attr("LOCK2_T", LOCK2_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after third try lock in REC_THREAD */
   SAL_Create_Thread(Try_Lock_Fun, (void *)(intptr_t) lock, Init_Thread_Attr("LOCK3_T", LOCK3_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after first unlock in REC_THREAD */
   SAL_Create_Thread(Try_Lock_Fun, (void *)(intptr_t) lock, Init_Thread_Attr("LOCK4_T", LOCK4_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* after second unlock in REC_THREAD */
   SAL_Create_Thread(Try_Lock_Fun, (void *)(intptr_t) lock, Init_Thread_Attr("LOCK5_T", LOCK5_THREAD, priority, &thread_attr));

   SAL_Sleep(100);

   /* mutex should be locked */
   lock = true;

   /* after third unlock in REC_THREAD */
   SAL_Create_Thread(Try_Lock_Fun, (void *)(intptr_t) lock, Init_Thread_Attr("LOCK6_T", LOCK6_THREAD, priority, &thread_attr));
}

/**  Tests recursive mutex.
 *
 *   Creates MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T Recursive_Try_Mutex_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;
   SAL_Int_T ret;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Main_Thread_Try_Fun, NULL, Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   return ret;
}

/** \}
 */

/** \name Other Functions
 *  \{
 */

/**  Test of mutex functions.
 *   
 *   Tests SAL_Create_Mutex and SAL_Destroy_Mutex functions and calls the tests:
 *   SAL_Lock_Mutex, SAL_Unlock_Mutex and SAL_Try_Lock_Mutex.
 */

SAL_Int_T Mutex_Test_Suite()
{
   SAL_Int_T ret = 0;
   SAL_Mutex_Attr_T mutex_attr;

   Tr_Info_Hi("Mutex Test Suite");

   SAL_Init_Mutex_Attr(&mutex_attr);

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("SAL_Create_Mutex: Mutex has not been created");
      return 1;
   }

   ret = ret || Lock_Mutex_Timeout_Test_1();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("SAL_Destroy_Mutex: Mutex has not been destroyed");
      return 1;
   }

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = ret || Lock_Mutex_Timeout_Test_2();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }

#if defined (XSAL)
   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = ret || Lock_Mutex_Test();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }
#endif

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = ret || Try_Lock_Mutex_Test();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }

   /* tests for recursive mutex */

   mutex_attr.recursive = true;

#if defined (XSAL)

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = ret || Recursive_Mutex_Test(0);

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }

#endif

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = ret || Recursive_Mutex_Test(LONG_TIMEOUT);

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }

#if !defined (LINUX)
   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = ret || Recursive_Mutex_Test(SHORT_TIMEOUT);

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }
#endif

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = ret || Recursive_Try_Mutex_Test();

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }

   if (!SAL_Create_Mutex(&Mutex, &mutex_attr))
   {
      Tr_Fault("Mutex has not been created");
      return 1;
   }

   ret = Share_Mutex_Test(SAL_NORMAL_PRIORITY) || ret;
   ret = Share_Mutex_Test(SAL_HIGH_PRIORITY) || ret;

   if (!SAL_Destroy_Mutex(&Mutex))
   {
      Tr_Fault("Mutex has not been destroyed");
      return 1;
   }

   return ret;
}

/** \}
 */

SAL_Int_T Set_Up()
{
   SAL_Config_T config;

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&config))
   {
      return 1;
   }

   return 0;
}

void Tear_Down()
{
}

int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up())
   {
      PBC_Failed("Set_Up failed");
   }

   ret = ret || Mutex_Test_Suite();

   Tear_Down();

   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));

   return ret;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  01 Sep 2006, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Initial revision
 *
 * -----------------------------------------------------------------------*/
