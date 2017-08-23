#===========================================================================
# FILE: project.mak
#===========================================================================
# Copyright 2011-2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: project.mak~kok_inf#48:makefile:kok_aud#14 %
# %version: kok_inf#48 %
# %derived_by: dzq92s %
# %date_modified: Fri Jan  9 07:58:41 2015 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file provides the reusable definitions for the main project's makefile.
# By including this file, the main Makefile automatically inherits the
# capability of dynamically discovering and building rules to make all of
# the modules within the project that follow the established conventions
# (which they will do if their Makefile.X files are derived from
# Makefile_template).
#
# Prior to including this file, the main Makefile MUST define the variable
# BUILD_LIST to contain the list of "builds" for the project. Each build
# represents a separate set of generated files with its own unique set of
# properties. For each build in this list, a variable <build>_FLAVOR must
# also be defined specifying the "flavor" (tool set) associated with the
# build.
#
# In addition, the main Makefile can optionally define PROJECT_CFG_ROOT. It
# can be the relative path to the directory used to hold project and build
# specific configuration header files. If it is defined, then the variables
# <build>_CFG_DIRS will be created for use in specifying the paths to
# these header files.
#
#===========================================================================

# Save the time that the processing of this file started
MAKE_START := $(shell date +%s.%N)

#
# Delete all default suffixes
#
.SUFFIX:

#
# Commands used
#
BC      := bc
CAT     := cat
CHMOD   := chmod
CP      := cp
CYGPATH := cygpath
DATE    := date
ECHO    := echo
FIND    := find
GREP    := grep
LN      := ln
LS      := ls
MKDIR   := mkdir
MV      := mv
RM      := rm
SED     := sed
TOUCH   := touch
TRUE    := true
#
# Other executables (these must be in the PATH)
#
DOXYGEN := doxygen
HHC := hhc # Microsoft's HTML Help Compiler

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)

#
# Command timing (via ENABLE_CMD_TIMING) is only available if the host has $(BC)
#
WHICH_BC := $(shell which $(BC) 2>/dev/null)
ifeq ($(WHICH_BC),)
  ifeq ($(ENABLE_CMD_TIMING),1)
    $(warning Overriding ENABLE_CMD_TIMING (disabling) because $(BC) utility not found)
    override ENABLE_CMD_TIMING := 0
  endif
endif
# Clear out ENABLE_CMD_TIMING if not "1" so $(if ) considers it "FALSE"
ifneq ($(ENABLE_CMD_TIMING),1)
  override ENABLE_CMD_TIMING :=
endif
#
# If this is a Linux host doing the build, default to using links instead of copies
#
WHICH_LSB := $(shell which lsb_release 2>/dev/null)
ifneq ($(WHICH_LSB),)
  ifndef USE_LINKS
    USE_LINKS := 1
  endif
endif

#
# Make sure the project defined the list of builds
#
ifeq ($(BUILD_LIST),)
   $(error BUILD_LIST must specify the list of builds for the project)
endif
#
# If the project didn't specify a root directory for the generated files,
# default to "out" in the project directory. 
#
ifeq ($(PROJECT_OUTPUT_ROOT),)
  PROJECT_OUTPUT_ROOT := out
endif
PROJECT_DIR := $(shell pwd)
#
# Define a separate root directory for each build's generated files.
#
$(foreach _build_,$(BUILD_LIST),\
         $(eval $(_build_)_OUTPUT_ROOT := $(PROJECT_OUTPUT_ROOT)/$(_build_)))

#
# Define default list of include file extensions if not overridden
#
ifndef API_EXTENSIONS
  API_EXTENSIONS := def h hpp inc
endif

#
# This function makes sure that the specified header file ($2) is not already
# in the list ($1) and adds it to the list.
#
# ARGUMENTS: $1 - name of variable containing list of files
#            $2 - name of file to be added to list
#            $3 - (optional) name of flavor, if this is a flavor-specific
#                   list of files
#
define ADD_API_FILE
  $(if $(findstring /$(notdir $2),$($1)),\
       $(warning Illegal multiple occurrences of public file '$(notdir $2)':\
         '$2', '$(filter %/$(notdir $2),$($1))' $(if $3,for flavor '$3',))\
         $(eval AMBIGUOUS_PUBLIC_APIS := y),\
       $1+=$2\
    )
endef

#
# This function adds list of paths to API_DIRS.
# 
# ARGUMENTS: $1 paths to add to API_DIRS.
#
# Example usage: $(eval $(call Get_API_HDRS,$(_cur_module_api_dirs_)))
#
define Get_API_HDRS
  API_DIRS+=$1
  $(eval $(_cur_module_name_)_api_hdrs:=)
  $(foreach ext,$(API_EXTENSIONS),\
      $(foreach dir,$1,\
          $(eval  $(_cur_module_name_)_api_hdrs+=$(wildcard $(dir)/*.$(ext))) ) )
  ALL_API_HDRS+=$($(_cur_module_name_)_api_hdrs)
endef

#
# This function adds list of paths to API_DIRS and the files they contain
# to ALL_API_HDRS.
# 
# ARGUMENTS: $1 paths to add to API_DIRS.
#
# Example usage: $(eval $(call Add_API_Paths,$(_cur_module_api_dirs_)))
#
define Add_API_Paths
  $(if $1,$(eval $(call Get_API_HDRS,$1)),)
endef

###############################################################################
#
# NOTE:
# Conditional logic is used to isolate the following cases
#
# 1) No targets are specified; e.g., "make"
# 2) "help" is one of the targets; e.g., "make help"
# 3) "clean" or "clean-<something>" is one of the targets; e.g., "make clean"
#
# For these cases, only the targets necessary are defined. Specifically,
# none of the logic that recursively examines the contents of the projects
# to dynamically build rules is run. This analysis can take many seconds on
# a project containing many modules and is skipped for these common cases
# because it is unnecessary and will speed them up.
#
###############################################################################

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ifeq ($(strip $(MAKECMDGOALS)),)
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@
#@ No target was specified. Default to the "help" target.
#@

.PHONY: help
help:
	@for l in $(HELP); do echo "$$l"; done;

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
else ifeq ($(MAKECMDGOALS),help)
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@
#@ If "help" is one of the targets, ignore all others and define/execute it.
#@

.PHONY: help
help:
	@for l in $(HELP); do echo "$$l"; done;

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
else
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

###############
# Rule: clean #
###############
#
# Set up a rule to delete all generated files by creating a dependency on each
# individual build's clean rule. These individual rules are constructed by
# prolog.mak for each build.
#
.PHONY: clean
clean:  $(PRE_CLEAN) $(foreach _build_,$(BUILD_LIST),clean-$(_build_))
####################
# Rule: clean-all  #
####################
#
# Set up a rule to augment a "clean" by also doing a "clean-docs", and
# "clean-includes".
#
.PHONY: clean-all
clean-all: $(PRE_CLEAN_ALL) clean clean-docs clean-includes
####################
# Rule: clean-docs #
####################
#
# Set up a rule to clean out the automatically generated documentation.
#
.PHONY: clean-docs
clean-docs:
	-$(RM) -rf doc/*.chm doc/*_html $(PROJECT_OUTPUT_ROOT)/*.log
########################
# Rule: clean-includes #
########################
#
# Set up a rule to clean out the copied public header files.
#
.PHONY: clean-includes
clean-includes:
	-$(CHMOD) -f -R +w $(PROJECT_OUTPUT_ROOT)/include 2>/dev/null;$(RM) -rf $(PROJECT_OUTPUT_ROOT)/include
########################
# Rule: clean-$(build) #
########################
#
# Set up a rule to delete all generated files by creating a dependency on each
# individual build's clean rule. These individual rules are constructed by
# prolog.mak for each build.
#
.PHONY: clean
$(foreach _build_,$(BUILD_LIST),clean-$(_build_)):
	$(RM) -rf $(PROJECT_OUTPUT_ROOT)/$(patsubst clean-%,%,$@)

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
ifeq ($(filter clean clean-%,$(MAKECMDGOALS)),)
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#@
#@ If a "clean" rule is one of the targets, ignore all others. This is done
#@ to speed up processing of the "clean" rules by not loading in all of the
#@ modules' makefiles and generating their rules.
#@
#@ For all non-clean targets, the project contents are dynamically analyzed
#@ and dynamic rules are generated.
#@

#
# MAKE_INCL_DIR must specify exactly one directory.
#
ifneq ($(words $(MAKE_INCL_DIR)),1)
   $(error MAKE_INCL_DIR must specify exactly one directory)
endif
ifeq ($(wildcard $(MAKE_INCL_DIR)),)
   $(error MAKE_INCL_DIR must specify a directory with reusable make rules)
endif
# ----------------------------------------------------------------------------
# CREATE_COPY_RULES
#
# This function creates rules to copy a list of files ($(1)) to a specified
# directory ($(2)) and also creates a rule that makes the resultant files
# prerequisites of another specified target ($(3)).
#
# $(1) - this list of files
# $(2) - the directory into which the files are to be copied
# $(3) - the target that will have, as prerequisites, the copied files.
#
define CREATE_COPY_RULES
  $(eval $(3):$(addprefix $(2)/,$(notdir $(1))))
  $(foreach file,$(1),\
    $(eval $(2)/$(notdir $(file)):$(file) ; \
           $(if $(VERBOSE_MAKE),$(CP),@$(CP)) -fp $$< $$@))
endef
# ----------------------------------------------------------------------------
# Create_Cp_Ln_Rules
#
# This function creates rules to copy a list of files ($(1)) to a specified
# directory ($(2)) and also creates a rule that makes the resultant files
# prerequisites of another specified target ($(3)).
#
# $(1) - this list of files
# $(2) - the directory into which the files are to be copied
# $(3) - the target that will have, as prerequisites, the copied files.
#
# NOTE: If USE_LINKS is defined, then soft links are created instead of
#       copies. This only works if the underlying filesystem supports
#       links.
#
DOLLAR:=$
CP_LN_SED_CMD:=sed -r -e 's:/$$(DOLLAR)::' -e 's:(.*):\1/:' -e 's:[^/]*/:../:g'
ifneq ($(USE_LINKS),1)
  define Create_Cp_Ln_Rules
    $(eval $(3):$(addprefix $(2)/,$(notdir $(1))))
    $(foreach file,$(1),\
      $(eval $(2)/$(notdir $(file)):$(file) ; \
             $(if $(VERBOSE_MAKE),$(CP),@$(CP)) -rfp $$< $$@))
  endef
else
  define Create_Cp_Ln_Rules
    $(eval rel_path:=$(shell echo $(2) | $(CP_LN_SED_CMD)))
    $(eval $(3):$(addprefix $(2)/,$(notdir $(1))))
    $(foreach file,$(1),\
      $(eval $(2)/$(notdir $(file)):$(file) ; \
             $(if $(VERBOSE_MAKE),$(LN),@$(LN)) -fs $(rel_path)$$< $$@))
  endef
endif
#
# Define the list of potential flavors (those supported by the make rules).
# Note that this list can be "pre-loaded" by the project Makefile to
# specify flavors not handled by the make rules. Such pre-loaded flavors
# are not candidates for builds, but allow projects to have flavor-specific
# header files that will be correctly recognized as such. sort will eliminate
# any duplicates.
#
ifndef POTENTIAL_FLAVORS
  POTENTIAL_FLAVORS :=
endif
POTENTIAL_FLAVORS += $(patsubst %_def.mak,%,\
                       $(notdir $(wildcard $(MAKE_INCL_DIR)/*_def.mak)))
POTENTIAL_FLAVORS := $(sort $(POTENTIAL_FLAVORS))

#
# This function adds the specified path to the variable GLOBAL_$(_build_)_INC_PATH,
# making sure that the path only appears once. This variable is a list of
# paths searched for header files for the build identified by $(_build_).
# 
# ARGUMENTS: $(1) path to add to GLOBAL_$(_build_)_INC_PATH.
#
# NOTE: This function uses the value of $(_build_) at the time it is called.
#
# Example usage: $(eval $(call Add_Build_Include_Path,$(_cur_module_path_)))
#
define Add_Build_Include_Path
  $(if $(findstring $(1),$(GLOBAL_$(_build_)_INC_PATH)),\
       ,\
       GLOBAL_$(_build_)_INC_PATH += $(1)\
    )
endef

#
# This function adds the specified path to the variable GLOBAL_$(_flavor_)_INC_PATH,
# making sure that the path only appears once. This variable is a list of
# paths searched for header files for all builds with flavor $(_flavor_).
# 
# ARGUMENTS: $(1) path to add to GLOBAL_$(_flavor_)_INC_PATH.
#
# NOTE: This function uses the value of $(_flavor_) at the time it is called.
#
# Example usage: $(eval $(call Add_Flavor_Include_Path,$(_cur_module_path_)))
#
define Add_Flavor_Include_Path
  $(if $(findstring $(1),$(GLOBAL_$(_flavor_)_INC_PATH)),\
       ,\
       GLOBAL_$(_flavor_)_INC_PATH += $(1)\
   )
endef

#
# This function adds the specified path to the variable GLOBAL_INC_PATH,
# making sure that the path only appears once. This variable is a list of
# paths searched for header files for all builds.
# 
# ARGUMENTS: $(1) path to add to GLOBAL_INC_PATH.
#
# Example usage: $(eval $(call Add_Global_Include_Path,$(_cur_module_path_)))
#
define Add_Global_Include_Path
  $(if $(findstring $(1),$(GLOBAL_INC_PATH)),\
       ,\
       GLOBAL_INC_PATH += $(1)\
    )
endef

##############################################################################
#
# This function uses the "find" utility to locate and include the file 
# named by the first argument to this function.
#
# Calling arguments: $(1) is the name of the file to find and include
#                    $(2) is the build name
#
# Example Usage: $(eval $(call Find_And_Include,foo.mk,$(_build_)))
#
define Find_And_Include
  ifdef $(1)_$(2)_PATH
    # Use file sepcified by $(1)_$(2)_PATH, if it is defined.
    $$(eval $(1)_$(2)_extra_makefile := $($(1)_$(2)_PATH))
  
  else
    # Otherwise search for the file starting at the root of the project
    $$(eval $(1)_$(2)_extra_makefile := $$(shell $(FIND) . -name $(1)))
    
    ifeq ($$($(1)_$(2)_extra_makefile),)
      $$(error Project must provide $(1) to complete the build information for this module.)
    endif
    
    ifneq ($$(words $$($(1)_$(2)_extra_makefile)),1)
      $$(error Project has multiple instances of $(1): $$($(1)_$(2)_extra_makefile).)
    endif
    
  endif
  
  include $$($(1)_$(2)_extra_makefile)
endef
##############################################################################

#
# This function checks a build's flavor definition to make sure it is valid. It
# is valid only if there are files named <flavor>*.mak in the MAKE_INCL_DIR;
# otherwise, the specified flavor has no files with rules for building
# 
# ARGUMENTS: $(1) is the name of the build.
#
define Check_For_Valid_Flavor
$(if $(wildcard $(MAKE_INCL_DIR)/$($(1)_FLAVOR)_def.mak),,
    $(error The flavor "$($(1)_FLAVOR)" was specified for build "$(1)" in\
      variable "$(1)_FLAVOR", but there is no file $($(1)_FLAVOR)_def.mak\
      in directory "$(MAKE_INCL_DIR)"))
$(if $(wildcard $(MAKE_INCL_DIR)/$($(1)_FLAVOR)_epilog.mak),,
    $(error The flavor "$($(1)_FLAVOR)" was specified for build "$(1)" in\
      variable "$(1)_FLAVOR", but there is no file $($(1)_FLAVOR)_epilog.mak\
      in directory "$(MAKE_INCL_DIR)"))
$(if $(wildcard $(MAKE_INCL_DIR)/$($(1)_FLAVOR)_prolog.mak),,
    $(error The flavor "$($(1)_FLAVOR)" was specified for build "$(1)" in\
      variable "$(1)_FLAVOR", but there is no file $($(1)_FLAVOR)_prolog.mak\
      in directory "$(MAKE_INCL_DIR)"))
endef
#
# This function checks that a flavor has been specified for the build. The
# flavor specifies the set of files used to define the rules for building files
# for the "build".
# 
# ARGUMENTS: $(1) is the name of the build.
#
define Check_Build_Flavor
$(if $($(1)_FLAVOR),$(call Check_For_Valid_Flavor,$(1)),\
    $(error Flavor for build "$(1)" must be assigned to variable "$(1)_FLAVOR"))
endef
#
# Check to make sure a valid "flavor" has been defined for each "build" and
# then generate FLAVOR_LIST to contain the list of all of the used flavors
# (with no duplicates).
#
FLAVOR_LIST :=

$(foreach _build_,$(BUILD_LIST),$(eval $(call Check_Build_Flavor,$(_build_))))

$(foreach _build_,$(BUILD_LIST),\
  $(if $(findstring $($(_build_)_FLAVOR),$(FLAVOR_LIST)),,\
       $(eval FLAVOR_LIST += $($(_build_)_FLAVOR))))

# If any qnx_* flavor is involved, then the "common" qnx flavor is too.
$(if $(findstring qnx_,$(FLAVOR_LIST)),$(eval FLAVOR_LIST += qnx))

#
# Set ALL_MAKEFILES to the list of all Makefile.* files within the
# project (with relative path).
#
ALL_MAKEFILES := $(shell $(FIND) . -name Makefile*.mk)
#
# Remove the "./" prefix and the "/Makefile.*" suffix to get just the list
# of relative paths to directories containing Makefiles. These are the
# paths to the modules within the project.
#
ALL_MODULE_DIRS := $(patsubst ./%/,%,$(dir $(ALL_MAKEFILES)))
#
# By using "notdir", the "leaf name" is extracted, which is the name used for
# the module. "sort" removes any duplicates (which may exist in the list if
# a module has more than one Makefile).
#
ALL_MODULE_NAMES := $(sort $(notdir $(ALL_MODULE_DIRS)))
#
# Create an empty, simply expanded, variable named <module_name>_DIR for
# each module. This will be filled it with the relative path to the module.
#
$(foreach _mod_,$(ALL_MODULE_NAMES),$(eval $(_mod_)_DIR:= ))
#
# This function extracts the module's name from the relative path to a
# file in the module. It does this by first stripping off the filename suffix
# using "dir". It then uses "patsubst" to strip off the "./" prefix and the
# "/" suffix. Finally, by using "notdir", the leaf subdirectory name is
# obtained, which is the module's name.
# 
# ARGUMENTS: $(1) is the relative path (starting with "./") to a file in the
#            module.
#
define Extract_Module_Name
  $(notdir $(patsubst ./%/,%,$(dir $(1))))
endef
#
# This function adds the name of a Makefile to the list of Makefiles for
# a module. The list is kept in the variable <module_name>_MAKEFILES.
# 
# ARGUMENTS: $(1) is the relative path (starting with "./") to a Makefile for
#            the module.
#
define Add_Module_Makefile
  $(eval $(call Extract_Module_Name,$(1))_MAKEFILES+=$(notdir $(1)))
endef
#
# Add each Makefile in the project to the <module_name>_MAKEFILES variable
# that keeps a list of the makefiles for each module.
#
$(foreach _mf_,$(ALL_MAKEFILES),$(eval $(call Add_Module_Makefile,$(_mf_))))
#
# This function assigns the relative path to a module to the variable
# <module_name>_DIR. In the process, it also verifies that each module
# name is unique within the project. It does this by making sure all assignments
# to a given <module_name>_DIR have the same value. If not, the module name
# is not unique and an error report is issued.
#
# ARGUMENTS: $(1) is the relative path to a module
#
define Define_Module_Dir
$(if $(filter-out $(1),$($(notdir $(1))_DIR)),\
  $(error Module $(notdir $(1)) appears in project more than once),\
  $(eval $(notdir $(1))_DIR := $(1)))
endef
#
# Define a variable <module_name>_DIR for each module, containing the path
# to the module and verify that each module name is unique within the project.
#
$(foreach _dir_,$(ALL_MODULE_DIRS),\
  $(eval $(call Define_Module_Dir,$(_dir_))))
#
# This function makes sure that it is okay to allow a build to have the default
# selection of a Makefile for a module. This is only allowed when a module has
# only one Makefile. If a module has more than one Makefile, then the build
# is required to specify which one to use.
#
# ARGUMENTS: $(1) is the name of the build. $(2) is the name of the module.
#
# RETURN: The variable $(1)-$(2) is assigned the name of the module's only
#         Makefile. If the module has more than one, an error is reported.
#
define Assign_Default_Build_Defn
$(if $(word 2,$($(2)_MAKEFILES)),\
  $(error Makefile for build "$(1)" of module "$(2)" most be specified\
          using variable "$(1)-$(2)" since the module contains more than one\
          Makefile),\
  $(eval $(1)-$(2)_MAKEFILE := $($(2)_MAKEFILES)))
endef
#
# This function verifies that the Makefile specified to use for a build of
# the module is valid. "NONE" is a valid value if the module is to be skipped
# for this "build"; otherwise, the value must be the name of a Makefile in
# the module. If not, an error is reported.
#
# ARGUMENTS: $(1) is the name of the build. $(2) is the name of the module.
#
# RETURN: The variable $(1)-$(2) is assigned either "NONE", or the name of one
#         of the module's Makefiles.
#
define Check_Specified_Build_Defn
$(if $(filter-out NONE,$($(1)-$(2))),\
  $(if $(findstring $($(1)-$(2)),$($(2)_MAKEFILES)),\
    $(eval $(1)-$(2)_MAKEFILE := $($(1)-$(2))), \
    $(error "$($(1)-$(2))" specified by variable "$(1)-$(2)" does not exist in\
            module $(2))),\
  $(eval $(1)-$(2)_MAKEFILE := NONE))
endef
#
# This function adds a module to a build by adding the path to the appropriate
# Makefile to the build's <build>_MAKEFILES variable and by adding the module's
# name to the <build>_MODULE_CANDIDATES variable. The name of the appropriate
# Makefile must already exist in the variable <build>-<module>_MAKEFILE.
#
# ARGUMENTS: $(1) is the name of the build. $(2) is the name of the module.
#
# RETURN: I) The variable $(1)_MAKEFILES has the path to the module's Makefile
#            appended to it.
#         II) $(1)_MODULE_CANDIDATES has the module name appended to it.
#
define Add_Makefile_To_Build
  $(1)_MAKEFILES += $($(2)_DIR)/$($(1)-$(2)_MAKEFILE)
  $(1)_MODULE_CANDIDATES += $(2)
endef
#
# This function excludes a module from a build by adding the module's name
# to the <build>_EXCLUDED variable.
#
# ARGUMENTS: $(1) is the name of the build. $(2) is the name of the module.
#
# RETURN: 
#
define Exclude_Module_From_Build
  $(1)_EXCLUDED += $(2)
endef
#
# This function either assigns the default Makefile for a module to the
# build or verifies that the specified Makefile is valid. The default is
# assigned if the variable <build>-<module> has no value; otherwise, its
# value is interpreted as the name of the Makefile within the module to use
# for "build".
#
# ARGUMENTS: $(1) is the name of the build. $(2) is the name of the module.
#
define Check_Build_Defn
$(if $($(1)-$(2)),\
  $(call Check_Specified_Build_Defn,$(1),$(2)),\
  $(call Assign_Default_Build_Defn,$(1),$(2)))
$(if $(findstring NONE,$($(1)-$(2)_MAKEFILE)),
  $(call Exclude_Module_From_Build,$(1),$(2)),\
  $(call Add_Makefile_To_Build,$(1),$(2)))
endef
#
# For each build, make sure that the list of modules to build is unambiguous.
# By default, any module with a single Makefile will automatically be part
# of a build without any setup required. If a module has more than one
# Makefile, then action is required to specify which Makefile is to be
# used for a build. This is done by creating a variable named
# "<build>-<module_name>" and assigning to it the name of the Makefile within
# the module directory that is to be used; e.g., "Makefile.1". If a value is
# assigned that does not match the name of a file within the module directory,
# then an error is reported and make will terminate. The value of "NONE" can
# be assigned to the variable to cause the module to be not built for that
# particular "build".
#
$(foreach _build_,$(BUILD_LIST),\
  $(foreach _module_,$(ALL_MODULE_NAMES),\
    $(eval $(call Check_Build_Defn,$(_build_),$(_module_)))))
#
# Get list of all Doxygen specification files in the doc subdirectory
#
DXY_FILES := $(wildcard doc/*.dxy)
#
# For each .dxy file, extract the value of PROJECT_NAME from it and make
# a file name of the form doc/<project_name>_REF.chm. CHM_FILES is used to
# keep this list. This is the list of automatically generated Doxygen files.
#
CHM_FILES :=
$(foreach file,$(DXY_FILES),\
          $(eval CHM_FILES += doc/$(shell $(GREP) "^PROJECT_NAME" $(file) |\
                                  $(SED) -e 's,PROJECT_NAME *= *\([^ ]*\),\1_REF.chm,g')))
#############
# Rule: all #
#############
#
# The "all" rule is for building everything: code and documentation.
#
.PHONY: all
all: code docs ;
##############
# Rule: code #
##############
#
# The "code" is made to depend on the "date" rule (so that the date is printed
# at the start of the rule) and the command invokes $(DATE) (so that the date
# is printed at the end). This causes a "make code" to be "timed" (bracketed by
# timestamps).
#
.PHONY: code
code: date bin
	@echo "End:   $(shell $(DATE))"
	@$(if $(ENABLE_CMD_TIMING),echo Build took $$(echo `date +%s.%N`-$(MAKE_START) | $(BC)) seconds,)
##############
# Rule: date #
##############
#
# Set up a rule to run the "date" command.
#
.PHONY: date
date: ; @echo "Start: $(shell $(DATE))"
##############
# Rule: docs #
##############
#
# Set up a rule to generate the documentation
#
.PHONY: docs
docs: docs-ref
##################
# Rule: docs-ref #
##################
#
# Set up a rule to generate the reference manual documentation
#
.PHONY: docs-ref
docs-ref: $(CHM_FILES)
##############
# Rule: flat #
##############
#
# Set up a rule to make a "flat" version of each build. A flat view means
# that all source for the build is copied to a single directory.
#
.PHONY: flat
flat: $(foreach _build_,$(BUILD_LIST),flat-$(_build_))

##################
# Parse_Dep_Files
#
# This function "parses" the specified dependency (.d) file to extract
# the list of prerequisites and append them to the specified variable.
#
# NOTE: This function is highly dependent upon the format of the .d
#       file and is unlikely to work if that format changes.
#
# $(1) = dependency file (with relative path)
# $(2) = name of variable onto which to append the list of prerequisites
#
define Parse_Dep_Files
$(eval deps := $(shell $(CAT) $(1)))
$(eval deps := $(wordlist 2,$(words $(deps)),$(filter-out \,$(deps))))
$(eval $(2) += $(deps))
endef

##################
# Get_Module_Files
#
# This function finds all of the source files for a module and uses
# that list to generate a list of dependency (.d) files that correspond
# to them. It then uses Parse_Dep_Files to extract the list of files
# upon which the source files depend. Finally, sort is used to remove
# any duplicates and the result is assigned to the specified variable.
#
# $(1) = module name.
# $(2) = build name.
# $(3) = name of variable into which to place the list of source files
#
define Get_Module_Files
$(eval mod_name := $(1))
$(eval _build_ := $(2))
$(eval target := $(1)-$(2))
$(eval file_list:=$($(target)_SRCS))
$(if $(file_list),,$(error No source files for '$(target)'))
$(eval dep_files := $(addsuffix .d,$(addprefix $($(_build_)_OBJ_OUTDIR)/,\
                                   $(basename $(notdir $(file_list))))))
$(foreach d_file,$(dep_files),\
  $(if $(wildcard $(d_file)),\
       $(call Parse_Dep_Files,$(d_file),file_list),\
       $(warning WARNING $(d_file) does not exist.)))
$(eval $(3) := $(sort $(file_list)))
endef

##################
# Rule: flat-%
##################
#
# The following rule can be used to create a "flat" view of a module
# and the header files upon which it depends. The targets specified
# must be flat-<module_name>-<build> name. This rule copies the files
# to $($(_build_)_OUTPUT_ROOT)/flat/<module_name>.
#
flat-%:
	$(eval target := $(patsubst flat-%,%,$@))
	$(eval mod_build := $(subst -,$(SPACE),$(target)))
	$(eval mod_name := $(word 1,$(mod_build)))
	$(eval _build_ := $(word 2,$(mod_build)))
	$(if $(filter $(_build_),$(BUILD_LIST)),,\
      $(error '$(_build_)' is not a build from the list '$(BUILD_LIST)'))
	$(if $(filter $(mod_name),$($(_build_)_MODULE_CANDIDATES)),,\
      $(error '$(mod_name)' is not a module for build '$(_build_)'))
	$(call Get_Module_Files,$(mod_name),$(_build_),files)
	$(eval dir := $($(_build_)_OUTPUT_ROOT)/flat/$(mod_name))
	$(if $(wildcard $(dir)),$(RM) -f $(dir)/*,$(MKDIR) -p $(dir))
	$(CP) $(files) $(dir)

##################
# Rule: includes #
##################
#
# This function finds all of the header files in the specified directory
# ($(1)). If any of these files are NOT in the prerequisite list for the
# "includes" target, then there are extra files and the build terminates
# with an error.
#
ifdef ECLOUD_BUILD_ID
define CHECK_INCLUDES
  $(eval INCL_FILES := $(addprefix $(1)/,$(shell $(LS) $(1))))
  $(eval EXTRA_FILES := $(filter-out $^,$(INCL_FILES)))
  $(if $(EXTRA_FILES),$(error Extra files in include area: $(EXTRA_FILES)))
endef
endif
#
# Report an error if the include directories have any inappropriate
# header files. Also make the contents of the include directories
# non-writeable (but leave the directories writeable so that changes
# to the orginals can be copied).
#
.PHONY: includes
includes:
ifdef ECLOUD_BUILD_ID
	$(warning WARNING: Consistency checks of public header files have been disabled)
else
	$(call CHECK_INCLUDES,$(COMMON_INC_PATH))
	$(foreach flavor,$(FLAVOR_LIST),\
              $(call CHECK_INCLUDES,$($(flavor)_INC_PATH)))
endif
ifneq ($(USE_LINKS),1)
ifdef VERBOSE_MAKE
	-$(if $(wildcard $(COMMON_INC_PATH)/*),\
	      $(CHMOD) -f -w $(COMMON_INC_PATH)/* 2>/dev/null;)
	$(foreach flavor,$(FLAVOR_LIST),\
	          $(if $(wildcard $($(flavor)_INC_PATH)/*),\
	               $(CHMOD) -w $($(flavor)_INC_PATH)/* 2>/dev/null;,$(TRUE)))
else
	-@$(if $(wildcard $(COMMON_INC_PATH)/*),\
	      $(CHMOD) -f -w $(COMMON_INC_PATH)/* 2>/dev/null;)\
	$(foreach flavor,$(FLAVOR_LIST),\
	          $(if $(wildcard $($(flavor)_INC_PATH)/*),\
	               $(CHMOD) -w $($(flavor)_INC_PATH)/* 2>/dev/null;,$(TRUE)))
endif
endif

######################
# Rule: sdk          #
######################

sdk-%: %
	$(eval _build_ := $(patsubst sdk-%,%,$@))
	$(if $(filter $(_build_),$(BUILD_LIST)),,\
      $(error '$(_build_)' is not a build from the list '$(BUILD_LIST)'))
	@$(ECHO) Building SDK for $(_build_)
	$(eval out_dir := $(PROJECT_OUTPUT_ROOT)/sdk-$(_build_))
ifdef VERBOSE_MAKE
	$(RM) -rf $(out_dir)
	$(MKDIR) -p $(out_dir)/public_api
	$(MKDIR) -p $(out_dir)/lib
	$(MKDIR) -p $(out_dir)/bin
	$(eval files := $(strip $(filter-out %_cfg.h,$($(_build_)_CFG_FILES))))
	$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/public_api/,)
	$(eval files := $(strip $(wildcard $($(_flavor_)_INC_PATH)/*)))
	$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/public_api/,)
	$(eval files := $(strip $(wildcard $(COMMON_INC_PATH)/*)))
	$(if $(files),$(CP) -Lrfp $(COMMON_INC_PATH)/* $(out_dir)/public_api/,)
	$(eval files := $(strip $(wildcard $(PROJECT_OUTPUT_ROOT)/$(_build_)/bin/*)))
	$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/bin/,)
	$(eval files := $(strip $(wildcard $(PROJECT_OUTPUT_ROOT)/$(_build_)/lib/*)))
	$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/lib/,)
else
	@$(RM) -rf $(out_dir)
	@$(MKDIR) -p $(out_dir)/public_api
	@$(MKDIR) -p $(out_dir)/lib
	@$(MKDIR) -p $(out_dir)/bin
	@$(eval files := $(strip $(filter-out %_cfg.h,$($(_build_)_CFG_FILES))))
	@$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/public_api/,)
	$(eval files := $(strip $(wildcard $($(_flavor_)_INC_PATH)/*)))
	@$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/public_api/,)
	$(eval files := $(strip $(wildcard $(COMMON_INC_PATH)/*)))
	@$(if $(files),$(CP) -Lrfp $(COMMON_INC_PATH)/* $(out_dir)/public_api/,)
	$(eval files := $(strip $(wildcard $(PROJECT_OUTPUT_ROOT)/$(_build_)/bin/*)))
	@$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/bin/,)
	$(eval files := $(strip $(wildcard $(PROJECT_OUTPUT_ROOT)/$(_build_)/lib/*)))
	@$(if $(files),$(CP) -Lrfp $(files) $(out_dir)/lib/,)
endif

.PHONY: sdks
sdks: $(foreach _build_,$(BUILD_LIST),sdk-$(_build_))

##################
# Rule: show-all #
##################
#
# Set up a rule to print out all of the show-* info.
#
.PHONY: show-all
show-all: show-builds show-flavors show-modules show-apis show-friends\
          show-makefiles show-cfg show-src show-deps show-obj
######################
# Rule: show-apis #
######################
#
# Set up a rule to print out the list of automatically discovered public 
# header files in the project.
#
.PHONY: show-apis
show-apis:
ifeq ($(ENABLE_NEW_API_LOGIC),y)
	@$(ECHO)
	@$(ECHO) This project has the following public header files:
	@$(ECHO) ---------------------------------------------------
	@for f in\
         $(shell $(ECHO) $(ALL_API_HDRS) | $(SED) -e 's,[^ ]*_inc/[^ ]*,,g');\
       do echo $$f; done;
	@$(ECHO)
else
	@$(ECHO)
	@$(ECHO) This project has the following public header files:
	@$(ECHO) ---------------------------------------------------
	@for f in\
         $(sort $(shell $(ECHO) $(foreach ext,$(API_EXTENSIONS),\
                                          $(ALL_PUBLIC_$(ext)_FILES)) |\
         $(SED) -e 's,[^ ]*_inc/[^ ]*,,g'));\
       do echo $$f; done;
	@$(ECHO)
endif
#####################
# Rule: show-builds #
#####################
#
# Set up a rule to print out the list of builds.
#
.PHONY: show-builds
show-builds:
	@$(ECHO)
	@$(ECHO) Builds for this project:
	@$(ECHO) "------------------------"
	@for b in $(BUILD_LIST); do echo $$b; done;
	@$(ECHO)
##################
# Rule: show-cfg #
##################
#
# Set up a rule to print out the list of config files for each build.
#
.PHONY: show-cfg
show-cfg:
	@$(ECHO) ;\
    $(ECHO) "----------------------------------------------------------";\
    $(ECHO) ALL_CFG_FILES = $(ALL_CFG_FILES)
	@$(foreach _build_,$(BUILD_LIST),\
       $(if $($(_build_)_SPECIFIC_CFG_FILES),\
         $(ECHO) ;\
         $(ECHO) "----------------------------------------------------------";\
         $(ECHO) $(_build_)_SPECIFIC_CFG_FILES =\
                 $($(_build_)_SPECIFIC_CFG_FILES);))
	@$(foreach _flavor_,$(POTENTIAL_FLAVORS),\
       $(if $($(_flavor_)_SPECIFIC_CFG_FILES),\
         $(ECHO) ;\
         $(ECHO) "----------------------------------------------------------";\
         $(ECHO) $(_flavor_)_SPECIFIC_CFG_FILES =\
                 $($(_flavor_)_SPECIFIC_CFG_FILES);))
	@$(if $(COMMON_CFG_FILES),\
	   $(ECHO) ;\
       $(ECHO) "-----------------------------------------------------------";\
	   $(ECHO) COMMON_CFG_FILES = $(COMMON_CFG_FILES);)
	@$(foreach _build_,$(BUILD_LIST),$(ECHO);\
       $(ECHO) "-----------------";\
       $(ECHO) Build $(_build_):;\
       $(ECHO) "-----------------";\
       $(ECHO) $(_build_)_CFG_DIRS = $($(_build_)_CFG_DIRS); $(ECHO) ;\
       $(ECHO) $(_build_) configuration files:;\
       for f in $(sort $($(_build_)_CFG_FILES)); do echo $$f; done;)
	@$(ECHO)
###################
# Rule: show-deps #
###################
#
# Set up a rule to print out the list of generated dependency files for each
# build.
#
.PHONY: show-deps
show-deps:
	@$(foreach _build_,$(BUILD_LIST),$(ECHO);\
       $(ECHO) Dependency files for $(_build_):;\
       $(ECHO) "--------------------------------";\
       for f in $(sort $($(_build_)_DEP_FILES)); do echo $$f; done;)
	@$(ECHO)
###################
# Rule: show-env  #
###################
#
# Set up a rule to print out the list of generated dependency files for each
# build.
#
.PHONY: show-env
show-env:
	@$(ECHO); uname -a; $(ECHO); env | sort; $(ECHO)
######################
# Rule: show-flavors #
######################
#
# Set up a rule to print out the list of flavors for the builds.
#
.PHONY: show-flavors
show-flavors:
	@$(ECHO) POTENTIAL_FLAVORS = $(POTENTIAL_FLAVORS)
	@$(ECHO)
	@$(foreach _build_,$(BUILD_LIST),\
	   $(ECHO) Build \'$(_build_)\' has flavor \'$($(_build_)_FLAVOR)\'.;)
	@$(ECHO)
######################
# Rule: show-friends #
######################
#
# Set up a rule to print out the list of automatically discovered 'friend'
# header files in the project.
#
.PHONY: show-friends
show-friends:
ifeq ($(ENABLE_NEW_API_LOGIC),y)
	@$(ECHO)
	@$(ECHO) This project has the following \'friend\' header files:
	@$(ECHO) -----------------------------------------------------
	@for f in\
         $(shell $(ECHO) $(ALL_API_HDRS) | $(SED) -e 's,[^ ]*_api/[^ ]*,,g');\
       do echo $$f; done;
	@$(ECHO)
else
	@$(ECHO)
	@$(ECHO) This project has the following \'friend\' header files:
	@$(ECHO) -----------------------------------------------------
	@for f in\
         $(sort $(shell \
           $(ECHO) $(foreach ext,$(API_EXTENSIONS),$(ALL_PUBLIC_$(ext)_FILES)) |\
           $(SED) -e 's,[^ ]*_api/[^ ]*,,g'));\
       do echo $$f; done;
	@$(ECHO)
endif
########################
# Rule: show-makefiles #
########################
#
# Set up a rule to print out the list of makefiles for each build.
#
.PHONY: show-makefiles
show-makefiles:
	@$(foreach _build_,$(BUILD_LIST),$(ECHO);\
       $(ECHO) Makefiles for $(_build_):;\
       $(ECHO) "----------------------------";\
       for f in $(sort $($(_build_)_MAKEFILES)); do echo $$f; done;)
	@$(ECHO)
	@$(ECHO) MAKEFILE_LIST =
	@for f in $(MAKEFILE_LIST); do echo $$f; done;
	@$(ECHO)
######################
# Rule: show-modules #
######################
#
# Set up a rule to print out the list of modules for each build.
#
.PHONY: show-modules
show-modules:
	@$(foreach _build_,$(BUILD_LIST),$(ECHO);\
       $(ECHO) Modules built for $(_build_):;\
       $(ECHO) "----------------------------";\
       for m in $(sort $($(_build_)_MODULES)); do \
           echo $$m; \
       done;)
	@$(ECHO)

##################
# Rule: show-obj #
##################
#
# Set up a rule to print out the list of object files for each build.
#
.PHONY: show-obj
show-obj:
	@$(foreach _build_,$(BUILD_LIST),$(ECHO);\
       $(ECHO) Object files for $(_build_):;\
       $(ECHO) "----------------------------";\
       for f in $(sort $(ALL_$(_build_)_OBJS)); \
       do \
           echo $$f; \
       done;)
	@$(ECHO)
##################
# Rule: show-src #
##################
#
# Set up a rule to print out the list of source files for each build.
#
.PHONY: show-src
show-src:
	@$(foreach _build_,$(BUILD_LIST),$(ECHO);\
       $(ECHO) Source files for $(_build_):;\
       $(ECHO) "----------------------------";\
       for f in $(sort $(call GET_SRC_LIST,$(_build_))); do \
           echo $$f; \
       done;)
	@$(ECHO)
#####################
# Rule: show-tools  #
#####################
#
# Set up a rule to print out the list of generated dependency files for each
# build.
#
.PHONY: show-tools
show-tools:
	-@$(ECHO);$(ECHO) System:; $(ECHO) "-------"; uname -a -v; $(ECHO)
	-@$(ECHO) make:;   $(ECHO) "-----"; which $(MAKE); $(if $(VERBOSE_MAKE),$(MAKE) -v;,)
	-@$(foreach _build_,$(BUILD_LIST),\
	    $(ECHO);\
	    $(ECHO) C Compiler for build $(_build_):;   $(ECHO) "-------------------------------------";\
	            which $(CC_$(_build_));$(if $(VERBOSE_MAKE),$(CC_$(_build_)) $(CC_$(_build_)_VERSION);,)\
	    $(ECHO);\
	    $(ECHO) C++ Compiler for build $(_build_):;   $(ECHO) "---------------------------------------";\
	            which $(CPP_$(_build_));$(if $(VERBOSE_MAKE),$(CPP_$(_build_)) $(CPP_$(_build_)_VERSION);,)\
	    $(ECHO);\
	    $(ECHO) Assembler for build $(_build_):;   $(ECHO) "------------------------------------";\
	            which $(AS_$(_build_));$(if $(VERBOSE_MAKE),$(AS_$(_build_)) $(AS_$(_build_)_VERSION);,)\
	    $(ECHO);\
	    $(ECHO) Linker for build $(_build_):;   $(ECHO) "---------------------------------";\
	            which $(LD_$(_build_));$(if $(VERBOSE_MAKE),$(LD_$(_build_)) $(LD_$(_build_)_VERSION);,)\
	    $(ECHO);\
	    $(ECHO) Archiver for build $(_build_):;   $(ECHO) "-----------------------------------";\
	            which $(AR_$(_build_));$(if $(VERBOSE_MAKE),$(AR_$(_build_)) $(AR_$(_build_)_VERSION);,)\
	    )
	-@$(ECHO)

#
# Create the directories used to hold copies of the public header files.
#
COMMON_INC_PATH := $(PROJECT_OUTPUT_ROOT)/include/common

X_IGNORE := $(shell $(MKDIR) -p $(COMMON_INC_PATH))

$(foreach flavor,$(POTENTIAL_FLAVORS),\
  $(eval $(flavor)_INC_PATH := $(PROJECT_OUTPUT_ROOT)/include/$(flavor)))

$(foreach flavor, $(FLAVOR_LIST),\
  $(eval X_IGNORE := $(shell $(MKDIR) -p $($(flavor)_INC_PATH))))
#
# Include the logic to dynamically discover all public header files and
# create make variables with values equal to their relative paths.
#
ifneq ($(ENABLE_NEW_API_LOGIC),y)

  $(if $(VERBOSE_MAKE),$(info !!! Using original public header file logic !!!))
  include $(MAKE_INCL_DIR)/header_vars.mak
  
endif

#
# Include the logic to dynamically discover all configuration header files.
#
include $(MAKE_INCL_DIR)/cfg_vars.mak

#
# This returns the list of all source files for all builds.
#
define GET_ALL_SRC
$(foreach _build_,$(BUILD_LIST),$(call GET_SRC_LIST,$(_build_)))
endef
#
# This returns the list of source files for the specified build.
#
ifeq ($(ENABLE_NEW_API_LOGIC),y)
  define GET_SRC_LIST
    $($(1)_CFG_FILES) $(ALL_$(1)_ASM_SRCS) $(ALL_$(1)_C_SRCS) $(ALL_$(1)_CPP_SRCS)\
    $(COMMON_API_HDRS) $($($(1)_FLAVOR)_API_HDRS)\
    $(foreach ext,$(API_EXTENSIONS),$(ALL_$(1)_LOCAL_$(ext)))
  endef
else
  define GET_SRC_LIST
    $($(1)_CFG_FILES) $(ALL_$(1)_ASM_SRCS) $(ALL_$(1)_C_SRCS) $(ALL_$(1)_CPP_SRCS)\
      $(foreach ext,$(API_EXTENSIONS),\
        $(ALL_$(1)_LOCAL_$(ext)) $(NON_FLAVOR_PUBLIC_$(ext)_FILES)\
        $($($(1)_FLAVOR)_PUBLIC_$(ext)_FILES))
  endef
endif

#
# Create rule to generate a flat view of the source code for the specified
# ($(1)) build.
#
define MAKE_FLAT_RULE
.PHONY: flat-$(1)
flat-$(1): $(call GET_SRC_LIST,$(1))
	@$$(ECHO) Generating 'flat' version of $$(patsubst flat-%,%,$$@)
	@$$(ECHO) ------------------------------------------------------
	$$(RM) -rf $$($$(patsubst flat-%,%,$$@)_OUTPUT_ROOT)/flat
	$$(MKDIR) -p $$($$(patsubst flat-%,%,$$@)_OUTPUT_ROOT)/flat
	$$(CP) $$^ $$($$(patsubst flat-%,%,$$@)_OUTPUT_ROOT)/flat/
	@$$(ECHO)
endef

##############################################################################
# Create_Build_Rules($(1)) - Create the build rules for the specified build
#
#   $(1) - The name of the build for which the rules are created.
#
# Each build must define a base set of rules for activities like compiling
# a source file, linking a library, etc. The default rules for a build
# are associated with its flavor and are found in the file
# $(MAKE_INCL_DIR)/$($(_build_)_FLAVOR)_def.mak. Various parts of these
# rules are configurable, but it is also possible to completely replace
# them with a different set. To do so, the variable $(_build_)_DEF must
# contain the name of the file to use instead of the default flavor's file.
# This file can be anywhere in the project; the variable's value must contain
# the path (relative to the root of the project) where it is located.
#
define Create_Build_Rules
  $(eval _build_ := $(1))
  $(eval AR_$(_build_)_VERSION:=-V)
  $(eval AS_$(_build_)_VERSION:=-v)
  $(eval CC_$(_build_)_VERSION:=-v)
  $(eval CPP_$(_build_)_VERSION:=-v)
  $(eval LD_$(_build_)_VERSION:=-v)
  $(if $($(_build_)_DEF),\
       $(eval include $($(_build_)_DEF)),\
       $(eval include $(MAKE_INCL_DIR)/$($(_build_)_FLAVOR)_def.mak))
endef

#
# Create the build rules for each build. 
#
$(foreach _build_,$(BUILD_LIST),$(eval $(call Create_Build_Rules,$(_build_))))

##############################################################################
# Exclude_Makefile($(1)) - Exclude the specified makefile from the build
#
#   $(1) - The makefile to be excluded (path relative to root of project).
#
# Function called when Filter_Makefile_Include decides to exclude a makefile
# instead of include it. There is no functionality associated with this call,
# but it does provide a place holder for debugging by inserting a $(info) call.
#
define Exclude_Makefile
endef

##############################################################################
# Include_Makefile($(1)) - Include the specified makefile into the build
#
#   $(1) - The makefile to be included (path relative to root of project).
#
# Function called when Filter_Makefile_Include decides to include a makefile.
#
define Include_Makefile
  include $(1)
endef

##############################################################################
# Filter_Makefile_Include($(1),$(2)) - Apply filter to makefile in deciding
#                                 whether or not to include it for this build.
#
#   $(1) - The makefile to be included/excluded (path relative to root of project).
#   $(2) - List of makefiles to be excluded.
#
define Filter_Makefile_Include
  $(if $(findstring $(1),$(2)),\
       $(call Exclude_Makefile,$(1)),\
       $(call Include_Makefile,$(1)))
endef

##############################################################################
# Include_Makefiles_For_Build($(1),$(2)) - Apply filter to list of makefiles.
#
#   $(1) - The name of the variable containing the list of makefiles to check.
#   $(2) - The list of makefiles to be excluded at this time.
#
define Include_Makefiles_For_Build
  $(foreach makefile,$($(1)),\
            $(eval $(call Filter_Makefile_Include,$(makefile),$(2))))
endef

######################################
#                                    #
# Include .mk files from subprojects #
#                                    #
######################################

#
# Include each of the makefiles for supported builds. These will in turn define
# the rules for their respective modules. Note that the $(_build_) variable
# will be used during the expansion of the included files.
#
# If the project has defined INITIAL_PROJ_MAKEFILES, then those makefiles are
# included first (and in the order specified); otherwise, the order that
# makefiles are included is not something that can be counted on.
#
# Example setting for INITIAL_PROJ_MAKEFILES (note use of '=', not ':='):
#
# INITIAL_PROJ_MAKEFILES =                                     \
#   $(bbc_esol_prk_prf_DIR)/$(bbc_esol_prk_prf_MAKEFILES)      \
#   $(bbc_cunit_framework_DIR)/$(bbc_cunit_framework_MAKEFILES)
#
ifneq ($(INITIAL_PROJ_MAKEFILES),)
###  $(info Including $(INITIAL_PROJ_MAKEFILES))
  $(if $(filter /,$(INITIAL_PROJ_MAKEFILES)),\
    $(error INITIAL_PROJ_MAKEFILES references a non-existent makefile. It evaluates to '$(INITIAL_PROJ_MAKEFILES)')\
   )
  $(foreach _build_,$(BUILD_LIST),\
      $(eval $(call Include_Makefiles_For_Build,INITIAL_PROJ_MAKEFILES))\
   )
endif
###$(info Including $($(_build_)_MAKEFILES))
$(foreach _build_,$(BUILD_LIST),\
  $(eval $(call Include_Makefiles_For_Build,$(_build_)_MAKEFILES,$(INITIAL_PROJ_MAKEFILES))))

#
# Create a target that will generate a flat view of the build's source.
#
$(foreach _build_,$(BUILD_LIST),$(eval $(call MAKE_FLAT_RULE,$(_build_))))

#############
# Rule: obj #
#############

#
# For each build, create a target "obj-<build>" to build its object files.
#
$(foreach _build_,$(BUILD_LIST),$(eval .PHONY: obj-$(_build_)))
$(foreach _build_,$(BUILD_LIST),$(eval obj-$(_build_):$(ALL_$(_build_)_OBJS)))

#
# Set up a rules to build ALL object files.
#
.PHONY: obj
obj: $(foreach _build_,$(BUILD_LIST),obj-$(_build_))

#############
# Rule: lib #
#############

#
# For each build, create a target "lib-<build>" to build its library files.
#
$(foreach _build_,$(BUILD_LIST),$(eval .PHONY: lib-$(_build_)))
$(foreach _build_,$(BUILD_LIST),$(eval lib-$(_build_):$(ALL_$(_build_)_LIBS)))

#
# Set up a rules to build ALL library files.
#
.PHONY: lib
lib: $(foreach _build_,$(BUILD_LIST),lib-$(_build_))

#############
# Rule: bin #
#############

#
# For each build, create a target "bin-<build>" to build its executable files.
#
$(foreach _build_,$(BUILD_LIST),$(eval .PHONY: bin-$(_build_)))
$(foreach _build_,$(BUILD_LIST),$(eval bin-$(_build_):$(ALL_$(_build_)_BINS)))

#
# Set up a rules to build ALL executable files.
#
.PHONY: bin
bin: $(foreach _build_,$(BUILD_LIST),bin-$(_build_))

###################
# Rule: doc/*.chm #
###################

#
# Make each DXY_FILE a prerequisite of its CHM_FILE
#
CHM_RULES := $(join $(addsuffix :,$(CHM_FILES)),$(DXY_FILES))
$(foreach rule,$(CHM_RULES),$(eval $(rule)))

#
# Make all source a prerequisite of any generated documents.
#
ALL_SRC := $(call GET_ALL_SRC)

##############################################################################
# Invoke_Doxygen($(1),$(2)) - Generate Doxygen output
#
#  $(1) - Doxygen config. file (with path relative to project root)
#  $(2) - target directory for Doxygen output (path relative to project root)
#
ifndef VERBOSE_MAKE

define Invoke_Doxygen
 	@$(ECHO) Invoking Doxygen for $(1)
	@$(RM) -f $(2);\
    ( $(CAT) $(1) ; $(ECHO) "HTML_OUTPUT=$(notdir $(2))" )\
     | $(DOXYGEN) - >$(PROJECT_OUTPUT_ROOT)/$(notdir $(2))_doxygen.log
endef

else

define Invoke_Doxygen
 	@$(ECHO) Invoking Doxygen for $(1)
	$(RM) -f $(2);\
    ( $(CAT) $(1) ; $(ECHO) "HTML_OUTPUT=$(notdir $(2))" )\
     | $(DOXYGEN) - >$(PROJECT_OUTPUT_ROOT)/$(notdir $(2))_doxygen.log
endef

endif

##############################################################################
# Invoke_HHC($(1),$(2)) - Use HHC to generate a .chm file from a directory
#
#   $(1) - .chm file to generate (with path relative to project root)
#   $(2) - relative path to directory containing Doxygen output (.hhp)
#
ifndef VERBOSE_MAKE

define Invoke_HHC
 	@$(HHC) $(2)/index.hhp >$(PROJECT_OUTPUT_ROOT)/$(notdir $(2))_hhc.log;\
    $(MV) -f $(2)/index.chm $(1);\
    $(RM) -rf $(2);\
 	$(ECHO) Compiled HTML file $(1) generated
endef

else

define Invoke_HHC
 	$(HHC) $(2)/index.hhp >$(PROJECT_OUTPUT_ROOT)/$(notdir $(2))_hhc.log;\
    $(MV) -f $(2)/index.chm $(1);\
    $(RM) -rf $(2);\
 	$(ECHO) Compiled HTML file $(1) generated
endef

endif

##############################################################################
# Create_CHM_Rules($(1),$(2)) - Create rules for building a .chm file.
#
#   $(1) - <chm_file>:<dxy_file>
#
define Create_CHM_Rules
  $(eval chm_dxy_files := $(subst :, ,$(1)))
  $(eval chm_file := $(word 1,$(chm_dxy_files)))
  $(eval dxy_file := $(word 2,$(chm_dxy_files)))
  $(eval $(chm_file):$(ALL_SRC))
  $(eval chm_dir := $(basename $(chm_file))_html)
  $(eval $(chm_file):$(chm_dir) ; $(call Invoke_HHC,$(chm_file),$(chm_dir)))
  $(eval .INTERMEDIATE:$(chm_dir))
  $(eval $(chm_dir): ; $(call Invoke_Doxygen,$(dxy_file),$(chm_dir)))
endef

#
# Create the rules for building any/all .chm files
#
$(foreach rule,$(CHM_RULES),$(eval $(call Create_CHM_Rules,$(rule))))

#
# include all of the .d files that define the prerequisites for the obj files
#
$(foreach _build_,$(BUILD_LIST),\
  $(eval $(if $($(_build_)_DEP_FILES),-include $($(_build_)_DEP_FILES))))

#
# If PIC_ALL is set, then the requirement is that all code is built with the
# -fPIC option. This means there is no difference between object files built
# for a shared libary and those built for a static library - meaning there is
# no need to build two sets. The logic below verifies that the C and C++
# command options have -fPIC set.
#
$(foreach BUILD,$(BUILD_LIST),\
          $(if $(findstring -fPIC,$(CC_$(BUILD)_FLAGS)),,\
               $(if $(findstring "1",$(PIC_ALL_$(BUILD))),\
                    $(error PIC_ALL_$(BUILD) set, but no -fPIC in CC_$(BUILD)_FLAGS: $(CC_$(BUILD)_FLAGS)),)\
            ))
            
$(foreach BUILD,$(BUILD_LIST),\
          $(if $(findstring -fPIC,$(CPP_$(BUILD)_FLAGS)),,\
               $(if $(findstring "1",$(PIC_ALL_$(BUILD))),\
                    $(error PIC_ALL set, but no -fPIC in CPP_$(BUILD)_FLAGS: $(CPP_$(BUILD)_FLAGS)),)\
            ))

ifeq ($(ENABLE_NEW_API_LOGIC),y)

  $(if $(VERBOSE_MAKE),$(info !!! Using NEW public header file logic !!!))
  
  -include api_callout.mk
  
  # Use sort to eliminate any duplicates
  ALL_API_HDRS:=$(sort $(ALL_API_HDRS))
  #
  # Extract any flavor-specific header files and place them in $(_flavor_)_API_HDRS
  #
  $(foreach _flavor_,$(POTENTIAL_FLAVORS),\
      $(foreach file,$(ALL_API_HDRS),\
          $(if $(or $(findstring $(_flavor_)_api/,$(file)),\
                    $(findstring $(_flavor_)_inc/,$(file))),\
               $(eval $(call ADD_API_FILE,$(_flavor_)_API_HDRS,$(file),$(_flavor_))))))
  #
  # Get complete list of flavor-specific headers
  #
  $(foreach _flavor_,$(POTENTIAL_FLAVORS),$(eval ALL_FLAVOR_API_HDRS+=$($(_flavor_)_API_HDRS)))
  #
  # Eliminate flavor-specifc headers
  #
  TEMP_COMMON_API_HDRS:=$(filter-out $(ALL_FLAVOR_API_HDRS),$(ALL_API_HDRS))
  #
  # Add files one at a time to COMMON_API_HDRS using function that checks for
  # files with the same name from different blocks.
  #
  $(foreach file,$(TEMP_COMMON_API_HDRS),\
      $(eval $(call ADD_API_FILE,COMMON_API_HDRS,$(file))))
  #  
  # Create the rules to copy the headers to the common locations
  #
  $(eval $(call Create_Cp_Ln_Rules,$(COMMON_API_HDRS),$(COMMON_INC_PATH),includes))
  
  $(foreach _flavor_, $(FLAVOR_LIST),\
    $(eval $(call Create_Cp_Ln_Rules,$($(_flavor_)_API_HDRS),\
                                     $($(_flavor_)_INC_PATH),includes)))
  
endif # ifeq ($(ENABLE_NEW_API_LOGIC),y)

ifeq ($(AMBIGUOUS_PUBLIC_APIS),y)
  $(error Terminating build because of ambiguous public header file list.")
endif

#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
endif # ifeq ($(filter clean clean-%,$(MAKECMDGOALS)),)
endif # ($(strip $(MAKECMDGOALS)),)
#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#
# The following definition can be used by any rule to print out the help
# information.
#
HELP :=\
""\
"The following targets are available:"\
"-----------------------------------------------------------------------"\
"  all              : 'code' + 'docs' (build all code and documentation)"\
"  bin              : update all executables and their prerequisites."\
"  bin-<build>      : update all executables for the specified build (and"\
"                        their prerequisites)."\
"  ---------------------------------------------------------------------"\
"  clean            : remove all 'code' for all builds"\
"  clean-all        : 'clean' + 'clean-docs' + 'clean-includes'."\
"  clean-<build>    : clean all code for the specified build."\
"  clean-docs       : remove all files generated by 'docs' target"\
"  clean-includes   : remove all copied public header files."\
"  ---------------------------------------------------------------------"\
"  code             : build code for all builds of all modules (same as"\
"                         'bin')."\
"  date             : display the current date and time."\
"  docs             : autogenerate all documentation"\
"  docs-ref         : autogenerate reference manual documentation"\
"  ---------------------------------------------------------------------"\
"  flat             : generate a 'flat' version of all builds."\
"  flat-<build>     : generate a 'flat' version of the entire project for the"\
"                       specified build."\
"  flat-<module>-<build> : generate a 'flat' version of the specified module"\
"                            for the specified build."\
"  ---------------------------------------------------------------------"\
"  help (default)   : print this help message."\
"  includes         : copy all public header files to their fixed locations."\
"  ---------------------------------------------------------------------"\
"  lib              : update all libraries and their prerequisites."\
"  lib-<build>      : update all libraries for the specified build (and"\
"                        their prerequisites)."\
"  lib-<module>     : update all libraries for all builds of the"\
"                           specified module."\
"  lib-<module>-<build> : update all libraries for the specified build of"\
"                           the specified module."\
"  ---------------------------------------------------------------------"\
"  obj                  : update all object files for all builds."\
"  obj-<build>          : update all object files for the specified build."\
"  obj-<module>         : update all object files for all builds of the"\
"                           specified module."\
"  obj-<module>-<build> : update all object files for the specified build of"\
"                           the specified module."\
"  sdk-<build>          : build an SDK for the specified build."\
"  sdks                 : build an SDK for each build in BUILD_LIST."\
"  ---------------------------------------------------------------------"\
"  show-all         : do all of the show-* targets listed below."\
"  show-apis        : show public API include files for the project." \
"  show-builds      : show the list of builds for the project."\
"  show-cfg         : show configuration .h files for each build."\
"  show-deps        : show list of auto-generated .d files."\
"  show-env         : show environment variables (Linux/QNX only)."\
"  ---------------------------------------------------------------------"\
"  show-flavors     : show the flavor assigned to each build."\
"  show-friends     : show 'friend' include files for the project." \
"  show-makefiles   : show list of makefiles per build."\
"  show-modules     : show list of modules in the project."\
"  show-obj         : show list of object files for each build."\
"  show-src         : show list of source files for each build."\
"  ---------------------------------------------------------------------"\
"  <build-name>     : build everything for the specified build."\
"  <module>-<build> : build only the specified build of the"\
"                       specified module (and its prerequisites)."\
"  <module>         : build all builds of the specified module (and its"\
"                       prerequisites)."\
"  <generated file> : build only the specified generated file (and its"\
"                       prerequisites); e.g., an object file (path required)."\
"-----------------------------------------------------------------------"\
"The following environment variables control build options. These can be"\
"set on the command line; e.g.,  'make code PP_MAKE=1':"\
"-----------------------------------------------------------------------"\
"  PP_MAKE=1        : save the pre-processor output (.i files) in" \
"                       addition to the object files."\
"  VERBOSE_MAKE=1   : show detail of all commands issued by make."\
"-----------------------------------------------------------------------"\
""

##########
# print-%
##########
# This rule aids debugging by printing the value of a variable. It uses
# $(error...) to terminate the build to support the printing of target-
# specific variables without building those targets; i.e., "make print-FOO all"
# will print the value FOO has for the target "all" without also trying to
# build all.
#
print-%: ; @$(error $* is "$($*)". defn = "$(value $*)". origin = $(origin $*))

#
# By making any print-% on the command line a prerequisite of any of the
# command line targets that are not "print-%", the "print-%" is forced
# to run first, providing the target-specific value.
#
$(filter-out print-%,$(MAKECMDGOALS)):$(filter print-%,$(MAKECMDGOALS))

#######
# DUMP
#######
# If "DUMP=1" is added to the make command line, debug information is added
# to each command executed by make. The information includes: 1) The target
# leading to the command, and 2) The prerequisites of the target that are
# newer than it.
#
ifdef DUMP
  OLD_SHELL := $(SHELL)
  SHELL = $(warning target="$@" new="$?")$(OLD_SHELL)
endif

###########
# DUMP_ALL
###########
# If "DUMP_ALL=1" is added to the make command line, debug information is added
# to each command executed by make. The information includes: 1) The target
# leading to the command, 2) The prerequisites of the target that are
# newer than it, 3) All prerequisites of the target, and 4) The command
# executed.
#
ifdef DUMP_ALL
  OLD_SHELL := $(SHELL)
  SHELL = $(warning target="$@" new="$?" all="$^")$(OLD_SHELL) -x
endif

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
#
# -- Revision log prior to 2012 deleted
#
# 07-Aug-2014 Joel Kiger
# + Fix building a single library with the word help in it
#
# 12-Aug-2014 Kirk Bailey Rev 46
# + Added logic to measure "code" build time.
# + Added PIC_ALL default logic
# + USE_LINK now defaults on if host is Linux
#
# 29-Oct-2014 Kirk Bailey Rev 47
# + Added ENABLE_NEW_API_LOGIC option for public header files
#
# 09-Jan-2015 Kirk Bailey Rev 48
# + Added "-f" option when creating symbolic links to allow replacing
# + existing one.
#
#===========================================================================
