/*===========================================================================*/
/**
 * @file iic_qnx.c
 *
 * IIC wrapper for qnx
 *
 * %full_filespec: iic_qnx.c~2:csrc:kok_inf#1 %
 * @version %version: 2 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Sat May 16 18:17:42 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   This module provides an IIC interface.
 *
 * @section ABBR ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - @todo Update list of design document(s).
 *
 *   - Requirements Document(s):
 *     - @todo Update list of requirements document(s)
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *     - @todo Update list of other applicable standards
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @addtogroup iic_grp
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/
#include "reuse.h"
#include "pbc_trace.h"
#include "iic.h"
#include "iic_cfg.h"
#include "i2c_qnx.h"
#include <pthread.h>

/*===========================================================================*\
 * Local Preprocessor #define MACROS
 \*===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 1); /* define this file for assert handling */

/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/

/*===========================================================================*\
 * Local Type Declarations
 \*===========================================================================*/

/**
 * IIC channel attributes
 */
typedef struct iic_chan_Tag
{
   pthread_mutex_t access;
   int handle; /**< file handle returned from open */
   char_t * device_name;
} iic_chan_T;

/*===========================================================================*\
 * Exported Const Object Definitions
 \*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
 \*===========================================================================*/

#undef IIC_Define_Channel
#define IIC_Define_Channel(ID, name)   { PTHREAD_MUTEX_INITIALIZER, -1, name},

/**
 * Internal Channel specific data
 */
static iic_chan_T iic_chan[] =
   { IIC_Define_Channels };

#define NUMBER_IIC_CHANNELS Num_Elems(iic_chan)

/*===========================================================================*\
 * Local Function Prototypes
 \*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
 \*===========================================================================*/

/**
 * Converts API device to format for driver
 */
#define Device_Addr(device) (device >> 1)

/**
 * Returns if stop option is set
 */
#define Is_Stop(control)   (0 != (IIC_STOP & control))

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

static iic_chan_T *Channel_Info(IIC_Channel_T channel)
{
   PBC_Require_1((channel < NUMBER_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   return &iic_chan[channel];
}

/**
 * Locks and configures IIC channel for use by thread
 */
bool_t IIC_Lock(IIC_Channel_T channel, uint16_t speed_kHz, uint32_t timeout_msec)
{
   iic_chan_T * iic = Channel_Info(channel);
   bool_t ret = false;

   if (!pthread_mutex_lock(&iic->access))
   {
      if (iic->handle < 0)
      {
         if (I2C_Open(&(iic->handle), iic->device_name))
         {
            Tr_Fault_2("IIC_Lock: Failed to Open Channel %d - %s", channel, iic->device_name);
         }
      }

      if (iic->handle >= 0)
      {
         if (0 == I2C_Lock(iic->handle))
         {
            I2C_Set_Busspeed(iic->handle, speed_kHz * 1000);
            ret = true;
         }
      }
      else
      {
         Tr_Fault_1("IIC_Lock: Failed to Open Channel %d", channel);
      }

      pthread_mutex_unlock(&iic->access);
   }
   return ret;
}

/**
 * Releases IIC channel for use by other threads
 */
void IIC_Unlock(IIC_Channel_T channel)
{
   iic_chan_T * iic = Channel_Info(channel);

   if (!pthread_mutex_lock(&iic->access))
   {
      if (iic->handle >= 0)
      {
         I2C_UnLock(iic->handle);
      }
      else
      {
         Tr_Fault_1("IIC_Unlock: unopened channel %d", channel);
      }
      pthread_mutex_unlock(&iic->access);
   }
}

/**
 * IIC Master transmit
 */
bool_t IIC_Write(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, const uint8_t * wdata, size_t num_bytes,
   uint32_t timeout_msec)
{

   bool_t ret = false;
   iic_chan_T * iic = Channel_Info(channel);

   if (!pthread_mutex_lock(&iic->access))
   {

      if (iic->handle >= 0)
      {
         ret = (0 == I2C_Write(iic->handle, Device_Addr(device), I2C_ADDRFMT_7BIT, wdata, num_bytes, Is_Stop(control)));
      }
      else
      {
         Tr_Fault_1("IIC_Write: unopened channel %d", channel);
      }
      pthread_mutex_unlock(&iic->access);
   }
   return ret;
}

/**
 * IIC Master Recieve
 */
bool_t IIC_Read(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, uint8_t * rdata, size_t num_bytes,
   uint32_t timeout_msec)
{
   bool_t ret = false;
   iic_chan_T * iic = Channel_Info(channel);
   if (!pthread_mutex_lock(&iic->access))
   {
      if (iic->handle >= 0)
      {
         ret = (0 == I2C_Read(iic->handle, Device_Addr(device), I2C_ADDRFMT_7BIT, rdata, num_bytes, Is_Stop(control)));
      }
      else
      {
         Tr_Fault_1("IIC_Read: unopened channel %d", channel);
      }
      pthread_mutex_unlock(&iic->access);
   }
   return ret;
}

/**
 * IIC Master transmit
 */
bool_t IIC_Write_Read(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, const uint8_t * wdata, size_t num_wbytes,
   uint8_t * rdata, size_t num_rbytes)
{

   bool_t ret = false;
   iic_chan_T * iic = Channel_Info(channel);
   if (!pthread_mutex_lock(&iic->access))
   {

      if (iic->handle >= 0)
      {
         ret = (0 == I2C_Send_Receive(iic->handle, Device_Addr(device), I2C_ADDRFMT_7BIT, wdata, num_wbytes, rdata, num_rbytes));
      }
      else
      {
         Tr_Fault_1("IIC_Write_Read: unopened channel %d", channel);
      }
      pthread_mutex_unlock(&iic->access);
   }
   return ret;
}

/**
 * Initializes IIC module
 */
void IIC_Initialize(void)
{

}

/**
 * Disables all IIC channel.
 */
void IIC_Shutdown(void)
{
   IIC_Channel_T chan;

   for (chan = 0; chan < NUMBER_IIC_CHANNELS; chan++)
   {
      iic_chan_T * iic = Channel_Info(chan);
      if (!pthread_mutex_lock(&iic->access))
      {
         if (iic->handle >= 0)
         {
            I2C_Close(&(iic->handle));
         }
         pthread_mutex_unlock(&iic->access);
      }
   }
}

/*===========================================================================*/
/*!
 * @file iic_qnx.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 * - 27-Mar-2014 Dan Carman
 *   - Created initial file for Base Driver for qnx
 *
 *   16-May-15 Dan Carman
 *    - Task kok_inf#25281 - Add exclusive access to IIC functions
 *       Multiple threads where access Channel_info at the sametime and with
 *       error of throwing away the handle at unlock was leading to multiple errors
 */
/*===========================================================================*/
