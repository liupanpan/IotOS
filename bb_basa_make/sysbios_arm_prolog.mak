#===========================================================================
# FILE: sysbios_arm_prolog.mak
#===========================================================================
# Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: sysbios_arm_prolog.mak~1:makefile:kok_basa#1 %
# %version: 1 %
# %derived_by: dzq92s %
# %date_modified: Tue Sep 11 11:03:39 2012 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "prolog" for defining a module for this flavor.
#
#===========================================================================

include $(MAKE_INCL_DIR)/prolog.mak

#
# File extensions for Code Composer tools.
#
# NOTE: These should only be used to define make targets and prerequisites;
#       they should not be used in the commands of shell commands since they
#       take on multiple values (one per tool chain).
#
_asm_ext_ := .asm
_bin_ext_ := .out
_lib_ext_ := .a
_lib_prefix_ := lib
_obj_ext_ := .o

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 16-may-2012 heiko pohle
# + file name change: sysbios_m3_** to sysbios_arm_**
#
# 07-dec-2011 heiko pohle Mesut Sözüçok
# + obj extention changed to *.o 
#
# 01-dec-2011 heiko pohle
# + initial version for VAG MIB Standard 2.0
#
# 26-Oct-2011 kirk bailey
# + Created initial file.
#
#===========================================================================
