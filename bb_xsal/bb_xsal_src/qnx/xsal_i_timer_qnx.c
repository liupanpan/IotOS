/* -------------------------------------------------------------------------
 *
 * File:        xsal_i_timer_qnx.c
 *
 * Copyright:   2006 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman@delphi.com)
 *
 * Description: Implementation of SAL Timer for QNX platform.
 *
 * -----------------------------------------------------------------------*/

#include "xsal.h"

#if 1 /*(QNX_NTO) XSAL_TIMER_IMPL == XSAL_TIMER_USE_PULSES */

#include "xsal_i_assert.h"
#include "xsal_i_engine.h"
#include "xsal_i_timer.h"
#include "xsal_i_thread.h"
#include "xsal_i_config.h"

#if defined (XSAL) && defined (QNX_NTO)
#include "xsal_i_connections.h"
#endif /* XSAL && QNX_NTO */

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/timeb.h>

EM_FILENUM(XSAL_TIME_MODULE_ID, 7); /* Identify file for trace/assert purposes */

/* ---------------------------
 *  Local Object Definitions
 * --------------------------- */

#if defined(XSAL_LIGHT)

SAL_Channel_T XSAL_I_RT_Channel;
SAL_Connection_T XSAL_I_RT_Connection;

#endif /* XSAL_LIGHT */

/* ----------------------------
 *  Local Function Prototypes
 * ----------------------------*/

#if defined (XSAL_LIGHT)

/** Function SAL_Lock_Mutex is not defined in  XSAL_LIGHT external
 *  interface but are used internally by the XSAL. 
 */
bool_t SAL_Lock_Mutex(SAL_Mutex_T* mutex);

bool_t SAL_Create_Channel(SAL_Channel_T* channel);

bool_t SAL_Destroy_Channel(SAL_Channel_T* channel);

bool_t SAL_Disconnect(SAL_Connection_T* connection);

bool_t SAL_Create_Connection(
   const SAL_Channel_T* channel,
   SAL_Connection_T* connection);

bool_t SAL_Send_Pulse(
   SAL_Connection_T* connection,
   uint8_t code,
   uint32_t value);

bool_t SAL_Sync_Receive(
   SAL_Channel_T* channel,
   void* buffer,
   size_t buffer_size,
   size_t* message_size,
   uint8_t* is_pulse,
   SAL_Receive_T* receive_id);

bool_t SAL_Sync_Reply(
   const SAL_Receive_T* receive_id,
   int32_t reply_status,
   const void* reply_message,
   size_t reply_message_size);

#endif /* XSAL_LIGHT */

/* -----------------------
 *  Function Definitions
 * -----------------------*/

bool_t SAL_I_Init_Timer_Module(void)
{
   bool_t status = false;

   if (SAL_I_Timers == NULL)
   {
      if (SAL_I_Max_Number_Of_Timers > 0u)
      {
         if (SAL_Create_Mutex(&SAL_I_Timers_Mutex, NULL))
         {
            SAL_I_Timers = malloc(SAL_I_Max_Number_Of_Timers * sizeof(SAL_I_Timer_T));
            if (SAL_I_Timers != NULL)
            {
               status = true;
            }
            else
            {
               Tr_Fault("SAL_I_Init_Timer_Module: alloc memory failed");
               (void) SAL_Destroy_Mutex(&SAL_I_Timers_Mutex);
            }
         }
         else
         {
            Tr_Fault("SAL_I_Init_Timer_Module/SAL_Create_Mutex() failed");
         }
      }
      else
      {
         status = true;
      }
   }
   else
   {
      Tr_Fault("SAL_I_Init_Timer_Module: Timer Module initialized second time without deinitialization");
   }

#if defined (XSAL_LIGHT)

   if (status)
   {
      if (SAL_Create_Channel(&XSAL_I_RT_Channel))
      {
         status = SAL_Create_Connection(&XSAL_I_RT_Channel, &XSAL_I_RT_Connection);
         if(!status)
         {  Tr_Fault("SAL_I_Init_Timer_Module/SAL_Create_Connection() failed");
            SAL_Destroy_Channel(&XSAL_I_RT_Channel);
            free(SAL_I_Timers);
            SAL_I_Timers = NULL;
            (void)SAL_Destroy_Mutex(&SAL_I_Timers_Mutex);
         }
      }
#endif /* XSAL_LIGHT */

      if (!status)
      {
         Tr_Fault("SAL_I_Init_Timer_Module failed");
      }

      return status;
   }

   void SAL_I_Deinit_Timer_Module(void)
   {
#if defined (XSAL_LIGHT)

      (void)SAL_Disconnect(&XSAL_I_RT_Connection);
      (void)SAL_Destroy_Channel(&XSAL_I_RT_Channel);

#endif /* XSAL_LIGHT */

      if (SAL_I_Timers != NULL)
      {
         free(SAL_I_Timers);
         SAL_I_Timers = NULL;
         (void) SAL_Destroy_Mutex(&SAL_I_Timers_Mutex);
      }
   }

   void SAL_I_Stop_Timer_Module(void)
   {
   }

   void SAL_I_Start_Timer_Module(void)
   {
      size_t tm_id;

      (void) SAL_Lock_Mutex(&SAL_I_Timers_Mutex);

      SAL_I_Timers_Free_List = &SAL_I_Timers[0];

      for (tm_id = 0; tm_id < SAL_I_Max_Number_Of_Timers - 1; tm_id++)
      {
         SAL_I_Timers[tm_id].event_id = -1;
         SAL_I_Timers[tm_id].next_thread_timer = &SAL_I_Timers[tm_id + 1];
      }
      SAL_I_Timers[SAL_I_Max_Number_Of_Timers - 1].event_id = -1;
      SAL_I_Timers[SAL_I_Max_Number_Of_Timers - 1].next_thread_timer = NULL;
      (void) SAL_Unlock_Mutex(&SAL_I_Timers_Mutex);
   }

#if defined (XSAL_LIGHT)

   int32_t SAL_I_RT_Light(void)
   {
      int32_t exit_status;

      (void)SAL_Signal_Ready();
      while(1)
      {
         bool_t status;
         SAL_Pulse_T pulse_data;
         uint8_t is_pulse;
         size_t message_size;
         SAL_Receive_T rec_id;

         status = SAL_Sync_Receive(&XSAL_I_RT_Channel, &pulse_data, sizeof(pulse_data), &message_size, &is_pulse, &rec_id);

         if (status)
         {
            if (is_pulse)
            {
               /** Pulse code specifies destination thread.
                *  Pulse value depends on pulse code. For pulse code = 0, pulse
                *  value defines Router Thread command. Code value > 0 defines
                *  timer event (code specifies thread id) and pulse
                *  value defines event id.
                */
               int8_t pulse_code = pulse_data.code;
               int32_t pulse_value = pulse_data.value.sival_int;
#ifdef XSAL_ENABLE_MSG_TRACE
               Tr_Info_Lo_2("SAL_Run: received pulse: %d, %d", (int)pulse_code, (int)pulse_value);
#endif /* XSAL_ENABLE_MSG_TRACE */
               /** Is it a pulse to the router thread?
                */
               if (pulse_code == SAL_ROUTER_THREAD_ID)
               {
                  if ((pulse_value & 0xff) == SAL_PULSE_RT_EXIT)
                  {
                     exit_status = pulse_value >> 8;

                     /** It is a pulse. Do not replay to the sender
                      */defined
                     break;
                  }
               }
               else if (pulse_code > 0)
               {
                  /** It is a pulse from timer.
                   *  puls_code value defines a thread id
                   *  puls_value points to expired Timer structure
                   */
                  SAL_I_Post_Timer_Event(
                     ((SAL_I_Timer_T*)pulse_value)->event_id, pulse_code,
                     ((SAL_I_Timer_T*)pulse_value)->use_param, ((SAL_I_Timer_T*)pulse_value)->param);

                  /** It is a pulse. Do not replay to the sender
                   */
                  continue;
               }
            }
         }
         else
         {
            Tr_Fault("ERROR SAL_Receive");
         }
         PBC_Failed("Should not get here");
         SAL_Sync_Reply(&rec_id, 0, NULL, 0);
      }

      return exit_status;
   }

   void SAL_I_Stop_RT_Light(int32_t status)
   {
      (void)SAL_Send_Pulse(
         &XSAL_I_RT_Connection,
         SAL_ROUTER_THREAD_ID,
         SAL_PULSE_RT_EXIT | (status << 8));
   }

#endif /* XSAL && XSAL_LIGHT */

   bool_t SAL_I_Create_Timer(SAL_I_Timer_T* timer)
   {
      bool_t status;
      struct sigevent event;

#if defined (XSAL)
#define TIMER_CONNECTION SAL_I_Connection_Id[SAL_I_App_Id]

#else /* XSAL_LIGHT */

#define TIMER_CONNECTION XSAL_I_RT_Connection

#endif /* XSAL && XSAL_LIGHT */

      SIGEV_PULSE_INIT(
         &event,
         TIMER_CONNECTION,
         SIGEV_PULSE_PRIO_INHERIT,
         timer->thread_id, /* pulse code with destination thread id */
         timer); /* pulse value points to Timer data      */

      if (timer_create(CLOCK_MONOTONIC, &event, &timer->os_timer_id) == 0)
      {
         status = true;
      }
      else
      {
         PBC_Failed_1("SAL_Create_Timer: Timer for event_id=%d not created", (int)timer->event_id);
         status = false;
      }

      return status;
   }

   void SAL_I_Start_Timer(SAL_I_Timer_T* timer, uint32_t interval_msec, bool_t is_periodic, bool_t use_param, uintptr_t param)
   {
      struct itimerspec os_timer;

      os_timer.it_value.tv_sec = interval_msec / 1000u;
      os_timer.it_value.tv_nsec = (interval_msec % 1000u) * 1000000u; /* ns resolution */

      if (is_periodic)
      {
         os_timer.it_interval.tv_sec = os_timer.it_value.tv_sec;
         os_timer.it_interval.tv_nsec = os_timer.it_value.tv_nsec;
      }
      else
      {
         os_timer.it_interval.tv_sec = 0;
         os_timer.it_interval.tv_nsec = 0;
      }

      timer->use_param = use_param;
      timer->param = param;

      timer_settime(timer->os_timer_id, 0, &os_timer, NULL);
   }

   void SAL_I_Stop_Timer(SAL_I_Timer_T* timer)
   {
      struct itimerspec os_timer;

      os_timer.it_value.tv_sec = 0;
      os_timer.it_value.tv_nsec = 0;
      os_timer.it_interval.tv_sec = 0;
      os_timer.it_interval.tv_nsec = 0;

      timer_settime(timer->os_timer_id, 0, &os_timer, NULL);
   }

   void SAL_I_Destroy_Timer(SAL_I_Timer_T* timer)
   {
      if (timer_delete(timer->os_timer_id) != 0)
      {
         Tr_Fault_1("SAL_I_Destroy_Timer: %s", strerror(errno));
      }
   }

#endif /* XSAL_TIMER_IMPL == XSAL_TIMER_USE_PULSES */

   /* -------------------------------------------------------------------------
    * Revision history:
    *
    * 01 Sep 2006, Jacek Roman (jacek.roman@delphi.com)
    *   Initial revision.
    *
    * - 12/15/14 - Dan Carman
    *    Changed SAL_Timers to use CLOCK_MONOTONIC to avoid discontinuities when system time is changed.
    *
    * - 08/26/15 Kirk Bailey
    *    Fixed Coverity issue: Missing Lock
    *
    * -----------------------------------------------------------------------*/
