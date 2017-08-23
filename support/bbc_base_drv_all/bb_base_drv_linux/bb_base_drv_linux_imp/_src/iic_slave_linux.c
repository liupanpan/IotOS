/*===========================================================================*/
/**
 * @file iic_slave_linux.c
 *
 * IIC wrapper for linux
 *
 * %full_filespec: iic_slave_linux.c~kok_inf#16:csrc:kok_basa#1 %
 * @version %version: kok_inf#16 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue May  5 10:17:28 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
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
#include "xsal.h"
#include "iic.h"
#include "iic_cfg.h"
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "state_of_health.h"
#include "xsal_util.h"
#include "iic_slave_cbk.h"
#include "iic_slave.h"
#include "iic_slave_cfg.h"



/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 2);      /* define this file for assert handling */

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
/**
 * Driver name for IIC slave
 */
#define IIC_SLV_DRIVER_NAME          "mxc_i2c.0"

/**
 * Driver path for IIC slave
 */
#define IIC_SLV_DRIVER_PATH          "/sys/bus/platform/devices/mxc_i2c.0/driver"

/**
 * IIC master driver unbind path
 */
#define IIC_SLV_MASTER_UNBIND_PATH   "/sys/bus/platform/drivers/mxc_i2c/unbind"

/**
 * IIC slave driver bind path
 */
#define IIC_SLV_SLAVE_BIND_PATH      "/sys/bus/platform/drivers/mxc_i2c_slave/bind"

/**
 * IIC slave driver device node name
 */
#define IIC_SLV_DEVICE_NODE_NAME     "/dev/slave-i2c-0"


/**
 * IIC slave driver load check retries
 */
#define IIC_SLV_SLAVE_CHECK_RETRIES  (3)

/**
 * IIC slave driver load recheck interval
 */
#define IIC_SLV_SLAVE_CHECK_WAIT_MS  (300)

/**
 * Default wait time to access shared IIC slave variables
 */
#define IIC_SLAVE_TIMEOUT_MS        100

/**
 * wait time for mutex availability for channel pending and active buffer flags
 */
#define IIC_SLAVE_MUTEX_WAIT_TIMEOUT_MS        1000

/**
 * Maximum time for transmit message to be loaded by IIC slave thread
 */
#define IIC_SLV_WAIT_TX_LOADED_MS   15

/**
 * wait time for iic slave threads to report thread destroyed
 */
#define IIC_SLAVE_SEND_WAIT_DESTROYED_TIMEOUT_MS        10000   
#define IIC_SLAVE_RCV_WAIT_DESTROYED_TIMEOUT_MS          2000    

/**
 * Error codes
 */
#define E_OK                        (0)
#define E_ERROR                     (-1)

#define IIC_SLAVE_SEND_THREAD_LOOP_TIME_MS        10
#define IIC_SLAVE_RECEIVE_THREAD_LOOP_TIME_MS     10

#ifndef IIC_SLAVE_SEND_SOH_TIMEOUT_MS
#define IIC_SLAVE_SEND_SOH_TIMEOUT_MS  SOH_DEFAULT_TIMEOUT_MS
#endif

#ifndef IIC_SLAVE_RECEIVE_SOH_TIMEOUT_MS
#define IIC_SLAVE_RECEIVE_SOH_TIMEOUT_MS  SOH_DEFAULT_TIMEOUT_MS
#endif

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

#undef IIC_Slave_Define_Channel
#define IIC_Slave_Define_Channel(ID, name)   name,

/**
 * define slave iic channel names
 */
const char_t *const iic_slave_channel_name[NUMBER_IIC_CHANNELS] =
{
   IIC_Slave_Define_Channels
};


/**
 * IIC channel locks
 */
typedef struct IIC_Slave_Lock_Tag
{
   SAL_Mutex_T lock;               /**< Limits channel access to one thread at a time */
   SAL_Thread_Id_T owner;          /**< thread currently locking IIC */
} IIC_Slave_Lock_T;

/**
 * Interm Slave transmit buffers
 */
typedef struct IIC_SLV_Tx_Buf_Tag
{
   uint32_t num_bytes;
   bool_t pending;
   uint8_t buffer[IIC_SLV_MAX_TX_SIZE];
} IIC_SLV_Tx_Buf_T;

/**
 * IIC Slave Channel Information
 */
typedef struct IIC_Slave_Channel_Tag
{
   int read_handle;                    /**< file handle returned from open */
   int write_handle;                   /**< file handle returned from open */

   SAL_Thread_Id_T slv_rec_thread;     /**< slave receive  thread */
   SAL_Thread_Id_T slv_snd_thread;     /**< slave send thread */
   bool_t enabled;                     /**< flag used to control when slave threads die */
   bool_t tx_active_buf;               /**< false is tx buf 0, true is tx buf 1 */
   SAL_Semaphore_T tx_loaded;          /**< use to signal transmitting thread when tx data actually loaded */
   IIC_SLV_Tx_Buf_T tx_buf[2];         /**< transmit buffers */
   uint8_t rx_buf[IIC_SLV_MAX_RX_SIZE];/**< buffer to receive IIC data */

} IIC_Slave_Channel_T;

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/
/**
 * pointers to each's channel slave object.
 *    if channel is not enable, pointer will be NULL
 */
static IIC_Slave_Channel_T *Slave_Attr[NUMBER_IIC_CHANNELS];

/**
 * Mutex to protect mutli-thread access to IIC Slave Data
 */
static SAL_Mutex_T Slave_Lock;

/**
 * Controls initialization of common data
 */
static SAL_Util_Once_T Slave_Initalized = SAL_UTIL_ONCE_INIT;


/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/
static void IIC_Slave_Send_Thread(void *param);
static void IIC_Slave_Receive_Thread(void *param);
static IIC_Slave_Channel_T *Create_Slave_Channel(IIC_Channel_T chan_num);
static bool_t IIC_SLV_Load_Driver(void);
static void Destroy_Slave_Channel(IIC_Channel_T chan_num);
static void IIC_Slave_Init(void);
static void Start_Critical(void);
static void End_Critical(void);
static bool_t Open_IIC_Slave_Handles(IIC_Channel_T chan_num, uint8_t slave_addr);
/*static bool_t Lock_Slave_IIC_Resource(IIC_Channel_T chan_num);
static void Free_Slave_IIC_Resource(IIC_Channel_T chan_num);*/
/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/*
 * Enable IIC Slave operation on a given channel
 */
int32_t IIC_SLV_Enable(IIC_Channel_T chan_num, uint8_t slave_addr)
{
   SAL_Thread_Attr_T thread_attr;
   int32_t status = E_ERROR;
   Tr_Info_Hi("Enable iic");
   PBC_Require_1(chan_num == IIC_CHANNEL_0, "Invalid IIC Slave Channel %d", chan_num);

   SAL_Util_Once(&Slave_Initalized, IIC_Slave_Init);

   if (NULL != Slave_Attr[chan_num])
   {
      Tr_Warn_1("Slave Channel %d was already enabled", chan_num);
      IIC_SLV_Disable(chan_num);
   }

   Slave_Attr[chan_num] = Create_Slave_Channel(chan_num);

   PBC_Ensure(NULL != Slave_Attr[chan_num], "Failed to allocate Slave Channel Information");
   if(IIC_SLV_Load_Driver())
   {
      if(Open_IIC_Slave_Handles(chan_num,slave_addr))
      {
   	   /*create send thread*/
         IIC_SLV_Get_Send_Thread_Attr(&thread_attr);
         if (!SAL_Is_Thread_Alive(thread_attr.id))
         {
            Slave_Attr[chan_num]->slv_snd_thread = SAL_Create_Thread(IIC_Slave_Send_Thread, (void *)chan_num, &thread_attr);
         }
         if (SAL_Wait_Ready_Timeout(&(Slave_Attr[chan_num]->slv_snd_thread), 1, IIC_SLAVE_TIMEOUT_MS))
         {
             /*create receive thread*/
             IIC_SLV_Get_Receive_Thread_Attr(&thread_attr);
             if (!SAL_Is_Thread_Alive(thread_attr.id))
             {
                Slave_Attr[chan_num]->slv_rec_thread = SAL_Create_Thread(IIC_Slave_Receive_Thread, (void *)chan_num, &thread_attr);
             }
             if (SAL_Wait_Ready_Timeout(&(Slave_Attr[chan_num]->slv_rec_thread), 1, IIC_SLAVE_TIMEOUT_MS))
             {
                status = E_OK;
             }
             else
             {
                /*Unable to create receive thread*/
             }
   	   }
   	   else
   	   {
   	      /*Unable to create send thread*/
   	   }

      }
      else
      {
         /*Unable to setup slave port handles*/
      }
   }
   else
   {
      Tr_Fault("Unable to load slave driver");
   }
   return status;
}

/*
 * Close IIC slave operation for channel
 */
void IIC_SLV_Disable(IIC_Channel_T chan_num)
{
	IIC_Slave_Channel_T *chan;

   PBC_Require_1(chan_num == IIC_CHANNEL_0, "Invalid IIC Slave Channel %d", chan_num);

   chan = Slave_Attr[chan_num];

   if (chan != NULL)
   {
      Start_Critical();

      chan->enabled = false;

      End_Critical();

      if(SAL_Wait_Destroyed_Timeout(&(chan->slv_snd_thread), 1, IIC_SLAVE_SEND_WAIT_DESTROYED_TIMEOUT_MS))
      {
         if(!SAL_Wait_Destroyed_Timeout(&(chan->slv_rec_thread), 1, IIC_SLAVE_RCV_WAIT_DESTROYED_TIMEOUT_MS))
         {
            Tr_Warn("IIC Slave Receive Thread did not terminate");
         }
      }
      else
      {
         Tr_Warn("IIC Slave Send Thread did not terminate");
      }   

      close(chan->write_handle);
      close(chan->read_handle);

      Destroy_Slave_Channel(chan_num);

   }
   else
   {
      Tr_Warn_1("Attempting to disable non-enabled IIC Slave Channel %d", chan_num);
   }
}



/*
 * Set up new transmit data to be read by master
 */
int32_t IIC_SLV_Transmit(IIC_Channel_T chan_num, const uint8_t * transmit_data, size_t num_bytes)
{

   IIC_Slave_Channel_T *chan;
   IIC_SLV_Tx_Buf_T *tx_buf;

   int32_t status = E_ERROR;

   PBC_Require_1(chan_num == IIC_CHANNEL_0, "Invalid IIC Slave Channel %d", chan_num);
   PBC_Require_1(num_bytes <= IIC_SLV_MAX_TX_SIZE, "IIC Slave Tx Size %zu too large", num_bytes);

   Start_Critical();

   chan = Slave_Attr[chan_num];
   PBC_Require_1(chan != NULL, "IIC Slave Channel %d is not enabled", chan_num);

   tx_buf = &(chan->tx_buf[!chan->tx_active_buf]);
   if (!tx_buf->pending)
   {
      memcpy(tx_buf->buffer, transmit_data, num_bytes);
      tx_buf->num_bytes = num_bytes;
      tx_buf->pending = true;

      status = E_OK;
   }
   else
   {
      /*Device was busy, return with error code*/
   }
   End_Critical();

   if (E_OK == status)
   {
      /* consider canceling I/O here */
      /* IIC_Slave_Cancel(chan_num); */

      /* wait for IIC monitor thread to pick up new transmit message */
      if (!SAL_Wait_Semaphore_Timeout(&chan->tx_loaded, IIC_SLV_WAIT_TX_LOADED_MS))
      {
         status = E_ERROR;
      }
   }

   return status;
}


/**
 *
 * This section is for all modules internal functions
 *
 */
static void IIC_Slave_Send_Thread(void *param)
{
   IIC_Slave_Channel_T* chan=NULL;
   IIC_Channel_T chan_num = (IIC_Channel_T) param;
   bool_t new_tx;
   uint32_t tx_length;
   uint32_t retval;
   PBC_Require_1(chan_num == IIC_CHANNEL_0, "Invalid IIC Slave Channel %d", chan_num);

   chan = Slave_Attr[chan_num];
   PBC_Require_1(chan != NULL, "IIC Slave Channel %d is not enabled", chan_num);

   SAL_Signal_Ready();
   Tr_Info_Lo("IIC Slave send thread started");

   SOH_Register(IIC_SLAVE_SEND_SOH_TIMEOUT_MS);

   while(chan->enabled)
   {
      /* if current tx has been read and there is a pending new one */
      Start_Critical();
      new_tx = !chan->tx_buf[chan->tx_active_buf].pending && chan->tx_buf[!chan->tx_active_buf].pending;
      End_Critical();

      if (new_tx)
      {
         chan->tx_active_buf = !chan->tx_active_buf; /* switch to new tx message */
         SAL_Signal_Semaphore(&chan->tx_loaded);     /* signal requestor that we are using new tx data */
         tx_length = chan->tx_buf[chan->tx_active_buf].num_bytes;
                                                                            /*tx_length*/
        retval = write(chan->write_handle,chan->tx_buf[chan->tx_active_buf].buffer,tx_length);

         if(0<retval)
         {
            chan->tx_buf[chan->tx_active_buf].pending = false;
            IIC_SLV_Transmit_Done();
         }
         else
         {
            /*Data was not written correctly*/
            /*clear pending flag to allow tx to resume with next message*/
            chan->tx_buf[chan->tx_active_buf].pending = false;
            Tr_Warn("IIC_Slave_Send_Thread got error on write");
         }
      }
      SAL_Sleep(IIC_SLAVE_SEND_THREAD_LOOP_TIME_MS);
      SOH_Alive(IIC_SLAVE_SEND_SOH_TIMEOUT_MS);
   }
   SOH_Dormant(); 
}


static void IIC_Slave_Receive_Thread(void *param)
{
   IIC_Slave_Channel_T* chan=NULL;
   IIC_Channel_T chan_num = (IIC_Channel_T) param;
   int32_t rx_length=0;

   PBC_Require_1(chan_num == IIC_CHANNEL_0, "Invalid IIC Slave Channel %d", chan_num);

   chan = Slave_Attr[chan_num];
   PBC_Require_1(chan != NULL, "IIC Slave Channel %d is not enabled", chan_num);
   SAL_Signal_Ready();
   Tr_Info_Lo_1("IIC Slave received thread started for chan:%d",chan_num);

   SOH_Register(IIC_SLAVE_RECEIVE_SOH_TIMEOUT_MS);

   	while(chan->enabled)
   	{
       rx_length = read(chan->read_handle,chan->rx_buf,IIC_SLV_MAX_RX_SIZE );

       if((0<rx_length) && (IIC_SLV_MAX_RX_SIZE >= rx_length))
       {

    	   IIC_SLV_Receive(chan_num, chan->rx_buf, rx_length);

       }
       else
       {
          /*Incorrect data lenght*/
       }
       SAL_Sleep(IIC_SLAVE_RECEIVE_THREAD_LOOP_TIME_MS);
       SOH_Alive(IIC_SLAVE_RECEIVE_SOH_TIMEOUT_MS);
   	}
   SOH_Dormant();
}

static bool_t Open_IIC_Slave_Handles(IIC_Channel_T chan_num, uint8_t slave_addr)
{
   bool_t ret_val=false;
   IIC_Slave_Channel_T* chan=NULL;
   int set_adress_results;
   PBC_Require_1(chan_num < NUMBER_IIC_CHANNELS, "Invalid IIC Slave Channel %d", chan_num);

   chan = Slave_Attr[chan_num];
   PBC_Require_1(chan != NULL, "IIC Slave Channel %d is not enabled", chan_num);

   chan->write_handle=open(iic_slave_channel_name[chan_num], O_WRONLY);
   if(0<=chan->write_handle)
   {
      chan->read_handle=open(iic_slave_channel_name[chan_num], O_RDONLY);
      if(0<=chan->read_handle)
      {
    	  set_adress_results=ioctl(chan->write_handle, I2C_SLAVE, slave_addr);
    	  if(0<=set_adress_results)
    	  {
    	     ret_val=true;
    	  }
    	  else
    	  {
    	     Tr_Warn("Unable to Set slave address");
    		 close(chan->write_handle);
             close(chan->read_handle);
    	  }
      }
      else
      {
         Tr_Warn("Unable to open slave-i2c-0 for read");
		 close(chan->write_handle);
      }
   }
   else
   {
      Tr_Warn("Unable to open slave-i2c-0 for write");
   }

   return(ret_val);
}


static IIC_Slave_Channel_T *Create_Slave_Channel(IIC_Channel_T chan_num)
{
   IIC_Slave_Channel_T *chan;
   SAL_Semaphore_Attr_T attr;

   chan = SAL_Alloc(sizeof(IIC_Slave_Channel_T));

   if(NULL != chan)
   {
      memset(chan, 0, sizeof(IIC_Slave_Channel_T));

      SAL_Init_Semaphore_Attr(&attr);
      SAL_Create_Semaphore(&(chan->tx_loaded), &attr);

      chan->enabled = true;
   }
   return chan;
}

/**
 * Release memory allocated for slave channel information
 */
static void Destroy_Slave_Channel(IIC_Channel_T chan_num)
{
	IIC_Slave_Channel_T *chan = Slave_Attr[chan_num];

   Start_Critical();

   Slave_Attr[chan_num] = NULL;

   End_Critical();

   SAL_Destroy_Semaphore(&(chan->tx_loaded));

   SAL_Free(chan);
}

/**
 * Initializes IIC Slave Wrapper
 */
static void IIC_Slave_Init(void)
{
   SAL_Mutex_Attr_T attr;

   /* create mutex */
   SAL_Init_Mutex_Attr(&attr);
   SAL_Create_Mutex(&Slave_Lock, &attr);

   /* initialize chan information (set pointers to NULL) */
   memset(&Slave_Attr, 0, sizeof(Slave_Attr));
}

/**
 * Begin access to shared iic slave data
 */
static void Start_Critical(void)
{
   bool_t success;

   success = SAL_Lock_Mutex_Timeout(&Slave_Lock, IIC_SLAVE_MUTEX_WAIT_TIMEOUT_MS);

   PBC_Ensure(success, "Failed to lock  data");
}

/**
 * End access to shared iic slave data
 */
static void End_Critical(void)
{
   SAL_Unlock_Mutex(&Slave_Lock);
}

/*
 * Report if last trasnmit message has been read by master
 */
bool_t IIC_SLV_Transmit_Available(IIC_Channel_T chan_num)
{
	IIC_Slave_Channel_T *chan;
   bool_t available = false;

   PBC_Require_1(chan_num < NUMBER_IIC_CHANNELS, "Invalid IIC Slave Channel %d", chan_num);

   Start_Critical();

   chan = Slave_Attr[chan_num];
   PBC_Require_1(chan != NULL, "IIC Slave Channel %d is not enabled", chan_num);

   available = !(chan->tx_buf[0].pending || chan->tx_buf[1].pending);

   End_Critical();
   return available;
}

/*
 * Allow new transmit message to be sent without requiring last message to have been read
 */
void IIC_SLV_Transmit_Reset(IIC_Channel_T chan_num)
{
   IIC_Slave_Channel_T *chan;
   PBC_Require_1(chan_num < NUMBER_IIC_CHANNELS, "Invalid IIC Slave Channel %d", chan_num);
   Tr_Info_Hi_1("channel %d",chan_num);
   Start_Critical();

   chan = Slave_Attr[chan_num];
   PBC_Require_1(chan != NULL, "IIC Slave Channel %d is not enabled", chan_num);

   chan->tx_buf[0].pending = false;
   chan->tx_buf[1].pending = false;

   End_Critical();

   /*IIC_Slave_Cancel(chan_num);*/
}

/**
 * Function: IIC_SLV_Load_Driver
 *
 * Loads slave IIC device driver and checks it was correctly loaded
 * the actual loading of the driver may not happen synchronously so
 * if first check fails, a re-check is performed IIC_SLV_SLAVE_CHECK_RETRIES
 * times with an interval of IIC_SLV_SLAVE_CHECK_WAIT_MS
 *
 * @return success True if correctly loaded the device driver
 *                 False is loading failed
 */

static bool_t IIC_SLV_Load_Driver(void)
{

   int32_t file_desc;
   uint8_t check_retries;
   size_t  byte_num;

   bool_t ret_val;

   /*Unbind from master device*/
   file_desc=open(IIC_SLV_MASTER_UNBIND_PATH,O_WRONLY);
   if(-1 != file_desc)
   {
	 byte_num=write(file_desc,IIC_SLV_DRIVER_NAME,strlen(IIC_SLV_DRIVER_NAME));
     if((strlen(IIC_SLV_DRIVER_NAME) < byte_num))
     {
       Tr_Warn("Write to master unbind path failed");
     }
     close(file_desc);
   }
   else
   {
      Tr_Warn("Master i2c driver does not exist");
   }

   /*Bind to slave device*/
   file_desc=open(IIC_SLV_SLAVE_BIND_PATH,O_WRONLY);
   if(-1 != file_desc)
   {
      byte_num=write(file_desc,IIC_SLV_DRIVER_NAME,strlen(IIC_SLV_DRIVER_NAME));
      if((strlen(IIC_SLV_DRIVER_NAME) < byte_num))
      {
        Tr_Warn("Write to slave bind path failed");
      }
      close(file_desc);
   }
   else
   {
      Tr_Warn("Slave i2c driver does not exist");
   }
   /*Validate device node creation*/
   ret_val=false;
   check_retries=0;
   while( (!ret_val) && (IIC_SLV_SLAVE_CHECK_RETRIES > check_retries))
   {
	  file_desc=open(IIC_SLV_DEVICE_NODE_NAME,O_RDONLY);
	  if(-1 != file_desc)
      {
         Tr_Info_Mid("IIC slave device node created correctly");
         close(file_desc);
         ret_val=true;
         break;
      }
      else
      {
         if (IIC_SLV_SLAVE_CHECK_RETRIES > check_retries + 1)
         {
            Tr_Info_Mid_1("IIC slave device node creation check failed in retry:%d",check_retries);
         }
      }
      check_retries++;
      SAL_Sleep(IIC_SLV_SLAVE_CHECK_WAIT_MS);
   }

   if (!ret_val)
   {
       Tr_Fault_1("IIC slave device node creation check failed after %d tries",check_retries);
   }

   return(ret_val);
}

/*===========================================================================*/
/*!
 * @file iic_slave_linux.ch
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 * ----------- --------
 *
 * - 29-Jul-2013 Larry Piekarski Rev 15
 *   - Task 162164: Removed unneeded include file
 *
 *   05-July-2012 Keerthika Pandiselvan
 *   Task kok_basa#107099 NULL Derefernce fix
 *
 *   9-May-2012 Brian Bolinger
 *   SCR 22979 Incorporate system level SOH Timeout - bb_base_drv_linux
 *  
 * - 23-Apr-2012 Doug Srail
 *   Task kok_basa#91956 - clear pending flag on failed write() to recover 
 *   more quickly from failed write
 *
 * - 10-Feb-2012 Doug Srail
 *   Task kok_basa#75630 - change IIC_SLAVE_SEND_THREAD SOH timeout to 35 seconds
 *   to allow for lockup in the write() function for up to 30 seconds
 *
 * - 30-Jan-2012 Doug Srail
 *   task kok_basa#72839 - ensure threads are not active before starting/restarting 
 *   them.  This avoids a potential "Uninitialized thread attribute" PBC violation 
 *   if thread was not terminated in IIC_SLV_Disable() prior to being restarted in 
 *   IIC_SLV_Enable().
 *
 * - 29-Nov-2011 Jon Knoch
 *   Task kok_basa#60252 - In IIC_SLV_Load_Driver(), changed debug output so
 *   that a WARN message is not generated at each check for IIC slave device
 *   node creation since an initial "failure" is expected.  Replaced with 
 *   Info_Mid for each failure and added a Fault if routine is exited without 
 *   success.
 *
 * - 11-Oct-2011 Doug Srail
 *   modified mutex wait and thread destroy timeouts to avoid unnecessary faults
 *
 * - 28-Apr-2011 Doug Srail
 *   moved max size limits to config file
 *
 * - 14-Apr-2011 Doug Srail
 *   Task kok_basa#27989 - add SOH at 5 seconds
 *
 * - 06-Apr-2011 Antonio Fernandez
 *   Task kok_basa#27528 - Integrate last iic linux slave support
 *                         (dynamic slave driver load)
 *
 * - 05-Apr-2011 Doug Srail
 *   Task kok_basa#27317 - temporarily disable SOH for slave send
 *
 * - 31-Mar-2011 Doug Srail
 *   Task kok_basa#26620 - add SOH
 *
 * - 21-dec-2010 Antonio Fernandez rev 1
 *   - created initial file
 *
 */
/*===========================================================================*/
