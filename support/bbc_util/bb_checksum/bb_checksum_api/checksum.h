#ifndef CHECKSUM_H
#   define CHECKSUM_H
/*===========================================================================*/
/**
 * @file checksum.h
 *
 * API for functions to calculate checksums
 *
 * %full_filespec:checksum.h~kok_basa#9:incl:tck_aud#1 %
 * @version %version:kok_basa#9 %
 * @author  %derived_by:czhswm %
 * @date    %date_modified:Mon Jul 12 12:42:49 2010 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page checksum_page Checksum User Manual
 *
 * @section DESC DESCRIPTION:
 *
 * The checksum sub-module provides set of function used for checksum calculation 
 * and verification. The function can be used to protect data against errors 
 * during transmission or accessing to non volatile memory. 
 *
 * The checksum sub-module provides currently only one function. The function 
 * Chks_Sum_Bytes can be used to calculate 16-bit checksum of given data block. 
 * It can be useful e.g. to detect errors during data packet transmission. 
 *
 * - #Chks_Sum_Bytes 	- returns a checksum of defined data block
 * - #Chks_Update 	- returns calculated 16-bit checksum of given data block. Allows for initialization of the checksum with a predefined value. Supports continuation of the checksum calculation. In this case user must take care about storing an intermediate sum and providing them to the function. 
 * - #Chks_Verify	- verify if given checksum match a checksum of given data block
 * - #Chks_Write		- adjusts a checksum for partial change of data block 
 *
 * The functions: Chks_Update and Chks_Write are not thread-safe. This is the 
 * user responsibility for providing whatever synchronization primitives are 
 * necessary in order to ensure exclusive access to the used static data during 
 * checksum calculation.
 *
 * @section USAGE USAGE INSTRUCTIONS:
 *
 * -# Include bb_util in the project
 * -# Include utilities.h in your code
 *
 * @section ABBR ABBREVIATIONS:
 *   - CS - checksum
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
 * @defgroup checksum_grp Checksum calculation
 * @ingroup utilities_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
\*===========================================================================*/
#   include "reuse.h"

/*===========================================================================*\
 * Exported Preprocessor #define Constants
\*===========================================================================*/
/**
 *  Standard offset for checksum
 */
#   define CHECKSUM_OFFSET (0x5555)

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
#   ifdef __cplusplus
extern "C"
{

#   endif                       /* __cplusplus */
/**
 * Calculates checksum of given data block
 *
 * @return Calculated checksum
 *
 * @param [in] start_addr  pointer of begin address of data to be checksumed
 * @param [in] size        number of bytes to sum
 *
 * @pre
 *    - start_addr != NULL
 */
   uint16_t Chks_Sum_Bytes(const void *start_addr, size_t size);

/**
 * Calculates and updates 16-bit checksum for given memory block.
 *
 * @return 16-bit checksum
 *
 * @param [in] start_addr           pointer of begin address of data to be checksummed
 * @param [in] size                 number of bytes to sum
 * @param [in] checksum_offset      offset for checksum initialization
 * @param [in] checksum             pointer to checksum to be updated
 *
 * The function calculates and updates a 16 bit checksum.
 * Has capability to calculate a continuation of a previous call.
 * If 8 bit Checksum is only needed, then the MSB can simply be ignored.
 *
 * @note The function is not thread-safe.
 *
 * @pre
 *    - start_addr != NULL
 *    - checksum != NULL
 */
   void Chks_Update(const void *start_addr, size_t size, uint16_t checksum_offset, uint16_t * checksum);

/**
 * Calculates 16-bit checksum for given memory block and compares to input checksum
 *
 * @return true if calculated checksum match given checksum
 *
 * @param [in] start_addr        pointer of begin address of data to be checksummed
 * @param [in] size              number of bytes to sum
 * @param [in] checksum_offset   offset for checksum initialization
 * @param [in] checksum          checksum to be verified
 *
 * @pre
 *    - start_addr != NULL
 */
   bool_t Chks_Verify(const void *start_addr, size_t size, uint16_t checksum_offset, uint16_t checksum);

/**
 * The function writes data and adjust checksum for partial change.
 *
 * @param [in] dest       address of old data
 * @param [in] source     address of new data
 * @param [in] count      number of bytes changing
 * @param [in] checksum   address of checksum
 *
 * @return true if data changed 
 *
 * @note The function is not thread-safe.
 *
 * @pre
 *    - dest != NULL
 *    - source != NULL
 *    - checksum != NULL
 */
   bool_t Chks_Write(void *dest, const void *source, size_t count, uint16_t * checksum);

/*===========================================================================*\
 * Exported Inline Function Definitions and #define Function-Like Macros
\*===========================================================================*/

#   ifdef __cplusplus
}
#   endif                       /* __cplusplus */
/*===========================================================================*/
/*!
 * @file checksum.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Jul-2010 Larry Piekarski Rev 9
 *   - SCR kok_basa#2323 - Changed booleans to either bool_t or bool_fast
 *
 * - 21-apr-2010 Kirk Bailey Rev 8
 *   - Moved extern C block to only wrap function prototypes to match new
 *     template.
 *
 * - 27-May-2009 Larry Piekarski Rev 6-7
 *   - Cleaned up code from review checklist
 *
 * - 14-May-2009 Larry Piekarski Rev 5
 *   - Updated Doxygen Comments
 *
 * - 22-oct-2007 kirk bailey
 *    - Fixed Doxygen file directive.
 *
 * - 12-Oct-2007 Dan Carman
 *    - Changed Chks_Write to return bool_t if data modifided
 *    - Changed Chks_Verify to pass checksum by value
 *
 * - 06-Oct-2007 Bartosz Jamrozik
 *  - Added API functions declerations: Chks_Update, Chks_Verify, Chks_Write
 *
 * - 02-May-2007 Bartosz Jamrozik
 *  - Created initial file.
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* CHECKSUM_H */
