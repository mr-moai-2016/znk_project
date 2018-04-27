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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall \
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall \
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
	-Wa,--noexecstack -Wformat -Werror=format-security -Wall \
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
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=hello
EXE_FILE0=$O\hello.cgi
OBJS0=\
	$O\cgi_util.o \
	$O\hello.o \

BASENAME1=evar1
EXE_FILE1=$O\evar1.cgi
OBJS1=\
	$O\cgi_util.o \
	$O\evar1.o \

BASENAME2=evar2
EXE_FILE2=$O\evar2.cgi
OBJS2=\
	$O\cgi_util.o \
	$O\evar2.o \

BASENAME3=query_string1
EXE_FILE3=$O\query_string1.cgi
OBJS3=\
	$O\cgi_util.o \
	$O\query_string1.o \

BASENAME4=query_string2
EXE_FILE4=$O\query_string2.cgi
OBJS4=\
	$O\cgi_util.o \
	$O\query_string2.o \

BASENAME5=post1
EXE_FILE5=$O\post1.cgi
OBJS5=\
	$O\cgi_util.o \
	$O\post1.o \

BASENAME6=post2
EXE_FILE6=$O\post2.cgi
OBJS6=\
	$O\cgi_util.o \
	$O\post2.o \

BASENAME7=transfer_chunked
EXE_FILE7=$O\transfer_chunked.cgi
OBJS7=\
	$O\cgi_util.o \
	$O\transfer_chunked.o \

BASENAME8=hello_template
EXE_FILE8=$O\hello_template.cgi
OBJS8=\
	$O\cgi_util.o \
	$O\hello_template.o \

BASENAME9=progress
EXE_FILE9=$O\progress.cgi
OBJS9=\
	$O\cgi_util.o \
	$O\progress.o \

SUB_LIBS=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE1) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE2) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE3) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE4) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE5) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE6) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE7) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE8) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE9) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libZnk.so \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libRano.so \



# Entry rule.
all: $O $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(EXE_FILE4) $(EXE_FILE5) $(EXE_FILE6) $(EXE_FILE7) $(EXE_FILE8) $(EXE_FILE9) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS0) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE0)

$(EXE_FILE1): $(OBJS1)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS1) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE1)

$(EXE_FILE2): $(OBJS2)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS2) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE2)

$(EXE_FILE3): $(OBJS3)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS3) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE3)

$(EXE_FILE4): $(OBJS4)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS4) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE4)

$(EXE_FILE5): $(OBJS5)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS5) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE5)

$(EXE_FILE6): $(OBJS6)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS6) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE6)

$(EXE_FILE7): $(OBJS7)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS7) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE7)

$(EXE_FILE8): $(OBJS8)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS8) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE8)

$(EXE_FILE9): $(OBJS9)
	$(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc \
	$(OBJS9) $(SUB_LIBS) \
	-Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.so $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.so \
	$(LINKER_OPT) \
	-o $(EXE_FILE9)


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
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox 
	@if exist "*.c" @$(CP) /F "*.c" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)
	@if exist "*.cpp" @$(CP) /F "*.cpp" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)
	@if exist "*.html" @$(CP) /F "*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)
	@if exist "*.js" @$(CP) /F "*.js" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(EXE_FILE4) $(EXE_FILE5) $(EXE_FILE6) $(EXE_FILE7) $(EXE_FILE8) $(EXE_FILE9)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE1)" @$(CP) /F "$(EXE_FILE1)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE2)" @$(CP) /F "$(EXE_FILE2)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE3)" @$(CP) /F "$(EXE_FILE3)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE4)" @$(CP) /F "$(EXE_FILE4)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE5)" @$(CP) /F "$(EXE_FILE5)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE6)" @$(CP) /F "$(EXE_FILE6)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE7)" @$(CP) /F "$(EXE_FILE7)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE8)" @$(CP) /F "$(EXE_FILE8)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE9)" @$(CP) /F "$(EXE_FILE9)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	del /Q $O\ 

# Src and Headers Dependency
cgi_util.o: cgi_util.h
evar1.o: cgi_util.h
evar2.o:
hello.o: cgi_util.h
hello_template.o:
post1.o: cgi_util.h
post2.o:
progress.o:
query_string1.o: cgi_util.h
query_string2.o:
transfer_chunked.o: cgi_util.h
