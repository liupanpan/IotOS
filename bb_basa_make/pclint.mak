#===========================================================================
# FILE: pclint.mak
#===========================================================================
# Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: pclint.mak~kok_basa#6:makefile:kok_aud#1 %
# %version: kok_basa#6 %
# %derived_by: dzq92s %
# %date_modified: Wed Oct 20 09:06:54 2010 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules that can be used to run pclint instead of compile
# code.
#
#===========================================================================

#
# PC Lint Tool Definitions
#

ifndef LINT_$(_build_)_PATH
  LINT_$(_build_)_PATH := C:/Lint
endif

ifndef LINT_$(_build_)
  LINT_$(_build_) := $(LINT_$(_build_)_PATH)/lint-nt
endif

ifndef LINT_$(_build_)_BASE
  LINT_$(_build_)_BASE := -v -b -zero
endif

ifndef LINT_$(_build_)_FMT
  LINT_$(_build_)_FMT := env-xml.lnt 
endif

LINT_$(_build_)_CC_INCL =\
  $(patsubst %,-i"%",$(strip $(patsubst -I%,%,$(CC_$(BUILD)_INCL))))

ifndef LINT_$(_build_)_INCL
  LINT_$(_build_)_INCL =\
    $(subst "",",$(patsubst %,-i"%",$(INCL_DIRS) $($(BUILD)_TOOL_INCL_PATHS)) $(LINT_$(BUILD)_CC_INCL) -i"$(MAKE_INCL_DIR)" -i"$(LINT_$(BUILD)_PATH)/lnt")
endif

ifndef LINT_$(_build_)_LIBS
  LINT_$(_build_)_LIBS =
endif

ifndef LINT_$(_build_)_OPTIONS
  LINT_$(_build_)_OPTIONS := std.lnt
endif

ifndef LINT_$(_build_)_CPP_OPTIONS
  LINT_$(_build_)_CPP_OPTIONS =
endif

ifndef LINT_$(_build_)_FLAGS
  LINT_$(_build_)_FLAGS = $(LINT_$(BUILD)_BASE) $(LINT_$(BUILD)_LIBS) $(LINT_$(BUILD)_INCL) $(LINT_$(BUILD)_OPTIONS) $(LINT_$(BUILD)_FMT)
endif

ifndef LINT_$(_build_)_OUTDIR
  LINT_$(_build_)_OUTDIR = $(dir $@)
endif


###############################################################################
# Commands for generating LINT files:
####################################

ifndef LINT_$(_build_)_CMD
  LINT_$(_build_)_CMD = $(LINT_$(BUILD)) $(LINT_$(BUILD)_FLAGS)
endif

#
# The following variable contains the command for "compiling" a .c file using
# the pclint tool chain. Note that it uses recursive expansion.
#
define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Generating LINT output for $$(notdir $$<)"
	@$$(LINT_$$(BUILD)_CMD) $$< > $$(LINT_$$(BUILD)_OUTDIR)$$(notdir $$<).xml
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Generating LINT output for $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LINT_$$(BUILD)_CMD) $$< > $$(LINT_$$(BUILD)_OUTDIR)$$(notdir $$<).xml
	@$$(ECHO)
endif
endef

#
# The following variable contains the command for "compiling" a .cpp file using
# the pclint tool chain. Note that it uses recursive expansion.
#
define MAKE_$(_build_)_COMPILE_CPP_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Generating LINT output for $$(notdir $$<)"
	@$$(LINT_$$(BUILD)_CMD) $$(LINT_$$(BUILD)_CPP_OPTIONS) $$< >$$(LINT_$$(BUILD)_OUTDIR)$$(notdir $$<).xml
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Generating LINT output for $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LINT_$$(BUILD)_CMD) $$(LINT_$$(BUILD)_CPP_OPTIONS) $$< >$$(LINT_$$(BUILD)_OUTDIR)$$(notdir $$<).xml
	@$$(ECHO)
endif
endef

define MAKE_$(_build_)_LIB_RULE
$(1)
	@$$(ECHO) "  PCLint has analyzed compilation units in $$(notdir $$@) for $$(BUILD)"
endef

define MAKE_$(_build_)_SHARED_LIB_RULE
$(1)
	@$$(ECHO) "  PCLint has analyzed compilation units in $$(notdir $$@) for $$(BUILD)"
endef

define MAKE_$(_build_)_BIN_RULE
$(1)
	@$$(ECHO) "  PCLint has analyzed compilation units in $$(notdir $$@) for $$(BUILD)"
endef

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 14-oct-2010 kirk bailey
# + Added rules for libraries and executables to support "make <build>".
#
# 03-feb-2010 kirk bailey
# + Task kok_basa 5710:Added LINT_$$(BUILD)_CPP_OPTIONS.
#
# 05-jan-2010 kirk bailey
# + Fixed output directory for C++ analysis to be same as for C.
# + Removed double quotes from include directives to resolve issues with
# + "unable to open include file" errors.
#
# 29-oct-2009 kirk bailey
# + Expanded LINT_$(_build_)_INCL to contain any additional paths that
#   modules added to CC_$(_build_)_INCL so that those modules' header
#   files would also be visible to PCLint.
#
# 26-jun-2009 kirk bailey
# + Added LINT_$(_build_)_OUTDIR.
#
# 16-jun-2009 kirk bailey
# + Created initial file.
#
#===========================================================================
