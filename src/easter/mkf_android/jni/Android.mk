# It should be ndk root directory(which contains jni directory.)
LOCAL_PATH := $(call my-dir)/..
S:=..

ifndef MY_LIBS_ROOT
  MY_LIBS_ROOT := ../..
endif

# Declaring a prebuilt external-dynamic-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libZnk
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/libZnk/mkf_android/libs/$(TARGET_ARCH_ABI)/libZnk.so
include $(PREBUILT_SHARED_LIBRARY)

# Declaring a prebuilt external-dynamic-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libRano
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/libRano/mkf_android/libs/$(TARGET_ARCH_ABI)/libRano.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := easter
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/Est_assort.c \
	$S/Est_assort_list.c \
	$S/Est_assort_ui.c \
	$S/Est_base.c \
	$S/Est_bat_operator.c \
	$S/Est_box.c \
	$S/Est_boxmap_viewer.c \
	$S/Est_box_base.c \
	$S/Est_box_ui.c \
	$S/Est_cinf_ui.c \
	$S/Est_config.c \
	$S/Est_dir_util.c \
	$S/Est_filter.c \
	$S/Est_finf.c \
	$S/Est_get.c \
	$S/Est_hint_manager.c \
	$S/Est_img_viewer.c \
	$S/Est_linf_list.c \
	$S/Est_link.c \
	$S/Est_link_manager.c \
	$S/Est_parser.c \
	$S/Est_post.c \
	$S/Est_recentory.c \
	$S/Est_record.c \
	$S/Est_search_manager.c \
	$S/Est_sqi.c \
	$S/Est_tag.c \
	$S/Est_tag_manager.c \
	$S/Est_topic.c \
	$S/Est_ui.c \
	$S/Est_ui_base.c \
	$S/Est_unid.c \
	$S/main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := cache_task
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/Est_assort.c \
	$S/Est_assort_list.c \
	$S/Est_assort_ui.c \
	$S/Est_base.c \
	$S/Est_bat_operator.c \
	$S/Est_box.c \
	$S/Est_boxmap_viewer.c \
	$S/Est_box_base.c \
	$S/Est_box_ui.c \
	$S/Est_cinf_ui.c \
	$S/Est_config.c \
	$S/Est_dir_util.c \
	$S/Est_filter.c \
	$S/Est_finf.c \
	$S/Est_get.c \
	$S/Est_hint_manager.c \
	$S/Est_img_viewer.c \
	$S/Est_linf_list.c \
	$S/Est_link.c \
	$S/Est_link_manager.c \
	$S/Est_parser.c \
	$S/Est_post.c \
	$S/Est_recentory.c \
	$S/Est_record.c \
	$S/Est_search_manager.c \
	$S/Est_sqi.c \
	$S/Est_tag.c \
	$S/Est_tag_manager.c \
	$S/Est_topic.c \
	$S/Est_ui.c \
	$S/Est_ui_base.c \
	$S/Est_unid.c \
	$S/cache_task.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

