#===========================================================================
# FILE: uitron_sh2a_prolog.mak
#===========================================================================
# Copyright 2006 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: uitron_sh2a_prolog.mak~4:makefile:kok_aud#3 %
# %version: 4 %
# %derived_by: dzq92s %
# %date_modified: Wed Jun 10 07:28:22 2009 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================

include $(MAKE_INCL_DIR)/prolog.mak

#
# File extensions for IAR tools for the V850.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .src
_bin_ext_ := .abs
_lib_ext_ := .lib
_lib_prefix_ :=
_obj_ext_ := .o

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 06-dec-2007 kirk bailey
# + Changed "userid" to "Name" in revision history.
#
# 19-nov-2006 kab-kok
# + Created initial file.
#
#===========================================================================
