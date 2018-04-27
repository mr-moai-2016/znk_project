# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=../..
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


include Makefile_version.mak

BASENAME0=zlib
SLIB_FILE0=$O\libzlib.a
OBJS0=\
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

SUB_LIBS=\

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \



# Entry rule.
all: $O $(SLIB_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if exist $(SLIB_FILE0) del $(SLIB_FILE0)
	ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)
	ranlib $(SLIB_FILE0)


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
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib: $(SLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(SLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(SLIBS_DIR) 
	@if exist "$(SLIB_FILE0)" @$(CP) /F "$(SLIB_FILE0)" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install rule.
install: all install_slib 


# Clean rule.
clean:
	del /Q $O\ 

# Src and Headers Dependency
adler32.o: zlib.h
compress.o: zlib.h
crc32.o: zutil.h crc32.h
deflate.o: deflate.h
gzio.o: zutil.h
infback.o: zutil.h inftrees.h inflate.h inffast.h inffixed.h
inffast.o: zutil.h inftrees.h inflate.h inffast.h
inflate.o: zutil.h inftrees.h inflate.h inffast.h inffixed.h
inftrees.o: zutil.h inftrees.h
trees.o: deflate.h trees.h
uncompr.o: zlib.h
zutil.o: zutil.h
