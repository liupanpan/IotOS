#==============================================================================
# FILE: Makefile_dbg_trace_xsal.mk
#
#------------------------------------------------------------------------------
#
# Copyright 2013 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#
#------------------------------------------------------------------------------
#
# %full_filespec: Makefile_dbg_trace_xsal.mk~kok_inf#4:makefile:kok_basa#1 %
# %version: kok_inf#4 %
# %derived_by: dzq92s %
# %date_modified: Mon May 11 13:57:10 2015 %
#
# DESCRIPTION:
# ------------
# This file provides the project-specific information required for a build
# of this module. On entry, the variable "_build_" identifies which build is
# being performed, and the variable $(_build_)_FLAVOR defines the flavor for
# this build. This file includes template Makefiles for the flavor
# from the $(MAKE_INCL_DIR) directory, which must be defined prior to
# including this file.
#
#==============================================================================
_flavor_ := $($(_build_)_FLAVOR)

include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

_src_dirs_ := $(_cur_module_path_)/bb_dbg_trace_xsal_imp/_src

_lib_name_ := dbg_trace_xsal

_lib_objs_ :=                                                       \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_xsal$(_obj_ext_)               \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace$(_obj_ext_)                    \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_isa$(_obj_ext_)                \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_ps$(_obj_ext_)                 \
  $($(_build_)_OBJ_OUTDIR)/em_assert$(_obj_ext_)                    \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_compact$(_obj_ext_)            \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_verbose$(_obj_ext_)            \
  $($(_build_)_OBJ_OUTDIR)/dbg_call_stack_backtrace_c99$(_obj_ext_) \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_c99$(_obj_ext_)                \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_c99_log$(_obj_ext_)            \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_c99_store$(_obj_ext_)          \
  $($(_build_)_OBJ_OUTDIR)/em_assert_c99$(_obj_ext_)                \
  $($(_build_)_OBJ_OUTDIR)/printf_gate$(_obj_ext_)                  \

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 28-Feb-2013 Kirk Bailey
# + Created initial file.
#
# 1-July-2013 Dan Carman
# + Modified library to include all debug trace files for xsal 
#
# 03-Oct-2013 Kirk Bailey REV 3
# + Fixed order of revision history comments - no functional changes
#
#===========================================================================
