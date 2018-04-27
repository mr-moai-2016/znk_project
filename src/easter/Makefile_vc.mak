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

BASENAME0=easter
EXE_FILE0=$O\easter.cgi
OBJS0=\
	$O\Est_assort.obj \
	$O\Est_assort_list.obj \
	$O\Est_assort_ui.obj \
	$O\Est_base.obj \
	$O\Est_bat_operator.obj \
	$O\Est_box.obj \
	$O\Est_boxmap_viewer.obj \
	$O\Est_box_base.obj \
	$O\Est_box_ui.obj \
	$O\Est_cinf_ui.obj \
	$O\Est_config.obj \
	$O\Est_dir_util.obj \
	$O\Est_filter.obj \
	$O\Est_finf.obj \
	$O\Est_get.obj \
	$O\Est_hint_manager.obj \
	$O\Est_img_viewer.obj \
	$O\Est_linf_list.obj \
	$O\Est_link.obj \
	$O\Est_link_manager.obj \
	$O\Est_parser.obj \
	$O\Est_post.obj \
	$O\Est_recentory.obj \
	$O\Est_record.obj \
	$O\Est_search_manager.obj \
	$O\Est_sqi.obj \
	$O\Est_tag.obj \
	$O\Est_tag_manager.obj \
	$O\Est_topic.obj \
	$O\Est_ui.obj \
	$O\Est_ui_base.obj \
	$O\Est_unid.obj \
	$O\main.obj \

BASENAME1=cache_task
EXE_FILE1=$O\cache_task.cgi
OBJS1=\
	$O\Est_assort.obj \
	$O\Est_assort_list.obj \
	$O\Est_assort_ui.obj \
	$O\Est_base.obj \
	$O\Est_bat_operator.obj \
	$O\Est_box.obj \
	$O\Est_boxmap_viewer.obj \
	$O\Est_box_base.obj \
	$O\Est_box_ui.obj \
	$O\Est_cinf_ui.obj \
	$O\Est_config.obj \
	$O\Est_dir_util.obj \
	$O\Est_filter.obj \
	$O\Est_finf.obj \
	$O\Est_get.obj \
	$O\Est_hint_manager.obj \
	$O\Est_img_viewer.obj \
	$O\Est_linf_list.obj \
	$O\Est_link.obj \
	$O\Est_link_manager.obj \
	$O\Est_parser.obj \
	$O\Est_post.obj \
	$O\Est_recentory.obj \
	$O\Est_record.obj \
	$O\Est_search_manager.obj \
	$O\Est_sqi.obj \
	$O\Est_tag.obj \
	$O\Est_tag_manager.obj \
	$O\Est_topic.obj \
	$O\Est_ui.obj \
	$O\Est_ui_base.obj \
	$O\Est_unid.obj \
	$O\cache_task.obj \

SUB_LIBS=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE1) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Rano-$(DL_VER).dll \



# Entry rule.
all: $O $(EXE_FILE0) $(EXE_FILE1) 

# Mkdir rule.
$O:
	if not exist $O mkdir $O


# Product files rule.
$(EXE_FILE0): $(OBJS0) 
	$(LINKER) /OUT:$(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib ws2_32.lib 

$(EXE_FILE1): $(OBJS1) 
	$(LINKER) /OUT:$(EXE_FILE1)  $(OBJS1) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/Znk-$(DL_VER).imp.lib $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/Rano-$(DL_VER).imp.lib ws2_32.lib 


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
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\default @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\default 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\icons @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\icons 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\android @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\android 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\iphone @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\iphone 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\agree.5ch.net\js @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\agree.5ch.net\js 
	@if exist "default\*.myf" @$(CP) /F "default\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\default\ $(CP_END)
	@if exist "templates\*.html" @$(CP) /F "templates\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates\ $(CP_END)
	@if exist "templates\*.myf" @$(CP) /F "templates\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates\ $(CP_END)
	@if exist "publicbox\*" @$(CP) /F "publicbox\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\ $(CP_END)
	@if exist "publicbox\icons\*.png" @$(CP) /F "publicbox\icons\*.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\icons\ $(CP_END)
	@if exist "publicbox\alternative\5ch\itest.5ch.net\assets\js\android\*" @$(CP) /F "publicbox\alternative\5ch\itest.5ch.net\assets\js\android\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\android\ $(CP_END)
	@if exist "publicbox\alternative\5ch\itest.5ch.net\assets\js\iphone\*" @$(CP) /F "publicbox\alternative\5ch\itest.5ch.net\assets\js\iphone\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\iphone\ $(CP_END)
	@if exist "publicbox\alternative\5ch\agree.5ch.net\js\*.js" @$(CP) /F "publicbox\alternative\5ch\agree.5ch.net\js\*.js" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\agree.5ch.net\js\ $(CP_END)

# Install exec rule.
install_exec: $(EXE_FILE0) $(EXE_FILE1)
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter 
	@if exist "$(EXE_FILE0)" @$(CP) /F "$(EXE_FILE0)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\ $(CP_END)
	@if exist "$(EXE_FILE1)" @$(CP) /F "$(EXE_FILE1)" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\ $(CP_END)

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
cache_task.obj: Est_search_manager.h Est_config.h Est_base.h Est_sqi.h Est_recentory.h Est_box.h
Est_assort.obj: Est_assort.h Est_config.h Est_ui.h Est_recentory.h Est_base.h Est_dir_util.h Est_tag.h
Est_assort_list.obj: Est_assort_list.h Est_config.h Est_ui.h Est_box.h Est_base.h Est_assort.h Est_assort_ui.h
Est_assort_ui.obj: Est_assort_ui.h Est_config.h Est_ui_base.h Est_unid.h
Est_base.obj: Est_base.h Est_config.h
Est_bat_operator.obj: Est_bat_operator.h
Est_box.obj: Est_box.h Est_finf.h Est_recentory.h Est_bat_operator.h Est_assort.h Est_dir_util.h Est_tag.h Est_config.h Est_sqi.h Est_search_manager.h
Est_boxmap_viewer.obj: Est_boxmap_viewer.h Est_config.h Est_ui.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_tag.h Est_bat_operator.h Est_box.h Est_box_ui.h Est_assort_list.h
Est_box_base.obj: Est_box_base.h Est_config.h Est_base.h
Est_box_ui.obj: Est_box_ui.h Est_config.h Est_ui_base.h Est_cinf_ui.h Est_linf_list.h
Est_cinf_ui.obj: Est_cinf_ui.h Est_base.h
Est_config.obj: Est_config.h Est_base.h Est_hint_manager.h Est_unid.h
Est_dir_util.obj: Est_dir_util.h
Est_filter.obj: Est_filter.h Est_link.h Est_config.h Est_parser.h Est_base.h Est_hint_manager.h
Est_finf.obj: Est_finf.h Est_record.h
Est_get.obj: Est_get.h Est_config.h Est_link.h Est_base.h Est_filter.h Est_img_viewer.h
Est_hint_manager.obj: Est_hint_manager.h
Est_img_viewer.obj: Est_img_viewer.h Est_config.h Est_ui.h Est_ui_base.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_tag.h Est_finf.h Est_hint_manager.h
Est_linf_list.obj: Est_linf_list.h Est_base.h
Est_link.obj: Est_link.h Est_parser.h Est_config.h
Est_link_manager.obj: Est_link_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_base.h Est_linf_list.h Est_box_ui.h
Est_parser.obj: Est_parser.h
Est_post.obj: Est_post.h Est_base.h Est_config.h Est_ui.h Est_hint_manager.h
Est_recentory.obj: Est_recentory.h Est_search_manager.h Est_finf.h Est_unid.h
Est_record.obj: Est_record.h Est_base.h
Est_search_manager.obj: Est_search_manager.h Est_boxmap_viewer.h Est_ui.h Est_box_ui.h Est_record.h Est_config.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_base.h Est_finf.h Est_box_base.h Est_unid.h
Est_sqi.obj: Est_sqi.h Est_tag.h
Est_tag.obj: Est_tag.h Est_unid.h
Est_tag_manager.obj: Est_tag_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_assort_ui.h Est_assort.h Est_tag.h Est_base.h
Est_topic.obj: Est_topic.h Est_config.h Est_ui.h Est_ui_base.h Est_box.h Est_box_ui.h Est_base.h Est_assort.h Est_assort_ui.h Est_search_manager.h Est_recentory.h Est_finf.h Est_assort_list.h Est_hint_manager.h
Est_ui.obj: Est_ui.h Est_ui_base.h Est_config.h Est_assort.h Est_base.h Est_box_ui.h
Est_ui_base.obj: Est_ui_base.h Est_box_base.h
Est_unid.obj:
main.obj: Est_config.h Est_get.h Est_post.h Est_filter.h Est_link_manager.h Est_boxmap_viewer.h Est_search_manager.h Est_img_viewer.h Est_topic.h Est_tag_manager.h
