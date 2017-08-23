#==============================================================================
# FILE: Makefile_bb_gen_xsal_msg_list_imp.mk
#
#------------------------------------------------------------------------------
#
# Copyright 2011 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#
#------------------------------------------------------------------------------
#
# %full_filespec:Makefile_bb_gen_xsal_msg_list.mk~4:makefile:kok_basa#1 %
# %version:4 %
# %derived_by:jz4fpf %
# %date_modified:Tue Apr 14 15:18:54 2015 %
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

#
# If the build of this module is only appropriate for a certain flavor, or
# flavors, then uncomment the 'ifeq" (AND THE CORRESPONDING "endif" AT THE
# END OF THE FILE!).
#
##################################
# Only build for full XSAL!
ifeq ($(XSAL_$(_build_)_TYPE),XSAL)
##################################

include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

###############################################################################
# Definitions for compiling/assembling source code into object code:
####################################################################

#
# Define the directory (relative to project root directory) containing the
# source files for this module.
#
# EXAMPLE: _src_dirs_ := $(_cur_module_path_)/<implementation directory>/_src
#
_src_dirs_ := $(_cur_module_path_)/bb_gen_xsal_msg_list_imp/_src

###############################################################################
# Definitions for generating an executable from object files and libraries:
###########################################################################

#
# Define the name of the executable:
#
# EXAMPLE: _bin_name_ := my_prog$(_bin_ext_)
#
# Note: If GEN_XSAL_MSG_LIST_$(_build_)_SUFFIX is set to something in the
# root Makefile, then it will be appended to the application gen_xsal_msg_list.
# To make this backwards compatable, the "_" is not included, so, if used, the
# suffix must contain the leading underscore. For example:
#
# GEN_XSAL_MSG_LIST_pc_SUFFIX := _windows
# 
# This will cause "gen_xsal_msg_list_windows" to be generated when "make pc" is run.
#
_bin_name_ := gen_xsal_msg_list$(GEN_XSAL_MSG_LIST_$(_build_)_SUFFIX)$(_bin_ext_)
#
# Define the list of object files for the executable:
#
# EXAMPLE: _bin_objs_ := $($(_build_)_OBJ_OUTDIR)/my_prog$(_obj_ext_)
#
_bin_objs_ := $($(_build_)_OBJ_OUTDIR)/gen_xsal_msg_list$(_obj_ext_)

# Must turn off -ansi for Cygwin gcc to see strdup prototype
ifeq ($(_flavor_),win32_gcc)
  $(_bin_objs_):CC_$(_build_)_WARN := -Wall
endif
#
# Define the list of libraries to link against. If you want the library to be
# defined as a prerequisite of the binary (so that the binary is rebuilt any
# time the library is new), then you must specify the path to the library; 
# e.g., "<rel_proj_dir>/libfoo.a". Libraries specified without a path (e.g.,
# "libfoo.a") will not be made prerequisites of the binary and will be
# found by dynamically searching the library directories (_bin_lib_dirs_,
# defined below) in whatever manner is prescribed for the tools associated with
# this flavor.
#
# EXAMPLE:
# _bin_libs_ := $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)platform$(_lib_ext_)
#
_bin_libs_ :=\
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)dbg_trace_xsal$(_lib_ext_) \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)xsal$(_lib_ext_)          \

  
ifeq ($(_flavor_),linux_all)
  _bin_libs_ += pthread.so rt.so
endif

ifeq ($(_flavor_),win32_gcc)
  _bin_libs_ += WS2_32
endif

#
# Define the list of directories to search for libraries (in the order in which
# they are to be searched). These are for libraries that were not specified
# with a path in _bin_libs_ above:
#
# EXAMPLE: _bin_lib_dirs_ := <path to library>
#
_bin_lib_dirs_ := 
#
# If appropriate, define the linker command file for the binary.
#
# EXAMPLE: _bin_lnk_cmd_ := $(_cur_module_path_)/$(_flavor_)_cmd.lnk
#
_bin_lnk_cmd_ :=

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

#
# If the #ifeq at the beginning of the file was used to only build this module
# for certain flavors, the uncomment the "endif".
#
#####
endif # ifeq ($(XSAL_$(_build_)_TYPE),XSAL)
#####

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 19-Jul-2013 Larry Piekarski Rev 3
# + Task 161388: Tip off to new version bbc_util.
#
# 24-Sep-2012 Larry Piekarski Rev 2
# + Task 120466: Added the ability to add a suffix to gen_xsal_msg_list by
#   setting GEN_XSAL_MSG_LIST_$(_build_)_SUFFIX in Makefile to "_something".
#
# 18-Aug-2011 Dan Carman
# + Created initial file.
#
#===========================================================================
