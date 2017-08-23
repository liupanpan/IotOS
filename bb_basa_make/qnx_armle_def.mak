#===========================================================================
# FILE: qnx_armle_def.mak
#===========================================================================
# Copyright 2010-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: qnx_armle_def.mak~kok_inf#6:ascii:kok_basa#1 %
# %version: kok_inf#6 %
# %derived_by: dzq92s %
# %date_modified: Tue Oct  7 15:09:07 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================
#
# Define QCC_CPU_VARIANT_$(_build_) for this build and include the
# common QNX definitions to create the build commands for this QNX target.
#
# select cpp lib version for targets on Momentics >= 6.2.1
#
ifdef USE_LIB_GPP_$(_build_)
  CPPLIB_VERSION     := _gpp
else
  ifdef USE_LIB_CPPNE_$(_build_)
    CPPLIB_VERSION   := _cpp-ne
  else
    ifdef USE_LIB_CPP_$(_build_)
      CPPLIB_VERSION := _cpp
    else
      CPPLIB_VERSION := _gpp
    endif
  endif
endif

PROCESSOR_$(_build_) := armv7le
ifndef GCC_VERSION_$(_build_)
    COMPILER_VERSION :=
else
    COMPILER_VERSION := $(GCC_VERSION_$(_build_)),
endif # GCC_VERSION_$(_build_)

QCC_CPU_VARIANT_$(_build_) := $(COMPILER_VERSION)gcc_nto$(PROCESSOR_$(_build_))$(CPPLIB_VERSION)

ifndef QCC_ROOT
  QCC_ROOT := /opt/qnx660
endif

ifndef QNX_HOST
  export QNX_HOST := $(QCC_ROOT)/host/linux/x86
endif

ifndef QNX_TARGET
  export QNX_TARGET := $(QCC_ROOT)/target/qnx6
endif

ifndef QCC_TOOL_PATH
  export QCC_TOOL_PATH := $(QNX_HOST)/usr/bin
endif

NO_STRIP := 1

include $(MAKE_INCL_DIR)/qnx_def.mak

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 08-Oct-2014 Kirk Bailey
# + Brought in recent changes to support QNX 6.6 platform.
#
#===========================================================================
