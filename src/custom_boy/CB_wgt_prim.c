#include <CB_wgt_prim.h>

#include <Znk_stdc.h>
#include <Znk_myf.h>
#include <Znk_rand.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct CBWgtPrimImpl {
	struct ZnkPrim_tag prim_;
	double  rate_;
};

void
CBWgtRand_init( void )
{
	ZnkRand_shuffle( 0 );
}
double
CBWgtRand_getRandomReal( double min_real, double max_real )
{
	return ZnkRand_getRandR( min_real, max_real );
}


CBWgtPrim
CBWgtPrim_create( ZnkPrimType pm_type, double rate, ZnkElemDeleterFunc elem_deleter )
{
	CBWgtPrim wpm = (CBWgtPrim)Znk_malloc( sizeof(struct CBWgtPrimImpl) );
	ZnkPrim_set_null( &wpm->prim_ );
	ZnkPrim_compose( &wpm->prim_, pm_type, elem_deleter );
	wpm->rate_ = rate;
	return wpm;
}

void
CBWgtPrim_destroy( CBWgtPrim wpm )
{
	if( wpm ){
		ZnkPrim_dispose( &wpm->prim_ );
		Znk_free( wpm );
	}
}

ZnkPrim
CBWgtPrim_prim( CBWgtPrim wpm )
{
	return &wpm->prim_;
}
double
CBWgtPrim_rate( const CBWgtPrim wpm )
{
	return wpm->rate_;
}

static void
deleteElem( void* elem ){
	CBWgtPrim_destroy( (CBWgtPrim)elem );
}
CBWgtPrimAry
CBWgtPrimAry_create( bool elem_responsibility )
{
	ZnkElemDeleterFunc deleter = elem_responsibility ? deleteElem : NULL;
	return (CBWgtPrimAry)ZnkObjAry_create( deleter );
}

CBWgtPrim
CBWgtPrimAry_registStr( CBWgtPrimAry ary, const char* str, double rate )
{
	CBWgtPrim wpm = CBWgtPrim_create( ZnkPrim_e_Str, rate, NULL );
	ZnkStr_set( ZnkPrim_str(&wpm->prim_), str );
	CBWgtPrimAry_push_bk( ary, wpm );
	return wpm;
}

ZnkPrim
CBWgtPrimAry_select( const CBWgtPrimAry ary )
{
	const size_t size    = CBWgtPrimAry_size( ary );
	double rnd_val = 0.0;
	const CBWgtPrim* wpm_ap = CBWgtPrimAry_ary_ptr( (CBWgtPrimAry)ary );
	size_t idx;
	double rate = 0.0;
	size_t re_count = 0;
	size_t re_idx = 0;

	/***
	 * 確率が与えられている項目からまず選択.
	 */
	rnd_val = CBWgtRand_getRandomReal( 0.0, 1.0 );
	for( idx=0; idx<size; ++idx ){
		const CBWgtPrim wpm = wpm_ap[ idx ];
		if( wpm->rate_ == -1.0 ){
			++re_count;
		} else {
			rate += wpm->rate_;
			if( rnd_val < rate ){
				return (ZnkPrim)( &wpm->prim_ );
			}
		}
	}

	/***
	 * 確率が与えられていない(内部値としては-1.0となっている)項目から
	 * 等確率で選択.
	 */
	if( re_count ){
		rnd_val = CBWgtRand_getRandomReal( 0.0, 1.0 );
		re_idx = (size_t)( re_count * rnd_val );
		if( re_idx == re_count ){
			--re_idx;
		}
	}
	re_count = 0;
	for( idx=0; idx<size; ++idx ){
		const CBWgtPrim wpm = wpm_ap[ idx ];
		if( wpm->rate_ == -1.0 ){
			if( re_idx == re_count ){
				return (ZnkPrim)( &wpm->prim_ );
			}
			++re_count;
		}
	}
	return NULL;
}
