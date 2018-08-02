# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
!IFNDEF MKFSYS_DIR
MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
!ENDIF

!IF "$(MACHINE)" == "x64"
VC_MACHINE=AMD64
PLATFORM=win64
!ELSE
MACHINE=x86
VC_MACHINE=X86
PLATFORM=win32
!ENDIF
# Output directory
ABINAME=vc$(MACHINE)$(DEBUG)
O = .\out_dir\$(ABINAME)

!IF "$(DEBUG)" == "d"
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG 
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \


# We cannot use variables in include of nmake.
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

# Suffix rule.

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
	rmdir /S /Q $O\ 

# Src and Headers Dependency
