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
	-I$(MY_LIBS_ROOT)/libRano \
	-I$(MY_LIBS_ROOT)/libZnk \


include Makefile_version.mak

BASENAME0=patgen
EXE_FILE0=$O\patgen.exe
OBJS0=\
	$O\pat_diff.o \
	$O\pat_make.o \
	$O\main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libtls-17.dll \



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
	@if not exist ..\..\patgen @mkdir ..\..\patgen 
	@if not exist ..\..\patgen\apply_this_scripts\android @mkdir ..\..\patgen\apply_this_scripts\android 
	@if not exist ..\..\patgen\apply_this_scripts\cygwin @mkdir ..\..\patgen\apply_this_scripts\cygwin 
	@if not exist ..\..\patgen\apply_this_scripts\linux @mkdir ..\..\patgen\apply_this_scripts\linux 
	@if not exist ..\..\patgen\apply_this_scripts\windows @mkdir ..\..\patgen\apply_this_scripts\windows 
	@if exist "cert.pem" @$(CP) /F "cert.pem" ..\..\patgen\ $(CP_END)
	@if exist "patgen.myf" @$(CP) /F "patgen.myf" ..\..\patgen\ $(CP_END)
	@if exist "pat_make.bat" @$(CP) /F "pat_make.bat" ..\..\patgen\ $(CP_END)
	@if exist "pat_make.sh" @$(CP) /F "pat_make.sh" ..\..\patgen\ $(CP_END)
	@if exist "pat_diff.bat" @$(CP) /F "pat_diff.bat" ..\..\patgen\ $(CP_END)
	@if exist "pat_diff.sh" @$(CP) /F "pat_diff.sh" ..\..\patgen\ $(CP_END)
	@if exist "apply_this_scripts\android\*" @$(CP) /F "apply_this_scripts\android\*" ..\..\patgen\apply_this_scripts\android\ $(CP_END)
	@if exist "apply_this_scripts\cygwin\*" @$(CP) /F "apply_this_scripts\cygwin\*" ..\..\patgen\apply_this_scripts\cygwin\ $(CP_END)
	@if exist "apply_this_scripts\linux\*" @$(CP) /F "apply_this_scripts\linux\*" ..\..\patgen\apply_this_scripts\linux\ $(CP_END)
	@if exist "apply_this_scripts\windows\*" @$(CP) /F "apply_this_scripts\windows\*" ..\..\patgen\apply_this_scripts\windows\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\patgen\$(PLATFORM) @mkdir ..\..\patgen\$(PLATFORM) 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\patgen\$(PLATFORM)\ $(CP_END)
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" ..\..\patgen\$(PLATFORM)\ $(CP_END)

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
main.o: pat_diff.h pat_make.h
pat_diff.o: pat_diff.h
