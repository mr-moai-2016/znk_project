#include "Znk_bird.h"
#include "Znk_mem_find.h"
#include "Znk_stdc.h"
#include "Znk_varp_ary.h"
#include "Znk_bms_find.h"

/***
 * ZnkBird (Basic Intrinsic Replacement Directive)
 */

struct ZnkBirdImpl {
	ZnkVarpAry         varp_ary_;
	ZnkBmsFinder       key_finder_;
	ZnkBirdProcessFunc func_;
	void*              func_arg_;
	ZnkStr             wk_key_;
	ZnkStr             wk_dst_;
};

ZnkBird
ZnkBird_create( const char* key_beginer, const char* key_ender )
{
	ZnkBird info = Znk_malloc( sizeof( struct ZnkBirdImpl ) );
	info->varp_ary_   = ZnkVarpAry_create( true );
	info->key_finder_ = ZnkBmsFinder_create( key_beginer, key_ender );
	info->func_       = NULL;
	info->func_arg_   = NULL;
	info->wk_key_     = ZnkStr_new( "" );
	info->wk_dst_     = ZnkStr_new( "" );
	return info;
}
void
ZnkBird_destroy( ZnkBird info )
{
	if( info ){
		ZnkBmsFinder_destroy( info->key_finder_ );
		ZnkVarpAry_destroy( info->varp_ary_ );
		ZnkStr_delete( info->wk_key_ );
		ZnkStr_delete( info->wk_dst_ );
		Znk_free( info );
	}
}
void
ZnkBird_regist( ZnkBird info, const char* key, const char* val )
{
	static const bool use_eqCase = false;
	ZnkVarp var = ZnkVarpAry_find_byName( info->varp_ary_,
			key, Znk_strlen(key), use_eqCase );
	if( var ){
		ZnkVar_set_val_Str( var, val, Znk_strlen(val) );
	} else {
		ZnkVarp new_var = ZnkVarp_create( key, "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( new_var, val, Znk_strlen(val) );
		ZnkVarpAry_push_bk( info->varp_ary_, new_var );
	}
}

void
ZnkBird_setFunc( ZnkBird info, ZnkBirdProcessFunc func, void* func_arg )
{
	info->func_     = func;
	info->func_arg_ = func_arg;
}

const char*
ZnkBird_at( ZnkBird info, const char* key )
{
	static const bool use_eqCase = false;
	ZnkVarp var = ZnkVarpAry_find_byName( info->varp_ary_,
			key, Znk_strlen(key), use_eqCase );
	return var ? ZnkVar_cstr( var ) : NULL;
}
const bool
ZnkBird_exist( ZnkBird info, const char* key )
{
	static const bool use_eqCase = false;
	ZnkVarp var = ZnkVarpAry_find_byName( info->varp_ary_,
			key, Znk_strlen(key), use_eqCase );
	return (bool)( var != NULL );
}
void
ZnkBird_clear( ZnkBird info )
{
	ZnkVarpAry_clear( info->varp_ary_ );
}

/***
 * #[key]# Directiveが埋め込まれた文字列を展開する.
 *
 * この処理はsrcが頻繁に変更されるような処理に向いている.
 * 一方で、srcは初期化後固定であり、info内のhashのvalの値(keyは不変)が頻繁に変わるような処理の場合、
 * この関数でも一応対応できるが、前処理によりもっと高速な実装を考えることができる.
 * これはMsgObjというモジュールで将来実装予定.
 *
 * 尚、dstとsrcは同一のZnkStrであってはならない.
 */
bool ZnkBird_extend( const ZnkBird info,
		ZnkStr dst, const char* src, size_t expected_size )
{
	/***
	 * コード上はkey_beginer, key_ender という形で抽象化してあるが、
	 * コメントでは便宜上、#[, ]# を使う.
	 */
	const size_t key_beginer_size = ZnkBmsFinder_begin_ptn_leng( info->key_finder_ );
	const size_t key_ender_size   = ZnkBmsFinder_end_ptn_leng(   info->key_finder_ );
	const size_t src_leng = Znk_strlen( src );

	bool status = true;
	bool result = true;
	size_t src_pos = 0;
	size_t btwn_begin;
	size_t btwn_leng;
	ZnkStr key = info->wk_key_;

	if( ZnkS_empty( src ) ){
		return true;
	}
	if( src_leng <= key_beginer_size ){
		ZnkStr_add( dst, src );
		return true;
	}

	ZnkBfr_reserve( key, 16 );
	if( expected_size == 0 ){
		ZnkBfr_reserve( dst, src_leng + ZnkBfr_size( dst ) );
	} else {
		ZnkBfr_reserve( dst, expected_size + ZnkBfr_size( dst ) );
	}

	while( true ){
		if( src_pos >= src_leng ){
			/* 終端に到達した. */
			break;
		}

		/***
		 * 現在の位置(pos) から #[ ... ]# が存在する範囲を調べる.
		 *   ....#[....]#
		 *   ^     ^   ^
		 *   p     b   e
		 *
		 * p = src_pos
		 * b = p + btwn_begin
		 * e = p + btwn_begin + btwn_leng
		 */
		if( ZnkBmsFinder_getBetweenRange( info->key_finder_,
				src+src_pos, src_leng-src_pos, 0, &btwn_begin, &btwn_leng ) )
		{
			if( btwn_leng == Znk_NPOS ){
				/**
				 * #[ は登場しているが、]# が登場せずに終端を迎えている.
				 * この場合 #[ も含めそのまま追加.
				 */
				const char* key_ender = ZnkBmsFinder_end_ptn( info->key_finder_ );
				ZnkStr_append( dst, src + src_pos, src_leng - src_pos );
				Znk_printf_e( "ZnkBird_extend : Error : [%s] src_leng=[%zu] src_pos=[%zu] key_ender=[%s] key_ender_size=[%zu]\n",
						src+src_pos, src_leng, src_pos, key_ender, key_ender_size );
				return false;
			}
			/**
			 * 現在の位置(pos) から #[ が始まる前までの範囲を dst へ追加する.
			 */
			ZnkStr_append( dst, src + src_pos, btwn_begin-key_beginer_size );
		} else {
			/* #[ が存在しない場合は、残りをそのまま全部追加し、終了する */
			ZnkStr_append( dst, src + src_pos, src_leng - src_pos );
			return result;
		}

		/***
		 * 新しいsrc_posへ更新.
		 * ....#[....]#
		 *       ^
		 *       p
		 */
		src_pos += btwn_begin;
		/***
		 * src における Betweenの範囲を keyに代入する.
		 */
		ZnkStr_assign( key, 0, src + src_pos, btwn_leng );

		/***
		 * key を元に処理を行う.
		 */
		if( info->func_ ){
			/***
			 * 該当するkeyが存在しない場合でもカスタム処理したい場合がある.
			 * そのため、ここでは存在性のチェックもスキップし、func_を即時呼ぶ仕様とした.
			 *
			 * 例えば環境変数から展開したい場合などは、予めkeyをregistしておくのが難しくなる.
			 * これはどのようなkeyが実行環境で設定されているのかを予め取得できる移植性のある
			 * 方法が存在しないためである.
			 * その場合、keyが与えられた後にgetenvで取得を試みるなど、遅延的なkey決定処理を
			 * 行う必要がある.
			 */
			status = (*info->func_)( info, dst, src, ZnkStr_cstr(key), src_pos, info->func_arg_ );
		} else {
			if( ZnkBird_exist( info, ZnkStr_cstr(key) ) ){
				ZnkStr_add( dst, ZnkBird_at( info, ZnkStr_cstr(key) ) );
				status = true;
			} else {
				status = false;
			}
		}
		if( !status ){
			result = false;
			/***
			 * #[...]# の部分をそのまま追加.
			 */
			ZnkStr_append( dst, src + src_pos-key_beginer_size,
					key_beginer_size + btwn_leng + key_ender_size );
		}

		/***
		 * 新しいsrc_posへ更新.
		 * ...]#...
		 *      ^
		 *      p
		 */
		src_pos += btwn_leng+key_ender_size;
	}
	return result;
}

bool ZnkBird_extend_self( const ZnkBird info, ZnkStr self, size_t expected_size )
{
	bool result;
	ZnkStr dst = info->wk_dst_;
	ZnkStr_clear( dst );
	result = ZnkBird_extend( info, dst, ZnkStr_cstr(self), expected_size );
	ZnkBfr_swap( dst, self );
	return result;
}


