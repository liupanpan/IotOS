#===========================================================================
# FILE: sysbios_arm_def.mak
#===========================================================================
# Copyright 2012-2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: sysbios_arm_def.mak~kok_inf#5:makefile:kok_basa#1 %
# %version: kok_inf#5 %
# %derived_by: dzq92s %
# %date_modified: Wed Jan  7 10:50:41 2015 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================

#
# The following environment variables are required for the TI Code Composer
# tools to operate properly
#
ifndef TI_CCS_TOOL_PATH
  TI_CCS_TOOL_PATH_NOT_DEFINED:=1
  TI_CCS_TOOL_PATH := C:/ti/ccsv5/tools/compiler/arm_5.0.5/bin
  $(warning ENVIRONMENT variable TI_CCS_TOOL_PATH is not defined. Using default value of $(TI_CCS_TOOL_PATH))
endif

ifndef TI_CCS_INC_PATH
  TI_CCS_INC_PATH := $(subst bin,include,$(TI_CCS_TOOL_PATH))
endif

ifndef TI_CCS_LIB_PATH
  TI_CCS_LIB_PATH := $(subst bin,lib,$(TI_CCS_TOOL_PATH))
endif

ifndef TI_LIBS
  TI_LIBS         := "libc.a"
endif

ifndef SYSBIOS_CFG_PKG
  SYSBIOS_CFG_PKG := ./out/configPkg
endif

#
# TI Code Composer Tool Definitions
#
ifndef AR_$(_build_)
AR_$(_build_)   := $(TI_CCS_TOOL_PATH)/armar
endif

ifndef AS_$(_build_)
AS_$(_build_)   := $(TI_CCS_TOOL_PATH)/armasm
endif
ifndef CC_$(_build_)
CC_$(_build_)   := $(TI_CCS_TOOL_PATH)/armcl
endif
ifndef CPP_$(_build_) 
CPP_$(_build_)  := $(TI_CCS_TOOL_PATH)/armcl
endif
ifndef LD_$(_build_)
LD_$(_build_)   := $(TI_CCS_TOOL_PATH)/armcl
endif


###############################################################################
# Individual armcl Tool Options:
#
# Note that these are only defined default values for the build if they
# have not already been defined.
#

#####################################################################
# C
#####################################################################
ifndef CC_$(_build_)_BASE
  CC_$(_build_)_BASE := -DSYSBIOS -DTI814x -DUSER_CORE_DUCATI_M3_1 -eo=.o -c -me --abi=eabi
endif

#####################################################################
# Selects processor version: ARM V4 (ARM7), ARM V5e (ARM9E),
# ARM V6 (ARM11), ARM V7A8 (Cortex-A8), ARM V7M3 (Cortex-M3),
# ARM V7M4 (Cortex-M4), OR ARM V7R (Cortex-R4), . The default is
# ARM V4.
#  -mv={4|5e|6|7A8|7M3|7M4|7R4}
#
ifndef CC_$(_build_)_CPU
  CC_$(_build_)_CPU := -mv=7M3 
endif

#####################################################################
# --symdebug:dwarf Generates directives that are used by the C/C++ source-level debugger
# and enables assembly source debugging in the assembler. The
# --symdebug:dwarf option's short form is -g. The --symdebug:dwarf option
# disables many code generator optimizations, because they disrupt the
# debugger. You can use the --symdebug:dwarf option with the --opt_level
# (aliased as -O) option to maximize the amount of optimization that is
# compatible with debugging
#
ifndef CC_$(_build_)_DBG
  CC_$(_build_)_DBG := -g  
endif

ifndef CC_$(_build_)_GCOV
  ifdef GCOV_MAKE
    CC_$(_build_)_GCOV :=
  else
    CC_$(_build_)_GCOV :=
  endif
endif

#####################################################################
#• --opt_level=0 or -O0
#– Performs control-flow-graph simplification
#– Allocates variables to registers
#– Performs loop rotation
#– Eliminates unused code
#– Simplifies expressions and statements
#– Expands calls to functions declared inline
#• --opt_level=1 or -O1
# Performs all --opt_level=0 (-O0) optimizations, plus:
#– Performs local copy/constant propagation
#– Removes unused assignments
#– Eliminates local common expressions
#• --opt_level=2 or -O2
# Performs all --opt_level=1 (-O1) optimizations, plus:
#– Performs loop optimizations
#– Eliminates global common subexpressions
#– Eliminates global unused assignments
#– Performs loop unrolling
# The optimizer uses --opt_level=2 (-O2) as the default if you use --opt_level
# (-O) without an optimization level.
#• --opt_level=3 or -O3
# Performs all --opt_level=2 (-O2) optimizations, plus:
#– Removes all functions that are never called
#– Simplifies functions with return values that are never used
#– Inlines calls to small functions
#– Reorders function declarations; the called functions attributes are known
#  when the caller is optimized
#– Propagates arguments into function bodies when all calls pass the same
#  value in the same argument position
#– Identifies file-level variable characteristics
#• --opt_level=4 or -O4
#Performs link-time optimization. 
#####################################################################
ifndef CC_$(_build_)_OPT
  CC_$(_build_)_OPT := -O2
endif

ifndef CC_$(_build_)_WARN
  CC_$(_build_)_WARN :=
endif

#####################################################################
# C++
#####################################################################
ifndef CPP_$(_build_)_BASE
  CPP_$(_build_)_BASE := -DSYSBIOS -DTI814x -DUSER_CORE_DUCATI_M3_1 -eo=.o -c -me --abi=eabi
endif

ifndef CPP_$(_build_)_CPU
  CPP_$(_build_)_CPU := -mv=7M3
endif

ifndef CPP_$(_build_)_DBG
  CPP_$(_build_)_DBG := -g  
endif

ifndef CPP_$(_build_)_GCOV
  ifdef GCOV_MAKE
    CPP_$(_build_)_GCOV :=
  else
    CPP_$(_build_)_GCOV :=
  endif
endif

ifndef CPP_$(_build_)_OPT
  CPP_$(_build_)_OPT := -O2
endif

ifndef CPP_$(_build_)_WARN
  CPP_$(_build_)_WARN :=
endif

#####################################################################
# Linker
#####################################################################
ifndef LD_$(_build_)_BASE
  LD_$(_build_)_BASE := $(CC_$(_build_)_CPU) -g --diag_warning=225  --abi=eabi
endif

ifndef LD_$(_build_)_GCOV
  ifdef GCOV_MAKE
    LD_$(_build_)_GCOV :=
  else
    LD_$(_build_)_GCOV :=
  endif
endif

###############################################################################

###############################################################################
# Composite Tool Options:
###############################
#
# Options for archiving (generating a static library)
#
ifndef AR_$(_build_)_FLAGS
  AR_$(_build_)_FLAGS = rq
endif
#
# Options for the C compiler
#
ifndef CC_$(_build_)_FLAGS
  CC_$(_build_)_FLAGS =\
    $(CC_$(BUILD)_CPU) --obj_directory=$(@D)\
    $(CC_$(BUILD)_BASE) $(CC_$(BUILD)_WARN) $(CC_$(BUILD)_DBG)\
    $(CC_$(BUILD)_OPT) $(CC_$(BUILD)_GCOV) $(CC_$(BUILD)_MACRO)\
    $(patsubst %,-I%,$(INCL_DIRS)) $(patsubst %,-I%,$(TI_CCS_INC_PATH))\
    --cmd_file=$(SYSBIOS_CFG_PKG)/compiler.opt
endif
#
# Options for the C++ compiler
#
ifndef CPP_$(_build_)_FLAGS
  CPP_$(_build_)_FLAGS =\
    $(CPP_$(BUILD)_CPU) --obj_directory=$(@D)\
    $(CPP_$(BUILD)_BASE) $(CPP_$(BUILD)_WARN) $(CPP_$(BUILD)_DBG)\
    $(CPP_$(BUILD)_OPT) $(CPP_$(BUILD)_GCOV) $(CPP_$(BUILD)_MACRO)\
    $(patsubst %,-I%,$(INCL_DIRS)) $(patsubst %,-I%,$(TI_CCS_INC_PATH))\
    --cmd_file=$(SYSBIOS_CFG_PKG)/compiler.opt
endif
#
# Options for linking to form an executable image
#
ifndef LD_$(_build_)_FLAGS
  LD_$(_build_)_FLAGS =\
    $(LD_$(BUILD)_BASE) -z -m=$(basename $@).map -I$(TI_CCS_LIB_PATH)\
    -l $(SYSBIOS_CFG_PKG)/linker.cmd --reread_libs --rom_model -l $(TI_LIBS)
endif
###############################################################################


###############################################################################
# Commands for compiling C files:
#################################
#
# The following variable contains the command for compiling a .c file using
# the gcc tool chain. Note that it uses recursive expansion.
#

define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
ifeq ($$(PP_MAKE),1)
	@$$(ECHO) "  PreCompiling $$(notdir $$<)"
	@$$(CC_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CC_$$(BUILD)_FLAGS) $$< 
endif
	@$$(CC_$$(BUILD)) --preproc_with_compile --preproc_dependency="$$(basename $$@).tmp" $$(CC_$$(BUILD)_FLAGS) $$<;\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
         $$(RM) $$(basename $$@).tmp
ifdef ASM_MAKE
	@$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	@$$(ECHO) "  PreCompiling $$(notdir $$<) for $$(BUILD)"
	@$$(CC_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CC_$$(BUILD)_FLAGS) $$< 
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifeq ($$(PP_MAKE),1)
	@$$(ECHO) "  PreCompiling $$(notdir $$<)"
	$$(CC_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CC_$$(BUILD)_FLAGS) $$< 
	@$$(ECHO)
endif
	$$(CC_$$(BUILD)) --preproc_with_compile --preproc_dependency="$$(basename $$@).tmp" $$(CC_$$(BUILD)_FLAGS) $$<;\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
         $$(RM) $$(basename $$@).tmp
ifdef ASM_MAKE
	$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	@$$(ECHO)
	@$$(ECHO) "  PreCompiling $$(notdir $$<) for $$(BUILD)"
	$$(CC_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CC_$$(BUILD)_FLAGS) $$< 
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C++ files:
###################################
#
# The following variable contains the command for compiling a .c file using
# the gcc tool chain. Note that it uses recursive expansion.
#

define MAKE_$(_build_)_COMPILE_CPP_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
ifeq ($$(PP_MAKE),1)
	@$$(ECHO) "  PreCompiling $$(notdir $$<)"
	@$$(CPP_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CPP_$$(BUILD)_FLAGS) $$< 
endif
	@$$(CPP_$$(BUILD)) --preproc_with_compile --preproc_dependency="$$(basename $$@).tmp" $$(CPP_$$(BUILD)_FLAGS) $$<;\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
         $$(RM) $$(basename $$@).tmp
ifdef ASM_MAKE
	@$$(CPP_$$(BUILD)) -S $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	@$$(ECHO) "  PreCompiling $$(notdir $$<) for $$(BUILD)"
	@$$(CPP_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CPP_$$(BUILD)_FLAGS) $$< 
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifeq ($$(PP_MAKE),1)
	@$$(ECHO) "  PreCompiling $$(notdir $$<)"
	$$(CPP_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CPP_$$(BUILD)_FLAGS) $$< 
	@$$(ECHO)
endif
	$$(CPP_$$(BUILD)) --preproc_with_compile --preproc_dependency="$$(basename $$@).tmp" $$(CPP_$$(BUILD)_FLAGS) $$<;\
    $$(CAT) $$(basename $$@).tmp |\
         $$(SED) -e 's,\(.\)\:\([^\t ]\),/cygdrive/\1\2,g' >$$(basename $$@).d;\
         $$(RM) $$(basename $$@).tmp
ifdef ASM_MAKE
	$$(CPP_$$(BUILD)) -S $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	@$$(ECHO)
	@$$(ECHO) "  PreCompiling $$(notdir $$<) for $$(BUILD)"
	$$(CPP_$$(BUILD)) --preproc_with_comment --pp_directory=$($(_build_)_OBJ_OUTDIR) $$(CPP_$$(BUILD)_FLAGS) $$< 
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a static library:
########################################

define MAKE_$(_build_)_LIB_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(AR_$$(BUILD)) $$(AR_$$(BUILD)_FLAGS) $$@ $$(filter %.o,$$^) 2>/dev/null
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
endif # ifndef VERBOSE_MAKE
endef # MAKE_$(_build_)_LIB_RULE

###############################################################################
# Commands for linking building an executable image:
####################################################

ifndef LD_$(_build_)_CMD

define LD_$(_build_)_CMD
  $(LD_$(BUILD)) $(LD_$(BUILD)_FLAGS) $(LNK_CMD_FILE) -o $@ $(filter %.o,$^)\
                    $(addprefix -l ,$(filter %.a,$^))		   
endef

endif # LD_$(_build_)_CMD

define MAKE_$(_build_)_BIN_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(LD_$$(BUILD)_CMD)
else
	@$$(ECHO) "###############################################################"
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

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
#
# 07-jan-2015 heiko pohle
# + adaptation to new TI ARM compiler 5.0.5
#
# 07-sep-2012 Mesut Sözüçok 
# + adaptaion for the new TI Compiler and for Cortex M4
#
# 10-apr-2013 heiko pohle
# + corrected echoing of compiler output in VERBOSE_MAKE mode
#
# 10-sep-2012 heiko pohle
# + corrected PP_MAKE options, for =1 and =2 should be the same, the difference
#   is only when it will happen, before or after the compile run
# + general cleaned up 
#
# 07-sep-2012 Mesut Sözüçok 
# xdctools and bios path took out is not needed
#
# 2-Aug-2011 Mesut Sözüçok 
# + dependency file generation add
# 
# 27-juli-2011 Mesut Sözüçok 
# + changes for CCS5.2.0.00069_win32 includes also new compiler tms470_4.9.1
#
# 31-may-2012 heiko pohle
# + corrected TI_CCS_LIB_PATH 
# + created new variable SYSBIOS_CFG_PKG to be used with sysbios specific 
#   compiler.opt and linker.cmd
# + added linker.cmd to LD_$(_build)_FLAGS
#
# 16-may-2012 heiko pohle
# + file name change: sysbios_m3_** to sysbios_arm_**
#
# 11-may-2012 heiko pohle
# + changed CC_$(_build_)_BASE, CPP_$(_build_)_BASE, CC_$(_build_)_FLAGS,
#   CPP_$(_build_)_BASE, MAKE_$(_build_)_COMPILE_C_RULE and
#   MAKE_$(_build_)_COMPILE_CPP_RULE to correct configuration for 
#   PP_MAKE and VERBOSE_MAKE 
#
# 12-mar-2012 heiko pohle
# + removed standard inc path from TI_CCS_INC_PATH again as not needed
#
# 09-mar-2012 heiko pohle
# + added standard inc path to TI_CCS_INC_PATH
#
# 07-dec-2011 heiko pohle Mesut Sözüçok
# + folder path changes for configPkg and -DSYSBIOS added 
#
# 01-dec-2011 heiko pohle
# + initial version for VAG MIB Standard 2.0
#
# 26-Oct-2011 kirk bailey
# + Created initial file.
#
#===========================================================================
