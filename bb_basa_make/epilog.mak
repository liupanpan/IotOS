#===========================================================================
# FILE: epilog.mak
#===========================================================================
# Copyright 2008-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: epilog.mak~kok_inf#31:makefile:kok_aud#5 %
# %version: kok_inf#31 %
# %derived_by: dzq92s %
# %date_modified: Wed Nov 12 08:11:13 2014 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the common "epilog" for a module's Makefile. It takes
# the definitions provided by the Makefile that includes this file
# and dynamically generates the make rules for building the module. This
# file currently supports three operations:
#
# 1) Translating assembly or C language source code into object modules.
#
# 2) Linking object modules into static libraries.
#
# 3) Linking one or more object modules with one or more libraries to
#    generate executable images (binaries).
#
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# NOTE: Several variables with names of the form _<name>_ are used to
#       communicate values between "prolog.mak", the module's Makefile, and
#       "epilog.mak". Any changes or additions must be coordinated between
#       all of these files!
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#
#===========================================================================

#
# If a Makefile for a module includes the epilog, then count that module
# as being part of this build. That means a module that does NOT include
# the epilog is NOT considered part of the build. This will be the case
# for modules whose Makefiles have conditional logic that causes them
# to only be built for certain flavors, etc. If a module has a complicated
# Makefile that replaces this epilog, it should perform the following
# line to ensure its module is considered part of the build.
#
$(_build_)_MODULES += $(_cur_module_name_)

#
# The following logic implements a new way of creating rules that copy the
# "public" header files for a block to a common directory. This new logic
# differs from the old method in the following ways:
# 
#   1) Files in arbitrary directories whose names end in "_api" or "_inc" are
#      no longer copied. Only directories following those naming conventions
#      which are in a block whose makefile is processed have their files copied.
#
#   2) The copying of a block's public header files can be disabled by setting
#      $(_cur_module_name_)_NO_API to 'y' for that block's module name.
#
#   3) The standard directory locations can be overridden by assigning the
#      directory paths to $(_cur_module_name_)_API_DIRS. These paths must be
#      the relative path from the directory in which the root Makefile is found.
#
#   4) The option exists for ISA_BLOCKs to have their "public" directory copied. 
#
ifeq ($(ENABLE_NEW_API_LOGIC),y)

  ifeq ($(ISA_BLOCK),1)
  
    ifeq ($($(_cur_module_name_)_NO_API),y)
      # Allow API logic to be disabled for a block
      $(_cur_module_name_)_API_DIRS:=
    else
      # Allow API directories for a module to be overridden; otherwise, use find to
      # pick subdirectories with "public" name.
      ifeq ($($(_cur_module_name_)_API_DIRS),)
        
        $(_cur_module_name_)_API_DIRS := \
          $(shell $(FIND) $(_cur_module_path_) -type d -name "public" | $(SED) -e 's,^\./,,g')
      
      endif # ifeq ($($(_cur_module_name_)_API_DIRS),)
      
    endif # ifeq ($($(_cur_module_name_)_NO_API),y)
  
  else
  
    ifeq ($($(_cur_module_name_)_NO_API),y)
      # Allow API logic to be disabled for a block
      $(_cur_module_name_)_API_DIRS:=
    else
      # Allow API directories for a module to be overridden; otherwise, use find to
      # pick subdirectories with "_api" or "_inc" names.
      ifeq ($($(_cur_module_name_)_API_DIRS),)
        
        $(_cur_module_name_)_API_DIRS := \
          $(shell $(FIND) $(_cur_module_path_) -type d -name "*_api" -o -name "*_inc" | $(SED) -e 's,^\./,,g')
      
      endif # ifeq ($($(_cur_module_name_)_API_DIRS),)
      
    endif # ifeq ($($(_cur_module_name_)_NO_API),y)
    
  endif # ifeq ($(ISA_BLOCK),1)
  
  $(eval $(call Add_API_Paths,$($(_cur_module_name_)_API_DIRS)))
  
endif # ifeq ($(ENABLE_NEW_API_LOGIC),y)

###############################################################################
# Definitions for translating source code into object code:
###########################################################

#
# Get the list of local (private) header files for this module
#
$(foreach ext,$(API_EXTENSIONS),\
  $(eval ALL_$(_build_)_LOCAL_$(ext) += \
         $(foreach _src_dir_,$(_src_dirs_),$(wildcard $(_src_dir_)/*.$(ext)))))
#
# Get the list of assembler language source files for this module
#
ifeq ($(_find_asm_srcs_),1)
  _asm_srcs_ :=\
    $(foreach _src_dir_,$(_src_dirs_),$(wildcard $(_src_dir_)/*$(_asm_ext_)))
endif

ALL_$(_build_)_ASM_SRCS += $(_asm_srcs_)
#
# Create a list of object files, one for each assembler source file. Then
# generate <target>:<prerequisite> rules, where the object file is the target
# and the assembler source file is the prerequisite.
#
_asm_objs_ := \
  $(addprefix $(_objdir_)/,\
              $(patsubst %$(_asm_ext_),%$(_obj_ext_),$(notdir $(_asm_srcs_))))

_asm_obj_rules_ := $(join $(addsuffix :,$(_asm_objs_)),$(_asm_srcs_))

#
# Get the list of C language source files for this module
#
ifeq ($(_find_c_srcs_),1)
  _c_srcs_ := $(foreach _src_dir_,$(_src_dirs_),$(wildcard $(_src_dir_)/*.c))
endif

ALL_$(_build_)_C_SRCS += $(_c_srcs_)
#
# Create a list of object files, one for each C source file. Then
# generate <target>:<prerequisite> rules where the object file is the target
# and the C source file is the prerequisite.
#
_c_shared_objs_ :=
_c_shared_obj_rules_ :=
ifneq ($(PIC_ALL_$(_build_)),1)
  ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
    _c_shared_objs_ := $(addprefix $(_objdir_shared_)/,\
                                   $(patsubst %.c,%$(_obj_ext_),$(notdir $(_c_srcs_))))
  
    _c_shared_obj_rules_ := $(join $(addsuffix :,$(_c_shared_objs_)),$(_c_srcs_))
  endif # ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
endif

_c_objs_ := $(addprefix $(_objdir_)/,\
                        $(patsubst %.c,%$(_obj_ext_),$(notdir $(_c_srcs_))))

_c_obj_rules_ := $(join $(addsuffix :,$(_c_objs_)),$(_c_srcs_)) $(_c_shared_obj_rules_)

#
# Create a list of dependency files, one for each C object file.
#
$(_build_)_DEP_FILES += $(patsubst %$(_obj_ext_),%.d,$(_c_objs_) $(_c_shared_objs_))

#
# Get the list of C++ language source files for this module
#
ifeq ($(_find_cpp_srcs_),1)
  _cpp_srcs_ := $(foreach _src_dir_,$(_src_dirs_),$(wildcard $(_src_dir_)/*.cpp))
endif

ALL_$(_build_)_CPP_SRCS += $(_cpp_srcs_)

$(_cur_module_target_)_SRCS := $(_asm_srcs_) $(_c_srcs_) $(_cpp_srcs_)
#
# Create a list of object files, one for each C++ source file. Then
# generate <target>:<prerequisite> rules where the object file is the target
# and the C source file is the prerequisite.
#
_cpp_shared_objs_ :=
_cpp_shared_obj_rules_ :=
ifneq ($(PIC_ALL_$(_build_)),1)
  ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
    _cpp_shared_objs_ := $(addprefix $(_objdir_shared_)/,\
                                     $(patsubst %.cpp,%$(_obj_ext_),$(notdir $(_cpp_srcs_))))
    
    _cpp_shared_obj_rules_ := $(join $(addsuffix :,$(_cpp_shared_objs_)),$(_cpp_srcs_))
  endif # ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
endif

_cpp_objs_ := $(addprefix $(_objdir_)/,\
                          $(patsubst %.cpp,%$(_obj_ext_),$(notdir $(_cpp_srcs_))))

_cpp_obj_rules_ := $(join $(addsuffix :,$(_cpp_objs_)),$(_cpp_srcs_)) $(_cpp_shared_obj_rules_)
#
# Create a list of dependency files, one for each C++ object file.
#
$(_build_)_DEP_FILES += $(patsubst %$(_obj_ext_),%.d,$(_cpp_objs_) $(_cpp_shared_objs_))

#
# Make the module's target a prerequisite of the build target so that all
# modules associated with a build can be built by specifying the build
# as the target.
#
.PHONY: $(_build_)
$(_build_):$(_cur_module_target_)
#
# For a module target "<module>-<build>", create a target "<module>" and
# make "<module>-<build>" a prerequisite of it. This allows all builds
# of a module to be built via "make <module>".
#
.PHONY: $(_cur_module_name_)
.PHONY: $(_cur_module_target_)
$(_cur_module_name_):$(_cur_module_target_)

#
# The object files need to be made dependent on their makefile; so that,
# they will be rebuilt if the makefile is changed.
# The list of include directories for a module is unique; so, a target-specific
# rule is defined to assign the unique value to INCL_DIRS when any of the
# module's object files are built.
#
_cur_module_objs_ := $(strip $(_asm_objs_) $(_c_objs_) $(_c_shared_objs_) $(_cpp_objs_) $(_cpp_shared_objs_))

ifneq "$(_cur_module_objs_)" ""

  ALL_$(_build_)_OBJS += $(_cur_module_objs_)
  #
  # Create a target-specific assignment of the block's name to BB_NAME so that
  # commands expanded to build the object files will have access to it
  #
  $(_cur_module_objs_):BB_NAME:=$(_cur_module_name_)
  #
  # The module target used to have its object files as a prerequisite. This
  # caused object files to be generated for the module target even if they
  # were not needed for a library or binary image. This is no longer the
  # case; the object files will only be built for the module target if
  # they are prerequisites of something else. This change was needed to
  # support modules containing libraries (and their source) which developers
  # would not have to build.
  #
  ###$(_cur_module_target_):$(_cur_module_objs_)
  #
  # The following targets allow an individual module's object files to be
  # built even if they are not used in a library or program.
  #
  .PHONY:obj-$(_cur_module_target_)
  obj-$(_cur_module_target_):$(_cur_module_objs_)
  .PHONY:obj-$(_cur_module_name_)
  obj-$(_cur_module_name_):obj-$(_cur_module_target_)

  #
  # includes is an order-only-prerequisite: just because its commands are
  # executed does not mean the object files need to be updated.
  #
  $(_cur_module_objs_):$(_cur_makefile_) | includes
  #
  # Note that SIMPLE_INCL_DIRS is a simply-expanded variable, which means its
  # value is assigned now by expanding the current values of the variables
  # assigned to it. Later changes to these variables will not affect this value.
  #
  $(_cur_module_objs_):SIMPLE_INCL_DIRS:=\
     $(_cur_module_inc_path_) $(SPECIAL_INC_PATH) $(_src_dirs_)\
     $($(_build_)_CFG_DIRS) $($(_flavor_)_INC_PATH) $(COMMON_INC_PATH)
  #
  # INCL_DIRS is recursively expanded, which means the variables assigned to
  # it will only be evaluated when the INCL_DIRS needs to be expanded. This
  # will happen when the compile command is formed. Note that it will have
  # the above simply-expanded value of SIMPLE_INCL_DIRS. It also allows for
  # a more complex recursive expansion (through use of a target-specific
  # value for RECURSIVE_INC_PATH). The "GLOBAL_*" include variables provide
  # a mechanism for defining a hierarchy of include paths based on 1) build,
  # 2) build flavor, and 3) all builds. See "Add_*_Include_Path" functions
  # in project.mak for how to add paths to these variables.
  #
  $(_cur_module_objs_):INCL_DIRS=\
    $($(BB_NAME)_INC) $(GLOBAL_$(BUILD)_INC_PATH) $(GLOBAL_$($(BUILD)_FLAVOR)_INC_PATH)\
    $(GLOBAL_INC_PATH) $(RECURSIVE_INC_PATH) $(SIMPLE_INCL_DIRS)

endif

#
# Generate a rule to build each object file from its source file.
#
$(foreach _rule_,\
  $(_asm_obj_rules_),$(eval $(call MAKE_$(_build_)_ASSEMBLE_RULE,$(_rule_))))

$(foreach _rule_,\
  $(_c_obj_rules_),$(eval $(call MAKE_$(_build_)_COMPILE_C_RULE,$(_rule_))))

$(foreach _rule_,\
  $(_cpp_obj_rules_),$(eval $(call MAKE_$(_build_)_COMPILE_CPP_RULE,$(_rule_))))

###############################################################################

###############################################################################
# Definitions for linking static libraries from object files:
#############################################################

_lib_obj_files_:=

# Skip library stuff unless _lib_name_ has a value
ifneq "$(_lib_name_)" ""
#vvvvvvvvvvvvvvvvvvvvvvv#

  #---------------------------------------------------------------------
  ifeq "$(_lib_dir_)" "" # Build the library in $($(_build_)_LIB_OUTDIR)
  #---------------------------------------------------------------------
    #
    # If this flavor supports shared libraries, then two sets of rules are
    # needed: one for shared objects/libraries and one for static objects/
    # libraries.
    #
    ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)

      _lib_file_ := $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)$(_lib_name_).so

      ifneq ($(PIC_ALL_$(_build_)),1)
        # get list of shared objects by substituting "/obj/shared/" for /obj/" 
        _shared_obj_list_ := $(subst /obj/,/obj/shared/,$(_lib_objs_))
        _lib_obj_files_ += $(_shared_obj_list_)
      else
        # object files are the same for shared/static libraries
        _shared_obj_list_ := $(_lib_objs_)
      endif
      # Create a rule making the shared object files prerequisites of the shared lib.
      _lib_rule_:=$(_lib_file_):$(_shared_obj_list_)
      # Rebuild the library if its makefile changes.
      $(_lib_file_):$(_cur_makefile_)
      # Create the rule to build the shared library.
      $(eval $(call MAKE_$(_build_)_SHARED_LIB_RULE,$(_lib_rule_)))
      # The library is a prerequisite of the current module's target.
      ### DISABLED!$(_cur_module_target_):$(_lib_file_) # DISABLED!
      
      # Keep track of all libraries used for this build
      ALL_$(_build_)_LIBS += $(_lib_file_)

      # Keep value of _lib_libs_ in LIB_FILES for each _lib_file_ so it is available
      # during the command expansion, if needed.
      $(_lib_file_):LIB_FILES:=$(_lib_libs_)
      
      # Create a target-specific value of LIB_DIRS for the .so.
      $(_lib_file_):LIB_DIRS:=$($(_build_)_LIB_OUTDIR) $(_lib_lib_dirs_)
  
      # Rebuild the library if any of the libraries it uses change.
      _lib_prerequisites_ :=\
          $(foreach _lib_,$(_lib_libs_),$(if $(findstring /,$(_lib_)),$(_lib_)))
  
      ifneq "$(_lib_prerequisites_)" ""
        $(_lib_file_):$(_lib_prerequisites_)
      endif

      ifneq ($(PIC_ALL_$(_build_)),1)
        $(_shared_obj_list_): CC_$(_build_)_FLAGS  += -fPIC
        $(_shared_obj_list_): CPP_$(_build_)_FLAGS += -fPIC
      endif
      #
      # The following targets allow an individual module's library to be
      # built even if it is not used in a program.
      #
      .PHONY:lib-$(_cur_module_target_)
      lib-$(_cur_module_target_):$(_lib_file_)
      .PHONY:lib-$(_cur_module_name_)
      lib-$(_cur_module_name_):lib-$(_cur_module_target_)

    endif # ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
    #
    # Static library rules:
    #
    ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
      _lib_file_ := $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)$(_lib_name_).a
    else
      _lib_file_ := $($(_build_)_LIB_OUTDIR)/$(_lib_prefix_)$(_lib_name_)$(_lib_ext_)
    endif # ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
   
    # If object files have directories specified, use them "as is"; otherwise,
    # prepend $(_objdir_)/ to the object file names
    _static_obj_list_ :=\
      $(foreach obj_file,$(_lib_objs_),\
                $(if $(subst ./,,$(dir $(obj_file))),$(obj_file),$(_objdir_)/$(obj_file))\
       )

    _lib_obj_files_ += $(_static_obj_list_)
    # Create a rule making the static object files prerequisites of the static lib.
    _lib_rule_:=$(_lib_file_):$(_static_obj_list_)
    # Rebuild the library if its makefile changes.
    $(_lib_file_):$(_cur_makefile_)
    # Create the rule to build the static library.
    $(eval $(call MAKE_$(_build_)_LIB_RULE,$(_lib_rule_)))
    # The library is a prerequisite of the current module's target.
    ### DISABLED! $(_cur_module_target_):$(_lib_file_) # DISABLED!
     
    # Keep track of all libraries used for this build
    ALL_$(_build_)_LIBS += $(_lib_file_)
    #
    # The following targets allow an individual module's library to be
    # built even if it is not used in a program.
    #
    .PHONY:lib-$(_cur_module_target_)
    lib-$(_cur_module_target_):$(_lib_file_)
    .PHONY:lib-$(_cur_module_name_)
    lib-$(_cur_module_name_):lib-$(_cur_module_target_)
   
  #---------------------------------------------------------------------
  else # A non-standard location for the library was specified
  #---------------------------------------------------------------------
    
    # Make sure the specified directory (and ../lst) exist
    X_IGNORE := $(shell $(MKDIR) -p $(_lib_dir_) $(_lib_dir_)/../lst)
    #
    # If this flavor supports shared libraries, then two sets of rules are
    # needed: one for shared objects/libraries and one for static objects/
    # libraries.
    #
    ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)

      _lib_file_ := $(_lib_dir_)/$(_lib_prefix_)$(_lib_name_).so

      ifneq ($(PIC_ALL_$(_build_)),1)
        # get list of shared objects by substituting "/obj/shared" for /obj/" 
        _shared_obj_list_ := $(subst /obj/,/obj/shared/,$(_lib_objs_))
        _lib_obj_files_ += $(_shared_obj_list_)
      else
        # object files are the same for shared/static libraries
        _shared_obj_list_ := $(_lib_objs_)
      endif
      # Because the location of the library is being specified, we need to make
      # sure the rule to build it is only created once, even if the same library
      # is used for multiple builds.
      ifndef $(_lib_file_)_DEFINED
        $(_lib_file_)_DEFINED := 1
        _lib_rule_:=$(_lib_file_):$(_shared_obj_list_)
        $(_lib_file_):BUILD:=$(_build_)
        $(_lib_file_):$(_cur_makefile_)
        $(eval $(call MAKE_$(_build_)_SHARED_LIB_RULE,$(_lib_rule_)))
      endif # ifndef $(_lib_file_)_DEFINED

      $(_cur_module_target_):$(_lib_file_)
    
      # A copy of the library is placed in the standard location for typical use
      copied_lib := $($(_build_)_LIB_OUTDIR)/$(notdir $(_lib_file_))
      $(copied_lib):$(_lib_file_) # update the copy if the original changes
	   $(if $(VERBOSE_MAKE),$(CP),@$(CP)) -fp $< $@
      
      # Keep track of all libraries used for this build
      ALL_$(_build_)_LIBS += $(_lib_file_)

      ifneq ($(PIC_ALL_$(_build_)),1)
        $(_shared_obj_list_): CC_$(_build_)_FLAGS  += -fPIC
        $(_shared_obj_list_): CPP_$(_build_)_FLAGS += -fPIC
      endif
      #
      # The following targets allow an individual module's library to be
      # built even if it is not used in a program.
      #
      .PHONY:lib-$(_cur_module_target_)
      lib-$(_cur_module_target_):$(_lib_file_)
      .PHONY:lib-$(_cur_module_name_)
      lib-$(_cur_module_name_):lib-$(_cur_module_target_)

    endif # ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
    #
    # Static library rules:
    #
    ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
      _lib_file_ := $(_lib_dir_)/$(_lib_prefix_)$(_lib_name_).a
    else
      _lib_file_ := $(_lib_dir_)/$(_lib_prefix_)$(_lib_name_)$(_lib_ext_)
    endif # ifeq ($($(_flavor_)_SUPPORTS_SHARED_LIBS),y)
   
    _static_obj_list_ :=\
      $(foreach obj_file,$(_lib_objs_),$(if $(dir $(obj_file)),$(obj_file),$(_objdir_)/$(obj_file)))

    _lib_obj_files_ += $(_static_obj_list_)
    # Because the location of the library is being specified, we need to make
    # sure the rule to build it is only created once, even if the same library
    # is used for multiple builds.
    ifndef $(_lib_file_)_DEFINED
      $(_lib_file_)_DEFINED := 1
      _lib_rule_:=$(_lib_file_):$(_static_obj_list_)
      $(_lib_file_):BUILD:=$(_build_)
      $(_lib_file_):$(_cur_makefile_)
      $(eval $(call MAKE_$(_build_)_LIB_RULE,$(_lib_rule_)))
    
    endif # ifndef $(_lib_file_)_DEFINED

    $(_cur_module_target_):$(_lib_file_)
    
    # A copy of the library is placed in the standard location for typical use
    copied_lib := $($(_build_)_LIB_OUTDIR)/$(notdir $(_lib_file_))
    $(copied_lib):$(_lib_file_) # update the copy if the original changes
	 $(if $(VERBOSE_MAKE),$(CP),@$(CP)) -fp $< $@

    # Keep track of all libraries used for this build
    ALL_$(_build_)_LIBS += $(copied_lib)
  
    # Only build the object files if the library needs to be built. Without
    # this, the object files would always be built which would result in no
    # build-time savings from distributing the pre-built library.
    .INTERMEDIATE: $(_lib_obj_files_)
    #
    # The following targets allow an individual module's library to be
    # built even if it is not used in a program.
    #
    .PHONY:lib-$(_cur_module_target_)
    lib-$(_cur_module_target_):$(_lib_file_)
    .PHONY:lib-$(_cur_module_name_)
    lib-$(_cur_module_name_):lib-$(_cur_module_target_)

  #---------------------------------------------------------------------
  endif # ifeq "$(_lib_dir_)" ""
  #---------------------------------------------------------------------

#^^^^^^^^^^^^^^^^^^^^^^^^#
endif # ifneq "$(_lib_name_)" ""

###############################################################################

###############################################################################
# Definitions for building executables from object files and libraries:
######################################################################

# Skip bin rules unless _bin_name_ has a value
ifneq "$(_bin_name_)" ""
#vvvvvvvvvvvvvvvvvvvvvvv#

  #---------------------------------------------------------------------
  ifeq "$(_bin_dir_)" "" # Build the binary in $($(_build_)_BIN_OUTDIR)
  #---------------------------------------------------------------------

    # Create a rule making the _bin_objs_ prerequisites of the binary.
ifeq ($(filter %$(_bin_ext_),$(_bin_name_)),)
    _bin_file_:=$($(_build_)_BIN_OUTDIR)/$(_bin_name_)$(_bin_ext_)
else
    _bin_file_:=$($(_build_)_BIN_OUTDIR)/$(_bin_name_)
endif
    _bin_rule_:=$(_bin_file_):$(_bin_objs_)
    
    # The binary is a prerequisite of the current module's target.
    $(_cur_module_target_):$(_bin_file_)
    
    # Rebuild the binary if its makefile changes.
    $(_bin_file_):$(_cur_makefile_)
    
    # Create the rule to build the binary file
    $(eval $(call MAKE_$(_build_)_BIN_RULE,$(_bin_rule_)))
    
    # Keep track of all binaries used for this build
    ALL_$(_build_)_BINS += $(_bin_file_)

  #---------------------------------------------------------------------
  else # A non-standard location for the binary was specified
  #---------------------------------------------------------------------

    # Only build the object files if the binary needs to be built. Without
    # this, the object files would always be built which would result in no
    # build-time savings from distributing the pre-built binary.
    .INTERMEDIATE: $(_bin_objs_)
    
    # Make sure the specified directory (and ../lst) exist
    X_IGNORE := $(shell $(MKDIR) -p $(_bin_dir_) $(_bin_dir_)/../lst)
    
ifeq ($(filter %$(_bin_ext_),$(_bin_name_)),)
    _bin_file_:=$(_bin_dir_)/$(_bin_name_)$(_bin_ext_)
else
    _bin_file_:=$(_bin_dir_)/$(_bin_name_)
endif
    # Because the location of the binary is being specified, we need to make
    # sure the rule to build it is only created once, even if the same binary
    # is used for multiple builds.
    ifndef $(_bin_file_)_DEFINED
    
      $(_bin_file_)_DEFINED := 1
      _bin_rule_:=$(_bin_file_):$(_bin_objs_)
      $(_bin_file_):BUILD:=$(_build_)
      $(_bin_file_):$(_cur_makefile_)
      $(eval $(call MAKE_$(_build_)_BIN_RULE,$(_bin_rule_)))
    
    endif # ifndef $(_bin_file_)_DEFINED
    
    # Build the binary as part of the current module's target.
    $(_cur_module_target_):$(_bin_file_)
    
    # A copy of the binary is placed in the standard location for typical use
    copied_bin := $($(_build_)_BIN_OUTDIR)/$(notdir $(_bin_file_))
    
    $(copied_bin):$(_bin_file_) # update the copy if the original changes
	  $(if $(VERBOSE_MAKE),$(CP),@$(CP)) -fp $< $@
    
    # Keep track of all binaries used for this build
    ALL_$(_build_)_BINS += $(copied_bin)

  #---------------------------------------------------------------------
  endif # ifeq "$(_bin_dir_)" ""
  #---------------------------------------------------------------------
  
  # Rebuild the binary if any of the libraries used for it change.
  _lib_prerequisites_ :=\
      $(foreach _lib_,$(_bin_libs_),$(if $(findstring /,$(_lib_)),$(_lib_)))
  
  ifneq "$(_lib_prerequisites_)" ""
    $(_bin_file_):$(_lib_prerequisites_)
  endif
  
  #
  # Create a target-specific rule that assigns the LIBS variable a unique
  # value for this binary. The value is the list of libraries used during
  # the link process to create the binary. This is needed because the LIBS
  # variable is expanded when the link command is executed and so must have
  # a unique value for that target.
  #
  ifeq (($(filter .a .so,$(_lib_ext_))),) # non-standard library extension
  
    $(_bin_file_):\
      LIBS:=$(patsubst $(_lib_prefix_)%$(_lib_ext_),%,$(notdir $(_bin_libs_)))
  
  else # The following is used for the standard ".a", ".so" extensions
  
      #
      # The following logic converts a list (_bin_libs_) of libraries of the
      # form "<path>/lib<name>.a" or "<path>/lib<name>.so" to "<name>" while
      # preserving their order. Also, any libraries whose names do not match
      # one of these patterns (are, instead of the form <path>/<name>.<ext>)
      # are "passed through" as "<name>.<ext>".
      #
      _lib_names_ := $(foreach _lib_,$(notdir $(_bin_libs_)),\
                       $(if $(filter $(_lib_prefix_)%.a,$(_lib_)),\
                            $(patsubst $(_lib_prefix_)%.a,%,$(_lib_)),\
                            $(if $(filter $(_lib_prefix_)%.so,$(_lib_)),\
                                 $(patsubst $(_lib_prefix_)%.so,%,$(_lib_)),\
                                 $(_lib_)\
                              )\
                          )\
                        )

      $(_bin_file_):LIBS:=$(_lib_names_)

  endif # ifeq (($(filter .a .so,$(_lib_ext_))),)

  # Keep value of _bin_libs_ in LIB_FILES for each _bin_file_ so it is available
  # during the command expansion, if needed.
  $(_bin_file_):LIB_FILES:=$(_bin_libs_)
  
  # Create a target-specific value of LIB_DIRS for the binary.
  $(_bin_file_):LIB_DIRS:=$($(_build_)_LIB_OUTDIR) $(_bin_lib_dirs_)
  
  # Rebuild the binary if the link command file changes.
  $(_bin_file_):$(_bin_lnk_cmd_)
  
  # Create a target-specific value of LNK_CMD_FILE for the binary.
  $(_bin_file_):LNK_CMD_FILE:=$(_bin_lnk_cmd_)

#^^^^^^^^^^^^^^^^^^^^^^^^#
endif

###############################################################################


ISA_BLOCK := 0     # Clear prior to creating rules for next block
OBJ_DIR_OVERRIDE:= # Clear prior to creating rules for next block

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
#
# ...Revision details prior to Aug 2010 removed...
#
# 16-Apr-2014 Kirk Bailey Rev 28
# + Added _lib_libs_ and _lib_lib_dirs_ to allow .so files to be linked
# + against other libraries (similar to use of _bin_libs_ and _bin_lib_dirs_
# + for binaries).
#
# 17-Apr-2014 Kirk Bailey Rev 29
# + Fixed prerequisites for libraries that link against other libraries.
# + Removed -fPIC -shared for .so builds; -fPIC is now always used.
#
# 08-Aug-2014 Kirk Bailey Rev 30
# + Added logic to support ISA_BLOCKs.
#
# 27-Oct-2014 Kirk Bailey Rev 31
# + Made target-specific assignment of BB_NAME work for BASA blocks too.
# + Added ENABLE_NEW_API_LOGIC option for public header files
#
#===========================================================================
