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
LOCAL_MODULE := custom_boy
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/CB_config.c \
	$S/CB_custom_automatic.c \
	$S/CB_custom_cookie.c \
	$S/CB_custom_feature.c \
	$S/CB_custom_postvars.c \
	$S/CB_fgp_info.c \
	$S/CB_finger.c \
	$S/CB_snippet.c \
	$S/CB_ua_conf.c \
	$S/CB_ua_info.c \
	$S/CB_vars_base.c \
	$S/CB_virtualizer.c \
	$S/CB_wgt_prim.c \
	$S/CB_wgt_prim_db.c \
	$S/main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

