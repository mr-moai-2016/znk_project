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
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=hello
EXE_FILE0=$O/hello.cgi
OBJS0=\
	$O/cgi_util.o \
	$O/hello.o \

BASENAME1=evar1
EXE_FILE1=$O/evar1.cgi
OBJS1=\
	$O/cgi_util.o \
	$O/evar1.o \

BASENAME2=evar2
EXE_FILE2=$O/evar2.cgi
OBJS2=\
	$O/cgi_util.o \
	$O/evar2.o \

BASENAME3=query_string1
EXE_FILE3=$O/query_string1.cgi
OBJS3=\
	$O/cgi_util.o \
	$O/query_string1.o \

BASENAME4=query_string2
EXE_FILE4=$O/query_string2.cgi
OBJS4=\
	$O/cgi_util.o \
	$O/query_string2.o \

BASENAME5=post1
EXE_FILE5=$O/post1.cgi
OBJS5=\
	$O/cgi_util.o \
	$O/post1.o \

BASENAME6=post2
EXE_FILE6=$O/post2.cgi
OBJS6=\
	$O/cgi_util.o \
	$O/post2.o \

BASENAME7=transfer_chunked
EXE_FILE7=$O/transfer_chunked.cgi
OBJS7=\
	$O/cgi_util.o \
	$O/transfer_chunked.o \

BASENAME8=hello_template
EXE_FILE8=$O/hello_template.cgi
OBJS8=\
	$O/cgi_util.o \
	$O/hello_template.o \

BASENAME9=progress
EXE_FILE9=$O/progress.cgi
OBJS9=\
	$O/cgi_util.o \
	$O/progress.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE1) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE2) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE3) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE4) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE5) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE6) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE7) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE8) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE9) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/cygZnk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/cygRano-$(DL_VER).dll \



# Entry rule.
all: $O $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(EXE_FILE4) $(EXE_FILE5) $(EXE_FILE6) $(EXE_FILE7) $(EXE_FILE8) $(EXE_FILE9) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	@echo $(LINKER) -o $(EXE_FILE0) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE0) $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE1): $(OBJS1)
	@echo $(LINKER) -o $(EXE_FILE1) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE1) $(OBJS1) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE2): $(OBJS2)
	@echo $(LINKER) -o $(EXE_FILE2) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE2) $(OBJS2) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE3): $(OBJS3)
	@echo $(LINKER) -o $(EXE_FILE3) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE3) $(OBJS3) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE4): $(OBJS4)
	@echo $(LINKER) -o $(EXE_FILE4) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE4) $(OBJS4) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE5): $(OBJS5)
	@echo $(LINKER) -o $(EXE_FILE5) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE5) $(OBJS5) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE6): $(OBJS6)
	@echo $(LINKER) -o $(EXE_FILE6) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE6) $(OBJS6) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE7): $(OBJS7)
	@echo $(LINKER) -o $(EXE_FILE7) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE7) $(OBJS7) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE8): $(OBJS8)
	@echo $(LINKER) -o $(EXE_FILE8) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE8) $(OBJS8) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE9): $(OBJS9)
	@echo $(LINKER) -o $(EXE_FILE9) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE9) $(OBJS9) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 


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
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/publicbox 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected 
	for tgt in *.c ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/publicbox/ ; fi ; done
	for tgt in *.cpp ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/publicbox/ ; fi ; done
	for tgt in *.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/publicbox/ ; fi ; done
	for tgt in *.js ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/publicbox/ ; fi ; done
	for tgt in *.go ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/publicbox/ ; fi ; done
	for tgt in *.go ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(EXE_FILE4) $(EXE_FILE5) $(EXE_FILE6) $(EXE_FILE7) $(EXE_FILE8) $(EXE_FILE9)
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE1) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE2) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE3) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE4) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE5) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE6) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE7) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE8) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done
	for tgt in $(EXE_FILE9) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/cgi_developers/protected/ ; fi ; done

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rm -rf $O/ 

# Src and Headers Dependency
cgi_util.o: cgi_util.h
evar1.o: cgi_util.h
hello.o: cgi_util.h
post1.o: cgi_util.h
query_string1.o: cgi_util.h
transfer_chunked.o: cgi_util.h
