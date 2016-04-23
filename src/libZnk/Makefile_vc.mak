O = .\out_dir
S = .
COMPILER=cl -nologo /MT /O2 /DNDEBUG
LINKER=LINK /MACHINE:X86 /SUBSYSTEM:console

MY_LIBS_ROOT=..
INCLUDE_FLAG =
include Makefile_version.mak

BASENAME=Znk
OBJS = \
	$O\dll_main.obj \
	$O\Znk_bfr.obj \
	$O\Znk_bfr_dary.obj \
	$O\Znk_cookie.obj \
	$O\Znk_dir.obj \
	$O\Znk_dirent.obj \
	$O\Znk_dlink.obj \
	$O\Znk_envvar.obj \
	$O\Znk_err.obj \
	$O\Znk_fdset.obj \
	$O\Znk_htp_hdrs.obj \
	$O\Znk_htp_rar.obj \
	$O\Znk_math.obj \
	$O\Znk_md5.obj \
	$O\Znk_mem_find.obj \
	$O\Znk_missing_libc.obj \
	$O\Znk_mutex.obj \
	$O\Znk_myf.obj \
	$O\Znk_net_base.obj \
	$O\Znk_obj_dary.obj \
	$O\Znk_prim.obj \
	$O\Znk_server.obj \
	$O\Znk_socket.obj \
	$O\Znk_stdc.obj \
	$O\Znk_stock_bio.obj \
	$O\Znk_str.obj \
	$O\Znk_str_dary.obj \
	$O\Znk_str_ex.obj \
	$O\Znk_str_fio.obj \
	$O\Znk_str_ptn.obj \
	$O\Znk_sys_errno.obj \
	$O\Znk_s_atom.obj \
	$O\Znk_s_base.obj \
	$O\Znk_s_posix.obj \
	$O\Znk_thread.obj \
	$O\Znk_txt_filter.obj \
	$O\Znk_var.obj \
	$O\Znk_varp_dary.obj \
	$O\Znk_yy_base.obj \
	$O\Znk_zlib.obj \

DLIB_FILE=$O\$(BASENAME)-$(DL_VER).dll
ILIB_FILE=$O\$(BASENAME)-$(DL_VER).lib
RES_FILE=$O\$(BASENAME).res
DEF_FILE=.\$(BASENAME)_vc.def

all: $O subdir_zlib $(DLIB_FILE)

subdir_zlib:
	@echo ======
	@echo === Entering [zlib] ===
	cd zlib
	nmake -f Makefile_vc.mak
	cd ..
	@echo === Leaving [zlib] ===
	@echo ======

SUB_LIBS=zlib\$O\zlib.lib

$O:
	if not exist $O mkdir $O

$(DLIB_FILE): $(OBJS)
	$(LINKER) /DLL /OUT:$(DLIB_FILE) /IMPLIB:$(ILIB_FILE) $(OBJS) $(SUB_LIBS) ws2_32.lib /DEF:$(DEF_FILE)

# サフィックスルール
{$S}.cpp{$O}.obj:
	$(COMPILER) -Fo$@ -c $< -I$S

{$S}.c{$O}.obj:
	$(COMPILER) -Fo$@ -c $< -I$S

$(RES_FILE): $S\$(BASENAME).rc
	rc /fo$(RES_FILE) $S\$(BASENAME).rc

