#===========================================================================
# FILE: Makefile_xsal.mk
#===========================================================================
# Copyright 2007-2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:libxsal.mk~kok_basa#20:makefile:kok_basa#1 %
# %version:kok_basa#20 %
# %derived_by:dzq92s %
# %date_modified:Fri Nov 20 10:01:14 2015 %
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
# XSAL can be built two ways: 1) XSAL Light, or 2) "Full" XSAL. Each
# build must define a variable (name XSAL_$(_build_)_TYPE) and set
# it to "XSAL_LIGHT", "XSAL", or "NONE" (no quotes) in order to specify
# which is appropriate.
#

ifneq ($(XSAL_$(_build_)_TYPE),NONE) # ignore rest of file, if "NONE"

_build_instructions_ :=

ifneq ($(XSAL_$(_build_)_TYPE),XSAL)
  ifneq ($(XSAL_$(_build_)_TYPE),XSAL_LIGHT)
    $(error XSAL_$(_build_)_TYPE must be defined to be NONE, XSAL, or XSAL_LIGHT)
  endif # ifneq ($(XSAL_$(_build_)_TYPE),XSAL_LIGHT)
endif # ifneq ($(XSAL_$(_build_)_TYPE),XSAL)

#
# In addition, XSAL has flavor-specific portions and these are handled
# by add-ons of additional makefiles.
#
ifneq ($(filter qnx_%,$(_flavor_)),)
  _build_instructions_ := $(_cur_module_path_)/qnx.mk
else ifneq ($(filter win32_%,$(_flavor_)),)
  _build_instructions_ := $(_cur_module_path_)/win32.mk
else ifneq ($(filter uitron_%,$(_flavor_)),)
  _build_instructions_ := $(_cur_module_path_)/uitron.mk
else ifneq ($(filter linux%,$(_flavor_)),)
  _build_instructions_ := $(_cur_module_path_)/linux.mk
else
  $(error No instructions for building XSAL for $(_flavor_))
endif # ifneq ($(filter qnx_%,$(_flavor_)),)

###############################################################################
# Definitions for compiling/assembling source code into object code:
####################################################################

#
# Define the directory (relative to project root directory) containing the
# source files for this module.
#
# EXAMPLE: _src_dirs_ := $(_cur_module_path_)/<implementation directory>/_src
#
_src_dirs_ := $(_cur_module_path_)/bb_xsal_src/common

ifeq ($(XSAL_$(_build_)_TYPE),XSAL)
  _src_dirs_ += $(_cur_module_path_)/bb_xsal_src/full
endif # ifeq ($(XSAL_$(_build_)_TYPE),XSAL)

ifeq ($(XSAL_$(_build_)_TYPE),XSAL_LIGHT)
  _src_dirs_ += $(_cur_module_path_)/bb_xsal_src/light
endif # ifeq ($(XSAL_$(_build_)_TYPE),XSAL_LIGHT)

###############################################################################
# Definitions for linking object code into a static library:
############################################################

#
# Define the name of the library; e.g., "foo" for libfoo.a.
#
# EXAMPLE: _lib_name_ := test_platform 
#         for $(_lib_prefix_)test_platform$(_lib_ext_)
#
ifeq ($(XSAL_$(_build_)_TYPE),XSAL)
  _lib_name_ := xsal
else
  _lib_name_ := xsal_light
endif # ifeq ($(XSAL_$(_build_)_TYPE),XSAL)

#
# Define the list of object files that go into the static library.
#
# EXAMPLE: _lib_objs_ := $($(_build_)_OBJ_OUTDIR)/filename$(_obj_ext_)
#

#
# Files unique to XSAL_LIGHT
#
$(_build_)_XSAL_LIGHT_OBJ_FILES :=\
        $($(_build_)_OBJ_OUTDIR)/xsal_i_run_light$(_obj_ext_)

#
# Files only in XSAL (not XSAL_LIGHT)
#
$(_build_)_FULL_XSAL_OBJ_FILES :=                                         \
        $($(_build_)_OBJ_OUTDIR)/xsal_init_rwlock_attr$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_rwlock$(_obj_ext_)           \
        $($(_build_)_OBJ_OUTDIR)/xsal_destroy_rwlock$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_rlock_rwlock$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_rlock_rwlock_timeout$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_try_rlock_rwlock$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_wlock_rwlock$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_wlock_rwlock_timeout$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_try_wlock_rwlock$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_unlock_rwlock$(_obj_ext_)           \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_shared_memory$(_obj_ext_)         \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_shared_memory$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_open_shared_memory$(_obj_ext_)      \
        $($(_build_)_OBJ_OUTDIR)/xsal_close_shared_memory$(_obj_ext_)     \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_ns_proxy$(_obj_ext_)                \
        $($(_build_)_OBJ_OUTDIR)/xsal_ns_thread$(_obj_ext_)               \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_destroyed$(_obj_ext_)          \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_connections$(_obj_ext_)           \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_queue_for_thread$(_obj_ext_) \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_run$(_obj_ext_)                   \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_dt$(_obj_ext_)                    \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_pend_message$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_receive_from$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_receive$(_obj_ext_)                 \
        $($(_build_)_OBJ_OUTDIR)/xsal_receive_from$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_receive_from_timeout$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_receive_only$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_try_receive_from$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_local_publish$(_obj_ext_)           \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_declare_urgent$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_undeclare_urgent$(_obj_ext_)        \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_first_message$(_obj_ext_)           \
        $($(_build_)_OBJ_OUTDIR)/xsal_last_message$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_queue_end$(_obj_ext_)               \
        $($(_build_)_OBJ_OUTDIR)/xsal_next_message$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_prev_message$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_message$(_obj_ext_)             \
        $($(_build_)_OBJ_OUTDIR)/xsal_remove_message$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_is_queue_begin$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_is_queue_end$(_obj_ext_)            \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_cond$(_obj_ext_)             \
        $($(_build_)_OBJ_OUTDIR)/xsal_destroy_cond$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_signal_cond$(_obj_ext_)             \
                                                                          \
        $($(_build_)_OBJ_OUTDIR)/xsal_select$(_obj_ext_)                  \
        $($(_build_)_OBJ_OUTDIR)/xsal_unselect$(_obj_ext_)                \


#
# Files common to XSAL and XSAL_LIGHT
#
$(_build_)_COMMON_XSAL_OBJ_FILES :=\
        $($(_build_)_OBJ_OUTDIR)/xsal_init_mutex_attr$(_obj_ext_)           \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_init_semaphore_attr$(_obj_ext_)       \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_semaphore$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_destroy_semaphore$(_obj_ext_)         \
        $($(_build_)_OBJ_OUTDIR)/xsal_signal_semaphore$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_semaphore$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_semaphore_timeout$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_try_wait_semaphore$(_obj_ext_)        \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_buffer_pools$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_buffer_pool$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_destroy_buffer_pool$(_obj_ext_)       \
        $($(_build_)_OBJ_OUTDIR)/xsal_alloc_buffer$(_obj_ext_)              \
        $($(_build_)_OBJ_OUTDIR)/xsal_free_buffer$(_obj_ext_)               \
        $($(_build_)_OBJ_OUTDIR)/xsal_alloc_free$(_obj_ext_)                \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_thread$(_obj_ext_)                  \
        $($(_build_)_OBJ_OUTDIR)/xsal_init_thread_attr$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_thread$(_obj_ext_)             \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_thread_id$(_obj_ext_)             \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_thread_name$(_obj_ext_)           \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_thread_name_self$(_obj_ext_)      \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_thread_priority$(_obj_ext_)       \
        $($(_build_)_OBJ_OUTDIR)/xsal_is_thread_alive$(_obj_ext_)           \
        $($(_build_)_OBJ_OUTDIR)/xsal_signal_ready$(_obj_ext_)              \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_ready$(_obj_ext_)                \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_ready_timeout$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_destroyed_timeout$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_set_thread_priority$(_obj_ext_)       \
        $($(_build_)_OBJ_OUTDIR)/xsal_delay$(_obj_ext_)                     \
        $($(_build_)_OBJ_OUTDIR)/xsal_sleep$(_obj_ext_)                     \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_message_queue$(_obj_ext_)           \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_queue$(_obj_ext_)              \
        $($(_build_)_OBJ_OUTDIR)/xsal_stat_queue$(_obj_ext_)                \
        $($(_build_)_OBJ_OUTDIR)/xsal_set_message_dropped_callback$(_obj_ext_) \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_event_property$(_obj_ext_)          \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_pend_message_timeout$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_receive_from_timeout$(_obj_ext_)    \
        $($(_build_)_OBJ_OUTDIR)/xsal_receive_timeout$(_obj_ext_)           \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_receive_only$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_receive_only_timeout$(_obj_ext_)      \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_try_pend_message$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_try_receive_from$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_try_receive$(_obj_ext_)               \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_select$(_obj_ext_)                  \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_send$(_obj_ext_)                      \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_publish$(_obj_ext_)                 \
        $($(_build_)_OBJ_OUTDIR)/xsal_publish$(_obj_ext_)                   \
        $($(_build_)_OBJ_OUTDIR)/xsal_subscribe$(_obj_ext_)                 \
        $($(_build_)_OBJ_OUTDIR)/xsal_unsubscribe$(_obj_ext_)               \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_engine$(_obj_ext_)                  \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_config$(_obj_ext_)                  \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_config$(_obj_ext_)                \
        $($(_build_)_OBJ_OUTDIR)/xsal_init$(_obj_ext_)                      \
        $($(_build_)_OBJ_OUTDIR)/xsal_run$(_obj_ext_)                       \
        $($(_build_)_OBJ_OUTDIR)/xsal_exit$(_obj_ext_)                      \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_app_id$(_obj_ext_)                \
        $($(_build_)_OBJ_OUTDIR)/xsal_get_max_number_of_threads$(_obj_ext_) \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_i_time$(_obj_ext_)                    \
        $($(_build_)_OBJ_OUTDIR)/xsal_clock$(_obj_ext_)		                \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_purge$(_obj_ext_)                     \
        $($(_build_)_OBJ_OUTDIR)/xsal_purge_only$(_obj_ext_)	            \


ifeq ($(filter linux%,$(_flavor_)),)
$(_build_)_COMMON_XSAL_OBJ_FILES +=\
        $($(_build_)_OBJ_OUTDIR)/xsal_i_timer$(_obj_ext_)                   \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_timer$(_obj_ext_)              \
        $($(_build_)_OBJ_OUTDIR)/xsal_destroy_timer$(_obj_ext_)             \
        $($(_build_)_OBJ_OUTDIR)/xsal_start_timer$(_obj_ext_)               \
        $($(_build_)_OBJ_OUTDIR)/xsal_start_timer_ex$(_obj_ext_)            \
        $($(_build_)_OBJ_OUTDIR)/xsal_stop_timer$(_obj_ext_)                \
                                                                            \
        $($(_build_)_OBJ_OUTDIR)/xsal_create_mutex$(_obj_ext_)              \
        $($(_build_)_OBJ_OUTDIR)/xsal_destroy_mutex$(_obj_ext_)             \
        $($(_build_)_OBJ_OUTDIR)/xsal_lock_mutex$(_obj_ext_)                \
        $($(_build_)_OBJ_OUTDIR)/xsal_unlock_mutex$(_obj_ext_)              \
        $($(_build_)_OBJ_OUTDIR)/xsal_lock_mutex_timeout$(_obj_ext_)        \
        $($(_build_)_OBJ_OUTDIR)/xsal_try_lock_mutex$(_obj_ext_)            \

$(_build_)_FULL_XSAL_OBJ_FILES +=\
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_cond$(_obj_ext_)                 \
        $($(_build_)_OBJ_OUTDIR)/xsal_wait_cond_timeout$(_obj_ext_)		    \

endif

ifeq ($(XSAL_$(_build_)_TYPE),XSAL)
  _lib_objs_ := $($(_build_)_COMMON_XSAL_OBJ_FILES) \
                $($(_build_)_FULL_XSAL_OBJ_FILES)

  CC_$(_build_)_MACRO += -DXSAL_LIB
else
  _lib_objs_ := $($(_build_)_COMMON_XSAL_OBJ_FILES) \
                $($(_build_)_XSAL_LIGHT_OBJ_FILES)

  CC_$(_build_)_MACRO += -DXSAL_LIGHT_LIB
endif # ifeq ($(XSAL_$(_build_)_TYPE),XSAL)

#
# Now include the build instructions that customize XSAL for the
# target OS.
#
include $(_build_instructions_)

#
# The following lines cause all calls used by libxsal.so to be resolved via
# static linking so that libxsal.so can be used and not depend on additional
# shared libraries.
#
_lib_libs_:=                                       \
  $($(_build_)_LIB_OUTDIR)/libdouble_linked_list.a \
  $($(_build_)_LIB_OUTDIR)/libstring_resource.a    \
  $($(_build_)_LIB_OUTDIR)/libdbg_trace_xsal.a     \
  
ifneq ($(filter qnx%,$(_flavor_)),)
  _lib_libs_ += $(SYS_LIB_$(_build_))
endif


include $(MAKE_INCL_DIR)/$(_flavor_)_epilog.mak

ifneq ($(filter win32_%,$(_flavor_)),)
  $(_c_objs_): CC_$(_build_)_WARN := -Wall
endif # ifneq ($(filter win32_%,$(_flavor_)),)


#####
endif # ifneq ($(XSAL_$(_build_)_TYPE),NONE)
#####

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 24-aug-2007 kirk bailey
# + Created initial file.
#
# 04-mar-2008 kirk bailey
# + Removed special build flags now that debugtrace.h is compatible with
# + -ansi flag.
#
# 26-mar-2008 kirk bailey
# + Renamed bb_xsal_bsd_src to bb_xsal_src.
#
# 02-jun-2008 kirk bailey
# + Added compiler options so that the type of XSAL library being built can
#   be confirmed to match the statically configured type.
# + Added SAL_Clock, SAL_Delta_Time, SAL_Alloc, SAL_Free to all XSAL impls.
#
# 20-Jul-2015 Kirk Bailey
# + Added SAL_Purge() calls.
#
# 20-Nov-2015 Kirk Bailey Rev 20
# + Added logic to resolve libxsal.so symbols via static links
#===========================================================================
