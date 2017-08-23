
/* -------------------------------------------------------------------------
 *
 * File:        test_callback.c
 *
 * Copyright:   2006-2014 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Urszula Gessing (Urszula.Gessing@delphi.com)
 *
 * Description: Unit tests of callback for local publish.
 *
 * -----------------------------------------------------------------------*/

#include "pbc_trace.h"
#include <stdlib.h>
#include "xsal.h"

EM_FILENUM(TEST_MODULE_ID, 13);

#define APP_ID 2

#define RECEIVER_NUMB  3

#define MAX_NUMB_OF_THREADS 4

typedef enum Event_Id_Tag {
   EV_E1 = 1,
   EV_E2,
   EV_E3
} Event_Id_T;

typedef enum Thread_Id_Tag
{
   PUBLISHER_THREAD = 1,
   RECEIVER_THREAD_1,
   RECEIVER_THREAD_2,
   RECEIVER_THREAD_3
} Thread_Id_T;

typedef struct Pointer_Tag 
{
   void* malloc_pointer;
   void* free_pointer;
} Pointer_T;

SAL_Int_T Timeout = 3000;


/**  \file
 *
 *   Test of callback for local publish.
 *
 *   \section T2 Tests:
 *   <OL>
 *   <LI> \ref NQT - test of no queue callback. </LI>
 *   <LI> \ref QFT - test of queue full callback. </LI>
 *   <LI> \ref NMT - test of no memory callback. </LI>
 *   <LI> \ref TSBT - test of too small buffer callback. </LI>
 *   </OL>
 *
 *   \section NQT No_Queue_Test 
 *
 *   Four threads are created: PUBLISHER_THREAD, RECEIVER_THREAD_1,
 *   RECEIVER_THREAD_2 and RECEIVER_THREAD_3. \n
 *   PUBLISHER_THREAD publishes message locally to RECEIVER_THREADs. First two
 *   receiver threads have not created the queues.
 * 
 *   \subsection TP1 Test procedure:
 *
 *   <OL>
 *   <LI> PUBLISHER_THREAD waits until RECEIVER_THREADs are ready. </LI>
 *   <LI> One of three RECEIVER_THREADs creates the queue. </LI>
 *   <LI> RECEIVER_THREADs subscribe on EV_E1 event. </LI>
 *   <LI> RECEIVER_THREADs signal that are ready. </LI>
 *   <LI> RECEIVER_THREADs wait until PUBLSHER_THREAD terminates. </LI>
 *   <LI> PUBLISHER_THREADs publishes message locally. </LI>
 *   </OL>
 *
 *   \section QFT Queue_Full_Test 
 *
 *   Four threads are created: PUBLISHER_THREAD, RECEIVER_THREAD_1,
 *   RECEIVER_THREAD_2 and RECEIVER_THREAD_3. \n
 *   RECEIVER_THREADs create queues with sizes 1, 2 and 3.
 *   PUBLISHER_THREAD publishes 2 messages locally, so the number of messages
 *   is greater than one RECEIVER_THREAD's queue_size (with queue size = 1).
 *
 *   \subsection TP2 Test procedure:
 *
 *   <OL>
 *   <LI> PUBLISHER_THREAD waits until RECEIVER_THREADs are ready. </LI>
 *   <LI> RECEIVER_THREADs creates the queues of sizes 1, 2 and 3. </LI>
 *   <LI> RECEIVER_THREADs subscribe on EV_E1 and EV_E2 events. </LI>
 *   <LI> RECEIVER_THREADs signal that are ready. </LI>
 *   <LI> RECEIVER_THREADs wait until PUBLISHER_THREAD terminates. </LI>
 *   <LI> PUBLISHER_THREAD publishes 2 messages locally. </LI>
 *   </OL>
 *
 *   \section NMT No_Memory_Test 
 *
 *   Four threads are created: PUBLISHER_THREAD, RECEIVER_THREAD_1,
 *   RECEIVER_THREAD_2 and RECEIVER_THREAD_3. \n
 *   PUBLISHER_THREAD locally publishes the message of greater size that 
 *   is allocated and there is no memory for allocation. As the third 
 *   argument of SAL_Create_Queue function there is Null_Fun which only 
 *   returns 0 (this is simulation of situation when there is no memory 
 *   to allocate).
 *   The first 2 RECEIVER_THREADs uses Null_Fun function for allocation 
 *   and the third one uses malloc function. So the situation of No_Memory
 *   takes place only for the first 2 functions.
 *
 *   \subsection TP3 Test procedure:
 *
 *   <OL>
 *   <LI> PUBLISHER_THREAD waits until RECEIVER_THREADs are ready. </LI>
 *   <LI> RECEIVER_THREADs create the queues with buffer_size = 0 and
 *        allocation function Null_Fun or malloc. </LI>
 *   <LI> RECEIVER_THREADs subscribe on EV_E1 and EV_E2 events. </LI>
 *   <LI> RECEIVER_THREADs signal that are ready. </LI>
 *   <LI> RECEIVER_THREADs wait until PUBLISHER_THREAD terminates. </LI>
 *   <LI> PUBLISHER_THREAD locally publishes message of size SAL_Int_T type. </LI>
 *   </OL>
 *
 *   \section TSBT Too_Small_Buffer_Test 
 *
 *   Four threads are created: PUBLISHER_THREAD, RECEIVER_THREAD_1,
 *   RECEIVER_THREAD_2 and RECEIVER_THREAD_3. \n
 *   PUBLISHER_THREAD locally publishes message of greater size that is allocated.
 *   First two RECEIVER_THREADs create queue of buffer_size = 0, the third
 *   RECEIVER_THREAD creates queue of buffer_size equal published message size.
 *
 *   \subsection TP4 Test procedure:
 *
 *   <OL>
 *   <LI> PUBLISHER_THREAD waits until RECEIVER_THREADs are ready. </LI>
 *   <LI> RECEIVER_THREAD creates the queue with buffer_size = 0 and 
 *        sizeof(SAL_Int_T). </LI>
 *   <LI> RECEIVER_THREADs subscribe on event EV_E1. </LI>
 *   <LI> RECEIVER_THREADs signal that are ready. </LI>
 *   <LI> RECEIVER_THREADs wait until PUBLISHER_THREAD terminates. </LI>
 *   <LI> PUBLISHER_THREAD locally publishes message of size SAL_Int_T type. </LI>
 *   </OL>
 *
 */




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

void Callback_Fun(
   SAL_Thread_Id_T thread_id, 
   SAL_Message_Dropped_Reason_T reason,
   const SAL_Message_T* msg)
{
   Tr_Fault_2("Message dropped in thread: %d, reason: %d", (int)thread_id, (int)reason);
}



/**  \name Functions of Local_No_Queue_Test
 *
 *   \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - it depends on param if thread creates queue 
 *   - subscribes on events
 *   - signals that is ready
 *   - waits until PUBLISHER_THREAD terminates
 */

void Local_Receiver_No_Queue_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { PUBLISHER_THREAD };
   SAL_Event_Id_T subscribe_list[] = { EV_E1 };

   Tr_Info_Lo("Local_Receiver_No_Queue_Fun: Start");

   if (param)
   {
      if (!SAL_Create_Queue(1, sizeof(SAL_Int_T), malloc, free))
      {
         PBC_Failed("Queue creation error");
      }
   }

   SAL_Subscribe(subscribe_list, sizeof(subscribe_list)/sizeof(SAL_Event_Id_T));

   SAL_Signal_Ready();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Local_Receiver_No_Queue_Fun: Stop");
}

/**  Function of PUBLISHER_THREAD.
 *
 *   - waits until RECEIVER_THREADs are ready
 *   - publishes the message
 */

void Local_Publisher_No_Queue_Fun(void* param)
{
   SAL_Thread_Id_T wait_list[] = { RECEIVER_THREAD_1, 
                                   RECEIVER_THREAD_2, 
                                   RECEIVER_THREAD_3 };

   Tr_Info_Lo("Local_Publisher_No_Queue_Fun: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_Local_Publish(EV_E1, NULL, 0);

   Tr_Info_Lo("Local_Publisher_No_Queue_Fun: Stop");
}

/**  Test of SAL_No_Queue callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_No_Queue_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD_1, 
                                        RECEIVER_THREAD_2,
                                        RECEIVER_THREAD_3,
                                        PUBLISHER_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Local_Receiver_No_Queue_Fun, 
                     (void*)0, 
                     Init_Thread_Attr("LPRec11", RECEIVER_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(Local_Receiver_No_Queue_Fun, 
                     (void*)0, 
                     Init_Thread_Attr("LPRec12", RECEIVER_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(Local_Receiver_No_Queue_Fun, 
                     (void*)1, 
                     Init_Thread_Attr("LPRec13", RECEIVER_THREAD_3, priority, &thread_attr));

   SAL_Create_Thread(Local_Publisher_No_Queue_Fun, 
                     NULL, 
                     Init_Thread_Attr("LPub1", PUBLISHER_THREAD, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/**  \}
 */


/**  \name Functions of Local_Queue_Full_Test
 *
 *   \{
 */

/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue of size 1, 2 or 3
 *   - subscribes on events
 *   - signals that is ready
 *   - waits until PUBLISHER_THREAD terminates
 */

void Local_Receiver_Queue_Full_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { PUBLISHER_THREAD };
   SAL_Event_Id_T subscribe_list[] = { EV_E1, EV_E2, EV_E3 };
   size_t queue_size = (size_t)param;

   Tr_Info_Lo("Local_Receiver_Queue_Full_Fun: Start");

   if (!SAL_Create_Queue(queue_size, 0, malloc, free))
   {
      PBC_Failed("Queue creation error");
   }

   SAL_Subscribe(subscribe_list, sizeof(subscribe_list)/sizeof(SAL_Event_Id_T));

   SAL_Signal_Ready();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Local_Receiver_Queue_Full_Fun: Stop");
}

/**  Function of PUBLISHER_THREAD.
 *
 *   - waits until RECEIVER_THREADs are ready
 *   - publishes messages
 */

void Local_Publisher_Queue_Full_Fun(void* param)
{
   SAL_Thread_Id_T wait_list[] = { RECEIVER_THREAD_1, 
                                   RECEIVER_THREAD_2, 
                                   RECEIVER_THREAD_3 };

   Tr_Info_Lo("Local_Publisher_Queue_Full_Fun: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   SAL_Local_Publish(EV_E1, NULL, 0);
   SAL_Local_Publish(EV_E2, NULL, 0);
   SAL_Local_Publish(EV_E3, NULL, 0);

   Tr_Info_Lo("Local_Publisher_Queue_Full_Fun: Stop");
}

/**  Test of SAL_Queue_Full callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_Queue_Full_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD_1, 
                                        RECEIVER_THREAD_2,
                                        RECEIVER_THREAD_3,
                                        PUBLISHER_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Local_Receiver_Queue_Full_Fun, 
                     (void*)1, 
                     Init_Thread_Attr("LPRec21", RECEIVER_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(Local_Receiver_Queue_Full_Fun, 
                     (void*)2, 
                     Init_Thread_Attr("LPRec22", RECEIVER_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(Local_Receiver_Queue_Full_Fun, 
                     (void*)3, 
                     Init_Thread_Attr("LPRec23", RECEIVER_THREAD_3, priority, &thread_attr));

   SAL_Create_Thread(Local_Publisher_Queue_Full_Fun, 
                     NULL, 
                     Init_Thread_Attr("LPub2", PUBLISHER_THREAD, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/**  \}
 */


/**  \name Functions of Local_No_Memory_Test
 *
 *   \{
 */

/**  Function of allocation memory.
 *
 *   - returns NULL
 */

void* Null_Fun(size_t size)
{
   return NULL;
}

/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue of buffer_size = 0 and with allocation 
 *     function dependent on param (Null_Fun or malloc)
 *   - subscribes on events
 *   - signals that is ready
 *   - waits until PUBLISHER_THREAD terminates
 */

void Local_Receiver_No_Memory_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { PUBLISHER_THREAD };
   SAL_Event_Id_T subscribe_list[] = { EV_E1 };
   Pointer_T* pointers = (void*)param;

   Tr_Info_Lo("Local_Receiver_No_Memory_Fun: Start");

   if (!SAL_Create_Queue(1, 0, pointers->malloc_pointer, pointers->free_pointer))
   {
      PBC_Failed("Queue creation error");
   }

   SAL_Subscribe(subscribe_list, sizeof(subscribe_list)/sizeof(SAL_Event_Id_T));

   SAL_Signal_Ready();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   Tr_Info_Lo("Local_Receiver_No_Memory_Fun: Stop");

}

/**  Function of PUBLISHER_THREAD.
 *
 *   - waits until RECEIVER_THREADs are ready
 *   - publishes the message
 */

void Local_Publisher_No_Memory_Fun(void* param)
{
   SAL_Int_T data;
   SAL_Thread_Id_T wait_list[] = { RECEIVER_THREAD_1, 
                                   RECEIVER_THREAD_2, 
                                   RECEIVER_THREAD_3 };

   Tr_Info_Lo("Local_Publisher_No_Memory_Fun: Start");

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

   data = 5;

   SAL_Local_Publish(EV_E1, &data, sizeof(SAL_Int_T));

   Tr_Info_Lo("Local_Publisher_No_Memory_Fun: Stop");
}

/**  Test of SAL_No_Memory callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_No_Memory_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD_1,
                                        RECEIVER_THREAD_2, 
                                        RECEIVER_THREAD_3,
                                        PUBLISHER_THREAD };
   Pointer_T pointers_1, pointers_2;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   pointers_1.malloc_pointer = Null_Fun;
   pointers_1.free_pointer = NULL;

   SAL_Create_Thread(Local_Receiver_No_Memory_Fun, 
                     (void*)&pointers_1,
                     Init_Thread_Attr("LPRec31", RECEIVER_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(Local_Receiver_No_Memory_Fun, 
                     (void*)&pointers_1,
                     Init_Thread_Attr("LPRec32", RECEIVER_THREAD_2, priority, &thread_attr));

   pointers_2.malloc_pointer = malloc;
   pointers_2.free_pointer = free;

   SAL_Create_Thread(Local_Receiver_No_Memory_Fun, 
                     (void*)&pointers_2,
                     Init_Thread_Attr("LPRec33", RECEIVER_THREAD_3, priority, &thread_attr));

   SAL_Create_Thread(Local_Publisher_No_Memory_Fun, 
                     NULL, 
                     Init_Thread_Attr("LPub3", PUBLISHER_THREAD, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

   return ret;
}

/** \}
 */

/** \name Functions of Local_Too_Small_Buffer_Test
 *
 *  \{
 */


/**  Function of RECEIVER_THREAD.
 *
 *   - creates queue with no allocation function and
 *     buffer_size dependent on param
 *   - subscribes on events
 *   - signals that is ready
 *   - waits until PUBLISHER_THREAD terminates
 */

void Local_Receiver_Too_Small_Buffer_Fun(void* param)
{
   SAL_Thread_Id_T destroyed_list[] = { PUBLISHER_THREAD };
   SAL_Event_Id_T subscribe_list[] = { EV_E1 };
   size_t buffer_size = (size_t)param;

   Tr_Info_Lo("Local_Receiver_Too_Small_Buffer_Fun: Start");

   if (!SAL_Create_Queue(1, buffer_size, NULL, NULL))
   {
      PBC_Failed("Queue has not been created");
   }

   SAL_Subscribe(subscribe_list, sizeof(subscribe_list)/sizeof(SAL_Event_Id_T));

   SAL_Signal_Ready();

#if defined (XSAL)

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   Tr_Info_Lo("Local_Receiver_Too_Small_Buffer_Fun: Stop");

}

/**  Function of PUBLISHER_THREAD.
 *
 *   - waits until RECEIVER_THREADs are ready
 *   - publishes the message
 */

void Local_Publisher_Too_Small_Buffer_Fun(void* param)
{
   SAL_Int_T* data;
   SAL_Thread_Id_T wait_list [] = { RECEIVER_THREAD_1, 
                                    RECEIVER_THREAD_2, 
                                    RECEIVER_THREAD_3 };

   Tr_Info_Lo("Local_Publisher_Too_Small_Buffer_Fun: Start");

#if defined (XSAL)

   SAL_Wait_Ready(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T));

#else

   if (!SAL_Wait_Ready_Timeout(wait_list, sizeof(wait_list)/sizeof(SAL_Thread_Id_T), Timeout))
   {
      PBC_Failed("Timeout has been reached");
   }

#endif

   data = (SAL_Int_T*) malloc(sizeof(SAL_Int_T));
   if (data != NULL)
   {
      *data = 5;
      SAL_Local_Publish(EV_E1, data, sizeof(SAL_Int_T));
      free(data);
   }

   Tr_Info_Lo("Local_Publisher_Too_Small_Buffer_Fun: Stop");
}

/**  Test of SAL_Too_Small_Buffer callback.
 *
 *   Function creates threads and starts SAL_Run.
 */

SAL_Int_T Local_Too_Small_Buffer_Test()
{
   SAL_Thread_Attr_T thread_attr;
   SAL_Priority_T priority;
   SAL_Int_T ret; 
   SAL_Thread_Id_T destroyed_list[] = { RECEIVER_THREAD_1, 
                                        RECEIVER_THREAD_2, 
                                        RECEIVER_THREAD_3, 
                                        PUBLISHER_THREAD };

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   SAL_Create_Thread(Local_Receiver_Too_Small_Buffer_Fun, 
                     (void*)0, 
                     Init_Thread_Attr("LPRec41", RECEIVER_THREAD_1, priority, &thread_attr));

   SAL_Create_Thread(Local_Receiver_Too_Small_Buffer_Fun,                      
                     (void*)0,
                     Init_Thread_Attr("LPRec42", RECEIVER_THREAD_2, priority, &thread_attr));

   SAL_Create_Thread(Local_Receiver_Too_Small_Buffer_Fun, 
                     (void*)sizeof(SAL_Int_T),
                     Init_Thread_Attr("LPRec43", RECEIVER_THREAD_3, priority, &thread_attr));

   SAL_Create_Thread(Local_Publisher_Too_Small_Buffer_Fun, 
                     NULL,
                     Init_Thread_Attr("LPub4", PUBLISHER_THREAD, priority, &thread_attr));

   SAL_Set_Message_Dropped_Callback(Callback_Fun);

   ret = SAL_Run();

   SAL_Wait_Destroyed(destroyed_list, sizeof(destroyed_list)/sizeof(SAL_Thread_Id_T));

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

   if (Set_Up(argc, argv))
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   ret = ret || Local_No_Queue_Test();
   ret = ret || Local_Queue_Full_Test();
   ret = ret || Local_No_Memory_Test();
   ret = ret || Local_Too_Small_Buffer_Test();

   Tear_Down();

   Tr_Info_Hi_1("Test %s", ((ret) ? ("FAILED") : ("PASSED")));

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
