O = ./out_dir
S = .
COMPILER=gcc -Wall -O2
LINKER=gcc

MY_LIBS_ROOT=..
INCLUDE_FLAG=

BASENAME=zlib
OBJS = \
	$O/adler32.o \
	$O/compress.o \
	$O/crc32.o \
	$O/deflate.o \
	$O/gzio.o \
	$O/infback.o \
	$O/inffast.o \
	$O/inflate.o \
	$O/inftrees.o \
	$O/trees.o \
	$O/uncompr.o \
	$O/zutil.o \

SLIB_FILE=$O/$(BASENAME).a
RES_FILE=$O/$(BASENAME).res

CLEAN_LIST= \
	$O/__sentinel_clean__ \
	./$O/*.a \
	./$O/*.so \
	./$O/*.o \

all: $O $(SLIB_FILE)

$O:
	mkdir -p $O

$(SLIB_FILE): $(OBJS)
	if test -e $(SLIB_FILE); then rm $(SLIB_FILE); fi
	ar cru $(SLIB_FILE) $(OBJS)
	ranlib $(SLIB_FILE)

##
# Sub-Directoryに格納されたファイルなどにも柔軟に対応するため、
# ここではサフィックスルールではなくパターンルールを用いる.
#
$O/%.o: $S/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c -fPIC $<
$O/%.o: $S/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c -fPIC $<

clean:
	@for tgt in $(CLEAN_LIST) ; do  if test -e "$$tgt" ; then echo "rm $$tgt" ; rm "$$tgt" ; fi ;  done
