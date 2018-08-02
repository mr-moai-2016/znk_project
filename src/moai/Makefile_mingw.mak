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
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=moai
EXE_FILE0=$O\moai.exe
OBJS0=\
	$O\Moai_cgi.o \
	$O\Moai_cgi_manager.o \
	$O\Moai_connection.o \
	$O\Moai_context.o \
	$O\Moai_fdset.o \
	$O\Moai_http.o \
	$O\Moai_info.o \
	$O\Moai_io_base.o \
	$O\Moai_post.o \
	$O\Moai_server.o \
	$O\Moai_server_info.o \
	$O\Moai_web_server.o \
	$O\main.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

RES_FILE0=$O\moai.res
RC_FILE0 =$S\moai.rc
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Rano-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/libtls-17.dll \



# Entry rule.
all: $O $(EXE_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) $(RES_FILE0)
	@echo $(LINKER) -o $(EXE_FILE0) $(RES_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 
	@     $(LINKER) -o $(EXE_FILE0) $(RES_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 


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
$(RES_FILE0): $(RC_FILE0)
	windres --input-format=rc --output-format=coff -o$(RES_FILE0) $(RC_FILE0)


# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM) @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM) 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\default\filters @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\default\filters 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\common @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\common 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\moai2.0 @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\moai2.0 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\imgs @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\imgs 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\public @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\public 
	@if exist "config.myf" @$(CP) /F "config.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "hosts.myf" @$(CP) /F "hosts.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "config_cgi.myf" @$(CP) /F "config_cgi.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "target.myf" @$(CP) /F "target.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "moai_without_proxy.bat" @$(CP) /F "moai_without_proxy.bat" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "moai_without_console.bat" @$(CP) /F "moai_without_console.bat" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "default\filters\*.myf" @$(CP) /F "default\filters\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\default\filters\ $(CP_END)
	@if exist "doc_root\*.html" @$(CP) /F "doc_root\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\ $(CP_END)
	@if exist "doc_root\*.css" @$(CP) /F "doc_root\*.css" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\ $(CP_END)
	@if exist "doc_root\*.png" @$(CP) /F "doc_root\*.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\ $(CP_END)
	@if exist "doc_root\*.jpg" @$(CP) /F "doc_root\*.jpg" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\ $(CP_END)
	@if exist "doc_root\*.gif" @$(CP) /F "doc_root\*.gif" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\ $(CP_END)
	@if exist "doc_root\*.js" @$(CP) /F "doc_root\*.js" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\ $(CP_END)
	@if exist "doc_root\common\*.html" @$(CP) /F "doc_root\common\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\common\ $(CP_END)
	@if exist "doc_root\moai2.0\*.html" @$(CP) /F "doc_root\moai2.0\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\moai2.0\ $(CP_END)
	@if exist "doc_root\imgs\*.png" @$(CP) /F "doc_root\imgs\*.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\imgs\ $(CP_END)
	@if exist "doc_root\imgs\*.jpg" @$(CP) /F "doc_root\imgs\*.jpg" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\imgs\ $(CP_END)
	@if exist "doc_root\imgs\*.gif" @$(CP) /F "doc_root\imgs\*.gif" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\imgs\ $(CP_END)
	@if exist "doc_root\public\moai.png" @$(CP) /F "doc_root\public\moai.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\doc_root\public\ $(CP_END)
	@if exist "moai_for_android.sh" @$(CP) /F "moai_for_android.sh" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "moai_for_linux.sh" @$(CP) /F "moai_for_linux.sh" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "vtag" @$(CP) /F "vtag" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@if exist "cert.pem" @$(CP) /F "cert.pem" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM) @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM) 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" ..\..\moai-v$(REL_VER)-$(PLATFORM)\ $(CP_END)

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
