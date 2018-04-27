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
LOCAL_MODULE := hello
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/hello.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := evar1
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/evar1.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := evar2
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/evar2.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := query_string1
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/query_string1.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := query_string2
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/query_string2.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := post1
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/post1.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := post2
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/post2.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := transfer_chunked
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/transfer_chunked.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := hello_template
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/hello_template.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := progress
LOCAL_CFLAGS += -I.. \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \

LOCAL_SRC_FILES := \
	$S/cgi_util.c \
	$S/progress.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES := libZnk libRano
include $(BUILD_EXECUTABLE)

