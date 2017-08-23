#===========================================================================
# FILE: iar_k0r_def.mak
#===========================================================================
# Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: iar_k0r_def.mak~kok_inf#4:makefile:kok_basa#1 %
# %version: kok_inf#4 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 09:41:14 2014 %
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
ifndef IAR_K0R_TOOL_VARIANT_$(_build_) # Part of PATH to IAR tools
  IAR_K0R_TOOL_VARIANT_$(_build_) := 78k
endif

ifndef IAR_K0R_OBJ_EXT_$(_build_) # file extension for object files
  IAR_K0R_OBJ_EXT_$(_build_) := .r26
endif

ifndef IAR_K0R_VARIANT_$(_build_) # Part of tool executable name
  IAR_K0R_VARIANT_$(_build_) := 78k0r
endif

#
# Env. var. can specify location of IAR tools to eliminate this warning
#
# NOTE: Can NOT have spaces in path (use "short" DOS names - see dir \X)
#
ifndef IAR_K0R_TOOL_PATH
  $(warning ENVIRONMENT variable IAR_K0R_TOOL_PATH is not defined.\
    Using default value of C:/PROGRA~2/IARSYS~1/EMBEDD~1.0)
  IAR_K0R_TOOL_PATH := C:/PROGRA~2/IARSYS~1/EMBEDD~1.0
endif

# Path to tools used for this build:
TOOL_PATH_$(_build_) := $(IAR_K0R_TOOL_PATH)/$(IAR_K0R_TOOL_VARIANT_$(_build_))
# Need a PATH usable by Cygwin shell for invoking the commands:
TOOL_PATH_CYG_$(_build_) := $(shell $(CYGPATH) -u $(subst \,/,"$(TOOL_PATH_$(_build_))"))

# Path to tool's libraries:
ifndef LIB_PATH_$(_build_)
  LIB_PATH_$(_build_) := $(TOOL_PATH_$(_build_))/lib/clib
endif

#
# IAR K0R Tool Definitions
#
AR_$(_build_) := $(TOOL_PATH_CYG_$(_build_))/bin/xar.exe
AS_$(_build_) := $(TOOL_PATH_CYG_$(_build_))/bin/a$(IAR_K0R_VARIANT_$(_build_)).exe
CC_$(_build_) := $(TOOL_PATH_CYG_$(_build_))/bin/icc$(IAR_K0R_VARIANT_$(_build_)).exe
LD_$(_build_) := $(TOOL_PATH_CYG_$(_build_))/bin/xlink.exe


###############################################################################
# Individual K0R Tool Options
#
#   These are the defaults, but they can be independently overridden in the
#   root Makefile on a per-build basis.
#

###########
# Assembler
###########

ifndef AS_$(_build_)_BASE
  AS_$(_build_)_BASE :=
endif

ifndef AS_$(_build_)_DBG
  AS_$(_build_)_DBG := -r# Generate debug information
endif

ifndef AS_$(_build_)_MACRO
  AS_$(_build_)_MACRO := -D__FAR_MODEL__ -D__NEAR_DATA_MODEL__
endif

ifndef AS_$(_build_)_SILENT
  ifndef VERBOSE_MAKE
    AS_$(_build_)_SILENT := --silent# Only display errors/warnings
  else
    AS_$(_build_)_SILENT :=
  endif
endif

############
# C Compiler
############

ifndef CC_$(_build_)_BASE
  CC_$(_build_)_BASE := -e --no_cse --no_unroll --no_inline --no_code_motion --no_tbaa
endif

ifndef CC_$(_build_)_CMOD
  CC_$(_build_)_CMOD := --code_model far# Code model
endif

ifndef CC_$(_build_)_CPU
  CC_$(_build_)_CPU := 78k0r
endif

ifndef CC_$(_build_)_DBG
  CC_$(_build_)_DBG := --debug# Generate debug information
endif

ifndef CC_$(_build_)_DMOD
  CC_$(_build_)_DMOD := --data_model near --near_const_location rom0 # Data model
endif

ifndef CC_$(_build_)_LIB
  CC_$(_build_)_LIB := --clib# Use legacy CLIB library floating-point types
endif

ifndef CC_$(_build_)_MACRO
  CC_$(_build_)_MACRO := -D SYSTEM_BUILD -D __MAX_ALIGNMENT__=2
endif

ifndef CC_$(_build_)_OPT
  CC_$(_build_)_OPT := -Ol# Low optimization
endif

ifndef CC_$(_build_)_SILENT
  ifndef VERBOSE_MAKE
    CC_$(_build_)_SILENT := --silent# Only display errors/warnings
  else
    CC_$(_build_)_SILENT :=
  endif
endif

ifndef CC_$(_build_)_INC # NOTE: Recursive expansion!
  CC_$(_build_)_INC =\
    -I$(TOOL_PATH_$(BUILD))/inc -I$(TOOL_PATH_$(BUILD))/inc/clib\
    $(patsubst %,-I%,$(INCL_DIRS))
endif

########
# Linker
########

ifndef LD_$(_build_)_ENTRY
  LD_$(_build_)_ENTRY := -s __program_start
endif

ifndef LD_$(_build_)_FMT
  LD_$(_build_)_FMT := -Fxcoff78k -yn
endif

ifndef LD_$(_build_)_LST_OPTIONS
  LD_$(_build_)_LST_OPTIONS := -xmsnioh
endif

ifndef LD_$(_build_)_OVERLAP
  LD_$(_build_)_OVERLAP := -z
endif

ifndef LD_$(_build_)_SILENT
  ifndef VERBOSE_MAKE
    LD_$(_build_)_SILENT := -S# Only display errors/warnings
  else
    LD_$(_build_)_SILENT :=
  endif
endif

###############################################################################
# Composite K0R Tool Options:
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
    $(AS_$(BUILD)_BASE) --core $(CC_$(BUILD)_CPU) $(AS_$(BUILD)_DBG)\
    $(AS_$(BUILD)_MACRO) $(AS_$(BUILD)_SILENT) $(CC_$(BUILD)_INC) 
endif

############
# C Compiler
############
ifndef CC_$(_build_)_FLAGS # NOTE: Recursive expansion!
  CC_$(_build_)_FLAGS =\
   $(CC_$(BUILD)_BASE) --core $(CC_$(BUILD)_CPU) $(CC_$(BUILD)_CMOD) $(CC_$(BUILD)_DMOD)\
   $(CC_$(BUILD)_DBG) $(CC_$(BUILD)_LIB) $(CC_$(BUILD)_OPT) $(CC_$(BUILD)_MACRO)\
   $(CC_$(BUILD)_SILENT) $(CC_$(BUILD)_INC)
endif

########
# Linker
########
ifndef LD_$(_build_)_FLAGS # NOTE: Recursive expansion!
  LD_$(_build_)_FLAGS =\
    $(LD_$(BUILD)_FMT) $(LD_$(BUILD)_OVERLAP)\
    $(LD_$(BUILD)_LST_OPTIONS) $(LD_$(BUILD)_ENTRY) $(LD_$(BUILD)_SILENT)
endif

###############################################################################

###############################################################################
# Commands for assembling files:
#################################

#
# sed is used to convert the DOS-style dependency paths to Cygwin paths:
# 1) '\' -> '/', and 2) ' C:' -> ' /cygdrive/C'
#
ifndef AS_$(_build_)_CMD # NOTE: Recursive expansion!
  AS_$(_build_)_CMD =\
    $(AS_$(BUILD)) $(AS_$(BUILD)_FLAGS)\
    -l $(dir $@)../lst --dependencies=m $(basename $@).dep -o $@ $< &&\
  $(CAT) $(basename $@).dep |\
  $(SED) -r -e 's,\\,/,g' -e 's,\s(.{1})\:, /cygdrive/\1,' >$(basename $@).d &&\
  $(RM) $(basename $@).dep
endif

define MAKE_$(_build_)_ASSEMBLE_RULE
$(1)
	$$(call Check_V850_TOOL_PATH_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(AS_$$(BUILD)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AS_$$(BUILD)_CMD)
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C files:
#################################

#
# sed is used to convert the DOS-style dependency paths to Cygwin paths:
# 1) '\' -> '/', and 2) ' C:' -> ' /cygdrive/C'
#
ifndef CC_$(_build_)_CMD # NOTE: Recursive expansion!
  CC_$(_build_)_CMD  =\
    $(CC_$(BUILD)) $(CC_$(BUILD)_FLAGS)\
    -lCN $(dir $@)../lst --dependencies=m $(basename $@).dep -o $@ $< &&\
  $(CAT) $(basename $@).dep |\
  $(SED) -r -e 's,\\,/,g' -e 's,\s(.{1})\:, /cygdrive/\1,' >$(basename $@).d &&\
  $(RM) $(basename $@).dep 
endif

define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
	$$(call Check_V850_TOOL_PATH_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(CC_$$(BUILD)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(CC_$$(BUILD)_CMD)
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a static library:
########################################

ifndef AR_$(_build_)_CMD # NOTE: Recursive expansion!
  AR_$(_build_)_CMD =\
    $(RM) -f $@ && $(AR_$(BUILD)) $(AR_$(BUILD)_FLAGS)\
    $(filter $(LD_$(BUILD)_OBJ_FILTER_PATTERN),$^) -o $@
endif

define MAKE_$(_build_)_LIB_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(AR_$$(BUILD)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AR_$$(BUILD)_CMD)
	@$$(ECHO)
endif # ifndef VERBOSE_MAKE
endef # MAKE_$(_build_)_LIB_RULE

###############################################################################
# Commands for linking building an executable image:
####################################################

ifndef LD_$(_build_)_OBJ_FILTER_PATTERN
  LD_$(_build_)_OBJ_FILTER_PATTERN := %$(IAR_K0R_OBJ_EXT_$(_build_))
endif

#
# Because object files and library files have the same extensions for IAR
# tools, any libraries built in the project will be included in the command
# line by the first "$(filter )" (in the following command). This is because
# libraries built in the project are in the prerequisite list and will come
# in via "$^". In order for these to not appear a second time in the command
# the must be "$(filter-out )" of $(LIB_FILES).
#
ifndef LD_$(_build_)_CMD # NOTE: Recursive expansion!
  LD_$(_build_)_CMD  =\
    $(LD_$(BUILD)) $(filter $(LD_$(BUILD)_OBJ_FILTER_PATTERN),$^)\
    $(LD_$(BUILD)_FLAGS) -o $(basename $@).lnk\
    -l $(dir $@)../lst/$(notdir $(basename $@)).html -f $(LNK_CMD_FILE)\
    -I$(LIB_PATH_$(BUILD)) $(filter-out $($(BUILD)_OUTPUT_ROOT)/%,$(LIB_FILES))\
    -Omotorola-s28=$@
endif

define MAKE_$(_build_)_BIN_RULE
$(1)
	$$(call Check_V850_TOOL_PATH_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(LD_$$(BUILD)_CMD)
else
	@$$(ECHO) "################################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LD_$$(BUILD)_CMD)
	@$$(ECHO)
endif
endef

###############################################################################

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 26-Jun-2012 Kirk Bailey
# + Created initial file.
#
# 12-Jul-2012 Kirk Bailey
# + Changed extension of file output by link from .xcoff78k to match what
# + is done by the IAR workshop.
#
# 12-Nov-2014 Kirk Bailey Rev 4
# + Switch from using ';' to '&&' for command lines with multiple commands
# + so that the return code reflects any failures.
#
#===========================================================================
