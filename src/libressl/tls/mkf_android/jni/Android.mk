# It should be ndk root directory(which contains jni directory.)
LOCAL_PATH := $(call my-dir)/..
S:=..

ifndef MY_LIBS_ROOT
  MY_LIBS_ROOT := ../..
endif

# Declaring a prebuilt external-static-library module
include $(CLEAR_VARS)
LOCAL_MODULE := $(MY_LIBS_ROOT)/$(SLIBS_DIR)
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/$(MY_LIBS_ROOT)/$(SLIBS_DIR)/mkf_android/libs/$(TARGET_ARCH_ABI)/$(MY_LIBS_ROOT)/$(SLIBS_DIR).a
include $(PREBUILT_STATIC_LIBRARY)

# Declaring a prebuilt external-static-library module
include $(CLEAR_VARS)
LOCAL_MODULE := $(MY_LIBS_ROOT)/$(SLIBS_DIR)
LOCAL_SRC_FILES := $(MY_LIBS_ROOT)/$(MY_LIBS_ROOT)/$(SLIBS_DIR)/mkf_android/libs/$(TARGET_ARCH_ABI)/$(MY_LIBS_ROOT)/$(SLIBS_DIR).a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tls
LOCAL_CFLAGS += -I.. \
	-I../include \
	-I../include/compat \
	-I../crypto/asn1 \
	-I../crypto/bn \
	-I../crypto/evp \
	-I../crypto/modes \
	-I../crypto \

LOCAL_SRC_FILES := \
	$S/tls_conninfo.c \
	$S/tls_verify.c \
	$S/tls_bio_cb.c \
	$S/tls_config.c \
	$S/tls_keypair.c \
	$S/tls_server.c \
	$S/tls_peer.c \
	$S/compat/pwrite.c \
	$S/compat/getuid.c \
	$S/compat/pread.c \
	$S/compat/ftruncate.c \
	$S/tls_client.c \
	$S/tls_ocsp.c \
	$S/tls.c \
	$S/tls_util.c \
	$S/dll_main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES :=
include $(BUILD_SHARED_LIBRARY)

