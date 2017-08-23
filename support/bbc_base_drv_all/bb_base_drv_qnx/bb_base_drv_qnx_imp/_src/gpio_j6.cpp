/*===========================================================================*/
/**
 * @file gpio_j6.cpp
 *
 * @brief Discrete I/O wrapper for qnx J6 / J6Eco GPIO
 *
 * %full_filespec: gpio_j6.cpp~2:ascii:kok_inf#1 %
 * @version %version: 2 %
 * @author  %derived_by: jz4fpf %
 * @date    %date_modified: Wed May  6 19:03:08 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2011-2015 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 *   This module provides a implementation of GPIO wrapper
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
#include <stdint.h>
#include <sys/mman.h>
#include <hw/inout.h>
#include <stdio.h>
#include "omap_gpio.h"

#if defined(_MICRO_J6_)
#include "arm/dra74x.h"
#elif defined(_MICRO_J6Eco_)
#include "arm/dra72x.h"
#else
#error Unsupported micro
#endif

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
#define NUM_PORTS       (NUM_GPIO_BANKS)
#endif

#define FIRST_PORT   DIO_GPIO1
#define LAST_PORT    DIO_GPIO7

/**
 * Number of pins per "chip" or port
 */
#define PIN_PER_PORT    (32)

/*===========================================================================*\
 * Local Type Declarations
 \*===========================================================================*/

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

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

#if defined(_MICRO_J6_)

static uint32_t gpio_phys_base[NUM_GPIO_BANKS] =
   { DRA74X_GPIO1_BASE,
      DRA74X_GPIO2_BASE,
      DRA74X_GPIO3_BASE,
      DRA74X_GPIO4_BASE,
      DRA74X_GPIO5_BASE,
      DRA74X_GPIO6_BASE,
      DRA74X_GPIO7_BASE,
      DRA74X_GPIO8_BASE };
	  
#elif defined(_MICRO_J6Eco_)

static uint32_t gpio_phys_base[NUM_GPIO_BANKS] =
   { DRA72X_GPIO1_BASE,
      DRA72X_GPIO2_BASE,
      DRA72X_GPIO3_BASE,
      DRA72X_GPIO4_BASE,
      DRA72X_GPIO5_BASE,
      DRA72X_GPIO6_BASE,
      DRA72X_GPIO7_BASE,
      DRA72X_GPIO8_BASE };
	  
#else
#error Unsupported micro
#endif

static uintptr_t gpio_virt_base[NUM_GPIO_BANKS];

void my_out32_g(int bank, uint32_t offset, uint32_t value)
{
   out32(gpio_virt_base[bank] + offset, value);
}

uint32_t my_in32_g(int bank, uint32_t offset)
{
   return in32(gpio_virt_base[bank] + offset);
}

#define GPIO_OFF(_regname)       (GPIO_ ## _regname)
#define GPIO_BIT_MASK(_num)         (1 << ((_num) & 0x1f))

#define READ_MOD_WRITE_REG_BIT(_bank, _regname, _num, _value) \
   my_out32_g(_bank, GPIO_OFF(_regname), (my_in32_g(_bank, GPIO_OFF(_regname)) & ~GPIO_BIT_MASK(_num)) | ((_value) << _num))

#define READ_REG_BIT(_bank, _regname, _num) \
   ((my_in32_g(_bank, GPIO_OFF(_regname)) >> _num) & 1)

int omap_gpio_init(void)
{
   int bank;

   for (bank = 0; bank < NUM_GPIO_BANKS; bank++)
   {
      gpio_virt_base[bank] = mmap_device_io(GPIO_SIZE, gpio_phys_base[bank]);
      if (gpio_virt_base[bank] == (uintptr_t) MAP_FAILED )
      {
         perror("mmap_device_io");
         return -1;
      }
   }

   return 0;
}

void omap_gpio_deinit(void)
{
   int bank;

   for (bank = 0; bank < NUM_GPIO_BANKS; bank++)
   {
      if ((gpio_virt_base[bank]) && (gpio_virt_base[bank] != (uintptr_t) MAP_FAILED ))
      {
         munmap_device_io(gpio_virt_base[bank], GPIO_SIZE);
         gpio_virt_base[bank] = (uintptr_t) NULL;
      }
   }
}

/*
 * gpio:
 * 0 -- GPIO1
 * 1 -- GPIO2
 * 2 -- GPIO3
 * ...
 * num: bit number
 * dir:
 * 0: Output enabled
 * 1: Output disabled
 */
int omap_gpio_set_direction(int gpio, int num, int dir)
{
   READ_MOD_WRITE_REG_BIT(gpio, OE, num, dir);
   return 0;
}

int omap_gpio_get_direction(int gpio, int num)
{
   return READ_REG_BIT(gpio, OE, num);
}

/*
 * gpio: 0/1/2/3/4/5/6/7
 * num: bit number: 0--31
 * 1: high, 0: low
 */
int omap_gpio_set_output_state(int gpio, int num, int value)
{
   if (value)
      my_out32_g(gpio, GPIO_OFF(SETDATAOUT), 1 << num);
   else
      my_out32_g(gpio, GPIO_OFF(CLEARDATAOUT), 1 << num);
   return 0;
}

int omap_gpio_get_output_state(int gpio, int num)
{
   return READ_REG_BIT(gpio, DATAOUT, num);
}

int omap_gpio_get_input_state(int gpio, int num)
{
   return READ_REG_BIT(gpio, DATAIN, num);
}

int omap_gpio_set_highleveldetect(int gpio, int num, int enabled)
{
   READ_MOD_WRITE_REG_BIT(gpio, LEVELDETECT1, num, enabled);
   return 0;
}

int omap_gpio_get_highleveldetect(int gpio, int num)
{
   return READ_REG_BIT(gpio, LEVELDETECT1, num);
}

int omap_gpio_set_lowleveldetect(int gpio, int num, int enabled)
{
   READ_MOD_WRITE_REG_BIT(gpio, LEVELDETECT0, num, enabled);
   return 0;
}

int omap_gpio_get_lowleveldetect(int gpio, int num)
{
   return READ_REG_BIT(gpio, LEVELDETECT0, num);
}

int omap_gpio_set_risingdetect(int gpio, int num, int enabled)
{
   READ_MOD_WRITE_REG_BIT(gpio, RISINGDETECT, num, enabled);
   return 0;
}

int omap_gpio_get_risingdetect(int gpio, int num)
{
   return READ_REG_BIT(gpio, RISINGDETECT, num);
}

int omap_gpio_set_fallingdetect(int gpio, int num, int enabled)
{
   READ_MOD_WRITE_REG_BIT(gpio, FALLINGDETECT, num, enabled);
   return 0;
}

int omap_gpio_get_fallingdetect(int gpio, int num)
{
   return READ_REG_BIT(gpio, FALLINGDETECT, num);
}

int omap_gpio_set_debounce_state(int gpio, int num, int enabled)
{
   READ_MOD_WRITE_REG_BIT(gpio, DEBOUNCEENABLE, num, enabled);
   return 0;
}

int omap_gpio_get_debounce_state(int gpio, int num)
{
   return READ_REG_BIT(gpio, DEBOUNCEENABLE, num);
}

int omap_gpio_get_irq_status(int gpio, int num)
{
   return READ_REG_BIT(gpio, IRQSTATUS_0, num);
}

int omap_gpio_clear_irq(int gpio, int num)
{
   my_out32_g(gpio, GPIO_OFF(IRQSTATUS_0), 1 << num);
   return 0;
}

int omap_gpio_set_irqenable_state(int gpio, int num, int enabled)
{
   if (enabled)
      my_out32_g(gpio, GPIO_OFF(IRQSTATUS_SET_0), 1 << num);
   else
      my_out32_g(gpio, GPIO_OFF(IRQSTATUS_CLR_0), 1 << num);
   return 0;
}

int omap_gpio_get_irqenable_state(int gpio, int num)
{
   return READ_REG_BIT(gpio, IRQSTATUS_SET_0, num);
}

/*
 * Initialize DIO module
 */
bool_t DIO_Initialize(void)
{
   omap_gpio_init();
   return true;
}

void DIO_Shutdown(void)
{
   omap_gpio_deinit();
}

/*
 * Read DIO input
 */
bool_t DIO_Is_High(DIO_Port_T port, uint8_t pin)
{
   bool_t high = false;

   if (!gpio_virt_base[0])
   {
      DIO_Initialize();
   }

   if ((pin < PIN_PER_PORT) && ((port >= FIRST_PORT) && (port <= LAST_PORT)))
   {
      int gpio_bank = port - FIRST_PORT;
      high = omap_gpio_get_input_state(gpio_bank, pin) ? true : false;
   }
   else
   {
      Tr_Fault_2("DIO_Is_High: Invalid GPIO port %d or pin %d", port, pin);
   }

   return high;
}

/*
 * Sets DIO output
 */
void DIO_Set_High(DIO_Port_T port, uint8_t pin, bool_t high)
{
   if (!gpio_virt_base[0])
   {
      DIO_Initialize();
   }

   if ((port >= FIRST_PORT) && (port <= LAST_PORT)&& (pin < PIN_PER_PORT))
   {
      int gpio_bank = port - FIRST_PORT;
      omap_gpio_set_output_state(gpio_bank, pin, high);
   }
   else
   {
      Tr_Fault_2("DIO_Set_High: Invalid GPIO port %d or pin %d", port, pin);
   }
}

   /*
    * Set Direction for DIO pin
    */
void DIO_Set_Direction(DIO_Port_T port, uint8_t pin, DIO_Direction_T direction)
{
   if (!gpio_virt_base[0])
   {
      DIO_Initialize();
   }

   if ((port >= FIRST_PORT) && (port <= LAST_PORT)&& (pin < PIN_PER_PORT))
   {
      int gpio_bank = port - FIRST_PORT;
      omap_gpio_set_direction(gpio_bank, pin, ((DIO_INPUT == direction) ? GPIO_DIR_IN : GPIO_DIR_OUT));
   }
   else
   {
      Tr_Fault_2("DIO_Set_Direction: Invalid GPIO port %d or pin %d", port, pin);
   }
}

/*===========================================================================*/
/*!
 * @file gpio_j6.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 3-April-2014 Dan Carman
 *   - Initial version
 */
/*===========================================================================*/
