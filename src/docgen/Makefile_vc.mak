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

BASENAME0=docgen
EXE_FILE0=$O\docgen.cgi
OBJS0=\
	$O\cgi_helper.obj \
	$O\docgen.obj \
	$O\Doc_html.obj \
	$O\Doc_source.obj \
	$O\Doc_util.obj \
	$O\main.obj \

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
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	@echo $(LINKER) /OUT:$(EXE_FILE0)  {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib
	@     $(LINKER) /OUT:$(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib


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
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen\templates @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen\templates 
	@if exist "docgen.myf" @$(CP) /F "docgen.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen\ $(CP_END)
	@if exist "templates\*.html" @$(CP) /F "templates\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen\templates\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\docgen\ $(CP_END)

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
$O\cgi_helper.obj: cgi_helper.h
$O\docgen.obj: Doc_html.h
$O\Doc_html.obj: Doc_html.h Doc_util.h
$O\Doc_source.obj: Doc_source.h Doc_util.h
$O\Doc_util.obj: Doc_util.h
$O\main.obj: cgi_helper.h Doc_html.h Doc_source.h
