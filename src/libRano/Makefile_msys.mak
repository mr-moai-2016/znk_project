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


include Makefile_version.mak

BASENAME0=Rano
DLIB_NAME0=Rano-$(DL_VER).dll
DLIB_FILE0=$O/$(DLIB_NAME0)
ILIB_FILE0=$O/libRano-$(DL_VER).dll.a
SLIB_FILE0=$O/libRano.a
OBJS0=\
	$O/Rano_cgi_util.o \
	$O/Rano_conf_util.o \
	$O/Rano_dir_util.o \
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
	$O/Rano_vtag_util.o \
	$O/tls_module/tls_module.o \
	$O/dll_main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

PRODUCT_DLIBS= \
	__mkg_sentinel_target__ \
	$(DLIB_FILE0) \

PRODUCT_SLIBS= \
	__mkg_sentinel_target__ \
	$(SLIB_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \



# Entry rule.
all: $O/tls_module $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O/tls_module:
	mkdir -p $O/tls_module

$O:
	mkdir -p $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	if test -e $(SLIB_FILE0) ; then rm -f $(SLIB_FILE0); fi
	@echo ar cru $(SLIB_FILE0) {[objs]} $(SUB_OBJS_ECHO)
	@     ar cru $(SLIB_FILE0) $(OBJS0) $(SUB_OBJS)
	ranlib $(SLIB_FILE0)

gsl.myf: $(SLIB_FILE0)
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE) ; fi

gsl.def: gsl.myf
	if test -e $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe ; then $(MKFSYS_DIR)/$(PLATFORM)/gslconv.exe -d gsl.myf gsl.def ; fi

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0) gsl.def
	@echo gcc -static-libgcc -g -Wl,--disable-stdcall-fixup,--kill-at -shared -o $(DLIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a -lws2_32  gsl.def
	@     gcc -static-libgcc -g -Wl,--disable-stdcall-fixup,--kill-at -shared -o $(DLIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a -lws2_32  gsl.def
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
$O/tls_module/%.o: $S/tls_module/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O/tls_module/%.o: $S/tls_module/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

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
	rm -rf $O/ 

# Src and Headers Dependency
Rano_cgi_util.o: Rano_cgi_util.h Rano_type.h Rano_log.h Rano_post.h
Rano_conf_util.o: Rano_conf_util.h
Rano_dir_util.o: Rano_dir_util.h Rano_file_info.h
Rano_file_info.o: Rano_file_info.h
Rano_hash.o: Rano_hash.h
Rano_htp_boy.o: Rano_htp_boy.h Rano_log.h Rano_post.h tls_module/tls_module.h
Rano_htp_modifier.o: Rano_htp_modifier.h
Rano_log.o: Rano_log.h
Rano_module.o: Rano_module.h Rano_module_ary.h Rano_log.h Rano_myf.h Rano_txt_filter.h Rano_plugin_dev.h Rano_parent_proxy.h Rano_file_info.h
Rano_myf.o: Rano_myf.h Rano_log.h
Rano_parent_proxy.o: Rano_parent_proxy.h Rano_log.h
Rano_post.o: Rano_post.h
Rano_sset.o: Rano_sset.h
Rano_txt_filter.o: Rano_txt_filter.h
Rano_vtag_util.o: Rano_vtag_util.h
tls_module/tls_module.o: tls_module/tls.h
