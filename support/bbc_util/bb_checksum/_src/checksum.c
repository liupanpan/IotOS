/*===========================================================================*/
/**
 * @file checksum.c
 *
 * The file contains functions for checksum calculation and verification.
 *
 * %full_filespec:checksum.c~kok_basa#8:csrc:tck_aud#1 %
 * @version %version:kok_basa#8 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Wed Jul 17 14:17:31 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * The checkum.c file contains functions for checksum calculation and verification.
 *
 * @section ABBR ABBREVIATIONS:
 *   - None.
 *
 * @section TRA TRACEABILITY INFO:
 *   - Design Document(s):
 *     - SDD_BASA_Utilities.doc
 *
 *   - Requirements Document(s):
 *     - SRS_BASA_Utilities.doc
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#include "checksum.h"
#include "checksum_cfg.h"
#include "pbc_trace.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 7); /**< IDs file for EM assert handling */

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

/*===========================================================================*\
 * Local Function Prototypes
\*===========================================================================*/

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
\*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
\*===========================================================================*/


/**
 * Calculates checksum of given data block
 */
uint16_t Chks_Sum_Bytes(const void *start_addr, size_t size)
{
   const uint8_t *ptr = (const uint8_t *)start_addr;    /* pointer to start addr. */
   uint16_t sum = 0;

#if CHKS_NULL_ADDRESS_PROHIBITED  
   PBC_Require(start_addr != NULL, "Chks_Sum_Bytes cannot be called with a NULL pointer");
#endif

   for (; (0 != size); size--)
   {
      sum += *ptr++;            /* add the bytes */
   }

   return (sum);
}

/*
 * This function writes data and adjusts checksum for partial change.
 */
bool_t Chks_Write(void *dest, const void *source, size_t count, uint16_t * checksum)
{
   uint8_t *dptr = (uint8_t *) dest;    /* pointer to dest addr */
   const uint8_t *sptr = (const uint8_t *)source;       /* pointer to source addr */
   uint16_t delta = 0;
   uint8_t new_data;
   uint8_t old_data;

   bool_fast data_change = false;

#if CHKS_NULL_ADDRESS_PROHIBITED  
   PBC_Require((dest != NULL) && (source != NULL), "Chks_Write cannot be called with a NULL pointer");
#endif
   PBC_Require((checksum != NULL), "Chks_Write cannot be called with a NULL pointer");

   for (; (0 != count); count--)
   {
      new_data = *sptr;
      old_data = *dptr;
      if (new_data != old_data)
      {
         delta += new_data;
         delta -= old_data;
         *dptr = new_data;
         data_change = true;
      }
      dptr++;
      sptr++;
   }

   if (0 != delta)
   {
      *checksum += delta;
   }

   return (data_change);
}

/**
 * Calculates and updates 16-bit chceck sum for given memory block.
 */
void Chks_Update(const void *start_addr, size_t size, uint16_t checksum_offset, uint16_t * checksum)
{
   const uint8_t *ptr = (const uint8_t *)start_addr;
   uint16_t temp_chks = checksum_offset;        /* initialize checksum calculation */

#if CHKS_NULL_ADDRESS_PROHIBITED  
   PBC_Require((start_addr != NULL), "Chks_Update cannot be called with a NULL pointer");
#endif
   PBC_Require((checksum != NULL), "Chks_Update cannot be called with a NULL pointer");

   /* checksum calculation */
   for (; (0 != size); size--)
   {
      temp_chks += *ptr++;
   }

   *checksum = temp_chks;
}


/**
 * Calculates 16-bit checksum for given memory block and compares to input checksum
 */
bool_t Chks_Verify(const void *start_addr, size_t size, uint16_t checksum_offset, uint16_t checksum)
{
   const uint8_t *ptr = (const uint8_t *)start_addr;
   uint16_t chs_sum = checksum_offset;  /* initialize checksum calculation */

#if CHKS_NULL_ADDRESS_PROHIBITED  
   PBC_Require(start_addr != NULL, "Chks_Verify cannot be called with a NULL pointer");
#endif

   /* checksum calculation */
   for (; (0 != size); size--)
   {
      chs_sum += *ptr++;
   }

   return (chs_sum == checksum);
}

/*===========================================================================*/
/*!
 * @file checksum.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 17-Jul-2013 Larry Piekarski Rev 8
 *   - Task 160933: Renamed util_cfg.h to checksum_cfg.h to support the
 *     conversion of bb_util into a building block cluster.
 *
 * 4Nov11  David Origer  Rev 7
 * SCR kok_basa#17494 : Allow use of NULL pointer for checksums when used in
 *    200C systems.
 *
 * - 12-Jul-2010 Larry Piekarski Rev 6
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 08-Jul-2009 Larry Piekarski Rev 5
 *   - Peer Review Updates
 *
 * - 12-Oct-2007 Dan Carman
 *   - Modified chks_write to return if data changed
 *
 * - 06-Oct-2007 Bartosz Jamrozik
 *   - Added functions: Chks_Write, Chks_Update, Chks_Verify
 *
 * - 18-Aug-2007 Dan Carman
 *   - MISRA Changed
 *
 * - 22-May-2007 Bartosz Jamrozik
 *   - Created initial file.
 */
/*==========================================================================*/
