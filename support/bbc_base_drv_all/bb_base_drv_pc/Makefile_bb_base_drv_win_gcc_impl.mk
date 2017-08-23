#===========================================================================
# FILE: Makefile_bb_base_drv_win_gcc_impl.mk
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: Makefile_bb_base_drv_win_gcc_impl.mk~kok_basa#3:makefile:kok_aud#1 %
# %version: kok_basa#3 %
# %derived_by: jz4fpf %
# %date_modified: Tue Nov 16 08:40:54 2010 %
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

##############################
# Only build for win32!
ifeq ($(_flavor_),win32_gcc)
##############################

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
_src_dirs_ := $(_cur_module_path_)/bb_base_drv_win_gcc_imp/_src

###############################################################################
# Definitions for linking object code into a static library:
############################################################

#
# Define the name of the library; e.g., "foo" for libfoo.a.
#
# EXAMPLE: _lib_name_ := test_platform # for libtest_platform.a
#
_lib_name_ := base_drivers
#
# Define the list of object files that go into the static library.
#
# EXAMPLE: _lib_objs_ := $($(_build_)_OBJ_OUTDIR)/filename$(_obj_ext_)
#
_lib_objs_ := \
    $($(_build_)_OBJ_OUTDIR)/dio_win32$(_obj_ext_)\
    $($(_build_)_OBJ_OUTDIR)/free_run_timer_win32$(_obj_ext_)\
    $($(_build_)_OBJ_OUTDIR)/iic_win32$(_obj_ext_)\
    $($(_build_)_OBJ_OUTDIR)/interrupts_win32$(_obj_ext_) \
    $($(_build_)_OBJ_OUTDIR)/spi_win32$(_obj_ext_) \
    $($(_build_)_OBJ_OUTDIR)/uart_win32$(_obj_ext_) \
    $($(_build_)_OBJ_OUTDIR)/watchdog_win32$(_obj_ext_)

###############################################################################
# Definitions for generating an executable from object files and libraries:
###########################################################################

#
# Define the name of the executable:
#
# EXAMPLE: _bin_name_ := my_prog.exe
#
_bin_name_ :=
#
# Define the list of object files for the executable:
#
# EXAMPLE: _bin_objs_ := $($(_build_)_OBJ_OUTDIR)/my_prog$(_obj_ext_)
#
_bin_objs_ :=
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
# EXAMPLE: _bin_libs_ := $($(_build_)_LIB_OUTDIR)/libtest_platform.a
#
_bin_libs_ :=
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
# EXAMPLE: _bin_lnk_cmd_ := $(_cur_module_path_)/ose_rtos_services.xcl
#
_bin_lnk_cmd_ :=

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

#####
endif
#####

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        userid    (Description on following lines: SCR #, etc.)
# ----------- --------
# 26-mar-2007 kirk bailey
# + Got rid of _incl_list_ for new build strategy.
#
# 15-Feb-2007 Dan Carman
# + Created initial file.
#
#===========================================================================
