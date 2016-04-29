O = .\out_dir
S = .
COMPILER=gcc -Wall -O2
LINKER=gcc

MY_LIBS_ROOT=..
INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)\libZnk \

BASENAME=moai
OBJS = \
	$O\Moai_connection.o \
	$O\Moai_info.o \
	$O\Moai_io_base.o \
	$O\Moai_post_vars.o \
	$O\Moai_context.o \
	$O\Moai_module.o \
	$O\Moai_post.o \
	$O\Moai_server.o \
	$O\Moai_log.o \
	$O\Moai_fdset.o \
	$O\Moai_parent_proxy.o \
	$O\main.o \

EXE_FILE=$O\$(BASENAME).exe
RES_FILE=$O\$(BASENAME).res

all: $O $(EXE_FILE)

$O:
	if not exist $O mkdir $O

$(EXE_FILE): $(OBJS) $(RES_FILE)
	$(LINKER) -o $(EXE_FILE) $(RES_FILE) $(OBJS) $(MY_LIBS_ROOT)\libZnk\out_dir\Znk-0.9.dll.a

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

