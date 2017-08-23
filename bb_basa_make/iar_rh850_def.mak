#===========================================================================
# FILE: iar_rh850_def.mak
#===========================================================================
# Copyright 2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: iar_rh850_def.mak~1:ascii:kok_inf#1 %
# %version: 1 %
# %derived_by: dzq92s %
# %date_modified: Tue Apr 14 12:12:57 2015 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================

#
# The following environment variables are required for the Renesas tools
# to operate properly
#
#
# Env. var. can specify location of IAR tools to eliminate this warning
#
# NOTE: Can NOT have spaces in path (use "short" DOS names - see dir \X)
#

# If a Tool Path is not defined the default installation path of IAR System 
# will be used. 
ifndef RH850_TOOL_PATH
  $(warning ENVIRONMENT variable RH850_TOOL_PATH is not defined.\
  Using default value of C:\PROGRA~2\IARSYS~1\EMBEDD~1.0\rh850)
  RH850_TOOL_PATH := C:\PROGRA~2\IARSYS~1\EMBEDD~1.0\rh850
endif

# RH850_VARIANT specifies the linker option file to be used.
ifndef RH850_VARIANT
  $(error RH850_VARIANT is not defined.)
endif

# Change path format to a CYGWIN format.
RH850_TOOL_PATH := $(subst \,/,"$(RH850_TOOL_PATH)")

# Need a PATH usable by Cygwin shell for invoking the commands:
RH850_TOOL_PATH_CYG := $(shell $(CYGPATH) -u $(subst \,/,"$(RH850_TOOL_PATH)"))

# Path to tool's libraries:
ifndef RH850_LIB_PATH
  RH850_LIB_PATH := $(subst \,/,"$(RH850_TOOL_PATH)/lib")
endif

#
# IAR RH850 Tool Definitions
#
AR_$(_build_) := $(RH850_TOOL_PATH_CYG)/bin/iarchive.exe
AS_$(_build_) := $(RH850_TOOL_PATH_CYG)/bin/iasmrh850.exe
CC_$(_build_) := $(RH850_TOOL_PATH_CYG)/bin/iccrh850.exe
LD_$(_build_) := $(RH850_TOOL_PATH_CYG)/bin/ilinkrh850.exe
ELF_$(_build_):= $(RH850_TOOL_PATH_CYG)/bin/ielftool.exe

# Defining the CPU core for the flavor. 
ifndef CC_$(_build_)_CPU
  CC_$(_build_)_CPU := rh850
endif

###############################################################################
# Individual RH850 Tool Options
#
#   These are the defaults, but they can be independently overridden in the
#   root Makefile on a per-build basis.
#

###########
# Assembler
###########
# General default values.
ifndef AS_$(_build_)_BASE
  AS_$(_build_)_BASE := --code_model normal --data_model small
endif

# Generate debug information
ifndef AS_$(_build_)_DBG
  AS_$(_build_)_DBG := -r
endif

# Use "<" and ">" as delimiters for Macro.
ifndef AS_$(_build_)_MACRO
  AS_$(_build_)_MACRO := -M'<>'
endif

# Only display errors/warnings
ifndef AS_$(_build_)_SILENT
  ifndef VERBOSE_MAKE
    AS_$(_build_)_SILENT := --silent
  else
    AS_$(_build_)_SILENT :=
  endif
endif

# Includes to the assembling.
ifndef AS_$(_build_)_INC # NOTE: Recursive expansion!
  AS_$(_build_)_INC =\
    -I$(RH850_TOOL_PATH)/inc -I$(RH850_TOOL_PATH)/inc/c\
    -I$(RH850_LIB_PATH) $(patsubst %,-I%,$(INCL_DIRS))
endif


############
# C Compiler
############
# General default values.
ifndef CC_$(_build_)_GENERAL
  CC_$(_build_)_GENERAL := --data_model small --code_model normal --dlib_config full
endif

# C99 is used as default.
# Language conformance is Standar with IAR extensions.
ifndef CC_$(_build_)_LANG1
  CC_$(_build_)_LANG1 := -e
endif

# Floating-point semantics is relaxed (smaller and/or faster).
ifndef CC_$(_build_)_LANG2
  CC_$(_build_)_LANG2 := --relaxed_fp
endif

# Optimization is low.
ifndef CC_$(_build_)_OPT
  CC_$(_build_)_OPT := -Ol --no_cse --no_inline --no_code_motion --no_tbaa\
     --no_cross_call --no_scheduling --no_clustering --lock_regs 0 --no_unroll
endif

# Generate debug information.
ifndef CC_$(_build_)_OUT
  CC_$(_build_)_OUT := -r
endif

# List configuration.
ifndef CC_$(_build_)_LIST
  CC_$(_build_)_LIST := 
endif

# Macro defines.
ifndef CC_$(_build_)_MACRO
  CC_$(_build_)_MACRO := 
endif

# Diagnostics configuration.
ifndef CC_$(_build_)_DIAG
  CC_$(_build_)_DIAG := 
endif

# Only display errors/warnings
ifndef CC_$(_build_)_SILENT
  ifndef VERBOSE_MAKE
    CC_$(_build_)_SILENT := --silent# Only display errors/warnings
  else
    CC_$(_build_)_SILENT :=
  endif
endif

# Includes to the C compiling.
ifndef CC_$(_build_)_INC # NOTE: Recursive expansion!
  CC_$(_build_)_INC =\
    -I$(RH850_TOOL_PATH)/inc -I$(RH850_TOOL_PATH)/inc/c\
    -I$(RH850_LIB_PATH) $(patsubst %,-I%,$(INCL_DIRS))
endif

########
# Linker
########
# Basic configuration for Linker.
ifndef LD_$(_build_)_CONF
  LD_$(_build_)_CONF := --config_def CSTACK_SIZE=0x1000 --config_def HEAP_SIZE=4096
endif

# Including C-SPY debug library.
ifndef LD_$(_build_)_LIB
  LD_$(_build_)_LIB := --debug_lib
endif

# Treats __rh850_start as the start of the application.
ifndef LD_$(_build_)_ENTRY
  LD_$(_build_)_ENTRY := --entry __rh850_start
endif

# Input configuration to the linking.
ifndef LD_$(_build_)_INPUT
  LD_$(_build_)_INPUT := 
endif

# Optimization.
ifndef LD_$(_build_)_OPT
  LD_$(_build_)_OPT := --vfe
endif

# Debug information.
ifndef LD_$(_build_)_DBG
  LD_$(_build_)_DBG := 
endif

# List configuration.
ifndef LD_$(_build_)_LIST
  LD_$(_build_)_LIST := 
endif

# Macro defines.
ifndef LD_$(_build_)_MACRO
  LD_$(_build_)_MACRO := 
endif

# Diagnostics configuration.
ifndef LD_$(_build_)_DIAG
  LD_$(_build_)_DIAG := 
endif

# Checksum configuration. 
ifndef LD_$(_build_)_CHECK
  LD_$(_build_)_CHECK := 
endif

# Only display errors/warnings
ifndef LD_$(_build_)_SILENT
  ifndef VERBOSE_MAKE
    LD_$(_build_)_SILENT := --silent# Only display errors/warnings
  else
    LD_$(_build_)_SILENT :=
  endif
endif

###############################################################################
# Composite RH850 Tool Options:
#
#   These are the defaults, but they can be independently overridden in the
#   root Makefile on a per-build basis.
#

##########
# Archiver (generating a static library)
##########
ifndef AR_$(_build_)_FLAGS
  ifndef VERBOSE_MAKE
    AR_$(_build_)_FLAGS :=
  else
    AR_$(_build_)_FLAGS := -V
  endif
endif

###########
# Assembler
###########
ifndef AS_$(_build_)_FLAGS # NOTE: Recursive expansion!
  AS_$(_build_)_FLAGS =\
    $(AS_$(_build_)_INC) $(AS_$(_build_)_BASE) --core $(CC_$(_build_)_CPU)\
    $(AS_$(_build_)_DBG) $(AS_$(_build_)_MACRO) $(AS_$(_build_)_SILENT)
endif

############
# C Compiler
############
ifndef CC_$(_build_)_FLAGS # NOTE: Recursive expansion!
  CC_$(_build_)_FLAGS =\
   $(CC_$(_build_)_INC) $(CC_$(_build_)_LANG1) $(CC_$(_build_)_LANG2) --core $(CC_$(_build_)_CPU) $(CC_$(_build_)_OPT)\
   $(CC_$(_build_)_OUT) $(CC_$(_build_)_LIST) $(CC_$(_build_)_MACRO) $(CC_$(_build_)_DIAG)\
   $(CC_$(_build_)_SILENT) $(CC_$(_build_)_GENERAL)
endif

########
# Linker
########
ifndef LD_$(_build_)_FLAGS # NOTE: Recursive expansion!
  LD_$(_build_)_FLAGS =\
    $(LD_$(_build_)_CONF) $(LD_$(_build_)_LIB) $(LD_$(_build_)_ENTRY) $(LD_$(_build_)_INPUT)\
    $(LD_$(_build_)_OPT) $(LD_$(_build_)_DBG) $(LD_$(_build_)_LIST) $(LD_$(_build_)_MACRO)\
    $(LD_$(_build_)_DIAG) $(LD_$(_build_)_CHECK) $(LD_$(_build_)_SILENT)  
endif

###############################################################################
# Commands for assembling files:
#################################

ifndef AS_$(_build_)_CMD # NOTE: Recursive expansion!
  AS_$(_build_)_CMD =\
    $(AS_$(_build_)) $(AS_$(_build_)_FLAGS)\
    -ld $(dir $@)../lst --dependencies=m $(basename $@).dep -o $@ $< &&\
    $(CAT) $(basename $@).dep |\
    $(SED) -r -e 's,\\,/,g' -e 's,\s(.{1})\:, /cygdrive/\1,' >$(basename $@).d &&\
    $(RM) $(basename $@).dep
endif

define MAKE_$(_build_)_ASSEMBLE_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(_build_)"
	@$$(AS_$$(_build_)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(_build_)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AS_$$(_build_)_CMD)
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C files:
#################################

ifndef CC_$(_build_)_CMD # NOTE: Recursive expansion!
  CC_$(_build_)_CMD  =\
    $(CC_$(_build_)) $(CC_$(_build_)_FLAGS)\
    -lC $(dir $@)../lst --dependencies=m $(basename $@).dep -o $@ $< &&\
  $(CAT) $(basename $@).dep |\
  $(SED) -r -e 's,\\,/,g' -e 's,\s(.{1})\:, /cygdrive/\1,' >$(basename $@).d &&\
  $(RM) $(basename $@).dep 
endif

define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(_build_)"
	@$$(CC_$$(_build_)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(_build_)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(CC_$$(_build_)_CMD)
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a static library:
########################################

ifndef AR_$(_build_)_CMD # NOTE: Recursive expansion!
  AR_$(_build_)_CMD =\
    $(RM) -f $@ && $(AR_$(_build_)) $(AR_$(_build_)_FLAGS)\
    $(filter %.o,$^) -o $@
endif

define MAKE_$(_build_)_LIB_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(_build_)"
	@$$(AR_$$(_build_)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(_build_)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AR_$$(_build_)_CMD)
	@$$(ECHO)
endif # ifndef VERBOSE_MAKE
endef # MAKE_$(_build_)_LIB_RULE

###############################################################################
# Commands for linking building an executable image:
####################################################

ifndef LD_$(_build_)_CMD # NOTE: Recursive expansion!
  LD_$(_build_)_CMD  =\
    $(LD_$(_build_)) $(filter %.o,$^) $(filter %.a,$(LIB_FILES))\
    $(LD_$(_build_)_FLAGS) --map $(dir $@)../lst/$(notdir $(basename $@)).map\
    -o $(basename $@).out --config $(RH850_VARIANT).icf
endif

define MAKE_$(_build_)_BIN_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(_build_)"
	@$$(LD_$$(_build_)_CMD)
else
	@$$(ECHO) "################################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(_build_)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LD_$$(_build_)_CMD)
	@$$(ECHO)
endif
endef


#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name    (Description on following lines: SCR #, etc.)
# ----------- --------
# 13-Mar-2015 Emmanuel Yepez (zzxc1x) Rev. 1
# + Created initial file.
#
#===========================================================================