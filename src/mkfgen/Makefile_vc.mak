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
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 /wd4819
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG /wd4819
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \
	-I$(MY_LIBS_ROOT)/libZnk \


# We cannot use variables in include of nmake.
include Makefile_version.mak

BASENAME0=mkfgen
EXE_FILE0=$O\mkfgen.exe
OBJS0=\
	$O\Mkf_android.obj \
	$O\Mkf_include.obj \
	$O\Mkf_install.obj \
	$O\Mkf_lib_depend.obj \
	$O\Mkf_seek.obj \
	$O\Mkf_src_depend.obj \
	$O\Mkf_sset.obj \
	$O\mkfgen.obj \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	@echo $(LINKER) /OUT:$(EXE_FILE0)  {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk.lib ws2_32.lib 
	@     $(LINKER) /OUT:$(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk.lib ws2_32.lib 


# Suffix rule.
{$S}.c{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.cpp{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	@if not exist ..\..\mkfsys\template @mkdir ..\..\mkfsys\template 
	@if exist "template\*.mak" @$(CP) /F "template\*.mak" ..\..\mkfsys\template\ $(CP_END)
	@if exist "template\*.mk" @$(CP) /F "template\*.mk" ..\..\mkfsys\template\ $(CP_END)
	@if exist "template\*.myf" @$(CP) /F "template\*.myf" ..\..\mkfsys\template\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\mkfsys\$(PLATFORM) @mkdir ..\..\mkfsys\$(PLATFORM) 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\mkfsys\$(PLATFORM)\ $(CP_END)
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" ..\..\mkfsys\$(PLATFORM)\ $(CP_END)

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rmdir /S /Q $O\ 

# Src and Headers Dependency
$O\mkfgen.obj: Mkf_install.h Mkf_include.h Mkf_seek.h Mkf_src_depend.h Mkf_lib_depend.h Mkf_android.h Mkf_sset.h
$O\Mkf_android.obj: Mkf_android.h Mkf_seek.h Mkf_lib_depend.h Mkf_include.h
$O\Mkf_include.obj: Mkf_include.h
$O\Mkf_install.obj: Mkf_install.h
$O\Mkf_lib_depend.obj: Mkf_lib_depend.h
$O\Mkf_seek.obj: Mkf_seek.h
$O\Mkf_src_depend.obj: Mkf_src_depend.h Mkf_seek.h
$O\Mkf_sset.obj: Mkf_sset.h
