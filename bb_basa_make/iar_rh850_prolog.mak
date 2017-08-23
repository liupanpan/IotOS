#===========================================================================
# FILE: iar_rh850_prolog.mak
#===========================================================================
# Copyright 2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: iar_rh850_prolog.mak~1:ascii:kok_inf#1 %
# %version: 1 %
# %derived_by: dzq92s %
# %date_modified: Tue Apr 14 12:13:00 2015 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================
include $(MAKE_INCL_DIR)/prolog.mak

#
# File extensions for IAR RH850 Tools.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .s
_bin_ext_ := 
 _lib_ext_ := .a
_lib_prefix_ :=
_obj_ext_ := .o

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name    (Description on following lines: SCR #, etc.)
# ----------- --------
# 13-Mar-2015 Emmanuel Yepez (zzxc1x) Rev. 1
# + Created initial file.
#
#===========================================================================
