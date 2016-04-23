O = ./out_dir
S = .
COMPILER=gcc -Wall -O2
LINKER=gcc

MY_LIBS_ROOT=..
INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libZnk \

BASENAME=moai
OBJS = \
	$O/Moai_connection.o \
	$O/Moai_info.o \
	$O/Moai_io_base.o \
	$O/Moai_post_vars.o \
	$O/Moai_context.o \
	$O/Moai_module.o \
	$O/Moai_post.o \
	$O/Moai_server.o \
	$O/Moai_log.o \
	$O/Moai_fdset.o \
	$O/Moai_parent_proxy.o \
	$O/main.o \

EXE_FILE=$O/$(BASENAME)

CLEAN_LIST= \
	$O/__sentinel_clean__ \
	./$O/*.a \
	./$O/*.so \
	./$O/*.o \
	./$(EXE_FILE) \

all: $O $(EXE_FILE)

$O:
	mkdir -p $O

$(EXE_FILE): $(OBJS)
	$(LINKER) -o $(EXE_FILE) $(OBJS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/libZnk.so -lm

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
