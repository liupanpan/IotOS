#===========================================================================
# FILE: qnx_x86_prolog.mak
#===========================================================================
# Copyright 2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: qnx_x86_prolog.mak~1:ascii:kok_inf#1 %
# %version: 1 %
# %derived_by: dzq92s %
# %date_modified: Tue Oct  7 15:08:58 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================

ifndef $(_build_)_ONLY_ONCE
  #
  # QNX builds add an "image" output directory to those created by prolog.mak
  #
  $(_build_)_IMG_OUTDIR := $($(_build_)_OUTPUT_ROOT)/image
  X_IGNORE := $(shell $(MKDIR) -p $($(_build_)_IMG_OUTDIR))
  $($(_build_)_IMG_OUTDIR)/%:BUILD:=$(_build_)
  qnx_x86_INC_PATH += $(PROJECT_OUTPUT_ROOT)/include/qnx

endif # ifndef $(_build_)_ONLY_ONCE

include $(MAKE_INCL_DIR)/prolog.mak

$(_flavor_)_SUPPORTS_SHARED_LIBS := y
qnx_SUPPORTS_SHARED_LIBS := y

#
# File extensions for QNX qcc tools on WIN32.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .s
_bin_ext_ :=
ifndef QNX_LIB_EXT_$(_build_)
  _lib_ext_ := .a     # default library type
else
  _lib_ext_ := $(QNX_LIB_EXT_$(_build_))
endif  # QNX_LIB_EXT_$(_build_)
_lib_prefix_ := lib
_obj_ext_ := .o

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 03-Oct-2014 Kirk Bailey
# + Created initial file.
#===========================================================================
