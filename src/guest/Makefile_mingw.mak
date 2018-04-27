# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
endif

ifeq ($(MACHINE), x64)
  PLATFORM=win64
else
  MACHINE=x86
  PLATFORM=win32
endif
# Output directory
ABINAME=mingw$(MACHINE)$(DEBUG)
O = .\out_dir\$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=gcc -Wall -Wstrict-aliasing=2 -g
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)_mingwd
SLIBS_DIR=slib\$(PLATFORM)_mingwd
else
COMPILER=gcc -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
endif

CP=xcopy /H /C /Y

INCLUDE_FLAG+=  \


include Makefile_version.mak

SUB_LIBS=\

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
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\protected\futaba_bbs @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\protected\futaba_bbs 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\protected\futaba_up @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\protected\futaba_up 
	@if exist "futaba_bbs\*" @$(CP) /F "futaba_bbs\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\protected\futaba_bbs\ $(CP_END)
	@if exist "futaba_up\*" @$(CP) /F "futaba_up\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\protected\futaba_up\ $(CP_END)

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
	del /Q $O\ 

# Src and Headers Dependency
