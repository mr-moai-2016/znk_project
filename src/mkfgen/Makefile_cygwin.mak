# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

ifndef MACHINE
  UNAME_M=$(shell uname -m)
  ifeq ($(UNAME_M), i386)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), i686)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), x86_64)
    MACHINE=x64
  endif
  ifeq ($(UNAME_M), amd64)
    MACHINE=x64
  endif
endif
GCC_CMD=gcc
PLATFORM=cygwin
ifeq ($(MACHINE), x64)
  GCC_CMD=gcc -m64
  PLATFORM=cygwin64
endif
ifeq ($(MACHINE), x86)
  GCC_CMD=gcc -m32
  PLATFORM=cygwin32
endif
# Output directory
ABINAME = cygwin$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g 
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)d
SLIBS_DIR=slib/$(PLATFORM)d
else
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG 
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)
SLIBS_DIR=slib/$(PLATFORM)
endif

CP=cp

INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libZnk \


include Makefile_version.mak

BASENAME0=mkfgen
EXE_FILE0=$O/mkfgen.exe
OBJS0=\
	$O/Mkf_android.o \
	$O/Mkf_include.o \
	$O/Mkf_install.o \
	$O/Mkf_lib_depend.o \
	$O/Mkf_seek.o \
	$O/Mkf_src_depend.o \
	$O/Mkf_sset.o \
	$O/mkfgen.o \

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
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	@echo $(LINKER) -o $(EXE_FILE0) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.a -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE0) $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk.a -lpthread -ldl -lstdc++ 


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
$O/%.o: $S/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O/%.o: $S/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	mkdir -p ../../mkfsys/template 
	for tgt in template/*.mak ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../mkfsys/template/ ; fi ; done
	for tgt in template/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../mkfsys/template/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0)
	mkdir -p ../../mkfsys/$(PLATFORM) 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../mkfsys/$(PLATFORM)/ ; fi ; done
	@for tgt in $(RUNTIME_FILES) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../mkfsys/$(PLATFORM)/ ; fi ; done

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rm -r $O/ 

# Src and Headers Dependency
mkfgen.o: Mkf_install.h Mkf_include.h Mkf_seek.h Mkf_src_depend.h Mkf_lib_depend.h Mkf_android.h Mkf_sset.h
Mkf_android.o: Mkf_android.h Mkf_seek.h Mkf_lib_depend.h Mkf_include.h
Mkf_include.o: Mkf_include.h
Mkf_install.o: Mkf_install.h
Mkf_lib_depend.o: Mkf_lib_depend.h
Mkf_seek.o: Mkf_seek.h
Mkf_src_depend.o: Mkf_src_depend.h Mkf_seek.h
Mkf_sset.o: Mkf_sset.h
