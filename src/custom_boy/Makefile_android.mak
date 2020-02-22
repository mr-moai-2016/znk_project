# Source directory
S = .
MAKE_CMD=$(ZNK_NDK_DIR)/prebuilt/windows/bin/make
# Root path of common libraries
MY_LIBS_ROOT=..
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -Wno-invalid-source-encoding \
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -Wno-invalid-source-encoding \
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -Wno-invalid-source-encoding \
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall -Wno-invalid-source-encoding --sysroot $(ZNK_NDK_DIR)/platforms/android-21/arch-arm64 \
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
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=custom_boy
EXE_FILE0=$O\custom_boy.cgi
OBJS0=\
	$O\CB_config.o \
	$O\CB_custom_automatic.o \
	$O\CB_custom_cookie.o \
	$O\CB_custom_feature.o \
	$O\CB_custom_postvars.o \
	$O\CB_fgp_info.o \
	$O\CB_finger.o \
	$O\CB_snippet.o \
	$O\CB_ua_conf.o \
	$O\CB_ua_info.o \
	$O\CB_vars_base.o \
	$O\CB_virtualizer.o \
	$O\CB_wgt_prim.o \
	$O\CB_wgt_prim_db.o \
	$O\main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libZnk-$(DL_VER).so \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libRano-$(DL_VER).so \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	@echo $(LINKER) -pie -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).so $(LINKER_OPT) -o $(EXE_FILE0)
	@     $(LINKER) -pie -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).so $(LINKER_OPT) -o $(EXE_FILE0)


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
$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA 
	@if exist "custom_boy.myf" @$(CP) /F "custom_boy.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\ $(CP_END)
	@if exist "rano_app.myf" @$(CP) /F "rano_app.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\ $(CP_END)
	@if exist "templates\*.html" @$(CP) /F "templates\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates\ $(CP_END)
	@if exist "publicbox\*" @$(CP) /F "publicbox\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox\ $(CP_END)
	@if exist "RE\*.png" @$(CP) /F "RE\*.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE\ $(CP_END)
	@if exist "RE\*.myf" @$(CP) /F "RE\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE\ $(CP_END)
	@if exist "UA\*.myf" @$(CP) /F "UA\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\ $(CP_END)

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
$O\CB_config.o: CB_config.h
$O\CB_custom_automatic.o: CB_custom_automatic.h
$O\CB_custom_cookie.o: CB_custom_cookie.h
$O\CB_custom_feature.o: CB_custom_feature.h CB_vars_base.h CB_finger.h
$O\CB_custom_postvars.o: CB_custom_postvars.h
$O\CB_fgp_info.o: CB_fgp_info.h CB_finger.h CB_wgt_prim.h CB_config.h
$O\CB_finger.o: CB_finger.h CB_snippet.h CB_wgt_prim.h CB_ua_conf.h
$O\CB_snippet.o: CB_snippet.h
$O\CB_ua_conf.o: CB_ua_conf.h CB_wgt_prim_db.h
$O\CB_ua_info.o: CB_ua_info.h
$O\CB_vars_base.o: CB_vars_base.h
$O\CB_virtualizer.o: CB_virtualizer.h CB_config.h CB_fgp_info.h CB_ua_conf.h CB_snippet.h CB_wgt_prim.h CB_finger.h CB_vars_base.h
$O\CB_wgt_prim.o: CB_wgt_prim.h
$O\CB_wgt_prim_db.o: CB_wgt_prim_db.h
$O\main.o: CB_virtualizer.h CB_fgp_info.h CB_ua_info.h CB_config.h CB_custom_automatic.h CB_custom_postvars.h CB_custom_feature.h CB_custom_cookie.h
