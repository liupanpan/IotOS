#===========================================================================
# FILE: qnx.mk
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:qnx.mk~3:makefile:kok_aud#1 %
# %version:3 %
# %derived_by:dzq92s %
# %date_modified:Wed Mar 26 15:22:45 2008 %
#---------------------------------------------------------------------------
#===========================================================================

#
# The following settings customize the build for QNX
#
_src_dirs_ += $(_cur_module_path_)/bb_xsal_src/qnx

#
# Files unique to QNX
#
$(_build_)_QNX_XSAL_OBJ_FILES :=\
        $($(_build_)_OBJ_OUTDIR)/xsal_i_timer_qnx$(_obj_ext_) \
        $($(_build_)_OBJ_OUTDIR)/xsal_rpc_qnx$(_obj_ext_)

_lib_objs_ += $($(_build_)_QNX_XSAL_OBJ_FILES)

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 26-mar-2008 kirk bailey
# + Renamed bb_xsal_bsd_src to bb_xsal_src.
#
# 24-aug-2007 kirk bailey
# + Created initial file.
#
#===========================================================================
