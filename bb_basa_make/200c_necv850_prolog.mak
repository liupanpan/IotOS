#===========================================================================
# FILE: win32_cygwin_prolog.mak
#===========================================================================
# Copyright 2006 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:200c_necv850_prolog.mak~4:makefile:kok_aud#2 %
# %version:4 %
# %derived_by:dzq92s %
# %date_modified:Wed Jun 10 07:27:55 2009 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================

include $(MAKE_INCL_DIR)/prolog.mak

#
# File extensions for gcc tools on CYGWIN.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .s
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
# 21-Jun-2006 kab-kok
# + Created initial file.
#
# 16-jan-2007 kab-kok
# + Added _bin_ext_, _lib_ext_, and _lib_prefix_.
#
#===========================================================================
