#===========================================================================
# FILE: win32_gcc_def.mak
#===========================================================================
# Copyright 2010-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: win32_gcc_def.mak~kok_inf#34:makefile:kok_aud#5 %
# %version: kok_inf#34 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 09:41:18 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================

#
# gcc Tool Definitions
#
ifndef GCC_$(_build_)_PATH
  GCC_$(_build_)_PATH := /cygdrive/c/cygwin/bin/
endif
AR_$(_build_)  := $(GCC_$(_build_)_PATH)ar
ifndef AS_$(_build_)
  AS_$(_build_)  := $(GCC_$(_build_)_PATH)gcc
endif

ifndef CC_$(_build_)
  CC_$(_build_)  := $(GCC_$(_build_)_PATH)gcc
endif

ifndef CPP_$(_build_)
  CPP_$(_build_) := $(GCC_$(_build_)_PATH)g++
endif

ifndef LD_$(_build_)
  LD_$(_build_)  := $(GCC_$(_build_)_PATH)gcc
endif

###############################################################################
# Individual gcc Tool Options:
#
# Note that these are only defined default values for the build if they
# have not already been defined.
#

ifndef CC_$(_build_)_BASE
  CC_$(_build_)_BASE := -c
endif

ifndef CC_$(_build_)_BT
  CC_$(_build_)_BT := -fasynchronous-unwind-tables -fexceptions -fnon-call-exceptions -funwind-tables
endif

#
# This option is primarily for building XSAL for Win32 under
# Cygwin while using Windows sockets. An alternative is to replace
# the value with "-mno-cygwin", which will perform a build
# that does not depend upon the Cygwin DLL (like MinGW). Such a
# build, however, loses visibility to ALL Cygwin facilities (like
# termios.h support for UART). Note similar options exist for CPP_
# and LD_
#
ifndef CC_$(_build_)_CYGOPT
  CC_$(_build_)_CYGOPT := -DUSE_CYGWIN -DWIN32 -D__USE_W32_SOCKETS
endif

ifndef CC_$(_build_)_DBG
  CC_$(_build_)_DBG := -g
endif

ifndef CC_$(_build_)_GCOV
  ifdef GCOV_MAKE
    CC_$(_build_)_GCOV := -fprofile-arcs -ftest-coverage
  else
    CC_$(_build_)_GCOV :=
  endif
endif

ifndef CC_$(_build_)_OPT
  CC_$(_build_)_OPT := -O0
endif

ifndef CC_$(_build_)_WARN
  CC_$(_build_)_WARN := -ansi -Wall -std=c89 -Wswitch-default -Wdeclaration-after-statement -Wfloat-equal -Wshadow -Wundef -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
endif


ifndef CPP_$(_build_)_BASE
  CPP_$(_build_)_BASE := -c
endif

ifndef CPP_$(_build_)_BT
  CPP_$(_build_)_BT := -fasynchronous-unwind-tables -fexceptions -fnon-call-exceptions -funwind-tables
endif

ifndef CPP_$(_build_)_CYGOPT
  CPP_$(_build_)_CYGOPT := -DUSE_CYGWIN -DWIN32 -D__USE_W32_SOCKETS
endif

ifndef CPP_$(_build_)_DBG
  CPP_$(_build_)_DBG := -g
endif

ifndef CPP_$(_build_)_GCOV
  ifdef GCOV_MAKE
    CPP_$(_build_)_GCOV := -fprofile-arcs -ftest-coverage
  else
    CPP_$(_build_)_GCOV :=
  endif
endif

ifndef CPP_$(_build_)_OPT
  CPP_$(_build_)_OPT := -O0
endif

ifndef CPP_$(_build_)_WARN
  CPP_$(_build_)_WARN := -ansi -Wall -Wundef -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
endif


ifndef LD_$(_build_)_CYGOPT
  LD_$(_build_)_CYGOPT :=
endif

ifndef LD_$(_build_)_GCOV
  ifdef GCOV_MAKE
    LD_$(_build_)_GCOV := -fprofile-arcs -ftest-coverage
  else
    LD_$(_build_)_GCOV :=
  endif
endif
###############################################################################

###############################################################################
# Composite gcc Tool Options:
###############################
#
# Options for archiving (generating a static library)
#
ifndef AR_$(_build_)_FLAGS
  AR_$(_build_)_FLAGS = rcs
endif
#
# Options for the assembler
#
ifndef AS_$(_build_)_FLAGS
  AS_$(_build_)_FLAGS = 
endif
#
# Options for the C compiler
#
ifndef CC_$(_build_)_FLAGS
  CC_$(_build_)_FLAGS =\
    $(CC_$(BUILD)_BASE) $(CC_$(BUILD)_BT) $(CC_$(BUILD)_CYGOPT) $(CC_$(BUILD)_WARN)\
    $(CC_$(BUILD)_DBG)  $(CC_$(BUILD)_OPT) $(CC_$(BUILD)_GCOV) $(CC_$(BUILD)_MACRO)\
    $(patsubst %,-I %,$(INCL_DIRS))
endif
#
# Options for the C++ compiler
#
ifndef CPP_$(_build_)_FLAGS
  CPP_$(_build_)_FLAGS =\
    $(CPP_$(BUILD)_BASE) $(CPP_$(BUILD)_BT) $(CPP_$(BUILD)_CYGOPT) $(CPP_$(BUILD)_WARN)\
    $(CPP_$(BUILD)_DBG)  $(CPP_$(BUILD)_OPT) $(CPP_$(BUILD)_GCOV) $(CPP_$(BUILD)_MACRO)\
    $(patsubst %,-I %,$(INCL_DIRS))
endif
#
# Options for linking to form an executable image
#
ifndef LD_$(_build_)_FLAGS
 LD_$(_build_)_FLAGS = $(LD_$(BUILD)_CYGOPT) $(LD_$(BUILD)_GCOV)
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
	@$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$< 
endif
	@$$(CC_$$(BUILD)) $$(CC_$$(BUILD)_FLAGS) -MMD -MP -MT $$@ -o $$@ $$<
ifdef ASM_MAKE
	@$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	@$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$< 
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
	$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	$$(CC_$$(BUILD)) $$(CC_$$(BUILD)_FLAGS) -MMD -MP -MT $$@ -o $$@ $$<
ifdef ASM_MAKE
	$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
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
ifdef ASM_MAKE
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	@$$(CPP_$$(BUILD)) $$(CPP_$$(BUILD)_FLAGS) -MMD -MP -MT $$@ -o $$@ $$<
ifeq ($$(PP_MAKE),2)
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifdef ASM_MAKE
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	$$(CPP_$$(BUILD)) $$(CPP_$$(BUILD)_FLAGS) -MMD -MP -MT $$@ -o $$@ $$<
ifeq ($$(PP_MAKE),2)
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
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
	@$$(RM) -f $$@ && $$(AR_$$(BUILD)) $$(AR_$$(BUILD)_FLAGS) $$@ $$(filter %.o,$$^) 2>/dev/null
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(RM) -f $$@ && $$(AR_$$(BUILD)) $$(AR_$$(BUILD)_FLAGS) $$@ $$(filter %.o,$$^)
	@$$(ECHO)
endif
endef

##############################################################################
# Commands for linking a shared library:
########################################

###SO_EXT := .1

ifndef SO_$(_build_)_CMD

  define SO_$(_build_)_CMD
    /usr/bin/ld -shared --unresolved-symbols=ignore-all\
                -soname,$(notdir $@)$(SO_EXT) -o $@ $(filter %.o,$^)
  endef

endif # ifndef SO_$(_build_)_CMD

define MAKE_$(_build_)_SHARED_LIB_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(SO_$$(BUILD)_CMD)
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(SO_$$(BUILD)_CMD)
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking building an executable image:
####################################################

ifndef LD_$(_build_)_CMD

  define LD_$(_build_)_CMD
    $(LD_$(BUILD)) $(LD_$(BUILD)_FLAGS) -o $@ $(filter %.o,$^)\
                   $(addprefix -L,$(LIB_DIRS)) -Wl,--start-group\
                   $(addprefix -l,$(LIBS)) $(NON_STD_LIBS) -Wl,--end-group
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
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# Revision history truncated. See verion 32 of the file to previous revision
# comments.
#
# 01-Oct-2013 Larry Piekarski Rev 33
# + Task 1486: Enabled -Wextra warning. -Wextra is a collection of new
#   warnings, and disabled 2 that are not desirable. -Wunused-parameter 
#   issues a warning if argument in a function is not used. 
#   -Wmissing-field-initializers gives a warning for initializing a
#   structure wtih {0} instead of explicitly initializing each field. 
#   Both of these are common design patterns in IDI and we don't want to 
#   make them warnings, so those two were specifically disabled. Also
#   added -Wundef to warn the preprocessor uses symbols that are not 
#   defined.
#
# 12-Nov-2014 Kirk Bailey Rev 34
# + Switch from using ';' to '&&' for command lines with multiple commands
# + so that the return code reflects any failures.
#===========================================================================
