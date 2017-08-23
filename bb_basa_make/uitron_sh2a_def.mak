#===========================================================================
# FILE: uitron_sh2a_def.mak
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:  uitron_sh2a_def.mak~kok_basa#20:makefile:kok_aud#3 %
# %version:  kok_basa#20 %
# %derived_by:  dzq92s %
# %date_modified: Mon Apr 19 15:06:49 2010 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================

#
# HEW SH2A Tool Definitions
#

#
# The following environment variables are required for the Renesas tools
# to operate properly
#
ifndef SHC_INC
  SHC_INC_NOT_DEFINED:=1
  export SHC_INC := C:\\Progra~1\\Renesas\\Hew\\Tools\\Renesas\\Sh\\9_1_0_5\\Include
endif

ifndef SHC_LIB
  SHC_LIB_NOT_DEFINED:=1
  export SHC_LIB := C:\\Progra~1\\Renesas\\Hew\\Tools\\Renesas\\Sh\\9_1_0_5\\Bin
endif

ifndef SHC_TMP
  SHC_TMP_NOT_DEFINED:=1
  export SHC_TMP := C:\\Progra~1\\Renesas\\Hew\\Tools\\Renesas\\Sh\\9_1_0_5\\Ctemp
endif

define Issue_SHC_INC_Warning
  $(warning ENVIRONMENT variable SHC_INC is not defined. Using default value of $(SHC_INC))
  $(eval SHC_INC_NOT_DEFINED:=)
endef

define Issue_SHC_LIB_Warning
  $(warning ENVIRONMENT variable SHC_LIB is not defined. Using default value of $(SHC_LIB))
  $(eval SHC_LIB_NOT_DEFINED:=)
endef

define Issue_SHC_TMP_Warning
  $(warning ENVIRONMENT variable SHC_TMP is not defined. Using default value of $(SHC_TMP))
  $(eval SHC_TMP_NOT_DEFINED:=)
endef

define Check_SHC_Definitions
$(if $(SHC_INC_NOT_DEFINED),$(call Issue_SHC_INC_Warning),)
$(if $(SHC_LIB_NOT_DEFINED),$(call Issue_SHC_LIB_Warning),)
$(if $(SHC_TMP_NOT_DEFINED),$(call Issue_SHC_TMP_Warning),)
endef

SH2A_TOOL_DIR := $(subst \,/,$(SHC_LIB))
SH2A_TOOL_DIR_CYG := $(shell $(CYGPATH) -u $(SH2A_TOOL_DIR))
#
# Make sure Renesas tool dir. is in PATH; otherwise, lbgsh will
# fail and the reason will not be obvious.
#
PATH := $(PATH):$(SH2A_TOOL_DIR_CYG)

AR_$(_build_)  := $(SH2A_TOOL_DIR_CYG)/optlnk.exe
AS_$(_build_)  := $(SH2A_TOOL_DIR_CYG)/asmsh.exe
CC_$(_build_)  := $(SH2A_TOOL_DIR_CYG)/shc.exe
CPP_$(_build_) := $(SH2A_TOOL_DIR_CYG)/shcpp.exe
LD_$(_build_)  := $(SH2A_TOOL_DIR_CYG)/optlnk.exe
#
# NOTE: The Renesas tools have a special Library Generator Tool used to
#       generate custom stdlib implementations. This tool is different
#       from the standard linker:
#
LIBGEN_$(_build_) := $(SH2A_TOOL_DIR_CYG)/lbgsh.exe
#
# NOTE: gcc is used to generate the dependency files (.d) for C source.
#       The Renesas compiler does not support this.
#
GCC_$(_build_) := gcc
GCC_$(_build_)_FLAGS =\
  -M $(patsubst %,-I %,$(INCL_DIRS)) $(GCC_$(BUILD)_MACROS) -I $(SH2A_TOOL_DIR_CYG)/../include

GCPP_$(_build_) := gcc
GCPP_$(_build_)_FLAGS =\
  -M $(patsubst %,-I %,$(INCL_DIRS)) $(GCPP_$(BUILD)_MACROS) -I $(SH2A_TOOL_DIR_CYG)/../include

##############################################################################
# HEW TOOL OPTIONS:
#

#
# HEW SH2A CPU Variants: sh2a, sh2afpu
#
ifndef AS_$(_build_)_CPU
  AS_$(_build_)_CPU  := -cpu=sh2afpu
endif
ifndef CC_$(_build_)_CPU
  CC_$(_build_)_CPU  := -cpu=sh2afpu
endif
#
# HEW SH2A FPU Variants: single, double
#
ifndef CC_$(_build_)_FPU
  CC_$(_build_)_FPU  := -fpu=single
endif
#
# HEW debug information: -debug, -nodebug
#
ifndef AS_$(_build_)_DBG
  AS_$(_build_)_DBG := -debug
endif
ifndef CC_$(_build_)_DBG
  CC_$(_build_)_DBG := -debug
endif
#
# HEW misc. options
#
ifndef AS_$(_build_)_MISC
  AS_$(_build_)_MISC := -errorpath -literal=pool,branch,jump,return -nologo
endif
ifndef CC_$(_build_)_MISC
  CC_$(_build_)_MISC := -errorpath -message -nologo -show=source
endif
#
# HEW compiler optimzation: -optimize=[0|1] (for off/on)
#                           [-speed|-size|-nospeed]
#                           -gbr=[auto|user]
#
ifndef CC_$(_build_)_OPT
  CC_$(_build_)_OPT := -optimize=1 -size -gbr=auto -global_volatile=1
endif
#
# HEW C++ template option: -template=none|static|used|all|auto
#
ifndef CPP_$(_build_)_TEMPLATE
  CPP_$(_build_)_TEMPLATE := -template=static
endif

#
# The stupid assembler requires absolute paths in the Windows format
# (with '/' instead of '\') to work
#
ifndef AS_$(_build_)_INCL
  AS_$(_build_)_INCL = $(if $(INCL_DIRS),\
    -include=$(subst $(SPACE),$(COMMA),$(subst \,/,$(foreach dir,$(INCL_DIRS),$(shell $(CYGPATH) -wa $(dir))))))
endif

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)# a single space
COMMA := ,

ifndef CC_$(_build_)_INCL
  CC_$(_build_)_INCL =\
    $(if $(INCL_DIRS),-include=$(subst $(SPACE),$(COMMA),$(strip $(INCL_DIRS))))
endif

##############################################################################
#
# Composite Tool Option Definitions
#

ifndef AR_$(_build_)_FLAGS
  AR_$(_build_)_FLAGS = -noprelink -form=library -nologo
endif

ifndef AS_$(_build_)_FLAGS
  AS_$(_build_)_FLAGS += $(AS_$(BUILD)_CPU) $(AS_$(BUILD)_DBG)\
  $(AS_$(BUILD)_DBG) $(AS_$(BUILD)_MISC) $(AS_$(BUILD)_MACROS)\
  $(AS_$(BUILD)_INCL) -list=$(dir $(@))../lst/$(basename $(@F)).lst
endif

ifndef CC_$(_build_)_FLAGS
  CC_$(_build_)_FLAGS += $(CC_$(BUILD)_CPU) $(CC_$(BUILD)_FPU)\
   $(CC_$(BUILD)_DBG) $(CC_$(BUILD)_OPT) $(CC_$(BUILD)_MISC)\
   $(CC_$(BUILD)_MACROS) $(CC_$(BUILD)_INCL)\
   -list=$(dir $(@))../lst/$(basename $(@F)).lst
endif

ifndef CPP_$(_build_)_FLAGS
  CPP_$(_build_)_FLAGS += $(CC_$(BUILD)_CPU) $(CC_$(BUILD)_FPU)\
   $(CC_$(BUILD)_DBG) $(CC_$(BUILD)_OPT) $(CPP_$(BUILD)_TEMPLATE)\
   $(CC_$(BUILD)_MISC) $(CPP_$(BUILD)_MACROS) $(CC_$(BUILD)_INCL)\
   -list=$(dir $(@))../lst/$(basename $(@F)).lst
endif

ifndef LD_$(_build_)_FLAGS
  LD_$(_build_)_FLAGS += $(if $(LNK_CMD_FILE),-sub=$(LNK_CMD_FILE))
endif

ifndef LD_$(_build_)_PTP_FLAGS
  LD_$(_build_)_PTP_FLAGS := -noprelink -form=stype -nologo
endif

###############################################################################

###############################################################################
# Commands for assembling .src files:
#####################################
#
# The following variable contains the command for assembing a .src file using
# the HEW SH2A tool chain. Note that it uses recursive expansion.
#
AS_$(_build_)_CMD = $(AS_$(BUILD)) $(AS_$(BUILD)_FLAGS)

define MAKE_$(_build_)_ASSEMBLE_RULE
$(1)
	$$(call Check_SHC_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(AS_$$(BUILD)_CMD) -object=$$@ ./$$<
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AS_$$(BUILD)_CMD) -object=$$@ ./$$<
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C files:
#################################
#
# The following variable contains the command for compiling a .c file using
# the HEW SH2A tool chain. Note that it uses recursive expansion. It also
# uses gcc to build the associated dependency (.d) file since the Renesas
# compiler does not support this. sed is used to reformat the .d file output
# such that 1) it includes the relative path to the object file, and 2)
# any absolute DOS paths (e.g., C:/) are converted to Cygwin paths
# (/cygdrive/c).
#
ifndef CC_$(_build_)_CMD
  CC_$(_build_)_CMD = $(CC_$(BUILD)) $(CC_$(BUILD)_FLAGS)
endif

ifndef GCC_$(_build_)_CMD
  GCC_$(_build_)_CMD = $(GCC_$(BUILD)) $(GCC_$(BUILD)_FLAGS)
endif

define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
	$$(call Check_SHC_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
ifdef ASM_MAKE
	@$$(CC_$$(BUILD)_CMD) -code=asmcode -object=$$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	@$$(CC_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	@$$(CC_$$(BUILD)_CMD) -object=$$@ $$<
ifeq ($$(PP_MAKE),2)
	@$$(CC_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	@$$(GCC_$$(BUILD)_CMD) -o $$(basename $$@).tmp $$<;\
    $$(CAT) $$(basename $$@).tmp |\
             $$(SED) -e 's,\(.*\): ,$$(@D)/\1: ,g' >$$(basename $$@).d\
                   -e 's,\(.\)\:\([^ ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
    $$(RM) $$(basename $$@).tmp
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifdef ASM_MAKE
	$$(CC_$$(BUILD)_CMD) -code=asmcode -object=$$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	$$(CC_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	$$(CC_$$(BUILD)_CMD) -object=$$@ $$<
ifeq ($$(PP_MAKE),2)
	$$(CC_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	@$$(ECHO)
	$$(GCC_$$(BUILD)_CMD) -o $$(basename $$@).tmp $$<;\
    $$(CAT) $$(basename $$@).tmp |\
            $$(SED) -e 's,\(.*\): ,$$(@D)/\1: ,g' >$$(basename $$@).d\
                  -e 's,\(.\)\:\([^ ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
    $$(RM) $$(basename $$@).tmp
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C++ files:
###################################
#
# The following variable contains the command for compiling a .c file using
# the HEW SH2A tool chain. Note that it uses recursive expansion. It also
# uses gcc to build the associated dependency (.d) file since the Renesas
# compiler does not support this. sed is used to reformat the .d file output
# such that 1) it includes the relative path to the object file, and 2)
# any absolute DOS paths (e.g., C:/) are converted to Cygwin paths
# (/cygdrive/c).
#
ifndef CPP_$(_build_)_CMD
  CPP_$(_build_)_CMD = $(CPP_$(BUILD)) $(CPP_$(BUILD)_FLAGS)
endif

ifndef GCPP_$(_build_)_CMD
  GCPP_$(_build_)_CMD = $(GCPP_$(BUILD)) $(GCPP_$(BUILD)_FLAGS)
endif

define MAKE_$(_build_)_COMPILE_CPP_RULE
$(1)
	$$(call Check_SHC_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
ifeq ($$(PP_MAKE),1)
	@$$(CPP_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	@$$(CPP_$$(BUILD)_CMD) -object=$$@ $$<
ifeq ($$(PP_MAKE),2)
	@$$(CPP_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	@$$(GCPP_$$(BUILD)_CMD) -o $$(basename $$@).tmp $$<;\
    $$(CAT) $$(basename $$@).tmp |\
             $$(SED) -e 's,\(.*\): ,$$(@D)/\1: ,g' >$$(basename $$@).d\
                   -e 's,\(.\)\:\([^ ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
    $$(RM) $$(basename $$@).tmp
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifeq ($$(PP_MAKE),1)
	$$(CPP_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	$$(CPP_$$(BUILD)_CMD) -object=$$@ $$<
ifeq ($$(PP_MAKE),2)
	$$(CPP_$$(BUILD)_CMD) -preprocessor=$$(basename $$@).i $$<
endif
	@$$(ECHO)
	$$(GCPP_$$(BUILD)_CMD) -o $$(basename $$@).tmp $$<;\
    $$(CAT) $$(basename $$@).tmp |\
            $$(SED) -e 's,\(.*\): ,$$(@D)/\1: ,g' >$$(basename $$@).d\
                  -e 's,\(.\)\:\([^ ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
    $$(RM) $$(basename $$@).tmp
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a static library:
########################################

define MAKE_$(_build_)_LIB_RULE
$(1)
	$$(call Check_SHC_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(RM) -rf $$@;\
    $$(AR_$$(BUILD)) $$(filter %.o,$$^) $$(AR_$$(BUILD)_FLAGS)\
      -output="$$@" -list="$$(dir $$@)../lst/$$(basename $$(notdir $$@)).lbp";\
    $$(TOUCH) $$@
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(RM) -rf $$@;\
    $$(AR_$$(BUILD)) $$(filter %.o,$$^) $$(AR_$$(BUILD)_FLAGS)\
      -output="$$@" -list="$$(dir $$@)../lst/$$(basename $$(notdir $$@)).lbp";\
    $$(TOUCH) $$@
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking building an executable image:
####################################################

ifndef LD_$(_build_)_CMD
define LD_$(_build_)_CMD
  $(LD_$(BUILD)) $(filter %.o,$^) $(if $(filter %.lib,$^),\
    -library=$(subst $(SPACE),$(COMMA),$(filter %.lib,$^)),)\
    $(LD_$(BUILD)_FLAGS)\
    -list="$(dir $@)../lst/$(basename $(notdir $@)).map" -output="$@";\
  $(LD_$(BUILD)) $@ $(LD_$(BUILD)_PTP_FLAGS) -output="$(basename $@).ptp"
endef
endif

define MAKE_$(_build_)_BIN_RULE
$(1)
	$$(call Check_SHC_Definitions)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) and $$(notdir $$(basename $$@)).ptp for $$(BUILD)"
	@$$(LD_$$(BUILD)_CMD);\
    $$(TOUCH) $$@
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "Linking $$(notdir $$@) and $$(notdir $$(basename $$@)).ptp for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LD_$$(BUILD)_CMD);\
    $$(TOUCH) $$@
	@$$(ECHO)
endif
endef

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 19-apr-2010 kirk bailey
# Changed PP_MAKE logic to run preprocessor prior to regular compilation
# if PP_MAKE=1, and afterward if PP_MAKE=2.
#
# 24-jul-2008 kirk bailey
# + Added ability to save assembly language output via ASM_MAKE option.
#
# 25-apr-2008 kirk bailey
# + Changed warnings for SHC_* environment variables to be issued upon
# + the first attempt to build something for SH2A.
#
# 11-dec-2007 kirk bailey
# + Added generation of .i file with preprocessor output if PP_MAKE is defined.
#
# 26-sep-2007 kirk bailey
# + Added "touch" to library generation commands to make sure that the library
# + file's timestamps were updated.
#
# 20-sep-2007 kirk bailey
# + Changed commands to ensure in-order execution with -j option.
#
# 18-sep-2007 kirk bailey
# + Added .ptp build. Corrected -template option. Changed default tool paths.
#
# 12-sep-2007 kirk bailey
# + Added -global_volatile=1 and -template=static options.
#
# 07-sep-2007 kirk bailey
# + Eliminated use of DEFINE_* calls.
#
# 23-may-2007 kirk bailey
# + Changed CCP prefix back to CPP.
#
# 22-may-2007 kirk bailey
# + Fixed SED command for dependency files to handle absolute paths.
# + Changed CPP prefix to CCP.
#
# 03-apr-2007 kirk bailey
# + Renamed MAKE_$(_build_)_COMPILE_RULE definitions. The one for C++ was
# + writing over the one for C. Separated C and C++ options more.
#
# 02-apr-2007 kirk bailey
# + Incorporated NUE changes for building C++.
#
# 17-feb-2007 kirk bailey
# + Replaced CC_BUILD, etc. with BUILD. Fixed C_FLAGS to include _OPT option.
#
# 18-jan-2007 kirk bailey
# + Changed env. variable errors to warnings with default values.
# + Made sure PATH pointed to tool area to prevent strange lbgsh failure.
# + Changed library rule to allow non-object file prerequisites.
# + Fixed executable build to work even if no libraries are specified.
#
# 19-dec-2006 kirk bailey
# + Improved ENV. variable error messages. Use CYGPATH for tool invocations.
#
# 13-dec-2006 kirk bailey
# + Changed cygpath use to $(CYGPATH).
#
# 19-nov-2006 kirk bailey
# + Created initial file.
#
#===========================================================================
