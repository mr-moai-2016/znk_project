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

include $(CLEAR_VARS)
LOCAL_MODULE := ssl
LOCAL_CFLAGS += -I.. \
	-I../include \
	-I../include/compat \
	-I../crypto/asn1 \
	-I../crypto/bn \
	-I../crypto/evp \
	-I../crypto/modes \
	-I../crypto \

LOCAL_SRC_FILES := \
	$S/t1_enc.c \
	$S/ssl_init.c \
	$S/ssl_srvr.c \
	$S/d1_pkt.c \
	$S/ssl_versions.c \
	$S/t1_hash.c \
	$S/t1_srvr.c \
	$S/ssl_txt.c \
	$S/s3_cbc.c \
	$S/ssl_both.c \
	$S/t1_meth.c \
	$S/ssl_cert.c \
	$S/d1_both.c \
	$S/ssl_err.c \
	$S/ssl_rsa.c \
	$S/ssl_packet.c \
	$S/ssl_algs.c \
	$S/bs_ber.c \
	$S/d1_srvr.c \
	$S/ssl_lib.c \
	$S/ssl_clnt.c \
	$S/ssl_asn1.c \
	$S/bs_cbs.c \
	$S/d1_lib.c \
	$S/ssl_pkt.c \
	$S/d1_meth.c \
	$S/ssl_ciph.c \
	$S/ssl_stat.c \
	$S/d1_enc.c \
	$S/pqueue.c \
	$S/d1_srtp.c \
	$S/t1_clnt.c \
	$S/bs_cbb.c \
	$S/bio_ssl.c \
	$S/s3_lib.c \
	$S/ssl_tlsext.c \
	$S/ssl_sess.c \
	$S/d1_clnt.c \
	$S/t1_lib.c \
	$S/dll_main.c \

# Refer to prebuilt external-dynamic-libraries
LOCAL_SHARED_LIBRARIES :=
include $(BUILD_SHARED_LIBRARY)

