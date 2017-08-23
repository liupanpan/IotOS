#===========================================================================
# FILE: cfg_vars.mak
#===========================================================================
# Copyright 2007 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec:cfg_vars.mak~kok_inf#8:makefile:kok_aud#5 %
# %version:kok_inf#8 %
# %derived_by:czhswm %
# %date_modified:Tue Sep  3 13:01:24 2013 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file dynamically defines make variables describing the list of
# configuration header files for each build. Configuration header files
# are slightly different than other public header files in the following
# sense:
#
# 1) They are optional. If the exist, they exist starting at the file paths
#    defined by the PROJECT_CFG_ROOT variable.
#
# 2) Configuration header files can have "common" and "build-specific"
#    versions simultaneously, with the latter taking precedence. Consider
#    the following example:
#
#       > The value of PROJECT_CFG_ROOT is "cfg"
#       > The only contents of cfg are i) foo.h and ii) subdirectory "pc_test".
#       > The "pc_test" subdirectory also contains a foo.h.
#       > "pc_test" is one of the builds in BUILD_LIST.
#
#    For this example, the configuration file used for the build "pc_test"
#    is the one in cfg/pc_test/foo.h and the configuration file for all other
#    builds is the one in cfg/foo.h.
#
# For each build, a variable named <build>_CFG_DIRS is created that has
# the list of potential configuration directories for the build. These
# values are appropriate for use in generating include (-I) options to
# compilers because they enforce the order of precedence described above.
#
#===========================================================================

ifneq "$(PROJECT_CFG_ROOT)" ""
#vvvvvvvvvvvvvvvvvvvvvvvvvvvvv

#
# Get relative paths to all configuration header files in the project:
#
ALL_CFG_FILES :=\
$(foreach ext,$(API_EXTENSIONS),\
  $(foreach _cfg_dir_,$(PROJECT_CFG_ROOT),\
    $(shell $(FIND) $(_cfg_dir_) -name *.$(ext) | $(SED) -e 's,^\./,,g')))
#
# Sort the list of ALL_CFG_FILES. This has the desired side effect of 
# removing duplicated files, included in the previous step.
#
ALL_CFG_FILES := $(sort $(ALL_CFG_FILES))
#
# For each build, create a variable (named <build>_SPECIFIC_CFG_FILES) that
# lists all of the configuration header files specifically for that build.
#
$(foreach _build_,$(BUILD_LIST),\
          $(eval $(_build_)_SPECIFIC_CFG_FILES += \
                 $(foreach file,$(ALL_CFG_FILES),\
                       $(if $(findstring /$(_build_)/,$(file)),$(file)))))
#
# For each build, strip off the paths to get just the filenames for
# the build-specific configuration header files.
#
$(foreach _build_,$(BUILD_LIST),\
  $(eval $(_build_)_SPECIFIC_FILENAMES :=\
         $(notdir $($(_build_)_SPECIFIC_CFG_FILES))))
#
# For each flavor, create a variable (named <flavor>_SPECIFIC_CFG_FILES) that
# lists all of the configuration header files specifically for that flavor.
#
$(foreach _flavor_,$(POTENTIAL_FLAVORS),\
          $(eval $(_flavor_)_SPECIFIC_CFG_FILES += \
                 $(foreach file,$(ALL_CFG_FILES),\
                       $(if $(findstring /$(_flavor_)/,$(file)),$(file)))))
#
# For each flavor, strip off the paths to get just the filenames for
# the flavor-specific configuration header files.
#
$(foreach _flavor_,$(POTENTIAL_FLAVORS),\
  $(eval $(_flavor_)_SPECIFIC_FILENAMES :=\
         $(notdir $($(_flavor_)_SPECIFIC_CFG_FILES))))
#
# Set ALL_BUILD_CFG_FILES to the union of all of the build-specific
# configuration header files.
#
$(foreach _build_,$(BUILD_LIST),\
          $(eval ALL_BUILD_CFG_FILES += $($(_build_)_SPECIFIC_CFG_FILES)))
#
# Set ALL_FLAVOR_CFG_FILES to the union of all of the flavor-specific
# configuration header files.
#
$(foreach _flavor_,$(POTENTIAL_FLAVORS),\
          $(eval ALL_FLAVOR_CFG_FILES += $($(_flavor_)_SPECIFIC_CFG_FILES)))
#
# Any non-build-specific and non-flavor-specific configuration headers are in
# the "common" area
#
COMMON_CFG_FILES :=\
  $(filter-out $(ALL_BUILD_CFG_FILES) $(ALL_FLAVOR_CFG_FILES),$(ALL_CFG_FILES))
#
# The list of configuration files for a given build is the combination
# of: 1) All the configuration files specific to that build, and 2) Any
# configuration files in the build's flavor area THAT DO NOT HAVE A FILE
# OF THE SAME NAME IN THE BUILD-SPECIFIC LIST, and 3) Any configuration files
# in the common area THAT DO NOT HAVE A FILE OF THE SAME NAME AS ANY FILE
# IN EITHER THE BUILD-SPECIFIC OR BUILD'S FLAVOR-SPECIFIC LIST. The resulting
# list is saved in $(_build_)_CFG_FILES.
#
$(foreach _build_,$(BUILD_LIST),\
  $(eval $(_build_)_CFG_FILES := $($(_build_)_SPECIFIC_CFG_FILES)))

$(foreach _build_,$(BUILD_LIST),\
  $(eval $(_build_)_CFG_FILES +=\
         $(foreach file,$($($(_build_)_FLAVOR)_SPECIFIC_CFG_FILES),\
                   $(if $(findstring $(notdir $(file)),\
                                 $($(_build_)_SPECIFIC_FILENAMES)),,$(file)))))

$(foreach _build_,$(BUILD_LIST),\
  $(eval $(_build_)_CFG_FILES +=\
         $(foreach file,$(COMMON_CFG_FILES),\
                   $(if $(findstring $(notdir $(file)),\
                          $($(_build_)_SPECIFIC_FILENAMES)\
                       $($($(_build_)_FLAVOR)_SPECIFIC_FILENAMES)),,$(file)))))

#
# Define the configuration directories (<build>_CFG_DIRS) for each build.
# Note that the order is significant in that the build-specific directory
# takes precedence over (precedes) the "common" root directory. All
# build-specific directories come first, followed by the any of the
# build flavor's flavor-specific directories, followed by the "common"
# directories.
#
$(foreach _build_,$(BUILD_LIST),\
  $(eval $(_build_)_CFG_DIRS :=\
         $(foreach dir,$(PROJECT_CFG_ROOT), $(wildcard $(dir)/$(_build_)))))
 
$(foreach _build_,$(BUILD_LIST),\
  $(eval $(_build_)_CFG_DIRS +=\
         $(foreach dir,$(PROJECT_CFG_ROOT), $(wildcard $(dir)/$($(_build_)_FLAVOR)))))
 
$(foreach _build_,$(BUILD_LIST),\
  $(eval $(_build_)_CFG_DIRS +=\
         $(foreach dir,$(PROJECT_CFG_ROOT), $(wildcard $(dir)))))

#^^^^^^^^^^^^^^^^^^^^^^^^^^^^
endif

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 03-sep-2013 larry piekarski
# + Added a $(sort ) of ALL_CFG_FILES to remove duplicate files in certain
#   directory structures.
#
# 22-jul-2010 kirk bailey
# + Use API_EXTENSIONS instead of assuming .h for cfg files.
#
# 26-aug-2009 kirk bailey
# + Only add cfg directories that actually exist to $(_build_)_CFG_DIRS.
#
# 06-dec-2007 kirk bailey
# + Changed "userid" to "Name" in revision history.
#
# 01-aug-2006 kab-kok
# + Created initial file.
#
# 25-aug-2006 kab-kok
# + Corrected full_filespec, version, author, date fields.
#
# 03-sep-2006 kab-kok
# + Switched from "flavors" to "builds".
#
# 06-sep-2006 kab-kok
# + Added flavor-specific builds. Priority is build-specific, flavor-specific,
#   and finally "common".
#
# 19-jan-2007 kab-kok
# + Switched from using FLAVOR_LIST to POTENTIAL_FLAVORS to support the case
# + where a project has flavor-specific configuration files for a flavor not
# + currently associated with one of its builds. Also fixed the logic looking
# + for build-specific and flavor-specific configuration files to not list
# + files in both if the build name matched the end of the flavor name.
#
#===========================================================================
