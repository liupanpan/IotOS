#===========================================================================
# FILE: qnx_def.mak
#===========================================================================
# Copyright 2010-2014 Delphi Technologies, Inc., All Rights Reserved.
# Delphi Confidential
#---------------------------------------------------------------------------
# %full_filespec: qnx_def.mak~kok_inf#34:makefile:kok_aud#1 %
# %version: kok_inf#34 %
# %derived_by: jz4fpf %
# %date_modified: Mon Mar 23 14:02:16 2015 %
#---------------------------------------------------------------------------
#
# DESCRIPTION:
# ------------
# This file defines the rules specific to performing builds for this flavor.
#
#===========================================================================

#
# QNX Tool Definitions
#
ifndef QCC_CPU_VARIANT_$(_build_)
  $(error QCC_CPU_VARIANT_$(_build_) is not defined.)
endif

ifndef QCC_ROOT
  QCC_ROOT := /opt/qnx660
endif

ifndef QNX_HOST
  export QNX_HOST := $(QCC_ROOT)/host/linux/x86
endif

ifndef QNX_TARGET
  export QNX_TARGET := $(QCC_ROOT)/target/qnx6
endif

ifdef QNX_$(_build_)_BSP
  QNX_BSP  := $(QNX_$(_build_)_BSP)
endif

ifndef QNX_BSP
  QNX_BSP  := $(QCC_ROOT)/bsp
endif

ifndef QCC_TOOL_PATH
  QCC_TOOL_PATH := $(QNX_HOST)/usr/bin
endif

ifndef PROCESSOR_$(_build_)
  $(error PROCESSOR_$(_build_) not defined)
endif

ifneq ($(filter armv7le,$(PROCESSOR_$(_build_))),)
	LIB_BSP_DIRS_$(_build_) := $(QNX_BSP)/armle-v7/usr/lib $(QNX_BSP)/armle-v7/lib $(QNX_TARGET)/armle-v7/usr/lib
else
	LIB_BSP_DIRS_$(_build_) := $(QNX_BSP)/x86/usr/lib $(QNX_BSP)/x86/lib $(QNX_TARGET)/x86/usr/lib
endif

INCL_BSP_DIRS_$(_build_) := $(QNX_BSP)/usr/include

ifndef AS_$(_build_)
  AS_$(_build_) := $(QCC_TOOL_PATH)/qcc -V$(QCC_CPU_VARIANT_$(_build_))
endif
ifndef CC_$(_build_)
  CC_$(_build_) := $(QCC_TOOL_PATH)/qcc -V$(QCC_CPU_VARIANT_$(_build_))
endif
ifndef CPP_$(_build_)
  CPP_$(_build_) := $(QCC_TOOL_PATH)/qcc -V$(QCC_CPU_VARIANT_$(_build_))
endif
ifndef LD_$(_build_)
  LD_$(_build_) := $(QCC_TOOL_PATH)/qcc -V$(QCC_CPU_VARIANT_$(_build_))
endif
ifndef MKEFS_$(_build_)
  MKEFS_$(_build_) := $(QCC_TOOL_PATH)/mkefs
endif
ifndef MKIFS_$(_build_)
  MKIFS_$(_build_) := $(QCC_TOOL_PATH)/mkifs
endif
ifndef MKIMAGE_$(_build_)
  MKIMAGE_$(_build_) := $(QCC_TOOL_PATH)/mkimage
endif
ifndef USEMSG_$(_build_)
  USEMSG_$(_build_)  := $(QCC_TOOL_PATH)/usemsg
endif

ifndef AR_$(_build_)
  ifneq ($(filter armv7le,$(PROCESSOR_$(_build_))),)
    AR_$(_build_)      := $(QCC_TOOL_PATH)/ntoarmv7-ar
    STRIP_$(_build_)   := $(QCC_TOOL_PATH)/ntoarmv7-strip
  else
    AR_$(_build_)      := $(QCC_TOOL_PATH)/nto$(PROCESSOR_$(_build_))-ar
    STRIP_$(_build_)   := $(QCC_TOOL_PATH)/nto$(PROCESSOR_$(_build_))-strip
  endif
endif

###############################################################################
# Individual qcc Tool Options:
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

ifeq "$(origin AS_$(_build_)_BASE)" "undefined"
  AS_$(_build_)_BASE := -c
  ifeq ($(PIC_ALL_$(_build_)),1)
    AS_$(_build_)_BASE += -fPIC
  endif
endif

ifeq "$(origin CC_$(_build_)_BASE)" "undefined"
  CC_$(_build_)_BASE := -c -D_QNX_SOURCE=1 -DQNX_NTO
  ifeq ($(PIC_ALL_$(_build_)),1)
    CC_$(_build_)_BASE += -fPIC
  endif
endif

ifeq "$(origin CC_$(_build_)_BT)" "undefined"
  CC_$(_build_)_BT := -fasynchronous-unwind-tables -fexceptions -fnon-call-exceptions -funwind-tables -fstack-protector-all
endif

ifdef $(_build_)_CPU
  CC_$(_build_)_BASE += -D_MICRO_$($(_build_)_CPU)_
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
  CC_$(_build_)_WARN := -Wall -std=c99 -Wswitch-default -Wdeclaration-after-statement -Wfloat-equal -Wshadow -Wundef -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
endif

ifeq "$(origin CPP_$(_build_)_BASE)" "undefined"
  CPP_$(_build_)_BASE := -c -lang-c++ -D_QNX_SOURCE=1 -DQNX_NTO
  ifeq ($(PIC_ALL_$(_build_)),1)
    CPP_$(_build_)_BASE += -fPIC
  endif
endif

ifeq "$(origin CPP_$(_build_)_BT)" "undefined"
  CPP_$(_build_)_BT := -fasynchronous-unwind-tables -fexceptions -fnon-call-exceptions -funwind-tables -fstack-protector-all
endif

ifdef $(_build_)_CPU
  CPP_$(_build_)_BASE += -D_MICRO_$($(_build_)_CPU)_
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



ifeq "$(origin MKEFS_$(_build_)_FLAGS)" "undefined"
  MKEFS_$(_build_)_FLAGS =\
     -vvvv -l"$(subst $(SPACE),$(EMPTY),[search=$(MKEFS_$(BUILD)_PATH))]"
endif

#
# Default setting for MKEFS search path attribute. Note that the default
# is to use the same paths as MKIFS.
#
ifeq "$(origin MKEFS_$(_build_)_PATH)" "undefined"
  MKEFS_$(_build_)_PATH = $(MKIFS_$(BUILD)_PATH)
endif

#
# Default MKIFS path for items within the QNX installation
#
ifeq "$(origin MKIFS_$(_build_)_DEFAULT_PATH)" "undefined"
  MKIFS_$(_build_)_DEFAULT_PATH := $(QNX_TARGET)/$(PROCESSOR_$(_build_))/sbin;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/usr/sbin;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/boot/sys;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/bin;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/usr/bin;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/lib;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/lib/dll;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/usr/lib;$(QNX_TARGET)/$(PROCESSOR_$(_build_))/usr/photon/bin
endif

#
# Default MKIFS for items within the project
#
ifeq "$(origin MKIFS_$(_build_)_PROJ_PATH)" "undefined"
  MKIFS_$(_build_)_PROJ_PATH :=\
    $(PROJECT_OUTPUT_ROOT)/$(_build_)/bin;$(PROJECT_OUTPUT_ROOT)/$(_build_)/lib
endif

#
# Default setting for MKIFS_PATH env. variable used by MKIFS
#
ifeq "$(origin MKIFS_$(_build_)_PATH)" "undefined"
  MKIFS_$(_build_)_PATH =\
        $(MKIFS_$(BUILD)_PROJ_PATH);$(MKIFS_$(BUILD)_DEFAULT_PATH)
endif

ifeq "$(origin MKIFS_$(_build_)_FLAGS)" "undefined"
  MKIFS_$(_build_)_FLAGS := -vvvv
endif

ifeq "$(origin MKIMAGE_$(_build_)_FLAGS)" "undefined"
  MKIMAGE_$(_build_)_FLAGS := -b128K
endif

ifeq "$(origin USEMSG_$(_build_)_FLAGS)" "undefined"
  USEMSG_$(_build_)_FLAGS :=
endif

ifeq "$(origin STRIP_$(_build_)_FLAGS)" "undefined"
  STRIP_$(_build_)_FLAGS :=
endif
###############################################################################

###############################################################################
# Composite qcc Tool Options:
###############################
#
# Options for archiving (generating a static library)
#
ifeq "$(origin AR_$(_build_)_FLAGS)" "undefined"
  AR_$(_build_)_FLAGS = rcs
endif
#
# Options for the assembler
#
COMMA := ,# Just a comma
ifeq "$(origin AS_$(_build_)_FLAGS)" "undefined"
  AS_$(_build_)_FLAGS = $(AS_$(BUILD)_BASE)\
    -Wa,-I. $(patsubst %,-Wa$(COMMA)-I%,$(INCL_DIRS))\
    -Wa,-I. $(patsubst %,-Wa$(COMMA)-I%,$(INCL_BSP_DIRS_$(BUILD)))\
    -Wa,-I $(QNX_TARGET)/usr/include\
    -Wa,-I $(PROJECT_OUTPUT_ROOT)/include/qnx\
    -Wa,-I $(QNX_BSP)/usr/include
endif
#
# Options for the C compiler
#
ifeq "$(origin CC_$(_build_)_FLAGS)" "undefined"
  CC_$(_build_)_FLAGS =\
    $(CC_$(BUILD)_BASE) $(CC_$(BUILD)_BT) $(CC_$(BUILD)_WARN) $(CC_$(BUILD)_DBG)\
    $(CC_$(BUILD)_OPT) $(CC_$(BUILD)_GCOV) $(CC_$(BUILD)_MACRO)\
    $(patsubst %,-I %,$(INCL_DIRS))\
    $(patsubst %,-I %,$(INCL_BSP_DIRS_$(BUILD)))
endif
#
# Options for the C++ compiler
#
ifeq "$(origin CPP_$(_build_)_FLAGS)" "undefined"
  CPP_$(_build_)_FLAGS =\
    $(CPP_$(BUILD)_BASE) $(CPP_$(BUILD)_BT) $(CPP_$(BUILD)_WARN) $(CPP_$(BUILD)_DBG)\
    $(CPP_$(BUILD)_OPT) $(CPP_$(BUILD)_GCOV) $(CPP_$(BUILD)_MACRO)\
    $(patsubst %,-I %,$(INCL_DIRS))\
    $(patsubst %,-I %,$(INCL_BSP_DIRS_$(BUILD)))
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
# Commands for assembling .s files:
###################################
#
# The following variable contains the command for compiling a .s file using
# the qcc tool chain. Note that it uses recursive expansion.
#

#
# Create a make rule for generating an object file from assembly source
#
define MAKE_$(_build_)_ASSEMBLE_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(AS_$$(BUILD)) $$(AS_$$(BUILD)_FLAGS) -o $$@ $$<
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Assembling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
	$$(AS_$$(BUILD)) $$(AS_$$(BUILD)_FLAGS) -o $$@ $$<
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C files:
#################################
#
# The following variable contains the command for compiling a .c file using
# the qcc tool chain. Note that it uses recursive expansion.
#

###############################################################################
# NOTE: The sequence "-Wp,-MMD -Wp,$$(basename $$@).d -Wp,-MP " in the following
#       compiler invocations is a workaround for a bug in qcc. A correctly-
#       functioning qcc would accept "-MMD -MP". An alternative to this
#       workaround is to remove the entire option and separately invoke
#       gcc to generate the .d file. Commands to do this are as follows:
#
###	$$(GCC_$$(BUILD)_CMD) -o $$(basename $$@).tmp $$<
###	$$(CAT) $$(basename $$@).tmp |\
###            $$(SED) -e 's,\(.*\): ,$$(@D)/\1: ,g' >$$(basename $$@).d
###	$$(RM) -f ./-nostdinc $$(basename $$@).tmp
###############################################################################

#
# This is the command used to compile a C file into an object file. Note that
# it also generates the dependency (.d) file - all with one compiler invocation.
#
ifndef CC_CMD_$(_build_)
  define CC_CMD_$(_build_)
    $(if $(ENABLE_CMD_TIMING),$(eval cc_start_$@:=$$(shell date +%s.%N)),)\
    $(CC_$(BUILD)) $(CC_$(BUILD)_FLAGS) -Wp,-MMD -Wp,$(basename $@).d -Wp,-MP -o $@ $< &&\
    $(CAT) $(basename $@).d |\
      $(SED) -e 's,\(.*\): ,$(@D)/\1: ,g' >$(basename $@).tmp\
             -e 's,\(.\)\:\([^ ]\),/cygdrive/\L\1\2,g' >$(basename $@).tmp &&\
    $(MV) $(basename $@).tmp $(basename $@).d\
    $(if $(ENABLE_CMD_TIMING),\
         && echo "  Compiled $(notdir $<) for $(BUILD) in $$(echo `date +%s.%N`-$(cc_start_$@) | $(BC)) sec",)
  endef
endif

#
# Create a make rule for generating an object file from C source
#
define MAKE_$(_build_)_COMPILE_C_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(if $$(ENABLE_CMD_TIMING),,$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)")
ifdef ASM_MAKE
	@$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	@$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
	@$$(CC_CMD_$$(BUILD))
ifeq ($$(PP_MAKE),2)
	@$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifdef ASM_MAKE
	$$(CC_$$(BUILD)) -S $$(CC_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
	$$(CC_CMD_$$(BUILD))
ifeq ($$(PP_MAKE),2)
	$$(CC_$$(BUILD)) -E $$(CC_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for compiling C++ files:
###################################
#
# The following variable contains the command for compiling a .c file using
# the qcc tool chain. Note that it uses recursive expansion.
#

#
# This is the command used to compile a C++ file into an object file. Note that
# it also generates the dependency (.d) file - all with one compiler invocation.
#
ifndef CPP_CMD_$(_build_)
  define CPP_CMD_$(_build_)
    $(if $(ENABLE_CMD_TIMING),$(eval cpp_start_$@:=$$(shell date +%s.%N)),)\
    $(CPP_$(BUILD)) $(CPP_$(BUILD)_FLAGS) -Wp,-MMD -Wp,$(basename $@).d -Wp,-MP -o $@ $< &&\
    $(CAT) $(basename $@).d |\
      $(SED) -e 's,\(.*\): ,$(@D)/\1: ,g' >$(basename $@).tmp\
             -e 's,\(.\)\:\([^ ]\),/cygdrive/\L\1\2,g' >$(basename $@).tmp &&\
    $(MV) $(basename $@).tmp $(basename $@).d\
    $(if $(ENABLE_CMD_TIMING),\
         && echo "  Compiled $(notdir $<) for $(BUILD) in $$(echo `date +%s.%N`-$(cpp_start_$@) | $(BC)) sec",)
  endef
endif

#
# Create a make rule for generating an object file from C++ source
#
define MAKE_$(_build_)_COMPILE_CPP_RULE
$(1)
ifndef VERBOSE_MAKE
	@$$(if $$(ENABLE_CMD_TIMING),,$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)")
ifdef ASM_MAKE
	@$$(CPP_$$(BUILD)) -S $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
	@$$(CPP_CMD_$$(BUILD))
ifeq ($$(PP_MAKE),2)
	@$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
else
	@$$(ECHO) "###############################################################"
	@$$(ECHO) "  Compiling $$(notdir $$<) for $$(BUILD)"
	@$$(ECHO)
	@$$(ECHO) "  All prerequisites: $$+"
	@$$(ECHO)
	@$$(ECHO) "  Newer prerequisites: $$?"
	@$$(ECHO)
ifdef ASM_MAKE
	$$(CPP_$$(BUILD)) -S $$(CPP_$$(BUILD)_FLAGS) -o $$(basename $$@).s $$<
endif
ifeq ($$(PP_MAKE),1)
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
	$$(CPP_CMD_$$(BUILD))
ifeq ($$(PP_MAKE),2)
	$$(CPP_$$(BUILD)) -E $$(CPP_$$(BUILD)_FLAGS) $$< >$$(basename $$@).i
endif
	@$$(ECHO)
endif
endef

###############################################################################
# Commands for linking a library:
#################################

#
# The following command can be used to strip symbols from shared libraries
# or executables.
#
define Execute_Strip_Cmd
$(STRIP_$(BUILD)) $(STRIP_$(BUILD)_FLAGS) $@
endef

########################################
# Commands for linking a static library:
########################################

#
# This is the command used to create a static archive file from object files.
#
ifndef AR_CMD_$(_build_)
  define AR_CMD_$(_build_)
    $(if $(ENABLE_CMD_TIMING),$(eval ar_start_$@:=$$(shell date +%s.%N)),)\
    $(RM) -f $@ &&\
    $(AR_$(BUILD)) $(AR_$(BUILD)_FLAGS) $@ $(filter %.o,$^)\
    $(if $(ENABLE_CMD_TIMING),\
         && echo "  Linked $(notdir $@) for $(BUILD) in $$(echo `date +%s.%N`-$(ar_start_$@) | $(BC)) sec",)
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

########################################
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
    	$(addprefix -L,$(LIB_DIRS)) $(addprefix -L,$(LIB_BSP_DIRS_$(_build_))) -Wl,--start-group -Wl,-Bstatic $(archives_$@) -Wl,--end-group\
      	-Wl,-Bdynamic $(shared_$@) $(others_$@) $(NON_STD_LIBS) $(LD_$(BUILD)_GCOV_SO) \
    $(if $?, ,\
      $(if $(NO_STRIP),&& $(TOUCH) $@,$(call Execute_Strip_Cmd))\
      && @$(AR_$(BUILD)) $(AR_$(BUILD)_FLAGS) $(basename $@)S.a $(filter %.o,$^))\
    $(if $(ENABLE_CMD_TIMING),\
         && echo "  Linked $(notdir $@) for $(BUILD) in $$(echo `date +%s.%N`-$(so_start_$@) | $(BC)) sec",)
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
      $(addprefix -L,$(LIB_DIRS)) $(addprefix -L,$(LIB_BSP_DIRS_$(_build_))) -Wl,--start-group -Wl,-Bstatic $(archives_$@) -Wl,--end-group\
      -Wl,-Bdynamic $(shared_$@) $(others_$@) $(NON_STD_LIBS)\
	$(if $?, ,$(if $(USEMSG_ENABLED),$(if $(USEFILE),&& $(call Execute_USE_Cmd)))\
                                     $(if $(NO_STRIP),&& $(TOUCH) $@,$(call Execute_Strip_Cmd)))\
    $(if $(ENABLE_CMD_TIMING),\
         && echo "  Linked $(notdir $@) for $(BUILD) in $$(echo `date +%s.%N`-$(ld_start_$@) | $(BC)) sec",)
  endef
endif # LD_CMD_$(_build_)

#
# The following executes the "usemsg" command, which inserts a specially
# formatted usage message into an executable. The $(USEFILE) is the file
# containing the message. The "usemsg" command is followed by the
# $(TRUE) command so that an error in "usemsg" will not cause the
# build to terminate. Any error messages "usemsg" generates will appear
# on the console. Unfortunately, some "usemsg" errors do not generate
# an error message; e.g., if the $(USEFILE) does not contain the specially
# formatted error message, then "usemsg" "fails", but there is no
# information printed to the console.
#
define Execute_USE_Cmd
$(USEMSG_$(BUILD)) $(USEMSG_$(BUILD)_FLAGS) $@ $(USEFILE);$(TRUE)
endef

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
	@$$(ECHO)
endif
endef

###############################################################################
# MAKE_$(_build_)_COMBO_RULE
############################
# Creates a rule to combine IFSs and EFSs into a single image file.
#
# ARGUMENTS:
#   $(1) = File which is to contain the resulting combination
#   $(2) = List of files to go into the image (in proper order).
#
ifndef VERBOSE_MAKE
define MAKE_$(_build_)_COMBO_RULE
$(1):$(2)
	@$(ECHO) "  Combining '$$(notdir $$^)' for $$(BUILD)"
	@$(RM) -f $$@ &&\
	$$(MKIMAGE_$$(BUILD)) $$(MKIMAGE_$$(BUILD)_FLAGS) -o $$@ $$^ 2>$$($$(BUILD)_LST_OUTDIR)/$$(notdir $$@).txt
endef
else
define MAKE_$(_build_)_COMBO_RULE
$(1):$(2)
	@$(ECHO) "###############################################################"
	@$(ECHO) "  Combining '$$(notdir $$^)' for $$(BUILD)"
	@$(ECHO)
	$(RM) -f $$@ &&\
	$$(MKIMAGE_$$(BUILD)) $$(MKIMAGE_$$(BUILD)_FLAGS) -o $$@ $$^ 2>$$($$(BUILD)_LST_OUTDIR)/$$(notdir $$@).txt
	@$(ECHO)
endef
endif

###############################################################################
# MAKE_$(_build_)_EFS_RULE
##########################
# Creates a rule to generate an IFS.
#
# ARGUMENTS:
#   $(1) = File which is to contain the EFS result
#   $(2) = Build file that defines the EFS.
#   $(3) = Any other dependencies to add to the EFS's list (e.g., bin).
#
ifndef VERBOSE_MAKE
define MAKE_$(_build_)_EFS_RULE
$(1):$(2) $(3)
	@$(ECHO) "  Building EFS '$$(notdir $$@)' for $$(BUILD)"
	@$(RM) -f $$@ &&\
	$$(MKEFS_$$(BUILD)) $$(MKEFS_$$(BUILD)_FLAGS) $$< $$@ 2>$$($$(BUILD)_LST_OUTDIR)/$$(notdir $$@).txt
endef
else
define MAKE_$(_build_)_EFS_RULE
$(1):$(2) $(3)
	@$(ECHO) "###############################################################"
	@$(ECHO) "  Building EFS '$$(notdir $$@)' for $$(BUILD)"
	@$(ECHO)
	$(RM) -f $$@ &&\
	$$(MKEFS_$$(BUILD)) $$(MKEFS_$$(BUILD)_FLAGS) $$< $$@ 2>$$($$(BUILD)_LST_OUTDIR)/$$(notdir $$@).txt
	@$(ECHO)
endef
endif

###############################################################################
# MAKE_$(_build_)_IFS_RULE
##########################
# Creates a rule to generate an IFS.
#
# ARGUMENTS:
#   $(1) = File which is to contain the IFS result
#   $(2) = Build file that defines the IFS.
#   $(3) = Any other dependencies to add to the IFS's list (e.g., bin).
#
ifndef VERBOSE_MAKE
define MAKE_$(_build_)_IFS_RULE
$(1):$(2) $(3)
	@$(ECHO) "  Building IFS '$$(notdir $$@)' for $$(BUILD)"
	@export MKIFS_PATH="$$(MKIFS_$$(BUILD)_PATH)" &&\
	$(RM) -f $$@ &&\
	$$(MKIFS_$$(BUILD)) $$(MKIFS_$$(BUILD)_FLAGS) $$< $$@ 2>$$($$(BUILD)_LST_OUTDIR)/$$(notdir $$@).txt
endef
else
define MAKE_$(_build_)_IFS_RULE
$(1):$(2) $(3)
	@$(ECHO) "###############################################################"
	@$(ECHO) "  Building IFS '$$(notdir $$@)' for $$(BUILD)"
	@$(ECHO)
	export MKIFS_PATH="$$(MKIFS_$$(BUILD)_PATH)" &&\
	$(RM) -f $$@ &&\
	$$(MKIFS_$$(BUILD)) $$(MKIFS_$$(BUILD)_FLAGS) $$< $$@ 2>$$($$(BUILD)_LST_OUTDIR)/$$(notdir $$@).txt
	@$(ECHO)
endef
endif

###############################################################################
# MAKE_$(_build_)_PREPEND_RULE
##############################
# Creates a rule to prepend an IPL binary to a combined ifs/efs image
#
# ARGUMENTS:
#   $(1) = File which is to contain the result
#   $(2) = IPL binary file
#   $(3) = File containing IFS/EFS image
#
ifndef VERBOSE_MAKE
define MAKE_$(_build_)_PREPEND_RULE
$(1):$(2) $(3)
	@$(ECHO) "  Prepending IPL to create '$$(notdir $$@)' for $$(BUILD)"
	@$(RM) -f $$@ &&\
	$(CAT) $$^ >$$@
	@$(ECHO) "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";\
     $(ECHO) "  You will need to inspect or test the created image to ensure";\
     $(ECHO) "  it has been created sucessfully";\
     $(ECHO) "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
endef
else
define MAKE_$(_build_)_PREPEND_RULE
$(1):$(2) $(3)
	@$(ECHO) "###############################################################"
	@$(ECHO) "  Prepending IPL to create '$$(notdir $$@)' for $$(BUILD)"
	@$(ECHO)
	$(RM) -f $$@ &&\
	$(CAT) $$^ >$$@
	@$(ECHO) "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";\
     $(ECHO) "  You will need to inspect or test the created image to ensure";\
     $(ECHO) "  it has been created sucessfully";\
     $(ECHO) "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
endef
endif


#===========================================================================
# File Revision History (top to bottom: first revision to last revision)
#===========================================================================
#
# Date        Name      (Description on following lines: SCR #, etc.)
# ----------- --------
# 08-Sep-2014 Kirk Bailey
# + Deleted revision history prior to 2013.
# + Added lessons-learned from Linux builds to QNX build.
# + Added logic to allow individual build command times to be output.
#
# 12-Nov-2014 Kirk Bailey Rev 33
# + Switch from using ';' to '&&' for command lines with multiple commands
# + so that the return code reflects any failures.
#===========================================================================
