# It should be ndk root directory(which contains jni directory.)
LOCAL_PATH := $(call my-dir)/..
S:=..

ifndef MY_LIBS_ROOT
  MY_LIBS_ROOT := ../..
endif

# Declaring a prebuilt sub-static-library module
include $(CLEAR_VARS)
LOCAL_MODULE := zlib
LOCAL_SRC_FILES := $S/zlib/mkf_android/obj/local/$(TARGET_ARCH_ABI)/libzlib.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := gslconv
LOCAL_CFLAGS += -I.. \

LOCAL_SRC_FILES := \
	$S/Znk_algo_vec.c \
	$S/Znk_bfr.c \
	$S/Znk_bfr_ary.c \
	$S/Znk_bfr_bif.c \
	$S/Znk_bird.c \
	$S/Znk_bms_find.c \
	$S/Znk_cookie.c \
	$S/Znk_date.c \
	$S/Znk_dir.c \
	$S/Znk_dirent.c \
	$S/Znk_dir_recursive.c \
	$S/Znk_dir_type.c \
	$S/Znk_dlink.c \
	$S/Znk_duple_ary.c \
	$S/Znk_envvar.c \
	$S/Znk_err.c \
	$S/Znk_fdset.c \
	$S/Znk_htp_hdrs.c \
	$S/Znk_htp_post.c \
	$S/Znk_htp_rar.c \
	$S/Znk_htp_util.c \
	$S/Znk_liba_scan.c \
	$S/Znk_math.c \
	$S/Znk_mbc_jp.c \
	$S/Znk_md5.c \
	$S/Znk_mem_find.c \
	$S/Znk_missing_libc.c \
	$S/Znk_mutex.c \
	$S/Znk_myf.c \
	$S/Znk_net_base.c \
	$S/Znk_net_ip.c \
	$S/Znk_nset.c \
	$S/Znk_obj_ary.c \
	$S/Znk_prim.c \
	$S/Znk_primp_ary.c \
	$S/Znk_process.c \
	$S/Znk_rand.c \
	$S/Znk_rgx.c \
	$S/Znk_rgx_dfa.c \
	$S/Znk_rgx_nfa.c \
	$S/Znk_rgx_tree.c \
	$S/Znk_server.c \
	$S/Znk_socket.c \
	$S/Znk_stdc.c \
	$S/Znk_stock_bio.c \
	$S/Znk_str.c \
	$S/Znk_str_ary.c \
	$S/Znk_str_ex.c \
	$S/Znk_str_fio.c \
	$S/Znk_str_path.c \
	$S/Znk_str_ptn.c \
	$S/Znk_sys_errno.c \
	$S/Znk_s_atom.c \
	$S/Znk_s_base.c \
	$S/Znk_s_posix.c \
	$S/Znk_thread.c \
	$S/Znk_tostr_double.c \
	$S/Znk_tostr_int.c \
	$S/Znk_var.c \
	$S/Znk_varp_ary.c \
	$S/Znk_vsnprintf.c \
	$S/Znk_yy_base.c \
	$S/Znk_zlib.c \
	$S/gslconv.c \

# Refer to prebuilt sub-static-libraries
LOCAL_STATIC_LIBRARIES := zlib
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := Znk
LOCAL_CFLAGS += -I.. \

LOCAL_SRC_FILES := \
	$S/Znk_algo_vec.c \
	$S/Znk_bfr.c \
	$S/Znk_bfr_ary.c \
	$S/Znk_bfr_bif.c \
	$S/Znk_bird.c \
	$S/Znk_bms_find.c \
	$S/Znk_cookie.c \
	$S/Znk_date.c \
	$S/Znk_dir.c \
	$S/Znk_dirent.c \
	$S/Znk_dir_recursive.c \
	$S/Znk_dir_type.c \
	$S/Znk_dlink.c \
	$S/Znk_duple_ary.c \
	$S/Znk_envvar.c \
	$S/Znk_err.c \
	$S/Znk_fdset.c \
	$S/Znk_htp_hdrs.c \
	$S/Znk_htp_post.c \
	$S/Znk_htp_rar.c \
	$S/Znk_htp_util.c \
	$S/Znk_liba_scan.c \
	$S/Znk_math.c \
	$S/Znk_mbc_jp.c \
	$S/Znk_md5.c \
	$S/Znk_mem_find.c \
	$S/Znk_missing_libc.c \
	$S/Znk_mutex.c \
	$S/Znk_myf.c \
	$S/Znk_net_base.c \
	$S/Znk_net_ip.c \
	$S/Znk_nset.c \
	$S/Znk_obj_ary.c \
	$S/Znk_prim.c \
	$S/Znk_primp_ary.c \
	$S/Znk_process.c \
	$S/Znk_rand.c \
	$S/Znk_rgx.c \
	$S/Znk_rgx_dfa.c \
	$S/Znk_rgx_nfa.c \
	$S/Znk_rgx_tree.c \
	$S/Znk_server.c \
	$S/Znk_socket.c \
	$S/Znk_stdc.c \
	$S/Znk_stock_bio.c \
	$S/Znk_str.c \
	$S/Znk_str_ary.c \
	$S/Znk_str_ex.c \
	$S/Znk_str_fio.c \
	$S/Znk_str_path.c \
	$S/Znk_str_ptn.c \
	$S/Znk_sys_errno.c \
	$S/Znk_s_atom.c \
	$S/Znk_s_base.c \
	$S/Znk_s_posix.c \
	$S/Znk_thread.c \
	$S/Znk_tostr_double.c \
	$S/Znk_tostr_int.c \
	$S/Znk_var.c \
	$S/Znk_varp_ary.c \
	$S/Znk_vsnprintf.c \
	$S/Znk_yy_base.c \
	$S/Znk_zlib.c \
	$S/dll_main.c \

# Refer to prebuilt sub-static-libraries
LOCAL_STATIC_LIBRARIES := zlib
include $(BUILD_SHARED_LIBRARY)

