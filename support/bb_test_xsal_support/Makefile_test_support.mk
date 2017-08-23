#===========================================================================
# FILE: Makefile_test_support.mk
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:Makefile_test_support.mk~3:makefile:kok_aud#1 %
# %version:3 %
# %derived_by:dzq92s %
# %date_modified:Tue Aug 19 15:23:43 2008 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file provides the project-specific information required for a build
# of this module. On entry, the variable "_build_" identifies which build is
# being performed, and the variable $(_build_)_FLAVOR defines the flavor for
# this build. This file includes template Makefiles for the flavor
# from the $(MAKE_INCL_DIR) directory, which must be defined prior to
# including this file.
#
#===========================================================================
_flavor_ := $($(_build_)_FLAVOR)

include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

#
# Include flavor specific make file 
#
ifneq ($(wildcard $(_cur_module_path_)/test_$(_flavor_).mk),)
   include $(_cur_module_path_)/test_$(_flavor_).mk
else ifneq ($(filter qnx_%,$(_flavor_)),)
   include $(_cur_module_path_)/test_qnx.mk
endif

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 18-aug-2008 kirk bailey
# + Added support for QNX.
#
# 05-aug-2008 kirk bailey
# + Added conditional include for flavor-specific file.
#
# 20-feb-2007 kab-kok
# + Created initial file.
#
#===========================================================================
