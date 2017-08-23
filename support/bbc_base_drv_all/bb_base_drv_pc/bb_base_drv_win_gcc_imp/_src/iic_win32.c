/*===========================================================================*/
/**
 * @file iic_win32.c
 *
 * @brief Simulates IIC interface for win32 
 *
 * %full_filespec: iic_win32.c~kok_basa#6:csrc:kok_aud#1 %
 * @version %version: kok_basa#6 %
 * @author  %derived_by: dzq92s %
 * @date    %date_modified: Mon Oct 17 13:16:48 2011 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *   This module provides IIC master interface. It does not support IIC slave 
 *    operation (which is not supported by the underlying Renesas' driver.
 *
 * ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * TRACEABILITY INFO:
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
 * DEVIATIONS FROM STANDARDS:
 *   - @todo List of deviations from standards in this file, or "None".
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

#include "iic.h"
#include "iic_cfg.h"
#include "pbc_trace.h"
#include "reuse.h"
#include <stdio.h>
#include "xsal.h"

#if IIC_SUPPORT_VIP
#   include "vip_iic.h"
#endif

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
/**
 * Default wait time to shutdown IIC channel
 */
#define IIC_DEFAULT_TIMEOUT      100

/**  Size of buffer to store read data 
 */
#define IIC_READ_BUFFER_SIZE     512

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 1);  /* define this file for assert handling */

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/** Parameter for IOCTL SIIC_SET_CHANNEL  */
typedef struct
{
   int bit_rate;                /* transfer rate */
   int slave_addr;              /* device address
                                   (This will be << 1 to make "address" byte) */
} T_SERIAL_IIC_CHAN_SETTING;

/**
 * IIC channel attributes 
 */
typedef struct iic_attributes_Tag
{
   SAL_Mutex_T lock;          /**< Limits channel access to one thread at a time */
   SAL_Thread_Id_T owner;              /**< thread currently locking IIC */
   int handle;                         /**< file handle returned from open */
   T_SERIAL_IIC_CHAN_SETTING config;   /**< bit rate and slave device address */
} iic_attributes_T;

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/

static iic_attributes_T iic_attributes[TOTAL_IIC_CHANNELS];

#undef IIC_Define_Channel
#define IIC_Define_Channel(ID, name)	name,

/** 
 * Variabled to track read data 
 */
uint8_t IIC_Read_Buffer[NUMBER_IIC_CHANNELS][IIC_READ_BUFFER_SIZE];
size_t IIC_Read_Index[NUMBER_IIC_CHANNELS];

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/
static bool_t set_device_address(IIC_Channel_T channel, uint8_t device);
static bool_t set_stop(IIC_Channel_T channel, IIC_Control_T control);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/*
 * Locks and configures IIC channel for use by thread
 */
bool_t IIC_Lock(IIC_Channel_T channel, uint16_t speed_kHz, uint32_t timeout_msec)
{
   bool_t success = false;

   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   if (SAL_Lock_Mutex_Timeout(&iic_attributes[channel].lock, timeout_msec))
   {
      iic_attributes[channel].owner = (int16_t) SAL_Get_Thread_Id();
      iic_attributes[channel].config.bit_rate = speed_kHz;
      iic_attributes[channel].config.slave_addr = 0;

      success = true;
   }
   else
   {
      Tr_Fault_1("Unable to lock IIC:%d", channel);
   }

   return success;
}

/*

 * Releases IIC channel for use by other threads
 */
void IIC_Unlock(IIC_Channel_T channel)
{
   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   SAL_Unlock_Mutex(&iic_attributes[channel].lock);
}


/*---------------------------------------------------------------------------*\
 * IIC bus master functions
\*---------------------------------------------------------------------------*/

#define IIC_WRITE_FMT "Tx IIC-%u %02X:"
/*
 * IIC Master transmit
 */
bool_t IIC_Write(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, const uint8_t * wdata, size_t num_bytes,
                 uint32_t timeout_msec)
{
   size_t i;
   bool_t status = true;

   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);
   PBC_Require_3(iic_attributes[channel].owner == SAL_Get_Thread_Id(),
                 "IIC %d owned by thread %d is being written by thread %d", channel, iic_attributes[channel].owner,
                 (int)SAL_Get_Thread_Id());

   if (channel < NUMBER_IIC_CHANNELS)
   {
      char_t *w_buf;
      size_t buf_len;
      ssize_t ofst;

      (void)set_device_address(channel, device);
      (void)set_stop(channel, control);

      buf_len = strlen(IIC_WRITE_FMT) + num_bytes*3 + 32;
      w_buf = SAL_Alloc(buf_len);

      ofst = snprintf(w_buf, buf_len, "Tx IIC-%u %02X:", (unsigned)channel, (unsigned)device);

      if (ofst > 0)
      {
         for (i = 0; i < num_bytes; i++)
         {
            ssize_t n;

            n = snprintf(w_buf+ofst, buf_len-ofst, " %02X", *(wdata + i));

            if (n < 0) break;

            ofst += n;
         }
      }

      Tr_Info_Lo_1("%s", w_buf);

      SAL_Free(w_buf);

      SAL_Sleep((((num_bytes + 1) * 9) / iic_attributes[channel].config.bit_rate) + 1);
   }
   else
   {
#if IIC_SUPPORT_VIP
      VIIC_Write(channel - IIC_CHANNEL_V0, device, (0 == (IIC_STOP & control)), wdata, num_bytes);
#else
      PBC_Failed("VIP IIC support not enabled"); 
#endif
   }
   return status;
}

static uint8_t get_read_byte(IIC_Channel_T channel)
{
   IIC_Read_Index[channel] = (IIC_Read_Index[channel] + 1) % IIC_READ_BUFFER_SIZE;
   return IIC_Read_Buffer[channel][IIC_Read_Index[channel]];
}

/*
 * IIC Master Recieve
 */
bool_t IIC_Read(IIC_Channel_T channel, uint8_t device, IIC_Control_T control, uint8_t * rdata, size_t num_bytes,
              uint32_t timeout_msec)
{
   size_t i;
   bool_t status = true;

   (void)timeout_msec;

   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);
   PBC_Require_3(iic_attributes[channel].owner == SAL_Get_Thread_Id(),
                 "IIC %d owned by thread %d is being written by thread %d", channel, iic_attributes[channel].owner,
                 (int)SAL_Get_Thread_Id());

   if (channel < NUMBER_IIC_CHANNELS)
   {
      (void)set_device_address(channel, device);
      (void)set_stop(channel, control);

      for (i = 0; i < num_bytes; i++)
      {
         *rdata++ = get_read_byte(channel);
      }

      SAL_Sleep((((num_bytes + 1) * 9) / iic_attributes[channel].config.bit_rate) + 1);
   }
   else
   {
#if IIC_SUPPORT_VIP
      status = VIIC_Read(channel - IIC_CHANNEL_V0, device, rdata, num_bytes);
#else
      PBC_Failed("VIP IIC support not enabled"); 
#endif
   }

   return status;
}

/*---------------------------------------------------------------------------*\
 * IIC module functions
\*---------------------------------------------------------------------------*/

/*
 * Initializes IIC module
 */
void IIC_Initialize(void)
{
   /** Initialize lock as 1 available with a max of 1 */
   SAL_Mutex_Attr_T mutex_attr;
   IIC_Channel_T chan;

   SAL_Init_Mutex_Attr(&mutex_attr);

   memset(iic_attributes, 0xFF, sizeof(iic_attributes));        /* set all parameters to -1 */

   for (chan = 0; chan < NUMBER_IIC_CHANNELS; chan++)
   {
      SAL_Create_Mutex(&iic_attributes[chan].lock, &mutex_attr);
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

   for (chan = 0; chan < NUMBER_IIC_CHANNELS; chan++)
   {
      if (SAL_Lock_Mutex_Timeout(&iic_attributes[chan].lock, IIC_DEFAULT_TIMEOUT))
      {
         SAL_Unlock_Mutex(&iic_attributes[chan].lock);
         SAL_Destroy_Mutex(&iic_attributes[chan].lock);
      }
      else
      {
         PBC_Failed_1("Failed to destroy mutex IIC %d", chan);
      }
   }
}

/**
 * Set the target device address
 *
 * @param [in] channel - Identifies IIC channel [0..NUMBER_IIC_CHANNELS-1]
 * @param [in] device - Device address (left shifted)
 *
 * @return true if device address is set correctly
 *
 */
static bool_t set_device_address(IIC_Channel_T channel, uint8_t device)
{
   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   iic_attributes[channel].config.slave_addr = (device >> 1);

   return true;
}

/**
 * Disable stop condition if requested 
 *
 * @param [in] channel - Identifies IIC channel [0..NUMBER_IIC_CHANNELS-1]
 * @param [in] control - message control
 *
 * @return true if stop condition set correctly
 */
static bool_t set_stop(IIC_Channel_T channel, IIC_Control_T control)
{
   PBC_Require_1((channel < TOTAL_IIC_CHANNELS), "Invalid IIC Channel: %d", channel);

   if (0 == (IIC_STOP & control))
   {

   }
   return true;
}


/*===========================================================================*\
 * File Revision History (top to bottom: first revision to last revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 09-may-2007 kirk bailey
 * + Got rid of printf format warnings.
 *
 * 18-Apr-2007 Dan Carman
 * + Updated for new DbC and Trace macros
 * + Added Trace of all individual read / write errors
 *
 * 16-Feb-2007 Dan Carman
 * + Created initial file.
 *
\*===========================================================================*/
