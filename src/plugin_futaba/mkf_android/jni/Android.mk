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
LOCAL_MODULE := futaba
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \

LOCAL_SRC_FILES := \
	$S/init.c \
	$S/main.c \
	$S/dll_main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk
include $(BUILD_SHARED_LIBRARY)

