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

include $(CLEAR_VARS)
LOCAL_MODULE := Rano
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \

LOCAL_SRC_FILES := \
	$S/Rano_cgi_util.c \
	$S/Rano_conf_util.c \
	$S/Rano_dir_util.c \
	$S/Rano_file_info.c \
	$S/Rano_hash.c \
	$S/Rano_html_ui.c \
	$S/Rano_htp_boy.c \
	$S/Rano_htp_modifier.c \
	$S/Rano_log.c \
	$S/Rano_module.c \
	$S/Rano_myf.c \
	$S/Rano_parent_proxy.c \
	$S/Rano_post.c \
	$S/Rano_sset.c \
	$S/Rano_txt_filter.c \
	$S/Rano_vtag_util.c \
	$S/tls_module/tls_module.c \
	$S/dll_main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk
include $(BUILD_SHARED_LIBRARY)

