#include "Znk_str_ptn.h"
#include "Znk_missing_libc.h"
#include "Znk_stdc.h"
#include "Znk_s_base.h"
#include "Znk_mem_find.h"
#include "Znk_str_ex.h"
#include <string.h>

#define DECIDE_LENG( leng, str ) do if( (leng) == Znk_NPOS ){ (leng) = Znk_strlen(str); } while(0)

const uint8_t*
ZnkStrPtn_getBetween( ZnkStr ans, const uint8_t* data, const size_t data_size,
		const char* ptn_begin, const char* ptn_end )
{
	const size_t ptn_begin_leng = Znk_strlen( ptn_begin );
	const size_t ptn_end_leng   = Znk_strlen( ptn_end );
	const uint8_t* end = data + data_size;
	const uint8_t* p = Znk_memmem( data, data_size, ptn_begin, ptn_begin_leng );
	const uint8_t* q = NULL;
	if( p ){
		p += ptn_begin_leng;
		q = Znk_memmem( p, end-p, ptn_end, ptn_end_leng );
		ZnkStr_assign( ans, 0, (const char*)p, q-p );
		q += ptn_end_leng;
		return q;
	}
	return NULL;
}

bool
ZnkStrPtn_replaceBetween( ZnkStr ans,
		const uint8_t* data, size_t data_size,
		const char* ptn_begin, const char* ptn_end,
		const char* replace_str, bool replaced_with_ptn )
{
	const size_t ptn_begin_leng = Znk_strlen( ptn_begin );
	const size_t ptn_end_leng   = Znk_strlen( ptn_end );
	const uint8_t* p = data;
	const uint8_t* end = data + data_size;
	const uint8_t* b = NULL;
	const uint8_t* e = NULL;
	b = Znk_memmem( p, end-p, ptn_begin, ptn_begin_leng );
	if( b ){
		p = b + ptn_begin_leng;
		e = Znk_memmem( p, end-p, ptn_end, ptn_end_leng );
		if( e ){
			/* found */
			ZnkStr_assign( ans, 0, (const char*)data, b-data );
			if( replaced_with_ptn ){ ZnkStr_add( ans, ptn_begin ); }
			ZnkStr_add( ans, replace_str );
			if( replaced_with_ptn ){ ZnkStr_add( ans, ptn_end ); }
			p = e + ptn_end_leng;
			ZnkStr_add( ans, (const char*)p );
			return true;
		}
	}
	return false;
}

void
ZnkStrPtn_invokeInQuote( ZnkStr str,
		const char* quo_beg, const char* quo_end,
		const char* coe_beg, const char* coe_end,
		const ZnkStrPtnProcessFuncT event_handler, void* event_arg,
		bool include_quote )
{
	ZnkSRef sref_quo_beg = { 0 };
	ZnkSRef sref_quo_end = { 0 };
	ZnkSRef sref_coe_beg = { 0 };
	ZnkSRef sref_coe_end = { 0 };
	size_t  occ_quo_beg[ 256 ];
	size_t  occ_quo_end[ 256 ];
	size_t  occ_coe_beg[ 256 ];
	size_t  occ_coe_end[ 256 ];
	size_t  curp = 0;
	size_t  begp = 0;
	size_t  endp = 0;
	size_t  coeB = 0;
	size_t  coeE = 0;
	size_t  text_leng = ZnkStr_leng( str );
	ZnkStr  tmp = ZnkStr_new( "" );

	sref_quo_beg.cstr_ = quo_beg;
	sref_quo_beg.leng_ = strlen( quo_beg );
	sref_quo_end.cstr_ = quo_end;
	sref_quo_end.leng_ = strlen( quo_end );
	ZnkMem_getLOccTable_forBMS( occ_quo_beg, (uint8_t*)sref_quo_beg.cstr_,  sref_quo_beg.leng_ );
	ZnkMem_getLOccTable_forBMS( occ_quo_end, (uint8_t*)sref_quo_end.cstr_,  sref_quo_end.leng_ );

	sref_coe_beg.cstr_ = coe_beg;
	if( sref_coe_beg.cstr_ ){
		sref_coe_beg.leng_ = strlen( coe_beg );
		if( sref_coe_beg.leng_ == 0 ){
			sref_coe_beg.cstr_ = NULL;
		} else {
			ZnkMem_getLOccTable_forBMS( occ_coe_beg, (uint8_t*)sref_coe_beg.cstr_, sref_coe_beg.leng_ );
		}
	}
	sref_coe_end.cstr_ = coe_end;
	if( sref_coe_end.cstr_ ){
		sref_coe_end.leng_ = strlen( coe_end );
		if( sref_coe_end.leng_ == 0 ){
			sref_coe_end.cstr_ = NULL;
		} else {
			ZnkMem_getLOccTable_forBMS( occ_coe_end, (uint8_t*)sref_coe_end.cstr_, sref_coe_end.leng_ );
		}
	}

	while( true ){
		text_leng = ZnkStr_leng( str );
		if( curp >= text_leng ){
			break;
		}

		/* lexical-scan phase1 */
		begp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(str)+curp, text_leng-curp,
				(uint8_t*)sref_quo_beg.cstr_, sref_quo_beg.leng_, 1, occ_quo_beg );
		if( begp == Znk_NPOS ){
			break;
		}
		begp += curp + sref_quo_beg.leng_;

		/* lexical-scan phase2 */
		curp = begp;
		while( true ){
			endp = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(str)+curp, text_leng-curp,
					(uint8_t*)sref_quo_end.cstr_, sref_quo_end.leng_, 1, occ_quo_end );
			if( sref_coe_beg.cstr_ ){
				coeB = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(str)+curp, text_leng-curp,
						(uint8_t*)sref_coe_beg.cstr_, sref_coe_beg.leng_, 1, occ_coe_beg );
			} else {
				coeB = Znk_NPOS;
			}
			if( endp == Znk_NPOS ){
				endp = text_leng;
			} else {
				if( coe_end ){
					if( coeB != Znk_NPOS && coeB < endp ){
						curp += coeB + sref_coe_beg.leng_;
						coeE = ZnkMem_lfind_data_BMS( (uint8_t*)ZnkStr_cstr(str)+curp, text_leng-curp,
								(uint8_t*)sref_coe_end.cstr_, sref_coe_end.leng_, 1, occ_coe_end );
						if( coeE == Znk_NPOS ){
							endp = text_leng;
							break;
						}
						/* Comment out is recognized. Try again. */
						curp += coeE + sref_coe_end.leng_;
						continue;
					}
				} else {
					if( coeB != Znk_NPOS && coeB + sref_coe_beg.leng_ == endp ){
						/* Escape sequence is recognized. Try again. */
						curp += endp + sref_quo_end.leng_;
						continue;
					}
				}
				endp += curp;
			}
			break;
		}

		if( include_quote ){
			begp -= sref_quo_beg.leng_;
			if( endp + sref_quo_end.leng_ <= text_leng ){
				endp += sref_quo_end.leng_;
			}
		}
		ZnkStr_assign( tmp, 0, ZnkStr_cstr( str ) + begp, endp-begp );

		/* invoke event callback. */
		event_handler( tmp, event_arg );

		/* replace by the result */
		ZnkBfr_replace( str, begp, endp-begp, (uint8_t*)ZnkStr_cstr(tmp), ZnkStr_leng(tmp) );

		if( include_quote ){
			curp = begp + ZnkStr_leng(tmp);
		} else {
			curp = begp + ZnkStr_leng(tmp) + sref_quo_end.leng_;
		}
	}

	ZnkStr_delete( tmp );
}

struct PtnInfo {
	const char* old_ptn_;
	const char* new_ptn_;
	size_t delta_to_next_;
};
static int
replaceStr_BMS( ZnkStr str, void* arg )
{
	struct PtnInfo* ptn_info = Znk_force_ptr_cast( struct PtnInfo*, arg );
	const char* old_ptn        = ptn_info->old_ptn_;
	const char* new_ptn        = ptn_info->new_ptn_;
	const size_t delta_to_next = ptn_info->delta_to_next_;
	ZnkSRef sref_old;
	ZnkSRef sref_new;
	size_t occ_tbl_target[ 256 ];
	sref_old.cstr_   = old_ptn;
	sref_old.leng_   = strlen( old_ptn );
	ZnkMem_getLOccTable_forBMS( occ_tbl_target, (uint8_t*)sref_old.cstr_,   sref_old.leng_ );
	sref_new.cstr_   = new_ptn;
	sref_new.leng_   = strlen( new_ptn );

	ZnkStrEx_replace_BMS( str, 0,
			sref_old.cstr_, sref_old.leng_, occ_tbl_target,
			sref_new.cstr_, sref_new.leng_,
			Znk_NPOS, delta_to_next );
	return 0;
}

void
ZnkStrPtn_replaceInQuote( ZnkStr str,
		const char* quo_beg, const char* quo_end,
		const char* coe_beg, const char* coe_end,
		const char* old_ptn, const char* new_ptn, bool include_quote,
		size_t delta_to_next )
{
	struct PtnInfo ptn_info = { 0 };
	ptn_info.old_ptn_       = old_ptn;
	ptn_info.new_ptn_       = new_ptn;
	ptn_info.delta_to_next_ = delta_to_next;
	ZnkStrPtn_invokeInQuote( str,
			quo_beg, quo_end,
			coe_beg, coe_end,
			replaceStr_BMS, &ptn_info, include_quote );
}
