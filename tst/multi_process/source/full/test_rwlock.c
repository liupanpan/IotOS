/* -------------------------------------------------------------------------
 *
 * File:        test_rwlock.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of rwlock.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xsal.h"

EM_FILENUM(TEST_MODULE2_ID, 1);

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 10

typedef enum Thread_Id_Tag {
   MAIN_THREAD = 1,
   WRITE_THREAD_1,
   WRITE_THREAD_2,
   WRITE_THREAD_3,
   READ_THREAD_1,
   READ_THREAD_2,
   READ_THREAD_3,
   READ_THREAD_4,
   READ_THREAD_5,
   READ_THREAD_6
} Thread_Id_T;

typedef enum Thread_Nr_Tag {
   FIRST_THREAD_1 = 5,
   SECOND_THREAD_1,
   THIRD_THREAD_1,
   FIRST_THREAD_2,
   SECOND_THREAD_2,
   THIRD_THREAD_2
} Thread_Nr_T;

SAL_RWLock_T RW_Lock;

SAL_Int_T Long_Timeout = 3000;
SAL_Int_T Data = 0;
short Flag = 0;

/**  \file 
 *   
 *   Test of all rwlock functions (<B><I>XSAL only</I></B>).
 *
 *   \section T Tests:
 *   Tests of SAL_RLock_RWLock, SAL_WLock_RWLock and SAL_Unlock_RWLock functions:
 *   <OL>
 *   <LI> \ref M1 - \b Main_Fun_1 - tests sequence: writer, three readers. </LI>
 *   <LI> \ref M2 - \b Main_Fun_2 - tests sequence: writer, three readers, writer, writer. </LI>
 *   <LI> \ref M3 - \b Main_Fun_3 - tests sequence: writer, two readers, writer, two readers. </LI>
 *   </OL>
 *
 *   Tests of SAL_RLock_RWLock_Timeout, SAL_WLock_RWLock_Timeout and SAL_Unlock_RWLock functions:
 *   <OL>
 *   <LI> \ref M4 - \b Main_Fun_4 - tests sequence: writer, three readers, long timeout. </LI>
 *   <LI> \ref M4 - \b Main_Fun_5 - tests sequence: writer, three readers, short timeout. </LI>
 *   <LI> \ref M4 - \b Main_Fun_6 - tests sequence: three readers, writer, long timeout . </LI>
 *   <LI> \ref M4 - \b Main_Fun_7 - tests sequence: three readers, writer, short timeout. </LI>
 *   </OL>
 *      
 *   Tests of SAL_Try_RLock_RWLock, SAL_Try_WLock_RWLock and SAL_Unlock_RWLock functions:
 *   <OL>
 *   <LI> \ref M5 - \b Main_Fun_8 - tests sequence: writer, three readers. </LI>
 *   <LI> \ref M5 - \b Main_Fun_9 - tests sequence: three readers, writer. </LI>
 *   </OL>
 *
 *   Tests of SAL_RLock_RWLock, SAL_RLock_RWLock_Timeout and SAL_Try_RLock_RWLock functions:
 *   <OL>
 *   <LI> \ref M6 - the first test case, \b Main_Fun_10 - tests sequence: writer, three readers, writer three readers. </LI>
 *   <LI> \ref M6 - the second test case, \b Main_Fun_11 - tests sequence: three readers, writer, three readers, writer. </LI>
 *   </OL>
 * 
 *   \section M1 the first RWLock_Test
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates four threads: 
 *   - WRITE_THREAD_1
 *   - after 200 msec creates three reader threads (READ_THREAD_1, READ_THREAD_2 and READ_THREAD_3)
 *
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> WRITE_THREAD_1 locks rwlock for writing. </LI>
 *   <LI> WRITE_THREAD_1 sets Data on 10. </LI>
 *   <LI> WRITE_THREAD_1 sleeps for 400 msec. </LI>
 *   <LI> READ_THREADs try to lock rwlock. </LI>
 *   <LI> WRITE_THREAD_1 unlocks rwlock. </LI>
 *   <LI> After locking rwlock READ_THREAD_1 checks if Data are valid. </LI>
 *   <LI> READ_THREADs unlock rwlock. </LI>
 *   <LI> WRITE_THREAD_1 sleeps for 200 msec. </LI>
 *   <LI> WRITE_THREAD_1 checks if READ_THREADs has terminated. </LI>
 *   </OL>
 *
 *   \section M2 the second RWLock_Test
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates six threads: 
 *   - WRITE_THREAD_1
 *   - after 200 msec creates three reader threads (READ_THREAD_1, READ_THREAD_2 and READ_THREAD_3)
 *   - after 400 msec creates WRITE_THREAD_2
 *   - after 400 msec creates WRITE_THREAD_3
 *
 *   \subsection TP2 Test procedure:
 *
 *   <OL>
 *   <LI> Test procedure of \ref M1 from p. 1 to p. 6. </LI>
 *   <LI> READ_THREADs sleeps for 400 msec. </LI>
 *   <LI> WRITE_THREAD_1 unlocks rwlock. </LI>
 *   <LI> WRITE_THREAD_2 tries to lock rwlock. </LI>
 *   <LI> READ_THREADs unlock rwlock. </LI>
 *   <LI> WRITE_THREAD_2 sets Data on 20. </LI>
 *   <LI> WRITE_THREAD_2 sleeps for 400 msec. </LI>
 *   <LI> WRITE_THREAD_3 tries to lock RW_Lock. </LI>
 *   <LI> WRITE_THREAD_2 unlocks rwlock. </LI>
 *   <LI> WRITE_THREAD_3 checks if Data is valid. </LI>
 *   <LI> WRITE_THREAD_3 sets Data on 30. </LI>
 *   <LI> WRITE_THREAD_3 unlocks rwlock. </LI>
 *   </OL>
 *
 *   \section M3 the third RWLock_Test
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates six threads: 
 *   - WRITE_THREAD_1
 *   - after 200 msec creates READ_THREAD_1 and READ_THREAD_2
 *   - after 400 msec creates WRITE_THREAD_2
 *   - after 200 msec creates READ_THREAD_3 and READ_THREAD_4
 *
 *   \subsection TP3 Test procedure:
 *
 *   <OL>
 *   <LI> Test procedure of \ref M1 from p. 1 to p. 6. </LI>
 *   <LI> READ_THREADs sleeps for 600 msec. </LI>
 *   <LI> WRITE_THREAD_2 tries to lock rwlock. </LI>
 *   <LI> READ_THREADs try to lock rwlock. </LI>
 *   <LI> READ_THREADs unlock rwlock. </LI>
 *   <LI> WRITE_THREAD_2 checks if Data is valid. </LI>
 *   <LI> WRITE_THREAD_2 sets Data on 30. </LI>
 *   <LI> WRITE_THREAD_2 unlocks rwlock. </LI>
 *   <LI> READ_THREADs checks if Data are valid. </LI>
 *   <LI> READ_THREADs unlock rwlock. </LI>
 *   </OL>
 *
 *   \section M4 RWLock_Test with timeout
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates four threads: 
 *   - WRITE_THREAD_1
 *   - after 200 msec creates READ_THREAD_1, READ_THREAD_2 and READ_THREAD_3
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> FIRST_THREADS \b LOCK rwlock with LONG_TIMEOUT. </LI>
 *   <LI> FIRST_THREADS check if timeout value is valid. </LI>
 *   <LI> FIRST_THREADS do operation \b OPER on Data. </LI>
 *   <LI> FIRST_THREADS suspend for some time. </LI>
 *   <LI> SECOND_THREADS \b LOCK rwlock with \b TIMEOUT. </LI>
 *   <LI> FIRST_THREADS unlocks rwlock. </LI>
 *   <LI> SECOND_THREADS check if timeout is valid. </LI>
 *   if \b TIMEOUT = LONG_TIMEOUT then \n
 *       {
 *   <LI> SECOND_THREADS do operation \b OPER on Data. </LI>
 *   <LI> SECOND_THREADs unlock rwlock. \n </LI>
 *       }
 *   </OL>
 *
 *   \section TC Test Cases
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER>NO</TD><TD>Name</TD><TD>Desc</TD><TD>TIMEOUT</CENTER></B></TD></TR>
 *   <TR><TD>1</TD><TD>Main_Fun_4</TD><TD>FIRST_THREAD = WRITE_THREAD \n SECOND_THREAD = READ_THREADS</TD><TD>LONG_TIMEOUT</TD></TR>
 *   <TR><TD>2</TD><TD>Main_Fun_5</TD><TD>FIRST_THREAD = WRITE_THREAD \n SECOND_THREAD = READ_THREADS</TD><TD>SHORT_TIMEOUT</TD></TR>
 *   <TR><TD>3</TD><TD>Main_Fun_6</TD><TD>FIRST_THREAD = READ_THREADS \n SECOND_THREAD = WRITE_THREAD</TD><TD>LONG_TIMEOUT</TD></TR>
 *   <TR><TD>4</TD><TD>Main_Fun_7</TD><TD>FIRST_THREAD = READ_THREADS \n SECOND_THREAD = WRITE_THREAD</TD><TD>SHORT_TIMEOUT</TD></TR>
 *   </TABLE>
 * 
 *   <TABLE>
 *   <TR><TD><B><CENTER>THREAD</TD><TD>FIRST_THREAD</TD><TD>SECOND_THREAD</CENTER></B></TD></TR>
 *   <TR><TD></TD><TD>WRITE_THREAD</TD><TD>READ_THREADs</TD></TR>
 *   <TR><TD></TD><TD>READ_THREADs</TD><TD>WRITE_THREAD</TD></TR>
 *   </TABLE>
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER>THREAD</TD><TD>LOCK</TD><TD>OPER</CENTER></B></TD></TR>
 *   <TR><TD>READ_THREADs</TD><TD>RLOCK</TD><TD>READ</TD></TR>
 *   <TR><TD>WRITE_THREAD</TD><TD>WLOCK</TD><TD>WRITE</TD></TR>
 *   </TABLE>
 *
 *   \section M5 RWLock_Test (Try RLock and Try WLock)
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates four threads: 
 *   - WRITE_THREAD_1
 *   - after 200 msec creates READ_THREAD_1, READ_THREAD_2 and READ_THREAD_3
 *
 *   \subsection TP5 Test procedure:
 *
 *   <OL>
 *   <LI> FIRST_THREADS \b TRY_LOCK rwlock. </LI>
 *   <LI> SECOND_THREADS \b TRY_LOCK rwlock which is locked by FIRST_THREADS. </LI>
 *   <LI> FIRST_THREADS do operation \b OPER on Data. </LI>
 *   <LI> FIRST_THREADS unlock rwlock. </LI>
 *   <LI> SECOND_THREADS \b TRY_LOCK rwlock after rwlock is unlocked. </LI>
 *   <LI> SECOND_THREADS do operation \b OPER on Data. </LI>
 *   <LI> SECOND_THREADs unlock rwlock. \n </LI>
 *   </OL>
 *
 *  \section TC Test Cases
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER>NO</TD><TD>Name</TD><TD>Desc</CENTER></B></TD></TR>
 *   <TR><TD>1</TD><TD>Main_Fun_8</TD><TD>FIRST_THREAD = WRITE_THREAD \n SECOND_THREAD = READ_THREADs</TD></TR>
 *   <TR><TD>2</TD><TD>Main_Fun_9</TD><TD>FIRST_THREAD = READ_THREADs \n SECOND_THREAD = WRITE_THREADS</TD></TR>
 *   </TABLE>
 * 
 *   <TABLE>
 *   <TR><TD><B><CENTER>THREAD</TD><TD>FIRST_THREAD</TD><TD>SECOND_THREAD</CENTER></B></TD></TR>
 *   <TR><TD></TD><TD>WRITE_THREAD</TD><TD>READ_THREADs</TD></TR>
 *   <TR><TD></TD><TD>READ_THREADs</TD><TD>WRITE_THREAD</TD></TR>
 *   </TABLE>
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER>THREAD</TD><TD>TRY_LOCK</TD><TD>OPER</CENTER></B></TD></TR>
 *   <TR><TD>READ_THREADs</TD><TD>TRY_RLOCK</TD><TD>READ</TD></TR>
 *   <TR><TD>WRITE_THREAD</TD><TD>TRY_WLOCK</TD><TD>WRITE</TD></TR>
 *   </TABLE>
 *  
 *   \section M6 RWLock_Test_Case with mixed locking
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER>NO</TD><TD>Thread Group</TD><TD>Members Of Group</CENTER></B></TD></TR>
 *   <TR><TD>1</TD><TD>FIRST_THREADS</TD><TD>READ_THREAD_1 \n READ_THREAD_2 \n READ_THREAD_3</TD></TR>
 *   <TR><TD>2</TD><TD>SECOND_THREADS</TD><TD>READ_THREAD_4 \n READ_THREAD_2 \n READ_THREAD_3</TD></TR>
 *   </TABLE>
 *   
 *   <TABLE>
 *   <TR><TD><B><CENTER>NO</TD><TD>Thread_Id</TD><TD>Lock Type</CENTER></B></TD></TR>
 *   <TR><TD>1</TD><TD>READ_THREAD_1</TD><TD>LOCK</TD></TR>
 *   <TR><TD>2</TD><TD>READ_THREAD_2</TD><TD>LOCK_TIMEOUT</TD></TR>
 *   <TR><TD>3</TD><TD>READ_THREAD_3</TD><TD>TRY_LOCK</TD></TR>
 *   <TR><TD>4</TD><TD>READ_THREAD_4</TD><TD>LOCK</TD></TR>
 *   <TR><TD>5</TD><TD>READ_THREAD_5</TD><TD>LOCK_TIMEOUT</TD></TR>
 *   <TR><TD>6</TD><TD>READ_THREAD_6</TD><TD>TRY_LOCK</TD></TR>
 *   </TABLE>
 *
 *   where:
 *   - \b LOCK - SAL_Lock_RWLock function
 *   - \b LOCK_TIMEOUT - SAL_Lock_RWLock_Timeout function
 *   - \b TRY_LOCK - SAL_Try_Lock_RWLock function
 *
 *   \section M7 the first RWLock_Test with mixed locking
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates eight threads: 
 *   - WRITE_THREAD_1
 *   - after 200 msec creates FIRST_THREADS
 *   - after 400 msec creates WRITE_THREAD_2
 *   - after 200 msec creates SECOND_THREADS
 *
 *   FIRST_THREADS try to lock rwlock for reading when rwlock is locked by WRITE_THREAD_1.
 *   READ_THREAD_3 should not lock rwlock (try_lock used).
 *   SECOND_THREADS try to lock rwlock for reading in the moment when rwlock is unlocked.
 *   READ_THREAD_6 can lock rwlock or not (try_lock used).
 *
 *   \subsection TP6 Test procedure:
 *  
 *   <OL>
 *   <LI> WRITE_THREAD_1 locks rwlock. </LI>
 *   <LI> WRITE_THREAD_1 sets Data. </LI>
 *   <LI> WRITE_THREAD_1 suspends for 400 msec. </LI>
 *   <LI> FIRST_THREADS try to lock rwlock </LI>
 *   <LI> FIRST_THREADS sleep for 400 msec. </LI>
 *   <LI> WRITE_THREAD_1 unlocks rwlock. </LI>
 *   <LI> WRITE_THREAD_2 tries to lock rwlock which is locked by FIRST_THREADS. </LI>
 *   <LI> After 200 msec SECOND_THREADS try to lock rwlock. </LI>
 *   <LI> READ_THREADs unlock rwlock. </LI>
 *   <LI> WRITE_THREAD_2 locks and unlocks rwlock. </LI>
 *   </OL>
 *
 *   \section M8 the second RWLock_Test with mixed locking
 *
 *   Creates MAIN_THREAD. \n
 *   MAIN_THREAD creates eight threads: 
 *   - FIRST_THREADS
 *   - after 200 msec creates WRITE_THREAD_1
 *   - after 400 msec creates SECOND_THREADS
 *   - after 400 msec creates WRITE_THREAD_2
 *
 *   FIRST_THREADS lock rwlock for reading when rwlock is locked by WRITE_THREAD_1.
 *   READ_THREAD_3 should lock rwlock (try_lock used).
 *   SECOND_THREADS try to lock rwlock for reading in the moment when rwlock is unlocked.
 *   READ_THREAD_6 can lock rwlock or not (try_lock used).
 *
 *   \subsection TP7 Test procedure:
 *  
 *   <OL>
 *   <LI> FIRST_THREADS locks rwlock. </LI>
 *   <LI> FIRST_THREADS sleeps for 400 msec. </LI>
 *   <LI> WRITE_THREAD_1 try to lock rwlock. </LI>
 *   <LI> FIRST_THREADS unlock rwlock. </LI>
 *   <LI> WRITE_THREAD_1 sets Data. </LI>
 *   <LI> WRITE_THREAD_1 suspends for 400 msec. </LI>
 *   <LI> SECOND_THREADS tries to lock rwlock which is locked by WRITE_THREAD_1. </LI>
 *   <LI> WRITE_THREAD_1 unlocks rwlock. </LI>
 *   <LI> SECOND_THREADs suspends for 400 msec. </LI>
 *   <LI> WRITE_THREAD_2 tries to lock rwlock. </LI>
 *   <LI> SECOND_THREADS unlock rwlock. </LI>
 *   <LI> WRITE_THREAD_2 sets Data. </LI>
 *   <LI> WRITE_THREAD_2 unlocks rwlock. </LI>
 *   </OL>
 *
 */


void Read_RWLock_Fun(void* param);
void Write_RWLock_Fun(void* param);


const SAL_Thread_Attr_T* Init_Thread_Attr(
   const char* name, 
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



/**  \name Functions of the first RWLock_Test_Case
 *
 *   \{
 */

/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_1(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Write_RWLock_Fun, 
      (void*)0, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)0, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)0, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)0, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));

}

/** \}
 */

/**  \name Functions of the second RWLock_Test_Case
 *
 *   \{
 */

/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_2(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Write_RWLock_Fun, 
      (void*)1, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)1, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)1, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)1, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));

   SAL_Sleep(400);

   SAL_Create_Thread(
      Write_RWLock_Fun, 
      (void*)2, 
      Init_Thread_Attr("WRITER_2", WRITE_THREAD_2, priority, &thread_attr));

   SAL_Sleep(400);

   SAL_Create_Thread(
      Write_RWLock_Fun, 
      (void*)3, 
      Init_Thread_Attr("WRITER_3", WRITE_THREAD_3, priority, &thread_attr));

}

/** \}
 */


/**  \name Functions of the third RWLock_Test_Case
 *
 *   \{
 */

/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_3(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Write_RWLock_Fun, 
      (void*)1, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)2, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)2, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Sleep(400);

   SAL_Create_Thread(
      Write_RWLock_Fun, 
      (void*)4, 
      Init_Thread_Attr("WRITER_2", WRITE_THREAD_2, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)3,
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun, 
      (void*)3, 
      Init_Thread_Attr("READER_4", READ_THREAD_4, priority, &thread_attr));

}

/** \}
 */

/**  \name Functions of the first RWLock_Test_Case with timeout
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - locks rwlock with timeout for reading
 *   - checks if Data is valid
 *   - unlocks rwlock
 */

void Read_RWLock_Timeout_Fun_1(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_1: Start");

   time_1 = SAL_Clock();

   /* try to lock rwlock which is locked by Writer Thread */
   ret_timeout = SAL_RLock_RWLock_Timeout(&RW_Lock, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 200;

   if (ret_timeout)
   {
      if (abs(res_time) > 25)
      {
         PBC_Failed_1("SAL_RLock_RWLock_Timeout: error: thread has been suspended for %d ms, expected time: 200 ms", (int)delta_time);
      }
      else if (abs(res_time) > 10)
      {
         Tr_Fault_1("SAL_RLock_RWLock_Timeout: warning: thread has been suspended for %d ms, expected time: 200 ms", (int)delta_time);
      }
      else
      {
      }
   }
   else /* !Ret_Timeout */
   {
      PBC_Failed("SAL_RLock_RWLock_Timeout: Thread doesn't lock rwlock");
   }

   /* read Data */
   if (Data != 40)
   {
      PBC_Failed_1("Invalid data: Data = %d", (int)Data);
   }

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_1: Stop");
}


/**  Function of Writer Thread.
 *
 *   - locks rwlock with timeout for writing
 *   - sets Data
 *   - unlocks rwlock
 *   - checks if reader threads are alive
 */

void Write_RWLock_Timeout_Fun_1(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_1: Start");

   time_1 = SAL_Clock();

   /* lock rwlock for writing */
   ret_timeout = SAL_WLock_RWLock_Timeout(&RW_Lock, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         PBC_Failed("SAL_WLock_RWLock_Timeout: suspends the thread");
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_WLock_RWLock_Timeout: thread doesn't lock rwlock");
   }

   /* write Data */
   Data = 40;

   SAL_Sleep(400);

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   SAL_Sleep(200);

   /* check if Reader Threads has unlocked rwlock */
   if (SAL_Is_Thread_Alive(READ_THREAD_1) || 
      SAL_Is_Thread_Alive(READ_THREAD_2) || 
      SAL_Is_Thread_Alive(READ_THREAD_3))
   {
      PBC_Failed("SAL_RLock_RWLock_Timeout: thread doesn't resume after unlocking rwlock");
   }

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_1: Stop");

}


/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_4(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Write_RWLock_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));
}


/** \}
 */


/**  \name Functions of the second RWLock_Test_Case with timeout
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - locks rwlock with timeout for reading
 *   - checks if timeout value is valid
 */

void Read_RWLock_Timeout_Fun_2(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;
   SAL_Thread_Id_T destroyed_list[] = { WRITE_THREAD_1 };

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_2: Start");

   time_1 = SAL_Clock();

   /* try to lock rwlock for reading when rwlock is locked by Writer Threads */
   ret_timeout = SAL_RLock_RWLock_Timeout(&RW_Lock, 100);

   time_2 = SAL_Clock();

   SAL_Signal_Ready();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 100;

   if (!ret_timeout)
   {
      if (abs(res_time) > 20)
      {
         PBC_Failed_1("SAL_RLock_RWLock_Timeout: invalid interval of timeout, expected: 100, achieved: %d", (int)delta_time);
      }
      else if (abs(res_time) > 10)
      {
         Tr_Fault_1("SAL_RLock_RWLock_Timeout: warning: thread has been suspended for %d ms, expected time: 100 ms", (int)delta_time);
      }
      else
      {
      }
   }
   else /* ret_timeout */
   {
      PBC_Failed_1("SAL_RLock_RWLock_Timeout: returns 1, res_time = %d", (int)res_time);
   }

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_2: Stop");
}


/**  Function of Writer Thread.
 *
 *   - locks rwlock with timeout for writing
 *   - sets Data
 *   - unlocks rwlock
 */

void Write_RWLock_Timeout_Fun_2(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;
   SAL_Thread_Id_T wait_list[] = { READ_THREAD_1, READ_THREAD_2 , READ_THREAD_3 };

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_2: Start");

   time_1 = SAL_Clock();

   /* lock rwlock for writing */
   ret_timeout = SAL_WLock_RWLock_Timeout(&RW_Lock, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         PBC_Failed("SAL_WLock_RWLock_Timeout: suspends the thread");
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_WLock_RWLock_Timeout: thread doesn't lock rwlock");
   }

   /* write Data */
   Data = 50;

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(int));

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_2: Stop");
}


/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_5(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Write_RWLock_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));
}

/** \} 
 */

/**  \name Functions of the third RWLock_Test_Case with timeout
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - locks rwlock with timeout for reading
 *   - checks if Data is valid
 *   - sleeps for 400 msec
 *   - unlocks rwlock
 */

void Read_RWLock_Timeout_Fun_3(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_3: Start");

   time_1 = SAL_Clock();

   /* lock rwlock for reading */
   ret_timeout = SAL_RLock_RWLock_Timeout(&RW_Lock, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         PBC_Failed_1("SAL_RLock_RWLock_Timeout: suspends the thread, delta_time: %d", (int)delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_RLock_RWLock_Timeout: thread doesn't lock rwlock");
   }

   /* read Data */
   if (Data != 50)
   {
      PBC_Failed_1("Invalid data: Data = %d", (int)Data);
   }

   SAL_Sleep(400);

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_3: Stop");
}


/**  Function of Writer Thread.
 *
 *   - locks rwlock with timeout for writing
 *   - sets Data
 *   - unlocks rwlock
 */

void Write_RWLock_Timeout_Fun_3(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_3: Start");

   time_1 = SAL_Clock();

   /* try to lock rwlock for writing when rwlock is locked by Reader Threads */
   ret_timeout = SAL_WLock_RWLock_Timeout(&RW_Lock, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 200;

   if (ret_timeout)
   {
      if (abs(res_time) > 20)
      {
         PBC_Failed_1("SAL_WLock_RWLock_Timeout: error: thread has been suspended for %d ms, expected time: 200 ms", (int)delta_time);
      }
      else if (abs(res_time) > 10)
      {
         Tr_Fault_1("SAL_WLock_RWLock_Timeout: warning: thread has been suspended for %d ms, expected time: 200 ms", (int)delta_time);
      }
      else
      {
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_WLock_RWLock_Timeout: Thread doesn't lock rwlock");
   }
   
   /* write Data */
   Data = 60;

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_3: Stop");

}



/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_6(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Data = 50;

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_3, 
      NULL, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_3, 
      NULL, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_3, 
      NULL, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Write_RWLock_Timeout_Fun_3, 
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));
}

/** \}
 */


/**  \name Functions of the fourth RWLock_Test_Case with timeout
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - locks rwlock for reading
 *   - checks if Data is valid
 *   - sleeps for 400 msec
 *   - unlocks rwlock
 */

void Read_RWLock_Timeout_Fun_4(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;
   bool_t ret_timeout;
   SAL_Thread_Id_T wait_list[] = { WRITE_THREAD_1};

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_4: Start");

   time_1 = SAL_Clock();

   /* lock rwlock for reading */
   ret_timeout = SAL_RLock_RWLock_Timeout(&RW_Lock, Long_Timeout);

   time_2 = SAL_Clock();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   if (ret_timeout)
   {
      if (delta_time > 1)
      {
         PBC_Failed_1("SAL_RLock_RWLock_Timeout: suspends the thread, delta_time: %d", (int)delta_time);
      }
   }
   else /* !ret_timeout */
   {
      PBC_Failed("SAL_RLock_RWLock_Timeout: thread doesn't lock rwlock");
   }

   /* read Data */
   if (Data != 70)
   {
      PBC_Failed_1("Invalid data: Data = %d", (int)Data);
   }

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Read_RWLock_Timeout_Fun_4: Stop");
}


/**  Function of Writer Thread.
 *
 *   - locks rwlock with timeout for writing
 *   - checks if timeout value is valid
 */

void Write_RWLock_Timeout_Fun_4(void* param)
{
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time, res_time;
   bool_t ret_timeout;
   SAL_Thread_Id_T destroyed_list[] = { READ_THREAD_1, READ_THREAD_2, READ_THREAD_3 };

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_4: Start");

   time_1 = SAL_Clock();

   /* try to lock rwlock for writing when rwlock is locked by Reading Threads */
   ret_timeout = SAL_WLock_RWLock_Timeout(&RW_Lock, 100);

   time_2 = SAL_Clock();

   SAL_Signal_Ready();

   delta_time = (SAL_Int_T)(time_2 - time_1);

   res_time = delta_time - 100;

   if (!ret_timeout)
   {
      if (abs(res_time) > 20)
      {
         PBC_Failed_1("SAL_WLock_RWLock_Timeout: error: thread has been suspended for %d ms, expected time: 100 ms", (int)delta_time);
      }
      else if (abs(res_time) > 10)
      {
         Tr_Fault_1("SAL_WLock_RWLock_Timeout: warning: thread has been suspended for %d ms, expected time: 100 ms", (int)delta_time);
      }
      else
      {
      }
   }
   else /* ret_timeout */
   {
      PBC_Failed("SAL_WLock_RWLock_Timeout: returns 1");
   }
   
   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Write_RWLock_Timeout_Fun_4: Stop");

}


/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_7(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Data = 70;

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_4, 
      NULL, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_4, 
      NULL, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Timeout_Fun_4, 
      NULL, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Write_RWLock_Timeout_Fun_4, 
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));
}

/** \}
 */

/**  \name Functions of the first RWLock_Test_Case for Try Lock
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - tries to lock rwlock for reading when it is locked by WRITE_THREAD_1
 *   - tries to lock rwlock for reading after unlocking rwlock by WRITE_THREAD_1
 *   - unlocks rwlock
 */

void Read_Try_RWLock_Fun_1(void* param)
{
   SAL_Thread_Id_T wait_list[] = { WRITE_THREAD_1 };

   Tr_Info_Lo("Read_Try_RWLock_Fun_1: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(int));

   /* try to lock rwlock for reading when it is locked by Writer Thread */
   if (SAL_Try_RLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Try_RLock_RWLock: returns 1 when rwlock is locked");
   }

   SAL_Signal_Ready();

   while (!Flag)
   {
      SAL_Sleep(10);
   }
   SAL_Sleep(10);

   /* try to lock rwlock which has been unlocked by WRITE_THREAD_1 */
   if (!SAL_Try_RLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Try_Lock_RWLock: doesn't lock rwlock after it has been unlocked");
   }

   /* read Data */
   if (Data != 90)
   {
      PBC_Failed_1("Invalid data: Data = %d", (int)Data);
   }

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Read_Try_RWLock_Fun_1: Stop");
}


/**  Function of Writer Thread.
 *
 *   - try to lock rwlock for writing
 *   - sets Data
 *   - unlocks rwlock
 */

void Write_Try_RWLock_Fun_1(void* param)
{
   SAL_Thread_Id_T wait_list[] = { READ_THREAD_1, READ_THREAD_2, READ_THREAD_3 };
   SAL_Thread_Id_T destroyed_list[] = { READ_THREAD_1, READ_THREAD_2, READ_THREAD_3 };

   Tr_Info_Lo("Write_Try_RWLock_Fun_1: Start");

   /* try to lock rwlock for writing */
   if (!SAL_Try_WLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Try_WLock_RWLock: can't lock rwlock");
   }

   SAL_Signal_Ready();

   /* write Data */
   Data = 90;

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Flag = 1;

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Write_Try_RWLock_Fun_1: Stop");
}


/**  Function of MAIN_THREAD.
 *
 *   Creates threads.
 */

void Main_Fun_8(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Write_Try_RWLock_Fun_1,
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_Try_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_Try_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_Try_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));

}

/** \}
 */

/**  \name Functions of the second RWLock_Test_Case for Try Lock
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - tries to lock rwlock for reading
 *   - unlocks rwlock
 */

void Read_Try_RWLock_Fun_2(void* param)
{
   SAL_Thread_Id_T wait_list [] = { WRITE_THREAD_1 };
   SAL_Thread_Id_T destroyed_list [] = { WRITE_THREAD_1 };

   Tr_Info_Lo("Read_Try_RWLock_Fun_2: Start");

   /* try to lock rwlock for reading */
   if (!SAL_Try_RLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Try_RLock_RWLock: Thread doesn't lock rwlock");
   }

   SAL_Signal_Ready();

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   /* read Data */
   if (Data != 100)
   {
      PBC_Failed_1("Invalid data: Data = %d", (int)Data);
   }

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: returns 0");
   }

   Flag = 1;

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));
  
   Tr_Info_Lo("Read_Try_RWLock_Fun_2: Stop");
}


/**  Function of Writer Thread.
 *
 *   - tries to lock rwlock for writing when it is locked by Reader Threads
 *   - tries to lock rwlock for writing after unlocking rwlock by Reader Threads
 *   - sets Data
 *   - unlocks rwlock
 */

void Write_Try_RWLock_Fun_2(void* param)
{
   SAL_Thread_Id_T wait_list [] = { READ_THREAD_1, READ_THREAD_2, READ_THREAD_3 };

   Tr_Info_Lo("Write_Try_RWLock_Fun_2: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   /* try to lock rwlock which is already locked by Readers threads */
   if (SAL_Try_WLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Try_WLock_RWLock: returns 1 when rwlock is locked");
   }

   SAL_Signal_Ready();

   while (!Flag)
   {
      SAL_Sleep(10);
   }
   SAL_Sleep(10);

   /* try to lock rwlock which has been unlocked by Readers threads */
   if (!SAL_Try_WLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Try_WLock_RWLock: doesn't lock rwlock after it has been unlocked");
   }

   /* write Data */
   Data = 110;

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("Can't unlock rwlock");
   }

   Tr_Info_Lo("Write_Try_RWLock_Fun_2: Stop");
}



/**  Thread of MAIN_THREAD of third Lock_RWLock_Timeout_Test.
 *
 *   Create threads.
 */

void Main_Fun_9(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Data = 100;

   SAL_Create_Thread(
      Read_Try_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_1", READ_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_Try_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_2", READ_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_Try_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_3", READ_THREAD_3, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Write_Try_RWLock_Fun_2,
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

}

/** \}
 */


/**  \name Functions of the first mixed rwlock test case
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - tries to lock rwlock for reading using usual locking, locking with timeout and try_lock
 *   - unlocks rwlock
 */

void Read_RWLock_Fun_1(void* param)
{
   bool_t b = 0;
   SAL_Thread_Id_T thread_id;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;

   Tr_Info_Lo("Read_RWLock_Fun_1: Start");

   thread_id = SAL_Get_Thread_Id();

   /* lock rwlock for reading */
   if ((thread_id == FIRST_THREAD_1) || (thread_id == FIRST_THREAD_2))
   {
      if (!SAL_RLock_RWLock(&RW_Lock))
      {
         PBC_Failed("SAL_RLock_RWLock: can't lock rwlock");
      }
   }
   /* lock rwlock for reading with timeout */
   else if ((thread_id == SECOND_THREAD_1) || (thread_id == SECOND_THREAD_2))
   {  
      time_1 = SAL_Clock();

      if (!SAL_RLock_RWLock_Timeout(&RW_Lock, Long_Timeout))
      {
         time_2 = SAL_Clock();

         delta_time = (SAL_Int_T)(time_2 - time_1);

         PBC_Failed_1("SAL_RLock_RWLock: lock rwlock timeout error, timeout = %d", (int)delta_time);
      }
   }
   /* try to lock rwlock for reading */
   else if ((thread_id == THIRD_THREAD_1) || (thread_id == THIRD_THREAD_2))
   { 
        time_1 = SAL_Clock();

        b = SAL_Try_RLock_RWLock(&RW_Lock);

        time_2 = SAL_Clock();

        delta_time = (SAL_Int_T)(time_2 - time_1);

         if ((b)&&(thread_id == THIRD_THREAD_1))
         {
            PBC_Failed_1("SAL_RLock_RWLock: try lock error, time: %d", (int)delta_time);
         }
   }
   else
   {
   }

   /* suspend the thread */
   if ((thread_id >= FIRST_THREAD_1) && (thread_id <= THIRD_THREAD_1))
   {
      SAL_Sleep(400);
   }

   /* unlock rwlock */
   if ((((thread_id == THIRD_THREAD_1) || (thread_id == THIRD_THREAD_2)) && (b)) 
      || ((thread_id != THIRD_THREAD_1) && (thread_id != THIRD_THREAD_2)))
   {            
      if (!SAL_Unlock_RWLock(&RW_Lock))
      {
         PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
      }
   }

   Tr_Info_Lo_1("Read_RWLock_Fun_1: %d, Stop", (int)thread_id);
}


/**  Function of Writer Threads.
 *
 *   - locks rwlock for writing
 *   - sets Data
 *   - unlocks rwlock
 */

void Write_RWLock_Fun_1(void* param)
{
   SAL_Thread_Id_T thread_id;

   Tr_Info_Lo("Write_RWLock_Fun_1: Start");

   thread_id = SAL_Get_Thread_Id();

   /* lock rwlock for writing */
   if (!SAL_WLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_WLock_RWLock: can't lock rwlock");
   }

   /* set Data on some value, suspend the thread */
   if (thread_id == WRITE_THREAD_1)
   {
      Data = 10;

      SAL_Sleep(400);
   }
   else if (thread_id == WRITE_THREAD_2)
   {
      if (Data != 10)
      {
         PBC_Failed_1("Invalid data: Data = %d", (int)Data);
      }

      Data = 30;
   }
   else
   {
   }

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Write_RWLock_Fun_1: Stop");

}


/**  Thread of MAIN_THREAD.
 *
 *   Create threads.
 */

void Main_Fun_10(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Write_RWLock_Fun_1,
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_1", FIRST_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_2", SECOND_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_3", THIRD_THREAD_1, priority, &thread_attr));

   SAL_Sleep(400);

   SAL_Create_Thread(
      Write_RWLock_Fun_1,
      NULL, 
      Init_Thread_Attr("WRITER_2", WRITE_THREAD_2, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Read_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_4", FIRST_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_5", SECOND_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_1, 
      NULL, 
      Init_Thread_Attr("READER_6", THIRD_THREAD_2, priority, &thread_attr)); 

}

/** \}
 */


/**  \name Functions of the second mixed rwlock test case
 *
 *   \{
 */

/**  Function of Reader Threads.
 *
 *   - tries to lock rwlock for reading using usual locking, locking with timeout and try_lock
 *   - unlocks rwlock
 */

void Read_RWLock_Fun_2(void* param)
{
   bool_t b = 0;
   SAL_Thread_Id_T thread_id;
   SAL_Clock_T time_1, time_2;
   SAL_Int_T delta_time;

   Tr_Info_Lo("Read_RWLock_Fun_2: Start");

   thread_id = SAL_Get_Thread_Id();

   /* lock rwlock for reading */
   if ((thread_id == FIRST_THREAD_1) || (thread_id == FIRST_THREAD_2))
   {
      if (!SAL_RLock_RWLock(&RW_Lock))
      {
         PBC_Failed("SAL_RLock_RWLock: can't lock rwlock");
      }
   }
   /* lock rwlock for reading with timeout */
   else if ((thread_id == SECOND_THREAD_1) || (thread_id == SECOND_THREAD_2))
   {
      time_1 = SAL_Clock();

      if (!SAL_RLock_RWLock_Timeout(&RW_Lock, Long_Timeout))
      {
         time_2 = SAL_Clock();

         delta_time = (SAL_Int_T)(time_2 - time_1);

         PBC_Failed_1("SAL_RLock_RWLock: lock rwlock timeout, timeout: %d", (int)delta_time);
      }
   }
   /* try to lock rwlock for reading */
   else if ((thread_id == THIRD_THREAD_1) || (thread_id == THIRD_THREAD_2))
   {
      time_1 = SAL_Clock();

      b = SAL_Try_RLock_RWLock(&RW_Lock);

      time_2 = SAL_Clock();

      delta_time = (SAL_Int_T)(time_2 - time_1);

      if ((!b)&&(thread_id == THIRD_THREAD_1))
      {
         PBC_Failed_1("SAL_RLock_RWLock: try lock error, time = %d", (int)delta_time);
      }
   }
   else
   {
   }

   /* suspend the thread */
   SAL_Sleep(400);

   /* unlock rwlock */
   if (((b)&&((thread_id == THIRD_THREAD_1) || (thread_id == THIRD_THREAD_2)))
      || ((thread_id != THIRD_THREAD_1) && (thread_id != THIRD_THREAD_2)))
   {
      if (!SAL_Unlock_RWLock(&RW_Lock))
      {
         PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
      }
   }

   Tr_Info_Lo("Read_RWLock_Fun_2: Stop");
}


/**  Function of Writer Threads.
 *
 *   - locks rwlock for writing
 *   - sets Data
 *   - unlocks rwlock
 */

void Write_RWLock_Fun_2(void* param)
{
   SAL_Thread_Id_T thread_id;

   Tr_Info_Lo("Write_RWLock_Fun_2: Start");

   thread_id = SAL_Get_Thread_Id();

   /* lock rwlock for writing */
   if (!SAL_WLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_WLock_RWLock: can't lock rwlock");
   }

   /* set Data on some value, suspend the thread */
   if (thread_id == WRITE_THREAD_1)
   {
      Data = 10;

      SAL_Sleep(400);
   }
   else if (thread_id == WRITE_THREAD_2)
   {
      if (Data != 10)
      {
         PBC_Failed_1("Invalid data: Data = %d", (int)Data);
      }

      Data = 30;
   }
   else
   {
   }

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Write_RWLock_Fun_2: Stop");
}


/**  Thread of MAIN_THREAD.
 *
 *   Create threads.
 */

void Main_Fun_11(void* param)
{
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   Data = 10;

   SAL_Create_Thread(
      Read_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_1", FIRST_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_2", SECOND_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_3", THIRD_THREAD_1, priority, &thread_attr));

   SAL_Sleep(200);

   SAL_Create_Thread(
      Write_RWLock_Fun_2,
      NULL, 
      Init_Thread_Attr("WRITER_1", WRITE_THREAD_1, priority, &thread_attr));

   SAL_Sleep(400);

   SAL_Create_Thread(
      Read_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_4", FIRST_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_5", SECOND_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(
      Read_RWLock_Fun_2, 
      NULL, 
      Init_Thread_Attr("READER_6", THIRD_THREAD_2, priority, &thread_attr));

   SAL_Sleep(400);

   SAL_Create_Thread(
      Write_RWLock_Fun_2,
      NULL, 
      Init_Thread_Attr("WRITER_2", WRITE_THREAD_2, priority, &thread_attr));

}

/** \}
 */

/**  \name Other Functions
 *
 *   \{
 */

/**  Function of Reader Threads of RWLock Tests.
 *
 *   - locks rwlock for reading
 *   - checks if Data is valid
 *   - optionally sleeps for some time
 *   - unlocks rwlock
 */

void Read_RWLock_Fun(void* param)
{
   SAL_Int_T test_nr = (intptr_t)param;

   Tr_Info_Lo("Read_RWLock_Fun: Start");

   /* lock rwlock for reading */
   if (!SAL_RLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_RLock_RWLock: can't lock rwlock");
   }

   /* read Data, suspend the thread */
   if (test_nr == 0)
   {
      if (Data != 10)
      {
         PBC_Failed_1("SAL_WLock_RWLock: invalid data: Data = %d", (int)Data);
      }   
   }
   if (test_nr == 1)
   {
    if (Data != 10)
      {
         PBC_Failed_1("SAL_WLock_RWLock: invalid data: Data = %d", (int)Data);
      }
   
      SAL_Sleep(400);
   }
   else if (test_nr == 2)
   {
      if (Data != 10)
         {
            PBC_Failed_1("SAL_WLock_RWLock: invalid data: Data = %d", (int)Data);
         }

      SAL_Sleep(600);
   }
   else if (test_nr == 3)
   {
      if ((Data != 10) && (Data != 40))
      {
         PBC_Failed_1("SAL_WLock_RWLock: invalid data: Data = %d", (int)Data);
      }
   }

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   Tr_Info_Lo("Read_RWLock_Fun: Stop");
}


/**  Function of Writer Threads of RWLock Tests.
 *
 *   - locks rwlock for writing
 *   - sets Data
 *   - unlocks rwlock
 *   - optionally checks if reader threads are alive
 */

void Write_RWLock_Fun(void* param)
{
   SAL_Int_T test_nr = (intptr_t)param;

   Tr_Info_Lo("Write_RWLock_Fun: Start");

   /* lock rwlock fo writing */
   if (!SAL_WLock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_WLock_RWLock: can't lock rwlock");
   }

   /* set Data on some value */
   if ((test_nr == 1) || (test_nr == 0))
   {
      Data = 10;

      SAL_Sleep(400);
   }
   else if (test_nr == 2)
   {
      Data = 20;

      SAL_Sleep(400);
   }
   else if (test_nr == 3)
   {
      if (Data != 20)
      {
         PBC_Failed_1("Invalid data: Data = %d", (int)Data);
      }

      Data = 30;
   }
   else if (test_nr == 4)
   {
      if (Data != 10)
      {
         PBC_Failed_1("Invalid data: Data = %d", (int)Data);
      }

      Data = 40;
   }
   else
   {
   }

   /* unlock rwlock */
   if (!SAL_Unlock_RWLock(&RW_Lock))
   {
      PBC_Failed("SAL_Unlock_RWLock: can't unlock rwlock");
   }

   /* check if Reader Threads has unlocked rwlock */
   if (test_nr == 0)
   {
      SAL_Sleep(200);

      if (SAL_Is_Thread_Alive(READ_THREAD_1) || 
         SAL_Is_Thread_Alive(READ_THREAD_2) || 
         SAL_Is_Thread_Alive(READ_THREAD_3))
      {
         PBC_Failed("SAL_RLock_RWLock: thread doesn't resume after unlocking rwlock");
      }
   }

   Tr_Info_Lo("Write_RWLock_Fun: Stop");

}


/**  Function of test case of RWLock_Test_Case.
 *
 *   Creates MAIN_THREAD and starts SAL_Run.
 */

SAL_Int_T RWLock_Test(void* Fun, const SAL_Thread_Id_T* destroyed_list)
{
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Fun, 
      NULL, 
      Init_Thread_Attr("MAIN_T", MAIN_THREAD, priority, &thread_attr));

   ret =  SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret; 
}



/**  Test of rwlock.
 *
 *   Creates rw_lock, calls test cases and destroys rw_lock.
 */

SAL_Int_T RWLock_Test_Suite()
{
   SAL_Int_T ret = 0;
   SAL_Thread_Id_T destroyed_list_1[] = { WRITE_THREAD_1, READ_THREAD_1, READ_THREAD_2, READ_THREAD_3, MAIN_THREAD };
   SAL_Thread_Id_T destroyed_list_2[] = { WRITE_THREAD_1, WRITE_THREAD_2, WRITE_THREAD_3, READ_THREAD_1, READ_THREAD_2, READ_THREAD_3, MAIN_THREAD };
   SAL_Thread_Id_T destroyed_list_3[] = { WRITE_THREAD_1, WRITE_THREAD_2, READ_THREAD_1, READ_THREAD_2, READ_THREAD_3, READ_THREAD_4, MAIN_THREAD };
   SAL_Thread_Id_T destroyed_list_4[] = { WRITE_THREAD_1, WRITE_THREAD_2, READ_THREAD_1, READ_THREAD_2, READ_THREAD_3, READ_THREAD_4, READ_THREAD_5, READ_THREAD_6, MAIN_THREAD }; 

   ret = ret || RWLock_Test(Main_Fun_1, destroyed_list_1);

   ret = ret || RWLock_Test(Main_Fun_2, destroyed_list_2);

   ret = ret || RWLock_Test(Main_Fun_3, destroyed_list_3);

   /* timeout tests */

   ret = ret || RWLock_Test(Main_Fun_4, destroyed_list_1);

   ret = ret || RWLock_Test(Main_Fun_5, destroyed_list_1);

   ret = ret || RWLock_Test(Main_Fun_6, destroyed_list_1);

   ret = ret || RWLock_Test(Main_Fun_7, destroyed_list_1);

   /* try lock tests */

   ret = ret || RWLock_Test(Main_Fun_8, destroyed_list_1);

   Flag = 0;

   ret = ret || RWLock_Test(Main_Fun_9, destroyed_list_1);

   ret = ret || RWLock_Test(Main_Fun_10, destroyed_list_3);

   ret = ret || RWLock_Test(Main_Fun_11, destroyed_list_4);

   return ret;
}

/** \}
 */

SAL_Int_T Set_Up()
{
   SAL_Config_T config;
   SAL_RWLock_Attr_T rwlock_attr;

   SAL_Init_RWLock_Attr(&rwlock_attr);

   if (!SAL_Create_RWLock(&RW_Lock, &rwlock_attr))
   {
      Tr_Fault("SAL_Create_RW_Lock: RWLock has not been created");
      return 1;
   }

   SAL_Get_Config(&config);

   config.app_id = APP_ID;
   config.max_number_of_threads = MAX_NUMB_OF_THREADS;

   if (!SAL_Init(&config))
   {
      return 1;
   }

   return 0;
}


SAL_Int_T Tear_Down()
{
   if (!SAL_Destroy_RWLock(&RW_Lock))
   {
      Tr_Fault("SAL_Destroy_RWLock: RWLock has not been destroyed");
      return 1;
   }

   return 0;
}


int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up())
   {
      PBC_Failed("Set_Up failed");
   }

   ret = ret || RWLock_Test_Suite();

   if (Tear_Down())
   {
      Tr_Fault("Tear_Down failed");
      return 1;
   }

   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));

   return ret;
}

/* -------------------------------------------------------------------------
 * Revision history:
 *
 *  27 Feb 2007, Urszula Gessing (Urszula.Gessing@delphi.com)
 *   Initial revision
 *
 * -----------------------------------------------------------------------*/
