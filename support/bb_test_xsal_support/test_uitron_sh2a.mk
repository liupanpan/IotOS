#===========================================================================
# FILE: test_uitron_sh2a.mk
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:test_uitron_sh2a.mk~4:makefile:kok_aud#1 %
# %version:4 %
# %derived_by:dzq92s %
# %date_modified:Tue Aug 19 15:23:46 2008 %
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
###_flavor_ := $($(_build_)_FLAVOR)

###include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

###############################################################################
# Definitions for compiling/assembling source code into object code:
####################################################################

#
# Define the directory (relative to project root directory) containing the
# source files for this module.
#
# EXAMPLE: _src_dirs_ := $(_cur_module_path_)/<implementation directory>/_src
#
_src_dirs_ := $(_cur_module_path_)/common_src\
              $(_cur_module_path_)/test_uitron_sh2a_src

###############################################################################
# Definitions for linking object code into a static library:
############################################################

#
# Define the name of the library; e.g., "foo" for libfoo.a.
#
# EXAMPLE: _lib_name_ := test_platform # for libtest_platform.a
#
_lib_name_ := test_support
#
# Define the list of object files that go into the static library.
#
# EXAMPLE: _lib_objs_ := $($(_build_)_OBJ_OUTDIR)/filename$(_obj_ext_)
#
_lib_objs_ := $($(_build_)_OBJ_OUTDIR)/app_stub$(_obj_ext_)          \
              $($(_build_)_OBJ_OUTDIR)/em_stub$(_obj_ext_)           \
              $($(_build_)_OBJ_OUTDIR)/init_drv$(_obj_ext_)          \
              $($(_build_)_OBJ_OUTDIR)/init_task$(_obj_ext_)         \
              $($(_build_)_OBJ_OUTDIR)/printf_gate$(_obj_ext_)

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
###############################################################################

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak


#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 05-jun-2008 kirk bailey
# + Rename "icbk" to "cbk" to follow BASA conventions.
#
# 25-apr-2007 kirk bailey
# + Removed object files from _bin_objs_ that are now in drv_7263_ren library.
#
# 16-apr-2007 kirk bailey
# + Moved LED toggle logic to r0k57263_led.
#
# 27-Feb-2007 Dan Carman
# + Added include of additional make file for additional test executables
#
# 23-feb-2007 kirk bailey
# + Changed contents of demo library to be parts to be reused for other
# + test apps. Renamed test app.
#
# 20-feb-2007 kirk bailey
# + Renamed from Changed main to test
#
# 7 Feb 2007 Dan Carman
# + Changed main to test
#
# 1 Feb 2007 Dan Carman
# + Eliminated Interrupt module with change to implementation of XSAL timer module
#
# 31-jan-2007 dzq92s
# + Added build name to end of executable name in order to have unique
# + names for different builds inside HEW.
#
# 19-nov-2006 kirk bailey
# + Created initial file.
#===========================================================================
