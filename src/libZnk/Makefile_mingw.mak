O = .\out_dir
S = .
COMPILER=gcc -Wall -O2
LINKER=gcc

MY_LIBS_ROOT=..
INCLUDE_FLAG=
include Makefile_version.mak

BASENAME=Znk
OBJS = \
	$O\dll_main.o \
	$O\Znk_bfr.o \
	$O\Znk_bfr_dary.o \
	$O\Znk_cookie.o \
	$O\Znk_dir.o \
	$O\Znk_dirent.o \
	$O\Znk_dlink.o \
	$O\Znk_envvar.o \
	$O\Znk_err.o \
	$O\Znk_fdset.o \
	$O\Znk_htp_hdrs.o \
	$O\Znk_htp_rar.o \
	$O\Znk_math.o \
	$O\Znk_md5.o \
	$O\Znk_mem_find.o \
	$O\Znk_missing_libc.o \
	$O\Znk_mutex.o \
	$O\Znk_myf.o \
	$O\Znk_net_base.o \
	$O\Znk_net_ip.o \
	$O\Znk_obj_dary.o \
	$O\Znk_prim.o \
	$O\Znk_server.o \
	$O\Znk_socket.o \
	$O\Znk_stdc.o \
	$O\Znk_stock_bio.o \
	$O\Znk_str.o \
	$O\Znk_str_dary.o \
	$O\Znk_str_ex.o \
	$O\Znk_str_fio.o \
	$O\Znk_str_ptn.o \
	$O\Znk_sys_errno.o \
	$O\Znk_s_atom.o \
	$O\Znk_s_base.o \
	$O\Znk_s_posix.o \
	$O\Znk_thread.o \
	$O\Znk_txt_filter.o \
	$O\Znk_var.o \
	$O\Znk_varp_dary.o \
	$O\Znk_yy_base.o \
	$O\Znk_zlib.o \

DLIB_FILE=$O\$(BASENAME)-$(DL_VER).dll
ILIB_FILE=$O\$(BASENAME)-$(DL_VER).dll.a
RES_FILE=$O\$(BASENAME).res
DEF_FILE=.\$(BASENAME)_mingw.def

all: $O subdir_zlib $(DLIB_FILE)

.PHONY : subdir_zlib
subdir_zlib:
	@echo ======
	@echo === Entering [zlib] ===
	mingw32-make -f Makefile_mingw.mak -C zlib
	@echo === Leaving [zlib] ===
	@echo ======

SUB_LIBS=zlib\$O\zlib.a

$O:
	if not exist $O mkdir $O

$(DLIB_FILE): $(OBJS)
	gcc -static-libgcc -g -Wl,--disable-stdcall-fixup,--kill-at -shared -o $(DLIB_FILE) $(OBJS) $(SUB_LIBS) -lws2_32 $(DEF_FILE)
	dlltool --kill-at --dllname Znk-$(DL_VER).dll -d $(DEF_FILE) -l $(ILIB_FILE)

##
# Sub-Directoryに格納されたファイルなどにも柔軟に対応するため、
# ここではサフィックスルールではなくパターンルールを用いる.
# このとき : より左側に関しては \ はエスケープシーケンスとして働き、
# それより右側についてはエスケープシーケンスとして働かないという
# 非常にややこしい仕様が存在する.
# よって、例えば $O\\%.o: $S\%.c のように : より左側に関しては \\ と
# 二つ重ね、それより右側については反対に \ を一つだけにしなければならない.
#
$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<
$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$(RES_FILE): $S\$(BASENAME).rc
	windres --input-format=rc --output-format=coff -o$(RES_FILE) $S\$(BASENAME).rc

