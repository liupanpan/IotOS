#==============================================================================
# FILE: Makefile_dt.mk
#
#------------------------------------------------------------------------------
#
# Copyright 2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#
#------------------------------------------------------------------------------
#
# %full_filespec:Makefile_dt.mk~1:makefile:kok_basa#1 %
# %version:1 %
# %derived_by:dzq92s %
# %date_modified:Thu Jul  2 07:42:01 2015 %
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

# Only build for full XSAL!
ifeq ($(XSAL_$(_build_)_TYPE),XSAL)

include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

_src_dirs_ := $(_cur_module_path_)/

_bin_name_ := dt$(_bin_ext_)

_bin_objs_ := $($(_build_)_OBJ_OUTDIR)/dt$(_obj_ext_)

_bin_libs_ :=\
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)dbg_trace_xsal$(_lib_ext_)     \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)double_linked_list$(_lib_ext_) \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)string_resource$(_lib_ext_)    \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)xsal$(_lib_ext_)               \
  
 
ifeq ($(_flavor_),linux_all)
  _bin_libs_ += pthread.so rt.so
endif

ifeq ($(_flavor_),win32_gcc)
  _bin_libs_ += WS2_32
endif

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

#####
endif # ifeq ($(XSAL_$(_build_)_TYPE),XSAL)
#####

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 01-Jul-2015 Kirk Bailey
# + Created initial file.
#
#===========================================================================
