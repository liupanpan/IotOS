/*===========================================================================*/
/**
 * @file dio_linux.c
 *
 * @brief Discrete I/O wrapper for Linux GPIO 
 *
 * %full_filespec: dio_linux.c~kok_inf#6:csrc:kok_basa#4 %
 * @version %version: kok_inf#6 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Tue Aug 19 17:16:06 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   This module provides a dummy implementation of GPIO wrapper
 *
 * @section ABBR ABBREVIATIONS:
 *   - DIO - Discrete I/O (standard binary (high / low) input / output pin)
 *   - GPIO - General Purpose I/O another common name for DIO
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/
#include "dio.h"
#include "pbc_trace.h"
#include "reuse.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*===========================================================================*\
 * Local Preprocessor #define MACROS
 \*===========================================================================*/

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 0);

/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/

#ifndef NUM_PORTS
/**
 * NUmber of supported Ports
 */
#define NUM_PORTS       (7)
#endif

/*
 * offset to first port - 1 for Freescale, 0 for TI
 */
#ifndef GPIO_PORT_OFFSET
#define GPIO_PORT_OFFSET 1
#endif

/**
 * Number of pins per "chip" or port
 */
#define PIN_PER_PORT    (32)

/**
 * Buffer size for path names.
 */
#define GPIO_NAME_SIZE  (40)

/**
 * Number of retries for failed GPIO operations
 */
#define GPIO_ATTEMPTS   (2)

/**
 * Values to track last set direction
 */
#define GPIO_IDR_OFFSET    (16)
#define GPIO_DIR_UNKNOWN   (0)
#define GPIO_DIR_IN        (DIO_INPUT + GPIO_IDR_OFFSET)
#define GPIO_DIR_OUT       (DIO_OUTPUT + GPIO_IDR_OFFSET)

/*===========================================================================*\
 * Local Type Declarations
 \*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
 \*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
 \*===========================================================================*/

/**
 * file handles to GPIO values
 */
static int GPIO_Handles[NUM_PORTS * PIN_PER_PORT] =
{ 0 };

/**
 * file handles to GPIO directions
 */
static int GPIO_Dir_Handles[NUM_PORTS * PIN_PER_PORT] =
{ 0 };

/**
 * Last set direction for each PIN
 */
static uint8_t GPIO_Direction[NUM_PORTS * PIN_PER_PORT] =
{ 0 };

/*===========================================================================*\
 * Local Function Prototypes
 \*===========================================================================*/
static unsigned int GPIO_ID(DIO_Port_T port, uint8_t pin);
static void GPIO_Name(char * gpio_name, const char * parameter, int gpio_pin);
static void Export_GPIO(int gpio_ID);
static bool_t Check_Export(int gpio_ID);
static int Get_Value_Handle(DIO_Port_T port, uint8_t pin);
static void Close_GPIO_Handle(DIO_Port_T port, uint8_t pin);
static bool_t Set_Direction_Out(unsigned int gpio_ID, bool_t output);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
 \*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

/** 
 * Convert Port / Pin ID to Linux sequential ID 
 *  This implementation assume homogeneous "pins" per
 *
 * @param port - Port designation
 * @param pin - pin number within port 
 *
 * @return Linux Pin ID 
 */
static unsigned int GPIO_ID(DIO_Port_T port, uint8_t pin)
{
   PBC_Require_1(port < NUM_PORTS, "Invalid port %d", port);
   PBC_Require_2(pin < PIN_PER_PORT, "Invalid pin %d on GPIO%d", pin, (int)port + GPIO_PORT_OFFSET);

   return (((int)port * PIN_PER_PORT) + pin);
}

/**
 * Generates a file name for a GPIO operation 
 *
 * @param [out] gpio_name - pointer to buffer to store file name 
 * @param parameter - string for parameter operation ("value" or "direction")
 * @param gpio_ID- Linux GPIO ID
 */
static void GPIO_Name(char * gpio_name, const char * parameter, int gpio_ID)
{
   snprintf(gpio_name, GPIO_NAME_SIZE, "/sys/class/gpio/gpio%d/%s", gpio_ID, parameter);
}

/**
 * Exports an given PIN interface
 *
 * @param gpio_ID- Linux GPIO ID
 */
static void Export_GPIO(int gpio_ID)
{
   int fd;
   char gpio_num[4];
   unsigned int bytes;

   Tr_Info_Mid_1("Exporting GPIO # %d", gpio_ID);

   fd = open("/sys/class/gpio/export", O_WRONLY, 0777);
   PBC_Ensure(fd > 0, "Cannot open GPIO export file");

   snprintf(gpio_num, sizeof(gpio_num), "%d", gpio_ID);
   bytes = write(fd, &gpio_num, strlen(gpio_num));

   if (bytes != strlen(gpio_num))
   {
      Tr_Warn_1("Unable to export gpio %d", gpio_ID);
   }

   close(fd);
}

/**
 * Does Exports only if needed (pin doesn't exist)
 *
 * @param gpio_ID - Pin ID
 *
 * @return true is pin was successfully exported
 */

static bool_t Check_Export(int gpio_ID)
{
   struct stat sb;
   int status;
   char gpio_name[GPIO_NAME_SIZE];

   GPIO_Name(gpio_name, "", gpio_ID);

   status = stat(gpio_name, &sb);

   if (status != 0)
   {
      Export_GPIO(gpio_ID);

      status = stat(gpio_name, &sb);
   }

   return (0 == status);
}

/**
 * Get the file descriptor for the requested pin value
 *    returns existing handle if already open, else tries to open
 *
 * @param port - Port designation
 * @param pin - pin number within port 
 *
 * @return file descriptor  
 */
static int Get_Value_Handle(DIO_Port_T port, uint8_t pin)
{
   int fd;
   int gpio_ID = GPIO_ID(port, pin);

   fd = GPIO_Handles[gpio_ID];

   if (fd <= 0) /* GPIO is not already open */
   {
      char gpio_name[GPIO_NAME_SIZE];

      GPIO_Name(gpio_name, "value", gpio_ID);

      fd = open(gpio_name, O_RDWR);

      /* if open failed then try exporting pin and reopening */
      if ((fd < 0) && Check_Export(gpio_ID))
      {
         fd = open(gpio_name, O_RDWR);
      }

      if (fd < 0)
      {
         Tr_Warn_2("Unable to open GPIO%d Pin: %d", ((int) port) + GPIO_PORT_OFFSET, pin);
      }

      GPIO_Handles[gpio_ID] = fd;
   }

   return fd;
}

/**
 * Get the file descriptor for the requested pin direction
 *    returns existing handle if already open, else tries to open
 *
 * @param gpio_ID - index to Port / Pin 
 *
 * @return file descriptor
 */
static int Get_Direction_Handle( int gpio_ID)
{
   int fd;

   fd = GPIO_Dir_Handles[gpio_ID];

   if (fd <= 0) /* GPIO Direction is not already open */
   {
      char gpio_name[GPIO_NAME_SIZE];

      GPIO_Name(gpio_name, "direction", gpio_ID);

      fd = open(gpio_name, O_RDWR);

      /* if open failed then try exporting pin and reopening */
      if ((fd < 0) && Check_Export(gpio_ID))
      {
         fd = open(gpio_name, O_RDWR);
      }

      if (fd < 0)
      {
         Tr_Warn_1("Unable to open GPIO %d", gpio_ID);
      }

      GPIO_Dir_Handles[gpio_ID] = fd;
   }

   return fd;
}

/**
 * Closes a value handle if the pin is open
 *
 * @param port - Port designation
 * @param pin - pin number within port 
 */
static void Close_GPIO_Handle(DIO_Port_T port, uint8_t pin)
{
   int fd;

   int gpio_ID = GPIO_ID(port, pin);

   fd = GPIO_Handles[gpio_ID];

   if (fd > 0) /* GPIO is open */
   {
      close(fd);
      GPIO_Handles[gpio_ID] = -1;
   }

   fd = GPIO_Dir_Handles[gpio_ID];

   if (fd > 0) /* GPIO Direction is open */
   {
      close(fd);
      GPIO_Dir_Handles[gpio_ID] = -1;
   }
}

/*
 * Initialize DIO module
 */
bool_t DIO_Initialize(void)
{
   /* nothing here */
   return true;
}

void DIO_Shutdown(void)
{
   DIO_Port_T port;
   int pin;

   for (port = (DIO_Port_T) 0; port < NUM_PORTS; port++)
   {
      for (pin = 0; pin < PIN_PER_PORT; pin++)
      {
         Close_GPIO_Handle(port, pin);
      }
   }
}

/*
 * Read DIO input
 */
bool_t DIO_Is_High(DIO_Port_T port, uint8_t pin)
{
   int fd;
   int ok = -1;
   int tries = GPIO_ATTEMPTS;
   char value = '0';

   PBC_Require_2((port < NUM_PORTS) && (pin < PIN_PER_PORT), "Invalid port %d or pin %d", port, pin);

   while ((tries-- > 0) && (ok <= 0))
   {
      fd = Get_Value_Handle(port, pin);

      if (fd >= 0) /* do we have a handle */
      {
         lseek(fd, SEEK_SET, 0);
         ok = read(fd, &value, 1);

         Tr_Info_Lo_3("Value %c read from GPIO%d Pin: %d", value, ((int) port) + GPIO_PORT_OFFSET, pin);

         if (ok <= 0) /* if failure then try set direction, reopening and re-reading the driver */
         {
            Set_Direction_Out(GPIO_ID(port, pin), false);
            Close_GPIO_Handle(port, pin);
         }
      }
   }

   if (ok <= 0)
   {
      Tr_Fault_2("Unable to read GPIO%d Pin: %d", ((int) port) + GPIO_PORT_OFFSET, pin);
   }

   return (value != '0');
}

/*
 * Sets DIO output
 */
void DIO_Set_High(DIO_Port_T port, uint8_t pin, bool_t high)
{
   int fd;
   int ok = -1;
   int tries = GPIO_ATTEMPTS;

   PBC_Require_2((port < NUM_PORTS) && (pin < PIN_PER_PORT), "Invalid port %d or pin %d", port, pin);

   while ((tries-- > 0) && (ok <= 0))
   {
      fd = Get_Value_Handle(port, pin);

      if (fd >= 0) /* do we have a handle */
      {
         ok = write(fd, (high ? "1" : "0"), 1);

         if (ok < 0)
         {
            Set_Direction_Out(GPIO_ID(port, pin), true);
            Close_GPIO_Handle(port, pin);
         }
      }
   }

   if (ok <= 0)
   {
      Tr_Fault_3("Unable to write %d to GPIO%d Pin: %d", high, ((int) port) + GPIO_PORT_OFFSET, pin);
   }
}

/*
 * Set Direction for DIO pin
 */
static bool_t Set_Direction_Out(unsigned int gpio_ID, bool_t output)
{
   int fd;
   bool_t success = false;

   PBC_Internal_1(gpio_ID < Num_Elems(GPIO_Direction), "Invalid Pin ID %d", gpio_ID);

   fd = Get_Direction_Handle(gpio_ID);

   if (fd >= 0)
   {
      if (output)
      {
         int bytes;
         bytes = write(fd, "out", 4);
         GPIO_Direction[gpio_ID] = GPIO_DIR_OUT;

         if (bytes != 4)
         {
            Tr_Warn_1("Unable to set direction for gpio %d", gpio_ID);
         }
         else
         {
            success = true;
         }
      }
      else
      {
         int bytes;
         bytes = write(fd, "in", 3);
         GPIO_Direction[gpio_ID] = GPIO_DIR_IN;
         if (bytes != 3)
         {
            Tr_Warn_1("Unable to set direction for gpio %d", gpio_ID);
         }
         else
         {
            success = true;
         }
      }
   }

   return success;
}

/*
 * Set Direction for DIO pin
 */
void DIO_Set_Direction(DIO_Port_T port, uint8_t pin, DIO_Direction_T direction)
{
   int gpio_ID = GPIO_ID(port, pin);

   PBC_Require_3((DIO_INPUT == direction) || (DIO_OUTPUT == direction), "Invalid direction %d for GPIO%d Pin: %d", (int) direction, ((int) port) + GPIO_PORT_OFFSET, pin);

   if (GPIO_Direction[gpio_ID] != ((uint8_t) direction + GPIO_IDR_OFFSET))
   {
      if (!Set_Direction_Out(gpio_ID, (DIO_OUTPUT == direction)))
      {
         Tr_Warn_3("Unable to set direction %d on GPIO%d Pin: %d", (int) direction, (int) port + GPIO_PORT_OFFSET, pin);
      }
   }
}

/*===========================================================================*/
/*!
 * @file xsal_thread_id.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 12-Aug-2014 Dan Carman
 *	  Cached GPIO Direction handles. Significant reduction on throughput for I/O that 
 *    changes direction frequently (e.g. keyboard)
 * 
 * - 15-Oct-2012 Chris Baker
 *   Task kok_basa#123878 Revision 3
 *   - Remove warnings introduced by new GCC compiler version.
 *
 * - 26-Sep-2013 Larry Piekarski Rev 4
 *   - Task 1331: Fixed compiler warnings
 */
/*===========================================================================*/
