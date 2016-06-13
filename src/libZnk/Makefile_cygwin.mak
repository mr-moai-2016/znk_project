O = ./out_dir
S = .
COMPILER=gcc -Wall -O2
LINKER=gcc

MY_LIBS_ROOT=..
INCLUDE_FLAG=
include Makefile_version.mak

LFLAGS+= -lpthread \
	-ldl \
	-lstdc++ \

BASENAME=Znk
OBJS = \
	$O/dll_main.o \
	$O/Znk_bfr.o \
	$O/Znk_bfr_ary.o \
	$O/Znk_cookie.o \
	$O/Znk_dir.o \
	$O/Znk_dirent.o \
	$O/Znk_dlink.o \
	$O/Znk_envvar.o \
	$O/Znk_err.o \
	$O/Znk_fdset.o \
	$O/Znk_htp_hdrs.o \
	$O/Znk_htp_rar.o \
	$O/Znk_math.o \
	$O/Znk_md5.o \
	$O/Znk_mem_find.o \
	$O/Znk_missing_libc.o \
	$O/Znk_mutex.o \
	$O/Znk_myf.o \
	$O/Znk_net_base.o \
	$O/Znk_net_ip.o \
	$O/Znk_obj_ary.o \
	$O/Znk_prim.o \
	$O/Znk_server.o \
	$O/Znk_socket.o \
	$O/Znk_stdc.o \
	$O/Znk_stock_bio.o \
	$O/Znk_str.o \
	$O/Znk_str_ary.o \
	$O/Znk_str_ex.o \
	$O/Znk_str_fio.o \
	$O/Znk_str_ptn.o \
	$O/Znk_sys_errno.o \
	$O/Znk_s_atom.o \
	$O/Znk_s_base.o \
	$O/Znk_s_posix.o \
	$O/Znk_thread.o \
	$O/Znk_txt_filter.o \
	$O/Znk_var.o \
	$O/Znk_varp_ary.o \
	$O/Znk_yy_base.o \
	$O/Znk_zlib.o \

DLIB_FILE=$O/cyg$(BASENAME).dll
ILIB_FILE=$O/lib$(BASENAME).dll.a
DEF_FILE=.\$(BASENAME)_mingw.def

CLEAN_LIST= \
	$O/__sentinel_clean__ \
	./$O/*.a \
	./$O/*.dll \
	./$O/*.o \

all: $O subdir_zlib $(DLIB_FILE)

.PHONY : subdir_zlib
subdir_zlib:
	@echo ======
	@echo === Entering [zlib] ===
	make -f Makefile_cygwin.mak -C zlib
	@echo === Leaving [zlib] ===
	@echo ======

SUB_LIBS=zlib/$O/zlib.a
SUB_OBJS=zlib/$O/*.o

$O:
	mkdir -p $O

$(DLIB_FILE): $(OBJS)
	if test -e $(DLIB_FILE); then rm $(DLIB_FILE); fi
	gcc -shared -o $(DLIB_FILE) $(OBJS) $(SUB_OBJS) $(LFLAGS) -lc
#	dlltool --kill-at --dllname lib$(BASENAME).dll -d $(DEF_FILE) -l $(ILIB_FILE)

##
# Sub-Directoryに格納されたファイルなどにも柔軟に対応するため、
# ここではサフィックスルールではなくパターンルールを用いる.
#
$O/%.o: $S/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<
$O/%.o: $S/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


clean:
	@for tgt in $(CLEAN_LIST) ; do  if test -e "$$tgt" ; then echo "rm $$tgt" ; rm "$$tgt" ; fi ;  done
