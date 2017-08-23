/*===========================================================================*/
/**
 * @file interpolate.c
 *
 * Interpolation Routines
 *
 * %full_filespec:interpolate.c~kok_inf#9:csrc:tck_aud#1 %
 * @version %version:kok_inf#9 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Fri Sep 20 14:02:03 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *   The functions compute the y value corresponding to the x value
 *   on a line between (x[i-1]/y[i-1]) and (x[i]/y[i])
 *
 * @section ABBR ABBREVIATIONS:
 *
 * @section TRACE TRACEABILITY INFO:
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
#include "interpolate.h"
#include "pbc_trace.h"

/*===========================================================================*\
 * Local Preprocessor #define Constants
\*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 5); /**< IDs file for EM assert handling */

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

/*
 * The function computes the sigend y value corresponding to the signed x value
 */
int16_t Interpolate_I16(int16_t x, int16_t n_points, const int16_t x_table[], const int16_t y_table[])
{
   int16_t i = -1;
   int16_t y;

   PBC_Require(n_points >= 2, "Interpolate_I16 must be called with at least 2 points in the table");

   /* increment index while x > x[n] and x[n_points-1] (last x) not yet read */
   do
   {
      ++i;
   }
   while ((x_table[i] < x) && (i < n_points));

   if (n_points > i)            /* x is not larger than max. x_table entry? */
   {
      if (0 == i)               /* x is smaller or equal to first x_table entry? */

      {
         y = y_table[0];        /* y take first y_table entry */
      }
      else
      {
         y = (int16_t) (((y_table[i] - y_table[i - 1]) * (x - x_table[i - 1])) / (x_table[i] - x_table[i - 1]) +
                        y_table[i - 1]);
      }
   }
   else                         /* x is larger than max. x_table entry take last y_table entry */
   {
      y = y_table[n_points - 1];
   }
   return (y);
}


/*
 * The function computes the unsigned y value corresponding to the unsigned x value
 */
uint16_t Interpolate_U16(uint16_t x, uint16_t n_points, const uint16_t x_table[], const uint16_t y_table[])
{
   int16_t i = -1;
   uint16_t y;

   PBC_Require(n_points >= 2, "Interpolate_U16 must be called with at least 2 points in the table");

   /* increment index while x > x[n] and x[n_points-1] (last x) not yet read */
   do
   {
      ++i;
   }
   while ((x_table[i] < x) && (i < n_points));

   if (n_points > i)            /* x is not larger than max. x_table entry? */
   {
      if (0 == i)               /* x is smaller or equal to first x_table entry? */

      {
         y = y_table[0];        /* y take first y_table entry */
      }
      else
      {
         y = (uint16_t) (((y_table[i] - y_table[i - 1]) * (x - x_table[i - 1])) / (x_table[i] - x_table[i - 1]) +
                         y_table[i - 1]);
      }
   }
   else                         /* x is larger than max. x_table entry take last y_table entry */
   {
      y = y_table[n_points - 1];
   }
   return (y);
}


/*
 * The function computes the sigend y value corresponding to the signed x value
 */
int16_t Interpolate_I8_I16(int8_t x, int16_t n_points, const int8_t x_table[], const int16_t y_table[])
{
   int16_t i = -1;
   int16_t y;

   PBC_Require(n_points >= 2, "Interpolate_I8_I16 must be called with at least 2 points in the table");

   /* increment index while x > x[n] and x[n_points-1] (last x) not yet read */
   do
   {
      ++i;
   }
   while ((x_table[i] < x) && (i < n_points));

   if (n_points > i)            /* x is not larger than max. x_table entry? */
   {
      if (0 == i)               /* x is smaller or equal to first x_table entry? */

      {
         y = y_table[0];        /* y take first y_table entry */
      }
      else
      {
         y = (int16_t) (((y_table[i] - y_table[i - 1]) * (x - x_table[i - 1])) / (x_table[i] - x_table[i - 1]) +
                        y_table[i - 1]);
      }
   }
   else                         /* x is larger than max. x_table entry take last y_table entry */
   {
      y = y_table[n_points - 1];
   }
   return (y);
}


/*
 * The function computes the unsigned y value corresponding to the unsigned x value
 */
uint16_t Interpolate_U8_U16(uint8_t x, uint16_t n_points, const uint8_t x_table[], const uint16_t y_table[])
{
   int16_t i = -1;
   uint16_t y;

   PBC_Require(n_points >= 2, "Interpolate_U8_U16 must be called with at least 2 points in the table");

   /* increment index while x > x[n] and x[n_points-1] (last x) not yet read */
   do
   {
      ++i;
   }
   while ((x_table[i] < x) && (i < n_points));

   if (n_points > i)            /* x is not larger than max. x_table entry? */
   {
      if (0 == i)               /* x is smaller or equal to first x_table entry? */

      {
         y = y_table[0];        /* y take first y_table entry */
      }
      else
      {
         y = (uint16_t) (((y_table[i] - y_table[i - 1]) * (x - x_table[i - 1])) / (x_table[i] - x_table[i - 1]) +
                         y_table[i - 1]);
      }
   }
   else                         /* x is larger than max. x_table entry take last y_table entry */
   {
      y = y_table[n_points - 1];
   }
   return (y);
}


/*
 * The function computes the sigend y value corresponding to the signed x value
 */
int8_t Interpolate_I8(int8_t x, size_t n_points, const int8_t x_table[], const int8_t y_table[])
{
   int8_t i = -1;
   int8_t y;

   PBC_Require(n_points >= 2, "Interpolate_I8 must be called with at least 2 points in the table");

   /* increment index while x > x[n] and x[n_points-1] (last x) not yet read */
   do
   {
      ++i;
   }
   while ((x_table[i] < x) && ((size_t)i < n_points));

   if (n_points > (size_t)i)            /* x is not larger than max. x_table entry? */
   {
      if (0 == i)               /* x is smaller or equal to first x_table entry? */

      {
         y = y_table[0];        /* y take first y_table entry */
      }
      else
      {
         y = (int8_t) (((y_table[i] - y_table[i - 1]) * (x - x_table[i - 1])) / (x_table[i] - x_table[i - 1]) +
                        y_table[i - 1]);
      }
   }
   else                         /* x is larger than max. x_table entry take last y_table entry */
   {
      y = y_table[n_points - 1];
   }
   return (y);
}


/*
 * The function computes the unsigned y value corresponding to the unsigned x value
 */
uint8_t Interpolate_U8(uint8_t x, size_t n_points, const uint8_t x_table[], const uint8_t y_table[])
{
   int8_t i = -1;
   uint8_t y;

   PBC_Require(n_points >= 2, "Interpolate_U8 must be called with at least 2 points in the table");

   /* increment index while x > x[n] and x[n_points-1] (last x) not yet read */
   do
   {
      ++i;
   }
   while ((x_table[i] < x) && ((size_t)i < n_points));

   if (n_points > (size_t)i)            /* x is not larger than max. x_table entry? */
   {
      if (0 == i)               /* x is smaller or equal to first x_table entry? */

      {
         y = y_table[0];        /* y take first y_table entry */
      }
      else
      {
         y = (uint8_t) (((y_table[i] - y_table[i - 1]) * (x - x_table[i - 1])) / (x_table[i] - x_table[i - 1]) +
                         y_table[i - 1]);
      }
   }
   else                         /* x is larger than max. x_table entry take last y_table entry */
   {
      y = y_table[n_points - 1];
   }
   return (y);
}

/*===========================================================================*/
/*!
 * @file interpolate.c
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 20-Sep-2013 Kirk Bailey Rev 9
 *   - Task 1164: Fixed warnings due to enabling of -Wextra
 *
 * - 10-Jul-2012 Kevin Werner
 *   Task kok_basa#107801 Revision 7-8
 *   - Added functions for 8 bit to 16 bit interpolation.
 * 
 * - 28-Aug-2009 Kris Boultbee
 *   Task kok_aud#56145 Revision 6
 *   - Added functions for 8 bit interpolation.
 * 
 * - 27-May-2009 Larry Piekarski Rev 4-5
 *   - Cleaned up code from review checklist
 *
 * - 08-Oct-2007 Bartosz Jamrozik
 *   - Added interpolate function for unsigned values
 *
 * - 19-Jun-2007 Bartosz Jamrozik
 *   - Fixed variable types to conform with coding standard.
 *
 * - 22-May-2007 Bartosz Jamrozik
 *   - Created initial file.
 *
\*===========================================================================*/
