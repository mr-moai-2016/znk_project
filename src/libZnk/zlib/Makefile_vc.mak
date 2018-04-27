# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=../..
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
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 /wd4127 /wd4996 /wd4267 /wd4131 /wd4244
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG /wd4127 /wd4996 /wd4267 /wd4131 /wd4244
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \


# We cannot use variables in include of nmake.
include Makefile_version.mak

BASENAME0=zlib
SLIB_FILE0=$O\zlib.lib
OBJS0=\
	$O\adler32.obj \
	$O\compress.obj \
	$O\crc32.obj \
	$O\deflate.obj \
	$O\gzio.obj \
	$O\infback.obj \
	$O\inffast.obj \
	$O\inflate.obj \
	$O\inftrees.obj \
	$O\trees.obj \
	$O\uncompr.obj \
	$O\zutil.obj \

SUB_LIBS=\

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O $(SLIB_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	LIB /NOLOGO /OUT:$(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)


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

# Install exec rule.
install_exec:
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib: $(SLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(SLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(SLIBS_DIR) 
	@if exist "$(SLIB_FILE0)" @$(CP) /F "$(SLIB_FILE0)" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install rule.
install: all install_slib 


# Clean rule.
clean:
	del /Q $O\ 

# Src and Headers Dependency
adler32.obj: zlib.h
compress.obj: zlib.h
crc32.obj: zutil.h crc32.h
deflate.obj: deflate.h
gzio.obj: zutil.h
infback.obj: zutil.h inftrees.h inflate.h inffast.h inffixed.h
inffast.obj: zutil.h inftrees.h inflate.h inffast.h
inflate.obj: zutil.h inftrees.h inflate.h inffast.h inffixed.h
inftrees.obj: zutil.h inftrees.h
trees.obj: deflate.h trees.h
uncompr.obj: zlib.h
zutil.obj: zutil.h
