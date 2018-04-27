# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=$[my_libs_root]$
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

GCC_CMD=gcc
PLATFORM=win32
ifeq ($(MACHINE), x64)
  PLATFORM=win64
else
  MACHINE=x86
  PLATFORM=win32
endif
# Output directory
ABINAME=mingw$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)_mingwd
SLIBS_DIR=slib/$(PLATFORM)_mingwd
else
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG
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
