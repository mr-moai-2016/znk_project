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

BASENAME0=moai
EXE_FILE0=$O/moai.exe
OBJS0=\
	$O/Moai_cgi.o \
	$O/Moai_cgi_manager.o \
	$O/Moai_connection.o \
	$O/Moai_context.o \
	$O/Moai_fdset.o \
	$O/Moai_http.o \
	$O/Moai_info.o \
	$O/Moai_io_base.o \
	$O/Moai_post.o \
	$O/Moai_server.o \
	$O/Moai_server_info.o \
	$O/Moai_web_server.o \
	$O/main.o \

SUB_LIBS=\

RES_FILE0=$O/moai.res
RC_FILE0 =$S/moai.rc
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
$(EXE_FILE0): $(OBJS0) $(RES_FILE0)
	$(LINKER) -o $(EXE_FILE0) $(RES_FILE0) $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 


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
$(RES_FILE0): $(RC_FILE0)
	windres --input-format=rc --output-format=coff -o$(RES_FILE0) $(RC_FILE0)


# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM) 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/default/filters 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/common 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/moai1.1 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/moai2.0 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/imgs 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/public 
	for tgt in config.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done
	for tgt in hosts.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done
	for tgt in config_cgi.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done
	for tgt in target.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done
	for tgt in moai_without_proxy.bat ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done
	for tgt in moai_without_console.bat ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done
	for tgt in default/filters/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/default/filters/ ; fi ; done
	for tgt in doc_root/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/ ; fi ; done
	for tgt in doc_root/*.css ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/ ; fi ; done
	for tgt in doc_root/*.png ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/ ; fi ; done
	for tgt in doc_root/*.jpg ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/ ; fi ; done
	for tgt in doc_root/*.gif ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/ ; fi ; done
	for tgt in doc_root/*.js ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/ ; fi ; done
	for tgt in doc_root/common/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/common/ ; fi ; done
	for tgt in doc_root/moai1.1/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/moai1.1/ ; fi ; done
	for tgt in doc_root/moai2.0/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/moai2.0/ ; fi ; done
	for tgt in doc_root/imgs/*.png ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/imgs/ ; fi ; done
	for tgt in doc_root/imgs/*.jpg ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/imgs/ ; fi ; done
	for tgt in doc_root/imgs/*.gif ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/imgs/ ; fi ; done
	for tgt in doc_root/public/moai.png ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/doc_root/public/ ; fi ; done
	for tgt in moai_for_android.sh ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0)
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM) 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done
	@for tgt in $(RUNTIME_FILES) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/ ; fi ; done

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
main.o: Moai_server.h
Moai_cgi.o: Moai_cgi.h Moai_io_base.h Moai_server_info.h Moai_connection.h
Moai_cgi_manager.o: Moai_cgi.h
Moai_connection.o: Moai_connection.h
Moai_context.o: Moai_context.h
Moai_fdset.o: Moai_fdset.h Moai_connection.h
Moai_http.o: Moai_http.h Moai_io_base.h Moai_info.h
Moai_info.o: Moai_info.h Moai_server_info.h
Moai_io_base.o: Moai_io_base.h Moai_connection.h
Moai_post.o: Moai_post.h Moai_io_base.h Moai_server_info.h
Moai_server.o: Moai_server.h Moai_post.h Moai_context.h Moai_io_base.h Moai_connection.h Moai_info.h Moai_fdset.h Moai_http.h Moai_server_info.h Moai_web_server.h Moai_cgi.h
Moai_server_info.o: Moai_server_info.h
Moai_web_server.o: Moai_context.h Moai_io_base.h Moai_connection.h Moai_info.h Moai_fdset.h Moai_post.h Moai_server_info.h Moai_cgi.h
