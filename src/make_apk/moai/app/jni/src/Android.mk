# It should be ndk root directory(which contains jni directory.)
LOCAL_PATH := $(call my-dir)/../..
S:=jni/src

ifndef MY_LIBS_ROOT
  MY_LIBS_ROOT := ../../..
endif

# Declaring a prebuilt external-dynamic-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libZnk
# LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/libZnk/mkf_android/libs/$(TARGET_ARCH_ABI)/libZnk.so
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/dlib/android-$(TARGET_ARCH_ABI)/libZnk-2.2.so
include $(PREBUILT_SHARED_LIBRARY)

# Declaring a prebuilt external-dynamic-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libRano
# LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/libRano/mkf_android/libs/$(TARGET_ARCH_ABI)/libRano.so
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/dlib/android-$(TARGET_ARCH_ABI)/libRano-2.2.so
include $(PREBUILT_SHARED_LIBRARY)

# Declaring a prebuilt external-dynamic-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libMoai
# LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/libMoai/mkf_android/libs/$(TARGET_ARCH_ABI)/libMoai.so
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/dlib/android-$(TARGET_ARCH_ABI)/libMoai-2.2.so
include $(PREBUILT_SHARED_LIBRARY)

# Declaring a prebuilt external-dynamic-library module
include $(CLEAR_VARS)
LOCAL_MODULE := libtls-17
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/dlib/android-$(TARGET_ARCH_ABI)/libtls-17.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := main
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \
	-I$(MY_LIBS_ROOT)/libMoai \
	-I$(MY_LIBS_ROOT)/moai \

# Add your application source files here...
LOCAL_SRC_FILES := \
	$(wildcard $S/Moai_*.c) \
	$S/test.c

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano libMoai

LOCAL_LDLIBS := -landroid -llog

include $(BUILD_SHARED_LIBRARY)
