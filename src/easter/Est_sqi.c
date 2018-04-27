#include <Est_sqi.h>
#include <Est_tag.h>

#include <Rano_sset.h>

#include <Znk_date.h>
#include <Znk_str_ary.h>
#include <Znk_str_ex.h>
#include <Znk_mem_find.h>
#include <stdio.h>

struct EstSQIImpl {
	char      fmt_version_[ 4 ];
	ZnkStr    search_path_;
	ZnkStrAry file_ext_;
	ZnkStrAry tags_;
	ZnkStr    tags_method_;
	size_t    tags_num_min_;
	size_t    tags_num_max_;
	ZnkStr    keyword_;
	size_t    keyword_occ_tbl_[ 256 ];
	ZnkDate   date_;
	ZnkStr    time_method_;
	size_t    size_;
	ZnkStr    size_method_;
	ZnkStr    result_msg_;
};

const char*
EstSQI_search_path( const EstSQI sqi )
{
	return ZnkStr_cstr( sqi->search_path_ );
}
ZnkStrAry
EstSQI_file_ext( const EstSQI sqi )
{
	return sqi->file_ext_;
}

ZnkStrAry
EstSQI_tags( const EstSQI sqi )
{
	return sqi->tags_;
}
size_t
EstSQI_tags_num_min( const EstSQI sqi )
{
	return sqi->tags_num_min_;
}
size_t
EstSQI_tags_num_max( const EstSQI sqi )
{
	return sqi->tags_num_max_;
}
bool
EstSQI_is_tags_method_or( const EstSQI sqi )
{
	return ZnkStr_eq( sqi->tags_method_, "or" );
}
bool
EstSQI_isMatchTags( const EstSQI sqi, const ZnkStrAry tags_list )
{
	bool found = false;
	const bool      is_or         = EstSQI_is_tags_method_or( sqi );
	const ZnkStrAry sqi_tags      = sqi->tags_;
	const size_t    qtg_size      = ZnkStrAry_size( sqi_tags );
	const size_t    tags_list_num = ZnkStrAry_size( tags_list );
	size_t          qtg_idx;

	if( tags_list_num < sqi->tags_num_min_ || tags_list_num > sqi->tags_num_max_){
		/* tagの個数の不適合 */
		return false;
	}

	if( is_or ){
		/* OR検索 */
		found = false;
		for( qtg_idx=0; qtg_idx<qtg_size; ++qtg_idx ){
			const char* query_tag = ZnkStrAry_at_cstr( sqi_tags, qtg_idx );
			if( ZnkStrAry_find( tags_list, 0, query_tag, Znk_strlen(query_tag) ) != Znk_NPOS ){
				/* found : 合格 */
				return true;
			}
		}
	} else {
		/* AND検索 */
		found = true;
		for( qtg_idx=0; qtg_idx<qtg_size; ++qtg_idx ){
			const char* query_tag = ZnkStrAry_at_cstr( sqi_tags, qtg_idx );
			if( ZnkStrAry_find( tags_list, 0, query_tag, Znk_strlen(query_tag) ) == Znk_NPOS ){
				/* found : 不適合 */
				return false;
			}
		}
	}
	return found;
}
ZnkDate
EstSQI_date( const EstSQI sqi )
{
	return sqi->date_;
}
bool
EstSQI_isMatchDate( const EstSQI sqi, const ZnkDate* date )
{
	const int compare_day = ZnkDate_compareDay( date, &sqi->date_ );
	if( ZnkStr_eq(sqi->time_method_, "old" ) && compare_day > 0 ){
		return false; /* 不適合 */
	}
	if( ZnkStr_eq(sqi->time_method_, "new" ) && compare_day < 0 ){
		return false; /* 不適合 */
	}
	if( ZnkStr_eq(sqi->time_method_, "eq" ) && compare_day != 0 ){
		return false; /* 不適合 */
	}
	return true; /* 合格 */
}
size_t
EstSQI_size( const EstSQI sqi )
{
	return sqi->size_;
}
bool
EstSQI_isMatchSize( const EstSQI sqi, uint64_t file_size64u )
{
	const size_t sqi_size = sqi->size_;
	if( ZnkStr_eq(sqi->size_method_, "big" ) && file_size64u < sqi_size ){
		return false; /* 不適合 */
	}
	if( ZnkStr_eq(sqi->size_method_, "small" ) && file_size64u > sqi_size ){
		return false; /* 不適合 */
	}
	if( ZnkStr_eq(sqi->size_method_, "eq" ) && file_size64u != sqi_size ){
		return false; /* 不適合 */
	}
	return true; /* 合格 */
}
const char*
EstSQI_keyword( const EstSQI sqi )
{
	return ZnkStr_cstr( sqi->keyword_ );
}
ZnkStr
EstSQI_result_msg( const EstSQI sqi )
{
	return sqi->result_msg_;
}

EstSQI
EstSQI_create( ZnkVarpAry post_vars )
{
	EstSQI info = Znk_alloc0( sizeof(struct EstSQIImpl) );
	ZnkVarp pv;
	ZnkStr tags_seq = ZnkStr_new( "" );

	ZnkS_copy( info->fmt_version_, sizeof(info->fmt_version_), "1", Znk_NPOS );

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_topdir", false );
	if( pv ){
		info->search_path_ = ZnkStr_new( ZnkVar_cstr( pv ) );
	} else {
		info->search_path_ = ZnkStr_new( "" );
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_file_ext", false );
	if( pv ){
		const char* file_ext = ZnkVar_cstr( pv );
		info->file_ext_ = ZnkStrAry_create( true );
		ZnkStrEx_addSplitC( info->file_ext_,
				file_ext, Znk_NPOS,
				' ', false, 4 );
	} else {
		info->file_ext_ = ZnkStrAry_create( true );
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_tags", false );
	info->tags_ = ZnkStrAry_create( true );
	EstTag_joinTagIDs( tags_seq, post_vars, ' ' );
	if( pv ){
		const char* tags = ZnkVar_cstr( pv );
		RanoSSet_addSeq( tags_seq, tags );
		ZnkStrEx_addSplitC( info->tags_,
				ZnkStr_cstr(tags_seq), Znk_NPOS,
				' ', false, 4 );
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_tags_method", false );
	if( pv ){
		info->tags_method_ = ZnkStr_new( ZnkVar_cstr( pv ) );
	} else {
		info->tags_method_ = ZnkStr_new( "" );
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_tags_num", false );
	if( pv ){
		const char* tags_num = ZnkVar_cstr( pv );
		const char* q = Znk_strchr( tags_num, ':' );
		if( q ){
			/***
			 * min:max という形式で指定されている場合
			 * 特に「min:」と「:」以降が省略された形なら「min 以上」を示す.
			 * また「:max」と「:」以前が省略された形なら「max 以下」を示す(「0:max」と同じことである).
			 * また「:」の場合は全範囲である.
			 */
			char buf[ 256 ] = "";
			ZnkS_copy( buf, sizeof(buf), tags_num, q-tags_num );
			if( !ZnkS_getSzU( &info->tags_num_min_, buf ) ){
				info->tags_num_min_ = 0;
			}
			if( !ZnkS_getSzU( &info->tags_num_max_, q+1 ) ){
				info->tags_num_max_ = Znk_NPOS;
			}
		} else {
			/***
			 * num という形式で指定されている場合
			 * 「num以上num以下」すなわちnumの場合のみが対象.
			 * 空の場合は全範囲である.
			 */
			if( ZnkS_getSzU( &info->tags_num_min_, tags_num ) ){
				info->tags_num_max_ = info->tags_num_min_;
			} else {
				info->tags_num_min_ = 0;
				info->tags_num_max_ = Znk_NPOS;
			}
		}
	} else {
		info->tags_num_min_ = 0;
		info->tags_num_max_ = Znk_NPOS;
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_keyword", false );
	if( pv ){
		info->keyword_ = ZnkStr_new( ZnkVar_cstr( pv ) );
		ZnkMem_getLOccTable_forBMS( info->keyword_occ_tbl_, (uint8_t*)ZnkStr_cstr(info->keyword_), ZnkStr_leng(info->keyword_) );
	} else {
		info->keyword_ = ZnkStr_new( "" );
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_time", false );
	if( pv ){
		ZnkStrAry tkns = ZnkStrAry_create( true );
		const char* time_str = ZnkVar_cstr( pv );
		const char* p = Znk_strchr( time_str, ' ' );
		{
			int year   = 0;
			int month  = 0;
			int day    = 0;
			sscanf( time_str, "%d/%d/%d", &year, &month, &day );
			ZnkDate_set_year(  &info->date_, (uint16_t)year );
			ZnkDate_set_month( &info->date_, (uint8_t)month );
			ZnkDate_set_day(   &info->date_, (uint8_t)day );
		}
		if( p ){
			int hour   = 0;
			int minute = 0;
			int second = 0;
			while( *p == ' ' ){ ++p; }
			sscanf( p,        "%d:%d:%d", &hour, &minute, &second );
			ZnkDate_set_hour(   &info->date_, (uint8_t)hour );
			ZnkDate_set_minute( &info->date_, (uint8_t)minute );
			ZnkDate_set_second( &info->date_, (uint8_t)second );
		}
		ZnkStrAry_destroy( tkns );
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_time_method", false );
	if( pv ){
		info->time_method_ = ZnkStr_new( ZnkVar_cstr( pv ) );
	} else {
		info->time_method_ = ZnkStr_new( "" );
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_size", false );
	if( pv ){
		const char* size_str = ZnkVar_cstr( pv );
		if( !ZnkS_getSzU( &info->size_, size_str ) ){
			info->size_ = Znk_NPOS;
		}
	} else {
		info->size_ = Znk_NPOS;
	}

	pv = ZnkVarpAry_find_byName_literal( post_vars, "EstSM_size_method", false );
	if( pv ){
		info->size_method_ = ZnkStr_new( ZnkVar_cstr( pv ) );
	} else {
		info->size_method_ = ZnkStr_new( "" );
	}

	info->result_msg_ = ZnkStr_new( "" );

	ZnkStr_delete( tags_seq );
	return info;
}
void
EstSQI_destroy( EstSQI info )
{
	if( info ){
		ZnkStr_delete( info->search_path_ );
		ZnkStrAry_destroy( info->file_ext_ );
		ZnkStrAry_destroy( info->tags_ );
		ZnkStr_delete( info->tags_method_ );
		ZnkStr_delete( info->keyword_ );
		ZnkStr_delete( info->time_method_ );
		ZnkStr_delete( info->size_method_ );
		ZnkStr_delete( info->result_msg_ );
		Znk_free( info );
	}
}
bool
EstSQI_convert_toSQIVars( ZnkVarpAry sqi_vars, EstSQI sqi )
{
	if( sqi_vars ){
		ZnkVarp var = NULL;
		ZnkStr dst_str = NULL;
		ZnkVarpAry_clear( sqi_vars );

		var = ZnkVarp_create( "fmt_version", "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( var, sqi->fmt_version_, Znk_strlen( sqi->fmt_version_ ) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_topdir", "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( var, ZnkStr_cstr( sqi->search_path_ ), ZnkStr_leng( sqi->search_path_ ) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_file_ext", "", 0, ZnkPrim_e_Str, NULL );
		dst_str = ZnkVar_str( var );
		ZnkStrEx_addJoin( dst_str, sqi->file_ext_,
				0, Znk_NPOS,
				" ", 1, ZnkStrAry_size(sqi->file_ext_) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_tags", "", 0, ZnkPrim_e_Str, NULL );
		dst_str = ZnkVar_str( var );
		ZnkStrEx_addJoin( dst_str, sqi->tags_,
				0, Znk_NPOS,
				" ", 1, ZnkStrAry_size(sqi->tags_) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_tags_method", "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( var, ZnkStr_cstr( sqi->tags_method_ ), ZnkStr_leng( sqi->tags_method_ ) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_tags_num", "", 0, ZnkPrim_e_Str, NULL );
		dst_str = ZnkVar_str( var );
		if( sqi->tags_num_max_ == Znk_NPOS ){
			if( sqi->tags_num_min_ == 0 || sqi->tags_num_min_ == Znk_NPOS ){
				/* 空とする(全範囲) */
				ZnkStr_clear( dst_str );
			} else {
				/* 「min:」形式(min以上) */
				ZnkStr_setf( dst_str, "%zu:", sqi->tags_num_min_ );
			}
		} else if( sqi->tags_num_min_ == sqi->tags_num_max_ ){
			/* 単一「num」形式(numそのもの) */
			ZnkStr_setf( dst_str, "%zu", sqi->tags_num_min_ );
		} else if( sqi->tags_num_min_ < sqi->tags_num_max_ ){
			/* 「min:max」形式(min以上max以下) */
			ZnkStr_setf( dst_str, "%zu:%zu", sqi->tags_num_min_, sqi->tags_num_max_ );
		} else {
			/* 不正な範囲の時は空とする(全範囲) */
			ZnkStr_clear( dst_str );
		}
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_keyword", "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( var, ZnkStr_cstr( sqi->keyword_ ), ZnkStr_leng( sqi->keyword_ ) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_time", "", 0, ZnkPrim_e_Str, NULL );
		dst_str = ZnkVar_str( var );
		ZnkDate_getStr( dst_str, 0, &sqi->date_, ZnkDateStr_e_Std );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_time_method", "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( var, ZnkStr_cstr( sqi->time_method_ ), ZnkStr_leng( sqi->time_method_ ) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_size", "", 0, ZnkPrim_e_Str, NULL );
		dst_str = ZnkVar_str( var );
		if( sqi->size_ == Znk_NPOS ){
			ZnkStr_clear( dst_str );
		} else {
			ZnkStr_setf( dst_str, "%zu", sqi->size_ );
		}
		ZnkVarpAry_push_bk( sqi_vars, var );

		var = ZnkVarp_create( "EstSM_size_method", "", 0, ZnkPrim_e_Str, NULL );
		ZnkVar_set_val_Str( var, ZnkStr_cstr( sqi->size_method_ ), ZnkStr_leng( sqi->size_method_ ) );
		ZnkVarpAry_push_bk( sqi_vars, var );

		{
			ZnkStr EstSM_result = sqi->result_msg_;
			var = ZnkVarp_create( "EstSM_result", "", 0, ZnkPrim_e_Str, NULL );
			ZnkVar_set_val_Str( var, ZnkStr_cstr( EstSM_result ), ZnkStr_leng( EstSM_result ) );
			ZnkVarpAry_push_bk( sqi_vars, var );
		}
		return true;
	}
	return false;
}
void
EstSQIVars_makeVars( ZnkVarpAry sqi_vars,
		const char* topdir, const char* file_ext,
		const char* tags, const char* tags_method, const char* tags_num,
		const char* keyword,
		const char* time, const char* time_method,
		const char* size, const char* size_method )
{
	ZnkVarp var = NULL;
	ZnkVarpAry_clear( sqi_vars );

	var = ZnkVarp_create( "fmt_version", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, "1", Znk_NPOS );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_topdir", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, topdir, Znk_strlen(topdir) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_file_ext", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, file_ext, Znk_strlen(file_ext) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_tags", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, tags,  Znk_strlen(tags) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_tags_method", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, tags_method,  Znk_strlen(tags_method) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_tags_num", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, tags_num,  Znk_strlen(tags_num) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_keyword", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, keyword, Znk_strlen(keyword) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_time", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, time, Znk_strlen(time) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_time_method", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, time_method, Znk_strlen(time_method) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_size", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, size, Znk_strlen(size) );
	ZnkVarpAry_push_bk( sqi_vars, var );

	var = ZnkVarp_create( "EstSM_size_method", "", 0, ZnkPrim_e_Str, NULL );
	ZnkVar_set_val_Str( var, size_method, Znk_strlen(size_method) );
	ZnkVarpAry_push_bk( sqi_vars, var );
}

