#==============================================================================
# FILE: Makefile_bb_base_drv_qnx_imp.mk
#
#------------------------------------------------------------------------------
#
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#
#------------------------------------------------------------------------------
#
# %full_filespec:Makefile_base_drv_qnx.mk~3:makefile:kok_inf#1 %
# %version:3 %
# %derived_by:jz4fpf %
# %date_modified:Wed May  6 19:03:10 2015 %
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
##############################
# Only build for qnx Flavor
ifneq ($(filter qnx%,$(_flavor_)),)
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
_src_dirs_ := $(_cur_module_path_)/bb_base_drv_qnx_imp/_src

###############################################################################
# Definitions for linking object code into a static library:
############################################################

#
# Define the name of the library; e.g., "foo" for libfoo.a.
#
# EXAMPLE: _lib_name_ := test_platform
#          for $(_lib_prefix_)test_platform$(_lib_ext_)
#
_lib_name_ := base_drivers_qnx
#
# Define the list of object files that go into the static library.
#
# EXAMPLE: _lib_objs_ := $($(_build_)_OBJ_OUTDIR)/filename$(_obj_ext_)
#
_lib_objs_ := \
    $($(_build_)_OBJ_OUTDIR)/i2c_qnx$(_obj_ext_)\
    $($(_build_)_OBJ_OUTDIR)/dio_qnx$(_obj_ext_)\
    $($(_build_)_OBJ_OUTDIR)/free_run_timer_qnx$(_obj_ext_)\
    $($(_build_)_OBJ_OUTDIR)/iic_qnx$(_obj_ext_)\
    $($(_build_)_OBJ_OUTDIR)/spi_qnx$(_obj_ext_) \
    $($(_build_)_OBJ_OUTDIR)/uart_qnx$(_obj_ext_) \
    
###############################################################################
# Definitions for generating an executable from object files and libraries:
###########################################################################

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

#
# If the #ifeq at the beginning of the file was used to only build this module
# for certain flavors, the uncomment the "endif".
#
#####
endif
#####

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
#
# - 5-Sept-2014 Dan Carman
#   Task 11694 - Update QNX base drivers to work with J6 and IMX6
#      Implementation specific GPIO function are conditional inlcuded into 
#       common base file (dio_qnx) so that we don't need confitional linking. 
#
# 26-Mar-2014 Dan Carman
# + Created initial file.
#
#===========================================================================
