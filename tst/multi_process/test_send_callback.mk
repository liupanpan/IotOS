#===========================================================================
# FILE: test_send_callback.mk
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:test_send_callback.mk~kok_basa#9:makefile:kok_aud#1 %
# %version:kok_basa#9 %
# %derived_by:jz4fpf %
# %date_modified:Tue Apr 14 15:18:37 2015 %
#---------------------------------------------------------------------------
#
#===========================================================================
_flavor_ := $($(_build_)_FLAVOR)

include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

_bin_name_ := test_send_callback$(_bin_ext_)
_bin_objs_ := $($(_build_)_OBJ_OUTDIR)/test_send_callback$(_obj_ext_)

ifeq ($(XSAL_$(_build_)_TYPE),XSAL)
_bin_libs_ := $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)xsal$(_lib_ext_)
else
_bin_libs_ := $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)xsal_light$(_lib_ext_)
endif

ifneq ($(filter linux%,$(_flavor_)),)
  _bin_libs_ += pthread.so rt.so
  ifeq ($(XSAL_$(_build_)_TYPE),XSAL_LIGHT)
    _bin_libs_ += stdc++
    _bin_objs_ += $($(_build_)_OBJ_OUTDIR)/procman_callouts$(_obj_ext_)
  endif
endif

ifneq ($(filter win32_%,$(_flavor_)),)
_bin_libs_ += WS2_32
endif

_bin_libs_ +=\
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)dbg_trace_xsal$(_lib_ext_)    \
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)double_linked_list$(_lib_ext_)\
  $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)string_resource$(_lib_ext_)   \


_bin_lib_dirs_ := 
_bin_lnk_cmd_ :=

include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 19-Jul-2013 Larry Piekarski Rev 8
# + Task 161388: Tip off to new version bbc_util.
#
# 21-Mar-2013 Kirk Bailey
# + Added link against libdbg_trace_xsal.a now that libdbg_trace has been
# + de-coupled from XSAL.
#
# 24-jun-2010 kirk bailey
# + Changed Linux build to specify shared versions of pthread and rt libs.
#
# 07-nov-2007 kirk bailey
# + Created initial file.
#
#===========================================================================
