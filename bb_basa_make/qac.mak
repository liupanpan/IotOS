#===========================================================================
# FILE: qac.mak
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: qac.mak~2:makefile:kok_aud#1 %
# %version: 2 %
# %derived_by: dzq92s %
# %date_modified: Wed Jun 10 07:28:06 2009 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules that can be used to run QAC instead of compile
# code.
#
#===========================================================================

#
# QAC Tool Definitions
#

ifndef QAC_ROOT
  QAC_ROOT := C:/Progra~1/PRQA/QAC-7.0
endif

ifndef QAC_TOOL_PATH
  QAC_TOOL_PATH := $(QAC_ROOT)/bin
endif

ifndef QACBIN
  export QACBIN = $(QAC_TOOL_PATH)
endif

ifndef QACHELPFILES
  export QACHELPFILES = $(QAC_ROOT)/help
endif

ifndef QACTEMP
  export QACTEMP = $(QAC_ROOT)/temp
endif

QAC_$(_build_)  := $(QAC_TOOL_PATH)/qac

ifndef QAC_$(_build_)_FLAGS
  QAC_$(_build_)_FLAGS = -maxerr 10 -op $($(BUILD)_OBJ_OUTDIR)
endif

ERRDSP_$(_build_)  := $(QAC_TOOL_PATH)/errdsp

ifndef ERRDSP_$(_build_)_FLAGS
  ERRDSP_$(_build_)_FLAGS = QAC -maxerr 10 -html+ -op $($(BUILD)_OBJ_OUTDIR)
endif

PRJDSP_$(_build_)  := $(QAC_TOOL_PATH)/prjdsp

ifndef PRJDSP_$(_build_)_FLAGS
  PRJDSP_$(_build_)_FLAGS = QAC -maxerr 10 -html+ -op $($(BUILD)_OBJ_OUTDIR)
endif

###############################################################################
# Commands for generating QAC files:
####################################
#
# The following variable contains the command for compiling a .c file using
# the qac tool chain. Note that it uses recursive expansion.
#

ifndef QAC_$(_build_)_CMD
  QAC_$(_build_)_CMD = $(QAC_$(BUILD)) $(QAC_$(BUILD)_FLAGS) -via $(QAC_$(BUILD)_VIA)
endif

ifndef ERRDSP_$(_build_)_CMD
  ERRDSP_$(_build_)_CMD = $(ERRDSP_$(BUILD)) $(ERRDSP_$(BUILD)_FLAGS) -via $(QAC_$(BUILD)_VIA)
endif

ifndef PRJDSP_$(_build_)_CMD
  PRJDSP_$(_build_)_CMD = $(PRJDSP_$(BUILD)) $(PRJDSP_$(BUILD)_FLAGS) -via $(QAC_$(BUILD)_VIA)
endif

define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
ifndef VERBOSE_MAKE
	$$(if $$(QAC_$$(BUILD)_VIA),,$$(error QAC_$$(BUILD)_VIA must be defined.))
	@$$(ECHO) "  Generating QAC output of $$(notdir $$<)"
	@$$(QAC_$$(BUILD)_CMD) $$<;\
    $$(ERRDSP_$$(BUILD)_CMD) -file $$(subst .c,.html,$$(notdir $$<)) $$<;\
    $$(PRJDSP_$$(BUILD)_CMD) -file $$(subst .c,.errlist.html,$$(notdir $$<)) $$<;\
    $$(TOUCH) $$@
else
	$$(if $$(QAC_$$(BUILD)_VIA),,$$(error QAC_$$(BUILD)_VIA must be defined.))
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Generating QAC output of $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(QAC_$$(BUILD)_CMD) $$<;\
    $$(ERRDSP_$$(BUILD)_CMD) -file $$(subst .c,.html,$$(notdir $$<)) $$<;\
    $$(PRJDSP_$$(BUILD)_CMD) -file $$(subst .c,.errlist.html,$$(notdir $$<)) $$<;\
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
# 23-oct-2007 kirk bailey
# + Created initial file.
#
#===========================================================================
