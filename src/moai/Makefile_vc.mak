# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
!IFNDEF MKFSYS_DIR
MKFSYS_DIR=$(MY_LIBS_ROOT)\..\mkfsys
!ENDIF

!IF "$(MACHINE)" == "x64"
VC_MACHINE=AMD64
PLATFORM=win64
!ELSE
MACHINE=x86
VC_MACHINE=X86
PLATFORM=win32
!ENDIF
# Output directory
ABINAME=vc$(MACHINE)$(DEBUG)
O = .\out_dir\$(ABINAME)

!IF "$(DEBUG)" == "d"
COMPILER=cl -nologo -EHsc /GR /W4 /MTd /Z7 /D_DEBUG /RTC1 
LINKER=LINK /MACHINE:$(VC_MACHINE) /DEBUG /INCREMENTAL:NO /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)_vcd
SLIBS_DIR=slib\$(PLATFORM)_vcd
!ELSE
COMPILER=cl -nologo -EHsc /GR /W4 /MT /O2 /DNDEBUG 
LINKER=LINK /MACHINE:$(VC_MACHINE) /SUBSYSTEM:console
DLIBS_DIR=dlib\$(PLATFORM)
SLIBS_DIR=slib\$(PLATFORM)
!ENDIF

CP=xcopy /H /C /Y

INCLUDE_FLAG =  \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \


# We cannot use variables in include of nmake.
include Makefile_version.mak

BASENAME0=moai
EXE_FILE0=$O\moai.exe
OBJS0=\
	$O\Moai_cgi.obj \
	$O\Moai_cgi_manager.obj \
	$O\Moai_connection.obj \
	$O\Moai_context.obj \
	$O\Moai_fdset.obj \
	$O\Moai_http.obj \
	$O\Moai_info.obj \
	$O\Moai_io_base.obj \
	$O\Moai_post.obj \
	$O\Moai_server.obj \
	$O\Moai_server_info.obj \
	$O\Moai_web_server.obj \
	$O\main.obj \

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
	@echo $(LINKER) /OUT:$(EXE_FILE0) $(RES_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib ws2_32.lib 
	@     $(LINKER) /OUT:$(EXE_FILE0) $(RES_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib ws2_32.lib 


# Suffix rule.
{$S}.c{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.cpp{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<


# Rc rule.
$(RES_FILE0): $(RC_FILE0)
	rc /fo$(RES_FILE0) $(RC_FILE0)


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
main.obj: Moai_server.h
Moai_cgi.obj: Moai_cgi.h Moai_io_base.h Moai_server_info.h Moai_connection.h
Moai_cgi_manager.obj: Moai_cgi.h
Moai_connection.obj: Moai_connection.h
Moai_context.obj: Moai_context.h
Moai_fdset.obj: Moai_fdset.h Moai_connection.h
Moai_http.obj: Moai_http.h Moai_io_base.h Moai_info.h
Moai_info.obj: Moai_info.h Moai_server_info.h
Moai_io_base.obj: Moai_io_base.h Moai_connection.h
Moai_post.obj: Moai_post.h Moai_io_base.h Moai_server_info.h
Moai_server.obj: Moai_server.h Moai_post.h Moai_context.h Moai_io_base.h Moai_connection.h Moai_info.h Moai_fdset.h Moai_http.h Moai_server_info.h Moai_web_server.h Moai_cgi.h
Moai_server_info.obj: Moai_server_info.h
Moai_web_server.obj: Moai_context.h Moai_io_base.h Moai_connection.h Moai_info.h Moai_fdset.h Moai_post.h Moai_server_info.h Moai_cgi.h
