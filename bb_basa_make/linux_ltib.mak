#===========================================================================
# FILE: linux_ltib.mak
#===========================================================================
# Copyright 2011 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: linux_ltib.mak~2:ascii:kok_basa#1 %
# %version: 2 %
# %derived_by: dzq92s %
# %date_modified: Tue Sep 13 14:01:00 2011 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file is used to customize the Linux cross development tool
# settings by extracting information from the ltib installation.
#
# The environment variables are modified to contain the necessary
# information that is extracted from ltib. The description of the
# ltib build process that follows higlights the environment variables
# that are important.
#
# Explanation of ltib build process
# ---------------------------------
# ltib simultaneously supports multiple tool installations for different
# boards and also supports the selection of different tools for the
# same board. It does this while presenting the same mechanism for
# invoking the cross-development tools. This is accomplished through
# a "spoof" mechanism: all of the cross-development tools (for example
# gcc) are invoked at /opt/freescale/ltib/usr/spoof. The contents of
# this directory are links to the spoof_wrapper script contained in that
# directory. This means that the spoof_wrapper is actually executed for
# each cross-development tool invocation.
# 
# The spoof_wrapper uses environment variables to redirect the invocation
# to the toolchain that is selected via environment variables. The
# environment variables of importances are:
# 
#   DEV_IMAGE - this points to the image created for the device
#       (usually /home/user/LTIB/rootfs). This is important for locating
#       system header files and libraries associated with the tool set.
#       It is used by the spoof_wrapper to form part of the "extra"
#       options passed to the compiler.
#               
#   TOOLCHAIN_PATH - this is the path where the cross-development tools are
#       installed. Note that the bin subdirectory at this path must be in the
#       PATH environment variable. The spoof logic does NOT use the path
#       as part of the command it issues; so, the command it issues will
#       not work unless PATH contains $(TOOLCHAIN_PATH)/bin. Also note
#       that PATH should contain only the current $(TOOLCHAIN_PATH)/bin
#       so that the command is not resolved by some other installed tool
#       chain that is not currently "selected" by the environment.
#                    
#   TOOLCHAIN_PREFIX - this is the prefix prepended to each command issued
#       by the spoof_wrapper; e.g., the "gcc" invocation becomes something
#       like "arm-none-linux-gnueabi-gcc". This prevents the cross-
#       development tools from "colliding" with the host tools when both
#       are in the PATH.
#       
#   TOOLCHAIN_CFLAGS - this is the set of CFLAGS passed to the compiler
#       by the spoof_wrapper. These options are automatically added to
#       the command issued by the spoof_wrapper. Any arguments passed to
#       the spoof_wrapper follows these. For example, if the gcc in the
#       spoof directory is invoked as "gcc -O0 -Wall", the value of
#       TOOLCHAIN_CFLAGS is "-O2 -fsigned-char -msoft-float", and the value
#       of TOOLCHAIN_PREFIX is "arm-none-linux-gnueabi-", then the command
#       issued by spoof_wrapper will be something like
#       "arm-none-linux-gnueabi-gcc -O2 -fsigned-char -msoft-float -O0 -Wall".
#       
#       Note that the above is not exactly correct since the spoof_wrapper
#       places other arguments on the front of the command having to do with
#       DEV_IMAGE. To see the actual command issued, define the environment
#       variable FS_DEBUG. With it defined, you will see a command issued
#       for the above example that looks something like
#       "arm-none-linux-gnueabi-gcc -B /home/user/LTIB/rootfs/usr/lib//
#       -idirafter /home/user/LTIB/rootfs/usr/include -iwithprefixbefore
#       include -O2 -fsigned-char -msoft-float -O0 -Wall".
#       
#       Also note that the "-O0" that appears later in the command line
#       takes precedence over the "-O2" that appears earlier because of
#       the way gcc handles arguments. 
#
#===========================================================================
ifndef LTIB_DIR
  LTIB_DIR:=/home/user/LTIB
endif

SPOOF_DIR := /opt/freescale/ltib/usr/spoof

#
# The ltib .config file contains an assignment for CONFIG_PLATFORM_DIR, which
# is the directory containing the configuration file for the selected platform.
#
LTIB_CONFIG_FILE:=$(wildcard $(LTIB_DIR)/.config)

ifeq ($(LTIB_CONFIG_FILE),)

  $(warning $(LTIB_DIR)/.config does not exist. ltib must be configured in order to select the cross development proper tool chain for Linux.)

else

  LTIB_PLATFORM_DIR := $(shell . $(LTIB_CONFIG_FILE); echo $$CONFIG_PLATFORM_DIR)
  LTIB_PLATFORM_CONFIG_FILE := $(wildcard $(LTIB_DIR)/$(LTIB_PLATFORM_DIR)/.config)

  ifeq ($(LTIB_PLATFORM_CONFIG_FILE),)
    $(error $(LTIB_PLATFORM_DIR)/.config does not exist. Cannot extract cross development tool information for selected platform.)
  endif

  ifeq ($(DEV_IMAGE),)
    $(warning DEV_IMAGE is not defined. Using default value of $(LTIB_DIR)/rootfs)
    export DEV_IMAGE := $(LTIB_DIR)/rootfs
  endif

  ifneq ($(GCC_$(_build_)_PATH),$(SPOOF_DIR)/)
    $(warning GCC_$(_build_)_PATH is set to $(GCC_$(_build_)_PATH),\
   not the value expected for ltib ($(SPOOF_DIR)/))
  endif

  #
  # Extract TOOLCHAIN_CFLAGS, TOOLCHAIN_PATH, and TOOLCHAIN_PREFIX from the
  # selected platform's configuration file. These are used by the ltib
  # spoof logic to direct the cross-compilation commands to the appropriate
  # tool chain installation.
  #
  export TOOLCHAIN_$(_build_)_CFLAGS :=\
    $(shell . $(LTIB_PLATFORM_CONFIG_FILE); echo $$CONFIG_TOOLCHAIN_CFLAGS)
  export TOOLCHAIN_PATH :=\
    $(shell . $(LTIB_PLATFORM_CONFIG_FILE); echo $$CONFIG_TOOLCHAIN_PATH)
  export TOOLCHAIN_PREFIX :=\
    $(shell . $(LTIB_PLATFORM_CONFIG_FILE); echo $$CONFIG_TOOLCHAIN_PREFIX)
  #
  # Append $(SPOOF_DIR) and $(TOOLCHAIN_PATH)/bin to the end
  # of PATH after first making sure any /opt/freescale paths are removed. The
  # removal is to make sure the TOOLCHAIN_PATH is used even if the default
  # PATH has a different one.
  #
  export PATH := $(shell echo $$PATH | sed -e 's/\:\/opt\/freescale[^\:]*//g')
  export PATH := $(PATH):$(SPOOF_DIR):$(TOOLCHAIN_PATH)/bin

endif # ifeq ($(LTIB_CONFIG_FILE),)

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 13-Sep-2011 kirk bailey
# + Removed show-ltib-env rule.
#
# 10-Aug-2011 kirk bailey
# + Created initial file.
#
#===========================================================================
