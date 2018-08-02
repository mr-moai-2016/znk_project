# It should be ndk root directory(which contains jni directory.)
LOCAL_PATH := $(call my-dir)/..
S:=..

ifndef MY_LIBS_ROOT
  MY_LIBS_ROOT := ../..
endif

# Declaring a prebuilt external-static-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libRano
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/libRano/mkf_android/libs/$(TARGET_ARCH_ABI)/libRano.a
include $(PREBUILT_STATIC_LIBRARY)

# Declaring a prebuilt external-static-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libZnk
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/libZnk/mkf_android/libs/$(TARGET_ARCH_ABI)/libZnk.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := docgen
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libRano \
	-I$(MY_LIBS_ROOT)/libZnk \

LOCAL_SRC_FILES := \
	$S/main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES :=
include $(BUILD_EXECUTABLE)

