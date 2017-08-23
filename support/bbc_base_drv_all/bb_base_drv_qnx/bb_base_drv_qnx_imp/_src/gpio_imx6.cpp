/*===========================================================================*/
/**
 * @file gpio_imx.c
 *
 * @brief Discrete I/O wrapper for qnx GPIO on IMX6
 *
 * %full_filespec: gpio_imx6.cpp~1:ascii:kok_inf#1 %
 * @version %version: 1 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Fri Sep  5 16:46:53 2014 %
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
#include <arm/mx6x.h>
#include <hw/inout.h>
#include <sys/mman.h>

EM_FILENUM(BASE_DRIVERS_MODULE_ID, 0);

/*===========================================================================*\
 * Exported Preprocessor #define X-MACROS
 \*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
 \*===========================================================================*/

/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/

#define FIRST_PORT   DIO_GPIO1
#define LAST_PORT    DIO_GPIO7
#define NUM_PORTS    Num_Elems(gpio_phys_base)
#define PIN_PER_PORT (32)

/*===========================================================================*\
 * Local Preprocessor #define MACROS
 \*===========================================================================*/

#define GPIO_OFF(_regname)       (MX6X_GPIO_ ## _regname)

#define GPIO_BIT_MASK(_num)		(1 << ((_num) & 0x1f))

#define READ_MOD_WRITE_REG_BIT(_bank, _regname, _num, _value) \
   my_out32_g(_bank, GPIO_OFF(_regname), (my_in32_g(_bank, GPIO_OFF(_regname)) & ~GPIO_BIT_MASK(_num)) | ((_value) << _num))

#define READ_REG_BIT(_bank, _regname, _num) \
   ((my_in32_g(_bank, GPIO_OFF(_regname)) >> _num) & 1)

/*===========================================================================*\
 * Local Type Declarations
 \*===========================================================================*/
static const uint32_t gpio_phys_base[] =
   { MX6X_GPIO1_BASE, MX6X_GPIO2_BASE, MX6X_GPIO3_BASE, MX6X_GPIO4_BASE, MX6X_GPIO5_BASE, MX6X_GPIO6_BASE, MX6X_GPIO7_BASE };

static uintptr_t gpio_virt_base[NUM_PORTS] =
   { (uintptr_t) MAP_FAILED,
      (uintptr_t) MAP_FAILED,
      (uintptr_t) MAP_FAILED,
      (uintptr_t) MAP_FAILED,
      (uintptr_t) MAP_FAILED,
      (uintptr_t) MAP_FAILED,
      (uintptr_t) MAP_FAILED };

/*===========================================================================*\
 * Exported Const Object Definitions
 \*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
 \*===========================================================================*/

/*===========================================================================*\
 * Local Function Prototypes
 \*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
 \*===========================================================================*/
static inline void my_out32_g(size_t bank, uint32_t offset, uint32_t value)
{
   out32(gpio_virt_base[bank] + offset, value);
}

static inline uint32_t my_in32_g(size_t bank, uint32_t offset)
{
   return in32(gpio_virt_base[bank] + offset);
}

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

/*
 * Initialize GPIO module
 */
bool_t DIO_Initialize(void)
{
   size_t bank;

   for (bank = 0; bank < NUM_PORTS; bank++)
   {
      gpio_virt_base[bank] = mmap_device_io(MX6X_GPIO_SIZE, gpio_phys_base[bank]);
      if (gpio_virt_base[bank] == (uintptr_t) MAP_FAILED )
      {
         perror("mmap_device_io");
         return false;
      }
   }

   return true;
}

void DIO_Shutdown(void)
{
   size_t bank;

   for (bank = 0; bank < NUM_PORTS; bank++)
   {
      if (gpio_virt_base[bank] != (uintptr_t) MAP_FAILED )
      {
         munmap_device_io(gpio_virt_base[bank], MX6X_GPIO_SIZE);
         gpio_virt_base[bank] = (uintptr_t) MAP_FAILED;
      }
   }
}

/*
 * Read GPIO input
 */
bool_t DIO_Is_High(DIO_Port_T port, uint8_t pin)
{
   bool_t high = false;

   if ((pin < PIN_PER_PORT) && ((port >= FIRST_PORT) && (port <= LAST_PORT)))
   {
      int gpio_bank = port - FIRST_PORT;
      high = READ_REG_BIT(gpio_bank, DR, pin);
   }
   else
   {
      Tr_Fault_2("DIO_Is_High: Invalid GPIO port %d or pin %d", port, pin);
   }

   return high;
}

/*
 * Sets GPIO output
 */
void DIO_Set_High(DIO_Port_T port, uint8_t pin, bool_t high)
{
   if ((port >= FIRST_PORT) && (port <= LAST_PORT) && (pin < PIN_PER_PORT))
   {
      int gpio_bank = port - FIRST_PORT;

      READ_MOD_WRITE_REG_BIT(gpio_bank, DR, pin, high);
   }
   else
   {
      Tr_Fault_2("DIO_Set_High: Invalid GPIO port %d or pin %d", port, pin);
   }
}

/*
 * Set Direction for GPIO pin
 */
void DIO_Set_Direction(DIO_Port_T port, uint8_t pin, DIO_Direction_T direction)
{
   if ((port >= FIRST_PORT) && (port <= LAST_PORT) && (pin < PIN_PER_PORT))
   {
      int gpio_bank = port - FIRST_PORT;
      READ_MOD_WRITE_REG_BIT(gpio_bank, GDIR, pin, direction);
   }
   else
   {
      Tr_Fault_2("DIO_Set_Direction: Invalid GPIO port %d or pin %d", port, pin);
   }
}

/*===========================================================================*/
/*!
 * @file gpio_imx6.cpp
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 5-Sept-2014 Dan Carman
 *   Task 11694 - Update QNX base drivers to work with J6 and IMX6
 *    + Use standard PORT Ids with checking for valid IMX6 ports
 *	  + Modified structure to have single micro specific implementation included into 
 *    common base file based on MICRO definition. 
 *
 * - 03-Sep-2014 Vijayalakshmi
 *   - Initial version
 */
/*===========================================================================*/
