#===========================================================================
# FILE: Makefile_xsal.mk
#===========================================================================
# Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: Makefile_xsal.mk~2:makefile:kok_basa#20 %
# %version: 2 %
# %derived_by: dzq92s %
# %date_modified: Mon May  7 12:57:43 2012 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file provides the project-specific information required for a build
# of this module. On entry, the variable "_build_" identifies which build is
# being performed, and the variable $(_build_)_FLAVOR defines the flavor for
# this build. This file includes other files (prolog.mak and epilog.mak)
# that are unique for the flavor from the $(MAKE_INCL_DIR) directory.
#
#===========================================================================
_flavor_ := $($(_build_)_FLAVOR)

include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

include $(_cur_module_path_)/libxsal.mk
include $(_cur_module_path_)/libxsal_event_names.mk

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 07-May-2012 kirk bailey
# + Moved libxsal logic to libxal.mk and added libxsal_event_names.
#===========================================================================
