#===========================================================================
# FILE: prolog.mak
#===========================================================================
# Copyright 2008-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: prolog.mak~kok_inf#18:makefile:kok_aud#5 %
# %version: kok_inf#18 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 08:11:22 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for a module's Makefile. It establishes
# the values of variables describing the "current" module. These can in turn
# be used by the modules Makefile and "epilog.mak".
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# NOTE: Several variables with names of the form _<name>_ are used to
#       communicate values between "prolog.mak", the module's Makefile, and
#       "epilog.mak". Any changes or additions must be coordinated between
#       all of these files!
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
#===========================================================================

#############################################
# Logic to be performed only once per build
#
ifndef $(_build_)_ONLY_ONCE

$(_build_)_ONLY_ONCE = 1 # make sure we only do this once
#
# Define and create the output directories for this build
#
$(_build_)_BIN_OUTDIR := $($(_build_)_OUTPUT_ROOT)/bin
$(_build_)_LIB_OUTDIR := $($(_build_)_OUTPUT_ROOT)/lib
$(_build_)_LST_OUTDIR := $($(_build_)_OUTPUT_ROOT)/lst
$(_build_)_OBJ_OUTDIR := $($(_build_)_OUTPUT_ROOT)/obj
$(_build_)_SHARED_OBJ_DIR := $($(_build_)_OUTPUT_ROOT)/obj/shared
X_IGNORE := $(shell $(MKDIR) -p \
                    $($(_build_)_BIN_OUTDIR) $($(_build_)_LIB_OUTDIR)\
                    $($(_build_)_LST_OUTDIR) $($(_build_)_SHARED_OBJ_DIR))
#
# The following allows a project to create extra directories under the output
# root by defining the list of subdirectories in $(_build_)_EXTRA_OUT_DIRS
#
$(foreach dir,$($(_build_)_EXTRA_OUT_DIRS),\
    $(eval X_IGNORE := $(shell $(MKDIR) -p $($(_build_)_OUTPUT_ROOT)/$(dir))))
#
# Make sure the following are simply-expanded vars, not recursively expanded
#
ALL_$(_build_)_ASM_SRCS :=
ALL_$(_build_)_BINS     :=
ALL_$(_build_)_C_SRCS   :=
ALL_$(_build_)_CPP_SRCS :=
ALL_$(_build_)_LIBS     :=
ALL_$(_build_)_OBJS     :=
$(_build_)_DEP_FILES    :=
$(_build_)_MODULES      :=
$(foreach ext,$(API_EXTENSIONS),$(eval ALL_$(_build_)_LOCAL_$(ext):=))

#
# These target-specific assignments allow $(BUILD) to be used in the command
# section for rules associated with files in the output directories. The
# variables in commands are not expanded until the command is executed; so,
# $(_build_) cannot be used in a command since its value changes throughout
# the evaluation of the makefile.
#
$($(_build_)_BIN_OUTDIR)/%:BUILD:=$(_build_)
$($(_build_)_LIB_OUTDIR)/%:BUILD:=$(_build_)
$($(_build_)_LST_OUTDIR)/%:BUILD:=$(_build_)
$($(_build_)_OBJ_OUTDIR)/%:BUILD:=$(_build_)
$($(_build_)_SHARED_OBJ_DIR)/%:BUILD:=$(_build_)

endif
#############################################

#
# Get the list of makefiles (in order processed) minus any with a .mak extension
#
_non_dot_mak_ := $(filter-out %.mak,$(MAKEFILE_LIST))
#
# Get the relative path to the most recent makefile that included this file.
# This can be used to extract the path to the module's directory.
#
_cur_makefile_ := $(word $(words $(_non_dot_mak_)),$(_non_dot_mak_))
#
# Get the path to the makefile by stripping off the /Makefile.X
#
_cur_module_path_ := $(patsubst %/,%,$(dir $(_cur_makefile_)))
#
# Use the leaf subdirectory's name as the module's name.
#
_cur_module_name_ := $(notdir $(_cur_module_path_))
#
# Form a unique make target for this build of the module
# (<module_name>-<build>).
#
_cur_module_target_ := $(_cur_module_name_)-$(_build_)
#
# Make sure the following variables (used in epilog) are cleared out so they
# don't inherit values from the last Makefile that was processed.
#
_asm_srcs_ :=
_bin_dir_ :=
_bin_name_ :=
_bin_objs_ :=
_bin_lib_dirs_ :=
_bin_libs_ :=
_bin_lnk_cmd_ :=
_c_srcs_ :=
_cpp_srcs_ :=
_cur_module_inc_path_ :=
_find_asm_srcs_ := 1
_find_c_srcs_ := 1
_find_cpp_srcs_ := 1
_incl_list_ :=
_lib_dir_ :=
_lib_lib_dirs_ :=
_lib_libs_ :=
_lib_name_ :=
_lib_objs_ :=
_src_dirs_ :=
# Create a variable that can be used by anyone to get the path to the module
$(_cur_module_name_)_PATH:=$(_cur_module_path_)

# Default for BASA blocks is to NOT disable copying the API files
ifndef BASA_DEFAULT_API_DISABLE
  BASA_DEFAULT_API_DISABLE := n
endif

# Default for ISA blocks is to disable copying the API files
ifndef ISA_DEFAULT_API_DISABLE
  ISA_DEFAULT_API_DISABLE := y
endif

ifeq ($(ISA_BLOCK),1)

  #
  # The object files for a block are put under build's output directory in
  # a subdirectory named after the block:
  #
  #   out/<build>/<block-name>/obj/        <- directory for "static" object files
  #   out/<build>/<block-name>/obj/shared  <- directory for "shared" object files
  #
  ifneq ($(OBJ_DIR_OVERRIDE),)
    $(_cur_module_name_)_OUT:=$($(_build_)_OUTPUT_ROOT)/$(OBJ_DIR_OVERRIDE)
  else
    $(_cur_module_name_)_OUT:=$($(_build_)_OUTPUT_ROOT)/$(_cur_module_name_)
  endif
  _objdir_:=$($(_cur_module_name_)_OUT)/obj
  _objdir_shared_:=$(_objdir_)/shared
  # At the time commands are executed, the $(_build_) variable may have a different
  # value; so, the current value is saved in $(BUILD), which has a target-specific
  # value for any thing in the object file directories.
  $(_objdir_)/%:BUILD:=$(_build_)
  $(_objdir_shared_)/%:BUILD:=$(_build_)
  
  ifndef $(_cur_module_name_)_NO_API
    $(_cur_module_name_)_NO_API := $(ISA_DEFAULT_API_DISABLE)
  endif
  
else

  #
  # Traditional BASA blocks place all object files in the same directory
  #
  #   out/<build>/obj/        <- directory for "static" object files
  #   out/<build>/obj/shared  <- directory for "shared" object files
  #
  $(_cur_module_name_)_OUT:=$($(_build_)_OUTPUT_ROOT)
  _objdir_:=$($(_cur_module_name_)_OUT)/obj
  _objdir_shared_:=$(_objdir_)/shared
  
  ifndef $(_cur_module_name_)_NO_API
    $(_cur_module_name_)_NO_API := $(BASA_DEFAULT_API_DISABLE)
  endif
  
endif
#
# If everything is built as position-independent code, then there is no
# difference between object files for shared libraries vs. "normal" object
# files.
#
ifeq ($(PIC_ALL_$(_build_)),1)
  _objdir_shared_ := $(_objdir_)
endif

ifeq ($(ISA_BLOCK),1)
  # Create the object file directory if it's not already there
  IGNORE := $(shell $(MKDIR) -p $(_objdir_shared_))
endif

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
#
# ...Revision details prior to March 2011 removed...
#
# 17-Apr-2014 Kirk Bailey
# + Added _lib_libs_ and _lib_lib_dirs_.
#
# 08-Aug-2014 Kirk Bailey Rev 17
# + Added logic to support ISA_BLOCKs.
#
# 27-Oct-2014 Kirk Bailey Rev 18
# + Moved target-specific assignment of BB_NAME to epilog and made it work
# + for BASA blocks too.
# + Added ENABLE_NEW_API_LOGIC option for public header files
#
#===========================================================================
