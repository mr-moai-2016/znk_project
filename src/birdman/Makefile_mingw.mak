# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
endif

ifeq ($(MACHINE), x64)
  PLATFORM=win64
else
  MACHINE=x86
  PLATFORM=win32
endif
# Output directory
ABINAME=mingw$(MACHINE)$(DEBUG)
O = .\out_dir\$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=gcc -Wall -Wstrict-aliasing=2 -g 
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)_mingwd
SLIBS_DIR=slib\$(PLATFORM)_mingwd
else
COMPILER=gcc -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG 
LINKER=gcc
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
endif

CP=xcopy /H /C /Y

INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=birdman
EXE_FILE0=$O\birdman.exe
OBJS0=\
	$O\Bdm_base.o \
	$O\main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	@echo $(LINKER) -o $(EXE_FILE0)  {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.a $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.a -lws2_32 
	@     $(LINKER) -o $(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano.a $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.a -lws2_32 


##
# Pattern rule.
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\',
# for example $O\\%.o: $S\%.c .
#
$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\birdman @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\birdman 
	@if exist "birdman.myf" @$(CP) /F "birdman.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\birdman\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\birdman @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\birdman 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\birdman\ $(CP_END)
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" ..\..\moai-v$(REL_VER)-$(PLATFORM)\birdman\ $(CP_END)

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rmdir /S /Q $O\ 

# Src and Headers Dependency
Bdm_base.o: Bdm_base.h
main.o: Bdm_base.h
