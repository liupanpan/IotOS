#ifndef BYTE_DATA_BUFFER_H
#   define BYTE_DATA_BUFFER_H
/*===========================================================================*/
/**
 * @file byte_data_buffer.h
 *
 * API for functions to create, write to, and read from a Byte Data Buffer
 *
 * %full_filespec: byte_data_buffer.h~1:incl:kok_basa#3 %
 * @version %version: 1 %
 * @author  %derived_by: czhswm %
 * @date    %date_modified: Tue Jul 16 16:35:03 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2007 - 2012 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @page bdb_lite_page Byte Data Buffer User Manual
 *
 * @section DESC DESCRIPTION:
 *
 * This module contains functions to create, destroy, write to, and read from a
 * Byte Data Buffer. A BDB is a data buffer that is essentially a dynamically
 * defined data structure. The intent of BDB is to serialize various types of
 * data into a byte array. This byte array can then be attached to an OS
 * message.
 *
 * One of the nice features of a Byte Data Buffer is the ability to resize as needed
 * when writing to the buffer. In general when using any of the BDB_Write_* functions
 * there is no need to manual resize the buffer as each of these writes will ensure
 * that there is enough space for the data being written. When a resize occurs, the buffer will
 * be resized to the next multiple of the block size. This block size is currently 128 bytes.
 * Similarly, when creating a writable Byte Data Buffer using BDB_Init(), the initial size will
 * be the next larger multiple of the block size. This is done to help minimize memory allocations
 * and fragmentation.
 *
 * @section USAGE USAGE INSTRUCTIONS:
 *
 * A common use (though certainly not the only) of the Byte Data Buffer module is to
 * serialize and deserialize data for transmission in an XSAL message.
 *
 * @subsection transmit Building a Transmit message
 *
 *    To build a transmit message, an empty buffer must be created. The typical way way to
 * do this is to use BDB_Init() to create the initial storage for the message that needs to be built.
 * Once the storage is created the data can be written into the buffer using functions like BDB_Write_U8()
 * and BDB_Write_String().
 *
 *    For instance, if you need to transmit a message that contains station number and a station name
 * would would execute the following code snippet:
 * @code
 * BDB_T bdb;
 *
 * Comment:       Station number     String Length     Station Name
 * BDB_Init(&bdb, sizeof(uint32_t) + sizeof(uint32_t)+ strlen(station_name));
 *
 * BDB_Write_U32(&bdb, station_number);
 *
 * BDB_Write_String(&bdb, station_name);
 *
 * Comment: Send your message.
 *
 * SAL_Send(APP_ID, THREAD_ID, EVENT_ID, BDB_Get_Buffer(&bdb), BDB_Get_Data_Length(&bdb);
 *
 * BDB_Free(&bdb);
 *
 * @endcode
 *
 * Of particular note is what is being sent in the XSAL message in the example above. You cannot
 * send the BDB itself. The BDB is used to create the serialized data that is to be sent. To obtain
 * this serialized data, use the function BDB_Get_Buffer which returns the pointer to the internal
 * data buffer.
 *
 * @subsection receive_message Receiving a message
 * The receiver of this message will then need to deserialize the message. This will require
 * the sender to know the format of the message. This restriction is currently unavoidable.
 *
 * To deserialize the message that was serialized above, the receiver must create
 * a BDB. This time however, the data buffer already exists so we need to use it instead of creating
 * a new data buffer. Then the data is read from the Byte Data Buffer in same order it was placed into
 * BDB.
 *
 * For example:
 *
 * @code
 * BDB_T bdb;
 *
 * BDB_Populate(&bdb, &received_message_buffer, received_buffer_length);
 *
 * station_number = BDB_Read_U32(&bdb);
 *
 * BDB_Read_String(&bdb, station_name, station_name_length);
 *
 * @endcode
 *
 * There are several ways to actually read the data from a Byte Data Buffer. The
 * proper function will depend on your needs.
 *
 * @subsection recieving_bdb_with_data Receiving a Byte Data Buffer
 * There is another way to receive serialized data that is common. That is to receive
 * a fully populated BDB as a parameter to a function or some other operation that is similar.
 *
 * In this situation, the typical first thing that should take place is to rewind the BDB using
 * BDB_Rewind() so that data can be read from the beginning of the buffer. BDB_Seek() may also be
 * appropriate for certain situation. Your needs will vary.
 *
 * A function could also receive a BDB that needs to have additional data placed into it.
 * In this situation BDB_Rewind() or BDB_Seek() probably should not be used as that will cause
 * some data already in the buffer to be overwritten. Typically, such a BDB can be written to using
 * any of the BDB_Write_* functions.
 *
 * @subsection network Network Transmissions
 *
 * Most network transmission are performed with data that is in network order. That is the bytes
 * within a particular field of data are transmitted in Big Endian order. BDB can be useful for
 * serializing data to be sent on such a network and deserializing data received from such a network.
 * If your network uses Little Endian or some other encoding scheme(which does not seem to be common)
 * you will be required to use some other serialization technique or to preprocess your data before
 * using BDB to serialize. If available, the Endian Conversion utilities in the @ref convert_endian_page
 * are useful for this preprocessing.
 *
 * @section ABBR ABBREVIATIONS:
 *   - BDB - Byte Data Buffer
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
 * @section DEV DEVIATIONS FROM STANDARDS:
 *   - None
 *
 * @defgroup bdb_list_grp Byte Data Buffer Functions
 * @ingroup utilities_grp
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"

#   ifdef __cplusplus
extern "C"
{ /* ! Inclusion of header files should NOT be inside the extern "C" block */
#   endif /* __cplusplus */
/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/
/**
 * Status code returned by some Byte Data Buffer functions to indicate the
 * requested operation completed without error.
 */
#   define BDB_OK    (0)

/**
 * Status code returned by some Byte Data Buffer functions to indicate the
 * requested operation did not complete successfully.
 */
#   define BDB_ERROR (-1)

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/
/**
 * Structure to hold a Byte Data Buffer. This structure not only contains a
 * pointer to the actual data buffer being built, but also values for the current
 * position and lengths of the buffer.
 *
 * @note The calling functions must treat this structure as opaque. That means,
 *       that the calling function needs access to this structure, but is MUST
 *       not use or change the data contained in this structure.
 */
typedef struct BDB_Tag
{
      /**
       * Pointer to a dynamically or statically allocated data buffer. This buffer
       * can grow if it is too small.
       */
      uint8_t *mBuffer;
      /**
       * Index of the current position in the buffer. This indicates the next
       * byte to be read or written. This value is updated with any read or
       * write command. It can also be adjusted with the BDB_Seek command.
       */
      size_t mBufPos;
      /**
       * This is the size of the data buffer area pointed to by mBuffer. This
       * is not the data size, but the allocated size. If an operation finds
       * that the available space is too small, it will be increased by
       * BDB_DEFAULT_BUFFER_INCREASE.
       */
      size_t mBufLen;
      /**
       * This is the size of the actual data that has been serialized. This
       * value must always be <= mBufLen.
       */
      size_t mBufDataLen;
      /**
       * This bit is set if the bdb family of functions allocated the data
       * buffer or not. A BDB that has been created using BDB_Init() will be writable.
       * A BDB that has been created using BDB_Populate() will not be writable.
       */
      bool_t mWritable;
} BDB_T;

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/**
 * Initializes a byte data buffer with at least the number of bytes requested. Sets
 * the data length and data position to 0.
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] min_buf_length
 *    - Minimum number of bytes to allocate for data buffer
 *
 * @return status
 *         - BDB_OK  Buffer creation was successful
 *         - BDB_ERROR Buffer creation failed
 *
 * @pre
 *    - bdb != NULL
 *    - 0 < min_buf_length <= UINT32_MAX - 128
 *
 * @note This function must not be used on a BDB that will also be used with
 *       BDB_Populate() as this will cause a memory leak. When a BDB is no longer
 *       needed that was initialized using BDB_Init(), BDB_Destroy() should be used to free the buffer memory.
 *
 * @note This function is typically used to create a buffer into which data is
 *       placed.
 *
 */
int32_t BDB_Init(BDB_T * bdb, size_t min_buf_length);

/**
 * Initializes a buffer using data passed in. In this function, you initialize
 * a bdb structure, except a fully populated data buffer is passed
 * in. This command is used to reconnect a bdb from data passed in a message. A bdb
 * created using BDB_Populate will be read-only.
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] buffer
 *    - Pointer to a data buffer.
 * @param [in] buf_length
 *    - The size of the buffer passed in above.
 *
 * @pre
 *    - bdb != NULL
 *    - buffer != NULL
 *    - buf_length > 0
 *
 * @note It is intended for this function to be called on a bdb structure without
 *       a dynamically allocated buffer already present. As such, this function
 *       blindly replaces the pointer to the existing pointer with the one passed
 *       in. If the old pointer was a valid pointer to dynamic memory, that address
 *       will be lost and a memory leak will occur.
 *
 * @note It is the callers responsibility to ensure that sizeof(buffer) >= buf_length
 *
 * @note The resulting BDB will be read-only. Any Write operations will result in a
 *       PBC failure.
 *
 * @note If the calling function dynamically allocates the passed in buffer, then it
 *       must save the pointer to that buffer so it may be freed later.
 *
 * @note It is not necessary to call BDB_Destroy() on a BDB created using BDB_Populate(),
 *       however it is allowed.
 */
void BDB_Populate(BDB_T * bdb, void const * buffer, size_t buf_length);

/**
 * Releases any dynamic memory allocated by a writeable buffer and resets the data structure.
 * For non-writeable buffers, erases link to static buffer
 * @note This function does not release the memory allocated by the BDB_T structure.
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 *
 * @return status
 *         - BDB_OK if successful
 *         - BDB_ERROR if bdb was not owner (could be because bdb was never owner, or
 *              bdb already freed it)
 *
 * @pre
 *    - bdb != NULL
 *    - bdb->mbuffer != NULL
 */
int32_t BDB_Destroy(BDB_T * bdb);

/**
 * Copies the data in buf to internal buffer in the Byte Data Buffer.
 * The existing data in the Byte Data Buffer is lost and replaced with
 * the data in buf. The internal buffer is re-allocated if needed.  When
 * this function exits, the data buffer position is set to the beginning
 * of the data buffer.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 * @param [in] buf
 *    - Pointer to the buffer to write or read from
 * @param [in] buf_length
 *    - The length of the buffer
 *
 * @pre
 *    - bdb != NULL
 *    - buf != NULL
 *    - bdb is writable
 */
void BDB_Copy_Buffer_Data(BDB_T * bdb, void const * buf, size_t buf_length);

/**
 * Writes a boolean to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] boolean_data
 *    - value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_Bool(BDB_T * bdb, bool_t boolean_data);

/**
 * Writes the bytes to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - Pointer to an abstract stream of bytes to write
 * @param [in] len
 *    - Number of bytes to write
 *
 * @pre
 *    - bdb != NULL
 *    - data != NULL
 *    - bdb is writable
 */
void BDB_Write_Bytes(BDB_T * bdb, void const * data, size_t len);

/**
 * Writes the characters in single bytes
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] chars
 *    - Pointer to an array of characters to write
 * @param [in] len
 *    - Number of characters to write
 *
 * @pre
 *    - bdb != NULL
 *    - chars != NULL
 *    - bdb is writable
 */
void BDB_Write_Chars(BDB_T * bdb, char const * chars, size_t len);


/**
 * Writes the string to the buffer. It does this in two pieces.
 * First the length of the string is calculated using "Safe_Strlen", excluding the NUL,
 * and stored as an unsigned 32 bit integer. Then each character is stored in the buffer
 * including NUL termination, up to the max_length passed in, including the NUL.
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] str
 *    - Pointer to a NULL terminated string
 * @param [in] max_len
 *    - Maximum offset from s to examine for the NUL terminator
 *    CAUTION: Do not use strlen to determine this length because you will defeat
 *    the purpose of this function.
 *
 * @pre
 *    - bdb != NULL
 *    - str != NULL
 *    - bdb is writable
 */
void BDB_Write_Full_String(BDB_T * bdb, char const * str, size_t max_len);

/**
 * Writes a uint8_t to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - unsigned 8-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_U8(BDB_T * bdb, uint8_t data);

/**
 * Writes an int8_t to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - signed 8-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_I8(BDB_T * bdb, int8_t data);

/**
 * Writes an unsigned 16 bit integer to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - unsigned 16-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_U16(BDB_T * bdb, uint16_t data);

/**
 * Writes a signed 16 bit integer to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - signed 16-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_I16(BDB_T * bdb, int16_t data);

/**
 * Writes an unsigned 32 bit integer to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - unsigned 32-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_U32(BDB_T * bdb, uint32_t data);

/**
 * Writes a signed 32 bit integer to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - signed 32-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_I32(BDB_T * bdb, int32_t data);

#   ifdef INT64_MIN

/**
 * Writes a unsigned 64 bit integer to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - unsigned 64-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_U64(BDB_T * bdb, uint64_t data);

/**
 * Writes a signed 64 bit integer to the buffer
 *
 * @param [out] bdb
 *    - Pointer to a byte data buffer
 * @param [in] data
 *    - signed 64-bit value to write
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Write_I64(BDB_T * bdb, int64_t data);

#   endif

/**
 * Reads a boolean value from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return boolean read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 1
 */
bool_t BDB_Read_Bool(BDB_T * bdb);

/**
 * Reads abstract data from a Byte Data buffer into the provided storage.
 *
 * @note It is assumed that the provided destination buffer contains
 *       enough space to hold the requested data. If not, a buffer overflow
 *       will occur. Take care to ensure there is enough storage for the data
 *       requested.
 *
 * @param [in]  bdb
 *    - Pointer to a byte data buffer from which to read data
 * @param [out] destination
 *    - Pointer to a data buffer in which to place data
 * @param [in]  len
 *    - Number of bytes to read
 *
 * @pre
 *    - bdb != NULL
 *    - bytes != NULL
 *    - Number of bytes remaining >= len
 */
void BDB_Read_Bytes(BDB_T * bdb, void * destination, size_t len);

/**
 * Provides a pointer to an array of bytes contained in the Byte Data Buffer.
 * Since this array of bytes can be a data stream of any type, a void pointer
 * is returned and it is left to the user to cast the data to the desired type.
 * The Byte Data Buffer position is updated to the position following this array
 * of bytes (moved len bytes).
 *
 * @param [in] bdb
 *    - Pointer to the Byte Data Buffer containing the byte array
 * @param [in] len
 *    - number of bytes contained in the byte array
 *
 * @return
 *   - A pointer to the beginning of the byte array; NULL if len==0.
 *
 * @pre
 *   - bdb != NULL
 *   - Number of bytes remaining >= len
 */
void const * BDB_Read_P_Bytes(BDB_T * bdb, size_t len);

/**
 * Reads character data from a Byte Data buffer into the provided storage.
 *
 * @note It is assumed that the provided destination buffer contains
 *       enough space to hold the requested data. If not, a buffer overflow
 *       will occur. Take care to ensure there is enough storage for the data
 *       requested.
 *
 * @param [in]  bdb
 *    - Pointer to a byte data buffer from which to read data
 * @param [out] destination
 *    - Pointer to a character buffer in which to place the read data
 * @param [in]  len
 *    - Number of character to read
 *
 * @pre
 *    - bdb != NULL
 *    - destination != NULL
 *    - Number of bytes remaining >= len
 */
void BDB_Read_Chars(BDB_T * bdb, char * destination, size_t len);

/**
 * Reads a string from a Byte Data Buffer and copies it to the provided destination
 * string. maxLen is the maximum size of the destination buffer, including the '/0'.
 * If the string stored in the BDB is less than or equal to max_len, then the data is
 * copied to the string buffer. If the string in the BDB is longer than max_len, then
 * max_len-1 characters are copied to the receive buffer, plus a '/0'. The remaining
 * characters in the buffer are purged and discarded. Regardless of how many characters
 * are read, the data position will be just after the terminating '/0'.
 *
 * @note a malformed string that does not possess enough data bytes in the buffer to cover
 *       the length of the string (not max_len, but the length of the string present in the
 *       buffer) will result in a PBC Failure.
 *
 * @param [in]  bdb
 *    - Pointer to a byte data buffer from which to read the string
 * @param [out] destination
 *    - Pointer to a data buffer in which to place the string
 * @param [in]  max_len
 *    - Maximum number of characters to read, including NIL terminator.
 *
 * @pre
 *    - bdb != NULL
 *    - destination != NULL
 */
void BDB_Read_String(BDB_T * bdb, char * destination, size_t max_len);

/**
 * Returns a pointer to a String in the Byte Data Buffer. The BDB position
 * is updated to the next item in the byte data buffer. Since, the string data
 * is not extracted from the message, care must be taken that the
 * data is used immediately, before the buffer is destroyed (i.e. Copy the data
 * immediately).
 *
 * For example
 *
 * @code
 * char* my_string;
 *
 * my_string = BDB_Read_P_String(&bdb);
 * if(strcmp(my_string, "Error"))
 * {
 *    error_returned = true;
 * }
 * // my_string no longer needed
 * @endcode
 *
 * @note a malformed string that does not possess enough data bytes in the buffer to cover
 *       the length of the string (not max_len, but the length of the string present in the
 *       buffer) will result in a PBC Failure.
 *
 * @param [in]  bdb
 *    - Pointer to a byte data buffer
 *
 * @return Pointer to string in BDB
 *
 * @pre
 *    - bdb != NULL
 *    - proper BDB string at current bdb location
 */
char const * BDB_Read_P_String(BDB_T * bdb);

/**
 * Allocate memory for, and read the string into the allocated memory.
 * This is similar to #BDB_Read_String, except that instead
 * of passing in a buffer, a buffer is created. A pointer
 * to that buffer is returned. The calling function is
 * responsible for freeing the memory.
 *
 * @note a malformed string that does not possess enough data bytes in the buffer to cover
 *       the length of the string (not max_len, but the length of the string present in the
 *       buffer) will result in a PBC Failure.
 *
 * @param [in]  bdb
 *    - Pointer to a byte data buffer
 *
 * @return Pointer to new allocated string buffer containing string
 *
 * @pre
 *    - bdb != NULL
 */
char * BDB_Read_Alloc_String(BDB_T * bdb);

/**
 * Reads an unsigned 8 bit integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return unsigned 8 bit integer read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 1
 */
uint8_t BDB_Read_U8(BDB_T * bdb);

/**
 * Reads a signed 8 bit integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return signed 8 bit integer read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 1
 */
int8_t BDB_Read_I8(BDB_T * bdb);

/**
 * Reads a 16 bit unsigned integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return uint16_t read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 2
 */
uint16_t BDB_Read_U16(BDB_T * bdb);

/**
 * Reads a 16 bit signed integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return int16_t read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 2
 */
int16_t BDB_Read_I16(BDB_T * bdb);

/**
 * Reads a 32 bit unsigned integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return uint32_t read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 4
 */
uint32_t BDB_Read_U32(BDB_T * bdb);

/**
 * Reads a 32 bit signed integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return int32_t read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 4
 */
int32_t BDB_Read_I32(BDB_T * bdb);

#   ifdef INT64_MIN
/**
 * Reads a 64 bit unsigned integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return uint64_t read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 8
 */
uint64_t BDB_Read_U64(BDB_T * bdb);

/**
 * Reads a 64 bit signed integer from the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to a byte data buffer
 *
 * @return int64_t read from buffer
 *
 * @pre
 *    - bdb != NULL
 *    - Number of bytes remaining >= 8
 */
int64_t BDB_Read_I64(BDB_T * bdb);
#   endif

/**
 * Returns a pointer to the internal data buffer of the Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 *
 * @return Pointer to the internal data buffer
 *
 * @pre
 *    - bdb != NULL
 */
void const * BDB_Get_Buffer(BDB_T const * bdb);

/**
 * Returns the length of the data stored in the buffer.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 *
 * @return Length of data in data buffer
 *
 * @pre
 *    - bdb != NULL
 */
size_t BDB_Get_Data_Length(BDB_T const * bdb);

/**
 * Increases the size of the internal buffer in order to guarantee that the size
 * is at least min_buf_length. If the internal buffer is already larger, then
 * nothing will be done.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 * @param [in] min_buf_length
 *    - Minimum size of the internal data buffer.
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 *    - min_buf_length > 0
 */
void BDB_Guarantee_Min_Buffer_Length(BDB_T * bdb, size_t min_buf_length);

/**
 *  Resets the (read/write) buffer position to 'seek_pos'.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 * @param [in] seek_pos
 *    - new read/write position
 *
 * @pre
 *    - bdb != NULL
 *    - seek_pos <= bdb->mBufDataLen
 */
void BDB_Seek(BDB_T * bdb, size_t seek_pos);

/**
 * Returns buffer position.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 *
 * @return Current read/write position
 *
 * @pre
 *    - bdb != NULL
 */
size_t BDB_Tell(BDB_T const * bdb);

/**
 *  Resets the read/write position to zero but retains all of the data in the
 *  buffer. Writing data after a BDB_Rewind() will overwright data within the buffer.
 *  Typically this function is used to read data from the beginning of a received
 *  Byte Data Buffer.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 *
 * @pre
 *    - bdb != NULL
 */
void BDB_Rewind(BDB_T * bdb);

/**
 * Removes all data from the buffer and sets the read/write position to 0. The storage
 * buffer is retained so the Byte Data Buffer is still appropriate for writing.
 *
 * @note This does not deallocate the buffer. To deallocate the buffer, @see BDB_Destroy.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 *
 * @pre
 *    - bdb != NULL
 *    - bdb is writable
 */
void BDB_Erase(BDB_T * bdb);

/**
 * Returns true if the buffer contains any data.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 *
 * @pre
 *    - bdb != NULL
 */
bool_t BDB_Good(BDB_T const * bdb);

/**
 * Dump the entire bdb structure to STDOUT using
 * printf. This is for debug only.
 *
 * @param [in] bdb
 *    - Pointer to bdb buffer
 *
 * @pre
 *    - bdb != NULL
 */
void BDB_Print(BDB_T const * bdb);

/**
 * Similar to snprintf, this function will print the contents of a Byte
 * Data Buffer into the provided string in a human readable format. It
 * will place at most the number of characters specified in str_len.
 *
 * @note This function will not provide the header information that BDB_Print
 *       provides. It only prints the buffer contents.
 *
 * @param [out] str
 *    - String in which to place the printed data of the buffer contents
 * @param [in] str_len
 *    - Maximum number of characters to place into str
 * @param [in] bdb
 *    - Pointer to a bdb to print
 *
 * @pre
 *    - str != NULL
 *    - bdb != NULL
 */
void BDB_Snprint(char * str, uint32_t str_len, BDB_T const * bdb);

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

#   ifdef __cplusplus
} /* extern "C" */
#   endif /* __cplusplus */
/*===========================================================================*/
/*!
 * @file byte_data_buffer.h
 *
 * @section RH REVISION HISTORY (top to bottom: last revision to first revision)
 *
 * - 12-Feb-2013 Kris Boultbee Rev 1
 *   - Created initial file from contents of bb_util
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* BYTE_DATA_BUFFER_H */
