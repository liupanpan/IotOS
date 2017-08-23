#===========================================================================
# FILE: linux.mk
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:linux.mk~6:makefile:kok_basa#1 %
# %version:6 %
# %derived_by:jz4fpf %
# %date_modified:Tue Aug 28 09:10:45 2012 %
#---------------------------------------------------------------------------
#===========================================================================

#
# The following settings customize the build for linux
#
_src_dirs_ += $(_cur_module_path_)/bb_xsal_src/linux

#
# Files unique to linux
#
$(_build_)_linux_XSAL_OBJ_FILES :=\
        $($(_build_)_OBJ_OUTDIR)/xsal_timer_linux$(_obj_ext_) \
        $($(_build_)_OBJ_OUTDIR)/xsal_rpc_linux$(_obj_ext_) \
        $($(_build_)_OBJ_OUTDIR)/xsal_mutex_linux$(_obj_ext_) \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_cond_linux$(_obj_ext_) \

_lib_objs_ += $($(_build_)_linux_XSAL_OBJ_FILES)

ifeq ($(XSAL_$(_build_)_TYPE),XSAL_LIGHT)
  _lib_objs_ +=\
    $($(_build_)_OBJ_OUTDIR)/xsal_light_linux$(_obj_ext_)
endif


#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 30-may-2010 kirk bailey
# + Created initial file.
#
#===========================================================================
