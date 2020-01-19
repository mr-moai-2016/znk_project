#ifndef INCLUDE_GUARD__Est_filter_h__
#define INCLUDE_GUARD__Est_filter_h__

#include <Rano_type.h>
#include <Znk_str.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

/* 終了タグを持たないもの(空要素タグ) */
typedef int (*EstFilterTagKind1Func_T)( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend );
/* 終了タグを持つもの(終了タグである場合はend_kindにtrueを与えて呼び出す) */
typedef int (*EstFilterTagKind2Func_T)( ZnkStr tagname, ZnkVarpAry varp_ary, void* arg, ZnkStr tagend, bool end_kind );
/* プレーンテキスト加工用 */
typedef int (*EstFilterPlaneTextFunc_T)( ZnkStr planetxt, void* arg );
/* BBSOperation付加用 */
typedef int (*EstFilterBBSOperationFunc_T)( ZnkStr txt, const char* result_filename, const char* src, ZnkStr console_msg );

typedef struct EstFilterModule_tag {
	EstFilterTagKind1Func_T on_base_;
	EstFilterTagKind2Func_T on_iframe_;
	EstFilterTagKind2Func_T on_frame_;
	EstFilterTagKind1Func_T on_img_;
	EstFilterTagKind2Func_T on_a_;
	EstFilterTagKind1Func_T on_link_;
	EstFilterTagKind2Func_T on_script_;
	EstFilterTagKind2Func_T on_form_;
	EstFilterTagKind1Func_T on_meta_;
	EstFilterTagKind2Func_T on_div_;
	EstFilterTagKind1Func_T on_input_;
	int (*on_other_)( ZnkStr text, void* arg, const char* landmarking );
	EstFilterPlaneTextFunc_T    on_plane_text_;
	EstFilterBBSOperationFunc_T on_bbs_operation_;
} EstFilterModule;

bool
EstFilter_ignoreHosts( ZnkStr str );

size_t
EstFilter_replaceToAutolink( ZnkStr planetxt, bool is_append_a_tag );

bool
EstFilter_insertBBSOperation( ZnkStr text, 
		const char* result_filename, const char* landmark, const char* src, const char* bbs_id_name, ZnkStr console_msg, ZnkStr require_js );

void
EstFilter_main( const char* result_file, const char* src, const char* target,
		RanoTextType txt_type, ZnkStr console_msg, bool save_img_cache, bool is_https );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
