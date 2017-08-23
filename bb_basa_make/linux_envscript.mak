#===========================================================================
# FILE: linux_envscript.mak
#===========================================================================
# Copyright 2013 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: linux_envscript.mak~1:ascii:kok_inf#1 %
# %version: 1 %
# %derived_by: hzgbb0 %
# %date_modified:  Wed Nov 20 14:45:00 2013 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file is used to customize the Linux cross development tool
# settings by extracting information from a shell script.
#
# The are several environment variables like CC and CFLAGS that are commonly
# used to control cross-compilation within make-like build systems.
# There are 2 issues with this:
#   1. The cross-compiler environment has to be set up before running make
#   2. bb_basa_make may want to build several flavors back-to-back, each with
#      its own compiler settings
#
# This .mak script solves these problems by running a pre-determined script
# to set up the environment. This script is unique for each build, so the
# environment it sets up will not affect other builds.
#
# UPSTREAM VARIABLES:
# -------------------
# This script is affected by the following variables that can be set in the
# top-level Makefile:
# - GCC_$(_build_)_ENV_SCRIPT
#   - Specifies the script that will be executed before extracting the
#     environment parameters.
# - GCC_$(_build_)_SYSROOT
#   - Specifies a substitute sysroot that will replace any sysroot parameters
#     found in the environment variables.
#   - See NOTE1 below.
# - GCC_$(_build_)_PATH
#   - Specifies the path to the cross-compiler tools. Normally, this path will
#     be automatically detected, so this only needs to be defined in extreme 
#     cases where the automatic detection falls short.
# - VERBOSE_MAKE
#   - Turns on logging of the output variables.
#
# ENVIRONMENT VARIABLES:
# ----------------------
# This script looks for the following variables in the environment:
# - TARGET_PREFIX
#   - Specifies the cross-compiler tool prefix. This is usually in the form of
#     a GNU target triplet, which is a 3-part string with the form:
#     "cpu-vendor-os".
#   - For example, it could be "arm-fsl-linux-gnueabi-" for LTIB or
#     "arm-poky-linux-gnueabi-" for Yocto.
#   - The extra dash at the end is important.
# - AR, CC, CXX
#   - These variables specify the name for the archiver, C compiler, and C++
#     compiler respectively.
# - CFLAGS, CXXFLAGS, LDFLAGS
#   - These variables specify additional flags to be used with the C compiler,
#     C++ compiler, and linker respectively.
# - PATH
#   - The PATH environment is used to find the TOOLCHAIN PATH, which will be
#     prepended to each of the cross-compiler commands.
#
# OUTPUT VARIABLES:
# ----------------------
# This script sets the following make variables that will control the rest of
# the build process:
# - AR_$(_build_)
#   - The full path to the archiver.
# - CC_$(_build_), AS_$(_build_), LD_$(_build_)
#   - The full path to the combined C compiler-assembler-linker.
# - CPP_$(_build_)
#   - The full path to the C++ compiler.
# - TOOLCHAIN_$(_build_)_CFLAGS
#   - Specify additional flags to be used with the C compiler.
# - TOOLCHAIN_$(_build_)_CXXFLAGS
#   - Specify additional flags to be used with the C++ compiler.
# - TOOLCHAIN_$(_build_)_LDFLAGS
#   - Specify additional flags to be used with the linker.
#
# NOTE1 - About sysroot
# ---------------------
# The sysroot (and isysroot) options tell gcc where to find the system include
# headers and libraries.
# The environment script probably added a --sysroot= parameter to the CFLAGS 
# environment variable in order to point the compiler at the cross-compiled
# versions of necessary system libraries like ld.so and libc.so.
#
# However, it can be convenient to point sysroot at a target sysroot of our
# choosing.
# For example, the environment script may point at a stripped-down sysroot with
# only basic libc/glib header files and libraries and we want to point at a
# target sysroot that we have populated with a complete set of
# packages.
#
# The GCC_$(_build_)_SYSROOT variable can be defined in the top-level Makefile
# and its value will be substituted into the environment below.
#===========================================================================

###############################################################################
# Execute the Custom Environment Script:
#
# The script will be executed and relavent variables will be extracted.
# It may make sense to define the script as an empty string if the necessary
# environment is already set before running make.
#

#
# Check that the script is non-empty before executing it.
#
ifneq ($(strip $(GCC_$(_build_)_ENV_SCRIPT)),)
  #
  # We extract the needed environmental parameters in a single shot so that the
  # script is only executed once (in case it has side-effects).
  #
  $(_build_)_CUSTOM_ENVIRONMENT :=\
    $(shell . $(GCC_$(_build_)_ENV_SCRIPT) > /dev/null;\
      echo \
        TOOLCHAIN_PREFIX=\"$$TARGET_PREFIX\"\
        AR=\"$$AR\"\
        CC=\"$$CC\"\
        CXX=\"$$CXX\"\
        CFLAGS=\"$$CFLAGS\"\
        CXXFLAGS=\"$$CXXFLAGS\"\
        LDFLAGS=\"$$LDFLAGS\"\
        TOOLCHAIN_PATH=\"`dirname \`which $${TARGET_PREFIX}gcc\``\"\
    )
endif

#
# Convert the environment variables into individual make variables.
#
TOOLCHAIN_$(_build_)_PREFIX :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv TOOLCHAIN_PREFIX)
AR_$(_build_) :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv AR)
CC_$(_build_) :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv CC)
CPP_$(_build_) :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv CXX)
TOOLCHAIN_$(_build_)_CFLAGS :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv CFLAGS)
TOOLCHAIN_$(_build_)_CXXFLAGS :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv CXXFLAGS)
TOOLCHAIN_$(_build_)_LDFLAGS :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv LDFLAGS)
TOOLCHAIN_$(_build_)_PATH :=\
  $(shell $($(_build_)_CUSTOM_ENVIRONMENT) printenv TOOLCHAIN_PATH)

###############################################################################
# Substitute the sysroot:
#
# Replace any predefined sysroot with the custom sysroot if it was defined in
# the top-level Makefile.
# Future improvement: allow definition of separate isysroot.
#
ifdef GCC_$(_build_)_SYSROOT
  # --sysroot= should really only show up in the various FLAGS parameters,
  # but the Yocto environment includes it as part of the CC/CPP variable.
  AR_$(_build_) := $(patsubst --sysroot=%,--sysroot=$(GCC_$(_build_)_SYSROOT),$(AR_$(_build_)))
  CC_$(_build_) := $(patsubst --sysroot=%,--sysroot=$(GCC_$(_build_)_SYSROOT),$(CC_$(_build_)))
  CPP_$(_build_) := $(patsubst --sysroot=%,--sysroot=$(GCC_$(_build_)_SYSROOT),$(CPP_$(_build_)))
  TOOLCHAIN_$(_build_)_CFLAGS := $(patsubst --sysroot=%,--sysroot=$(GCC_$(_build_)_SYSROOT),$(TOOLCHAIN_$(_build_)_CFLAGS))
  TOOLCHAIN_$(_build_)_CXXFLAGS := $(patsubst --sysroot=%,--sysroot=$(GCC_$(_build_)_SYSROOT),$(TOOLCHAIN_$(_build_)_CXXFLAGS))
  TOOLCHAIN_$(_build_)_LDFLAGS := $(patsubst --sysroot=%,--sysroot=$(GCC_$(_build_)_SYSROOT),$(TOOLCHAIN_$(_build_)_LDFLAGS))
  AR_$(_build_) := $(patsubst --isysroot=%,--isysroot=$(GCC_$(_build_)_SYSROOT),$(AR_$(_build_)))
  CC_$(_build_) := $(patsubst --isysroot=%,--isysroot=$(GCC_$(_build_)_SYSROOT),$(CC_$(_build_)))
  CPP_$(_build_) := $(patsubst --isysroot=%,--isysroot=$(GCC_$(_build_)_SYSROOT),$(CPP_$(_build_)))
  TOOLCHAIN_$(_build_)_CFLAGS := $(patsubst --isysroot=%,--isysroot=$(GCC_$(_build_)_SYSROOT),$(TOOLCHAIN_$(_build_)_CFLAGS))
  TOOLCHAIN_$(_build_)_CXXFLAGS := $(patsubst --isysroot=%,--isysroot=$(GCC_$(_build_)_SYSROOT),$(TOOLCHAIN_$(_build_)_CXXFLAGS))
  TOOLCHAIN_$(_build_)_LDFLAGS := $(patsubst --isysroot=%,--isysroot=$(GCC_$(_build_)_SYSROOT),$(TOOLCHAIN_$(_build_)_LDFLAGS))
endif

###############################################################################
# Bild up tool paths from the custom environment variables:
#
# Tool paths will be prepended with the TOOLCHAIN PATH variable that was 
# extracted from the environment script.
# This is necessary because the tools are unlikely to be in the PATH when the
# compilation/link steps occur.
#
# To prevent this path from being prepended, simply define GCC_$(_build_)_PATH
# as an empty string in your top-level Makefile.
#
ifneq ($(strip $(TOOLCHAIN_$(_build_)_PATH)),)
  ifndef GCC_$(_build_)_PATH
    GCC_$(_build_)_PATH := $(TOOLCHAIN_$(_build_)_PATH)
  endif
endif

#
# It is possible the env-script did not define some of the cross-compiler tools.
# However, we can recreate them if the TOOLCHAIN PREFIX is defined.
#
ifeq ($(strip $(AR_$(_build_))),)
  AR_$(_build_)  := $(TOOLCHAIN_$(_build_)_PREFIX)ar
endif
ifeq ($(strip $(CC_$(_build_))),)
  CC_$(_build_)  := $(TOOLCHAIN_$(_build_)_PREFIX)gcc
endif
ifeq ($(strip $(CPP_$(_build_))),)
  CPP_$(_build_) := $(TOOLCHAIN_$(_build_)_PREFIX)g++
endif

#
# Prepend the full path.
#
AR_$(_build_)  := $(GCC_$(_build_)_PATH)/$(AR_$(_build_))
CC_$(_build_)  := $(GCC_$(_build_)_PATH)/$(CC_$(_build_))
CPP_$(_build_) := $(GCC_$(_build_)_PATH)/$(CPP_$(_build_))
AS_$(_build_) := $(CC_$(_build_))
LD_$(_build_) := $(CC_$(_build_))

###############################################################################
# Log the final state of the variables:
#
ifdef VERBOSE_MAKE
  $(info )
  $(info Environment Extracted for $(_build_):)
  $(info $($(_build_)_CUSTOM_ENVIRONMENT))
  $(info )
  $(info TOOLCHAIN_$(_build_)_PREFIX   = $(TOOLCHAIN_$(_build_)_PREFIX))
  $(info TOOLCHAIN_$(_build_)_PATH     = $(TOOLCHAIN_$(_build_)_PATH))
  $(info TOOLCHAIN_$(_build_)_CFLAGS   = $(TOOLCHAIN_$(_build_)_CFLAGS))
  $(info TOOLCHAIN_$(_build_)_CXXFLAGS = $(TOOLCHAIN_$(_build_)_CXXFLAGS))
  $(info TOOLCHAIN_$(_build_)_LDFLAGS  = $(TOOLCHAIN_$(_build_)_LDFLAGS))
  $(info AR_$(_build_)  = $(AR_$(_build_)))
  $(info AS_$(_build_)  = $(AS_$(_build_)))
  $(info CC_$(_build_)  = $(CC_$(_build_)))
  $(info CPP_$(_build_) = $(CPP_$(_build_)))
  $(info LD_$(_build_)  = $(LD_$(_build_)))
  $(info )
endif

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 20-Nov-2013 Chris Baker Rev 1
# + Task 3052: Created initial file.
#
#===========================================================================
