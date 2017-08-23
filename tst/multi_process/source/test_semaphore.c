/* -------------------------------------------------------------------------
 *
 * File:        test_semaphore.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of semaphores.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 6);

#define APP_ID 2

#define MAX_NUMB_OF_THREADS 6

typedef enum Thread_Id_Tag
{
   MAIN_THREAD = 1,
   WAIT_THREAD1,
   WAIT_THREAD2,
   WAIT_THREAD3,  
   LWAIT_THREAD,
   SIGNAL_THREAD
} Thread_Id_T;

SAL_Semaphore_T Semaphore_0;
SAL_Semaphore_T Semaphore_3;
SAL_Semaphore_T Semaphore_1;

SAL_Int_T Long_Timeout = 3000;

/**  \file 
 *   
 *   Test of all semaphore functions.
 *
 *   \section T Tests:
 *   <OL>
 *   <LI> \ref TST01 - test of SAL_Wait_Semaphore_Timeout and SAL_Signal_Semaphore 
 *        functions for semaphore initial value 0 (uses semaphore Semaphore_0), long timeout. </LI>
 *   <LI> \ref TST02 - test of SAL_Wait_Semaphore_Timeout and SAL_Signal_Semaphore 
 *        functions for semaphore initial value 0 (uses semaphore Semaphore_0), short timeout. </LI>
 *   <LI> \ref ST0 - test of SAL_Wait_Semaphore and SAL_Signal_Semaphore 
 *        functions for semaphore initial value 0, uses semaphore Semaphore_0 (<B><I>XSAL only</I></B>). </LI>
 *   <LI> \ref ST3 - test of SAL_Wait_Semaphore and SAL_Signal_Semaphore
 *        functions for semaphore initial value 3, uses semaphore Semaphore_3 (<B><I>XSAL only</I></B>). </LI>
 *   <LI> \ref TWT - test of SAL_Try_Wait_Semaphore using semaphore initial
 *        value 1(uses semaphore Semaphore_1). </LI>
 *   <LI> Functions SAL_Create_Semaphore and SAL_Destroy_Semaphore are tested in
 *        \b Semaphore_Test function before and after calling three tests above. </LI>
 *   </OL>
 *      
 *
 *   \section TST01 Timeout_Semaphore_Test_01
 *
 *   Creates two threads: WAIT_THREAD1 and SIGNAL_THREAD. \n
 *   Initial value of semaphore Semaphore_0 is 0. \n
 *   Signalling of semaphore in SIGNAL_THREAD takes place earlier than the timeout of 
 *   SAL_Wait_Semaphore_Timeout in WAIT_THREAD1 is reached (timeout is 3000).
 *
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> WAIT_THREAD1 try to decrease semaphore which value is 0 
 *      (suspends itself), with timeout 3000. </LI>
 *   <LI> SIGNAL_THREAD checks if WAIT_THREAD1 is suspended. </LI>
 *   <LI> SIGNAL_THREAD increase semaphore value. </LI>
 *   <LI> SIGNAL_THREAD sleeps for 1000 msec. </LI>
 *   <LI> SIGNAL_THREAD checks if WAIT_THREAD1 has resumed and terminated. </LI>
 *   </OL>
 *
 *   \section TST02 Timeout_Semaphore_Test_02
 *
 *   Creates two threads: WAIT_THREAD1 and SIGNAL_THREAD. \n
 *   Initial value of semaphore Semaphore_0 is 0. \n
 *   Timeout of SAL_Wait_Semaphore_Timeout is short (500 ms) therefore timeout in WAIT_THREAD1 
 *   is reached earlier than signalling the semaphore by SIGNAL_THREAD.
 *
 *   \subsection TP2 Test procedure:
 *
 *   <OL>
 *   <LI> WAIT_THREAD1 try to decrease semaphore which value is 0 
 *      (suspends itself), with timeout 500. </LI>
 *   <LI> SIGNAL_THREAD sleeps for 1000 msec. </LI>
 *   <LI> SIGNAL_THREAD checks if WAIT_THREAD1 has resumed after timeout and has terminated. </LI>
 *   <LI> SIGNAL_THREAD increase semaphore value. </LI>
 *   </OL>
 *
 *   \section ST0 Semaphore_Test_0
 *
 *   Creates two threads: WAIT_THREAD1 and SIGNAL_THREAD. \n
 *   Initial value of semaphore Semaphore_0 is 0.
 *
 *   \subsection TP3 Test procedure:
 *
 *   <OL>
 *   <LI> WAIT_THREAD1 try to decrease semaphore which value is 0 
 *      (suspends itself). </LI>
 *   <LI> SIGNAL_THREAD checks if WAIT_THREAD1 is suspended. </LI>
 *   <LI> SIGNAL_THREAD increase semaphore value. </LI>
 *   <LI> SIGNAL_THREAD checks if WAIT_THREAD1 resumed and terminated. </LI>
 *   </OL>
 *
 *   \section ST3 Semaphore_Test_3
 *
 *   Creates five threads: SIGNAL_THREAD, WAIT_THREAD1, WAIT_THREAD2, 
 *   WAIT_THREAD3, LWAIT_THREAD. \n
 *   Initial value of semaphore Semaphore_3 is 3.
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> Each of threads: WAIT_THREAD1, WAIT_THREAD2 and WAIT_THREAD3 
 *      decreases the semaphore value by one in an incident order. </LI>
 *   <LI> LWAIT_THREAD try to decreases semaphore which value is 0
 *      (suspends itself). </LI>
 *   <LI> SIGNAL_THREAD checks if LWAIT_THREAD is suspended. </LI>
 *   <LI> SIGNAL_THREAD increase semaphore value. </LI>
 *   <LI> SIGNAL_THREAD checks if LWAIT_THREAD has resumed and terminated. </LI>
 *   </OL>
 *
 *   \section TWT Try_Wait_Test
 *
 *   Creates two threads: WAIT_THREAD1 and SIGNAL_THREAD. \n
 *   Initial value of semaphore Semaphore_1 is 1.
 *
 *   \subsection TP5 Test procedure:
 *
 *   <OL>
 *   <LI> WAIT_THREAD1 tries to decrease the semaphore value. </LI>
 *   <LI> WAIT_THREAD1 tries to decrease the semaphore which has a value 0. </LI>
 *   <LI> SIGNAL_THREAD increase the semaphore value. </LI>
 *   <LI> WAIT_THREAD1 tries to decrease the semaphore value. </LI>
 *   </OL>
 */

#define GREATER_TIME_WINDOW_MS   (25)

const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, 
                                          SAL_Thread_Id_T id, 
                                          SAL_Priority_T prior, 
                                          SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}


/**  \name Functions of Semaphore_Timeout_Test_01
 *
 *   \{
 */

/**  Function of WAIT_THREAD1.
 *
 *   - suspends the thread
 *   - finally the thread is resumed because timeout is long enough (first semaphore in 
 *     SIGNAL_THREAD is signaled)
 */

void Wait_Timeout_Fun_01(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Wait_Timeout_Fun_01: Start");

   SAL_Signal_Ready();

   time_1 = SAL_Clock();

   /* suspend the thread */
   ret_timeout = SAL_Wait_Semaphore_Timeout(&Semaphore_0, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 1000;

   if (ret_timeout)
   {
      if (( res_time < 0 ) || (res_time > GREATER_TIME_WINDOW_MS))
      {
         PBC_Failed_1("SAL_Wait_Semaphore_Timeout: thread has been suspended for %d ms, expected time: 1000 ms", (int)delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_Wait_Semaphore_Timeout: returns 0");
   }

   Tr_Info_Lo("Wait_Timeout_Fun_01: Stop");
}

/**  Function of SIGNAL_THREAD.
 *
 *   - signals the semaphore
 */

void Signal_Timeout_Fun_01(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { WAIT_THREAD1 };

   Tr_Info_Lo("Signal_Timeout_Fun_01: Start\n");

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   SAL_Sleep(1000);

   /* check if WAIT_THREAD1 is suspended */
   if (!SAL_Is_Thread_Alive(WAIT_THREAD1))
   {
      PBC_Failed("SAL_Wait_Semaphore_Timeout: doesn't suspend the thread");
   }

   /* increase the semaphore value */
   if (!SAL_Signal_Semaphore(&Semaphore_0))
   {
      PBC_Failed("SAL_Signal_Semaphore: returns 0");
   }

   SAL_Sleep(1000);

   /* check if WAIT_THREAD1 has resumed and terminated */
   if (SAL_Is_Thread_Alive(WAIT_THREAD1))
   {
      PBC_Failed("SAL_Signal_Semaphore: doesn't unlock the semaphore");
   }

   Tr_Info_Lo("Signal_Timeout_Fun_01: Stop\n");
}

/**  Test of semaphore functions SAL_Wait_Semaphore_Timeout and SAL_Signal_Semaphore
 *   for the semaphore initial value 0 (Semaphore_0).
 */

SAL_Int_T Semaphore_Timeout_Test_01()
{
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD, WAIT_THREAD1 };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Signal_Timeout_Fun_01, 
      NULL, 
      Init_Thread_Attr("SIGNAL", SIGNAL_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Wait_Timeout_Fun_01, 
      NULL, 
      Init_Thread_Attr("WAIT", WAIT_THREAD1, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;

}

/**  \}
 */



/**  \name Functions of Semaphore_Timeout_Test_02
 *
 *   \{
 */

/**  Function of WAIT_THREAD1.
 *
 *   - suspends the thread
 *   - finally timeout is reached before semaphore is signaled in SIGNAL_THREAD (timeout
 *     is short)
 */

void Wait_Timeout_Fun_02(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Wait_Timeout_Fun_02: Start");

   SAL_Signal_Ready();

   time_1 = SAL_Clock();

   /* suspend the thread */
   ret_timeout = SAL_Wait_Semaphore_Timeout(&Semaphore_0, 500);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 500;

   if (!ret_timeout)
   {
      if (( res_time < 0 ) || (res_time > GREATER_TIME_WINDOW_MS))
      {
         PBC_Failed_1("SAL_Wait_Semaphore_Timeout: invalid interval of timeout, expected: 500, achieved: %d", (int)delta_time);
      }
   }
   else /* ret_timeout */
   {
      PBC_Failed("SAL_Wait_Semaphore_Timeout: returns 1");
   }

   Tr_Info_Lo("Wait_Timeout_Fun_02: Stop");
}

/**  Function of SIGNAL_THREAD.
 *
 *   - signals the semaphore
 */

void Signal_Timeout_Fun_02(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { WAIT_THREAD1 };

   Tr_Info_Lo("Signal_Timeout_Fun_02: Start\n");

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

   SAL_Sleep(1000);

   /* check if WAIT_THREAD1 has resumed after timeout and has terminated */
   if (SAL_Is_Thread_Alive(WAIT_THREAD1))
   {
      PBC_Failed("SAL_Wait_Semaphore_Timeout: suspends the thread after timeout");
   }

   /* increase the semaphore value */
   if (!SAL_Signal_Semaphore(&Semaphore_0))
   {
      PBC_Failed("SAL_Signal_Semaphore: doesn't increase the semaphore value");
   }

   Tr_Info_Lo("Signal_Timeout_Fun_02: Stop\n");
}

/**  Test of semaphore functions SAL_Wait_Semaphore_Timeout and SAL_Signal_Semaphore
 *   for the semaphore initial value 0 (Semaphore_0).
 */

SAL_Int_T Semaphore_Timeout_Test_02()
{
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD, WAIT_THREAD1 };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Signal_Timeout_Fun_02, 
      NULL, 
      Init_Thread_Attr("SIGNAL", SIGNAL_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Wait_Timeout_Fun_02, 
      NULL, 
      Init_Thread_Attr("WAIT", WAIT_THREAD1, priority, &thread_attr));

   ret = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

   return ret;

}

/**  \}
 */


#if defined (XSAL)

/**  \name Functions of Sempahore_Test_0
 *
 *   \{
 */

/**  Function of WAIT_THREAD1.
 *
 *   - suspends the thread
 */

void Wait_Fun_0(void* param)
{
   Tr_Info_Lo("Wait_Fun_0: Start");

   SAL_Signal_Ready();

   /* suspend the thread */
   if (!SAL_Wait_Semaphore(&Semaphore_0))
   {
      PBC_Failed("SAL_Wait_Semaphore: returns 0");
   }

   Tr_Info_Lo("Wait_Fun_0: Stop");
}

/**  Function of SIGNAL_THREAD.
 *
 *   - signals the semaphore
 */

void Signal_Fun_0(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { WAIT_THREAD1 };

   Tr_Info_Lo("Signal_Fun_0: Start\n");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_Sleep(1000);

   /* check if WAIT_THREAD1 is suspended */
   if (!SAL_Is_Thread_Alive(WAIT_THREAD1))
   {
      PBC_Failed("SAL_Wait_Semaphore: doesn't suspend the thread");
   }

   /* increase the semaphore value */
   if (!SAL_Signal_Semaphore(&Semaphore_0))
   {
      PBC_Failed("SAL_Signal_Semaphore: returns 0");
   }

   SAL_Sleep(1000);

   /* check if WAIT_THREAD1 resumed and terminated */
   if (SAL_Is_Thread_Alive(WAIT_THREAD1))
   {
      PBC_Failed("SAL_Signal_Semaphore: doesn't unlock the semaphore");
   }

   Tr_Info_Lo("Signal_Fun_0: Stop\n");
}

/**  Test of semaphore functions SAL_Wait_Semaphore and SAL_Signal_Semaphore
 *   for the semaphore initial value 0 (Semaphore_0).
 */

SAL_Int_T Semaphore_Test_0()
{
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD, WAIT_THREAD1 };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Signal_Fun_0, 
      NULL, 
      Init_Thread_Attr("SIGNAL", SIGNAL_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Wait_Fun_0, 
      NULL, 
      Init_Thread_Attr("WAIT", WAIT_THREAD1, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;

}

/**  \}
 */


/**  \name Functions of Semaphore_Test_3
 *
 *   \{
 */

/**  Function of LWAIT_THREAD.
 *
 *   - suspends the thread
 */

void Last_Wait_Fun_3(void* param)
{
   static SAL_Thread_Id_T destroyed_list [] = { WAIT_THREAD1, WAIT_THREAD2, WAIT_THREAD3 }; 

   Tr_Info_Lo("Last_Wait_Fun_3: Start");

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   SAL_Signal_Ready();
   
   /* suspend the thread */
   SAL_Wait_Semaphore(&Semaphore_3);

   Tr_Info_Lo("Last_Wait_Fun_3: Stop");
}


/**  Function of WAIT_THREAD1, WAIT_THREAD2 and WAIT_THREAD3.
 *
 *   - decreases semaphore value
 */

void Wait_Fun_3(void* param)
{
   Tr_Info_Lo("Wait_Fun_3: Start");

   /* decrease the semaphore value */
   SAL_Wait_Semaphore(&Semaphore_3);

   Tr_Info_Lo("Wait_Fun_3: Stop");
}


/**  Function of SIGNAL_THREAD.
 *   
 *   - signals semaphore
 */

void Signal_Fun_3(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { LWAIT_THREAD };

   Tr_Info_Lo("Signal_Fun_3: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_Sleep(1000);

   /* check if LWAIT_THREAD is suspended */
   if (!SAL_Is_Thread_Alive(LWAIT_THREAD))
   {
      PBC_Failed("SAL_Wait_Semaphore: Doesn't suspend the thread");
   }

   /* increase semaphore value */
   SAL_Signal_Semaphore(&Semaphore_3);

   SAL_Sleep(1000);

   /* check if LWAIT_THREAD has resumed and terminated */
   if (SAL_Is_Thread_Alive(LWAIT_THREAD))
   {
      PBC_Failed("SAL_Signal_Semaphore: doesn't unlock the semaphore");
   }
   

   Tr_Info_Lo("Signal_Fun_3: Stop");
}


/**  Tests semaphore functions SAL_Wait_Semaphore and SAL_Signal_Semaphore for the
 *   semaphore initial value 3 (Semaphore_3). 
 */

SAL_Int_T Semaphore_Test_3()
{
   static SAL_Thread_Id_T destroyed_list[] = { SIGNAL_THREAD, WAIT_THREAD1 };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Wait_Fun_3, 
      NULL, 
      Init_Thread_Attr("WAIT_1", WAIT_THREAD1, priority, &thread_attr));

   SAL_Create_Thread(
      Wait_Fun_3, 
      NULL, 
      Init_Thread_Attr("WAIT_2", WAIT_THREAD2, priority, &thread_attr));

   SAL_Create_Thread(
      Wait_Fun_3, 
      NULL, 
      Init_Thread_Attr("WAIT_3", WAIT_THREAD3, priority, &thread_attr));

   SAL_Create_Thread(
      Last_Wait_Fun_3, 
      NULL, 
      Init_Thread_Attr("L_WAIT", LWAIT_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Signal_Fun_3, 
      NULL, 
      Init_Thread_Attr("SIGNAL", SIGNAL_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;

}

/**  \}
 */

#endif

/** \name Functions of Try_Wait_Test
 *
 *  \{
 */

/**  Function of WAIT_THREAD1.
 *
 *   - tries to decrease the semaphore
 *   - tries to decrease the semaphore when its value is equal 0
 *   - tries to decrease the semaphore after it was signalled in SIGNAL_THREAD
 */

void Try_Wait_Fun(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { SIGNAL_THREAD };

   Tr_Info_Lo("Try_Wait_Fun: Start");

   /* try to decrease semaphore value */
   if (!SAL_Try_Wait_Semaphore(&Semaphore_1))
   {
      PBC_Failed("SAL_Try_Wait_Semaphore: doesn't lock the semaphore");
   }

   /* try to decrease semaphore when its value is equal 0 */
   if (SAL_Try_Wait_Semaphore(&Semaphore_1))
   {
      PBC_Failed("SAL_Try_Wait_Semaphore: returns 1 when semaphore is locked");
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   /* try to decrease semaphore value */
   if (!SAL_Try_Wait_Semaphore(&Semaphore_1))
   {
      PBC_Failed("SAL_Try_Wait_Semaphore: doesn't lock the semaphore");
   }

   Tr_Info_Lo("Try_Wait_Fun: Stop");
}


/**  Function of SIGNAL_THREAD.
 *
 *   - signals the semaphore
 */

void Signal_Try_Fun(void* param)
{
   static SAL_Thread_Id_T wait_list [] = { WAIT_THREAD1 };

   Tr_Info_Lo("Signal_Try_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   /* increase semaphore value */
   SAL_Signal_Semaphore(&Semaphore_1);

   SAL_Signal_Ready();

   Tr_Info_Lo("Signal_Try_Fun: Stop");
   /*
    * Don't exit until WAIT_THREAD1 does to make sure it sees our SAL_Signal_Ready()
    */
#if defined (XSAL)

   SAL_Wait_Destroyed(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Long_Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif
}


/** Tests semaphore function SAL_Try_Wait_Semaphore for the semaphore initial value 1.
 */

SAL_Int_T Try_Wait_Test()
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Signal_Try_Fun, 
      NULL, 
      Init_Thread_Attr("SIGNAL", SIGNAL_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Try_Wait_Fun, 
      NULL, 
      Init_Thread_Attr("WAIT_1", WAIT_THREAD1, priority, &thread_attr));

   return SAL_Run();

}

/**  \}
 */


/** \name Other Functions 
 *
 *  \{
 */

/**  Test of semaphores.
 *
 *   Creates semaphores, calls Semaphore_Test_0, Semaphore_Test_3 and Try_Wait_Test
 *   and destroys semaphores.
 */

SAL_Int_T Semaphore_Test()
{
   SAL_Int_T ret = 0;
   SAL_Semaphore_Attr_T sem_attr;

   SAL_Init_Semaphore_Attr(&sem_attr);

   if (!SAL_Create_Semaphore(&Semaphore_0, &sem_attr))
   {
      Tr_Fault("SAL_Create_Semaphore: Semaphore with value 0 was not created");
      return 1;
   }

   ret = ret || Semaphore_Timeout_Test_01();

   ret = ret || Semaphore_Timeout_Test_02();

   if (!SAL_Destroy_Semaphore(&Semaphore_0))
   {
      Tr_Fault("SAL_Destroy_Semaphore: Semaphore with value 0 was not destroyed");
      return 1;
   }

#if defined (XSAL)

   if (!SAL_Create_Semaphore(&Semaphore_0, &sem_attr))
   {
      Tr_Fault("Semaphore with value 0 was not created");
      return 1;
   }

   ret = ret || Semaphore_Test_0();

   if (!SAL_Destroy_Semaphore(&Semaphore_0))
   {
      Tr_Fault("Semaphore with value 0 was not destroyed");
      return 1;
   }

   sem_attr.initial_value = 3;

   if (!SAL_Create_Semaphore(&Semaphore_3, &sem_attr))
   {
      Tr_Fault("SAL_Create_Semaphore: Semaphore with value 3 was not created");
      return 1;
   }

   ret = ret || Semaphore_Test_3();

   if (!SAL_Destroy_Semaphore(&Semaphore_3))
   {
      Tr_Fault("SAL_Destroy_Semaphore: Semaphore with value 3 was not destroyed");
      return 1;
   }

#endif

   sem_attr.initial_value = 1;

   if (!SAL_Create_Semaphore(&Semaphore_1, &sem_attr))
   {
      Tr_Fault("Semaphore with value 3 was not created");
      return 1;
   }

   ret = ret || Try_Wait_Test();

   if (!SAL_Destroy_Semaphore(&Semaphore_1))
   {
      Tr_Fault("Semaphore with value 3 was not destroyed");
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
      Tr_Fault("Set_Up failed");
      return 1;
   }

   ret = ret || Semaphore_Test();

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
