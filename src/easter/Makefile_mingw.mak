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

BASENAME0=easter
EXE_FILE0=$O\easter.cgi
OBJS0=\
	$O\Est_assort.o \
	$O\Est_assort_list.o \
	$O\Est_assort_ui.o \
	$O\Est_base.o \
	$O\Est_bat_operator.o \
	$O\Est_bmp_writer.o \
	$O\Est_box.o \
	$O\Est_boxmap_viewer.o \
	$O\Est_box_base.o \
	$O\Est_box_ui.o \
	$O\Est_cinf_ui.o \
	$O\Est_config.o \
	$O\Est_dir_util.o \
	$O\Est_filter.o \
	$O\Est_filter_5ch.o \
	$O\Est_filter_default.o \
	$O\Est_filter_futaba.o \
	$O\Est_finf.o \
	$O\Est_get.o \
	$O\Est_hint_manager.o \
	$O\Est_img_viewer.o \
	$O\Est_linf_list.o \
	$O\Est_link.o \
	$O\Est_link_manager.o \
	$O\Est_parser.o \
	$O\Est_post.o \
	$O\Est_recentory.o \
	$O\Est_record.o \
	$O\Est_rpsc.o \
	$O\Est_search_manager.o \
	$O\Est_sqi.o \
	$O\Est_tag.o \
	$O\Est_tag_manager.o \
	$O\Est_topic.o \
	$O\Est_ui.o \
	$O\Est_ui_base.o \
	$O\Est_unid.o \
	$O\main.o \

BASENAME1=easter_maintainer
EXE_FILE1=$O\easter_maintainer.cgi
OBJS1=\
	$O\Est_assort.o \
	$O\Est_assort_list.o \
	$O\Est_assort_ui.o \
	$O\Est_base.o \
	$O\Est_bat_operator.o \
	$O\Est_bmp_writer.o \
	$O\Est_box.o \
	$O\Est_boxmap_viewer.o \
	$O\Est_box_base.o \
	$O\Est_box_ui.o \
	$O\Est_cinf_ui.o \
	$O\Est_config.o \
	$O\Est_dir_util.o \
	$O\Est_filter.o \
	$O\Est_filter_5ch.o \
	$O\Est_filter_default.o \
	$O\Est_filter_futaba.o \
	$O\Est_finf.o \
	$O\Est_get.o \
	$O\Est_hint_manager.o \
	$O\Est_img_viewer.o \
	$O\Est_linf_list.o \
	$O\Est_link.o \
	$O\Est_link_manager.o \
	$O\Est_parser.o \
	$O\Est_post.o \
	$O\Est_recentory.o \
	$O\Est_record.o \
	$O\Est_rpsc.o \
	$O\Est_search_manager.o \
	$O\Est_sqi.o \
	$O\Est_tag.o \
	$O\Est_tag_manager.o \
	$O\Est_topic.o \
	$O\Est_ui.o \
	$O\Est_ui_base.o \
	$O\Est_unid.o \
	$O\easter_maintainer.o \

SUB_LIBS=\

SUB_OBJS=\

SUB_OBJS_ECHO=\

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
	@echo $(LINKER) -o $(EXE_FILE0)  {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 -luser32 -lgdi32 
	@     $(LINKER) -o $(EXE_FILE0)  $(OBJS0) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 -luser32 -lgdi32 

$(EXE_FILE1): $(OBJS1) 
	@echo $(LINKER) -o $(EXE_FILE1)  {[objs]} $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 -luser32 -lgdi32 
	@     $(LINKER) -o $(EXE_FILE1)  $(OBJS1) $(SUB_LIBS) $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 -luser32 -lgdi32 


##
# Pattern rule.
#
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\'.
# Note that we have to duplicate '\' only before special charactor(% etc) in the left of ':'.
#
# For example 1 :
#   $O\\mydir\\%.o: $S\%.c        .... NG
#   $O\mydir\\%.o:  $S\%.c        .... OK
# For example 2 :
#   $O\\mydir\%.o:  $S\mydir\%.c  .... NG
#   $O\mydir\\%.o:  $S\mydir\%.c  .... OK
# In the case of example 2, we can write more simply :
#   $O\\%.o: $S\%.c               .... OK
#   (Because '%' is wildcard and it indicates patical path 'mydir\filename_base' recursively )
#
$O\\%.o: $S\%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O\\%.o: $S\%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\default @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\default 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\icons @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\icons 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\pascua @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\pascua 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_futaba @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_futaba 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_5ch @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_5ch 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\android @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\android 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\iphone @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\itest.5ch.net\assets\js\iphone 
	@if not exist ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\agree.5ch.net\js @mkdir ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\alternative\5ch\agree.5ch.net\js 
	@if exist "core_behavior.myf" @$(CP) /F "core_behavior.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\ $(CP_END)
	@if exist "rano_app.myf" @$(CP) /F "rano_app.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\ $(CP_END)
	@if exist "default\*.myf" @$(CP) /F "default\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\default\ $(CP_END)
	@if exist "templates\*.html" @$(CP) /F "templates\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates\ $(CP_END)
	@if exist "templates\*.myf" @$(CP) /F "templates\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\templates\ $(CP_END)
	@if exist "publicbox\*" @$(CP) /F "publicbox\*" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\ $(CP_END)
	@if exist "publicbox\icons\*.png" @$(CP) /F "publicbox\icons\*.png" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\icons\ $(CP_END)
	@if exist "publicbox\pascua\*.js" @$(CP) /F "publicbox\pascua\*.js" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\pascua\ $(CP_END)
	@if exist "publicbox\bbs_futaba\*.myf" @$(CP) /F "publicbox\bbs_futaba\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_futaba\ $(CP_END)
	@if exist "publicbox\bbs_futaba\*.html" @$(CP) /F "publicbox\bbs_futaba\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_futaba\ $(CP_END)
	@if exist "publicbox\bbs_futaba\*.js" @$(CP) /F "publicbox\bbs_futaba\*.js" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_futaba\ $(CP_END)
	@if exist "publicbox\bbs_futaba\*.css" @$(CP) /F "publicbox\bbs_futaba\*.css" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_futaba\ $(CP_END)
	@if exist "publicbox\bbs_5ch\*.myf" @$(CP) /F "publicbox\bbs_5ch\*.myf" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_5ch\ $(CP_END)
	@if exist "publicbox\bbs_5ch\*.html" @$(CP) /F "publicbox\bbs_5ch\*.html" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_5ch\ $(CP_END)
	@if exist "publicbox\bbs_5ch\*.js" @$(CP) /F "publicbox\bbs_5ch\*.js" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_5ch\ $(CP_END)
	@if exist "publicbox\bbs_5ch\*.css" @$(CP) /F "publicbox\bbs_5ch\*.css" ..\..\moai-v$(REL_VER)-$(PLATFORM)\cgis\easter\publicbox\bbs_5ch\ $(CP_END)
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
	rmdir /S /Q $O\ 

# Src and Headers Dependency
$O\easter_maintainer.o: Est_search_manager.h Est_config.h Est_base.h Est_sqi.h Est_recentory.h Est_box.h
$O\Est_assort.o: Est_assort.h Est_config.h Est_ui.h Est_recentory.h Est_base.h Est_dir_util.h Est_tag.h
$O\Est_assort_list.o: Est_assort_list.h Est_config.h Est_ui.h Est_box.h Est_base.h Est_assort.h Est_assort_ui.h
$O\Est_assort_ui.o: Est_assort_ui.h Est_config.h Est_ui_base.h Est_unid.h
$O\Est_base.o: Est_base.h Est_config.h
$O\Est_bat_operator.o: Est_bat_operator.h
$O\Est_box.o: Est_box.h Est_finf.h Est_recentory.h Est_bat_operator.h Est_assort.h Est_dir_util.h Est_tag.h Est_config.h Est_sqi.h Est_search_manager.h
$O\Est_boxmap_viewer.o: Est_boxmap_viewer.h Est_config.h Est_ui.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_tag.h Est_bat_operator.h Est_box.h Est_box_ui.h Est_assort_list.h
$O\Est_box_base.o: Est_box_base.h Est_config.h Est_base.h
$O\Est_box_ui.o: Est_box_ui.h Est_config.h Est_ui_base.h Est_cinf_ui.h Est_linf_list.h
$O\Est_cinf_ui.o: Est_cinf_ui.h Est_base.h
$O\Est_config.o: Est_config.h Est_base.h Est_hint_manager.h Est_unid.h
$O\Est_dir_util.o: Est_dir_util.h
$O\Est_filter.o: Est_filter.h Est_link.h Est_config.h Est_parser.h Est_base.h Est_hint_manager.h Est_filter_default.h Est_filter_futaba.h Est_filter_5ch.h
$O\Est_filter_5ch.o: Est_filter_5ch.h Est_parser.h Est_link.h Est_config.h Est_rpsc.h
$O\Est_filter_default.o: Est_filter_default.h Est_parser.h Est_link.h Est_config.h
$O\Est_filter_futaba.o: Est_filter_futaba.h Est_parser.h Est_link.h Est_config.h Est_rpsc.h
$O\Est_finf.o: Est_finf.h Est_record.h
$O\Est_get.o: Est_get.h Est_config.h Est_link.h Est_base.h Est_filter.h Est_img_viewer.h
$O\Est_hint_manager.o: Est_hint_manager.h
$O\Est_img_viewer.o: Est_img_viewer.h Est_config.h Est_ui.h Est_ui_base.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_tag.h Est_finf.h Est_hint_manager.h
$O\Est_linf_list.o: Est_linf_list.h Est_base.h
$O\Est_link.o: Est_link.h Est_parser.h Est_config.h
$O\Est_link_manager.o: Est_link_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_base.h Est_linf_list.h Est_box_ui.h
$O\Est_parser.o: Est_parser.h
$O\Est_post.o: Est_post.h Est_base.h Est_config.h Est_ui.h Est_hint_manager.h Est_bmp_writer.h
$O\Est_recentory.o: Est_recentory.h Est_search_manager.h Est_finf.h Est_unid.h
$O\Est_record.o: Est_record.h Est_base.h
$O\Est_rpsc.o: Est_rpsc.h Est_parser.h Est_filter.h Est_link.h Est_base.h
$O\Est_search_manager.o: Est_search_manager.h Est_boxmap_viewer.h Est_ui.h Est_box_ui.h Est_record.h Est_config.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_base.h Est_finf.h Est_box_base.h Est_unid.h
$O\Est_sqi.o: Est_sqi.h Est_tag.h
$O\Est_tag.o: Est_tag.h Est_unid.h
$O\Est_tag_manager.o: Est_tag_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_assort_ui.h Est_assort.h Est_tag.h Est_base.h
$O\Est_topic.o: Est_topic.h Est_config.h Est_ui.h Est_ui_base.h Est_box.h Est_box_ui.h Est_base.h Est_assort.h Est_assort_ui.h Est_search_manager.h Est_recentory.h Est_finf.h Est_assort_list.h Est_hint_manager.h
$O\Est_ui.o: Est_ui.h Est_ui_base.h Est_config.h Est_assort.h Est_base.h Est_box_ui.h
$O\Est_ui_base.o: Est_ui_base.h Est_box_base.h
$O\main.o: Est_config.h Est_get.h Est_post.h Est_filter.h Est_link_manager.h Est_boxmap_viewer.h Est_search_manager.h Est_img_viewer.h Est_topic.h Est_tag_manager.h
