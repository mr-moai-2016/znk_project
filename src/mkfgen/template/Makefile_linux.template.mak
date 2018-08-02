# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=$[my_libs_root]$
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

ifndef MACHINE
  UNAME_M=$(shell uname -m)
  ifeq ($(UNAME_M), i386)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), i686)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), x86_64)
    MACHINE=x64
  endif
  ifeq ($(UNAME_M), amd64)
    MACHINE=x64
  endif
endif
GCC_CMD=gcc
PLATFORM=linux
ifeq ($(MACHINE), x64)
  GCC_CMD=gcc -m64
  PLATFORM=linux64
endif
ifeq ($(MACHINE), x86)
  GCC_CMD=gcc -m32
  PLATFORM=linux32
endif
# Output directory
ABINAME = linux$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g $[compiler_option_special]$
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)d
SLIBS_DIR=slib/$(PLATFORM)d
else
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG $[compiler_option_special]$
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)
SLIBS_DIR=slib/$(PLATFORM)
endif

CP=cp

INCLUDE_FLAG+=  \
$[include_flags]$

include Makefile_version.mak

$[objs_defs]$

# Entry rule.
$[all_target_rule]$
# Mkdir rule.
$[mkdir_rule]$
# Product files rule.
$[product_files_rule]$
##
# Pattern rule.
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\',
# for example $O\\%.o: $S\%.c .
#
$[suffix_rule]$
# Rc rule.
$[rc_rule]$
# Submkf rule.
$[submkf_rule]$
# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
$[install_data_rule]$
# Install exec rule.
$[install_exec_rule]$
# Install dlib rule.
$[install_dlib_rule]$
# Install slib rule.
$[install_slib_rule]$
# Install rule.
$[install_rule]$
# Clean rule.
$[clean_rule]$
