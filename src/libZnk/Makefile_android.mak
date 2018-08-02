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

BASENAME0=gslconv
EXE_FILE0=$O\gslconv
OBJS0=\
	$O\minizip/ioapi.o \
	$O\minizip/unzip.o \
	$O\Znk_algo_vec.o \
	$O\Znk_bfr.o \
	$O\Znk_bfr_ary.o \
	$O\Znk_bfr_bif.o \
	$O\Znk_bird.o \
	$O\Znk_bms_find.o \
	$O\Znk_cookie.o \
	$O\Znk_date.o \
	$O\Znk_dir.o \
	$O\Znk_dirent.o \
	$O\Znk_dir_recursive.o \
	$O\Znk_dir_type.o \
	$O\Znk_dlink.o \
	$O\Znk_duple_ary.o \
	$O\Znk_envvar.o \
	$O\Znk_err.o \
	$O\Znk_fdset.o \
	$O\Znk_htp_hdrs.o \
	$O\Znk_htp_post.o \
	$O\Znk_htp_rar.o \
	$O\Znk_htp_sbio.o \
	$O\Znk_htp_util.o \
	$O\Znk_liba_scan.o \
	$O\Znk_math.o \
	$O\Znk_mbc_jp.o \
	$O\Znk_md5.o \
	$O\Znk_mem_find.o \
	$O\Znk_missing_libc.o \
	$O\Znk_mutex.o \
	$O\Znk_myf.o \
	$O\Znk_net_base.o \
	$O\Znk_net_ip.o \
	$O\Znk_nset.o \
	$O\Znk_obj_ary.o \
	$O\Znk_prim.o \
	$O\Znk_primp_ary.o \
	$O\Znk_process.o \
	$O\Znk_rand.o \
	$O\Znk_rgx.o \
	$O\Znk_rgx_dfa.o \
	$O\Znk_rgx_nfa.o \
	$O\Znk_rgx_tree.o \
	$O\Znk_server.o \
	$O\Znk_socket.o \
	$O\Znk_stdc.o \
	$O\Znk_stock_bio.o \
	$O\Znk_str.o \
	$O\Znk_str_ary.o \
	$O\Znk_str_ex.o \
	$O\Znk_str_fio.o \
	$O\Znk_str_path.o \
	$O\Znk_str_ptn.o \
	$O\Znk_sys_errno.o \
	$O\Znk_s_atom.o \
	$O\Znk_s_base.o \
	$O\Znk_s_posix.o \
	$O\Znk_thread.o \
	$O\Znk_tostr_double.o \
	$O\Znk_tostr_int.o \
	$O\Znk_var.o \
	$O\Znk_varp_ary.o \
	$O\Znk_vsnprintf.o \
	$O\Znk_yy_base.o \
	$O\Znk_zip.o \
	$O\Znk_zlib.o \
	$O\gslconv.o \

BASENAME1=Znk
DLIB_NAME1=libZnk-$(DL_VER).so
DLIB_FILE1=$O\$(DLIB_NAME1)
ILIB_FILE1=$O\libZnk-$(DL_VER).so
SLIB_FILE1=$O\libZnk.a
OBJS1=\
	$O\minizip/ioapi.o \
	$O\minizip/unzip.o \
	$O\Znk_algo_vec.o \
	$O\Znk_bfr.o \
	$O\Znk_bfr_ary.o \
	$O\Znk_bfr_bif.o \
	$O\Znk_bird.o \
	$O\Znk_bms_find.o \
	$O\Znk_cookie.o \
	$O\Znk_date.o \
	$O\Znk_dir.o \
	$O\Znk_dirent.o \
	$O\Znk_dir_recursive.o \
	$O\Znk_dir_type.o \
	$O\Znk_dlink.o \
	$O\Znk_duple_ary.o \
	$O\Znk_envvar.o \
	$O\Znk_err.o \
	$O\Znk_fdset.o \
	$O\Znk_htp_hdrs.o \
	$O\Znk_htp_post.o \
	$O\Znk_htp_rar.o \
	$O\Znk_htp_sbio.o \
	$O\Znk_htp_util.o \
	$O\Znk_liba_scan.o \
	$O\Znk_math.o \
	$O\Znk_mbc_jp.o \
	$O\Znk_md5.o \
	$O\Znk_mem_find.o \
	$O\Znk_missing_libc.o \
	$O\Znk_mutex.o \
	$O\Znk_myf.o \
	$O\Znk_net_base.o \
	$O\Znk_net_ip.o \
	$O\Znk_nset.o \
	$O\Znk_obj_ary.o \
	$O\Znk_prim.o \
	$O\Znk_primp_ary.o \
	$O\Znk_process.o \
	$O\Znk_rand.o \
	$O\Znk_rgx.o \
	$O\Znk_rgx_dfa.o \
	$O\Znk_rgx_nfa.o \
	$O\Znk_rgx_tree.o \
	$O\Znk_server.o \
	$O\Znk_socket.o \
	$O\Znk_stdc.o \
	$O\Znk_stock_bio.o \
	$O\Znk_str.o \
	$O\Znk_str_ary.o \
	$O\Znk_str_ex.o \
	$O\Znk_str_fio.o \
	$O\Znk_str_path.o \
	$O\Znk_str_ptn.o \
	$O\Znk_sys_errno.o \
	$O\Znk_s_atom.o \
	$O\Znk_s_base.o \
	$O\Znk_s_posix.o \
	$O\Znk_thread.o \
	$O\Znk_tostr_double.o \
	$O\Znk_tostr_int.o \
	$O\Znk_var.o \
	$O\Znk_varp_ary.o \
	$O\Znk_vsnprintf.o \
	$O\Znk_yy_base.o \
	$O\Znk_zip.o \
	$O\Znk_zlib.o \
	$O\dll_main.o \

SUB_LIBS=\
	zlib/$O/libzlib.a \

SUB_OBJS=\
	zlib/$O/*.o \

SUB_OBJS_ECHO=\
	zlib/$O/{[objs]} \

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE1) \

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE1) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O\minizip $O submkf_zlib $(EXE_FILE0) $(DLIB_FILE1) $(SLIB_FILE1) 

# Mkdir rule.
$O\minizip:
	if not exist $O\minizip mkdir $O\minizip

$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	@echo $(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc {[objs]} $(SUB_LIBS) -Wl,-rpath,.  $(LINKER_OPT) -o $(EXE_FILE0)
	@     $(LINKER) -Wl,--gc-sections -Wl,-z,nocopyreloc $(RPATH_LINK) \
	-lgcc $(OBJS0) $(SUB_LIBS) -Wl,-rpath,.  $(LINKER_OPT) -o $(EXE_FILE0)

$(SLIB_FILE1): $(OBJS1)
	if exist $(SLIB_FILE1) del $(SLIB_FILE1)
	@echo $(LIBAR) crsD $(SLIB_FILE1) {[objs]} $(SUB_OBJS_ECHO)
	@     $(LIBAR) crsD $(SLIB_FILE1) $(OBJS1) $(SUB_OBJS)

gsl.myf: $(SLIB_FILE1)
	@if exist $O\gslconv.exe $O\gslconv.exe -g gsl.myf $(SLIB_FILE1) $(MACHINE)

gsl.def: gsl.myf
	@if exist $O\gslconv.exe $O\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE1): $(OBJS1)
	if exist $(DLIB_FILE1) del $(DLIB_FILE1)
	@echo $(LINKER) -shared -Wl,-soname,$(DLIB_NAME1) \
	-lgcc {[objs]} $(SUB_LIBS) -Wl,-rpath,.  $(LINKER_OPT) -o $(DLIB_FILE1)
	@     $(LINKER) -shared -Wl,-soname,$(DLIB_NAME1) \
	-lgcc $(OBJS1) $(SUB_LIBS) -Wl,-rpath,.  $(LINKER_OPT) -o $(DLIB_FILE1)
	$(STRIP_UNNEEDED) $(DLIB_FILE1)


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
$O\\minizip\\%.o: $S\minizip\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\minizip\\%.o: $S\minizip\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Submkf rule.
.PHONY : submkf_zlib
submkf_zlib:
	@echo ======
	@echo === Entering [zlib] ===
	$(MAKE_CMD) -f Makefile_android.mak -C zlib
	@echo === Leaving [zlib] ===
	@echo ======


# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\mkfsys\$(PLATFORM) @mkdir ..\..\mkfsys\$(PLATFORM) 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\mkfsys\$(PLATFORM)\ $(CP_END)
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" ..\..\mkfsys\$(PLATFORM)\ $(CP_END)

# Install dlib rule.
install_dlib: $(DLIB_FILE1)
	@if not exist $(MY_LIBS_ROOT)\$(DLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(DLIBS_DIR) 
	@if exist "$(DLIB_FILE1)" @$(CP) /F "$(DLIB_FILE1)" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install slib rule.
install_slib: $(SLIB_FILE1)
	@if not exist $(MY_LIBS_ROOT)\$(SLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(SLIBS_DIR) 
	@if exist "$(SLIB_FILE1)" @$(CP) /F "$(SLIB_FILE1)" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install rule.
install: all install_slib install_dlib install_exec 


# Clean rule.
clean:
	rmdir /S /Q $O\ 
	@echo === Entering [zlib] ===
	$(MAKE_CMD) -f Makefile_android.mak clean -C zlib
	@echo === Leaving [zlib] ===


# Src and Headers Dependency
gslconv.o: Znk_myf.h Znk_str_ary.h Znk_str_ex.h Znk_str_fio.h Znk_stdc.h Znk_missing_libc.h Znk_dir.h Znk_str_path.h Znk_liba_scan.h
minizip/ioapi.o: minizip/ioapi.h
minizip/unzip.o: minizip/unzip.h minizip/crypt.h
Znk_algo_vec.o: Znk_algo_vec.h
Znk_bfr.o: Znk_bfr.h Znk_stdc.h
Znk_bfr_ary.o: Znk_bfr_ary.h
Znk_bfr_bif.o: Znk_bfr_bif.h Znk_bfr.h
Znk_bird.o: Znk_bird.h Znk_mem_find.h Znk_stdc.h Znk_varp_ary.h Znk_bms_find.h
Znk_bms_find.o: Znk_bms_find.h Znk_mem_find.h Znk_s_base.h Znk_stdc.h
Znk_cookie.o: Znk_cookie.h Znk_s_base.h Znk_stdc.h Znk_str_fio.h
Znk_date.o: Znk_date.h Znk_base.h Znk_str.h Znk_s_base.h
Znk_dir.o: Znk_dir.h Znk_dir_recursive.h Znk_stdc.h Znk_s_base.h Znk_missing_libc.h Znk_sys_errno.h
Znk_dirent.o: Znk_dir.h Znk_s_base.h Znk_stdc.h
Znk_dir_recursive.o: Znk_dir_recursive.h Znk_dir.h Znk_str.h Znk_str_ary.h Znk_err.h Znk_stdc.h
Znk_dir_type.o: Znk_dir.h Znk_sys_errno.h Znk_stdc.h
Znk_dlink.o: Znk_dlink.h
Znk_duple_ary.o: Znk_duple_ary.h Znk_bfr.h Znk_stdc.h Znk_s_base.h Znk_base.h Znk_str.h Znk_obj_ary.h
Znk_envvar.o: Znk_envvar.h Znk_missing_libc.h Znk_mutex.h
Znk_err.o: Znk_err.h Znk_mutex.h Znk_str.h
Znk_fdset.o: Znk_fdset.h Znk_stdc.h
Znk_htp_hdrs.o: Znk_htp_hdrs.h Znk_s_base.h Znk_varp_ary.h Znk_missing_libc.h
Znk_htp_post.o: Znk_htp_post.h Znk_str.h Znk_missing_libc.h
Znk_htp_rar.o: Znk_htp_rar.h Znk_htp_sbio.h Znk_socket.h Znk_cookie.h Znk_s_base.h Znk_str_ary.h Znk_stdc.h Znk_sys_errno.h Znk_stock_bio.h
Znk_htp_sbio.o: Znk_htp_sbio.h Znk_net_base.h Znk_cookie.h Znk_s_base.h Znk_str_ary.h Znk_stdc.h Znk_mem_find.h Znk_sys_errno.h Znk_stock_bio.h Znk_zlib.h Znk_def_util.h
Znk_htp_util.o: Znk_htp_util.h Znk_str_ex.h Znk_mem_find.h
Znk_liba_scan.o: Znk_stdc.h Znk_s_base.h Znk_str.h Znk_str_ary.h Znk_myf.h Znk_bfr.h Znk_vpod.h
Znk_math.o: Znk_math.h
Znk_mbc_jp.o: Znk_mbc_jp.h Znk_bfr_bif.h
Znk_md5.o: Znk_md5.h Znk_stdc.h
Znk_mem_find.o: Znk_mem_find.h Znk_stdc.h
Znk_missing_libc.o: Znk_missing_libc.h Znk_vsnprintf.h Znk_s_base.h
Znk_mutex.o: Znk_mutex.h Znk_bfr.h Znk_stdc.h
Znk_myf.o: Znk_myf.h Znk_str.h Znk_str_fio.h Znk_str_ary.h Znk_stdc.h Znk_s_base.h Znk_varp_ary.h Znk_primp_ary.h
Znk_net_base.o: Znk_net_base.h Znk_s_base.h Znk_sys_errno.h Znk_stdc.h Znk_missing_libc.h
Znk_net_ip.o: Znk_net_ip.h Znk_stdc.h Znk_s_base.h Znk_missing_libc.h
Znk_nset.o: Znk_nset.h Znk_vpod.h Znk_stdc.h
Znk_obj_ary.o: Znk_obj_ary.h Znk_bfr.h Znk_stdc.h Znk_vpod.h
Znk_prim.o: Znk_prim.h Znk_primp_ary.h
Znk_primp_ary.o: Znk_primp_ary.h Znk_s_base.h
Znk_process.o: Znk_process.h Znk_str.h Znk_stdc.h
Znk_rand.o: Znk_rand.h
Znk_rgx.o: Znk_rgx.h Znk_rgx_tree.h Znk_rgx_nfa.h Znk_rgx_dfa.h Znk_stdc.h Znk_str_fio.h Znk_s_base.h
Znk_rgx_dfa.o: Znk_rgx_dfa.h Znk_base.h Znk_stdc.h Znk_str.h
Znk_rgx_nfa.o: Znk_rgx_nfa.h Znk_base.h Znk_stdc.h
Znk_rgx_tree.o: Znk_rgx_tree.h Znk_base.h Znk_stdc.h Znk_str.h
Znk_server.o: Znk_server.h Znk_socket.h Znk_stdc.h
Znk_socket.o: Znk_socket.h Znk_net_base.h Znk_sys_errno.h Znk_stdc.h Znk_thread.h Znk_missing_libc.h
Znk_stdc.o: Znk_stdc.h Znk_bfr.h Znk_vsnprintf.h
Znk_stock_bio.o: Znk_stock_bio.h Znk_stdc.h
Znk_str.o: Znk_str.h Znk_stdc.h Znk_s_base.h Znk_vsnprintf.h
Znk_str_ary.o: Znk_str_ary.h Znk_stdc.h Znk_s_base.h Znk_algo_vec.h
Znk_str_ex.o: Znk_str_ex.h Znk_str_ary.h Znk_s_base.h Znk_mem_find.h Znk_def_util.h
Znk_str_fio.o: Znk_str_fio.h Znk_stdc.h
Znk_str_path.o: Znk_str_path.h Znk_s_base.h Znk_missing_libc.h Znk_str_ary.h Znk_str_ex.h
Znk_str_ptn.o: Znk_str_ptn.h Znk_missing_libc.h Znk_stdc.h Znk_s_base.h Znk_mem_find.h Znk_str_ex.h
Znk_sys_errno.o: Znk_sys_errno.h
Znk_s_atom.o: Znk_s_atom.h Znk_str_ary.h Znk_stdc.h
Znk_s_base.o: Znk_s_base.h Znk_mem_find.h Znk_stdc.h Znk_def_util.h Znk_s_posix.h
Znk_s_posix.o: Znk_s_posix.h Znk_stdc.h
Znk_thread.o: Znk_thread.h Znk_bfr.h Znk_stdc.h Znk_mutex.h
Znk_tostr_double.o: Znk_tostr_double.h Znk_def_util.h
Znk_tostr_int.o: Znk_tostr_int.h Znk_base.h Znk_s_base.h Znk_def_util.h Znk_stdc.h
Znk_var.o: Znk_var.h Znk_stdc.h
Znk_varp_ary.o: Znk_varp_ary.h Znk_s_base.h
Znk_vsnprintf.o: Znk_vsnprintf.h Znk_stdc.h Znk_def_util.h Znk_tostr_int.h Znk_tostr_double.h
Znk_yy_base.o: Znk_yy_base.h
Znk_zip.o: Znk_zip.h Znk_stdc.h Znk_missing_libc.h Znk_dir.h minizip/unzip.h minizip/iowin32.h
Znk_zlib.o: Znk_zlib.h Znk_dlhlp.h Znk_stdc.h
