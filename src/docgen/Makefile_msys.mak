# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

GCC_CMD=gcc
PLATFORM=win32
ifeq ($(MACHINE), x64)
  PLATFORM=win64
else
  MACHINE=x86
  PLATFORM=win32
endif
# Output directory
ABINAME=mingw$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g 
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)_mingwd
SLIBS_DIR=slib/$(PLATFORM)_mingwd
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

BASENAME0=docgen
EXE_FILE0=$O/docgen.cgi
OBJS0=\
	$O/cgi_helper.o \
	$O/docgen.o \
	$O/Doc_html.o \
	$O/Doc_source.o \
	$O/Doc_util.o \
	$O/main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Rano-$(DL_VER).dll \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	@echo $(LINKER) -o $(EXE_FILE0)  {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a
	@     $(LINKER) -o $(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a


##
# Pattern rule.
#
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\'.
# Note that we have to duplicate '\' only before special charactor(% etc) in the left of ':'.
#
# For example 1 :
#   $O\\mydir\\%.o: $S\%.c        .... NG
#   $O\mydir\\%.o:  $S\%.c        .... OK
# For example 2 :
#   $O\\mydir\%.o:  $S\mydir\%.c  .... NG
#   $O\mydir\\%.o:  $S\mydir\%.c  .... OK
# In the case of example 2, we can write more simply :
#   $O\\%.o: $S\%.c               .... OK
#   (Because '%' is wildcard and it indicates patical path 'mydir\filename_base' recursively )
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
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/docgen 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/docgen/templates 
	for tgt in docgen.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/docgen/ ; fi ; done
	for tgt in templates/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/docgen/templates/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0)
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/docgen 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/docgen/ ; fi ; done

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
$O/cgi_helper.o: cgi_helper.h
$O/docgen.o: Doc_html.h
$O/Doc_html.o: Doc_html.h Doc_util.h
$O/Doc_source.o: Doc_source.h Doc_util.h
$O/Doc_util.o: Doc_util.h
$O/main.o: cgi_helper.h Doc_html.h Doc_source.h
