# Source directory
S = .
MAKE_CMD=$(ZNK_NDK_DIR)/prebuilt/windows/bin/make
# Root path of common libraries
MY_LIBS_ROOT=..\..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
endif

ifndef MACHINE
  MACHINE=armeabi
endif
PLATFORM := android-$(MACHINE)

# Output directory
ABINAME := android-$(MACHINE)
O = .\out_dir\$(PLATFORM)
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)

PLATFORMS_LIST=$(wildcard $(ZNK_NDK_DIR)/platforms/android-*)
PLATFORMS_LEVEL=$(findstring $(ZNK_NDK_DIR)/platforms/android-9, $(PLATFORMS_LIST))
ifndef PLATFORMS_LEVEL
  PLATFORMS_LEVEL=$(word 1, $(PLATFORMS_LIST))
endif

ifeq ($(MACHINE), armeabi)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/arm-linux-androideabi-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )

COMPILER := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc \
	-MMD -MP \
	-fpic -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -march=armv5te -mtune=xscale -msoft-float -mthumb -Os \
	-g -DNDEBUG -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 \
	-DANDROID \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	-I$(PLATFORMS_LEVEL)/arch-arm/usr/include

LINKER   := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-arm \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-arm/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -mthumb -fPIE  -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc-ar

endif
ifeq ($(MACHINE), armeabi-v7a)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/arm-linux-androideabi-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )

COMPILER := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc \
	-MMD -MP \
	-fpic -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -mthumb -Os \
	-g -DNDEBUG -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 \
	-DANDROID \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	-I$(PLATFORMS_LEVEL)/arch-arm/usr/include \

LINKER   := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-arm \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-arm/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes -march=armv7-a -Wl,--fix-cortex-a8  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -mthumb -fPIE  -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc-ar

endif
ifeq ($(MACHINE), x86)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/x86-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )

COMPILER := $(TOOLCHAINS_DIR)/bin/i686-linux-android-gcc \
	-MMD -MP \
	-ffunction-sections -funwind-tables -no-canonical-prefixes -fstack-protector -O2 \
	-g -DNDEBUG -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 \
	-DANDROID \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -DLIBRESSL_INTERNAL -D__BEGIN_HIDDEN_DECLS= -D__END_HIDDEN_DECLS= \
	-DOPENSSL_NO_HW_PADLOCK -DOPENSSL_NO_ASM \
	-I$(PLATFORMS_LEVEL)/arch-x86/usr/include \

LINKER   := $(TOOLCHAINS_DIR)/bin/i686-linux-android-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-x86 \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-x86/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -fPIE  -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/i686-linux-android-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/i686-linux-android-gcc-ar

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

BASENAME0=ssl
DLIB_NAME0=libssl-45.so
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\libssl-45.so
SLIB_FILE0=$O\libssl.a
OBJS0=\
	$O\t1_enc.o \
	$O\ssl_init.o \
	$O\ssl_srvr.o \
	$O\d1_pkt.o \
	$O\ssl_versions.o \
	$O\t1_hash.o \
	$O\t1_srvr.o \
	$O\ssl_txt.o \
	$O\s3_cbc.o \
	$O\ssl_both.o \
	$O\t1_meth.o \
	$O\ssl_cert.o \
	$O\d1_both.o \
	$O\ssl_err.o \
	$O\ssl_rsa.o \
	$O\ssl_packet.o \
	$O\ssl_algs.o \
	$O\bs_ber.o \
	$O\d1_srvr.o \
	$O\ssl_lib.o \
	$O\ssl_clnt.o \
	$O\ssl_asn1.o \
	$O\bs_cbs.o \
	$O\d1_lib.o \
	$O\ssl_pkt.o \
	$O\d1_meth.o \
	$O\ssl_ciph.o \
	$O\ssl_stat.o \
	$O\d1_enc.o \
	$O\pqueue.o \
	$O\d1_srtp.o \
	$O\t1_clnt.o \
	$O\bs_cbb.o \
	$O\bio_ssl.o \
	$O\s3_lib.o \
	$O\ssl_tlsext.o \
	$O\ssl_sess.o \
	$O\d1_clnt.o \
	$O\t1_lib.o \
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
all: $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if exist $(SLIB_FILE0) del $(SLIB_FILE0)
	@echo $(LIBAR) crsD $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     $(LIBAR) crsD $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0)
	if exist $(DLIB_FILE0) del $(DLIB_FILE0)
	@echo $(LINKER) -shared -Wl,-soname,$(DLIB_NAME0) \
	-lgcc {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a $(LINKER_OPT) -o $(DLIB_FILE0)
	@     $(LINKER) -shared -Wl,-soname,$(DLIB_NAME0) \
	-lgcc $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/$(SLIBS_DIR)/libcrypto.a $(LINKER_OPT) -o $(DLIB_FILE0)
	$(STRIP_UNNEEDED) $(DLIB_FILE0)


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
$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


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
t1_enc.o: ssl_locl.h
ssl_init.o: ssl_locl.h
ssl_srvr.o: ssl_locl.h bytestring.h ssl_tlsext.h
d1_pkt.o: ssl_locl.h bytestring.h
ssl_versions.o: ssl_locl.h
t1_hash.o: ssl_locl.h
t1_srvr.o: ssl_locl.h
ssl_txt.o: ssl_locl.h
s3_cbc.o: ssl_locl.h
ssl_both.o: ssl_locl.h bytestring.h
t1_meth.o: ssl_locl.h
ssl_cert.o: ssl_locl.h
d1_both.o: ssl_locl.h bytestring.h
ssl_err.o: ssl_locl.h
ssl_rsa.o: ssl_locl.h
ssl_packet.o: ssl_locl.h bytestring.h
ssl_algs.o: ssl_locl.h
bs_ber.o: bytestring.h
d1_srvr.o: ssl_locl.h
ssl_lib.o: ssl_locl.h bytestring.h
ssl_clnt.o: ssl_locl.h bytestring.h ssl_tlsext.h
ssl_asn1.o: ssl_locl.h bytestring.h
bs_cbs.o: bytestring.h
d1_lib.o: ssl_locl.h
ssl_pkt.o: ssl_locl.h bytestring.h
d1_meth.o: ssl_locl.h
ssl_ciph.o: ssl_locl.h
ssl_stat.o: ssl_locl.h
d1_enc.o: ssl_locl.h
d1_srtp.o: ssl_locl.h bytestring.h srtp.h
t1_clnt.o: ssl_locl.h
bs_cbb.o: bytestring.h
bio_ssl.o: ssl_locl.h
s3_lib.o: ssl_locl.h bytestring.h
ssl_tlsext.o: ssl_locl.h bytestring.h ssl_tlsext.h
ssl_sess.o: ssl_locl.h
d1_clnt.o: ssl_locl.h bytestring.h
t1_lib.o: ssl_locl.h bytestring.h ssl_tlsext.h
