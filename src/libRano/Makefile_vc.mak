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


# We cannot use variables in include of nmake.
include Makefile_version.mak

BASENAME0=Rano
DLIB_NAME0=Rano-$(DL_VER).dll
DLIB_FILE0=$O\$(DLIB_NAME0)
ILIB_FILE0=$O\Rano-$(DL_VER).imp.lib
SLIB_FILE0=$O\Rano.lib
OBJS0=\
	$O\Rano_cgi_util.obj \
	$O\Rano_conf_util.obj \
	$O\Rano_file_info.obj \
	$O\Rano_hash.obj \
	$O\Rano_html_ui.obj \
	$O\Rano_htp_boy.obj \
	$O\Rano_htp_modifier.obj \
	$O\Rano_log.obj \
	$O\Rano_module.obj \
	$O\Rano_myf.obj \
	$O\Rano_parent_proxy.obj \
	$O\Rano_post.obj \
	$O\Rano_sset.obj \
	$O\Rano_txt_filter.obj \
	$O\dll_main.obj \

SUB_LIBS=\

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
all: $O $(DLIB_FILE0) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(SLIB_FILE0): $(OBJS0)
	LIB /NOLOGO /OUT:$(SLIB_FILE0) $(OBJS0) $(SUB_LIBS)

gsl.myf: $(SLIB_FILE0)
	@if exist $(MKFSYS_DIR)\gslconv.exe $(MKFSYS_DIR)\gslconv.exe -g gsl.myf $(SLIB_FILE0) $(MACHINE)

gsl.def: gsl.myf
	@if exist $(MKFSYS_DIR)\gslconv.exe $(MKFSYS_DIR)\gslconv.exe -d gsl.myf gsl.def

$(DLIB_FILE0): $(OBJS0) $(SLIB_FILE0) gsl.def
	$(LINKER) /DLL /OUT:$(DLIB_FILE0) /IMPLIB:$(ILIB_FILE0) $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib ws2_32.lib  /DEF:gsl.def


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
	del /Q $O\ 

# Src and Headers Dependency
dll_main.obj:
Rano_cgi_util.obj: Rano_cgi_util.h Rano_type.h Rano_log.h Rano_post.h
Rano_conf_util.obj: Rano_conf_util.h
Rano_file_info.obj: Rano_file_info.h
Rano_hash.obj: Rano_hash.h
Rano_html_ui.obj:
Rano_htp_boy.obj: Rano_htp_boy.h Rano_log.h Rano_post.h
Rano_htp_modifier.obj: Rano_htp_modifier.h
Rano_log.obj: Rano_log.h
Rano_module.obj: Rano_module.h Rano_module_ary.h Rano_log.h Rano_myf.h Rano_txt_filter.h Rano_plugin_dev.h Rano_parent_proxy.h Rano_file_info.h
Rano_myf.obj: Rano_myf.h Rano_log.h
Rano_parent_proxy.obj: Rano_parent_proxy.h Rano_log.h
Rano_post.obj: Rano_post.h
Rano_sset.obj: Rano_sset.h
Rano_txt_filter.obj: Rano_txt_filter.h
