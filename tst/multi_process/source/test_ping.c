/*===========================================================================*/
/**
 * @file test_ping.c
 *
 * Test XSAL message rate between two threads
 *
 * %full_filespec:test_ping.c~kok_inf#7:csrc:kok_basa#1 %
 * @version %version:kok_inf#7 %
 * @author  %derived_by:jz4fpf %
 * @date    %date_modified:Mon Mar 16 14:08:41 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * * @section DESC DESCRIPTION:
 *
 *    Test scheculing of equal priority tasks.  Creates a set of equal priority 
 *    tasks that do nothing but increment different counters.  If cpu is being f
 *    shared fairly, then all counts should be approxiamately the same.  The
 *    countrol thread prints out all counter every half second.  
 *
 * @section ABBR ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * @section TINFO TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/
#include "pbc_trace.h"
#include "xsal.h"
#include "pi_unistd.h"

EM_FILENUM(TEST_MODULE_ID, 6); /* define this file for assert handling */

/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/

#define PONG_TIMEOUT       (1000)
#define PING_TIMEOUT       (1000)
#define NUMBER_RUNS        30

/*===========================================================================*\
 * Local Preprocessor #define MACROS
 \*===========================================================================*/

/*===========================================================================*\
 * Local Type Declarations
 \*===========================================================================*/

#define APP_PING    3
#define APP_PONG    4
#define APP_LOCAL   2

/* Thread ids */
typedef enum Thread_Id_Tag
{
   PING_THREAD = 1,
   PONG_THREAD, 
   
   LAST_THREAD_ID
} Thread_Id_T;

typedef enum Test_Private_Event_Id_Tag
{
   TEST_EV_TIMER_1 = SAL_EV_FIRST_PRIVATE_ID,
   TST_EV_PING,
   TST_EV_PONG,
   TST_EV_END
}
Test_Private_Event_Id_T;

typedef enum E_Test_Type_Tag
{
   E_STAND_ALONE, 
   E_PING, 
   E_PONG 
} E_Test_Type_T;

/*===========================================================================*\
 * Exported Const Object Definitions
 \*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
 \*===========================================================================*/

E_Test_Type_T Test_Type = E_STAND_ALONE;

/*===========================================================================*\
 * Local Function Prototypes
 \*===========================================================================*/
static void Pong_Thread(void * param);
static void Ping_Thread(void * param);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
 \*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

/**
 * Echo Thread - replies to ping message with pong message 
 * 
 * @param ctr pointer to counter for task to increment
 */
static void Pong_Thread(void * param)
{
   bool_t run = true;
   const SAL_Message_T *msg;
   uint32_t msg_count;

   /* create message queue */
   SAL_Create_Queue(5, 16, SAL_Alloc, SAL_Free);

   SAL_Signal_Ready();
   Tr_Notify("Pong Started");

   while (run)
   {
      msg = SAL_Receive_Timeout(PONG_TIMEOUT);

      if (msg != NULL)
      {
         switch ((int) msg->event_id)
         {
            case TST_EV_PING:
               msg_count = *((uint32_t *) msg->data);
               msg_count++;
               SAL_Send(msg->sender_app_id, msg->sender_thread_id, TST_EV_PONG, &msg_count, 4);
               break;
            case TST_EV_END:
               Tr_Notify("Received End Pong");
              run = false;
               break;
            default:
               Tr_Fault_3("Pong Unknown Event ID %d From %d:%d", msg->event_id, msg->sender_app_id, msg->sender_thread_id);
               break;
         }
      }
   }
}

static void Ping_Thread(void * param)
{
   int test_run;
   bool_t run;
   const SAL_Message_T *msg;
   uint32_t msg_count = 0;
   SAL_Timer_Id_T test_timer;
   uint32_t total_msg_count = 0;
   SAL_App_Id_T pong_app = (intptr_t) param;
   SAL_Thread_Id_T wait_threads[] = { PONG_THREAD };

   /* create message queue */
   SAL_Create_Queue(5, 16, SAL_Alloc, SAL_Free);

   SAL_Signal_Ready();

   if (SAL_Get_App_Id() == pong_app)
   {
      bool_t ok = SAL_Wait_Ready_Timeout(wait_threads, 1, 10000);
      PBC_Ensure(ok, "Pong_app timed out waiting for PONG_THREAD");
   }

   (void) SAL_Send(pong_app, PONG_THREAD, TST_EV_PING, &msg_count, sizeof(msg_count));

   (void) SAL_Create_Timer(TEST_EV_TIMER_1, &test_timer);
   (void) SAL_Start_Timer(test_timer, 1000, true);

   for (test_run = 0; test_run < NUMBER_RUNS; test_run++)
   {
      run = true;

      while (run)
      {
         msg = SAL_Receive_Timeout(PING_TIMEOUT);

         if (msg != NULL)
         {
            switch ((enum Test_Private_Event_Id_Tag)msg->event_id)
            {
               case TST_EV_PONG:
                  if (msg_count != 0)
                  {
                     msg_count = *(uint32_t *) msg->data;
                  }
                  msg_count++;
                  SAL_Send(pong_app, PONG_THREAD, TST_EV_PING, &msg_count, sizeof(msg_count));
                  break;
               case TEST_EV_TIMER_1:
                  Tr_Warn_1("Messages per second : %d", msg_count);
                  total_msg_count += msg_count;
                  msg_count = 0;
                  run = false;
                  break;
               default:
                  Tr_Fault_3("Ping Unknown Event ID %d From %d:%d", msg->event_id, msg->sender_app_id, msg->sender_thread_id);
                  break;
            }
         }
         else
         {
            Tr_Fault("No response from PONG");
            SAL_Send(pong_app, PONG_THREAD, TST_EV_PING, &msg_count, sizeof(msg_count));
         }
      }
   }

   /* wait  for last PONG response */
   msg = SAL_Receive_Timeout(PING_TIMEOUT);

   SAL_Destroy_Timer(test_timer);
   SAL_Send(pong_app, PONG_THREAD, TST_EV_END, NULL, 0);
   Tr_Warn_1("Average messages per second : %d", total_msg_count / NUMBER_RUNS);
}

/**  Test of SAL_Send function in local or remote communication. 
 */
static const SAL_Thread_Attr_T* Init_Thread_Attr(const char* name, SAL_Thread_Id_T id, SAL_Priority_T prior,
   SAL_Thread_Attr_T* attr)
{
   SAL_Init_Thread_Attr(attr);

   attr->name = name;
   attr->id = id;
   attr->priority = prior;

   return attr;
}

static SAL_Int_T Ping_Test_Suite()
{
   SAL_Thread_Id_T destroyed_list[] =
   { PING_THREAD, PONG_THREAD };
   SAL_Int_T retcode;
   SAL_Priority_T priority;
   SAL_Thread_Attr_T thread_attr;

   SAL_Get_Thread_Priority(SAL_Get_Thread_Id(), &priority);

   switch (Test_Type)
   {

#if defined (XSAL)
      /* remote communication - Ping */
      case E_PING:
         Tr_Notify("Ping App");
         SAL_Create_Thread(Ping_Thread, (void *) APP_PONG,
            Init_Thread_Attr("PING", PING_THREAD, priority, &thread_attr));
         break;

         /* remote communication - Pong (echo process) */
      case E_PONG:
         Tr_Notify("Pong App");
         SAL_Create_Thread(Pong_Thread, NULL, Init_Thread_Attr("PONG", PONG_THREAD, priority, &thread_attr));
         break;
#endif

      default:
         Tr_Notify("Local Ping / Pong");
         /* local communication */
         SAL_Create_Thread(Ping_Thread, (void *) APP_LOCAL, Init_Thread_Attr("PING", PING_THREAD, priority,
            &thread_attr));
         SAL_Create_Thread(Pong_Thread, NULL, Init_Thread_Attr("PONG", PONG_THREAD, priority, &thread_attr));
   }

   retcode = SAL_Run();

   if (!SAL_Wait_Destroyed_Timeout(destroyed_list, sizeof(destroyed_list) / sizeof(SAL_Thread_Id_T), 60000))
   {
      Tr_Fault("Timeout has been reached");
      retcode = 1;
   }

   if (retcode != 0)
   {
      Tr_Fault_1("Send_Test: SAL_Run() return %d", (int)retcode);
      return 1;
   }

   return 0;
}

SAL_Int_T Set_Up(SAL_Int_T argc, char** argv)
{
   SAL_Int_T option;
   SAL_Config_T config;

   while ((option = pi_getopt(argc, argv, "srv:g:l:teN:")) != -1)
   {
      switch (option)
      {
#if defined (XSAL)
         case 's':
            Test_Type = E_PING;
            break;

         case 'r':
            Test_Type = E_PONG;
            break;
#endif
         default:
            break;
      }
   }

   SAL_Get_Config(&config);

   switch (Test_Type)
   {

#if defined (XSAL)
      case E_PING:
         config.app_id = APP_PING;
         config.max_number_of_threads = LAST_THREAD_ID;
         break;

      case E_PONG:
         config.app_id = APP_PONG;
         config.max_number_of_threads = LAST_THREAD_ID;
         break;
#endif

      default:
         config.app_id = APP_LOCAL;
         config.max_number_of_threads = LAST_THREAD_ID + 2;
   }

   if (!SAL_Init(&config))
   {
      Tr_Fault("SAL_Init error");
      return 1;
   }

   return 0;
}

SAL_Int_T Tear_Down(void)
{
   return 0;
}

int main(int argc, char** argv)
{
   SAL_Int_T ret = 0;

   if (Set_Up(argc, argv))
   {
      Tr_Fault("Set_Up failed");
      return 1;
   }

   ret = ret || Ping_Test_Suite();

   if (Tear_Down())
   {
      Tr_Fault("Tear_Down failed");
      return 1;
   }

   Tr_Info_Hi_1("Test %s", ((ret)?("FAILED"):("PASSED")));

   return ret;
}

/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 16-Oct-2010 Dan Carman
 * + Created initial file.
 *
 * - 30-Aug-2014 Kirk Bailey Rev 6
 *   - Fixed Klocwork defects.
 *
 \*===========================================================================*/
