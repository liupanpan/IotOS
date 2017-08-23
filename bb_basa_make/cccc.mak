#===========================================================================
# FILE: cccc.mak
#===========================================================================
# Copyright 2009 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: cccc.mak~2:makefile:kok_aud#1 %
# %version: 2 %
# %derived_by: dzq92s %
# %date_modified: Tue Jul 21 12:59:44 2009 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules that can be used to run cccc instead of compile
# code.
#
#===========================================================================

#
# CCCC Tool Definitions
#

ifndef CCCC_$(_build_)_PATH
  CCCC_$(_build_)_PATH := C:/cccc
endif

ifndef CCCC_$(_build_)
  CCCC_$(_build_) := $(CCCC_$(_build_)_PATH)/cccc
endif

CCCC_$(_build_)_FMT_HTML = --html_outfile=$(CCCC_$(BUILD)_HTML_OUTDIR)/$(notdir $<).html
CCCC_$(_build_)_FMT_XML = --xml_outfile=$(CCCC_$(BUILD)_XML_OUTDIR)/$(notdir $<).xml

ifndef CCCC_$(_build_)_OPTIONS
  CCCC_$(_build_)_OPTIONS =
endif

ifndef CCCC_$(_build_)_FLAGS
  CCCC_$(_build_)_FLAGS = $(CCCC_$(BUILD)_OPTIONS) $(CCCC_$(BUILD)_FMT_HTML) $(CCCC_$(BUILD)_FMT_XML)
endif

ifndef CCCC_$(_build_)_HTML_OUTDIR
  CCCC_$(_build_)_HTML_OUTDIR = $(dir $@)
endif

ifndef CCCC_$(_build_)_XML_OUTDIR
  CCCC_$(_build_)_XML_OUTDIR = $(dir $@)
endif


###############################################################################
# Commands for generating CCCC files:
####################################

ifndef CCCC_$(_build_)_CMD
  CCCC_$(_build_)_CMD = $(CCCC_$(BUILD)) $(CCCC_$(BUILD)_FLAGS)
endif

#
# The following variable contains the command for "compiling" a .c file using
# the cccc tool chain. Note that it uses recursive expansion.
#
define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Generating CCCC output for $$(notdir $$<)"
	@$$(CCCC_$$(BUILD)_CMD) $$< > $$(CCCC_$$(BUILD)_OUTDIR)$$(notdir $$<).xml
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Generating CCCC output for $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(CCCC_$$(BUILD)_CMD) $$< > $$(CCCC_$$(BUILD)_OUTDIR)$$(notdir $$<).xml
	@$$(ECHO)
endif
endef

#
# The following variable contains the command for "compiling" a .cpp file using
# the cccc tool chain. Note that it uses recursive expansion.
#
define MAKE_$(_build_)_COMPILE_CPP_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Generating CCCC output for $$(notdir $$<)"
	@$$(CCCC_$$(BUILD)_CMD) $$< >$$(dir $$@)$$(notdir $$<).xml
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Generating CCCC output for $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(CCCC_$$(BUILD)_CMD) $$< >$$(dir $$@)$$(notdir $$<).xml
	@$$(ECHO)
endif
endef

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 15-jul-2009 matt wavra
# + Corrected CCCC_$(_build_)_OUTDIR override issue where contents of OUTDIR 
#   files were empty.
#
# 09-jul-2009 matt wavra
# + Created initial file.
#
#===========================================================================
