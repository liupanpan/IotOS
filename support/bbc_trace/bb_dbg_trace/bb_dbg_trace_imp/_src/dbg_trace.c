/*===========================================================================*/
/**
 * @file dbg_trace.c
 *
 *   Implements dbg_trace functions associated with enabling/disabling trace
 *   for individual modules.
 *
 * %full_filespec:dbg_trace.c~kok_inf#12:csrc:kok_aud#1 %
 * @version %version:kok_inf#12 %
 * @author  %derived_by:dzq92s %
 * @date    %date_modified:Tue May 12 08:49:57 2015 %
 *
 *------------------------------------------------------------------------------
 *
 * Copyright 2008-2013 Delphi Technologies, Inc., All Rights Reserved.
 * Delphi Confidential
 *
 *------------------------------------------------------------------------------
 *
 * @section DESC DESCRIPTION:
 *
 * @section ABBR ABBREVIATIONS:
 *   - TR = TRace
 *
 * @section TRACE TRACEABILITY INFO:
 *   - Design Document(s):
 *
 *   - Requirements Document(s):
 *
 *   - Applicable Standards (in order of precedence: highest first):
 *     - <a href="http://kogespw1.delcoelect.com:8508/passthru/consumer?name=SW%20REF%20264.15D&doctype=K8DBSDoc">
 *       SW REF 264.15D "Delphi C Coding Standards" [12-Mar-2006]</a>
 *
 * @section DFS DEVIATIONS FROM STANDARDS:
 *   - none
 */
/*==========================================================================*/

/*===========================================================================*
 * Header Files
 *===========================================================================*/
#include "dbg_trace_cbk.h"
#include "dbg_trace_cfg.h"
#include "dbg_trace_imp.h"
#include "dbg_trace_ps.h"
#include "em_assert.h"

/*===========================================================================*
 * Local Preprocessor #define Constants
 *===========================================================================*/

/*===========================================================================*
 * Local Preprocessor #define MACROS
 *===========================================================================*/

/*===========================================================================*
 * Local Type Declarations
 *===========================================================================*/

/*===========================================================================*
 * Exported Const Object Definitions
 *===========================================================================*/

/*===========================================================================*
 * Local Object Definitions
 *===========================================================================*/

/*
 * Construct a parallel "check" enum with the same tags as the API's
 * enum except that "_CHECK" is appended. For a properly constructed
 * implementation, there will be a <tag>_CHECK for each API <tag> and
 * it will have the same value.
 */
#undef TR_MODULE_LEVEL
#undef TR_MODULE_LEVEL_EQ
#define TR_MODULE_LEVEL(ID, init_lvl) ID##_CHECK,
#define TR_MODULE_LEVEL_EQ(ID, id_value, init_lvl) ID##_CHECK=id_value,


typedef enum 
{
   TR_NO_MODULE_ID_CHECK = 0,
   TR_MODULE_LEVELS
   TR_NUM_MODULE_LEVELS_CHECK
} Tr_Module_Level_Check_T;

/*
 * Construct a sum representing the number of differences between the API and
 * implementation enum tags. For a correct implementation, the sum must be zero.
 */
#undef TR_MODULE_LEVEL
#undef TR_MODULE_LEVEL_EQ
#define TR_MODULE_LEVEL(ID, init_lvl) (((int)ID == (int)ID##_CHECK) ? 0 : 1) +
#define TR_MODULE_LEVEL_EQ(ID, id_value, init_lvl) (((int)ID == (int)ID##_CHECK) ? 0 : 1) +

/**
 * This typedef checks whether the elements in the TR_MODULE_LEVELS X MACRO
 * are in the exact order as the elements in the Tr_Module_IDs X MACRO. If
 * they are not, then the compiler will generate an error because we will
 * attempt to create a type with a negative array size.
 */
typedef char Tr_Check_Module_Order_T[((TR_MODULE_LEVELS 0) == 0) ? 1:-1];

#undef Tr_Module_ID
#undef Tr_Module_ID_EQ
#undef TR_MODULE_LEVEL
#undef TR_MODULE_LEVEL_EQ
#define Tr_Module_ID(ID, name)                     1 +
#define Tr_Module_ID_EQ(ID, id_value, name)        1 +
#define TR_MODULE_LEVEL(ID, init_lvl)              1 +
#define TR_MODULE_LEVEL_EQ(ID, id_value, init_lvl) 1 +
/**
 * This typedef checks whether the number of elements in the TR_MODULE_LEVELS
 * X MACRO is the same as in the Tr_Module_IDs X MACRO. If not, then the
 * compiler will generate an error because we will attempt to create a type
 * with a negative array size.
 */
typedef char Tr_Check_Module_Num_T[((TR_MODULE_LEVELS 0) == (Tr_Module_IDs 0)) ? 1:-1];


#undef TR_MODULE_LEVEL
#undef TR_MODULE_LEVEL_EQ
/* PRQA S 3410 10 */
/**
 * X-Macro to defining enumeration for a module ID
 */
#define TR_MODULE_LEVEL(ID, init_lvl) init_lvl,
#define TR_MODULE_LEVEL_EQ(ID, id_value, init_lvl) init_lvl,

/**
 * Keeps track of the run-time trace level for each of the modules in the
 * system.
 */
static Tr_Trace_Level_T module_trace_levels[TR_NUM_MODULE_IDS] =
{
   TR_DEFAULT_RUNTIME_INFO_LEVEL,
   TR_MODULE_LEVELS /* Assign initial values for trace levels */
};

/*===========================================================================*
 * Local Function Prototypes
 *===========================================================================*/

/*===========================================================================*
 * Local Inline Function Definitions and Function-Like Macros
 *===========================================================================*/

/*===========================================================================*
 * Function Definitions
 *===========================================================================*/

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace_imp.h
 *
 *===========================================================================*/
bool_t tr_Is_Trace_Level_Enabled(uint16_t file_id, Tr_Trace_Level_T level)
{
   bool_t is_enabled = true;

   /* NOTE: TR_LVL_FAULT/WARN are always enabled - only check INFO levels */
   if ((level > TR_LVL_WARN) && (level <= TR_LVL_INFO_LO))
   {
      /* Extrace EM module id from the file id. */
      uint16_t module_id = (uint16_t) (file_id >> EM_MOD_SHIFT_CNT);

      if (module_id < TR_NUM_MODULE_IDS)
      {
         is_enabled = (module_trace_levels[module_id] >= level);
      }
   }
   return is_enabled;
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace.h
 *
 *===========================================================================*/
Tr_Trace_Level_T Tr_Get_Module_Info_Trace_Level(Tr_Module_ID_T module_id)
{
   Tr_Trace_Level_T level = TR_LVL_FAULT;
   int32_t index = module_id;

   if ((index >= 0) && (index < TR_NUM_MODULE_IDS))
   {
      level = module_trace_levels[index];
   }

   return level;
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace.h
 *
 *===========================================================================*/
void Tr_Read_Trace_Levels(char const *path)
{
   Tr_PS_Read_Trace_Levels(path, module_trace_levels, Num_Elems(module_trace_levels));
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace.h
 *
 *===========================================================================*/
void Tr_Save_Trace_Levels(char const *path)
{
   Tr_PS_Save_Trace_Levels(path, module_trace_levels, Num_Elems(module_trace_levels));
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace.h
 *
 *===========================================================================*/
void Tr_Set_Module_Info_Trace_Level(Tr_Module_ID_T module_id, Tr_Trace_Level_T lvl)
{
   int32_t index = module_id;

   if ((index >= 0) && (index < TR_NUM_MODULE_IDS) && (lvl <= TR_LVL_INFO_LO))
   {
      module_trace_levels[index] = lvl;
   }
}

/*===========================================================================*
 *
 * Please refer to the detailed description in dbg_trace.h
 *
 *===========================================================================*/
void Tr_Set_Info_Trace_Level(Tr_Trace_Level_T lvl)
{
   Tr_Module_ID_T module_id;

   for (module_id = (Tr_Module_ID_T) 0; module_id < TR_NUM_MODULE_IDS; module_id++)
   {
      Tr_Set_Module_Info_Trace_Level(module_id, lvl);
   }
}

/*===========================================================================*/
/*!
 * @file dbg_trace.c
 *
 * @section RH REVISION HISTORY (top to bottom: first revision to last revision)
 *
 * - 02-Jun-2011 kirk bailey Rev. 9
 *   - Task 34345 - Added Tr_Read_Trace_Levels and Tr_Save_Trace_Levels.
 *
 * - 31-May-2012 kirk bailey Rev. 10
 *   - Fix gcc 4.6 warning.
 *
 * - 03-Oct-2013 Kirk Bailey REV 11
 *   - Fixed order of revision history comments - no functional changes
 *   - Eliminate revision history prior to 2011
 *
 * - 12-May-2015 Kirk Bailey REV 12
 *   - Removed redundant conditional path.
 */
/*===========================================================================*/
