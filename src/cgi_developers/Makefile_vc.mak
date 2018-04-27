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

BASENAME0=hello
EXE_FILE0=$O\hello.cgi
OBJS0=\
	$O\cgi_util.obj \
	$O\hello.obj \

BASENAME1=evar1
EXE_FILE1=$O\evar1.cgi
OBJS1=\
	$O\cgi_util.obj \
	$O\evar1.obj \

BASENAME2=evar2
EXE_FILE2=$O\evar2.cgi
OBJS2=\
	$O\cgi_util.obj \
	$O\evar2.obj \

BASENAME3=query_string1
EXE_FILE3=$O\query_string1.cgi
OBJS3=\
	$O\cgi_util.obj \
	$O\query_string1.obj \

BASENAME4=query_string2
EXE_FILE4=$O\query_string2.cgi
OBJS4=\
	$O\cgi_util.obj \
	$O\query_string2.obj \

BASENAME5=post1
EXE_FILE5=$O\post1.cgi
OBJS5=\
	$O\cgi_util.obj \
	$O\post1.obj \

BASENAME6=post2
EXE_FILE6=$O\post2.cgi
OBJS6=\
	$O\cgi_util.obj \
	$O\post2.obj \

BASENAME7=transfer_chunked
EXE_FILE7=$O\transfer_chunked.cgi
OBJS7=\
	$O\cgi_util.obj \
	$O\transfer_chunked.obj \

BASENAME8=hello_template
EXE_FILE8=$O\hello_template.cgi
OBJS8=\
	$O\cgi_util.obj \
	$O\hello_template.obj \

BASENAME9=progress
EXE_FILE9=$O\progress.cgi
OBJS9=\
	$O\cgi_util.obj \
	$O\progress.obj \

SUB_LIBS=\

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
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Rano-$(DL_VER).dll \



# Entry rule.
all: $O $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(EXE_FILE4) $(EXE_FILE5) $(EXE_FILE6) $(EXE_FILE7) $(EXE_FILE8) $(EXE_FILE9) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	$(LINKER) /OUT:$(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE1): $(OBJS1) 
	$(LINKER) /OUT:$(EXE_FILE1)  $(OBJS1) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE2): $(OBJS2) 
	$(LINKER) /OUT:$(EXE_FILE2)  $(OBJS2) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE3): $(OBJS3) 
	$(LINKER) /OUT:$(EXE_FILE3)  $(OBJS3) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE4): $(OBJS4) 
	$(LINKER) /OUT:$(EXE_FILE4)  $(OBJS4) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE5): $(OBJS5) 
	$(LINKER) /OUT:$(EXE_FILE5)  $(OBJS5) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE6): $(OBJS6) 
	$(LINKER) /OUT:$(EXE_FILE6)  $(OBJS6) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE7): $(OBJS7) 
	$(LINKER) /OUT:$(EXE_FILE7)  $(OBJS7) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE8): $(OBJS8) 
	$(LINKER) /OUT:$(EXE_FILE8)  $(OBJS8) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib

$(EXE_FILE9): $(OBJS9) 
	$(LINKER) /OUT:$(EXE_FILE9)  $(OBJS9) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib


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
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox 
	@if exist "*.c" @$(CP) /F "*.c" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)
	@if exist "*.cpp" @$(CP) /F "*.cpp" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)
	@if exist "*.html" @$(CP) /F "*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)
	@if exist "*.js" @$(CP) /F "*.js" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\publicbox\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2) $(EXE_FILE3) $(EXE_FILE4) $(EXE_FILE5) $(EXE_FILE6) $(EXE_FILE7) $(EXE_FILE8) $(EXE_FILE9)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE1)" @$(CP) /F "$(EXE_FILE1)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE2)" @$(CP) /F "$(EXE_FILE2)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE3)" @$(CP) /F "$(EXE_FILE3)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE4)" @$(CP) /F "$(EXE_FILE4)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE5)" @$(CP) /F "$(EXE_FILE5)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE6)" @$(CP) /F "$(EXE_FILE6)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE7)" @$(CP) /F "$(EXE_FILE7)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE8)" @$(CP) /F "$(EXE_FILE8)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)
	@if exist "$(EXE_FILE9)" @$(CP) /F "$(EXE_FILE9)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\cgi_developers\protected\ $(CP_END)

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	del /Q $O\ 

# Src and Headers Dependency
cgi_util.obj: cgi_util.h
evar1.obj: cgi_util.h
evar2.obj:
hello.obj: cgi_util.h
hello_template.obj:
post1.obj: cgi_util.h
post2.obj:
progress.obj:
query_string1.obj: cgi_util.h
query_string2.obj:
transfer_chunked.obj: cgi_util.h
