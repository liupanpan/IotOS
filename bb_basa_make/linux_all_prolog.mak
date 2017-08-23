#===========================================================================
# FILE: linux_all_prolog.mak
#===========================================================================
# Copyright 2010 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: linux_all_prolog.mak~2:makefile:kok_basa#1 %
# %version: 2 %
# %derived_by: dzq92s %
# %date_modified: Fri Jun 25 09:08:18 2010 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================

include $(MAKE_INCL_DIR)/prolog.mak

$(_flavor_)_SUPPORTS_SHARED_LIBS := y

#
# File extensions for tools on linux.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .asm
_bin_ext_ :=
ifndef LINUX_LIB_EXT_$(_build_)
  _lib_ext_ := .a     # default library type
else
  _lib_ext_ := $(LINUX_LIB_EXT_$(_build_))
endif  # LINUX_LIB_EXT_$(_build_)
_lib_prefix_ := lib
_obj_ext_ := .o

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 21-jun-2010 kirk bailey
# + Added $(_flavor_)_SUPPORTS_SHARED_LIBS.
#
# 24-may-2010 kirk bailey
# + Created initial file.
#
#===========================================================================
