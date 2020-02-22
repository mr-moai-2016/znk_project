# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

ifndef MACHINE
  UNAME_M=$(shell uname -m)
  ifeq ($(UNAME_M), i386)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), i686)
    MACHINE=x86
  endif
  ifeq ($(UNAME_M), x86_64)
    MACHINE=x64
  endif
  ifeq ($(UNAME_M), amd64)
    MACHINE=x64
  endif
endif
GCC_CMD=gcc
PLATFORM=cygwin
ifeq ($(MACHINE), x64)
  GCC_CMD=gcc -m64
  PLATFORM=cygwin64
endif
ifeq ($(MACHINE), x86)
  GCC_CMD=gcc -m32
  PLATFORM=cygwin32
endif
# Output directory
ABINAME = cygwin$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g 
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)d
SLIBS_DIR=slib/$(PLATFORM)d
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

BASENAME0=easter
EXE_FILE0=$O/easter.cgi
OBJS0=\
	$O/Est_assort.o \
	$O/Est_assort_list.o \
	$O/Est_assort_ui.o \
	$O/Est_base.o \
	$O/Est_bat_operator.o \
	$O/Est_bmp_writer.o \
	$O/Est_box.o \
	$O/Est_boxmap_viewer.o \
	$O/Est_box_base.o \
	$O/Est_box_ui.o \
	$O/Est_cinf_ui.o \
	$O/Est_config.o \
	$O/Est_dir_util.o \
	$O/Est_filter.o \
	$O/Est_filter_5ch.o \
	$O/Est_filter_default.o \
	$O/Est_filter_futaba.o \
	$O/Est_finf.o \
	$O/Est_get.o \
	$O/Est_hint_manager.o \
	$O/Est_img_viewer.o \
	$O/Est_linf_list.o \
	$O/Est_link.o \
	$O/Est_link_manager.o \
	$O/Est_parser.o \
	$O/Est_post.o \
	$O/Est_recentory.o \
	$O/Est_record.o \
	$O/Est_rpsc.o \
	$O/Est_search_manager.o \
	$O/Est_sqi.o \
	$O/Est_tag.o \
	$O/Est_tag_manager.o \
	$O/Est_topic.o \
	$O/Est_ui.o \
	$O/Est_ui_base.o \
	$O/Est_unid.o \
	$O/main.o \

BASENAME1=easter_maintainer
EXE_FILE1=$O/easter_maintainer.cgi
OBJS1=\
	$O/Est_assort.o \
	$O/Est_assort_list.o \
	$O/Est_assort_ui.o \
	$O/Est_base.o \
	$O/Est_bat_operator.o \
	$O/Est_bmp_writer.o \
	$O/Est_box.o \
	$O/Est_boxmap_viewer.o \
	$O/Est_box_base.o \
	$O/Est_box_ui.o \
	$O/Est_cinf_ui.o \
	$O/Est_config.o \
	$O/Est_dir_util.o \
	$O/Est_filter.o \
	$O/Est_filter_5ch.o \
	$O/Est_filter_default.o \
	$O/Est_filter_futaba.o \
	$O/Est_finf.o \
	$O/Est_get.o \
	$O/Est_hint_manager.o \
	$O/Est_img_viewer.o \
	$O/Est_linf_list.o \
	$O/Est_link.o \
	$O/Est_link_manager.o \
	$O/Est_parser.o \
	$O/Est_post.o \
	$O/Est_recentory.o \
	$O/Est_record.o \
	$O/Est_rpsc.o \
	$O/Est_search_manager.o \
	$O/Est_sqi.o \
	$O/Est_tag.o \
	$O/Est_tag_manager.o \
	$O/Est_topic.o \
	$O/Est_ui.o \
	$O/Est_ui_base.o \
	$O/Est_unid.o \
	$O/easter_maintainer.o \

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
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/cygZnk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/cygRano-$(DL_VER).dll \



# Entry rule.
all: $O $(EXE_FILE0) $(EXE_FILE1) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	@echo $(LINKER) -o $(EXE_FILE0) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE0) $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 

$(EXE_FILE1): $(OBJS1)
	@echo $(LINKER) -o $(EXE_FILE1) {[objs]} $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 
	@     $(LINKER) -o $(EXE_FILE1) $(OBJS1) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/cygZnk-$(DL_VER).dll $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/cygRano-$(DL_VER).dll -lpthread -ldl -lstdc++ 


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
$O/%.o: $S/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O/%.o: $S/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/default 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/templates 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/icons 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/pascua 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_futaba 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_5ch 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/android 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/iphone 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/agree.5ch.net/js 
	for tgt in core_behavior.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/ ; fi ; done
	for tgt in rano_app.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/ ; fi ; done
	for tgt in default/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/default/ ; fi ; done
	for tgt in templates/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/templates/ ; fi ; done
	for tgt in templates/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/templates/ ; fi ; done
	for tgt in publicbox/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/ ; fi ; done
	for tgt in publicbox/icons/*.png ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/icons/ ; fi ; done
	for tgt in publicbox/pascua/*.js ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/pascua/ ; fi ; done
	for tgt in publicbox/bbs_futaba/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_futaba/ ; fi ; done
	for tgt in publicbox/bbs_futaba/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_futaba/ ; fi ; done
	for tgt in publicbox/bbs_futaba/*.js ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_futaba/ ; fi ; done
	for tgt in publicbox/bbs_futaba/*.css ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_futaba/ ; fi ; done
	for tgt in publicbox/bbs_5ch/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_5ch/ ; fi ; done
	for tgt in publicbox/bbs_5ch/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_5ch/ ; fi ; done
	for tgt in publicbox/bbs_5ch/*.js ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_5ch/ ; fi ; done
	for tgt in publicbox/bbs_5ch/*.css ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/bbs_5ch/ ; fi ; done
	for tgt in publicbox/alternative/5ch/itest.5ch.net/assets/js/android/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/android/ ; fi ; done
	for tgt in publicbox/alternative/5ch/itest.5ch.net/assets/js/iphone/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/iphone/ ; fi ; done
	for tgt in publicbox/alternative/5ch/agree.5ch.net/js/*.js ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/agree.5ch.net/js/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0) $(EXE_FILE1)
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/ ; fi ; done
	for tgt in $(EXE_FILE1) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/ ; fi ; done

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rm -rf $O/ 

# Src and Headers Dependency
$O/easter_maintainer.o: Est_search_manager.h Est_config.h Est_base.h Est_sqi.h Est_recentory.h Est_box.h
$O/Est_assort.o: Est_assort.h Est_config.h Est_ui.h Est_recentory.h Est_base.h Est_dir_util.h Est_tag.h
$O/Est_assort_list.o: Est_assort_list.h Est_config.h Est_ui.h Est_box.h Est_base.h Est_assort.h Est_assort_ui.h
$O/Est_assort_ui.o: Est_assort_ui.h Est_config.h Est_ui_base.h Est_unid.h
$O/Est_base.o: Est_base.h Est_config.h
$O/Est_bat_operator.o: Est_bat_operator.h
$O/Est_box.o: Est_box.h Est_finf.h Est_recentory.h Est_bat_operator.h Est_assort.h Est_dir_util.h Est_tag.h Est_config.h Est_sqi.h Est_search_manager.h
$O/Est_boxmap_viewer.o: Est_boxmap_viewer.h Est_config.h Est_ui.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_tag.h Est_bat_operator.h Est_box.h Est_box_ui.h Est_assort_list.h
$O/Est_box_base.o: Est_box_base.h Est_config.h Est_base.h
$O/Est_box_ui.o: Est_box_ui.h Est_config.h Est_ui_base.h Est_cinf_ui.h Est_linf_list.h
$O/Est_cinf_ui.o: Est_cinf_ui.h Est_base.h
$O/Est_config.o: Est_config.h Est_base.h Est_hint_manager.h Est_unid.h
$O/Est_dir_util.o: Est_dir_util.h
$O/Est_filter.o: Est_filter.h Est_link.h Est_config.h Est_parser.h Est_base.h Est_hint_manager.h Est_filter_default.h Est_filter_futaba.h Est_filter_5ch.h
$O/Est_filter_5ch.o: Est_filter_5ch.h Est_parser.h Est_link.h Est_config.h Est_rpsc.h
$O/Est_filter_default.o: Est_filter_default.h Est_parser.h Est_link.h Est_config.h
$O/Est_filter_futaba.o: Est_filter_futaba.h Est_parser.h Est_link.h Est_config.h Est_rpsc.h
$O/Est_finf.o: Est_finf.h Est_record.h
$O/Est_get.o: Est_get.h Est_config.h Est_link.h Est_base.h Est_filter.h Est_img_viewer.h
$O/Est_hint_manager.o: Est_hint_manager.h
$O/Est_img_viewer.o: Est_img_viewer.h Est_config.h Est_ui.h Est_ui_base.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_tag.h Est_finf.h Est_hint_manager.h
$O/Est_linf_list.o: Est_linf_list.h Est_base.h
$O/Est_link.o: Est_link.h Est_parser.h Est_config.h
$O/Est_link_manager.o: Est_link_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_base.h Est_linf_list.h Est_box_ui.h
$O/Est_parser.o: Est_parser.h
$O/Est_post.o: Est_post.h Est_base.h Est_config.h Est_ui.h Est_hint_manager.h Est_bmp_writer.h
$O/Est_recentory.o: Est_recentory.h Est_search_manager.h Est_finf.h Est_unid.h
$O/Est_record.o: Est_record.h Est_base.h
$O/Est_rpsc.o: Est_rpsc.h Est_parser.h Est_filter.h Est_link.h Est_base.h
$O/Est_search_manager.o: Est_search_manager.h Est_boxmap_viewer.h Est_ui.h Est_box_ui.h Est_record.h Est_config.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_base.h Est_finf.h Est_box_base.h Est_unid.h
$O/Est_sqi.o: Est_sqi.h Est_tag.h
$O/Est_tag.o: Est_tag.h Est_unid.h
$O/Est_tag_manager.o: Est_tag_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_assort_ui.h Est_assort.h Est_tag.h Est_base.h
$O/Est_topic.o: Est_topic.h Est_config.h Est_ui.h Est_ui_base.h Est_box.h Est_box_ui.h Est_base.h Est_assort.h Est_assort_ui.h Est_search_manager.h Est_recentory.h Est_finf.h Est_assort_list.h Est_hint_manager.h
$O/Est_ui.o: Est_ui.h Est_ui_base.h Est_config.h Est_assort.h Est_base.h Est_box_ui.h
$O/Est_ui_base.o: Est_ui_base.h Est_box_base.h
$O/main.o: Est_config.h Est_get.h Est_post.h Est_filter.h Est_link_manager.h Est_boxmap_viewer.h Est_search_manager.h Est_img_viewer.h Est_topic.h Est_tag_manager.h
