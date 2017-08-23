#===========================================================================
# FILE: uitron_arm_rvct_def.mak
#===========================================================================
# Copyright 2011-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: uitron_arm_rvct_def.mak~kok_inf#15:makefile:kok_aud#1 %
# %version: kok_inf#15 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 09:41:17 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================

#
# ARM RVCT Tool Definitions
#

#
# The following environment variables are required for the Renesas tools
# to operate properly
#
ifndef RVCTBIN
  RVCTBIN_NOT_DEFINED:=1
  export RVCTBIN := C:/eSOL/eBinder/cl/ARM/RVCT/Programs/2.2/503/win_32-pentium
endif

define Issue_RVCTBIN_Warning
  $(warning ENVIRONMENT variable RVCTBIN is not defined. Using default value of $(RVCTBIN))
  $(eval RVCTBIN_NOT_DEFINED:=)
endef

define Check_RVCT_Definitions
$(if $(RVCTBIN_NOT_DEFINED),$(call Issue_RVCTBIN_Warning),)
endef

ARM_RVCT_TOOL_DIR := $(subst \,/,$(RVCTBIN))
ARM_RVCT_TOOL_DIR_CYG := $(shell $(CYGPATH) -u $(ARM_RVCT_TOOL_DIR))

AR_$(_build_)  := $(ARM_RVCT_TOOL_DIR_CYG)/armar.exe
AS_$(_build_)  := $(ARM_RVCT_TOOL_DIR_CYG)/armasm.exe
CC_$(_build_)  := $(ARM_RVCT_TOOL_DIR_CYG)/armcc.exe
CPP_$(_build_) := $(ARM_RVCT_TOOL_DIR_CYG)/armcpp.exe
LD_$(_build_)  := $(ARM_RVCT_TOOL_DIR_CYG)/armlink.exe

AS_$(_build_)_VERSION:=--vsn # override version option of "-v"
CC_$(_build_)_VERSION:=--vsn # override version option of "-v"
CPP_$(_build_)_VERSION:=--vsn # override version option of "-v"
LD_$(_build_)_VERSION:=--vsn # override version option of "-v"

##############################################################################
# RVCT TOOL OPTIONS:
#

##############################
# AS [code-generation-options]
##############################
ifndef AS_$(_build_)_BASE
  AS_$(_build_)_BASE := --thumb --littleend --apcs /interwork
endif
####################
# AS [cpu-options]
####################
ifndef AS_$(_build_)_CPU
  AS_$(_build_)_CPU := --cpu=ARM926EJ-S
endif
####################
# AS [debug-options]
####################
ifndef AS_$(_build_)_DBG
  AS_$(_build_)_DBG := --debug
endif
###################
# AS [search-paths]
###################
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)# a single space
COMMA := ,
ifndef AS_$(_build_)_INCL
  AS_$(_build_)_INCL =\
    $(if $(INCL_DIRS),-i$(subst $(SPACE),$(COMMA),$(strip $(INCL_DIRS))))
endif


#####################
# C [source-language]
#####################
ifndef CC_$(_build_)_BASE
  CC_$(_build_)_BASE :=  -c --c90 --thumb --littleend --unsigned_chars --unix_depend_format --apcs /interwork --enum_is_int
endif
####################
# C [cpu-options]
####################
ifndef CC_$(_build_)_CPU
  CC_$(_build_)_CPU := --cpu=ARM926EJ-S
endif
#####################
# C (warnings)
#####################
ifndef CC_$(_build_)_WARN
  CC_$(_build_)_WARN :=  --strict_warnings --diag_suppress 228,230,450
endif
##################
# C (optimization)
##################
ifndef CC_$(_build_)_OPT
  CC_$(_build_)_OPT := -O3 -Ospace
endif
####################
# C [debug-options]
####################
ifndef CC_$(_build_)_DBG
  CC_$(_build_)_DBG := -g
endif
##########################
# C [preprocessor-options]
##########################
ifndef CC_$(_build_)_MACRO
  CC_$(_build_)_MACRO :=
endif
##################
# C [search-paths]
##################
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)# a single space
COMMA := ,
ifndef CC_$(_build_)_INCL
  CC_$(_build_)_INCL =\
    $(if $(INCL_DIRS),-I$(subst $(SPACE),$(COMMA),$(strip $(INCL_DIRS))))
endif


#######################
# C++ [source-language]
#######################
ifndef CPP_$(_build_)_BASE
  CPP_$(_build_)_BASE := -c --cpp --thumb --littleend --unsigned_chars --unix_depend_format --no_exceptions --no_rtti --enum_is_int
endif
######################
# C++ [cpu-options]
######################
ifndef CPP_$(_build_)_CPU
  CPP_$(_build_)_CPU := --cpu=ARM926EJ-S
endif
#######################
# C++ (warnings)
#######################
ifndef CPP_$(_build_)_WARN
  CPP_$(_build_)_WARN := --strict_warnings --diag_suppress 228,230,450
endif
####################
# C++ (optimization)
####################
ifndef CPP_$(_build_)_OPT
  CPP_$(_build_)_OPT := -O3 -Ospace
endif
#####################
# C++ [debug-options]
#####################
ifndef CPP_$(_build_)_DBG
  CPP_$(_build_)_DBG := -g
endif
############################
# C++ [preprocessor-options]
############################
ifndef CPP_$(_build_)_MACRO
  CPP_$(_build_)_MACRO :=
endif
####################
# C++ [search-paths]
####################
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)# a single space
COMMA := ,
ifndef CPP_$(_build_)_INCL
  CPP_$(_build_)_INCL =\
    $(if $(INCL_DIRS),-I$(subst $(SPACE),$(COMMA),$(strip $(INCL_DIRS))))
endif


##############################################################################
#
# Composite Tool Option Definitions
#

ifndef AR_$(_build_)_FLAGS
  AR_$(_build_)_FLAGS = --create
endif

ifndef AS_$(_build_)_FLAGS
  AS_$(_build_)_FLAGS = $(AS_$(BUILD)_BASE) $(AS_$(BUILD)_CPU)\
                        $(AS_$(BUILD)_DBG)  $(AS_$(BUILD)_INCL) 
endif

ifndef CC_$(_build_)_FLAGS
  CC_$(_build_)_FLAGS = $(CC_$(BUILD)_BASE) $(CC_$(BUILD)_CPU)\
    $(CC_$(BUILD)_WARN) $(CC_$(BUILD)_OPT) $(CC_$(BUILD)_DBG)\
    $(CC_$(BUILD)_MACRO) $(CC_$(BUILD)_INCL)
endif

ifndef CPP_$(_build_)_FLAGS
  CPP_$(_build_)_FLAGS = $(CPP_$(BUILD)_BASE) $(CPP_$(BUILD)_CPU)\
    $(CPP_$(BUILD)_WARN) $(CPP_$(BUILD)_OPT) $(CPP_$(BUILD)_DBG)\
    $(CPP_$(BUILD)_MACRO) $(CPP_$(BUILD)_INCL)
endif


ifndef LD_$(_build_)_COMPRESS
  LD_$(_build_)_COMPRESS = --datacompressor off
endif

ifndef LD_$(_build_)_INFO
  LD_$(_build_)_INFO = --callgraph --info sizes,totals --map --symbols
endif

ifndef LD_$(_build_)_LST
  LD_$(_build_)_LST = --list $(dir $(basename $@))../lst/$(notdir $(basename $@)).lst
endif

ifndef LD_$(_build_)_WARN
  LD_$(_build_)_WARN = --diag_suppress 6330
endif

ifndef LD_$(_build_)_FLAGS
  LD_$(_build_)_FLAGS = $(LD_$(BUILD)_WARN) $(LD_$(BUILD)_COMPRESS) $(LD_$(BUILD)_INFO) $(LD_$(BUILD)_LST)
endif

###############################################################################

###############################################################################
# Commands for assembling .s files:
#####################################
#
# The following variable contains the command for assembing a .src file using
# the HEW ARM_RVCT tool chain. Note that it uses recursive expansion.
#
AS_$(_build_)_CMD = $(AS_$(BUILD)) $(AS_$(BUILD)_FLAGS) --depend $(basename $@).tmp

define MAKE_$(_build_)_ASSEMBLE_RULE
$(1)
	$$(call Check_RVCT_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(AS_$$(BUILD)_CMD) -o $$@ $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AS_$$(BUILD)_CMD) -o $$@ $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C files:
#################################
#
# The following variable contains the command for compiling a .c file using
# the HEW ARM_RVCT tool chain. Note that it uses recursive expansion. sed
# is used to reformat the .d file output
# such that 1) it includes the relative path to the object file, and 2)
# any absolute DOS paths (e.g., C:/) are converted to Cygwin paths
# (/cygdrive/c).
#
ifndef CC_$(_build_)_CMD
  CC_$(_build_)_CMD = $(CC_$(BUILD)) $(CC_$(BUILD)_FLAGS)
endif

define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
	$$(call Check_RVCT_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
ifdef ASM_MAKE
	@$$(CC_$$(BUILD)_CMD) -S -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	@$$(CC_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
	@$$(CC_$$(BUILD)_CMD) --depend $$(basename $$@).tmp -o $$@ $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
ifeq ($$(PP_MAKE),2)
	@$$(CC_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifdef ASM_MAKE
	$$(CC_$$(BUILD)_CMD) -S -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	$$(CC_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
	@$$(ECHO)
	$$(CC_$$(BUILD)_CMD) --show_cmdline --depend $$(basename $$@).tmp -o $$@ $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
ifeq ($$(PP_MAKE),2)
	$$(CC_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C++ files:
###################################
#
# The following variable contains the command for compiling a .c file using
# the HEW ARM_RVCT tool chain. Note that it uses recursive expansion. sed
# is used to reformat the .d file output
# such that 1) it includes the relative path to the object file, and 2)
# any absolute DOS paths (e.g., C:/) are converted to Cygwin paths
# (/cygdrive/c).
#
ifndef CPP_$(_build_)_CMD
  CPP_$(_build_)_CMD = $(CPP_$(BUILD)) $(CPP_$(BUILD)_FLAGS)
endif

define MAKE_$(_build_)_COMPILE_CPP_RULE
$(1)
	$$(call Check_RVCT_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
ifdef ASM_MAKE
	@$$(CPP_$$(BUILD)_CMD) -S -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	@$$(CPP_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
	@$$(CPP_$$(BUILD)_CMD) --depend $$(basename $$@).tmp -o $$@ $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
ifeq ($$(PP_MAKE),2)
	@$$(CPP_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifdef ASM_MAKE
	$$(CPP_$$(BUILD)_CMD) -S -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	$$(CPP_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
	@$$(ECHO)
	$$(CPP_$$(BUILD)_CMD) --show_cmdline --depend $$(basename $$@).tmp -o $$@ $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
ifeq ($$(PP_MAKE),2)
	$$(CPP_$$(BUILD)_CMD) -E -C -o $$(basename $$@).i $$<
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a static library:
########################################

define MAKE_$(_build_)_LIB_RULE
$(1)
	$$(call Check_RVCT_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(AR_$$(BUILD)) $$(AR_$$(BUILD)_FLAGS) $$@ $$(filter %.o,$$^)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AR_$$(BUILD)) $$(AR_$$(BUILD)_FLAGS) $$@ $$(filter %.o,$$^)
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking building an executable image:
####################################################

ifndef LD_$(_build_)_CMD
define LD_$(_build_)_CMD
  $(LD_$(BUILD)) $(filter %.o,$^) $(filter %.a,$^) $(NON_STD_LIBS) $(LD_$(BUILD)_FLAGS)\
  $(if $(LNK_CMD_FILE),--via=$(LNK_CMD_FILE)) --output=$@
endef
endif

define MAKE_$(_build_)_BIN_RULE
$(1)
	$$(call Check_RVCT_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(LD_$$(BUILD)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LD_$$(BUILD)_CMD)
	@$$(ECHO)
endif
endef

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 12-Nov-2014 Kirk Bailey Rev 14
# + Deleted revision history prior to 2012.
# + Switch from using ';' to '&&' for command lines with multiple commands
# + so that the return code reflects any failures.
#
#===========================================================================
