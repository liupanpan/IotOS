#===========================================================================
# FILE: 200c_necv850_def.mak
#===========================================================================
# Copyright 2011-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: 200c_necv850_def.mak~kok_inf#24:makefile:kok_aud#2 %
# %version: kok_inf#24 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 09:41:12 2014 %
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
ifndef V850_TOOL_PATH
  V850_TOOL_PATH_NOT_DEFINED:=1
  V850_TOOL_PATH := C:/Devapps/Tools/nec_v850/c/nec/3.10/bin
endif

ifndef V850_DEV_PATH
  V850_DEV_PATH := $(V850_TOOL_PATH)/../dev
endif

define Issue_V850_TOOL_PATH_Warning
  $(warning ENVIRONMENT variable V850_TOOL_PATH is not defined. Using default value of $(V850_TOOL_PATH))
  $(eval V850_TOOL_PATH_NOT_DEFINED:=)
endef

define Check_V850_TOOL_PATH_Definitions
$(if $(V850_TOOL_PATH_NOT_DEFINED),$(call Issue_V850_TOOL_PATH_Warning),)
endef

V850_TOOL_PATH_CYG := $(shell $(CYGPATH) -u $(subst \,/,$(V850_TOOL_PATH)))

#
# NEC V850 Tool Definitions
#
AR_$(_build_)   := $(V850_TOOL_PATH_CYG)/ld850
AS_$(_build_)   := $(V850_TOOL_PATH_CYG)/as850
CC_$(_build_)   := $(V850_TOOL_PATH_CYG)/ca850
CPP_$(_build_)  := $(V850_TOOL_PATH_CYG)/ca850
GCC_$(_build_)  := gcc
HEX_$(_build_)  := $(V850_TOOL_PATH_CYG)/hx850
LD_$(_build_)   := $(V850_TOOL_PATH_CYG)/ld850
ROMP_$(_build_) := $(V850_TOOL_PATH_CYG)/romp850
DUMP_$(_build_) := $(V850_TOOL_PATH_CYG)/dump850

CC_$(_build_)_VERSION:=-V # override version option of "-v"
CPP_$(_build_)_VERSION:=-V # override version option of "-v"

###############################################################################
# Individual V850 Tool Options:
###############################
ifndef CC_$(_build_)_CPU
  CC_$(_build_)_CPU := f3335
endif
ifndef CC_$(_build_)_DBG
  CC_$(_build_)_DBG := -g
endif
ifndef CC_$(_build_)_GEN
  CC_$(_build_)_GEN := -Xr -Xmask_reg -Xbpc=0xFFB -Xsconst=5
endif
ifndef CC_$(_build_)_LST
  CC_$(_build_)_LST :=
endif
ifndef CC_$(_build_)_MACRO
  CC_$(_build_)_MACRO :=
endif
ifndef GCC_$(_build_)_MACRO
  GCC_$(_build_)_MACRO := -D _lint
endif
ifndef CC_$(_build_)_MISC
  CC_$(_build_)_MISC := -Xc -Xm32765 -c -Xcxxcom -XFo -a -devpath="$(V850_DEV_PATH)"
endif
ifndef CC_$(_build_)_OPT
  CC_$(_build_)_OPT := -Os -Wp,-S
endif
ifndef CC_$(_build_)_SEC_FILE
  CC_$(_build_)_SEC_FILE :=
endif
ifndef CC_$(_build_)_WARN
  CC_$(_build_)_WARN := -ansi -w2 -woff=2231 -wcallnodecl+
endif

ifndef DUMP_$(_build_)_FLAGS
  DUMP_$(_build_)_FLAGS := -t
endif

ifndef HEX_$(_build_)_FLAGS
  HEX_$(_build_)_FLAGS := -fS
endif

ifndef LD_$(_build_)_LST
  LD_$(_build_)_LST :=
endif

ifndef ROMP_$(_build_)_MAP
  ROMP_$(_build_)_MAP = -m=$(@D)/$(basename $(@F)).romp_map
endif 

ifndef ROMP_$(_build_)_FLAGS
  ROMP_$(_build_)_FLAGS = $(ROMP_$(BUILD)_MAP) -F "$(V850_DEV_PATH)"
endif
###############################################################################

###############################################################################
# Composite Tool Options:
###############################
#
# Options for archiving (generating a static library)
#
ifndef AR_$(_build_)_FLAGS
  AR_$(_build_)_FLAGS = -r -cpu $(CC_$(BUILD)_CPU) -F "$(V850_DEV_PATH)"
endif
#
# Options for the assembler
#
ifndef AS_$(_build_)_FLAGS
  AS_$(_build_)_FLAGS = -cpu $(CC_$(BUILD)_CPU) -g -m -f -bpc=0xFFB -F "$(V850_DEV_PATH)"
endif
#
# Options for the C compiler
#
ifndef CC_$(_build_)_FLAGS
  CC_$(_build_)_FLAGS =\
   -cpu $(CC_$(BUILD)_CPU) $(CC_$(BUILD)_WARN) $(CC_$(BUILD)_GEN) $(CC_$(BUILD)_LST)\
   $(CC_$(BUILD)_MISC) $(CC_$(BUILD)_SEC_FILE) $(CC_$(BUILD)_DBG) $(CC_$(BUILD)_OPT)\
   $(CC_$(BUILD)_MACRO)\
   $(patsubst %,-I%,$(INCL_DIRS))\
   -Fv=$(dir $@)../lst/$(basename $(notdir $@)).lst
endif

ifndef GCC_$(_build_)_FLAGS
  GCC_$(_build_)_FLAGS = -Wall -MM $(GCC_$(BUILD)_MACRO) $(patsubst %,-I %,$(INCL_DIRS))
endif
#
# Options for linking to form an executable image
#
ifndef $(_build_)_ROMPCRT_PATH
  $(_build_)_ROMPCRT_PATH := $(V850_TOOL_PATH_CYG)/../lib850/r32
endif
ifndef LD_$(_build_)_FLAGS
  LD_$(_build_)_FLAGS = \
   -cpu $(CC_$(BUILD)_CPU) -D $(LNK_CMD_FILE)\
	$(LD_$(BUILD)_LST) -mask_reg -F "$(V850_DEV_PATH)" -lc -lr\
	$(shell $(CYGPATH) -m $($(BUILD)_ROMPCRT_PATH))/rompcrt.o
endif

###############################################################################

###############################################################################
# Commands for assembling files:
#################################

AS_$(_build_)_CMD = $(AS_$(BUILD)) $(AS_$(BUILD)_FLAGS)

define MAKE_$(_build_)_ASSEMBLE_RULE
$(1)
	$$(call Check_V850_TOOL_PATH_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(AS_$$(BUILD)_CMD) -o $$@ $$<
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AS_$$(BUILD)_CMD) -o $$@ $$<
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C files:
#################################
#
# The following variable contains the command for compiling a .c file using
# the NEC V850 tool chain. Note that it uses recursive expansion. It also
# uses gcc to build the associated dependency (.d) file since the NEC
# compiler does not support this. sed is used to reformat the .d file output
# such that 1) it includes the relative path to the object file, and 2)
# any absolute DOS paths (e.g., C:/) are converted to Cygwin paths
# (/cygdrive/c).
#
ifndef CC_$(_build_)_CMD
  CC_$(_build_)_CMD  = $(CC_$(BUILD)) $(CC_$(BUILD)_FLAGS)
endif
ifndef GCC_$(_build_)_CMD
  GCC_$(_build_)_CMD = $(GCC_$(BUILD)) $(GCC_$(BUILD)_FLAGS)
endif

define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
	$$(call Check_V850_TOOL_PATH_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
ifdef ASM_MAKE
	@$$(CC_$$(BUILD)_CMD) -S -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	@$$(CC_$$(BUILD)_CMD) -C -E $$< >$$(basename $$@).i
endif
	@$$(CC_$$(BUILD)_CMD) -o $$@ $$<
ifeq ($$(PP_MAKE),2)
	@$$(CC_$$(BUILD)_CMD) -C -E $$< >$$(basename $$@).i
endif
	@$$(GCC_$$(BUILD)_CMD) -o $$(basename $$@).tmp $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
             $$(SED) -e 's,\(.*\): ,$$(@D)/\1: ,g' >$$(basename $$@).d\
                   -e 's,\(.\)\:\([^ ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifdef ASM_MAKE
	$$(CC_$$(BUILD)_CMD) -S -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	$$(CC_$$(BUILD)_CMD) -C -E $$< >$$(basename $$@).i
endif
	$$(CC_$$(BUILD)_CMD) -o $$@ $$<
ifeq ($$(PP_MAKE),2)
	$$(CC_$$(BUILD)_CMD) -C -E $$< >$$(basename $$@).i
endif
	@$$(ECHO)
	$$(GCC_$$(BUILD)_CMD) -o $$(basename $$@).tmp $$< &&\
    $$(CAT) $$(basename $$@).tmp |\
            $$(SED) -e 's,\(.*\): ,$$(@D)/\1: ,g' >$$(basename $$@).d\
                  -e 's,\(.\)\:\([^ ]\),/cygdrive/\1\2,g' >$$(basename $$@).d &&\
    $$(RM) $$(basename $$@).tmp
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a static library:
########################################

define MAKE_$(_build_)_LIB_RULE
$(1)
	$$(call Check_V850_TOOL_PATH_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(AR_$$(BUILD)) $$(AR_$$(BUILD)_FLAGS) -o $$@ $$(filter %.o,$$^) 2> /dev/null &&\
    $$(TOUCH) $$@
else
	@$$(ECHO) "################################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AR_$$(BUILD)) $$(AR_$$(BUILD)_FLAGS) -o $$@ $$(filter %.o,$$^) &&\
    $$(TOUCH) $$@
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking building an executable image:
####################################################

define ROMP_COMMAND_$(_build_)
@$(ECHO) "  Using romp.exe to generate $(basename $(notdir $@)).out for $(BUILD)"
$(ROMP_$(BUILD)) $(ROMP_$(BUILD)_FLAGS) -o $(@D)/$(basename $(notdir $@)).out $@
endef

define NO_ROMP_COMMAND_$(_build_)
@$(ECHO) "  Skipping romp.exe because of no compiler -Xr option."
$(CP) -f $@ $(@D)/$(basename $(notdir $@)).out
endef

define MAKE_$(_build_)_BIN_RULE
$(1)
	$$(call Check_V850_TOOL_PATH_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(LD_$$(BUILD)) $$(filter %.o %.lib,$$^) $$(LD_$$(BUILD)_FLAGS) -o $$@
	@$$(if $$(findstring -Xr,$$(CC_$$(BUILD)_FLAGS)),\
           $$(call ROMP_COMMAND_$$(BUILD)),$$(call NO_ROMP_COMMAND_$$(BUILD)))
	@$$(ECHO) "  Generating $$(basename $$(notdir $$@)).ptp for $$(BUILD)"
	@$$(HEX_$$(BUILD))  $$(HEX_$$(BUILD)_FLAGS) $$(@D)/$$(basename $$(notdir $$@)).out -o $$(@D)/$$(basename $$(notdir $$@)).ptp
	@$$(ECHO) "  Generating $$(basename $$(notdir $$@)).map for $$(BUILD)"
	@$$(DUMP_$$(BUILD)) $$(DUMP_$$(BUILD)_FLAGS) $$(@D)/$$(basename $$(notdir $$@)).out > $$(@D)/$$(basename $$(notdir $$@)).map
else
	@$$(ECHO) "################################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LD_$$(BUILD)) $$(filter %.o %.lib,$$^) $$(LD_$$(BUILD)_FLAGS) -o $$@
	$$(if $$(findstring -Xr,$$(CC_$$(BUILD)_FLAGS)),\
          $$(call ROMP_COMMAND_$$(BUILD)),$$(call NO_ROMP_COMMAND_$$(BUILD)))
	@$$(ECHO) "  Generating $$(basename $$(notdir $$@)).ptp for $$(BUILD)"
	$$(HEX_$$(BUILD)) $$(HEX_$$(BUILD)_FLAGS) $$(@D)/$$(basename $$(notdir $$@)).out -o $$(@D)/$$(basename $$(notdir $$@)).ptp
	@$$(ECHO) "  Generating $$(basename $$(notdir $$@)).map for $$(BUILD)"
	@$$(DUMP_$$(BUILD)) $$(DUMP_$$(BUILD)_FLAGS) $$(@D)/$$(basename $$(notdir $$@)).out > $$(@D)/$$(basename $$(notdir $$@)).map
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
# 12-Nov-2014 Kirk Bailey Rev 24
# + Removed revision history prior to 2012.
# + Switch from using ';' to '&&' for command lines with multiple commands
# + so that the return code reflects any failures.
#
#===========================================================================
