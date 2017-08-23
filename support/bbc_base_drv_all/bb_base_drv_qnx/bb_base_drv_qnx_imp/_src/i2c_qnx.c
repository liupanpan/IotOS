/*===========================================================================*/
/**
 * @file i2c.c
 *
 * @brief Here we have i2c typical functions.
 *
 * %full_filespec: i2c_qnx.c~3:csrc:kok_inf#1 %
 * @version %version: 3 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Mon Oct 20 14:46:57 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * DESCRIPTION:
 *   Here we have i2c typical functions.
 *
 * ABBREVIATIONS:
 *   - @todo List any abbreviations, precede each with a dash ('-').
 *
 * TRACEABILITY INFO:
 *   - Design Document(s):
 *     
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

#include <fcntl.h>
#include <devctl.h>
#include <hw/i2c.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "i2c_qnx.h"
#include "pbc_trace.h"
#include "reuse.h"

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 2);
/* define this file for assert handling */

/*===========================================================================*/
/**
 * @fn    I2C_Open
 * @brief The I2C_Open function initializes the master interface.
 *
 * @param I2C_Handle The handle needed by all other I2C functions.
 *
 * The function returns a handle that's passed to all other functions, or NULL if an error occurred
 *
 */
/*===========================================================================*/
int32_t I2C_Open(int32_t *I2C_Handle, char const * device)
{
   static int32_t state = EOK;
   int32_t ret_val = EOK;

/* If handle already open, close it before re-opening it */
   if (*I2C_Handle > 0)
   {
      close(*I2C_Handle);
   }

   *I2C_Handle = open(device, O_RDWR);
   if (-1 == *I2C_Handle)
   {
      ret_val = errno;
      if (state != ret_val)
      {
         Tr_Warn_2("I2C_Open: Failure to open %s - %s", device, strerror(ret_val));
         state = ret_val;
      }
   }

   return ret_val;
}

/*===========================================================================*/
/**
 * @fn    I2C_Set_Busspeed
 * @brief Will be called just before the scheduler is called.
 *
 * @param I2C_Handle The handle returned by the I2C_Open function.
 * @param bus_Speed  The I2C bus speed (I2C_SPEED_STANDARD, I2C_SPEED_FAST, I2C_SPEED_HIGH)
 *
 * If an invalid bus speed is requested, this function should return a failure and leave the bus speed unchanged
 */
/*===========================================================================*/
int32_t I2C_Set_Busspeed(int32_t I2C_Handle, uint32_t bus_Speed)
{
   int32_t ret_val = EBADF;

   if (I2C_Handle != -1)
   {
      /* Set speed. */
      ret_val = devctl(I2C_Handle, DCMD_I2C_SET_BUS_SPEED, &bus_Speed, sizeof(bus_Speed), NULL);
   }

   if (ret_val)
   {
      Tr_Warn_2("I2C_Set_Busspeed: Failure to set speed %d - %s", bus_Speed, strerror(ret_val));
   }

   return ret_val;
}

int32_t I2C_Lock(int32_t I2C_Handle)
{
   int32_t ret_val = EBADF;

   if (I2C_Handle != -1)
   {
      ret_val = devctl(I2C_Handle, DCMD_I2C_LOCK, NULL, 0, NULL);
   }

   if (ret_val)
   {
      Tr_Warn_1("I2C_Lock: %s", strerror(ret_val));
   }

   return ret_val;
}

int32_t I2C_UnLock(int32_t I2C_Handle)
{
   int32_t ret_val = EBADF;

   if (I2C_Handle != -1)
   {
      ret_val = devctl(I2C_Handle, DCMD_I2C_UNLOCK, NULL, 0, NULL);
   }

   if (ret_val)
   {
      Tr_Warn_1("I2C_Unlock: %s", strerror(ret_val));
   }

   return ret_val;
}

int32_t I2C_Write(int32_t I2C_Handle, uint32_t Slave_Addr, uint32_t Format, const uint8_t *Buffer, int32_t Bytes, uint32_t Stop)
{
   int32_t ret_val = EBADF;
   iov_t siov[3];
   i2c_send_t hdr;

   hdr.slave.addr = Slave_Addr; /* target address */
   hdr.slave.fmt = Format; /* address format */
   hdr.len = Bytes; /* number of cmd bytes */
   hdr.stop = 1; /* send stop when complete? (0=no, 1=yes) */

   SETIOV(&siov[0], &hdr, sizeof(hdr));
   SETIOV(&siov[1], Buffer, Bytes);
   ret_val = devctlv(I2C_Handle, DCMD_I2C_SEND, 2, 0, siov, NULL, NULL);
   if (ret_val)
   {
      Tr_Warn_2("I2C_Write Device: %02X: %s", Slave_Addr, strerror(ret_val));
   }

   return ret_val;
}

int32_t I2C_Send_Receive(int32_t I2C_Handle, uint32_t Slave_Addr, uint32_t Format, uint8_t const *Tx_Buffer, int32_t Tx_Bytes,
   uint8_t *Rx_Buffer, int32_t Rx_Bytes)
{
   int32_t ret_val = EBADF;

   iov_t siov[2];
   iov_t riov[2];
   i2c_sendrecv_t hdr;

   hdr.slave.addr = Slave_Addr;     /* target address */
   hdr.slave.fmt = Format;        	/* address format */
   hdr.send_len = Tx_Bytes;         /* number of cmd bytes */
   hdr.recv_len = Rx_Bytes;         /* number of receive bytes */
   hdr.stop = 1; /* send stop when complete? (0=no, 1=yes) */

   SETIOV(&siov[0], &hdr, sizeof(hdr));
   SETIOV(&siov[1], Tx_Buffer, Tx_Bytes);

   SETIOV(&riov[0], &hdr, sizeof(hdr));
   SETIOV(&riov[1], Rx_Buffer, Rx_Bytes);

   ret_val = devctlv(I2C_Handle, DCMD_I2C_SENDRECV, 2, 2, siov, riov, NULL);
   if (ret_val)
   {
      Tr_Warn_2("I2C_Send_Receive Device: %02X %s", Slave_Addr, strerror(ret_val));
   }

   return ret_val;
}

int32_t I2C_Read(int32_t I2C_Handle, uint32_t Slave_Addr, uint32_t Format, uint8_t *Buffer, int32_t Bytes, uint32_t Stop)
{
   int32_t ret_val = EBADF;
   iov_t riov[2];
   i2c_recv_t hdr;

   hdr.slave.addr = Slave_Addr; /* target address */
   hdr.slave.fmt = Format; /* address format */
   hdr.len = Bytes; /* number of cmd bytes */
   hdr.stop = Stop; /* send stop when complete? (0=no, 1=yes) */

   SETIOV(&riov[0], &hdr, sizeof(hdr));
   SETIOV(&riov[1], Buffer, Bytes);
   ret_val = devctlv(I2C_Handle, DCMD_I2C_RECV, 0, 2, NULL, riov, NULL);
   if (ret_val)
   {
      Tr_Warn_2("I2C_Read Device: %02X %s", Slave_Addr, strerror(ret_val));
   }

   return ret_val;
}

int32_t I2C_Close(int32_t *I2C_Handle)
{
   int32_t ret_val;

   ret_val = close(*I2C_Handle);
   if (ret_val)
   {
      Tr_Warn_1("I2C_Close: %s", strerror(ret_val));
   }

   /* Mark handle as closed so we don't accidentally reuse a closed device handle. */
   *I2C_Handle = -1;

   return ret_val;
}

/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 27-Mar-2914  Dan Carman
 * + Imported from MIB
 *
 \*===========================================================================*/

