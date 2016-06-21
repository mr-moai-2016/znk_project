LOCAL_PATH := $(call my-dir)

#libZnk
include $(CLEAR_VARS)
LOCAL_MODULE := libZnk-module
LOCAL_SRC_FILES := ../../../libZnk/android/libs/$(TARGET_ARCH_ABI)/libZnk.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := http_decorator
LOCAL_SRC_FILES := \
	../../main.cpp \

#LOCAL_LDLIBS += -ldl -llog -lGLESv1_CM

LOCAL_SHARED_LIBRARIES := libZnk-module
LOCAL_C_INCLUDE := ../../libZnk
LOCAL_CFLAGS += -I../../libZnk -I../../http_decorator
include $(BUILD_EXECUTABLE)

