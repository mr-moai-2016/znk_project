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


include Makefile_version.mak

BASENAME0=5ch
DLIB_NAME0=5ch.dll
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\5ch.dll.a
SLIB_FILE0=$O\5ch.a
OBJS0=\
	$O\init.o \
	$O\main.o \
	$O\dll_main.o \

SUB_LIBS=\

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \



# Entry rule.
all: $O $(DLIB_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if exist $(SLIB_FILE0) del $(SLIB_FILE0)
	ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)
	ranlib $(SLIB_FILE0)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\gslconv.exe $(MKFSYS_DIR)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\gslconv.exe $(MKFSYS_DIR)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0) gsl.def
	gcc -static-libgcc -g -Wl,--disable-stdcall-fixup,--kill-at -shared -o $(DLIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a -lws2_32  gsl.def
	dlltool --kill-at --dllname $(DLIB_FILE0) -d gsl.def -l $(ILIB_FILE0)


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

# Install exec rule.
install_exec:
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" ..\..\moai-v$(REL_VER)-$(PLATFORM)\plugins\ $(CP_END)

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\plugins @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\plugins 
	@if exist "$(DLIB_FILE0)" @$(CP) /F "$(DLIB_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\plugins\ $(CP_END)

# Install slib rule.
install_slib:

# Install rule.
install: all install_dlib 


# Clean rule.
clean:
	del /Q $O\ 

# Src and Headers Dependency
dll_main.o:
init.o: Moai_plugin_dev.h
main.o: Moai_plugin_dev.h
