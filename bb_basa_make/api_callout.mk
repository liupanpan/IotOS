#===========================================================================
# FILE: api_callout.mk
#===========================================================================
# Copyright 2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: api_callout.mk~1:makefile:kok_inf#1 %
# %version: 1 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 08:11:10 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# If this file exists in the same directory as the root Makefile, then it
# is included after all of the blocks' makefiles have been processed but
# prior to the generation of rules for populating the common API directories.
# This allows the project to specify directories that are not part of a
# block (or individual header files in such directories) that are to be
# included in the common API directories.
#
#===========================================================================

#
# Set proj_api_dirs to the list of directories (relative to the root Makefile)
# whose header files are to be copied to the "common" area (./out/include/)
#
proj_api_dirs :=\
###  $(wildcard ./*_api bb_reuse/*_api bbc_trace/*_api)

$(eval $(call Add_API_Paths,$(proj_api_dirs)))

#
# If there are directories whose entire list of header files should NOT be
# copied to the common area, but which have some files which should be copied
# then those files can be specifically added to ALL_API_HDRS.
#
ALL_API_HDRS +=\
###  $(wildcard Globals/public/xsal*.h)

#
# It is possible to also copy directory hierarchies (or create links)
# into the common area.
#
###dst_dir := $(COMMON_INC_PATH)/json
###hdr_list := $(wildcard ./AP_json-c/json/*.h)
###ignore := $(shell mkdir -p $(dst_dir))
###$(eval $(call Create_Cp_Ln_Rules,$(hdr_list),$(dst_dir),includes))


#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 29-Oct-2014 Kirk Bailey
# + Created initial version
#
#===========================================================================
