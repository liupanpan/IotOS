#===========================================================================
# FILE: linux_all_def.mak
#===========================================================================
# Copyright 2010-2015 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: linux_all_def.mak~kok_inf#26:makefile:kok_basa#1 %
# %version: kok_inf#26 %
# %derived_by: dzq92s %
# %date_modified: Tue May 12 13:29:39 2015 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================
#
# gcc Tool Definitions
#

ifneq ($($(_build_)_USES_PATH),y)
  ifndef GCC_$(_build_)_PATH
    $(error Must define path to the gcc compiler as GCC_$(_build_)_PATH!)
  endif
  ifneq ($($(_build_)_NO_LTIB),y)
    include $(MAKE_INCL_DIR)/linux_ltib.mak
    TOOLCHAIN_$(_build_)_CXXFLAGS := $(TOOLCHAIN_$(_build_)_CFLAGS)
  endif
else
  ifdef GCC_$(_build_)_ENV_SCRIPT
    # Extract Build Parameters from the supplied environment script.
    include $(MAKE_INCL_DIR)/linux_envscript.mak
  endif
endif

ifeq ($(strip $(AR_$(_build_))),)
  AR_$(_build_)  := $(GCC_$(_build_)_PATH)ar
endif
ifeq ($(strip $(AS_$(_build_))),)
  AS_$(_build_)  := $(GCC_$(_build_)_PATH)gcc
endif
ifeq ($(strip $(CC_$(_build_))),)
  CC_$(_build_)  := $(GCC_$(_build_)_PATH)gcc
endif
ifeq ($(strip $(CPP_$(_build_))),)
  CPP_$(_build_)  := $(GCC_$(_build_)_PATH)g++
endif
ifeq ($(strip $(LD_$(_build_))),)
  LD_$(_build_)  := $(GCC_$(_build_)_PATH)gcc
endif

###############################################################################
# Individual gcc Tool Options:
#
# Note that these are only defined default values for the build if they
# have not already been defined.
#

#
# Default to Position Independent Code for everything (not just .so) unless
# explicitly told otherwise.
#
ifeq "$(origin PIC_ALL_$(_build_))" "undefined"
  PIC_ALL_$(_build_) := 1
endif

ifeq "$(origin CC_$(_build_)_BASE)" "undefined"
  CC_$(_build_)_BASE := $(TOOLCHAIN_$(_build_)_CFLAGS) -D_GNU_SOURCE -DLINUX
  ifeq ($(PIC_ALL_$(_build_)),1)
    CC_$(_build_)_BASE += -fPIC
  endif
endif

ifeq "$(origin CC_$(_build_)_BT)" "undefined"
  CC_$(_build_)_BT := -fasynchronous-unwind-tables -fexceptions -fnon-call-exceptions -funwind-tables -fstack-protector-all
endif

ifeq "$(origin CC_$(_build_)_DBG)" "undefined"
  CC_$(_build_)_DBG := -g
endif

ifeq "$(origin CC_$(_build_)_GCOV)" "undefined"
  ifdef GCOV_MAKE
    CC_$(_build_)_GCOV := -fprofile-arcs -ftest-coverage
  else
    CC_$(_build_)_GCOV :=
  endif
endif

ifeq "$(origin CC_$(_build_)_OPT)" "undefined"
  CC_$(_build_)_OPT := -O2
endif

ifeq "$(origin CC_$(_build_)_WARN)" "undefined"
  ifeq "$(origin CC_$(_build_)_C99)" "undefined"
    CC_$(_build_)_WARN := -ansi -Wall -std=c89 -Wswitch-default -Wdeclaration-after-statement -Wfloat-equal -Wshadow -Wundef -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
  else
    CC_$(_build_)_WARN := -Wall -std=c99 -Wswitch-default -Wdeclaration-after-statement -Wfloat-equal -Wshadow -Wundef -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
  endif
endif


ifeq "$(origin CPP_$(_build_)_BASE)" "undefined"
  CPP_$(_build_)_BASE := $(TOOLCHAIN_$(_build_)_CXXFLAGS) -D_GNU_SOURCE -DLINUX
  ifeq ($(PIC_ALL_$(_build_)),1)
    CPP_$(_build_)_BASE += -fPIC
  endif
endif

ifeq "$(origin CPP_$(_build_)_BT)" "undefined"
  CPP_$(_build_)_BT := -fasynchronous-unwind-tables -fexceptions -fnon-call-exceptions -funwind-tables -fstack-protector-all
endif

ifeq "$(origin CPP_$(_build_)_DBG)" "undefined"
  CPP_$(_build_)_DBG := -g
endif

ifeq "$(origin CPP_$(_build_)_GCOV)" "undefined"
  ifdef GCOV_MAKE
    CPP_$(_build_)_GCOV := -fprofile-arcs -ftest-coverage 
  else
    CPP_$(_build_)_GCOV :=
  endif
endif

ifeq "$(origin CPP_$(_build_)_OPT)" "undefined"
  CPP_$(_build_)_OPT := -O2
endif

ifeq "$(origin CPP_$(_build_)_WARN)" "undefined"
  CPP_$(_build_)_WARN := -ansi -Wall -Wundef -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
endif


ifeq "$(origin LD_$(_build_)_BASE)" "undefined"
  LD_$(_build_)_BASE := $(TOOLCHAIN_$(_build_)_LDFLAGS)
endif

ifeq "$(origin LD_$(_build_)_GCOV)" "undefined"
  ifdef GCOV_MAKE
    LD_$(_build_)_GCOV := -fprofile-arcs -ftest-coverage
  else
    LD_$(_build_)_GCOV :=
  endif
endif

# To get rid of errors when building with GCOV and shared libraries, the
# shared libraries must be statically linked against gcov. The errors were of
# the form:
#
# "...hidden symbol `__gcov_merge_add' in /usr/lib/gcc/i686-linux-gnu/4.6/libgcov.a"
#
ifeq "$(origin LD_$(_build_)_GCOV_SO)" "undefined"
  ifdef GCOV_MAKE
    LD_$(_build_)_GCOV_SO := -lgcov
  else
    LD_$(_build_)_GCOV_SO :=
  endif
endif

###############################################################################

###############################################################################
# Composite gcc Tool Options:
###############################
#
# Options for archiving (generating a static library)
#
ifeq "$(origin AR_$(_build_)_FLAGS)" "undefined"
  AR_$(_build_)_FLAGS = rcs
endif
#
# Options for the C compiler
#
ifeq "$(origin CC_$(_build_)_FLAGS)" "undefined"
  CC_$(_build_)_FLAGS =\
    $(CC_$(BUILD)_BASE) $(CC_$(BUILD)_BT) $(CC_$(BUILD)_WARN) $(CC_$(BUILD)_DBG)\
    $(CC_$(BUILD)_OPT) $(CC_$(BUILD)_GCOV) $(CC_$(BUILD)_MACRO)\
    $(patsubst %,-I %,$(INCL_DIRS))
endif
#
# Options for the C++ compiler
#
ifeq "$(origin CPP_$(_build_)_FLAGS)" "undefined"
  CPP_$(_build_)_FLAGS =\
    $(CPP_$(BUILD)_BASE) $(CPP_$(BUILD)_BT) $(CPP_$(BUILD)_WARN) $(CPP_$(BUILD)_DBG)\
    $(CPP_$(BUILD)_OPT) $(CPP_$(BUILD)_GCOV) $(CPP_$(BUILD)_MACRO)\
    $(patsubst %,-I %,$(INCL_DIRS))
endif
#
# Options for linking a dynamic library (.so)
#
###SO_EXT := .1
###SO_$(_build_)_NO_UNDEF := -Wl,--no-undefined
ifeq "$(origin SO_$(_build_)_FLAGS)" "undefined"
  SO_$(_build_)_FLAGS := -shared -fPIC $(SO_$(_build_)_NO_UNDEF) -Wl,-soname,$$(notdir $$@)$$(SO_EXT)
endif
#
# Options for linking to form an executable image
#
ifeq "$(origin LD_$(_build_)_FLAGS)" "undefined"
  LD_$(_build_)_FLAGS = $(LD_$(BUILD)_BASE) $(LD_$(BUILD)_GCOV)
endif
###############################################################################


###############################################################################
# Commands for compiling C files:
#################################
#
# The following variable contains the command for compiling a .c file using
# the gcc tool chain. Note that it uses recursive expansion.
#

#
# This is the command used to compile a C file into an object file. Note that
# it also generates the dependency (.d) file - all with one compiler invocation.
#
ifndef CC_CMD_$(_build_)
  define CC_CMD_$(_build_)
    $(if $(ENABLE_CMD_TIMING),$(eval cc_start_$@:=$$(shell date +%s.%N)),)\
    $(CC_$(BUILD)) $(CC_$(BUILD)_FLAGS) -MMD -MP -MT $@ -c -o $@ $< &&\
    $(ECHO) "" >> $(basename $@).d && $(ECHO) "$<:" >> $(basename $@).d\
    $(if $(ENABLE_CMD_TIMING),\
         && $(ECHO) "  Compiled $(notdir $<) for $(BUILD) in $$($(ECHO) `date +%s.%N`-$(cc_start_$@) | $(BC)) sec",)
  endef
endif

#
# Create a make rule for generating an object file from C source
#
define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(if $$(ENABLE_CMD_TIMING),,$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)")
ifeq ($$(PP_MAKE),1)
	@$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$< 
endif
	@$$(CC_CMD_$$(BUILD))
ifdef ASM_MAKE
	@$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	@$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$< 
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifeq ($$(PP_MAKE),1)
	$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	$$(CC_CMD_$$(BUILD))
ifdef ASM_MAKE
	$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$< 
endif
ifeq ($$(PP_MAKE),2)
	$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C++ files:
###################################
#
# The following variable contains the command for compiling a .c file using
# the gcc tool chain. Note that it uses recursive expansion.
#

#
# This is the command used to compile a C++ file into an object file. Note that
# it also generates the dependency (.d) file - all with one compiler invocation.
#
ifndef CPP_CMD_$(_build_)
  define CPP_CMD_$(_build_)
    $(if $(ENABLE_CMD_TIMING),$(eval cpp_start_$@:=$$(shell date +%s.%N)),)\
    $(CPP_$(BUILD)) $(CPP_$(BUILD)_FLAGS) -MMD -MP -MT $@ -c -o $@ $< &&\
    $(ECHO) "" >> $(basename $@).d && $(ECHO) "$<:" >> $(basename $@).d\
    $(if $(ENABLE_CMD_TIMING),\
          && $(ECHO) "  Compiled $(notdir $<) for $(BUILD) in $$($(ECHO) `date +%s.%N`-$(cpp_start_$@) | $(BC)) sec",)
  endef
endif

#
# Create a make rule for generating an object file from C++ source
#
define MAKE_$(_build_)_COMPILE_CPP_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(if $$(ENABLE_CMD_TIMING),,$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)")
ifeq ($$(PP_MAKE),1)
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	@$$(CPP_CMD_$$(BUILD))
ifdef ASM_MAKE
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),2)
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifeq ($$(PP_MAKE),1)
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	$$(CPP_CMD_$$(BUILD))
ifdef ASM_MAKE
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),2)
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).i $$<
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a static library:
########################################

#
# This is the command used to create a static archive file from object files.
#
ifndef AR_CMD_$(_build_)
  define AR_CMD_$(_build_)
    $(if $(ENABLE_CMD_TIMING),$(eval ar_start_$@:=$$(shell date +%s.%N)),)\
    $(RM) -f $@ && $(AR_$(BUILD)) $(AR_$(BUILD)_FLAGS) $@ $(filter %.o,$^)\
    $(if $(ENABLE_CMD_TIMING),\
         && $(ECHO) "  Linked $(notdir $@) for $(BUILD) in $$($(ECHO) `date +%s.%N`-$(ar_start_$@) | $(BC)) sec",)
  endef
endif

#
# Create a make rule to generate a .a file from its constituent object files
#
define MAKE_$(_build_)_LIB_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(if $$(ENABLE_CMD_TIMING),,$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)")
	@$$(AR_CMD_$$(BUILD))
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AR_CMD_$$(BUILD))
	@$$(ECHO)
endif # ifndef VERBOSE_MAKE
endef # MAKE_$(_build_)_LIB_RULE

###############################################################################
# Commands for linking a shared library:
########################################

#
# The following builds the link command for a .so file in a way that allows 
# libraries to be linked against it. These can be static libraries (whose
# contents must have been built with -fPIC) or other .so libraries. The rules
# for how the list of libraries is used is the same as for LD_CMD_$(_build_) -
# refer to its comment for details.
#
ifndef SO_CMD_$(_build_)
  define SO_CMD_$(_build_)
    $(eval obj_files_$@ := $(filter %.o,$^))
    $(eval archives_$@  := $(filter %.a,$(LIB_FILES)))
    $(eval shared_$@    := $(filter %.so,$(LIB_FILES)))
    $(eval others_$@    := $(addprefix -l,$(basename $(notdir $(filter-out $(archives_$@) $(shared_$@),$(LIB_FILES))))))
    $(eval archives_$@  := $(addprefix -l,$(patsubst %.a,%,$(patsubst lib%.a,%,$(notdir $(archives_$@))))))
    $(eval shared_$@    := $(addprefix -l,$(patsubst %.so,%,$(patsubst lib%.so,%,$(notdir $(shared_$@))))))
    $(eval flags_$@     := $(SO_$(BUILD)_FLAGS))
    $(if $(ENABLE_CMD_TIMING),$(eval so_start_$@:=$$(shell date +%s.%N)),)\
    $(LD_$(BUILD)) $(flags_$@) -o $@ $(obj_files_$@)\
        $(addprefix -L,$(LIB_DIRS)) -Wl,--start-group -Wl,-Bstatic $(archives_$@) -Wl,--end-group\
        -Wl,-Bdynamic $(shared_$@) $(others_$@) $(NON_STD_LIBS) $(LD_$(BUILD)_GCOV_SO) \
    $(if $(ENABLE_CMD_TIMING),\
         && $(ECHO) "  Linked $(notdir $@) for $(BUILD) in $$($(ECHO) `date +%s.%N`-$(so_start_$@) | $(BC)) sec",)
  endef
endif # SO_CMD_$(_build_)

#
# Create a make rule to generate a .so file from its constituent object files
#
define MAKE_$(_build_)_SHARED_LIB_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(if $$(ENABLE_CMD_TIMING),,$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)")
	@$$(SO_CMD_$$(BUILD))
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(SO_CMD_$$(BUILD))
	@$$(ECHO)
endif # ifndef VERBOSE_MAKE
endef # MAKE_$(_build_)_SHARED_LIB_RULE

###############################################################################
# Commands for linking building an executable image:
####################################################

#
# The following builds the link command line for an executable in a way that
# allows a mixing of both static and shared libraries. Which type (static or
# shared) can be specified by including the appropriate file extension (.a or
# .so). A library can appear with it's full name ("lib" prefix and ".a"/".so"
# extension), in which case the prefix and extension are removed (with the
# extension used to place the library in the appropriate group, as described
# next).
#
# All specified static libraries are linked against first, using a "-Wl,-Bstatic"
# linker directive and -Wl,--start-group/-Wl,--end-group delimiters to insure
# that libraries that reference one another's symbols link properly. 
# and suffix are removed (and the suffix is used to generate the previously
# described "-Wl,-Bstatic" or "-Wl,Bdynamic").
#
# After static libraries, a -Wl,Bdynamic directive is issued followed by the
# list of shared libraries (and "unspecified" libraries - see below). Since
# the -Wl,Bdynamic directive comes last, the dynamic version of the system
# will be linked against.
#
# A library can be listed without a "lib" prefix or extension, in which case its
# type is "unspecified". These libraries follow the list of dynamic libraries.
# These are typically "system" libraries and, because of where they are placed,
# will follow the typical system convention of linking against a shared library
# if one exists; otherwise, using a static library that exists.
#
# Note that all libraries described above have a "-l" prepended to their basename.
# Some packages don't follow the convention of naming libraries with  a "lib"
# prefix; hence, the "-l" option cannot be used to identify these. For these
# corner cases, the $(NON_STD_LIBS) variable can be used. It is appended "as is"
# to the end of the link command and so it can list files that will not have a
# "-l" specifier generated for them. A target-specific assignment of NON_STD_LIBS
# should be made to use it (the target being the executable being built, with
# its complete relative path).
#
ifndef LD_CMD_$(_build_)
  define LD_CMD_$(_build_)
    $(eval obj_files_$@ := $(filter %.o,$^))
    $(eval archives_$@  := $(filter %.a,$(LIB_FILES)))
    $(eval shared_$@    := $(filter %.so,$(LIB_FILES)))
    $(eval others_$@    := $(addprefix -l,$(basename $(notdir $(filter-out $(archives_$@) $(shared_$@),$(LIB_FILES))))))
    $(eval archives_$@  := $(addprefix -l,$(patsubst %.a,%,$(patsubst lib%.a,%,$(notdir $(archives_$@))))))
    $(eval shared_$@    := $(addprefix -l,$(patsubst %.so,%,$(patsubst lib%.so,%,$(notdir $(shared_$@))))))
    $(if $(ENABLE_CMD_TIMING),$(eval ld_start_$@:=$$(shell date +%s.%N)),)\
    $(LD_$(BUILD)) $(LD_$(BUILD)_FLAGS) -o $@ $(obj_files_$@)\
      $(addprefix -L,$(LIB_DIRS)) -Wl,--start-group -Wl,-Bstatic $(archives_$@) -Wl,--end-group\
      -Wl,-Bdynamic $(shared_$@) $(others_$@) $(NON_STD_LIBS)\
    $(if $(ENABLE_CMD_TIMING),\
         && $(ECHO) "  Linked $(notdir $@) for $(BUILD) in $$($(ECHO) `date +%s.%N`-$(ld_start_$@) | $(BC)) sec",)
  endef
endif # LD_CMD_$(_build_)

#
# Create a make rule to generate an executable from its comstituent object
# files and libraries.
#
define MAKE_$(_build_)_BIN_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(if $$(ENABLE_CMD_TIMING),,$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)")
	@$$(LD_CMD_$$(BUILD))
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Linking $$(notdir $$@) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(LD_CMD_$$(BUILD))
endif
endef

#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 24-may-2010 kirk bailey
# + Created initial file.
#
# 23-jun-2010 kirk bailey version 2
# + Added simultaneous support for static and shared libraries for flavors
# + that support them (e.g., Linux, QNX).
#
# 17-Nov-2010 kirk bailey version 3
# + Task 16508: Added gcov build support.
#
# 11-aug-2011 kirk bailey
# + Added logic to include linux_ltib.mk to support multiple tool sets
# + in ltib environment.
#
# 03-Nov-2011 Kirk Bailey
# + Added CC/CPP_$(_build_)_BT to support -funwind-tables compile option
# + required to get stack bactrace to work for ARM (no affect for X86).
#
# 08-Jan-2012 Kirk Bailey
# + Added -fnon-call-exceptions to support backtrace from signal handlers.
#
# 16-Jan-2012 Kirk Bailey
# + Added -fasync-unwind-tables and -fexceptions.
# + Fixed "ar" command to remove previous file (otherwise it only appends
# + to the existing one.
#
# 18-Jan-2012 Kirk Bailey
# + Improved method used to generate .d file with gcc. The new way also
# + creates an empty rule for the .h file. This prevents builds from breaking
# + if a header file gets renamed and the .d file has the old name.
#
# 29-Jan-2012 Kirk Bailey
# + Added empty rule to generated .d file to allow build to work if the
# + source file is moved or renamed.
#
# 13-Apr-2012 Kirk Bailey
# + Changed way libraries are linked into an executable to adapt to change
# + in gcc 4.6. The --start-group/--end-group only applies to archives and
# + as of 4.6 does not work the same way if share libraries are placed in
# + the group. Now, static libraries are linked against first (in the order
# + listed), followed by dynamic libraries. This only applies to libraries
# + that were explicitly called out with a ".a" or ".so" extension - those
# + without such a designation continue to be linked the same way.
#
# 15-Jul-2013 Larry Piekarski Rev 13
# + Added the -Wundef warning for both C and C++ builds. C99 states that
#   that an undefined symbol will evaluate TRUE when used in the #if context.
#   This could lead to unexpected issues. For instance, if the code has a
#   "#if FEATURE_ENABLED" and FEATURE_ENABLED was not defined because the
#   author assumed the feature would not enabled, it would, in fact, be
#   enabled. This warning would indicate this. This was discovered on SBX
#   where a feature was specifically turned off in a cfg file, but the 
#   cfg file was only included in some, but not all, c files. So, different
#   functions either had or did not have this feature enabled.
#
# 25-Sep-2013 Larry Piekarski Rev 14
# + Task 1290: Reversed version control comments - no functional change.
#
# 25-Sep-2013 Larry Piekarski Rev 15
# + Task 1290: Enabled -Wextra warning. -Wextra is a collection of new
#   warnings, and disabled 2 that are not desirable. -Wunused-parameter 
#   issues a warning if argument in a function is not used. 
#   -Wmissing-field-initializers gives a warning for initializing a
#   structure wtih {0} instead of explicitly initializing each field. 
#   Both of these are common design patterns in IDI and we don't want to 
#   make them warnings, so those two were specifically disabled.
#
# 01-Oct-2013 Larry Piekarski Rev 16
# + Task 1486: Added -fstack-protector-all to the backtrace options. This
#   enables stack smashing protection for gcc. What it does is this: When
#   the calling function calls a function, it pushes its return address on
#   the stack, and jumps to the called function. The called function then
#   pushes a canary word on the stack, and then allocates its local storage.
#   When the called function is about to exit, it pops the local storage 
#   off the stack, and then checks the canary word. If the canary word is
#   still valid, then it pops the canary word and jumps to the return 
#   address. If the canary word is corrupt, indicating an accidental or
#   malicious local variable overflow, then gcc causes an assert.
#   bb_cratch is a library that catches this assert and ties in with
#   bb_signal_dbg to give useful debug information. The positive aspect
#   of this protection is that the program would probably crash anyway,
#   but now it crashes at the location of the stack corruption. The 
#   downside is that it increases the program size slightly in every
#   function. For ROM contrained programs, this default action can be 
#   disabled with -fno_stack_protector_all. 
#
# 20-Nov-2013 Chris Baker Rev 17
# + Task 3052: Add support for Yocto builds via linux_envscript.mak.
# + Added TOOLCHAIN_$(_build_)_CXXFLAGS to allow separate flags for C++ and
#   C compilation. This is not provided by LTIB environment, so the variable
#   is set to be a copy of the CFLAGS.
# + Added TOOLCHAIN_$(_build_)_LDFLAGS to supply additional flags to the
#   link stage. This is not provided by LTIB environment.
#
# 19-Feb-2014 Kirk Bailey Rev 18
# + Added fix to allow GCOV to be used with shared libraries.
#
# 16-Apr-2014 Kirk Bailey Rev 19
# + Added ability to link a .so against libraries by using new _lib_libs_
# + variable.
#
# 17-Apr-2014 Kirk Bailey Rev 20
# + Added -fPIC to all compiles so that all object files are usable in
# + a .so. Separated flags for building a .so into a separate variable
# + and made it easy to add the -Wl,--no-undefined option in development.
#
# 23-Apr-2014 Joel Kiger Rev 21
# + Corrected soname flag for linking shared object
#
# 29-Jul-2014 Paul Casto Rev 22
# + Changed "NO_LTIB" flag to "$($(_build_)_NO_LTIB)" so different builds can
# + have different LTIB status.
#
# 12-Sep-2014 Kirk Bailey Rev 23
# + Synced with recent improvements to QNX build.
# + Added logic to allow individual build command times to be output.
#
# 12-Nov-2014 Kirk Bailey Rev 24
# + Switch from using ';' to '&&' for command lines with multiple commands
# + so that the return code reflects any failures.
#
# 09-Jan-2015 Kirk Bailey Rev 25
# + Eliminated use of echo's "-e" option which is supported by bash but
# + not some other shells (like dash).
#
# 12-May-2015 Kirk Bailey Rev 26
# + Made it easier to switch to C99 for a build by defining CC_$(_build_)_C99.
#===========================================================================
