O = .\out_dir
S = .
COMPILER=gcc -Wall -O2
LINKER=gcc
RM=del

BASENAME=zlib
OBJS = \
	$O\adler32.o \
	$O\compress.o \
	$O\crc32.o \
	$O\deflate.o \
	$O\gzio.o \
	$O\infback.o \
	$O\inffast.o \
	$O\inflate.o \
	$O\inftrees.o \
	$O\trees.o \
	$O\uncompr.o \
	$O\zutil.o \

SLIB_FILE=$O\$(BASENAME).a
RES_FILE=$O\$(BASENAME).res

all: $O $(SLIB_FILE)

$O:
	if not exist $O mkdir $O

$(SLIB_FILE): $(OBJS)
	if exist $(SLIB_FILE) $(RM) $(SLIB_FILE)
	ar cru $(SLIB_FILE) $(OBJS)
	ranlib $(SLIB_FILE)

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
	$(COMPILER) -I$S -o $@ -c $<
$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S -o $@ -c $<

$(RES_FILE): $S\$(BASENAME).rc
	windres --input-format=rc --output-format=coff -o$(RES_FILE) $S\$(BASENAME).rc

