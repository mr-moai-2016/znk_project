# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
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
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 /wd4127 /wd4996 /wd4267
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG /wd4127 /wd4996 /wd4267
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \


# We cannot use variables in include of nmake.
include Makefile_version.mak

BASENAME0=gslconv
EXE_FILE0=$O\gslconv.exe
OBJS0=\
	$O\Znk_algo_vec.obj \
	$O\Znk_bfr.obj \
	$O\Znk_bfr_ary.obj \
	$O\Znk_bfr_bif.obj \
	$O\Znk_bird.obj \
	$O\Znk_bms_find.obj \
	$O\Znk_cookie.obj \
	$O\Znk_date.obj \
	$O\Znk_dir.obj \
	$O\Znk_dirent.obj \
	$O\Znk_dir_recursive.obj \
	$O\Znk_dir_type.obj \
	$O\Znk_dlink.obj \
	$O\Znk_duple_ary.obj \
	$O\Znk_envvar.obj \
	$O\Znk_err.obj \
	$O\Znk_fdset.obj \
	$O\Znk_htp_hdrs.obj \
	$O\Znk_htp_post.obj \
	$O\Znk_htp_rar.obj \
	$O\Znk_htp_util.obj \
	$O\Znk_liba_scan.obj \
	$O\Znk_math.obj \
	$O\Znk_mbc_jp.obj \
	$O\Znk_md5.obj \
	$O\Znk_mem_find.obj \
	$O\Znk_missing_libc.obj \
	$O\Znk_mutex.obj \
	$O\Znk_myf.obj \
	$O\Znk_net_base.obj \
	$O\Znk_net_ip.obj \
	$O\Znk_nset.obj \
	$O\Znk_obj_ary.obj \
	$O\Znk_prim.obj \
	$O\Znk_primp_ary.obj \
	$O\Znk_process.obj \
	$O\Znk_rand.obj \
	$O\Znk_rgx.obj \
	$O\Znk_rgx_dfa.obj \
	$O\Znk_rgx_nfa.obj \
	$O\Znk_rgx_tree.obj \
	$O\Znk_server.obj \
	$O\Znk_socket.obj \
	$O\Znk_stdc.obj \
	$O\Znk_stock_bio.obj \
	$O\Znk_str.obj \
	$O\Znk_str_ary.obj \
	$O\Znk_str_ex.obj \
	$O\Znk_str_fio.obj \
	$O\Znk_str_path.obj \
	$O\Znk_str_ptn.obj \
	$O\Znk_sys_errno.obj \
	$O\Znk_s_atom.obj \
	$O\Znk_s_base.obj \
	$O\Znk_s_posix.obj \
	$O\Znk_thread.obj \
	$O\Znk_tostr_double.obj \
	$O\Znk_tostr_int.obj \
	$O\Znk_var.obj \
	$O\Znk_varp_ary.obj \
	$O\Znk_vsnprintf.obj \
	$O\Znk_yy_base.obj \
	$O\Znk_zlib.obj \
	$O\gslconv.obj \

BASENAME1=Znk
DLIB_NAME1=Znk-$(DL_VER).dll
DLIB_FILE1=$O\$(DLIB_NAME1)
ILIB_FILE1=$O\Znk-$(DL_VER).imp.lib
SLIB_FILE1=$O\Znk.lib
OBJS1=\
	$O\Znk_algo_vec.obj \
	$O\Znk_bfr.obj \
	$O\Znk_bfr_ary.obj \
	$O\Znk_bfr_bif.obj \
	$O\Znk_bird.obj \
	$O\Znk_bms_find.obj \
	$O\Znk_cookie.obj \
	$O\Znk_date.obj \
	$O\Znk_dir.obj \
	$O\Znk_dirent.obj \
	$O\Znk_dir_recursive.obj \
	$O\Znk_dir_type.obj \
	$O\Znk_dlink.obj \
	$O\Znk_duple_ary.obj \
	$O\Znk_envvar.obj \
	$O\Znk_err.obj \
	$O\Znk_fdset.obj \
	$O\Znk_htp_hdrs.obj \
	$O\Znk_htp_post.obj \
	$O\Znk_htp_rar.obj \
	$O\Znk_htp_util.obj \
	$O\Znk_liba_scan.obj \
	$O\Znk_math.obj \
	$O\Znk_mbc_jp.obj \
	$O\Znk_md5.obj \
	$O\Znk_mem_find.obj \
	$O\Znk_missing_libc.obj \
	$O\Znk_mutex.obj \
	$O\Znk_myf.obj \
	$O\Znk_net_base.obj \
	$O\Znk_net_ip.obj \
	$O\Znk_nset.obj \
	$O\Znk_obj_ary.obj \
	$O\Znk_prim.obj \
	$O\Znk_primp_ary.obj \
	$O\Znk_process.obj \
	$O\Znk_rand.obj \
	$O\Znk_rgx.obj \
	$O\Znk_rgx_dfa.obj \
	$O\Znk_rgx_nfa.obj \
	$O\Znk_rgx_tree.obj \
	$O\Znk_server.obj \
	$O\Znk_socket.obj \
	$O\Znk_stdc.obj \
	$O\Znk_stock_bio.obj \
	$O\Znk_str.obj \
	$O\Znk_str_ary.obj \
	$O\Znk_str_ex.obj \
	$O\Znk_str_fio.obj \
	$O\Znk_str_path.obj \
	$O\Znk_str_ptn.obj \
	$O\Znk_sys_errno.obj \
	$O\Znk_s_atom.obj \
	$O\Znk_s_base.obj \
	$O\Znk_s_posix.obj \
	$O\Znk_thread.obj \
	$O\Znk_tostr_double.obj \
	$O\Znk_tostr_int.obj \
	$O\Znk_var.obj \
	$O\Znk_varp_ary.obj \
	$O\Znk_vsnprintf.obj \
	$O\Znk_yy_base.obj \
	$O\Znk_zlib.obj \
	$O\dll_main.obj \

SUB_LIBS=\
	zlib/$O/zlib.lib \

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
all: $O submkf_zlib $(EXE_FILE0) $(DLIB_FILE1) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	$(LINKER) /OUT:$(EXE_FILE0)  $(OBJS0) $(SUB_LIBS)  ws2_32.lib 

$(SLIB_FILE1): $(OBJS1)
	LIB /NOLOGO /OUT:$(SLIB_FILE1) $(OBJS1) $(SUB_LIBS)

gsl.myf: $(SLIB_FILE1)
	@if exist $O\gslconv.exe $O\gslconv.exe -g gsl.myf $(SLIB_FILE1) $(MACHINE)

gsl.def: gsl.myf
	@if exist $O\gslconv.exe $O\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE1): $(OBJS1) $(SLIB_FILE1) gsl.def
	$(LINKER) /DLL /OUT:$(DLIB_FILE1) /IMPLIB:$(ILIB_FILE1) $(OBJS1) $(SUB_LIBS)  ws2_32.lib  /DEF:gsl.def


# Suffix rule.
{$S}.c{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.cpp{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<


# Rc rule.

# Submkf rule.
submkf_zlib:
	@echo ======
	@echo === Entering [zlib] ===
	cd zlib
	nmake -f Makefile_vc.mak
	cd ..
	@echo === Leaving [zlib] ===
	@echo ======


# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\mkfsys @mkdir ..\..\mkfsys 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\mkfsys\ $(CP_END)
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" ..\..\mkfsys\ $(CP_END)

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
	del /Q $O\ 
	@echo === Entering [zlib] ===
	cd zlib
	nmake -f Makefile_vc.mak clean
	cd ..
	@echo === Leaving [zlib] ===


# Src and Headers Dependency
dll_main.obj:
gslconv.obj: Znk_myf.h Znk_str_ary.h Znk_str_ex.h Znk_str_fio.h Znk_stdc.h Znk_missing_libc.h Znk_dir.h Znk_str_path.h Znk_liba_scan.h
Znk_algo_vec.obj: Znk_algo_vec.h
Znk_bfr.obj: Znk_bfr.h Znk_stdc.h
Znk_bfr_ary.obj: Znk_bfr_ary.h
Znk_bfr_bif.obj: Znk_bfr_bif.h Znk_bfr.h
Znk_bird.obj: Znk_bird.h Znk_mem_find.h Znk_stdc.h Znk_varp_ary.h Znk_bms_find.h
Znk_bms_find.obj: Znk_bms_find.h Znk_mem_find.h Znk_s_base.h Znk_stdc.h
Znk_cookie.obj: Znk_cookie.h Znk_s_base.h Znk_stdc.h Znk_str_fio.h
Znk_date.obj: Znk_date.h Znk_base.h Znk_str.h Znk_s_base.h
Znk_dir.obj: Znk_dir.h Znk_dir_recursive.h Znk_stdc.h Znk_s_base.h Znk_missing_libc.h Znk_sys_errno.h
Znk_dirent.obj: Znk_dir.h Znk_s_base.h Znk_stdc.h
Znk_dir_recursive.obj: Znk_dir_recursive.h Znk_dir.h Znk_str.h Znk_str_ary.h Znk_err.h Znk_stdc.h
Znk_dir_type.obj: Znk_dir.h Znk_sys_errno.h Znk_stdc.h
Znk_dlink.obj: Znk_dlink.h
Znk_duple_ary.obj: Znk_duple_ary.h Znk_bfr.h Znk_stdc.h Znk_s_base.h Znk_base.h Znk_str.h Znk_obj_ary.h
Znk_envvar.obj: Znk_envvar.h Znk_missing_libc.h Znk_mutex.h
Znk_err.obj: Znk_err.h Znk_mutex.h Znk_str.h
Znk_fdset.obj: Znk_fdset.h Znk_stdc.h
Znk_htp_hdrs.obj: Znk_htp_hdrs.h Znk_s_base.h Znk_varp_ary.h Znk_missing_libc.h
Znk_htp_post.obj: Znk_htp_post.h Znk_str.h Znk_missing_libc.h
Znk_htp_rar.obj: Znk_htp_rar.h Znk_net_base.h Znk_socket.h Znk_cookie.h Znk_s_base.h Znk_str_ary.h Znk_stdc.h Znk_mem_find.h Znk_sys_errno.h Znk_str_ex.h Znk_stock_bio.h Znk_zlib.h Znk_def_util.h
Znk_htp_util.obj: Znk_htp_util.h Znk_str_ex.h Znk_mem_find.h
Znk_liba_scan.obj: Znk_stdc.h Znk_s_base.h Znk_str.h Znk_str_ary.h Znk_myf.h Znk_bfr.h Znk_vpod.h
Znk_math.obj: Znk_math.h
Znk_mbc_jp.obj: Znk_mbc_jp.h Znk_bfr_bif.h
Znk_md5.obj: Znk_md5.h Znk_stdc.h
Znk_mem_find.obj: Znk_mem_find.h Znk_stdc.h
Znk_missing_libc.obj: Znk_missing_libc.h Znk_vsnprintf.h Znk_s_base.h
Znk_mutex.obj: Znk_mutex.h Znk_bfr.h Znk_stdc.h
Znk_myf.obj: Znk_myf.h Znk_str.h Znk_str_fio.h Znk_str_ary.h Znk_stdc.h Znk_s_base.h Znk_varp_ary.h Znk_primp_ary.h
Znk_net_base.obj: Znk_net_base.h Znk_s_base.h Znk_sys_errno.h Znk_stdc.h Znk_missing_libc.h
Znk_net_ip.obj: Znk_net_ip.h Znk_stdc.h Znk_s_base.h Znk_missing_libc.h
Znk_nset.obj: Znk_nset.h Znk_vpod.h Znk_stdc.h
Znk_obj_ary.obj: Znk_obj_ary.h Znk_bfr.h Znk_stdc.h Znk_vpod.h
Znk_prim.obj: Znk_prim.h Znk_primp_ary.h
Znk_primp_ary.obj: Znk_primp_ary.h Znk_s_base.h
Znk_process.obj: Znk_process.h Znk_str.h Znk_stdc.h
Znk_rand.obj: Znk_rand.h
Znk_rgx.obj: Znk_rgx.h Znk_rgx_tree.h Znk_rgx_nfa.h Znk_rgx_dfa.h Znk_stdc.h Znk_str_fio.h Znk_s_base.h
Znk_rgx_dfa.obj: Znk_rgx_dfa.h Znk_base.h Znk_stdc.h Znk_str.h
Znk_rgx_nfa.obj: Znk_rgx_nfa.h Znk_base.h Znk_stdc.h
Znk_rgx_tree.obj: Znk_rgx_tree.h Znk_base.h Znk_stdc.h Znk_str.h
Znk_server.obj: Znk_server.h Znk_socket.h Znk_stdc.h
Znk_socket.obj: Znk_socket.h Znk_net_base.h Znk_sys_errno.h Znk_stdc.h Znk_thread.h Znk_missing_libc.h
Znk_stdc.obj: Znk_stdc.h Znk_bfr.h Znk_vsnprintf.h
Znk_stock_bio.obj: Znk_stock_bio.h Znk_stdc.h
Znk_str.obj: Znk_str.h Znk_stdc.h Znk_s_base.h Znk_vsnprintf.h
Znk_str_ary.obj: Znk_str_ary.h Znk_stdc.h Znk_s_base.h Znk_algo_vec.h
Znk_str_ex.obj: Znk_str_ex.h Znk_str_ary.h Znk_s_base.h Znk_mem_find.h Znk_def_util.h
Znk_str_fio.obj: Znk_str_fio.h Znk_stdc.h
Znk_str_path.obj: Znk_str_path.h Znk_s_base.h Znk_missing_libc.h Znk_str_ary.h Znk_str_ex.h
Znk_str_ptn.obj: Znk_str_ptn.h Znk_missing_libc.h Znk_stdc.h Znk_s_base.h Znk_mem_find.h Znk_str_ex.h
Znk_sys_errno.obj: Znk_sys_errno.h
Znk_s_atom.obj: Znk_s_atom.h Znk_str_ary.h Znk_stdc.h
Znk_s_base.obj: Znk_s_base.h Znk_mem_find.h Znk_stdc.h Znk_def_util.h Znk_s_posix.h
Znk_s_posix.obj: Znk_s_posix.h Znk_stdc.h
Znk_thread.obj: Znk_thread.h Znk_bfr.h Znk_stdc.h Znk_mutex.h
Znk_tostr_double.obj: Znk_tostr_double.h Znk_def_util.h
Znk_tostr_int.obj: Znk_tostr_int.h Znk_base.h Znk_s_base.h Znk_def_util.h Znk_stdc.h
Znk_var.obj: Znk_var.h Znk_stdc.h
Znk_varp_ary.obj: Znk_varp_ary.h Znk_s_base.h
Znk_vsnprintf.obj: Znk_vsnprintf.h Znk_stdc.h Znk_def_util.h Znk_tostr_int.h Znk_tostr_double.h
Znk_yy_base.obj: Znk_yy_base.h
Znk_zlib.obj: Znk_zlib.h Znk_dlhlp.h Znk_stdc.h
