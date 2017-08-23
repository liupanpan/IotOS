#===========================================================================
# FILE: header_vars.mak
#===========================================================================
# Copyright 2011-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: header_vars.mak~kok_inf#13:makefile:kok_aud#5 %
# %version: kok_inf#13 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 08:11:14 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file dynamically creates rules for copying all of the "public" and
# "friend" header files to fixed locations within the project that can then
# be used as the include paths for compilation. "Public" header files are
# any files with one of the API_EXTENSIONS file extensions found anywhere
# within the project in a directory whose name ends in "_api". The same
# applies to "friend" header files, with the exception that these are found
# in directories whose names end with "_inc".
#
# In general, only one file with a name <basename>.<ext> is allowed to
# exist within the "public"/"friend" collection. An exception is made for
# "flavor-specific" header files; these are found in directories whose names
# end in "_<flavor>_api" or "_<flavor>_inc", where <flavor> corresponds to one
# of the entries in POTENTIAL_FLAVORS. There are unique directories for
# each flavor's header files and so a different <basename>.<ext> is allowed
# for each flavor. Other that for this exception, duplicate header files are
# considered and unacceptable ambiguity and cause the build to terminate.
#
# This file creates rules for copying the above-described header files to
# known locations within the project. These locations are
#
#     $(PROJECT_OUTPUT_ROOT)/include/common
#     $(PROJECT_OUTPUT_ROOT)/include/<flavor> (for each <flavor>)
#
# The non-flavor-specific header files are placed into the common area.
#
# The copy rules ensure that the original file is a prerequisite of
# the copied file. This means that the copied file will be updated whenever
# the original is altered (becomes newer).
#
#===========================================================================

###############################################################################
# Definition of Variables Containing Include Directory Paths #
##############################################################
#
# NOTE: The "find" operation can take much longer on a networked drive than
#       on a local drive (40x longer has been observed regularly).
#
ifndef API_EXTENSIONS
  API_EXTENSIONS := def h hpp inc
endif

###$(info API_EXTENSIONS = $(API_EXTENSIONS))

#
# Get relative paths to all public API files in the project:
#
$(foreach ext,$(API_EXTENSIONS),\
  $(eval ALL_PUBLIC_$(ext)_FILES :=  $(shell $(FIND) . -name "*.$(ext)" |\
      $(GREP) -v "$(PROJECT_OUTPUT_ROOT)/sdk-"| $(GREP) "_api/\|_inc/"| $(SED) -e 's,^\./,,g')))

###$(foreach ext,$(API_EXTENSIONS),\
###  $(info  ALL_PUBLIC_$(ext)_FILES := $(ALL_PUBLIC_$(ext)_FILES)))

#
# For each flavor, create a variable (named <flavor>_PUBLIC_$(ext)_FILES) that
# lists all of the API files for that flavor. A given API file can only exist
# multiple times within the project if it is distinguished by flavor.
#
$(foreach ext,$(API_EXTENSIONS),\
  $(foreach _flavor_,$(POTENTIAL_FLAVORS),\
    $(eval $(_flavor_)_PUBLIC_$(ext)_FILES:=)))

$(foreach ext,$(API_EXTENSIONS),\
  $(foreach _flavor_,$(POTENTIAL_FLAVORS),\
    $(foreach file,$(ALL_PUBLIC_$(ext)_FILES),\
      $(if $(or $(findstring $(_flavor_)_api/,$(file)),\
           $(findstring $(_flavor_)_inc/,$(file))),\
           $(eval $(call ADD_API_FILE,$(_flavor_)_PUBLIC_$(ext)_FILES,\
                                      $(file),$(_flavor_)))))))
#
# Set ALL_FLAVOR_PUBLIC_$(ext)_FILES to the union of all of the flavors' APIs.
#
$(foreach ext,$(API_EXTENSIONS),\
  $(foreach _flavor_,$(POTENTIAL_FLAVORS),\
  $(eval ALL_FLAVOR_PUBLIC_$(ext)_FILES += $($(_flavor_)_PUBLIC_$(ext)_FILES))))

#
# Any remaining APIs are non-flavor specific API files.
#
$(foreach ext,$(API_EXTENSIONS),\
  $(eval TEMP_NON_FLAVOR_PUBLIC_$(ext)_FILES :=\
    $(filter-out $(ALL_FLAVOR_PUBLIC_$(ext)_FILES),$(ALL_PUBLIC_$(ext)_FILES))))
#
# Form NON_FLAVOR_PUBLIC_$(ext)_FILES while checking for duplicates
#
$(foreach ext,$(API_EXTENSIONS),$(eval NON_FLAVOR_PUBLIC_$(ext)_FILES:=))

$(foreach ext,$(API_EXTENSIONS),\
  $(foreach file,$(TEMP_NON_FLAVOR_PUBLIC_$(ext)_FILES),\
    $(eval $(call ADD_API_FILE,NON_FLAVOR_PUBLIC_$(ext)_FILES,$(file)))))
#
# For each of the non-flavor-specific header files, create a rule to copy
# it to the COMMON_INC_PATH directory and make this rule a prerequisite
# of the "includes" target.
#
$(foreach ext,$(API_EXTENSIONS),\
  $(eval $(call Create_Cp_Ln_Rules,$(NON_FLAVOR_PUBLIC_$(ext)_FILES),\
                                  $(COMMON_INC_PATH),includes)))

#
# For each of the flavor-specific header files, create a rule to copy it
# to the flavor-specific include directory. Note that FLAVOR_LIST is used
# instead of POTENTIAL_FLAVORS; this is done so that copy rules are only
# created for flavors that are used.
#
$(foreach flavor, $(FLAVOR_LIST),\
  $(foreach ext,$(API_EXTENSIONS),\
    $(eval $(call Create_Cp_Ln_Rules,$($(flavor)_PUBLIC_$(ext)_FILES),\
                                    $($(flavor)_INC_PATH),includes))))

#===========================================================================
# File Revision History (top to bottom: last revision to first revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 29-Oct-2014 Kirk Bailey Rev 12
# + Deleted revision history prior to 2012.
# + Added ENABLE_NEW_API_LOGIC option for public header files.
#
#===========================================================================
