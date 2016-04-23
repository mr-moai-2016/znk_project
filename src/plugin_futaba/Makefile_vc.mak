O = .\out_dir
S = .
COMPILER=cl -nologo /MT /O2 /DNDEBUG
LINKER=LINK /MACHINE:X86 /SUBSYSTEM:console

MY_LIBS_ROOT=..
INCLUDE_FLAG = \
	-I$(MY_LIBS_ROOT)\libZnk \

BASENAME=futaba
OBJS = \
	$O\dll_main.obj \
	$O\main.obj \

DLIB_FILE=$O\$(BASENAME).dll
ILIB_FILE=$O\$(BASENAME).lib
RES_FILE=$O\$(BASENAME).res
DEF_FILE=.\$(BASENAME).def

all: $O $(DLIB_FILE)

$O:
	if not exist $O mkdir $O

$(DLIB_FILE): $(OBJS)
	$(LINKER) /DLL /OUT:$(DLIB_FILE) /IMPLIB:$(ILIB_FILE) $(OBJS) $(MY_LIBS_ROOT)\libZnk\out_dir\Znk-0.8.lib $(SUB_LIBS) ws2_32.lib /DEF:$(DEF_FILE)

# サフィックスルール
{$S}.cpp{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.c{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

$(RES_FILE): $S\$(BASENAME).rc
	rc /fo$(RES_FILE) $S\$(BASENAME).rc

