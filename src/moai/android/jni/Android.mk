LOCAL_PATH := $(call my-dir)

#libZnk
include $(CLEAR_VARS)
LOCAL_MODULE := libZnk-module
LOCAL_SRC_FILES := ../../../libZnk/android/libs/$(TARGET_ARCH_ABI)/libZnk.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := moai
LOCAL_SRC_FILES := \
	../../Moai_connection.c \
	../../Moai_info.c \
	../../Moai_io_base.c \
	../../Moai_post_vars.c \
	../../Moai_context.c \
	../../Moai_module.c \
	../../Moai_post.c \
	../../Moai_server.c \
	../../Moai_log.c \
	../../Moai_fdset.c \
	../../Moai_parent_proxy.c \
	../../Moai_myf.c \
	../../Moai_enc_util.c \
	../../main.c

#LOCAL_LDLIBS += -ldl -llog -lGLESv1_CM

LOCAL_SHARED_LIBRARIES := libZnk-module
LOCAL_C_INCLUDE := ../../libZnk
LOCAL_CFLAGS += -I../../libZnk -I../../moai
include $(BUILD_EXECUTABLE)

