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

BASENAME0=custom_boy
EXE_FILE0=$O\custom_boy.cgi
OBJS0=\
	$O\CB_config.obj \
	$O\CB_custom_automatic.obj \
	$O\CB_custom_cookie.obj \
	$O\CB_custom_feature.obj \
	$O\CB_custom_postvars.obj \
	$O\CB_fgp_info.obj \
	$O\CB_finger.obj \
	$O\CB_snippet.obj \
	$O\CB_ua_conf.obj \
	$O\CB_ua_info.obj \
	$O\CB_vars_base.obj \
	$O\CB_virtualizer.obj \
	$O\CB_wgt_prim.obj \
	$O\CB_wgt_prim_db.obj \
	$O\main.obj \

SUB_LIBS=\

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
	$(LINKER) /OUT:$(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib ws2_32.lib 


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
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA 
	@if exist "custom_boy.myf" @$(CP) /F "custom_boy.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\ $(CP_END)
	@if exist "templates\*.html" @$(CP) /F "templates\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\templates\ $(CP_END)
	@if exist "publicbox\*" @$(CP) /F "publicbox\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\publicbox\ $(CP_END)
	@if exist "RE\*.png" @$(CP) /F "RE\*.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE\ $(CP_END)
	@if exist "RE\*.myf" @$(CP) /F "RE\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\RE\ $(CP_END)
	@if exist "UA\*.myf" @$(CP) /F "UA\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\UA\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\custom_boy\ $(CP_END)

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
CB_config.obj: CB_config.h
CB_custom_automatic.obj: CB_custom_automatic.h
CB_custom_cookie.obj: CB_custom_cookie.h
CB_custom_feature.obj: CB_custom_feature.h CB_vars_base.h CB_finger.h
CB_custom_postvars.obj: CB_custom_postvars.h
CB_fgp_info.obj: CB_fgp_info.h CB_finger.h CB_wgt_prim.h CB_config.h
CB_finger.obj: CB_finger.h CB_snippet.h CB_wgt_prim.h CB_ua_conf.h
CB_snippet.obj: CB_snippet.h
CB_ua_conf.obj: CB_ua_conf.h CB_wgt_prim_db.h
CB_ua_info.obj: CB_ua_info.h
CB_vars_base.obj: CB_vars_base.h
CB_virtualizer.obj: CB_virtualizer.h CB_config.h CB_fgp_info.h CB_ua_conf.h CB_snippet.h CB_wgt_prim.h CB_finger.h CB_vars_base.h
CB_wgt_prim.obj: CB_wgt_prim.h
CB_wgt_prim_db.obj: CB_wgt_prim_db.h
main.obj: CB_virtualizer.h CB_fgp_info.h CB_ua_info.h CB_config.h CB_custom_automatic.h CB_custom_postvars.h CB_custom_feature.h CB_custom_cookie.h
