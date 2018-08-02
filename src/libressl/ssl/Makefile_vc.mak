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
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM
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

BASENAME0=ssl
DLIB_NAME0=libssl-45.dll
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\libssl-45.imp.lib
SLIB_FILE0=$O\libssl.lib
OBJS0=\
	$O\t1_enc.obj \
	$O\ssl_init.obj \
	$O\ssl_srvr.obj \
	$O\d1_pkt.obj \
	$O\ssl_versions.obj \
	$O\t1_hash.obj \
	$O\t1_srvr.obj \
	$O\ssl_txt.obj \
	$O\s3_cbc.obj \
	$O\ssl_both.obj \
	$O\t1_meth.obj \
	$O\ssl_cert.obj \
	$O\d1_both.obj \
	$O\ssl_err.obj \
	$O\ssl_rsa.obj \
	$O\ssl_packet.obj \
	$O\ssl_algs.obj \
	$O\bs_ber.obj \
	$O\d1_srvr.obj \
	$O\ssl_lib.obj \
	$O\ssl_clnt.obj \
	$O\ssl_asn1.obj \
	$O\bs_cbs.obj \
	$O\d1_lib.obj \
	$O\ssl_pkt.obj \
	$O\d1_meth.obj \
	$O\ssl_ciph.obj \
	$O\ssl_stat.obj \
	$O\d1_enc.obj \
	$O\pqueue.obj \
	$O\d1_srtp.obj \
	$O\t1_clnt.obj \
	$O\bs_cbb.obj \
	$O\bio_ssl.obj \
	$O\s3_lib.obj \
	$O\ssl_tlsext.obj \
	$O\ssl_sess.obj \
	$O\d1_clnt.obj \
	$O\t1_lib.obj \
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
all: $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
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
	@echo $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.lib ws2_32.lib advapi32.lib  /DEF:gsl.def
	@     $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.lib ws2_32.lib advapi32.lib  /DEF:gsl.def


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
t1_enc.obj: ssl_locl.h
ssl_init.obj: ssl_locl.h
ssl_srvr.obj: ssl_locl.h bytestring.h ssl_tlsext.h
d1_pkt.obj: ssl_locl.h bytestring.h
ssl_versions.obj: ssl_locl.h
t1_hash.obj: ssl_locl.h
t1_srvr.obj: ssl_locl.h
ssl_txt.obj: ssl_locl.h
s3_cbc.obj: ssl_locl.h
ssl_both.obj: ssl_locl.h bytestring.h
t1_meth.obj: ssl_locl.h
ssl_cert.obj: ssl_locl.h
d1_both.obj: ssl_locl.h bytestring.h
ssl_err.obj: ssl_locl.h
ssl_rsa.obj: ssl_locl.h
ssl_packet.obj: ssl_locl.h bytestring.h
ssl_algs.obj: ssl_locl.h
bs_ber.obj: bytestring.h
d1_srvr.obj: ssl_locl.h
ssl_lib.obj: ssl_locl.h bytestring.h
ssl_clnt.obj: ssl_locl.h bytestring.h ssl_tlsext.h
ssl_asn1.obj: ssl_locl.h bytestring.h
bs_cbs.obj: bytestring.h
d1_lib.obj: ssl_locl.h
ssl_pkt.obj: ssl_locl.h bytestring.h
d1_meth.obj: ssl_locl.h
ssl_ciph.obj: ssl_locl.h
ssl_stat.obj: ssl_locl.h
d1_enc.obj: ssl_locl.h
d1_srtp.obj: ssl_locl.h bytestring.h srtp.h
t1_clnt.obj: ssl_locl.h
bs_cbb.obj: bytestring.h
bio_ssl.obj: ssl_locl.h
s3_lib.obj: ssl_locl.h bytestring.h
ssl_tlsext.obj: ssl_locl.h bytestring.h ssl_tlsext.h
ssl_sess.obj: ssl_locl.h
d1_clnt.obj: ssl_locl.h bytestring.h
t1_lib.obj: ssl_locl.h bytestring.h ssl_tlsext.h
