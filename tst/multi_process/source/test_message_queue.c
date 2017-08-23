/* -------------------------------------------------------------------------
 *
 * File:        test_message_queue.c
 *
 * Copyright:   2006-2014 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of message queue.
 * 
 * -----------------------------------------------------------------------*/

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "pbc_trace.h"
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 1);

#define APP_ID 2
#define MAX_NUMB_OF_THREADS 2

#define SENDER_THREAD 1
#define RECEIVER_THREAD 2

typedef enum Event_Id_Tag {
   EV_E1 = 1,
   EV_E2,
   EV_E3,
   EV_E4,
   EV_E5,
   EV_E6
} Event_Id_T;

typedef struct Sample_Msg_Tag1
{
   SAL_Int_T field1;
   SAL_Int_T field2;
} Sample_Msg_T1;

typedef struct Sample_Msg_Tag2
{
   SAL_Int_T field1;
   SAL_Int_T field2;
   SAL_Int_T field3;
} Sample_Msg_T2;

typedef struct Sizes_Tag
{
  size_t alloc_size;
  size_t max_buffer_size;
} Sizes_T;

Sizes_T Size;

size_t Queue_Size;
size_t Max_Message_Count, Message_Count;

SAL_Int_T Timeout = 3000;


/**  \file
 *   
 *   Test of message queue functions: SAL_Create_Queue, SAL_Create_Queue_For_Thread, all queue iterator
 *   functions and SAL_Stat_Queue_T struct.
 *
 *   \section T Tests:
 *   <OL>
 *   <LI> \ref QFTT - test of SAL_Create_Queue_For_Thread function and fields of the 
 *      struct SAL_Stat_Queue_T (<B><I>XSAL only</I></B>). </LI>
 *   <LI> \ref QT - test of SAL_Create_Queue function and fields of the 
 *      struct SAL_Stat_Queue_T. </LI>
 *   <LI> \ref QIT - test of all iterator queue functions, 
 *      SAL_Remove_Message, SAL_Get_Message (<B><I>XSAL only</I></B>). </LI>
 *   </OL>
 *
 *   \section QFTT Queue_For_Thread_Test
 * 
 *   SENDER_THREAD is created.
 * 
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> SENDER_THREAD creates queue for RECEIVER_THREAD. </LI>
 *   <LI> SENDER_THREAD sends five messages of different sizes and data. </LI>
 *   <LI> SENDER_THREAD creates RECEIVER_THREAD. </LI>
 *   <LI> RECEIVER_THREAD checks the queue_size and buffer_size. </LI>
 *   <LI> RECEIVER_THREAD receives two messages. </LI>
 *   <LI> RECEIVER_THREAD checks peak_message_count, message_count, 
 *      peak_message_size. </LI>    
 *
 *   \subsection TC Test cases:
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER> Number </TD><TD> alloc_size   </TD><TD> sending data </CENTER></B></TD></TR>
 *   <TR><TD> 1 </TD><TD> 0                     </TD><TD> no           </TD></TR>
 *   <TR><TD> 2 </TD><TD> sizeof(Sample_Msg_T1) </TD><TD> no           </TD></TR> 
 *   <TR><TD> 3 </TD><TD> 0                     </TD><TD> yes          </TD></TR>
 *   <TR><TD> 4 </TD><TD> sizeof(Sample_Msg_T1) </TD><TD> yes          </TD></TR>
 *   </TABLE>

 *
 *   \section QT Queue_Test
 *
 *   Two threads are created: SENDER_THREAD and RECEIVER_THREAD.
 *
 *   \subsection TP2 Test procedure:
 *
 *   <OL>
 *   <LI> RECEIVER_THREAD creates the queue of size Alloc_Size. </LI>
 *   <LI> RECEIVER_THREAD checks the queue_size and buffer_size. </LI>
 *   <LI> SENDER_THREAD sends five messages of different sizes and data. </LI>
 *   <LI> RECEIVER_THREAD receives two messages. </LI>
 *   <LI> RECEIVER_THREAD checks peak_message_count, message_count, 
 *      peak_message_size. </LI>
 *   </OL>
 *
 *   \subsection TC Test cases:
 *
 *   <TABLE>
 *   <TR><TD><B><CENTER> Number </TD><TD> alloc_size   </TD><TD> sending data </CENTER></B></TD></TR>
 *   <TR><TD> 1 </TD><TD> 0                     </TD><TD> no           </TD></TR>
 *   <TR><TD> 2 </TD><TD> sizeof(Sample_Msg_T1) </TD><TD> no           </TD></TR> 
 *   <TR><TD> 4 </TD><TD> 0                     </TD><TD> yes          </TD></TR>
 *   <TR><TD> 3 </TD><TD> sizeof(Sample_Msg_T1) </TD><TD> yes          </TD></TR>
 *   </TABLE>
 *
 *
 *   \section QIT Queue_Iterator_Test
 *
 *   Two threads are created: SENDER_THREAD and RECEIVER_THREAD.
 *
 *   \subsection TP3 Test procedure:
 *
 *   <OL>
 *   <LI> RECEIVER_THREAD creates the queue. </LI>
 *   <LI> SENDER_THREAD sends six messages. </LI>
 *   <LI> RECEIVER_THREAD moves to the first message of the queue 
 *      (checks SAL_First_Message, SAL_Get_Message, SAL_Is_Queue_Begin). </LI>
 *   <LI> RECEIVER_THREAD moves to the next message of the queue
 *      (checks SAL_Next_Message). </LI>
 *   <LI> RECEIVER_THREAD moves to the prevoius message
 *      (checks SAL_Prevoius_Message). </LI>
 *   <LI> RECEIVER_THREAD moves to the last message (checks SAL_Last_Message). </LI>
 *   <LI> RECEIVER_THREAD moves to the end of queue (checks SAL_Queue_End). </LI>
 *   <LI> RECEIVER_THREAD moves to the prevoius message (the last message). </LI>
 *   <LI> RECEIVER_THREAD moves to the next element (the end of queue, 
 *      checks SAL_Is_Queue_End). </LI>
 *   <LI> RECEIVER_THREAD moves to the message with event_id = 3. </LI>
 *   <LI> RECEIVER_THREAD removes message. </LI>
 *   <LI> RECEIVER_THREAD checks the event id of the current message. </LI>
 *   <LI> RECEIVER_THREAD moves to the previous message. </LI>
 *   <LI> RECEIVER_THREAD checks the event id of the current message. </LI>
 *   <LI> RECEIVER_THREAD moves to the next message. </LI>
 *   <LI> RECEIVER_THREAD checks the event id of the current message. </LI>
 *   </OL>
 */


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



#if defined (XSAL)

/** \name Functions of Queue_For_Thread_Test
 *
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - checks SAL_Stat_Queue_T fields
 *   - receives messages
 *   - checks SAL_Stat_Queue_T fields
 */

void Receive_For_Thread_Fun(void* param)
{
   SAL_Stat_Queue_T stat;
   const SAL_Message_T* msg;
   Sizes_T size = *(Sizes_T*)param;

   Tr_Info_Lo("Receive_For_Thread_Fun: Start");

   if (!SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat))
   {
      PBC_Failed("SAL_Stat_Queue: Queue has not been created");
   }

   if (stat.queue_size != Queue_Size)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid parameter of queue size");
   }

   if (stat.buffer_size != size.alloc_size)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid parameter of buffer size");
   }

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

   msg = SAL_Receive();

   PBC_Ensure(msg != NULL, "NULL XSAL message");

   Message_Count--;

   msg = SAL_Receive();

   PBC_Ensure(msg != NULL, "NULL XSAL message");

   Message_Count--;

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

   if (stat.peak_message_count != Max_Message_Count)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid max number of messages in the queue");
   }

   if (stat.message_count != Message_Count)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid number of messages in the queue");
   }


   if (stat.peak_message_size != size.max_buffer_size)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid parameter of max message size");
   }

   Tr_Info_Lo("Receive_For_Thread_Fun: Stop");
}



/**  Function of SENDER_THREAD.  
 *
 *   - creates queue for RECEIVER_THREAD
 *   - sends messages to the RECEIVER_THREAD
 */

void Send_For_Thread_Fun(void* param)
{
   Sample_Msg_T1* data1;
   Sample_Msg_T2* data2;
   size_t i;
   size_t size_of_1, size_of_2;
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   Sizes_T* size = (Sizes_T*)param;

   Tr_Info_Lo("Send_For_Thread_Fun: Start");
   
   Queue_Size = 5;

   if (!SAL_Create_Queue_For_Thread(RECEIVER_THREAD, Queue_Size, size->alloc_size, malloc, free))
   {
      PBC_Failed("SAL_Create_Queue_For_Thread: The queue for thread has not been created");
   }

   Max_Message_Count = Message_Count = 5;

   /* sending data */
   if (size->max_buffer_size > 0)
   {
      data1 = (Sample_Msg_T1*)malloc(sizeof(Sample_Msg_T1));
      data2 = (Sample_Msg_T2*)malloc(sizeof(Sample_Msg_T2));

      if (data1 != NULL)
      {
         data1->field1 = 123;
         data1->field2 = 456;
         size_of_1 = sizeof(Sample_Msg_T1);
      }
      else
      {
         size_of_1 = 0;
      }

      if (data2 != NULL)
      {
         data2->field1 = 123;
         data2->field2 = 456;
         data2->field3 = 789;
         size_of_2 = sizeof(Sample_Msg_T2);
      }
      else
      {
         size_of_2 = 0;
      }
   }
   /* sending no data */
   else
   {
      data1 = NULL;
      data2 = NULL;
      size_of_1 = size_of_2 = 0;
   }

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data1, size_of_1);

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data2, size_of_2);

   for (i = 0; i < Message_Count - 2; i++)
   {
      SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data1, size_of_1);
   }

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Receive_For_Thread_Fun, 
      (void*)size, 
      Init_Thread_Attr("RECEIVER", RECEIVER_THREAD, priority, &thread_attr));

   free(data1);
   free(data2);

   Tr_Info_Lo("Send_For_Thread_Fun: Stop");
}


/**  Function tests SAL_Create_Queue_For_Thread function and SAL_Stat_Message_Queue struct.
 *
 *   Creates threads and starts SAL_Run.
 */

SAL_Int_T Queue_For_Thread_Test(Sizes_T* size)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, RECEIVER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Send_For_Thread_Fun, 
      (void*)size,
      Init_Thread_Attr("SENDER", SENDER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

#endif

/** \name Functions of Queue_Test
 *
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates the queue, receives messages and checks SAL_Stat_Queue_T fields
 */

void Receive_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { SENDER_THREAD };
   SAL_Stat_Queue_T stat;
   const SAL_Message_T* msg;
   Sizes_T size = *(Sizes_T*)param;

   Tr_Info_Lo("Receive_Fun: Start");

   Queue_Size = 5;

   if (!SAL_Create_Queue(Queue_Size, size.alloc_size, malloc, free))
   {
      PBC_Failed("SAL_Create_Queue: Queue has not been created");
   }

   SAL_Signal_Ready();

   if (!SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat))
   {
      PBC_Failed("SAL_Stat_Queue: Queue has not been created");
   }

   if (stat.queue_size != Queue_Size)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid parameter of queue size");
   }

   if (stat.buffer_size != size.alloc_size)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid parameter of buffer size");
   }

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

#if defined (XSAL)

   msg = SAL_Receive();

   PBC_Ensure(msg != NULL, "NULL XSAL message");

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      Tr_Fault("Timeout has been reached");
   }

#endif

   Message_Count--;

#if defined (XSAL)

   msg = SAL_Receive();

   PBC_Ensure(msg != NULL, "NULL XSAL message");

#else

   msg = SAL_Receive_Timeout(Timeout);

   if (msg == NULL)
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Message_Count--;

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

   if (stat.peak_message_count != Max_Message_Count)
   {
      PBC_Failed_1("SAL_Stat_Queue: Invalid max number of messages in the queue: %d", 
                                                (int)stat.peak_message_count);
   }

   if (stat.message_count != Message_Count)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid number of messages in the queue");
   }

   if (stat.peak_message_size != size.max_buffer_size)
   {
      PBC_Failed("SAL_Stat_Queue: Invalid parameter of max message size");
   }

   Tr_Info_Lo("Receive_Fun: Stop");
}



/**  Function of SENDER_THREAD.  
 *
 *   - sends messages to the RECEIVER_THREAD
 */

void Send_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { RECEIVER_THREAD };
   SAL_Thread_Id_T destroyed_list [] = { RECEIVER_THREAD };
   Sample_Msg_T1* data1;
   Sample_Msg_T2* data2;
   size_t i;
   size_t size_of_1, size_of_2;
   SAL_Int_T data_bool = (intptr_t)param;

   Tr_Info_Lo("Send_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }
 
#endif

   Max_Message_Count = Message_Count = 5;

   /* sending data */
   if (data_bool)
   {
      data1 = (Sample_Msg_T1*) malloc(sizeof(Sample_Msg_T1));
      data2 = (Sample_Msg_T2*) malloc(sizeof(Sample_Msg_T2));

      if (data1 != NULL)
      {
         data1->field1 = 123;
         data1->field2 = 456;
         size_of_1 = sizeof(Sample_Msg_T1);
      }
      else
      {
         size_of_1 = 0;
      }

      if (data2 != NULL)
      {
         data2->field1 = 123;
         data2->field2 = 456;
         data2->field3 = 789;
         size_of_2 = sizeof(Sample_Msg_T2);
      }
      else
      {
         size_of_2 = 0;
      }
   }
   /* sending no data */
   else
   {
      data1 = NULL;
      data2 = NULL;
      size_of_1 = size_of_2 = 0;
   }

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data1, size_of_1);

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data2, size_of_2);

   for (i = 0; i < Message_Count - 2; i++)
   {
      SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, data1, size_of_1);
   }

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   free(data1);
   free(data2);

   Tr_Info_Lo("Send_Fun: Stop");
}


/**  Function tests SAL_Create_Queue function and SAL_Stat_Message_Queue struct.
 *
 *   Creates threads and starts SAL_Run.
 */

SAL_Int_T Queue_Test(Sizes_T size)
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, RECEIVER_THREAD };
   SAL_Int_T ret;
   SAL_Int_T param;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   /* sending data */
   if (size.max_buffer_size > 0)
   {
      param = 1;
   }
   /* sending no data */
   else
   {
      param = 0;
   }

   SAL_Create_Thread(
      Send_Fun, 
      (void*)(intptr_t)param,
      Init_Thread_Attr("SENDER", SENDER_THREAD, priority, &thread_attr));

    SAL_Create_Thread(
       Receive_Fun, 
       (void*) (intptr_t)&size,
       Init_Thread_Attr("RECEIVER", RECEIVER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      Tr_Fault("Timeout has been reached");
      ret = 1;
   }

#endif

   return ret;
}

/** \}
 */


#if defined (XSAL)

/** \name Functions of Queue_Iterator_Test
 *
 *  \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue and after receiving the messages checks all queue iterator functions
 */

void Receive_Iter_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { SENDER_THREAD };
   SAL_Queue_Iterator_T iter;
   const SAL_Message_T* msg1;
   SAL_Stat_Queue_T stat;

   Tr_Info_Lo("Receive_Iter_Fun: Start");

   SAL_Create_Queue(8, sizeof(Sample_Msg_T1), malloc, free);

   SAL_Signal_Ready();

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_First_Message(&iter);

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E1)
   {
      PBC_Failed("SAL_First_Message: Function doesn't return iterator to the first queue element");
   }

   if (!SAL_Is_Queue_Begin(&iter))
   {
      PBC_Failed("SAL_Is_Queue_Begin: returns 0 when iterator points to the beginning of the queue");
   }

   if (SAL_Is_Queue_End(&iter))
   {
      PBC_Failed("SAL_Is_Queue_End: returns 1 when iterator doesn't point to element after the last queue element");
   }

   SAL_Next_Message(&iter);

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E2)
   {
      PBC_Failed("SAL_Next_Message: Function doesn't return the next queue element");
   }

   SAL_Prev_Message(&iter);

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E1)
   {
      PBC_Failed("SAL_Prev_Message: Function doesn't return the previous queue element");
   }

   SAL_Last_Message(&iter);

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E6)
   {
      PBC_Failed("SAL_Last_Message: Function doesn't return the last queue element");
   }

   SAL_Queue_End(&iter);

   SAL_Prev_Message(&iter);

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E6)
   {
      PBC_Failed("SAL_Queue_End: Function doesn't return the element after the last queue element");
   }

   SAL_Next_Message(&iter);

   if (!SAL_Is_Queue_End(&iter))
   {
      PBC_Failed("SAL_Queue_End: returns 0 when iterator points to the element after last queue element");
   }
   
   if (SAL_Is_Queue_Begin(&iter))
   {
      PBC_Failed("SAL_Queue_Begin: returns 1 when iterator doesn't point to the beginning of the queue");
   }

   /* SAL_Remove_Message test */

   do
   {
      SAL_Prev_Message(&iter);

      msg1 = SAL_Get_Message(&iter);
   }
   while (msg1->event_id != (SAL_Event_Id_T)EV_E3);

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

   if (stat.message_count != 6)
   {
      PBC_Failed("Invalid message count in the queue");
   }

   SAL_Remove_Message(&iter);

   SAL_Stat_Queue(SAL_Get_Thread_Id(), &stat);

   if (stat.message_count != 5)
   {
      PBC_Failed_1("Invalid message count in the queue: %d", (int)stat.message_count);
   }

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E4)
   {
      PBC_Failed("SAL_Remove_Message: Invalid current message");
   }

   SAL_Prev_Message(&iter);

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E2)
   {
      PBC_Failed("SAL_Remove_Message: Invalid prevoius message");
   }

   SAL_Next_Message(&iter);

   msg1 = SAL_Get_Message(&iter);

   if (msg1->event_id != (SAL_Event_Id_T)EV_E4)
   {
      PBC_Failed("SAL_Remove_Message: Invalid next message");
   }

   Tr_Info_Lo("Receive_Iter_Fun: Stop");
}

/**  Function of SENDER_THREAD.
 *
 *   - sends messages to the RECEIVER_THREAD
 */

void Send_Iter_Fun(void* param)
{
   SAL_Thread_Id_T wait_list [] = { RECEIVER_THREAD };
   SAL_Thread_Id_T destroyed_list [] = { RECEIVER_THREAD };
   Sample_Msg_T1 data;

   Tr_Info_Lo("Send_Iter_Fun: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   data.field1 = 111;
   data.field2 = 222;

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E1, &data, sizeof(Sample_Msg_T1));

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E2, &data, sizeof(Sample_Msg_T1));

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E3, &data, sizeof(Sample_Msg_T1));

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E4, &data, sizeof(Sample_Msg_T1));

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E5, &data, sizeof(Sample_Msg_T1));

   SAL_Send(APP_ID, RECEIVER_THREAD, EV_E6, &data, sizeof(Sample_Msg_T1));

   SAL_Signal_Ready();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Send_Iter_Fun: Stop");
}


/**  Test of the queue iterator functions.
 *
 *   Creates threads and starts SAL_Run.
 */

SAL_Int_T Queue_Iterator_Test()
{
   SAL_Thread_Id_T destroyed_list[] = { SENDER_THREAD, RECEIVER_THREAD };
   SAL_Int_T ret;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(
      Send_Iter_Fun, 
      NULL, 
      Init_Thread_Attr("SENDER", SENDER_THREAD, priority, &thread_attr));

   SAL_Create_Thread(
      Receive_Iter_Fun, 
      NULL, 
      Init_Thread_Attr("RECEIVER", RECEIVER_THREAD, priority, &thread_attr));

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

#endif


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
  
#if defined (XSAL)

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_For_Thread_Test(&Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_For_Thread_Test(&Size);

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_For_Thread_Test(&Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_For_Thread_Test(&Size);

#endif

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_Test(Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending no data */
   Size.max_buffer_size = 0;

   ret = ret || Queue_Test(Size);

   /* allocation size == 0 */
   Size.alloc_size = 0;
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_Test(Size);

   /* allocation size != 0 */
   Size.alloc_size = sizeof(Sample_Msg_T1);
   /* sending data */
   Size.max_buffer_size = sizeof(Sample_Msg_T2);

   ret = ret || Queue_Test(Size);

#if defined (XSAL)
   
   ret = ret || Queue_Iterator_Test();

#endif

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
 * - 30-Aug-2014 Kirk Bailey
 *   - Fixed Klocwork defects.
 *
 * -----------------------------------------------------------------------*/
