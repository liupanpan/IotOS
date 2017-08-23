#===========================================================================
# FILE: uitron_arm_rvct_prolog.mak
#===========================================================================
# Copyright 2008 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: uitron_arm_rvct_prolog.mak~2:makefile:kok_aud#1 %
# %version: 2 %
# %derived_by: dzq92s %
# %date_modified: Wed Jun 10 07:28:19 2009 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================

include $(MAKE_INCL_DIR)/prolog.mak

#
# File extensions for RVCT tools for the ARM.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .s
_bin_ext_ := .axf
_lib_ext_ := .a
_lib_prefix_ := lib
_obj_ext_ := .o

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 02-may-2008 kirk bailey
# + Created initial file.
#
#===========================================================================
