# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
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


include Makefile_version.mak

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: 

# Mkdir rule.

# Product files rule.

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

# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/protected/futaba_bbs 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/protected/futaba_up 
	for tgt in futaba_bbs/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/protected/futaba_bbs/ ; fi ; done
	for tgt in futaba_up/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/protected/futaba_up/ ; fi ; done

# Install exec rule.
install_exec:

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_data 


# Clean rule.
clean:
	rm -rf $O/ 

# Src and Headers Dependency
