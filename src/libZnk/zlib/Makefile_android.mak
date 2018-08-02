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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall  \
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall  \
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall  \
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


include Makefile_version.mak

BASENAME0=zlib
SLIB_FILE0=$O\libzlib.a
OBJS0=\
	$O\adler32.o \
	$O\compress.o \
	$O\crc32.o \
	$O\deflate.o \
	$O\gzio.o \
	$O\infback.o \
	$O\inffast.o \
	$O\inflate.o \
	$O\inftrees.o \
	$O\trees.o \
	$O\uncompr.o \
	$O\zutil.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

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
	if exist $(SLIB_FILE0) del $(SLIB_FILE0)
	@echo $(LIBAR) crsD $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     $(LIBAR) crsD $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)


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
	rmdir /S /Q $O\ 

# Src and Headers Dependency
adler32.o: zlib.h
compress.o: zlib.h
crc32.o: zutil.h crc32.h
deflate.o: deflate.h
gzio.o: zutil.h
infback.o: zutil.h inftrees.h inflate.h inffast.h inffixed.h
inffast.o: zutil.h inftrees.h inflate.h inffast.h
inflate.o: zutil.h inftrees.h inflate.h inffast.h inffixed.h
inftrees.o: zutil.h inftrees.h
trees.o: deflate.h trees.h
uncompr.o: zlib.h
zutil.o: zutil.h
