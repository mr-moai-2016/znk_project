# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..\..
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
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	/wd4996
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	/wd4996
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \
	-I../include \
	-I../include/compat \
	-I../crypto/asn1 \
	-I../crypto/bn \
	-I../crypto/evp \
	-I../crypto/modes \
	-I../crypto \


# We cannot use variables in include of nmake.
include Makefile_version.mak

BASENAME0=tls
DLIB_NAME0=libtls-17.dll
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\libtls-17.imp.lib
SLIB_FILE0=$O\libtls.lib
OBJS0=\
	$O\tls_conninfo.obj \
	$O\tls_verify.obj \
	$O\tls_bio_cb.obj \
	$O\tls_config.obj \
	$O\tls_keypair.obj \
	$O\tls_server.obj \
	$O\tls_peer.obj \
	$O\compat/pwrite.obj \
	$O\compat/getuid.obj \
	$O\compat/pread.obj \
	$O\compat/ftruncate.obj \
	$O\tls_client.obj \
	$O\tls_ocsp.obj \
	$O\tls.obj \
	$O\tls_util.obj \
	$O\dll_main.obj \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE0) \

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O\compat $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O\compat:
	if not exist $O\compat mkdir $O\compat

$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	@echo LIB /NOLOGO /OUT:$(SLIB_FILE0) {[objs]} $(SUB_LIBS)
	@     LIB /NOLOGO /OUT:$(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0) gsl.def
	@echo $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libssl.lib $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.lib ws2_32.lib advapi32.lib  /DEF:gsl.def
	@     $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libssl.lib $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.lib ws2_32.lib advapi32.lib  /DEF:gsl.def


# Suffix rule.
{$S\compat}.c{$O\compat}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S\compat}.cpp{$O\compat}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

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
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(DLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(DLIBS_DIR) 
	@if exist "$(DLIB_FILE0)" @$(CP) /F "$(DLIB_FILE0)" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install slib rule.
install_slib: $(SLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(SLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(SLIBS_DIR) 
	@if exist "$(SLIB_FILE0)" @$(CP) /F "$(SLIB_FILE0)" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install rule.
install: all install_slib install_dlib 


# Clean rule.
clean:
	rmdir /S /Q $O\ 

# Src and Headers Dependency
tls_conninfo.obj: tls_internal.h
tls_verify.obj: tls_internal.h
tls_bio_cb.obj: tls_internal.h
tls_config.obj: tls_internal.h
tls_keypair.obj: tls_internal.h
tls_server.obj: tls_internal.h
tls_peer.obj: tls_internal.h
tls_client.obj: tls_internal.h
tls_ocsp.obj: tls_internal.h
tls.obj: tls_internal.h
tls_util.obj: tls_internal.h
