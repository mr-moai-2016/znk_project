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

BASENAME0=Moai
DLIB_NAME0=Moai-$(DL_VER).dll
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\Moai-$(DL_VER).imp.lib
SLIB_FILE0=$O\Moai.lib
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
	$O\dll_main.obj \

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
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Rano-$(DL_VER).dll \



# Entry rule.
all: $O $(DLIB_FILE0) $(SLIB_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	@echo LIB /NOLOGO /OUT:$(SLIB_FILE0) {[objs]} $(SUB_LIBS)
	@     LIB /NOLOGO /OUT:$(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe $(MKFSYS_DIR)\$(PLATFORM)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0) gsl.def
	@echo $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib ws2_32.lib  /DEF:gsl.def
	@     $(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib ws2_32.lib  /DEF:gsl.def


# Suffix rule.
{$S}.c{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<

{$S}.cpp{$O}.obj:
	$(COMPILER) -I$S $(INCLUDE_FLAG) -Fo$@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:

# Install exec rule.
install_exec:
	@for %%a in ( $(RUNTIME_FILES) ) do @if exist "%%a" @$(CP) /F "%%a" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install dlib rule.
install_dlib: $(DLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(DLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(DLIBS_DIR) 
	@if exist "$(DLIB_FILE0)" @$(CP) /F "$(DLIB_FILE0)" $(MY_LIBS_ROOT)\$(DLIBS_DIR)\ $(CP_END)

# Install slib rule.
install_slib: $(SLIB_FILE0)
	@if not exist $(MY_LIBS_ROOT)\$(SLIBS_DIR) @mkdir $(MY_LIBS_ROOT)\$(SLIBS_DIR) 
	@if exist "$(SLIB_FILE0)" @$(CP) /F "$(SLIB_FILE0)" $(MY_LIBS_ROOT)\$(SLIBS_DIR)\ $(CP_END)

# Install rule.
install: all install_slib install_dlib 


# Clean rule.
clean:
	rmdir /S /Q $O\ 

# Src and Headers Dependency
$O\Moai_cgi.obj: Moai_cgi.h Moai_io_base.h Moai_server_info.h Moai_connection.h
$O\Moai_cgi_manager.obj: Moai_cgi.h
$O\Moai_connection.obj: Moai_connection.h
$O\Moai_context.obj: Moai_context.h
$O\Moai_fdset.obj: Moai_fdset.h Moai_connection.h
$O\Moai_http.obj: Moai_http.h Moai_io_base.h Moai_info.h
$O\Moai_info.obj: Moai_info.h Moai_server_info.h
$O\Moai_io_base.obj: Moai_io_base.h Moai_connection.h
$O\Moai_post.obj: Moai_post.h Moai_io_base.h Moai_server_info.h
$O\Moai_server.obj: Moai_server.h Moai_post.h Moai_context.h Moai_io_base.h Moai_connection.h Moai_info.h Moai_fdset.h Moai_http.h Moai_server_info.h Moai_web_server.h Moai_cgi.h
$O\Moai_server_info.obj: Moai_server_info.h
$O\Moai_web_server.obj: Moai_context.h Moai_io_base.h Moai_connection.h Moai_info.h Moai_fdset.h Moai_post.h Moai_server_info.h Moai_cgi.h
