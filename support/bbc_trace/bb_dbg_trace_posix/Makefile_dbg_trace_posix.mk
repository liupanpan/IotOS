#==============================================================================
# FILE: Makefile_dbg_trace_posix.mk
#
#------------------------------------------------------------------------------
#
# Copyright 2013-2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#
#------------------------------------------------------------------------------
#
# %full_filespec: Makefile_dbg_trace_posix.mk~kok_inf#9:makefile:kok_basa#1 %
# %version: kok_inf#9 %
# %derived_by: dzq92s %
# %date_modified: Tue May 12 09:23:29 2015 %
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

_src_dirs_ := $(_cur_module_path_)/bb_dbg_trace_posix_imp/_src

_lib_name_ := dbg_trace_posix

_lib_objs_ :=                                                       \
  $($(_build_)_OBJ_OUTDIR)/dbg_trace_posix$(_obj_ext_)              \
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
  $($(_build_)_OBJ_OUTDIR)/posix_printf_gate$(_obj_ext_)            \

#
# Define the list of libraries to link against when linking as a shared object.
# If you want the library to be defined as a prerequisite of the shared object 
# (so that the shared object is rebuilt any time the library is new), then you 
# must specify the path to the library; e.g., "<rel_proj_dir>/libfoo.a". Libraries 
# specified without a path (e.g., "libfoo.a") will not be made prerequisites of 
# the shared object and will be found by dynamically searching the library directories
# (LIB_DIRS) in whatever manner is prescribed for the tools associated with this flavor.
#
# EXAMPLE: _lib_libs_ := $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)$($(_build_)_lib_name_lib)
#
ifneq ($(filter linux%,$(_flavor_)),)

  _lib_libs_  := rt      \
  $($(_build_)_LIB_OUTDIR)/libstring_resource.a
  
endif

# Need to remove "-ansi" for Cygwin build to find clock_gettime.
ifeq ($(_flavor_),win32_gcc)
  $(_lib_objs_):CC_$(_build_)_WARN := -Wall -Wswitch-default -Wdeclaration-after-statement -Wfloat-equal -Wshadow
endif

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
#    Modified library to include all files 
#
# 03-Oct-2013 Kirk Bailey REV 4
# + Fixed order of revision history comments - no functional changes
#
# 21-Apr-2014 Joel Kiger rev 5
# + Task kok_inf#6914 Add required libraries for shared linking
#
# 12-May-2015 Kirk Bailey rev 9
# + Moved Printf_Lock/Unlock to separate compilation units
#===========================================================================
