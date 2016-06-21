LOCAL_PATH := $(call my-dir)

#libZnk
include $(CLEAR_VARS)
LOCAL_MODULE := libZnk-module
LOCAL_SRC_FILES := ../../../libZnk/android/libs/$(TARGET_ARCH_ABI)/libZnk.so
include $(PREBUILT_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_CFLAGS += -I../

LOCAL_MODULE    := futaba
LOCAL_SRC_FILES := \
	../../dll_main.c \
	../../main.c \
	../../init.c \

#LOCAL_LDLIBS := -lz
LOCAL_SHARED_LIBRARIES := libZnk-module
LOCAL_C_INCLUDE := ../../libZnk
LOCAL_CFLAGS += -I../../libZnk -I../../plugin_futaba

include $(BUILD_SHARED_LIBRARY)

