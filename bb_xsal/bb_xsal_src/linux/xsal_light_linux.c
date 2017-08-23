/* -------------------------------------------------------------------------
 *
 * File: xsal_light_linux.c
 *
 * Implements "light" version of router thread for Linux
 *
 * -----------------------------------------------------------------------*/
#include "xsal.h"

#if defined (XSAL_LIGHT)
#include <string.h>
#include "xsal_i_assert.h"
#include "xsal_i_engine.h"
#include "xsal_i_timer.h"

/*
 * The following prototypes are excluded from xsal.h for XSAL_LIGHT, but are
 * still present for internal XSAL use.
 */
bool_t SAL_Create_Channel(SAL_Channel_T* channel);

bool_t SAL_Destroy_Channel(SAL_Channel_T* channel);

bool_t SAL_Create_Connection(const SAL_Channel_T* channel, SAL_Connection_T* connection);

bool_t SAL_Send_Pulse(SAL_Connection_T* connection, uint8_t code, uint32_t value);

bool_t SAL_Sync_Receive(SAL_Channel_T* channel, void* buffer, size_t buffer_size, size_t* message_size,
   uint8_t* is_pulse, SAL_Receive_T* receive_id);

EM_FILENUM(XSAL_IPC_MODULE_ID, 5); /* Identify file for trace/assert purposes */

static SAL_Channel_T XSAL_I_RT_Channel;
static SAL_Connection_T XSAL_I_RT_Connection;

int32_t SAL_I_RT_Light(void)
{
   int32_t exit_status;

   if (SAL_Create_Channel(&XSAL_I_RT_Channel))
   {
      exit_status = SAL_Create_Connection(&XSAL_I_RT_Channel, &XSAL_I_RT_Connection);
      if (!exit_status)
      {
         Tr_Fault("SAL_Create_Connection() failed");
      }
      if (!exit_status)
      {
         SAL_Destroy_Channel(&XSAL_I_RT_Channel);
      }
   }

   SAL_Signal_Ready();

   while (1)
   {
      bool_t status;
      uint8_t is_pulse;
      size_t message_size;
      SAL_Message_T msg_header;
      SAL_Receive_T receiver_id;
      SAL_Pulse_T pulse;

      status = SAL_Sync_Receive(&XSAL_I_RT_Channel, &msg_header, sizeof(SAL_Message_T), &message_size,
         &is_pulse, &receiver_id);

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
            int8_t pulse_code;
            int32_t pulse_value;
            memcpy(&pulse, &msg_header, sizeof(pulse));
            pulse_code = pulse.code;
            pulse_value = pulse.value.sival_int;

#ifdef XSAL_ENABLE_MSG_TRACE
            Tr_Info_Lo_2("SAL_Run: received pulse: %d, %d", (int)pulse_code, (int)pulse_value);
#endif /* XSAL_ENABLE_MSG_TRACE */
            /** Is it a pulse to the router thread?
             */
            if (pulse_code == SAL_ROUTER_THREAD_ID)
            {
               if (((uint32_t) pulse_value & 0xffu) == SAL_PULSE_RT_EXIT)
               {
                  exit_status = pulse_value / 256;
                  break;
               }
            }
         }
      }
      else
      {
         Tr_Fault("ERROR SAL_Receive");
      }
   }

   return exit_status;
}

void SAL_I_Stop_RT_Light(int32_t status)
{
   (void) SAL_Send_Pulse(&XSAL_I_RT_Connection, SAL_ROUTER_THREAD_ID, SAL_PULSE_RT_EXIT | (status << 8));
}

#endif /* XSAL_LIGHT */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 15-Aug-2012 Kirk Bailey Created initial file
 * -----------------------------------------------------------------------*/
