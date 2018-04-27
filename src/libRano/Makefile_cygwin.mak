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

BASENAME0=Rano
DLIB_NAME0=cygRano.dll
DLIB_FILE0=$O/$(DLIB_NAME0)
ILIB_FILE0=$O/cygRano.dll
SLIB_FILE0=$O/cygRano.a
OBJS0=\
	$O/Rano_cgi_util.o \
	$O/Rano_conf_util.o \
	$O/Rano_file_info.o \
	$O/Rano_hash.o \
	$O/Rano_html_ui.o \
	$O/Rano_htp_boy.o \
	$O/Rano_htp_modifier.o \
	$O/Rano_log.o \
	$O/Rano_module.o \
	$O/Rano_myf.o \
	$O/Rano_parent_proxy.o \
	$O/Rano_post.o \
	$O/Rano_sset.o \
	$O/Rano_txt_filter.o \
	$O/dll_main.o \

SUB_LIBS=\

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE0) \

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/cygZnk.dll \



# Entry rule.
all: $O $(DLIB_FILE0) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if test -e $(SLIB_FILE0) ; then rm -f $(SLIB_FILE0) ; fi
	ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)
	ranlib $(SLIB_FILE0)

gsl.myf: $(SLIB_FILE0)
	if test -e $(MKFSYS_DIR)/gslconv.exe ; then $(MKFSYS_DIR)/gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE) ; fi

gsl.def: gsl.myf
	if test -e $(MKFSYS_DIR)/gslconv.exe ; then $(MKFSYS_DIR)/gslconv.exe -d gsl.myf gsl.def ; fi

$(DLIB_FILE0): $(OBJS0)
	if test -e $(DLIB_FILE0) ; then rm -f $(DLIB_FILE0); fi
	gcc -shared -o $(DLIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk.dll -lpthread -ldl -lstdc++  -lc


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
	@for tgt in $(RUNTIME_FILES) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" $(MY_LIBS_ROOT)/$(DLIBS_DIR)/ ; fi ; done

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	mkdir -p $(MY_LIBS_ROOT)/$(DLIBS_DIR) 
	for tgt in $(DLIB_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" $(MY_LIBS_ROOT)/$(DLIBS_DIR)/ ; fi ; done

# Install slib rule.
install_slib: $(SLIB_FILE0)
	mkdir -p $(MY_LIBS_ROOT)/$(SLIBS_DIR) 
	for tgt in $(SLIB_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" $(MY_LIBS_ROOT)/$(SLIBS_DIR)/ ; fi ; done

# Install rule.
install: all install_slib install_dlib 


# Clean rule.
clean:
	rm -r $O/ 

# Src and Headers Dependency
dll_main.o:
Rano_cgi_util.o: Rano_cgi_util.h Rano_type.h Rano_log.h Rano_post.h
Rano_conf_util.o: Rano_conf_util.h
Rano_file_info.o: Rano_file_info.h
Rano_hash.o: Rano_hash.h
Rano_html_ui.o:
Rano_htp_boy.o: Rano_htp_boy.h Rano_log.h Rano_post.h
Rano_htp_modifier.o: Rano_htp_modifier.h
Rano_log.o: Rano_log.h
Rano_module.o: Rano_module.h Rano_module_ary.h Rano_log.h Rano_myf.h Rano_txt_filter.h Rano_plugin_dev.h Rano_parent_proxy.h Rano_file_info.h
Rano_myf.o: Rano_myf.h Rano_log.h
Rano_parent_proxy.o: Rano_parent_proxy.h Rano_log.h
Rano_post.o: Rano_post.h
Rano_sset.o: Rano_sset.h
Rano_txt_filter.o: Rano_txt_filter.h
