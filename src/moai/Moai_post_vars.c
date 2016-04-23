#include "Moai_post_vars.h"
#include <Znk_varp_dary.h>
#include <Znk_stdc.h>
#include <Znk_missing_libc.h>
#include <Znk_s_base.h>
#include <assert.h>
#include <string.h>


ZnkVarp
MoaiPostVars_regist( ZnkVarpDAry vars, const char* name, const char* filename,
		MoaiPostVarType type, const uint8_t* data, size_t data_size )
{
	ZnkVarp varp = ZnkVarp_create( name, filename, type, ZnkPrim_e_None );
	switch( type ){
	case MoaiPostVar_e_BinaryData:
		ZnkVar_set_val_Bfr( varp, data, data_size );
		break;
	case MoaiPostVar_e_None:
	default:
		ZnkVar_set_val_Str( varp, (char*)data, data_size );
		break;
	}
	ZnkVarpDAry_push_bk( vars, varp );
	return varp;
}

bool
MoaiPostVars_regist_byHttpBody( ZnkVarpDAry vars,
		const char* boundary, size_t boundary_leng,
		const uint8_t* body, size_t body_size )
{
	const uint8_t* p = body;
	const uint8_t* q = NULL;
	const uint8_t* r = NULL;
	const uint8_t* end = body + body_size;
	bool result = false;
	bool is_upfile = false;

	p = Znk_memmem( p, end-p, boundary, boundary_leng );
	if( p == NULL ){
		/* not multitype/form-data or invalid boundary */
		return false;
	} else {

		ZnkVarp varp = NULL;
		ZnkStr  cntype = ZnkStr_new( "" );

		p += boundary_leng;
		p += 2; /* skip \r\n or -- */

		while( end-p ){
			if( !ZnkS_isCaseBegin_literal( (char*)p, "Content-Disposition: form-data;" ) ){
				char dbg_buf[ 16 ];
				ZnkS_copy( dbg_buf, sizeof(dbg_buf), (char*)p, end-p );
				ZnkF_printf_e( "MoaiPostInfo_regist_byHttpBody : Not Content-Disposition p=[%p][%s]\n",
						p, dbg_buf );
				goto FUNC_END;
			}
			p += 31; /* skip Content-Disposition; form-data; */

			/* 改行までの位置を予め得ておく */
			r = Znk_memstr_literal( p, end-p, "\r\n" );
	
			varp = ZnkVarp_create( "", "", MoaiPostVar_e_None, ZnkPrim_e_None );

			/* これは必須である */
			p = Znk_memstr_literal( p, r-p, "name=" );
			if( p ){
				p += Znk_strlen_literal( "name=" ); /* skip keyword */
				if( *p == '"' ){ ++p; } else { assert(0); } /* skip quote */
				q = Znk_memstr_literal( p, r-p, "\"" );
				ZnkStr_assign( varp->name_, 0, (char*)p, q-p );
				p += q-p+1; /* after end quote */
			} else {
				assert(0);
			}
	
			/* これはオプションである */
			is_upfile = false;
			q = Znk_memstr_literal( p, r-p, "filename=" );
			if( q ){
				p += q-p+Znk_strlen_literal( "filename=" ); /* skip keyword */
				if( *p == '"' ){ ++p; } else { assert(0); } /* skip quote */
				q = Znk_memstr_literal( p, r-p, "\"" );
				ZnkStr_assign( varp->filename_, 0, (char*)p, q-p );
				p += q-p+1; /* after end quote */
				is_upfile = true;
			}
	
			/* next line */
			p += r-p+2;
			/* 改行までの位置を予め得ておく */
			r = Znk_memstr_literal( p, end-p, "\r\n" );

			if( ZnkS_isCaseBegin_literal( (char*)p, "Content-Type: " ) ){
				p += Znk_strlen_literal( "Content-Type: " ); /* skip Content-Type:  */
				ZnkStr_assign( cntype, 0, (char*)p, r-p );
				/***
				 * このcntypeの値はimage/png, image/jpegなど様々な値をとり得る.
				 * filenameが何も指定されていない場合などはapplication/octet-streamとなっていたりもする.
				 * しかしそもそも中身がなんであれ(たとえtext/plainであれ),
				 * fileのuploadの時点でその値はBinaryDataとみなすべきである.
				 */
				if( is_upfile ){
					varp->type_ = MoaiPostVar_e_BinaryData;
				}
				/* next line */
				p += r-p+2;
				/* 改行までの位置を予め得ておく */
				r = Znk_memstr_literal( p, end-p, "\r\n" );
			}

			/* ここに一つ空行が存在する. */
			/* next line */
			p += r-p+2;
			/* 改行までの位置を予め得ておく */
			r = Znk_memstr_literal( p, end-p, "\r\n" );
	
			/***
			 * 次にboundaryが現れるまでが実際の値である
			 * (正確には後ろに\r\nが付加されているものが取得されるのでこれを除去するため、-2を行う.)
			 *
			 * 尚、この箇所のみ、pを探索する範囲はbodyの最後までにしなければならないことにも注意する.
			 */
			q = Znk_memmem( p, end-p, boundary, boundary_leng );
			if( q ){
				if( varp->type_ == MoaiPostVar_e_BinaryData ){
					ZnkVar_set_val_Bfr( varp, p, q-p-2 );
				} else {
					ZnkVar_set_val_Str( varp, (char*)p, q-p-2 );
				}
				p += q-p+boundary_leng; /* after boundary */
				if( end-p && ZnkS_isBegin( (char*)p, "--" ) ){
					p += 2; /* skip -- */
				}
				if( end-p && ZnkS_isBegin( (char*)p, "\r\n" ) ){
					p += 2; /* skip \r\n */
				}
			} else {
				assert( 0 );
			}

			/* この varp の寿命は info->vars_の管理下に置かれる */
			ZnkVarpDAry_push_bk( vars, varp );
		}

		result = true;
FUNC_END:
		ZnkStr_delete( cntype );
	}
	return result;
}

