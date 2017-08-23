#==============================================================================
# FILE: Makefile_bb_dbg_trace_c89_imp.mk
#
#------------------------------------------------------------------------------
#
# Copyright 2009-2013 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#
#------------------------------------------------------------------------------
#
# %full_filespec:Makefile_bb_dbg_trace_c89_imp.mk~kok_inf#5:makefile:kok_aud#1 %
# %version:kok_inf#5 %
# %derived_by:dzq92s %
# %date_modified:Thu Oct  3 09:58:43 2013 %
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
#==============================================================================
_flavor_ := $($(_build_)_FLAVOR)

include $(MAKE_INCL_DIR)/$(_flavor_)_prolog.mak

#
# The dbg_trace_c89 library contains both the formatting functionsand the
# callout functions mapping the trace output to printf.
#
include $(_cur_module_path_)/dbg_trace_c89.mk

#
# The c89_trace_fmt library contains only the formatting functions; the callout
# functions mapping the trace output to printf are NOT included.
#
include $(_cur_module_path_)/c89_trace_fmt.mk


#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 15-aug-2008 kirk bailey
# + Created initial file.
#
# 17-sep-2008 kirk bailey
# + Removed override of compiler options.
#
# 02-dec-2008 kirk bailey
# + Added override of DBG_TRACE_PATH_SEPARATOR for uitron_arm_rvct flavor.
#
# 07-jul-2009 kirk bailey
# + Added separate lib c89_trace_fmt w/o callout functions.
#
# 03-Oct-2013 Kirk Bailey REV 5
# + Fixed order of revision history comments - no functional changes
#
#===========================================================================
