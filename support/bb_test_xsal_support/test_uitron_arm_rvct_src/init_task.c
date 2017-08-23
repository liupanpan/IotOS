/*   (C) Renesas Technology Europe Limited 2006, All rights reserved    */
/*===========================================================================*/
/**
 * @file init_task.c
 *
 * Startup task for the uItron system. This file contains the code for
 * the one task specified in the uItron configuration tool. Once the kernel
 * is up and running, it will start this task. All other tasks will be
 * started from this task once the necessary system initialization has
 * been completed.
 *
 * %full_filespec:init_task.c~1:csrc:kok_aud#7 %
 * @version %version:1 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Tue Aug  5 15:05:59 2008 %
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "em.h"
#include "init_drv.h"
/* #include "xsal.h"   */

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Local Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Local Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Definitions
\*===========================================================================*/

/*===========================================================================*\
 * Local Object Definitions
\*===========================================================================*/
EM_FILENUM(TEST_MODULE_ID, 1);  /* define this file for assert handling */

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/

/* Application logic */
extern void Application_Initialization(void);

/*
 * init_task()
 *
 * This task is created and activated by Configurator. After performing any
 * needed system initialization, it calls main(), which contains any
 * application-specific logic.
 *
 */
void App_Main(void)
{
   /* initialize the drivers */
   init_drv();

   Application_Initialization();        /* app. threads set up here */
   /* should not return */

   EM_REQUIRE_FAILED(0);
}

/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        Name      (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 30-jul-2007 kirk bailey
 * + Changed SMP_init() call to use SH2A cache line size of 16 instead of
 * + 32 per Renesas recommendation.
 *
 *  7 Feb 2007 Dan Carman
 *  + Modified Init_Task to call Applciation_Initailization instead of main 
 *
 *  1 Feb 2007 Dan Carman
 *  + Corrected test of return value for SMP_Init
 *
 * 31-jan-2007 kirk bailey
 * + Added configuration logic. Added revision block and header block with
 * + SCM keywords.
 *
\*===========================================================================*/
