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

BASENAME0=5ch
DLIB_NAME0=cyg5ch.dll
DLIB_FILE0=$O/$(DLIB_NAME0)
ILIB_FILE0=$O/cyg5ch.dll
SLIB_FILE0=$O/cyg5ch.a
OBJS0=\
	$O/init.o \
	$O/main.o \
	$O/dll_main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/cygZnk-$(DL_VER).dll \



# Entry rule.
all: $O $(DLIB_FILE0) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if test -e $(SLIB_FILE0) ; then rm -f $(SLIB_FILE0) ; fi
	@echo ar cru $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)
	ranlib $(SLIB_FILE0)

gsl.myf: $(SLIB_FILE0)
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE) ; fi

gsl.def: gsl.myf
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -d gsl.myf gsl.def ; fi

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0)
	if test -e $(DLIB_FILE0) ; then rm -f $(DLIB_FILE0); fi
	@echo gcc -shared -o $(DLIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll -lpthread -ldl -lstdc++  -lc
	@     gcc -shared -o $(DLIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll -lpthread -ldl -lstdc++  -lc


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

# Install exec rule.
install_exec:
	@for tgt in $(RUNTIME_FILES) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/plugins/ ; fi ; done

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/plugins 
	for tgt in $(DLIB_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/plugins/ ; fi ; done

# Install slib rule.
install_slib:

# Install rule.
install: all install_dlib 


# Clean rule.
clean:
	rm -rf $O/ 

# Src and Headers Dependency
init.o: Moai_plugin_dev.h
main.o: Moai_plugin_dev.h
