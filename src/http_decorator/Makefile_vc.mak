O = .\out_dir
S = .
COMPILER=cl -nologo /MT /O2 /DNDEBUG
LINKER=LINK /MACHINE:X86 /SUBSYSTEM:console

MY_LIBS_ROOT=..
INCLUDE_FLAG =  \
	-I$(MY_LIBS_ROOT)\libZnk \

BASENAME=http_decorator
OBJS = \
	$O\main.obj \

EXE_FILE=$O\$(BASENAME).exe
RES_FILE=$O\$(BASENAME).res

all: $O $(EXE_FILE)

$O:
	if not exist $O mkdir $O

$(EXE_FILE): $(OBJS) $(RES_FILE)
	$(LINKER) /OUT:$(EXE_FILE) $(RES_FILE) $(OBJS) $(MY_LIBS_ROOT)\libZnk\out_dir\Znk-1.0.lib

# サフィックスルール
{$S}.cpp{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.c{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

$(RES_FILE): $S\$(BASENAME).rc
	rc /fo$(RES_FILE) $S\$(BASENAME).rc

