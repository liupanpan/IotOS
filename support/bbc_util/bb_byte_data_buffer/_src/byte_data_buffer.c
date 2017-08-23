/**
 * @file byte_data_buffer.c
 *
 * Implements byte data buffer routines
 *
 * %full_filespec: byte_data_buffer.c~kok_basa#25:csrc:kok_basa#2 %
 * @version %version: kok_basa#25 %
 * @author  %derived_by:gzt3xl %
 * @date    %date_modified: Thu Nov  6 09:10:10 2014 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007-2014 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * Byte Data Buffer reading and writing functions..
 *
 * @section ABBR ABBREVIATIONS:
 *   - None
 *
 * @section TRA TRACEABILITY INFO:
 *   - Design Document(s):
 *     - None
 *
 *   - Requirements Document(s):
 *     - None
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DESC DEVIATIONS FROM STANDARDS:
 *   - None
 *
 */
/*==========================================================================*/

/*===========================================================================*\
 * Header Files
 \*===========================================================================*/

#define DISABLE_DEPRECATED_SPRINTF /* Allow sprintf usage in this file */

#include "byte_data_buffer.h"
#include "pbc_trace.h"
#include "string_resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*===========================================================================*\
 * Local Preprocessor #define Constants
 \*===========================================================================*/
EM_FILENUM(UTILITIES_MODULE_ID, 3); /**< IDs file for EM assert handling */

/**
 * If a write operation would cause the buffer to overflow, then the buffer
 * shall be increased by this amount. Care must be taken with this value.
 * Increasing the buffer size is an expensive operation, so you don't want to
 * pick too small a value and have it happen more than needed. On the other hand,
 * you don't want to pick a huge value and have lots of unused space. If the
 * buffer is initially created with the correct size (big enough) then this
 * should be used.
 */
#   define ALLOCATION_BLOCK_SIZE (128)

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
static size_t Calculate_Block_Count(size_t new_buffer_length);
static void Check_Buffer(BDB_T * bdb, size_t new_length);

/*===========================================================================*\
 * Local Inline Function Definitions and Function-Like Macros
 \*===========================================================================*/

/*===========================================================================*\
 * Function Definitions
 \*===========================================================================*/

/**
 * Update Data length of buffer
 *    Data Length is last posistion written
 *
 * mBufPos indicates the next location to write. If this is beyond the currently
 *    used end of the buffer, extend the length of used area.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 */
static void Update_Data_Length(BDB_T * bdb)
{
   if (bdb->mBufPos > bdb->mBufDataLen)
   {
      bdb->mBufDataLen = bdb->mBufPos;
   }
}

/**
 * Calculates the number of blocks to allocate to provide the minimum space for
 * the requested buffer size.
 *
 * We use integer math to calculate the smallest number of blocks that satisfies
 * the request for buffer_length.
 *
 * We take advantage of the fact that in integer math,
 * truncation occurs.  We use this formula:
 *
 * (buffer_length + (ALLOCATION_BLOCK_SIZE - 1))/ ALLOCATION_BLOCK_SIZE
 *
 * Conceptually, the above formula can be re-written like this:
 *
 *      buffer_length       (ALLOCATION_BLOCK_SIZE - 1)
 *  --------------------- + ---------------------------
 *  ALLOCATION_BLOCK_SIZE      ALLOCATION_BLOCK_SIZE
 *
 * The key thing in our solution is the term on the right.  If
 * ALLOCATION_BLOCK_SIZE is 128, then the term on the right is
 * (127 / 128), which is around 0.9.
 *
 * The reason we add 0.9 is to "round up" after integer truncation.
 *
 * So, for example, if buffer_length is 1:
 *
 *  (  1 + (128 - 1)) / 128  ==> 1
 *
 * when buffer_length is 128:
 *
 *  (128 + (128 - 1)) / 128  ==> 1
 *
 * ...and finally when buffer_length is 129:
 *
 *  (129 + (128 - 1)) / 128  ==> 2
 *
 * @param [in] buffer_length
 *    - Desired length of the buffer in bytes
 * @return
 *    - Number of blocks to allocate
 */
static size_t Calculate_Block_Count(size_t buffer_length)
{
   return ((buffer_length + (ALLOCATION_BLOCK_SIZE - 1)) / ALLOCATION_BLOCK_SIZE);
}

/**
 * This functions checks the data buffer size and sees if the new data will
 * fit. If the new data is longer, then the buffer is reallocated to a buffer
 * that is equal to the new size plus a default increase amount. If the buffer
 * is to be increased, then a new buffer is allocated, the data copied from the
 * old buffer to the new one.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 * @param [in] min_buf_length
 *    - The length of the buffer, consisting of all the old
 *             buffer data + the new data to be added.
 */
static void Check_Buffer(BDB_T * bdb, size_t min_buf_length)
{
   if (bdb != NULL)
   {
      uint8_t * new_buffer;
      size_t num_blocks_to_allocate = 0;
      size_t new_buf_length = 0;

      PBC_Require(bdb->mBuffer != NULL,
         "Write operations cannot increase the size of a NULL buffer. Use BDB_Init to initialize the buffer first.");

      if (bdb->mBufLen < min_buf_length) /* allocate new buffer */
      {
         num_blocks_to_allocate = Calculate_Block_Count(min_buf_length);
         new_buf_length = num_blocks_to_allocate * ALLOCATION_BLOCK_SIZE;
         new_buffer = (uint8_t *) realloc(bdb->mBuffer, new_buf_length);

         PBC_Ensure(NULL != new_buffer, "Failed to resize the buffer.");

         if (NULL == new_buffer)
         {
            new_buf_length = 0;
         }

         bdb->mBuffer = new_buffer;
         bdb->mBufLen = new_buf_length;
      }
   }
}

/*
 * Initializes a byte data buffer and allocates min_buf_length bytes of memory.
 */
int32_t BDB_Init(BDB_T * bdb, size_t min_buf_length)
{
   int32_t ret = BDB_OK;

   PBC_Require(bdb != NULL, "BDB_Init cannot be called with a NULL bdb pointer");

   if (NULL == bdb)
   {
      ret = BDB_ERROR;
   }
   else
   {
      size_t num_blocks_to_allocate = 0;
      size_t buf_length = 0;

      PBC_Require(min_buf_length > 0, "BDB_Init cannot create a buffer of length 0");
      if (0 == min_buf_length)
      {
         min_buf_length = ALLOCATION_BLOCK_SIZE; /* Substitute default for zero. */
      }
      PBC_Require((min_buf_length <= UINT32_MAX - ALLOCATION_BLOCK_SIZE),
         "Buffer size is too large, must be less than UINT32_MAX - 128");
      if (min_buf_length > UINT32_MAX - ALLOCATION_BLOCK_SIZE)
      {
         min_buf_length = UINT32_MAX - ALLOCATION_BLOCK_SIZE;
      }

      num_blocks_to_allocate = Calculate_Block_Count(min_buf_length);
      buf_length = num_blocks_to_allocate * ALLOCATION_BLOCK_SIZE;

      bdb->mBuffer = NULL;

      bdb->mBuffer = (uint8_t *) malloc(buf_length);

      if (bdb->mBuffer != NULL)
      {
         bdb->mWritable = true;
         bdb->mBufLen = buf_length;
         BDB_Erase(bdb);
      }
      else
      {
         bdb->mWritable = false;
         ret = BDB_ERROR;
      }
   }
   return ret;
}

/*
 * Initializes a buffer using a passed in buffer.
 */
void BDB_Populate(BDB_T * bdb, void const * buffer, size_t buf_length)
{
   PBC_Require(bdb != NULL, "BDB_Populate cannot be called with a NULL bdb pointer");
   PBC_Require(buffer != NULL, "BDB_Populate cannot be called with a NULL buffer pointer");
   PBC_Require(buf_length > 0, "Creating a read-only BDB with a data length of 0 has no use.");

   if (NULL == buffer)
   {
      buf_length = 0;
   }

   if (bdb != NULL)
   {
      bdb->mBuffer = (void *) buffer; /* Buffer can only be logically const */
      bdb->mBufLen = buf_length;
      bdb->mBufDataLen = buf_length;
      bdb->mBufPos = 0;
      bdb->mWritable = false;
   }
}

/*
 * Releases any dynamic memory allocated by the buffer and resets the data structure.
 */
int32_t BDB_Destroy(BDB_T * bdb)
{
   int32_t ret = BDB_OK;

   PBC_Require(bdb != NULL, "BDB_Destroy cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mBuffer != NULL, "BDB_Destroy cannot free an already freed buffer.");

   if (bdb != NULL)
   {
      if (bdb->mWritable)
      {
         free(bdb->mBuffer);
      }
      memset(bdb, 0, sizeof(*bdb));
   }
   else
   {
      ret = BDB_ERROR;
   }
   return ret;
}

/*
 * Copies the data in buf to internal buffer in the Byte Data Buffer.
 */
void BDB_Copy_Buffer_Data(BDB_T * bdb, void const * buf, size_t buf_length)
{
   PBC_Require(bdb != NULL, "BDB_Copy_Buffer_Data cannot be called with a NULL bdb pointer");
   PBC_Require(buf != NULL, "BDB_Copy_Buffer_Data cannot be called with a NULL buf pointer");
   PBC_Require(bdb->mWritable, "BDB_Copy_Buffer_Data cannot copy data into a read only buffer created with BDB_Populate");

   Check_Buffer(bdb, buf_length);

   if ((buf != NULL) && (bdb->mBuffer != NULL))
   {
      memcpy(bdb->mBuffer, buf, buf_length);

      bdb->mBufPos = 0;
      bdb->mBufDataLen = buf_length;
   }
   else
   {
      bdb->mBufPos = 0;
      bdb->mBufDataLen = 0;
   }
}

/*
 * Writes the boolean to the buffer
 */
void BDB_Write_Bool(BDB_T * bdb, bool_t boolean_data)
{
   PBC_Require(bdb != NULL, "BDB_Write_Bool cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_Bool cannot write to a read-only buffer created using BDB_Populate");

   Check_Buffer(bdb, bdb->mBufPos + 1);

   if ((bdb != NULL) && (bdb->mBuffer != NULL) && bdb->mWritable)
   {
      bdb->mBuffer[bdb->mBufPos] = boolean_data;
      bdb->mBufPos++;
      Update_Data_Length(bdb);
   }
}

/*
 * Writes the bytes to the buffer
 */
void BDB_Write_Bytes(BDB_T * bdb, void const * data, size_t len)
{
   if (len > 0)
   {
      PBC_Require(bdb != NULL, "BDB_Write_Bytes cannot be called with a NULL bdb pointer");
      PBC_Require(data != NULL, "BDB_Write_Bytes cannot be called with a NULL bytes pointer");
      PBC_Require(bdb->mWritable, "BDB_Write_Bytes cannot write to a read-only buffer created using BDB_Populate");

      if ((bdb != NULL) && (data != NULL) && bdb->mWritable)
      {
         Check_Buffer(bdb, bdb->mBufPos + len);
         if (bdb->mBuffer != NULL)
         {
            memcpy(&(bdb->mBuffer[bdb->mBufPos]), data, len);
            bdb->mBufPos += len;
            Update_Data_Length(bdb);
         }
      }
   }
}

/*
 * Writes the characters in single bytes
 */
void BDB_Write_Chars(BDB_T * bdb, char const * chars, size_t len)
{
   if (len > 0)
   {
      PBC_Require(bdb != NULL, "BDB_Write_Chars cannot be called with a NULL bdb pointer");
      PBC_Require(chars != NULL, "BDB_Write_Chars cannot be called with a NULL chars pointer");
      PBC_Require(bdb->mWritable, "BDB_Write_Chars cannot write to a read-only buffer created using BDB_Populate");

      if ((bdb != NULL) && (chars != NULL) && bdb->mWritable)
      {
         Check_Buffer(bdb, bdb->mBufPos + len);
         if (bdb->mBuffer != NULL)
         {
            memcpy(&(bdb->mBuffer[bdb->mBufPos]), chars, len);
            bdb->mBufPos += len;
            Update_Data_Length(bdb);
         }
      }
   }
}

/*
 * Writes a full string to the buffer. It does this in two pieces.
 */
void BDB_Write_Full_String(BDB_T * bdb, char const * str, size_t max_len)
{
   PBC_Require(bdb != NULL, "BDB_Write_String cannot be called with a NULL bdb pointer");
   PBC_Require(str != NULL, "BDB_Write_String cannot be called with a NULL str pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_String cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      size_t length;

      if (NULL == str)
      {
         str = "";
      }
      length = Safe_Strlen(str, max_len);
      BDB_Write_U32(bdb, length); /* Write the length of the string, not including the NULL termination */
      BDB_Write_Chars(bdb, str, length); /* Write the string, without NULL termination */
      BDB_Write_Chars(bdb, "", 1); /* Write Null termination */
   }
}

/*
 * Writes a uint8_t to the buffer
 */
void BDB_Write_U8(BDB_T * bdb, uint8_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_U8 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_U8 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + 1);
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = data;
         bdb->mBufPos++;
         Update_Data_Length(bdb);
      }
   }
}

/*
 * Writes a int8_t to the buffer
 */
void BDB_Write_I8(BDB_T * bdb, int8_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_I8 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_I8 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + 1);
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = data;
         bdb->mBufPos++;
         Update_Data_Length(bdb);
      }
   }
}

/*
 * Writes a 16 bit unsigned integer to the buffer
 */
void BDB_Write_U16(BDB_T * bdb, uint16_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_U16 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_U16 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + sizeof(uint16_t));
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = (uint8_t) ((data & 0xff00) >> 8);
         bdb->mBuffer[bdb->mBufPos + 1] = (uint8_t) (data & 0xff);

         bdb->mBufPos += sizeof(uint16_t);
         Update_Data_Length(bdb);
      }
   }
}

/*
 * Writes a 16 bit signed integer to the buffer
 */
void BDB_Write_I16(BDB_T * bdb, int16_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_I16 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_I16 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + sizeof(int16_t));
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = (uint8_t) ((data & 0xff00) >> 8);
         bdb->mBuffer[bdb->mBufPos + 1] = (uint8_t) (data & 0xff);

         bdb->mBufPos += sizeof(int16_t);
         Update_Data_Length(bdb);
      }
   }
}

/*
 * Writes an 32 bit unsigned integer to the buffer
 */
void BDB_Write_U32(BDB_T * bdb, uint32_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_U32 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_U32 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + sizeof(uint32_t));
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = (uint8_t) ((data & 0xff000000) >> 24);
         bdb->mBuffer[bdb->mBufPos + 1] = (uint8_t) ((data & 0x00ff0000) >> 16);
         bdb->mBuffer[bdb->mBufPos + 2] = (uint8_t) ((data & 0x0000ff00) >> 8);
         bdb->mBuffer[bdb->mBufPos + 3] = (uint8_t) (data & 0x000000ff);

         bdb->mBufPos += sizeof(uint32_t);
         Update_Data_Length(bdb);
      }
   }
}
/*
 * Writes an 32 bit signed integer to the buffer
 */
void BDB_Write_I32(BDB_T * bdb, int32_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_I32 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_I32 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + sizeof(int32_t));
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = (uint8_t) ((data & 0xff000000) >> 24);
         bdb->mBuffer[bdb->mBufPos + 1] = (uint8_t) ((data & 0x00ff0000) >> 16);
         bdb->mBuffer[bdb->mBufPos + 2] = (uint8_t) ((data & 0x0000ff00) >> 8);
         bdb->mBuffer[bdb->mBufPos + 3] = (uint8_t) (data & 0x000000ff);

         bdb->mBufPos += sizeof(int32_t);
         Update_Data_Length(bdb);
      }
   }
}

#ifdef INT64_MIN
/*
 * Writes a 64 bit unsigned integer to the buffer
 */
void BDB_Write_U64(BDB_T * bdb, uint64_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_U64 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_U64 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + sizeof(uint64_t));
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = (uint8_t) ((data & 0xff00000000000000ULL) >> 56);
         bdb->mBuffer[bdb->mBufPos + 1] = (uint8_t) ((data & 0x00ff000000000000ULL) >> 48);
         bdb->mBuffer[bdb->mBufPos + 2] = (uint8_t) ((data & 0x0000ff0000000000ULL) >> 40);
         bdb->mBuffer[bdb->mBufPos + 3] = (uint8_t) ((data & 0x000000ff00000000ULL) >> 32);
         bdb->mBuffer[bdb->mBufPos + 4] = (uint8_t) ((data & 0x00000000ff000000ULL) >> 24);
         bdb->mBuffer[bdb->mBufPos + 5] = (uint8_t) ((data & 0x0000000000ff0000ULL) >> 16);
         bdb->mBuffer[bdb->mBufPos + 6] = (uint8_t) ((data & 0x000000000000ff00ULL) >> 8);
         bdb->mBuffer[bdb->mBufPos + 7] = (uint8_t) (data & 0x00000000000000ffULL);

         bdb->mBufPos += sizeof(uint64_t);
         Update_Data_Length(bdb);
      }
   }
}

/*
 * Writes a 64 bit signed integer to the buffer
 */
void BDB_Write_I64(BDB_T * bdb, int64_t data)
{
   PBC_Require(bdb != NULL, "BDB_Write_I64 cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Write_I64 cannot write to a read-only buffer created using BDB_Populate");

   if ((bdb != NULL) && bdb->mWritable)
   {
      Check_Buffer(bdb, bdb->mBufPos + sizeof(int64_t));
      if (bdb->mBuffer != NULL)
      {
         bdb->mBuffer[bdb->mBufPos] = (uint8_t) ((data & 0xff00000000000000ULL) >> 56);
         bdb->mBuffer[bdb->mBufPos + 1] = (uint8_t) ((data & 0x00ff000000000000ULL) >> 48);
         bdb->mBuffer[bdb->mBufPos + 2] = (uint8_t) ((data & 0x0000ff0000000000ULL) >> 40);
         bdb->mBuffer[bdb->mBufPos + 3] = (uint8_t) ((data & 0x000000ff00000000ULL) >> 32);
         bdb->mBuffer[bdb->mBufPos + 4] = (uint8_t) ((data & 0x00000000ff000000ULL) >> 24);
         bdb->mBuffer[bdb->mBufPos + 5] = (uint8_t) ((data & 0x0000000000ff0000ULL) >> 16);
         bdb->mBuffer[bdb->mBufPos + 6] = (uint8_t) ((data & 0x000000000000ff00ULL) >> 8);
         bdb->mBuffer[bdb->mBufPos + 7] = (uint8_t) (data & 0x00000000000000ffULL);

         bdb->mBufPos += sizeof(int64_t);
         Update_Data_Length(bdb);
      }
   }
}

#endif

/*
 * Reads a single boolean.
 */
bool_t BDB_Read_Bool(BDB_T * bdb)
{
   bool_t ret = false;

   PBC_Require(bdb != NULL, "BDB_Read_Bool cannot be called with a NULL bdb pointer");
   PBC_Require_3((bdb->mBufPos + 1) <= bdb->mBufDataLen,
      "BDB_Read_Bool results in a read of 1 byte past the end of the buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && (bdb->mBuffer != NULL) && ((bdb->mBufPos + 1) <= bdb->mBufDataLen))
   {
      ret = bdb->mBuffer[bdb->mBufPos++];
   }

   return ret;
}

/*
 * Reads a number of bytes from a BDB into a buffer
 */
void BDB_Read_Bytes(BDB_T * bdb, void * destination, size_t len)
{
   if (len > 0)
   {
      PBC_Require(bdb != NULL, "BDB_Read_Bytes cannot be called with a NULL bdb pointer");
      PBC_Require(destination != NULL, "BDB_Read_Bytes cannot be called with a NULL destination pointer");

      if ((bdb != NULL) && (bdb->mBuffer != NULL))
      {
         if (!((bdb->mBufPos + len) <= bdb->mBufDataLen)) /* This conditional is the same as what would be used in a PBC_Require call without the ! */
         {
            /* There is no PBC_Require_5*/
            Tr_Fault_2("BDB_Read_Bytes of %d bytes results in a read of %d bytes past end of buffer.", (int )len,
               (int )((bdb->mBufPos + len) - bdb->mBufDataLen));
            PBC_Failed_3("Data length: %d. Current position: %d, Buffer Size: %d", (int )bdb->mBufDataLen, (int )bdb->mBufPos,
               (int )bdb->mBufLen);
         }
         else
         {
            if (destination != NULL)
            {
               memcpy(destination, &(bdb->mBuffer[bdb->mBufPos]), len);
            }
            bdb->mBufPos += len;
         }
      }
   }
}

/*
 * Provides a pointer to a byte buffer of the specified size. This pointer is of
 * type void requiring it to be cast to a certain type before usage.
 */
void const * BDB_Read_P_Bytes(BDB_T * bdb, size_t len)
{
   void * data_ptr = NULL;

   if (len > 0)
   {
      PBC_Require(bdb != NULL, "BDB_Read_P_Bytes cannot be called with a NULL bdb pointer");

      if ((bdb != NULL) && (bdb->mBuffer != NULL))
      {
         /* There is no PBC_Require_5*/
         if (!((bdb->mBufPos + len) <= bdb->mBufDataLen)) /* This conditional is the same as what would be used in a PBC_Require call without the ! */
         {
            Tr_Fault_2(
               "BDB_Read_P_Bytes attempt to create a pointer to a byte array of size %d that runs %d byte(s) past the end of the buffer.",
               (int )len, (int )((bdb->mBufPos + len) - bdb->mBufDataLen));
            PBC_Failed_3("Data length: %d. Current position: %d, Buffer Size: %d", (int )bdb->mBufDataLen, (int )bdb->mBufPos,
               (int )bdb->mBufLen);
         }
         else
         {
            /* Get the pointer to the current Buffer Position */
            data_ptr = (void *) &(bdb->mBuffer[bdb->mBufPos]);

            /* Advance the Buffer Position */
            bdb->mBufPos += len;
         }
      }
   }

   return data_ptr;
}

/*
 * Read a number characters into a byte buffer.
 */
void BDB_Read_Chars(BDB_T * bdb, char * destination, size_t len)
{
   if (len > 0)
   {
      PBC_Require(bdb != NULL, "BDB_Read_Chars cannot be called with a NULL bdb pointer");
      PBC_Require(destination != NULL, "BDB_Read_Chars cannot be called with a NULL destination pointer");

      if ((bdb != NULL) && (bdb->mBuffer != NULL))
      {
         /* There is no PBC_Require_5*/
         if (!((bdb->mBufPos + len) <= bdb->mBufDataLen)) /* This conditional is the same as what would be used in a PBC_Require call without the ! */
         {
            Tr_Fault_2("BDB_Read_Chars of %d characters results in a read of %d bytes past end of buffer.", (int )len,
               (int )((bdb->mBufPos + len) - bdb->mBufDataLen));
            PBC_Failed_3("Data length: %d. Current position: %d, Buffer Size: %d", (int )bdb->mBufDataLen, (int )bdb->mBufPos,
               (int )bdb->mBufLen);
         }
         else
         {
            if (destination != NULL)
            {
               memcpy(destination, &(bdb->mBuffer[bdb->mBufPos]), len);
            }
            bdb->mBufPos += len;
         }
      }
   }
}

/*
 * Read the string in str array.
 */
void BDB_Read_String(BDB_T * bdb, char * destination, size_t max_len)
{
   uint32_t len;

   PBC_Require(bdb != NULL, "BDB_Read_String cannot be called with a NULL bdb pointer");
   PBC_Require(destination != NULL, "BDB_Read_String cannot be called with a NULL destination pointer");

   if (bdb != NULL)
   {
      len = BDB_Read_U32(bdb); /* Get the length of the string not including the NULL termination */

      if (0 == max_len)
      {
         bdb->mBufPos += len + 1; /* No room to copy anything - skip string */
      }
      else if (len >= max_len) /* Have to truncate string, but some room */
      {
         BDB_Read_Chars(bdb, destination, max_len - 1);
         destination[max_len - 1] = 0;
         bdb->mBufPos += (len - (max_len - 1) + 1); /* Move the Buf Pos to the end of the string */
      }
      else
      {
         BDB_Read_Chars(bdb, destination, len + 1);
      }
   }
}

/*
 * Returns a pointer to the string in the message.
 */
char const * BDB_Read_P_String(BDB_T * bdb)
{
   int len;
   char *str = NULL;

   PBC_Require(bdb != NULL, "BDB_Read_P_String cannot be called with a NULL bdb pointer");

   if (bdb != NULL)
   {
      len = BDB_Read_U32(bdb); /* does not include NULL */
      /* There is no PBC_Require_5*/
      if (!((bdb->mBufPos + len) <= bdb->mBufDataLen)) /* This conditional is the same as what would be used in a PBC_Require call without the ! */
      {
         Tr_Fault_2("BDB_Read_P_String with length %d results in a read of %d bytes past end of buffer.", (int )len,
            (int )((bdb->mBufPos + len) - bdb->mBufDataLen));
         PBC_Failed_3("Data length: %d. Current position: %d, Buffer Size: %d", (int )bdb->mBufDataLen, (int )bdb->mBufPos,
            (int )bdb->mBufLen);
      }
      else
      {
         str = (char *) &bdb->mBuffer[bdb->mBufPos];
         bdb->mBufPos += len + 1; /* string length + NULL */
      }
   }

   return (str);
}

/*
 * Alloc memory for, and read the string into the allocation.
 */
char * BDB_Read_Alloc_String(BDB_T * bdb)
{
   char *str = NULL;

   PBC_Require(bdb != NULL, "BDB_Read_Alloc_String cannot be called with a NULL bdb pointer");

   if (bdb != NULL)
   {
      uint32_t len = BDB_Read_U32(bdb) + 1;

      str = malloc(len);

      PBC_Require(str != NULL, "Failed to allocate string");

      if (str != NULL)
      {
         BDB_Read_Chars(bdb, str, len);
      }
   }

   return str;
}

/*
 * Reads an 8 bit unsigned integer from the buffer
 */
uint8_t BDB_Read_U8(BDB_T * bdb)
{
   uint8_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Read_U8 cannot be called with a NULL bdb pointer");
   PBC_Require_3((bdb->mBufPos + sizeof(uint8_t)) <= bdb->mBufDataLen,
      "Read of uint8_t results in a read of 1 byte past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(uint8_t)) <= bdb->mBufDataLen))
   {
      return_value = bdb->mBuffer[bdb->mBufPos++];
   }
   return return_value;
}
/*
 * Reads an 8 bit signed integer from the buffer.
 */
int8_t BDB_Read_I8(BDB_T * bdb)
{
   int8_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Read_I8 cannot be called with a NULL bdb pointer");
   PBC_Require_3((bdb->mBufPos + sizeof(int8_t)) <= bdb->mBufDataLen,
      "Read of int8_t results in a read of 1 byte past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(int8_t)) <= bdb->mBufDataLen))
   {
      return_value = bdb->mBuffer[bdb->mBufPos++];
   }
   return return_value;
}

/*
 * Reads a 16 bit unsigned integer from a buffer
 */
uint16_t BDB_Read_U16(BDB_T * bdb)
{
   uint16_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Read_U16 cannot be called with a NULL bdb pointer");
   PBC_Require_4((bdb->mBufPos + sizeof(uint16_t)) <= bdb->mBufDataLen,
      "Read of uint16_t results in a read of %d byte(s) past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )((bdb->mBufPos + sizeof(uint16_t)) - bdb->mBufDataLen), (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(uint16_t)) <= bdb->mBufDataLen))
   {
      return_value = (uint16_t) ((uint16_t) ((bdb->mBuffer[bdb->mBufPos] << 8) & 0xff00)
         | (uint16_t) ((bdb->mBuffer[bdb->mBufPos + 1]) & 0x00ff));
      bdb->mBufPos += sizeof(uint16_t);
   }
   return return_value;
}

/*
 * Reads a 16 bit signed integer from a buffer
 */
int16_t BDB_Read_I16(BDB_T * bdb)
{
   int16_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Read_I16 cannot be called with a NULL bdb pointer");
   PBC_Require_4((bdb->mBufPos + sizeof(int16_t)) <= bdb->mBufDataLen,
      "Read of int16_t results in a read of %d byte(s) past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )((bdb->mBufPos + sizeof(int16_t)) - bdb->mBufDataLen), (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(int16_t)) <= bdb->mBufDataLen))
   {
      return_value = (int16_t) ((int16_t) ((bdb->mBuffer[bdb->mBufPos] << 8) & 0xff00)
         | (int16_t) ((bdb->mBuffer[bdb->mBufPos + 1]) & 0x00ff));
      bdb->mBufPos += sizeof(int16_t);
   }
   return return_value;
}

/*
 * Reads a 32 bit unsigned integer from a buffer.
 */
uint32_t BDB_Read_U32(BDB_T * bdb)
{
   uint32_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Read_I32 cannot be called with a NULL bdb pointer");
   PBC_Require_4((bdb->mBufPos + sizeof(uint32_t)) <= bdb->mBufDataLen,
      "Read of uint32_t results in a read of %d byte(s) past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )((bdb->mBufPos + sizeof(uint32_t)) - bdb->mBufDataLen), (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(uint32_t)) <= bdb->mBufDataLen))
   {
      return_value = (uint32_t) ((uint32_t) ((bdb->mBuffer[bdb->mBufPos] << 24) & 0xff000000)
         | (uint32_t) ((bdb->mBuffer[bdb->mBufPos + 1] << 16) & 0x00ff0000)
         | (uint32_t) ((bdb->mBuffer[bdb->mBufPos + 2] << 8) & 0x0000ff00)
         | (uint32_t) ((bdb->mBuffer[bdb->mBufPos + 3]) & 0x000000ff));

      bdb->mBufPos += sizeof(uint32_t);
   }
   return return_value;
}

/*
 * Reads a 32 bit signed integer from a buffer.
 */
int32_t BDB_Read_I32(BDB_T * bdb)
{
   int32_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Read_I32 cannot be called with a NULL bdb pointer");
   PBC_Require_4((bdb->mBufPos + sizeof(int32_t)) <= bdb->mBufDataLen,
      "Read of int32_t results in a read of %d byte(s) past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )((bdb->mBufPos + sizeof(int32_t)) - bdb->mBufDataLen), (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(int32_t)) <= bdb->mBufDataLen))
   {
      return_value = (int32_t) ((int32_t) ((bdb->mBuffer[bdb->mBufPos] << 24) & 0xff000000)
         | (int32_t) ((bdb->mBuffer[bdb->mBufPos + 1] << 16) & 0x00ff0000)
         | (int32_t) ((bdb->mBuffer[bdb->mBufPos + 2] << 8) & 0x0000ff00)
         | (int32_t) ((bdb->mBuffer[bdb->mBufPos + 3]) & 0x000000ff));

      bdb->mBufPos += sizeof(int32_t);
   }
   return return_value;
}

#ifdef INT64_MIN
/*
 * Reads a 64 bit unsigned integer from a buffer.
 */
uint64_t BDB_Read_U64(BDB_T * bdb)
{
   uint64_t return_value = 0;
   uint8_t i;

   PBC_Require(bdb != NULL, "BDB_Read_U64 cannot be called with a NULL bdb pointer");
   PBC_Require_4((bdb->mBufPos + sizeof(uint64_t)) <= bdb->mBufDataLen,
      "Read of uint64_t results in a read of %d byte(s) past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )((bdb->mBufPos + sizeof(uint64_t)) - bdb->mBufDataLen), (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(uint64_t)) <= bdb->mBufDataLen))
   {
      for (i = 0; i < sizeof(uint64_t); i++)
      {
         return_value <<= 8;
         return_value |= (uint64_t) ((uint8_t) bdb->mBuffer[bdb->mBufPos + i]);
      }

      bdb->mBufPos += sizeof(uint64_t);
   }
   return return_value;
}

/*
 * Reads a 64 bit signed integer from a buffer.
 */
int64_t BDB_Read_I64(BDB_T * bdb)
{
   int64_t return_value = 0;
   uint8_t i;

   PBC_Require(bdb != NULL, "BDB_Read_I64 cannot be called with a NULL bdb pointer");
   PBC_Require_4((bdb->mBufPos + sizeof(int64_t)) <= bdb->mBufDataLen,
      "Read of int64_t results in a read of %d byte(s) past end of buffer. Data length: %d. Current position: %d, Buffer Size: %d",
      (int )((bdb->mBufPos + sizeof(int64_t)) - bdb->mBufDataLen), (int )bdb->mBufDataLen, (int )bdb->mBufPos, (int )bdb->mBufLen);

   if ((bdb != NULL) && ((bdb->mBufPos + sizeof(int64_t)) <= bdb->mBufDataLen))
   {
      for (i = 0; i < sizeof(int64_t); i++)
      {
         return_value <<= 8;
         return_value |= (int64_t) ((uint8_t) bdb->mBuffer[bdb->mBufPos + i]);
      }

      bdb->mBufPos += sizeof(int64_t);
   }
   return return_value;
}
#endif

/*
 * Returns a pointer to the data buffer.
 */
void const * BDB_Get_Buffer(BDB_T const * bdb)
{
   void const *return_value = NULL;

   PBC_Require(bdb != NULL, "BDB_Get_Buffer cannot be called with a NULL bdb pointer");
   if (bdb != NULL)
   {
      return_value = bdb->mBuffer;
   }
   return return_value;
}

/*
 * Returns the length of the data stored in the buffer.
 */
size_t BDB_Get_Data_Length(BDB_T const * bdb)
{
   size_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Get_Data_Length cannot be called with a NULL bdb pointer");
   if (bdb != NULL)
   {
      return_value = bdb->mBufDataLen;
   }
   return return_value;
}

/*
 * If min_buf_length is greater than current buffer length then buffer length
 * is increased to min_buf_length.
 */
void BDB_Guarantee_Min_Buffer_Length(BDB_T * bdb, size_t min_buf_length)
{
   PBC_Require(bdb != NULL, "BDB_Guarantee_Min_Buffer_Length cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Guarantee_Min_Buffer_Length cannot resize a read-only buffer created using BDB_Populate");
   PBC_Require(min_buf_length > 0, "BDB_Guarantee_Min_Buffer_Length cannot resize a buffer to length 0");

   Check_Buffer(bdb, min_buf_length);
}

/*
 *  Resets the (read/write) buffer position to 'seek_pos'.
 */
void BDB_Seek(BDB_T * bdb, size_t seek_pos)
{
   PBC_Require(bdb != NULL, "BDB_Seek cannot be called with a NULL bdb pointer");
   PBC_Require_2(seek_pos <= bdb->mBufDataLen, "BDB_Seek command must position cursor within current data: DataLen=%u, SeekPos=%u",
      (unsigned )bdb->mBufDataLen, (unsigned )seek_pos);

   if ((bdb != NULL) && (seek_pos <= bdb->mBufDataLen))
   {
      bdb->mBufPos = seek_pos;
   }
}

/*
 * Returns buffer position.
 */
size_t BDB_Tell(BDB_T const * bdb)
{
   size_t return_value = 0;

   PBC_Require(bdb != NULL, "BDB_Tell cannot be called with a NULL bdb pointer");
   if (bdb != NULL)
   {
      return_value = bdb->mBufPos;
   }
   return return_value;
}

/*
 *  Resets the (read/write) buffer position to zero.
 */
void BDB_Rewind(BDB_T * bdb)
{
   PBC_Require(bdb != NULL, "BDB_Rewind cannot be called with a NULL bdb pointer");
   if (bdb != NULL)
   {
      bdb->mBufPos = 0;
   }
}

/*
 * Clears the buffer and set the buffer position to 0.
 */
void BDB_Erase(BDB_T * bdb)
{
   PBC_Require(bdb != NULL, "BDB_Erase cannot be called with a NULL bdb pointer");
   PBC_Require(bdb->mWritable, "BDB_Erase cannot erase the data from a read-only buffer.");
   if ((bdb != NULL) && (bdb->mWritable))
   {
      bdb->mBufPos = 0;
      bdb->mBufDataLen = 0;
   }
}

/*
 * Returns true if the buffer contains data.
 */
bool_t BDB_Good(BDB_T const * bdb)
{
   bool_t return_value = false;

   PBC_Require(bdb != NULL, "BDB_Good cannot be called with a NULL bdb pointer");
   if ((bdb != NULL) && (bdb->mBuffer != NULL) && (bdb->mBufDataLen != 0))
   {
      return_value = true;
   }
   return return_value;
}

/*
 * Dump the entire bdb structure to STDOUT using printf.
 */
void BDB_Print(BDB_T const * bdb)
{
   PBC_Require(bdb != NULL, "BDB_Print cannot be called with a NULL bdb pointer");

   if (bdb != NULL)
   {
      size_t i;

      printf("ByteDataBuffer::print: Buffer position: %d, Buffer total length:"
         " %d, Buffer data length: %d", (int) bdb->mBufPos, (int) bdb->mBufLen, (int) bdb->mBufDataLen);
      if (bdb->mBuffer != NULL)
      {
         printf("\n Buffer data: ");
         for (i = 0; i < bdb->mBufDataLen; i++)
         {
            printf("%02X ", (unsigned) bdb->mBuffer[i]);
         }
      }
      printf("\n");
   }
}

/*
 * Writes the entire contents of the buffer to a string in a human readable format.
 */
void BDB_Snprint(char * str, uint32_t str_len, BDB_T const * bdb)
{
   PBC_Require(str != NULL, "BDB_Snprint cannot be called with a NULL destination pointer");
   PBC_Require(bdb != NULL, "BDB_Snprint cannot be called with a NULL bdb pointer");

   if ((str != NULL) && (bdb != NULL) && (str_len > 0))
   {
      char hex_string[4];
      uint32_t byte_num = 0;
      uint32_t characters_printed = 0;

      str[0] = '\0'; /* Erase any existing string data */

      /* Must be able to place 3 more characters in the string to continue and a '\0' at the end*/
      while ((characters_printed + 3 <= str_len - 1) && (byte_num < bdb->mBufDataLen))
      {
         sprintf(hex_string, " %02X", (unsigned) bdb->mBuffer[byte_num]);
         strncat(str, hex_string, 3);
         byte_num++;
         characters_printed += 3;
         str[characters_printed] = '\0';
      }
   }
}
/*===========================================================================*/
/*!
 * @file byte_data_buffer.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 08-Jul-2013 Emmanuel Aguilasocho Favela Rev 20
 *   - Created new "BDB_Write_Full_String" function to write a safe string in to the buffer.
 *
 * - 17-Jul-2013 Larry Piekarski Rev 21
 *   - Task 160933: Removed depricated function BDB_Write_String. Use 
 *      BDB_Write_Full_String instead. 
 *
 * - 23-Jul-2013 Larry Piekarski Rev 22
 *   - Task 161654: Updated to new include files.
 *
 * - 20-Sep-2013 Kirk Bailey Rev 23
 *   - Task 1164: Fixed warnings due to enabling of -Wextra
 *
 * - 31-Jul-2014 Kirk Bailey Rev 24
 *   - Eliminated history prior to 2013
 *   - Ran Eclipse code formatter
 *   - Fixed warnings in 64-bit Linux build
 *
 * - 06-Nov-2014 Paul Casto Rev 25
 *   - Fix buffer overflow while writing NULL character in "BDB_Write_Full_String()" function.
 */
/*===========================================================================*/
