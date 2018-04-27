# It should be ndk root directory(which contains jni directory.)
LOCAL_PATH := $(call my-dir)/..
S:=..

ifndef MY_LIBS_ROOT
  MY_LIBS_ROOT := ../..
endif

include $(CLEAR_VARS)
LOCAL_MODULE := zlib
LOCAL_CFLAGS += -I.. \

LOCAL_SRC_FILES := \
	$S/adler32.c \
	$S/compress.c \
	$S/crc32.c \
	$S/deflate.c \
	$S/gzio.c \
	$S/infback.c \
	$S/inffast.c \
	$S/inflate.c \
	$S/inftrees.c \
	$S/trees.c \
	$S/uncompr.c \
	$S/zutil.c \

include $(BUILD_STATIC_LIBRARY)

