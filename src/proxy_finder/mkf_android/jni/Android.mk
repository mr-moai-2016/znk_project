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
LOCAL_MODULE := proxy_finder
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_helper.c \
	$S/proxy_finder.cpp \
	$S/proxy_info.c \
	$S/main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

