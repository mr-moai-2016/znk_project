# Source directory
S = .
MAKE_CMD=$(ZNK_NDK_DIR)/prebuilt/windows/bin/make
# Root path of common libraries
MY_LIBS_ROOT=$[my_libs_root]$
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

# Android 4.0
PLATFORMS_LEVEL4=$(findstring $(ZNK_NDK_DIR)/platforms/android-9, $(PLATFORMS_LIST))
ifndef PLATFORMS_LEVEL4
  PLATFORMS_LEVEL4=$(word 1, $(PLATFORMS_LIST))
endif

# Android 5.0 later
PLATFORMS_LEVEL5=$(findstring $(ZNK_NDK_DIR)/platforms/android-21, $(PLATFORMS_LIST))
ifndef PLATFORMS_LEVEL5
  PLATFORMS_LEVEL5=$(word 1, $(PLATFORMS_LIST))
endif

ifeq ($(MACHINE), armeabi)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/arm-linux-androideabi-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )
PLATFORMS_LEVEL=$(PLATFORMS_LEVEL4)

COMPILER := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc \
	-MMD -MP \
	-fpic -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -march=armv5te -mtune=xscale -msoft-float -mthumb -Os \
	-g -DNDEBUG -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 \
	-DANDROID \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall $[compiler_option_special]$ \
	-I$(PLATFORMS_LEVEL)/arch-arm/usr/include

LINKER   := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-arm \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-arm/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -mthumb -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc-ar
endif

ifeq ($(MACHINE), armeabi-v7a)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/arm-linux-androideabi-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )
PLATFORMS_LEVEL=$(PLATFORMS_LEVEL4)

COMPILER := $(TOOLCHAINS_DIR)/bin/arm-linux-androideabi-gcc \
	-MMD -MP \
	-fpic -ffunction-sections -funwind-tables -fstack-protector -no-canonical-prefixes -march=armv7-a -mfpu=vfpv3-d16 -mfloat-abi=softfp -mthumb -Os \
	-g -DNDEBUG -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 \
	-DANDROID -fPIE \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall $[compiler_option_special]$ \
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
PLATFORMS_LEVEL=$(PLATFORMS_LEVEL4)

COMPILER := $(TOOLCHAINS_DIR)/bin/i686-linux-android-gcc \
	-MMD -MP \
	-ffunction-sections -funwind-tables -no-canonical-prefixes -fstack-protector -O2 \
	-g -DNDEBUG -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 \
	-DANDROID -fPIE \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall $[compiler_option_special]$ \
	-I$(PLATFORMS_LEVEL)/arch-x86/usr/include \

LINKER   := $(TOOLCHAINS_DIR)/bin/i686-linux-android-g++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-x86 \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-x86/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes  -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now -fPIE  -lc -lm
STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/i686-linux-android-strip --strip-unneeded
LIBAR := $(TOOLCHAINS_DIR)/bin/i686-linux-android-gcc-ar
endif

ifeq ($(MACHINE), arm64-v8a)
TOOLCHAINS_LIST=$(wildcard $(ZNK_NDK_DIR)/toolchains/aarch64-linux-android-*/prebuilt/windows)
TOOLCHAINS_DIR=$(word $(words $(TOOLCHAINS_LIST)), $(TOOLCHAINS_LIST) )
PLATFORMS_LEVEL=$(PLATFORMS_LEVEL5)

COMPILER := $(ZNK_NDK_DIR)/toolchains/llvm/prebuilt/windows/bin/clang \
	-gcc-toolchain $(TOOLCHAINS_DIR) -target aarch64-none-linux-android \
	-fPIC -ffunction-sections -funwind-tables -fstack-protector-strong -Wno-invalid-command-line-argument -Wno-unused-command-line-argument -no-canonical-prefixes \
	-g -O2 -DNDEBUG \
	-DANDROID -fPIE \
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall $[compiler_option_special]$ --sysroot $(ZNK_NDK_DIR)/platforms/android-21/arch-arm64 \
	-I$(PLATFORMS_LEVEL)/arch-arm64/usr/include \

LINKER   := $(ZNK_NDK_DIR)/toolchains/llvm/prebuilt/windows/bin/clang++ \
	--sysroot=$(PLATFORMS_LEVEL)/arch-arm64 \
	-gcc-toolchain $(TOOLCHAINS_DIR) \

RPATH_LINK := -Wl,-rpath-link=$(PLATFORMS_LEVEL)/arch-arm64/usr/lib -Wl,-rpath-link=$O
LINKER_OPT := -no-canonical-prefixes -target aarch64-none-linux-android \
	-Wl,--build-id -Wl,--no-undefined -Wl,-z,noexecstack -Wl,-z,relro -Wl,-z,now \
	-Wl,--warn-shared-textrel -Wl,--fatal-warnings -fPIE \
	-lc -lm

STRIP_UNNEEDED := $(TOOLCHAINS_DIR)/bin/aarch64-linux-android-strip --strip-unneeded 
LIBAR := $(TOOLCHAINS_DIR)/bin/aarch64-linux-android-gcc-ar
endif


CP=xcopy /H /C /Y
INCLUDE_FLAG+=  \
$[include_flags]$

include Makefile_version.mak

$[objs_defs]$

# Entry rule.
$[all_target_rule]$
# Mkdir rule.
$[mkdir_rule]$
# Product files rule.
$[product_files_rule]$
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
#   (Because '%' is wildcard and it indicates patical path 'mydir\filename_base' )
#
$[suffix_rule]$
# Submkf rule.
$[submkf_rule]$
# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
$[install_data_rule]$
# Install exec rule.
$[install_exec_rule]$
# Install dlib rule.
$[install_dlib_rule]$
# Install slib rule.
$[install_slib_rule]$
# Install rule.
$[install_rule]$
# Clean rule.
$[clean_rule]$
