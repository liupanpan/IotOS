/*===========================================================================*/
/**
 * @file iic_linux.c
 *
 * IIC wrapper for linux
 *
 * %full_filespec: iic_linux.c~6:csrc:kok_basa#4 %
 * @version %version: 6 %
 * @author  %derived_by: czhswm %
 * @date    %date_modified: Mon Jul 29 14:17:51 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010, 2011 Delphi Technologies, Inc., All Rights Reserved.
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
#include "byte_data_buffer.h"
#include <errno.h>
#include <fcntl.h>
#include "iic.h"
#include "iic_cfg.h"
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "pbc_trace.h"
#include "reuse.h"
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "xsal.h"

#if IIC_SUPPORT_VIP
#   include "vip_iic.h"
#endif

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 1);      /* define this file for assert handling */

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
/**
 * Default wait time to shutdown IIC channel
 */
#define IIC_DEFAULT_TIMEOUT      100

/**  Initial Size of buffer to multi-segment write data
 */
#define IIC_INIT_BUFFER_SIZE     64

/**
 * Max number of supported segments (e.g., starts before stop)
 */
#define IIC_MAX_SEGMENTS   4

#if (IIC_MAX_SEGMENTS > I2C_RDRW_IOCTL_MAX_MSGS)
#   error "Invalid MAX IIC Segments"
#endif

#define I2C_FUNC_REQUIRED (I2C_FUNC_I2C | I2C_FUNC_SMBUS_I2C_BLOCK)

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/**
 * IIC channel locks
 */
typedef struct iic_lock_Tag
{
   SAL_Mutex_T lock;               /**< Limits channel access to one thread at a time */
   SAL_Thread_Id_T owner;          /**< thread currently locking IIC */
} iic_lock_T;

/**
 * IIC channel attributes
 */
typedef struct iic_chan_Tag
{
   int handle;                     /**< file handle returned from open */
   BDB_T msg_data;                 /**< holds pending multi-segment data */
   uint8_t num_segs;               /**< number of segments in current set */
   struct i2c_msg msgs[IIC_MAX_SEGMENTS]; /**< segment data */
} iic_chan_T;

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

#undef IIC_Define_Channel
#define IIC_Define_Channel(ID, name)   name,

/**
 * define iic channel names
 */
const char_t *const iic_channel_name[NUMBER_IIC_CHANNELS] =
{
   IIC_Define_Channels
};

/**
 * Controls ownership of a IIC channel
 */
static iic_lock_T iic_locks[TOTAL_IIC_CHANNELS];

/**
 * Internal Channel specific data
 */
static iic_chan_T iic_chan[NUMBER_IIC_CHANNELS];

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

static void Reset_Channel(iic_chan_T * chan);
static void Close_IIC(IIC_Channel_T channel);
static bool_t Check_IIC_Open(IIC_Channel_T channel);
static void Set_Buffers(iic_chan_T * iic);
static bool_t IIC_Transfer(IIC_Channel_T channel);

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

#if IIC_SUPPORT_VIP

/**
 * Returns if IIC Channel ID is a VIP IIC Channel
 *
 * @param channel - IIC channel ID
 *
 * @returns true if channel is on VIP
 */
#   define Is_VIP_Channel(channel) ((channel >= IIC_CHANNEL_V0) && (channel < TOTAL_IIC_CHANNELS))

#endif

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

static iic_chan_T *Channel_Info(IIC_Channel_T channel)
{
   PBC_Require_1((channel < NUMBER_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   return &iic_chan[channel];
}

static void Close_IIC(IIC_Channel_T channel)
{
   iic_chan_T * iic = Channel_Info(channel);

   if (iic->handle >= 0)
   {
      int error = close(iic->handle);

      if (0 != error)
      {
         Tr_Fault_3("Error %d Closing IIC %s - %s\n", channel, iic_channel_name[channel], strerror(errno));
      }

      iic->handle = -1;
   }
}

/**
 * Verify that the  driver is open
 */
static bool_t Check_IIC_Open(IIC_Channel_T channel)
{
   bool_t success = true;

   if (channel < NUMBER_IIC_CHANNELS)  /* only need for local channels */
   {
      iic_chan_T * iic = Channel_Info(channel);

      if (iic->handle < 0)    /* is channel already open? */
      {
         success = false;
         iic->handle = open(iic_channel_name[channel], O_RDWR);

         if (iic->handle >= 0)        /* driver open successfully */
         {
            int funcs;
            int results;
            results = ioctl(iic->handle, I2C_FUNCS, &funcs);

            if (results >= 0)
            {
               success = (I2C_FUNC_REQUIRED == (I2C_FUNC_REQUIRED & funcs));
               if (!success)
               {
                  Tr_Fault_1("I2C_FUNC_REQUIRED not met! Support %X", funcs);
               }
            }
            else
            {
               Tr_Fault_1("Unable to ioctl I2C_FUNCS! Error %d", results);
            }
         }
         else
         {
            Tr_Fault_1("Unable to open IIC %s\n", iic_channel_name[channel]);
         }
      }
   }
   return success;
}

/*
 * Locks and configures IIC channel for use by thread
 */
bool_t IIC_Lock(IIC_Channel_T channel, uint16_t speed_kHz, uint32_t timeout_msec)
{
   bool_t success = true;

   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   /** Linux Driver API does not support multiple IIC clock speeds */
   (void)speed_kHz;

   success = SAL_Lock_Mutex_Timeout(&iic_locks[channel].lock, timeout_msec);

   if (success)
   {
      iic_locks[channel].owner = SAL_Get_Thread_Id();  /* track owner for debug */
      success = Check_IIC_Open(channel);
      if (!success)
      {
         Tr_Fault_1("Failed IIC %d open", channel); 
         (void)SAL_Unlock_Mutex(&iic_locks[channel].lock);
      }
      else
      {
         Tr_Info_Lo_1("IIC %d Locked", channel);
      }
   }
   else
   {
      Tr_Warn_2("Failed to Lock IIC %d. Owned by %d", channel, iic_locks[channel].owner);
   }

   return success;
}

/*
 * Releases IIC channel for use by other threads
 */
void IIC_Unlock(IIC_Channel_T channel)
{
   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   if (iic_locks[channel].owner != SAL_Get_Thread_Id())
   {
      Tr_Warn_2("IIC %d is not being unlocked by owner %d", channel, iic_locks[channel].owner);
   }
   
   (void)SAL_Unlock_Mutex(&iic_locks[channel].lock);
   
   Tr_Info_Lo_1("IIC %d Unlocked", channel);
}

/*---------------------------------------------------------------------------*\
 * IIC bus master functions
\*---------------------------------------------------------------------------*/

/**
 * sets multi-segment data pointer to bdb buffer
 * num_segs does not include the last segment
 */
static void Set_Buffers(iic_chan_T * iic)
{
   int i;

   BDB_Rewind(&(iic->msg_data));

   for (i = 0; i < iic->num_segs; i++)
   {
      if (0 == iic->msgs[i].flags)     /* write segment? */
      {
         iic->msgs[i].buf = (uint8_t *) BDB_Read_P_Bytes(&(iic->msg_data), iic->msgs[i].len);
      }
   }
}

/**
 * Kick off actual IIC transfer
 */
static bool_t IIC_Transfer(IIC_Channel_T channel)
{
   struct i2c_rdwr_ioctl_data rdwr;
   int error;
   iic_chan_T * iic = Channel_Info(channel);
   static uint32_t error_counter = 0;

   Set_Buffers(iic);

   rdwr.msgs = iic->msgs;
   rdwr.nmsgs = iic->num_segs + 1;

   error = ioctl(iic->handle, I2C_RDWR, &rdwr);

   if (error < 0)
   {
      error_counter++;

      /* reduce number of faults thrown by only reporting at 10^n milestones then every 1000  reduce trace level to see all */
      
      if ((error_counter == 1) || (error_counter == 10) || (error_counter == 100) || (error_counter%1000 == 0))
      {
         Tr_Fault_4("IIC %d Addr 0x%02X RDWR Error %d : Overall IIC errors %d ", channel, iic->msgs[iic->num_segs].addr, error, error_counter);
      }
      else
      {
         Tr_Info_Mid_4("IIC %d Addr 0x%02X RDWR Error %d : Overall IIC errors %d ", channel, iic->msgs[iic->num_segs].addr, error, error_counter);
      }
   }
   Reset_Channel(iic);

   return (error >= 0);
}

/*
 * IIC Master transmit
 */
bool_t IIC_Write(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, const uint8_t * wdata,
   size_t num_bytes, uint32_t timeout_msec)
{
   bool_t success = true;

   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

#if IIC_SUPPORT_VIP
   if (Is_VIP_Channel(channel))
   {
      VIIC_Write(channel - IIC_CHANNEL_V0, device, !Is_Stop(control), wdata, num_bytes);
   }
   else
#endif
   {
      iic_chan_T * iic = Channel_Info(channel);

      PBC_Require_1(IIC_MAX_SEGMENTS > iic->num_segs, "Too many IIC segments %d", iic->num_segs);

      iic->msgs[iic->num_segs].len = num_bytes;
      iic->msgs[iic->num_segs].flags = 0;
      iic->msgs[iic->num_segs].addr = Device_Addr(device);
      iic->msgs[iic->num_segs].buf = (uint8_t *) wdata;

      if (Is_Stop(control))
      {
         success = IIC_Transfer(channel);
      }
      else
      {
         /* Store a copy of the message data. */
         BDB_Write_Bytes(&iic->msg_data, (void *)wdata, num_bytes);

         /* Post-increment message number on store to keep it 0..max-1. */
         iic->num_segs += 1;
      }
   }

#ifdef PRINT_IIC
   {
      int i;

      printf("IIC w 0x%x   ", device);
      for (i = 0; i < num_bytes; i++)
      {
         printf("0x%x ", *(wdata + i));
      }
      printf("\n");
   }
#endif

   return success;
}

/*
 * IIC Master Recieve
 */
bool_t IIC_Read(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, uint8_t * rdata, size_t num_bytes,
   uint32_t timeout_msec)
{
   bool_t success = true;

   (void)timeout_msec;

   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

#if IIC_SUPPORT_VIP
   if (Is_VIP_Channel(channel))
   {
      VIIC_Read(channel - IIC_CHANNEL_V0, device, rdata, num_bytes);
   }
   else
#endif
   {
      iic_chan_T * iic = Channel_Info(channel);
      PBC_Require_1(IIC_MAX_SEGMENTS > iic->num_segs, "Too many IIC segments %d", iic->num_segs);

      iic->msgs[iic->num_segs].flags = I2C_M_RD;
      iic->msgs[iic->num_segs].addr = Device_Addr(device);
      iic->msgs[iic->num_segs].buf = (uint8_t *) rdata;
      iic->msgs[iic->num_segs].len = num_bytes;

      if (Is_Stop(control))
      {
         success = IIC_Transfer(channel);
      }
      else
      {
         /* increment number of pending segments */
         iic->num_segs += 1;
      }
   }

#ifdef PRINT_IIC
   {
      int i;

      printf("IIC r 0x%x   ", device);
      for (i = 0; i < num_bytes; i++)
      {
         printf("0x%x ", *(rdata + i));
      }
      printf("\n");
   }

#endif

   return success;
}

/*---------------------------------------------------------------------------*\
 * IIC module functions
\*---------------------------------------------------------------------------*/

/*
 * Initializes IIC module
 */
void IIC_Initialize(void)
{
   /** Initialize semaphore as 1 available with a max of 1 */
   SAL_Mutex_Attr_T mutex_attr;
   IIC_Channel_T chan;
   iic_chan_T * iic;

   SAL_Init_Mutex_Attr(&mutex_attr);
   memset(iic_chan, 0xFF, sizeof(iic_chan));  /* set all parameters to -1 */

   for (chan = 0; chan < TOTAL_IIC_CHANNELS; chan++)
   {
      SAL_Create_Mutex(&iic_locks[chan].lock, &mutex_attr);
      if (chan < NUMBER_IIC_CHANNELS)
      {
         iic = Channel_Info(chan);
         BDB_Init(&(iic->msg_data), IIC_INIT_BUFFER_SIZE);
         Reset_Channel(iic);
      }
   }
}

/*
 * Disables all IIC channel.
 *
 *  Close all open IIC channels
 */
void IIC_Shutdown(void)
{

   IIC_Channel_T chan;

   for (chan = 0; chan < TOTAL_IIC_CHANNELS; chan++)
   {
      if (SAL_Lock_Mutex_Timeout(&iic_locks[chan].lock, IIC_DEFAULT_TIMEOUT))
      {
         if (chan < NUMBER_IIC_CHANNELS)
         {
            Close_IIC(chan);
         }

         SAL_Unlock_Mutex(&iic_locks[chan].lock);
         SAL_Destroy_Mutex(&iic_locks[chan].lock);
      }
      else
      {
         Tr_Fault_1("Failed to Shutdown IIC %d", chan);
      }
   }
}

/**
 *  Reset message segments to none pending
 */
static void Reset_Channel(iic_chan_T * iic)
{
   memset(iic->msgs, 0, sizeof(iic->msgs));
   iic->num_segs = 0;
   BDB_Erase(&iic->msg_data);
}

/*===========================================================================*/
/*!
 * @file iic_linux.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 * - 29-Jul-2013 Larry Piekarski Rev 6
 *   - Task 162164: Update to tip
 *
 * - 28-Jan-2012 Kirk Bailey
 *   - Fixed trace format warnings.
 *
 * - 29-apr-2011 Doug Srail
 *   - kok_basa#30289 - Reduce the frequency of faults due to iic errors
 *
 * - 27-aug-2010 Kris Boultbee
 *   - Added workaround for a gcc defect within the function set_device_address
 *
 * - 25-aug-2010 Kris Boultbee
 *   - Created initial file for Base Driver for Linux Host
 */
/*===========================================================================*/
