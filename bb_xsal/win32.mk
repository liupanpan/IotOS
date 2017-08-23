#===========================================================================
# FILE: win32.mk
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:win32.mk~3:makefile:kok_aud#1 %
# %version:3 %
# %derived_by:dzq92s %
# %date_modified:Wed Mar 26 15:22:46 2008 %
#---------------------------------------------------------------------------
#===========================================================================

#
# The following settings customize the build for Win32
#
_src_dirs_ += $(_cur_module_path_)/bb_xsal_src/win32

#
# Files unique to Win32
#
$(_build_)_WIN32_XSAL_OBJ_FILES :=\
        $($(_build_)_OBJ_OUTDIR)/xsal_i_timer_win32$(_obj_ext_) \
        $($(_build_)_OBJ_OUTDIR)/xsal_rpc_win32$(_obj_ext_)

_lib_objs_ += $($(_build_)_WIN32_XSAL_OBJ_FILES)

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
