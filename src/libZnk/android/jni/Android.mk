LOCAL_PATH := $(call my-dir)


include $(CLEAR_VARS)

LOCAL_CFLAGS += -I../zlib

LOCAL_MODULE    := libz
LOCAL_SRC_FILES := \
	../../zlib/adler32.c \
	../../zlib/compress.c \
	../../zlib/crc32.c \
	../../zlib/deflate.c \
	../../zlib/gzio.c \
	../../zlib/infback.c \
	../../zlib/inffast.c \
	../../zlib/inflate.c \
	../../zlib/inftrees.c \
	../../zlib/trees.c \
	../../zlib/uncompr.c \
	../../zlib/zutil.c \

#LOCAL_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)



include $(CLEAR_VARS)

LOCAL_CFLAGS += -I../

LOCAL_MODULE    := libZnk
LOCAL_SRC_FILES := \
	../../dll_main.c \
	../../Znk_bfr.c \
	../../Znk_bfr_ary.c \
	../../Znk_cookie.c \
	../../Znk_dir.c \
	../../Znk_dirent.c \
	../../Znk_dlink.c \
	../../Znk_envvar.c \
	../../Znk_err.c \
	../../Znk_fdset.c \
	../../Znk_htp_hdrs.c \
	../../Znk_htp_rar.c \
	../../Znk_math.c \
	../../Znk_md5.c \
	../../Znk_mem_find.c \
	../../Znk_missing_libc.c \
	../../Znk_mutex.c \
	../../Znk_myf.c \
	../../Znk_net_base.c \
	../../Znk_net_ip.c \
	../../Znk_obj_ary.c \
	../../Znk_prim.c \
	../../Znk_server.c \
	../../Znk_socket.c \
	../../Znk_stdc.c \
	../../Znk_stock_bio.c \
	../../Znk_str.c \
	../../Znk_str_ary.c \
	../../Znk_str_ex.c \
	../../Znk_str_fio.c \
	../../Znk_str_ptn.c \
	../../Znk_sys_errno.c \
	../../Znk_s_atom.c \
	../../Znk_s_base.c \
	../../Znk_s_posix.c \
	../../Znk_thread.c \
	../../Znk_txt_filter.c \
	../../Znk_var.c \
	../../Znk_varp_ary.c \
	../../Znk_yy_base.c \
	../../Znk_zlib.c \

#LOCAL_LDLIBS := -lz
LOCAL_SHARED_LIBRARIES += libz

include $(BUILD_SHARED_LIBRARY)

