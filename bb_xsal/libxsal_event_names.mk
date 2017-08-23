#===========================================================================
# FILE: libxsal_event_names.mk
#===========================================================================
# Copyright 2012 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: libxsal_event_names.mk~2:makefile:kok_basa#1 %
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

###_src_dirs_ :=

_lib_name_ := xsal_event_names
_lib_objs_ := $($(_build_)_OBJ_OUTDIR)/xsal_event_names$(_obj_ext_)

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 07-May-2012 kirk bailey
# + Added libxsal_event_names.
#===========================================================================
