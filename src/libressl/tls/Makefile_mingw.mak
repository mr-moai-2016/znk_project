# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..\..
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
COMPILER=gcc -Wall -Wstrict-aliasing=2 -g -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)_mingwd
SLIBS_DIR=slib\$(PLATFORM)_mingwd
else
COMPILER=gcc -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
endif

CP=xcopy /H /C /Y

INCLUDE_FLAG+=  \
	-I../include \
	-I../include/compat \
	-I../crypto/asn1 \
	-I../crypto/bn \
	-I../crypto/evp \
	-I../crypto/modes \
	-I../crypto \


include Makefile_version.mak

BASENAME0=tls
DLIB_NAME0=libtls-17.dll
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\libtls-17.dll.a
SLIB_FILE0=$O\libtls.a
OBJS0=\
	$O\compat\ftruncate.o \
	$O\compat\getuid.o \
	$O\compat\pread.o \
	$O\compat\pwrite.o \
	$O\tls.o \
	$O\tls_bio_cb.o \
	$O\tls_client.o \
	$O\tls_config.o \
	$O\tls_conninfo.o \
	$O\tls_keypair.o \
	$O\tls_ocsp.o \
	$O\tls_peer.o \
	$O\tls_server.o \
	$O\tls_util.o \
	$O\tls_verify.o \
	$O\dll_main.o \

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
	if exist $(SLIB_FILE0) del $(SLIB_FILE0)
	@echo ar cru $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)
	ranlib $(SLIB_FILE0)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0) gsl.def
	@echo gcc -static-libgcc -g -Wl,--disable-stdcall-fixup,--kill-at -shared -o $(DLIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libssl.a $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a -lws2_32  gsl.def
	@     gcc -static-libgcc -g -Wl,--disable-stdcall-fixup,--kill-at -shared -o $(DLIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libssl.a $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a -lws2_32  gsl.def
	dlltool --kill-at --dllname $(DLIB_FILE0) -d gsl.def -l $(ILIB_FILE0)


##
# Pattern rule.
#
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\'.
# Note that we have to duplicate '\' only before special charactor(% etc) in the left of ':'.
#
# For example 1 :
#   $O\\mydir\\%.o: $S\%.c        .... NG
#   $O\mydir\\%.o:  $S\%.c        .... OK
# For example 2 :
#   $O\\mydir\%.o:  $S\mydir\%.c  .... NG
#   $O\mydir\\%.o:  $S\mydir\%.c  .... OK
# In the case of example 2, we can write more simply :
#   $O\\%.o: $S\%.c               .... OK
#   (Because '%' is wildcard and it indicates patical path 'mydir\filename_base' recursively )
#
$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


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
$O\tls.o: tls_internal.h
$O\tls_bio_cb.o: tls_internal.h
$O\tls_client.o: tls_internal.h
$O\tls_config.o: tls_internal.h
$O\tls_conninfo.o: tls_internal.h
$O\tls_keypair.o: tls_internal.h
$O\tls_ocsp.o: tls_internal.h
$O\tls_peer.o: tls_internal.h
$O\tls_server.o: tls_internal.h
$O\tls_util.o: tls_internal.h
$O\tls_verify.o: tls_internal.h
