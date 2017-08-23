#===========================================================================
# FILE: iar_k0r_prolog.mak
#===========================================================================
# Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: iar_k0r_prolog.mak~2:makefile:kok_basa#1 %
# %version: 2 %
# %derived_by:dzq92s %
# %date_modified: Thu Jun 28 09:16:33 2012 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================

include $(MAKE_INCL_DIR)/prolog.mak

#
# File extensions for IAR K0R Tools.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .asm
_bin_ext_ := 
_lib_ext_ := $(IAR_K0R_OBJ_EXT_$(_build_))
_lib_prefix_ :=
_obj_ext_ := $(IAR_K0R_OBJ_EXT_$(_build_))

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name    (Description on following lines: SCR #, etc.)
# ----------- --------
# 26-Jun-2012 Kirk Bailey
# + Created initial file.
#===========================================================================
