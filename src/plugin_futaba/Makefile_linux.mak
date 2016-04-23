O = ./out_dir
S = .
COMPILER=gcc -Wall -O2
LINKER=gcc

MY_LIBS_ROOT=..
INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libZnk \

BASENAME=futaba
OBJS = \
	$O/main.o \

DLIB_FILE=$O/$(BASENAME).so
DLIB_NAME_ALIAS=$(BASENAME).so

CLEAN_LIST= \
	$O/__sentinel_clean__ \
	./$O/*.a \
	./$O/*.so \
	./$O/*.o \


all: $O $(DLIB_FILE)

$O:
	mkdir -p $O

$(DLIB_FILE): $(OBJS)
	if test -e $(DLIB_FILE); then rm $(DLIB_FILE); fi
	gcc -shared -Wl,-soname,$(DLIB_NAME_ALIAS) -o $(DLIB_FILE) $(OBJS) $(SUB_OBJS) $(LFLAGS) -lc
	if [ x"$O" != x ]; then /sbin/ldconfig -n $O; else /sbin/ldconfig -n .; fi

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
