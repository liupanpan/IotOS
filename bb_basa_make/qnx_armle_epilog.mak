#===========================================================================
# FILE: qnx_shle_epilog.mak
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:qnx_armle_epilog.mak~2:ascii:kok_basa#1 %
# %version:2 %
# %derived_by:dzq92s %
# %date_modified:Thu Sep 20 09:31:34 2012 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "epilog" for defining a module for this flavor.
#
#===========================================================================

include $(MAKE_INCL_DIR)/epilog.mak

ifneq "$(_lib_objs_)" ""
ifeq  "$(_lib_ext_)" ".so"
#vvvvvvvvvvvvvvvvvvvvvvv#

#
# For QNX shared libraries (which end in a .so instead of a .a), the object
# files must be built with the "-shared" option. The following target-specific
# rules cause this to happen for the object files.
#
$(_lib_objs_): CC_$(_build_)_FLAGS  += -shared
$(_lib_objs_): CPP_$(_build_)_FLAGS += -shared

#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^#
endif # ifeq  "$(_lib_ext_)" ".so"
endif # ifneq "$(_lib_objs_)" ""

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 12-sep-2007 kirk bailey
# + Created initial file.
#===========================================================================
