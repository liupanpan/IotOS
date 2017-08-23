/*   (C) Renesas Technology Europe Limited 2006, All rights reserved    */

#ifndef INIT_DRV_H_INCLUDED
#   define INIT_DRV_H_INCLUDED
/*===========================================================================*/
/**
 * @file init_drv.h
 *
 * Defines the public interface for initializing the drivers and I/O interface.
 *
 * %full_filespec:init_drv.h~1:incl:kok_aud#8 %
 * @version %version:1 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Tue Aug  5 15:05:48 2008 %
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/

/*===========================================================================*\
 * Exported Preprocessor #define MACROS
\*===========================================================================*/

/*===========================================================================*\
 * Exported Type Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Const Object Declarations
\*===========================================================================*/

/*===========================================================================*\
 * Exported Function Prototypes
\*===========================================================================*/

/**
 * Initializes device drivers and I/O interface.
 */
void init_drv(void);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * File Revision History (top to bottom: last revision to first revision)
 *===========================================================================
 *
 * Date        userid    (Description on following lines: SCR #, etc.)
 * ----------- --------
 * 30-jan-2007 dzq92s
 * + Moved constants to ioif_cfg.h. Added revision block and file header with
 * + SCM keywords.
 *
\*===========================================================================*/
#endif /* INIT_DRV_H_INCLUDED */
