#ifndef EM_H
#   define EM_H
/*===========================================================================*/
/**
 * @file em.h
 *
 * API for Error Management (EM) Assertions and Trace functionality
 *
 * %full_filespec:em.h~kok_inf#5:incl:kok_aud#6 %
 * @version %version:kok_inf#5 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Thu Oct  3 09:33:58 2013 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * This header defines the depracated EM macros in terms of the new
 * PBC / Trace APIs
 *
 * It does not support the message or binary traces
 *
 * @defgroup em OLD Programming by Contract (PbC) APIs
 * @{
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#   include "reuse.h"
#   include "pbc_trace.h"

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Preprocessor #define MACROS
 *===========================================================================*/

/**
 * This macro should be used at the start of enumeration typedef to declare common module 
 * trace IDs in a shared module level header file.
 *
 * @param mod_id Module ID value   
 * @param id The first enumerated trace id for a module  
 * @param desc  Text description of trace value used by external tools
 */
#   define EM_First_Module_Trace_ID(mod_id, id, desc) id = mod_id,

/* PRQA S 3409 10 */
/**
 * This alternate macro can used at the start of enumeration typedef to declare file  
 * specific trace IDs. It should be used if trace IDs are declared in a c file.
 * Bit 11 = 1 is used to differentiate file 0 local trace IDS from module level common ids
 *
 * @param id The first enumerated trace id withing a c file
 * @param description  Text description of trace value used by external tools
 */
#   define EM_First_File_Trace_ID(id, description)   id = (((EM_F_ID) << 12) + 0x800),

/**
 * This macro shall be used to declare Trace IDs.  The standard define expands to just 
 * the ID name for use in enumeration typedef. It's primary intent is to allow an 
 * external tool to scan for all trace ids and their description. 
 * It should be used in an enumberation type declaration either @see EM_First_Module_Trace_ID 
 * or @see EM_First_File_Trace_ID macros to initialize the enumeration.
 *
 * @param id Enumerated trace id
 * @param description  Text description of trace value used by external tools
 */
#   define EM_Trace_ID(id, description)   id,

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

   typedef enum EM_Common_Trace_ID_Tag
   {
      EMT_ASSERT_FAULT,         /* Assert failure */
      EMT_FUNCTION_START,       /* Enter Function */
      EMT_FUNCTION_END,         /* Exit Function */
      EMT_TIMEOUT,              /* Timeout expired */
      EMT_UNKNOWN_EVENT,        /* Unknown event received */
      EMT_PARAMETER_INVALID,    /* Invalid paramter / out of range */
      EMT_PARAMETER_TOO_LOW,    /* Given paramter is too low */
      EMT_PARAMETER_TOO_HIGH,   /* Given paramter is too high */
      EMT_NULL_POINTER,         /* Pointer value is NULL */
      EMT_CREATE_OBJECT,        /* Create Object */
      EMT_DESTROY_OBJECT,       /* Destroyed Object */
      EMT_INVALID_CHECKSUM,     /* Checksum is not the expected one */
      EMT_INVALID_FORMAT,       /* Format mismatch */
      EMT_CORRUPT_DATA,         /* Data is corrupt */
      EM_NUM_GLOBAL_TRACE_IDS
   }
   EM_Common_Trace_ID_T;

/*===========================================================================*
 * Exported Const Object Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/
#   ifdef __cplusplus
extern "C"
{
#   endif       /*__cplusplus*/

/*===========================================================================*
 * Exported Inline Function Definitions and #define Function-Like Macros
 *===========================================================================*/

/**
 * General purpose assertion that makes sure the @a expr_ argument is
 * TRUE. Calls the EM_Assert_Handler() callback if the @a expr_ evaluates
 * to FALSE.
 */
#   define EM_ASSERT(expr_, error_value)   PBC_Ensure_1(expr_, "Error Value : %d", (int) (error_value))

/**
 * Compile-time assertion produces a compiler error if invalid;  produces no
 * code if valid. For use at function scope. from http://www.panelsoft.com/murphyslaw/apr01.htm 
 */
#   define EM_ASSERT_COMPILE(ex) do {\
typedef uint8_t COMPILE_TIME_ASSERTION_FAILURE[(ex) ? 1 : -1];\
} while(0)

/**
 * Compile-time assertion produces a compiler error if invalid;  produces no
 * code if valid. For use at file scope. From http://www.panelsoft.com/murphyslaw/apr01.htm 
 */
#   define EM_FS_ASSERT_COMPILE(ex) extern uint8_t COMPILE_TIME_ASSERTION_FAILURE[(ex) ? 1 : -1]

/*===========================================================================*
 *  Programming by Contract (PbC) assertions
 *===========================================================================*/

/**
 * EM_ASSERT_CHECK_INTERNAL is used in order to check assertions 
 * used inside modules as opposed to at API interfaces. 
 *
 * In order to configure this on a module-by-module basis, the makefile 
 * for a module would include the following -DEM_ASSERT_CHECK_INTERNAL=false
 * (see Makefile_bb_em_tst.mk in the em mdf test directory for an example)
 *
 * Alternative is explictly set value by setting this define 
 * before include of em.h 
 * @code 
 * #   undef EM_ASSERT_CHECK_INTERNAL
 * #   define EM_ASSERT_CHECK_INTERNAL false    / **< disable internal checks for this file * /
 *   or 
 * #   undef EM_ASSERT_CHECK_INTERNAL
 * #   define EM_ASSERT_CHECK_INTERNAL true    / **< force internal checks for this file * /
 * @endcode
 */
#   define ASSERT_INTERNAL(expr_, error_value) PBC_Internal_1(expr_, "Internal Error Value : %d", (int) (error_value))

/**
 * Assertion that checks for a precondition. This macro is equivalent to
 * @ref EM_ASSERT, except the name provides a better documentation of the
 * intention of this assertion. Should be used in API functions
 */
#   define EM_REQUIRE(expr_ ,error_value) EM_ASSERT(expr_ , (error_value))

/**
 * Assertion that checks for a module's internal precondition.
 * This macro is equivalent to @ref EM_REQUIRE when ASSERT_CHECK_INTERNAL 
 * is defined else does nothing
 */
#   define EM_REQUIRE_INTERNAL(expr_ , error_value) ASSERT_INTERNAL(expr_ , (error_value))

/**
 * Generates a Assertion when a precondition is unmet. The unmet precondition
 * must be detected by normal code 
 */
#   define EM_REQUIRE_FAILED(error_value)  EM_REQUIRE(false, (error_value))

/**
 * Assertion that checks for a postcondition. This macro is equivalent to
 * @ref EM_ASSERT, except the name provides a better documentation of the
 * intention of this assertion. Should be used in API functions
 */
#   define EM_ENSURE(expr_, error_value) EM_ASSERT(expr_ , (error_value))

/**
 * Assertion that checks for a module's internal postcondition. 
 * This macro is equivalent to @ref EM_ENSURE when ASSERT_CHECK_INTERNAL 
 * is defined else does nothing.
 */
#   define EM_ENSURE_INTERNAL(expr_, error_value) ASSERT_INTERNAL(expr_ , error_value)

/**
 * Generates a Assertion when a postcondition is unmet. The unmet postcondition
 * must be detected by normal code 
 */
#   define EM_ENSURE_FAILED(error_value)  EM_ENSURE(false, (error_value))

/**
 * Assertion that checks for an invariant. This macro is equivalent to
 * @ref EM_ASSERT, except the name provides a better documentation of the
 * intention of this assertion. Should be used in API functions
 */
#   define EM_INVARIANT(expr_, error_value)  EM_ASSERT(expr_ , (error_value))

/**
 * Assertion that checks for an module's internal invariant. 
 * This macro is equivalent to @ref EM_INVARIANT when ASSERT_CHECK_INTERNAL 
 * is defined else does nothing.
 */
#   define EM_INVARIANT_INTERNAL(expr_, error_value) ASSERT_INTERNAL(expr_ , error_value)

/** Assertion that always fails */
#   define EM_ASSERT_ERROR(error_value) EM_ASSERT(false, (error_value))

/** @} doxygen end group */

/** @addtogroup em
 * @{
 */

/*===========================================================================*
 * Exported Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Exported Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Function Prototypes
 *===========================================================================*/

/*============================================================================*
 *  Program Trace Macros
 *============================================================================*/

/**
 * Interface for logging / storing FAULT events
 * @param trace_id = Each module defines its own Trace ID’s
 * @param trace_value = Optional parameter. Used to inform why the trace was logged.
 */
#   define EM_TRACE_FAULT(trace_id, trace_value) Tr_Fault_2("EM Trace ID: %d Param: %d", (int) (trace_id), (int) (trace_value))

/**
 * Interface for logging / storing WARNING events
 * @param trace_id = Each module defines its own Trace ID’s
 * @param trace_value = Optional parameter. Used to inform why the trace was logged.
 */
#   define EM_TRACE_WARNING(trace_id, trace_value) Tr_Warn_2("EM Trace ID: %d Param: %d", (int) (trace_id), (int) (trace_value))

/**
 * Interface for logging / storing Critical Operations events
 * @param trace_id = Each module defines its own Trace ID’s
 * @param trace_value = Optional parameter. Used to inform why the trace was logged.
 */
#   define EM_TRACE_INFO_HIGH(trace_id, trace_value) Tr_Info_Hi_2("EM Trace ID: %d Param: %d", (int) (trace_id), (int) (trace_value))

/**
 * Interface for logging / storing non-critical major operations events
 * @param trace_id = Each module defines its own Trace ID’s
 * @param trace_value = Optional parameter. Used to inform why the trace was logged.
 */
#   define EM_TRACE_INFO_MID(trace_id, trace_value) Tr_Info_Mid_2("EM Trace ID: %d Param: %d", (int) (trace_id), (int) (trace_value))

/**
 * Interface for logging / storing low level debug events
 * @param trace_id = Each module defines its own Trace ID’s
 * @param trace_value = Optional parameter. Used to inform why the trace was logged.
 */
#   define EM_TRACE_INFO_LOW(trace_id, trace_value) Tr_Info_Lo_2("EM Trace ID: %d Param: %d", (int) (trace_id), (int) (trace_value))


/**
 * Interface for logging / storing user debug events
 * @param msg = A message (string)
 * params a1...a4 = Optional parameter like printf!
 *
 * Note, new Trace module only supports upto 4 parameters 
 *
 * e.g. EM_TRACE_INFO_MSG("Test");
 * e.g. EM_TRACE_INFO_MSG1("Value= %d", my_value);
 */
#   define EM_TRACE_INFO_MSG(msg)                     Tr_Info_Lo(msg)
#   define EM_TRACE_INFO_MSG1(msg, a1)                Tr_Info_Lo_1(msg, a1)
#   define EM_TRACE_INFO_MSG2(msg, a1, a2)            Tr_Info_Lo_2(msg, a1, a2)
#   define EM_TRACE_INFO_MSG3(msg, a1, a2, a3)        Tr_Info_Lo_3(msg, a1, a2, a3)
#   define EM_TRACE_INFO_MSG4(msg, a1, a2, a3, a4)    Tr_Info_Lo_4(msg, a1, a2, a3, a4)

#   ifdef __cplusplus
}                               /* close off extern "C" specification */
#   endif  /*__cplusplus*/

/*===========================================================================*/
/*!
 * @file em.h
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 07-Jan-2009 Dan Carman
 *   - Created replacement for old EM APIs
 *
 * - 09-feb-2009 kirk bailey
 *   - Fixed Doxygen warning.
 *
 * - 03-Oct-2013 Kirk Bailey REV 5
 *   - Fixed order of revision history comments - no functional changes
 */
/*===========================================================================*/
/** @} doxygen end group */
#endif                          /* EM_H */
