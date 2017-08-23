#===========================================================================
# FILE: Makefile_procman.mk
#===========================================================================
# Copyright 2007-2013 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:Makefile_procman.mk~kok_basa#16:makefile:kok_aud#1 %
# %version:kok_basa#16 %
# %derived_by:jz4fpf %
# %date_modified:Fri Apr 17 14:55:39 2015 %
#---------------------------------------------------------------------------
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
#===========================================================================
_flavor_ := $($(_build_)_FLAVOR)

#
# Only build this module for FULL XSAL!
#
ifeq ($(XSAL_$(_build_)_TYPE),XSAL)

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
_src_dirs_ := $(_cur_module_path_)/source $(_cur_module_path_)/procman_h

ifneq ($(filter linux% qnx_%,$(_flavor_)),)
_src_dirs_ += $(_cur_module_path_)/source/posix
endif

###############################################################################
# Definitions for linking object code into a static library:
############################################################

#
# Define the name of the library; e.g., "foo" for libfoo.a.
#
# EXAMPLE: _lib_name_ := test_platform
#         for $(_lib_prefix_)test_platform$(_lib_ext_)
#
_lib_name_ :=
#
# Define the list of object files that go into the static library.
#
# EXAMPLE: _lib_objs_ := $($(_build_)_OBJ_OUTDIR)/filename$(_obj_ext_)
#
_lib_objs_ :=

###############################################################################
# Definitions for generating an executable from object files and libraries:
###########################################################################

#
# Define the name of the executable:
#
# EXAMPLE: _bin_name_ := my_prog$(_bin_ext_)
#
_bin_name_ := procman$(_bin_ext_)
#
# Define the list of object files for the executable:
#
# EXAMPLE: _bin_objs_ := $($(_build_)_OBJ_OUTDIR)/my_prog$(_obj_ext_)
#
_bin_objs_ := \
    $($(_build_)_OBJ_OUTDIR)/pm_main$(_obj_ext_)        \
    $($(_build_)_OBJ_OUTDIR)/pm_main_thread$(_obj_ext_) \
    $($(_build_)_OBJ_OUTDIR)/pm_parser$(_obj_ext_)      \
    $($(_build_)_OBJ_OUTDIR)/pm_process$(_obj_ext_)     \
    $($(_build_)_OBJ_OUTDIR)/pm_queue$(_obj_ext_)	\
    $($(_build_)_OBJ_OUTDIR)/procman_callouts$(_obj_ext_) \
    $($(_build_)_OBJ_OUTDIR)/pm_os_native_thread$(_obj_ext_) \

ifneq ($(filter linux% qnx_%,$(_flavor_)),)
  _bin_objs_ += $($(_build_)_OBJ_OUTDIR)/pm_exit_monitor$(_obj_ext_)
endif

ifneq ($(filter qnx%,$(_flavor_)),)
  _bin_objs_ += $($(_build_)_OBJ_OUTDIR)/process_observer$(_obj_ext_)
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
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)dbg_trace_xsal$(_lib_ext_)     \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)double_linked_list$(_lib_ext_) \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)pi$(_lib_ext_)                 \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)string_resource$(_lib_ext_)    \
 
ifeq ($(USE_ISA_CORE),y)  
_bin_libs_ += \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)XSAL$(_lib_ext_) \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)DebugTrace$(_lib_ext_) \
  $($(_build_)_LIB_OUTDIR)/libTSD.a \
  
else
_bin_libs_ += \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)xsal$(_lib_ext_) \
 
endif 

ifneq ($(filter win32_%,$(_flavor_)),)
  _bin_libs_ += WS2_32
endif

ifneq ($(filter linux%,$(_flavor_)),)
_bin_libs_ += \
  	pthread.so \
  	rt.so \

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
# EXAMPLE: _bin_lnk_cmd_ := $(_cur_module_path_)/$(_flavor_).lnk
#
_bin_lnk_cmd_ :=

#
# The following allows a file outside the block to be included and to have it
# modify the way procman is built. This may be necessary if the currently
# linked against libraries are refactored (such an activity for bb_dbg_trace is
# what lead to this change).
#
ifdef procman_EXTRA_BUILD_RULES
   include $(procman_EXTRA_BUILD_RULES)
endif

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

$(_c_objs_): CC_$(_build_)_OPT := -O0

ifneq ($(filter win32_%,$(_flavor_)),)
$(_c_objs_): CC_$(_build_)_WARN := -Wall
endif

ifneq ($(filter qnx_%,$(_flavor_)),)
# $(_c_objs_): CC_$(_build_)_WARN := -w8
endif

endif # ifeq ($(XSAL_$(_build_)_TYPE),XSAL)

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 19-Jul-2013 Larry Piekarski Rev 14
# + Task 161388: Tip off to new version bbc_util.
#
# 21-Mar-2013 Kirk Bailey
# + Added link against libdbg_trace_xsal.a now that libdbg_trace has been
# + de-coupled from XSAL.
#
# 05-Mar-2013 Kirk Bailey
# + Added include of $(procman_EXTRA_BUILD_RULES) to support ability to
# + modify link commands outside block without affecting current build.
#
# 24-jan-2011 kirk bailey
# + Added link to utilities library.
#
# 24-jun-2010 kirk bailey
# + Changed Linux build to make sure it specifies shared verions of pthread
# + and rt libraries.
#
# 29-oct-2007 kirk bailey
# + Renamed main to pm_main.
#
# 27-aug-2007 kirk bailey
# + Created initial file.
#
#===========================================================================
