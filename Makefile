#===========================================================================
# FILE: Makefile
#===========================================================================
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:Makefile~kok_basa#20:makefile:kok_aud#107 %
# %version:kok_basa#20 %
# %derived_by:jz4fpf %
# %date_modified:Tue Apr 14 15:17:57 2015 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This is the root makefile for the MDF.
#
#===========================================================================

###############################################################################
# Build Definition
##################

#
# BUILD_LIST must contain a non-empty list of identifiers for builds
# to be performed. These identifiers are used to place generated files into
# separate areas.
#
BUILD_LIST := pc pc_light linux_host linux_light_host linux_imx qnx_arm qnx_x86
#
# For each of the builds specified in BUILD_LIST, a variable named
# <build>_FLAVOR must be created and its value must be the name of the
# "flavor" associated with the build. The "flavor" identifies the tools
# used for the build and the definitions for these tools must be found
# in the MAKE_INCL_DIR within files named <flavor>_def.mak, 
# <flavor>_epilog.mak, and <flavor>_prolog.mak.
#
linux_host_FLAVOR := linux_all
linux_imx_FLAVOR  := linux_all
linux_light_host_FLAVOR := linux_all
linux_light_imx_FLAVOR  := linux_all
pc_FLAVOR := win32_gcc
pc_light_FLAVOR := win32_gcc
qnx_arm_FLAVOR := qnx_armle
qnx_x86_FLAVOR := qnx_x86
v850_FLAVOR := 200c_necv850

###############################################################################
# Project Configuration
########################
#
# PROJECT_CFG_ROOT is optional. If used it contains a list of paths to
# subdirectories that contain configuration header files. For each directory
# path "<dirpath>" in the list, the include paths "<dirpath>/<build>" and
# "<dirpath>" are added to the default compiler include paths (in that order).
# This means that configuration header files will first be taken from the
# build-specific path, then from the "common" path.
#
PROJECT_CFG_ROOT := cfg
#
# MAKE_INCL_DIR is the name of the subdirectory containing the reusable make
# rules that analyze the project's contents and generate the project-specific
# rules.
#
MAKE_INCL_DIR := bb_basa_make

###############################################################################
# Tool Options
########################
#
# Any definitions to override the tool options for the builds goes here.
#

###############################################################################
# Makefile Selection
#####################
#
# Any subdirectory containing a file named "Makefile*.mk" is called a "module".
# By default, any module with only one Makefile*.mk in it is automatically
# associated with each build; no definition is required to cause this to
# happen. 
#
# If a module has more than one Makefile*.mk, then it is required that
# a variable whose name is of the form "<build>-<modulename>" be assigned
# the name of the Makefile to be used; otherwise, an error will be reported.
# The name of the module (<modulename>) is the name of the leaf directory in
# the path to the module. All such leaf names are required to be unique within
# the project so they can uniquely identify the module.
#
# The value "NONE" (without quotes) can be assigned to the variable to exclude
# the module from the build. This can also be done if there is only one
# Makefile in the module. This is useful for modules containing hardware-
# specific code to exclude them from builds not associated with their
# hardware; e.g., PC unit test.
#
###pc-bb_basa_mdf_generator := NONE

###############################################################################


XSAL_linux_host_TYPE := XSAL
XSAL_linux_imx_TYPE  := XSAL
XSAL_linux_light_host_TYPE := XSAL_LIGHT
XSAL_linux_light_imx_TYPE  := XSAL_LIGHT
XSAL_pc_TYPE := XSAL
XSAL_pc_light_TYPE := XSAL_LIGHT
XSAL_qnx_arm_TYPE  := XSAL
XSAL_qnx_x86_TYPE  := XSAL

# Create gen_xsal_msg_list_test for linux_host build to exercise the suffix ability
GEN_XSAL_MSG_LIST_linux_host_SUFFIX := _test

PRE_CLEAN = clean-ti

# use host tools from PATH for linux_host build:
linux_host_USES_PATH       := y
linux_light_host_USES_PATH := y
# use freescale cross compiler for linux_imx build:
GCC_linux_imx_PATH       := /opt/freescale/ltib/usr/spoof/
GCC_linux_light_imx_PATH := /opt/freescale/ltib/usr/spoof/

#
# ... and then let the reusable project rules do the rest
#
include $(MAKE_INCL_DIR)/project.mak

clean-ti:
	$(RM) -rf $(PROJECT_OUTPUT_ROOT)/ti

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
#
# 16-Mar-2015 Dan Carman
# + Task 21687: Removed support for j2 / esol and PPC builds
#
# 24-Sep-2012 Larry Piekarski Rev 18
# + Task 120466: Added a suffix of "_test" to gen_xsal_msg_list for the linux
#   host build to test the suffix change.
#
# 15-sep-2010 kirk bailey
# + Renamed pc_cunit_bullseye to pc_light since CUnit not used.
#
# 27-jun-2010 kirk bailey
# + Added a linux build for XSAL Light.
#
# 30-may-2010 kirk bailey
# + Added linux build. Added clean extension for ti code.
#
# 04-feb-2010 kirk bailey
# + Removed pc_light from BUILD_LIST since it's identical to pc_cunit_bullseye.
#
# 01-aug-2008 kirk bailey
# + Added j2 build.
#
# 22-aug-2007 kirk bailey
# + Created file.
#
#===========================================================================
