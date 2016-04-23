O = .\out_dir
S = .
COMPILER=cl -nologo /MT
LINKER=LINK /MACHINE:X86 /SUBSYSTEM:console

BASENAME=zlib
OBJS = \
	$O\adler32.obj \
	$O\compress.obj \
	$O\crc32.obj \
	$O\deflate.obj \
	$O\gzio.obj \
	$O\infback.obj \
	$O\inffast.obj \
	$O\inflate.obj \
	$O\inftrees.obj \
	$O\trees.obj \
	$O\uncompr.obj \
	$O\zutil.obj \

SLIB_FILE=$O\$(BASENAME).lib
RES_FILE=$O\$(BASENAME).res

all: $O $(SLIB_FILE)

$O:
	if not exist $O mkdir $O

$(SLIB_FILE): $(OBJS)
	LIB /NOLOGO /OUT:$(SLIB_FILE) $(OBJS)

# サフィックスルール
{$S}.cpp{$O}.obj:
	$(COMPILER) -Fo$@ -c $< -I$S

{$S}.c{$O}.obj:
	$(COMPILER) -Fo$@ -c $< -I$S

$(RES_FILE): $S\$(BASENAME).rc
	rc /fo$(RES_FILE) $S\$(BASENAME).rc

