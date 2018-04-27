#include "Znk_obj_ary.h"
#include "Znk_bfr.h"
#include "Znk_stdc.h"
#include "Znk_vpod.h"
#include <assert.h>


struct ZnkObjAryImpl {
	ZnkBfr zkbfr_;
	ZnkElemDeleterFunc elem_deleter_;
};


ZnkObjAry
ZnkObjAry_create( ZnkElemDeleterFunc elem_deleter )
{
	ZnkObjAry obj_ary = (ZnkObjAry)Znk_malloc( sizeof(struct ZnkObjAryImpl) );
	obj_ary->zkbfr_ = ZnkBfr_create_null();
	obj_ary->elem_deleter_ = elem_deleter;
	return obj_ary;
}
void
ZnkObjAry_destroy( ZnkObjAry obj_ary )
{
	if( obj_ary ){
		/***
		 * elem_deleter_Ç™ê›íËÇ≥ÇÍÇƒÇ¢ÇÈèÍçáÇÕóvëféıñΩÇ…ê”îCÇéùÇ¬ÉPÅ[ÉXÇ∆Ç›Ç»Çµ
		 * óvëfÇÇ∑Ç◊Çƒîjä¸.
		 */
		ZnkObjAry_clear( obj_ary );

		ZnkBfr_destroy( obj_ary->zkbfr_ );
		Znk_free( obj_ary );
	}
}

size_t
ZnkObjAry_size( const ZnkObjAry obj_ary )
{
	return ZnkBfr_size( obj_ary->zkbfr_ ) / sizeof(ZnkObj);
}

ZnkObj
ZnkObjAry_at( const ZnkObjAry obj_ary, size_t idx )
{
#if !defined(NDEBUG)
	if( idx * sizeof(ZnkObj) >= ZnkBfr_size( obj_ary->zkbfr_ ) ){
		Znk_printf_e( "ZnkObjAry_at : idx=[%zu] obj_ary=[%p]\n", idx, obj_ary );
		Znk_printf_e( "             : zkbfr=[%p]\n", obj_ary->zkbfr_ );
		Znk_printf_e( "             : zkbfr_size=[%zu]\n", ZnkBfr_size( obj_ary->zkbfr_ ) );
		assert( 0 );
	}
#endif
	return (ZnkObj)ZnkBfr_at_ptr( obj_ary->zkbfr_, idx );
}
ZnkObj*
ZnkObjAry_ary_ptr( ZnkObjAry obj_ary )
{
	return (ZnkObj*)ZnkBfr_data( obj_ary->zkbfr_ );
}
ZnkElemDeleterFunc
ZnkObjAry_getElemDeleter( const ZnkObjAry obj_ary )
{
	return obj_ary->elem_deleter_;
}

bool
ZnkObjAry_erase( ZnkObjAry obj_ary, ZnkObj query_obj )
{
	const size_t size = ZnkBfr_size( obj_ary->zkbfr_ );
	const size_t num  = size / sizeof(ZnkObj);
	ZnkObj elem;
	size_t idx;
	for( idx=0; idx<num; ++idx ){
		elem = (ZnkObj)ZnkBfr_at_ptr( obj_ary->zkbfr_, idx );
		if( elem == query_obj ){
			/* found */
			if( obj_ary->elem_deleter_ ){
				if( elem ){
					obj_ary->elem_deleter_( elem );
				}
			}
			if( idx == num-1 ){
				ZnkBfr_pop_bk_ptr( obj_ary->zkbfr_ );
			} else {
				ZnkBfr_erase_ptr( obj_ary->zkbfr_, idx*sizeof(void*), 1 );
			}
			return true;
		}
	}
	return false;
}
bool
ZnkObjAry_erase_byIdx( ZnkObjAry obj_ary, size_t idx )
{
	const size_t size = ZnkBfr_size( obj_ary->zkbfr_ );
	const size_t num  = size / sizeof(ZnkObj);
	if( idx < num ){
		if( obj_ary->elem_deleter_ ){
			ZnkObj elem = (ZnkObj)ZnkBfr_at_ptr( obj_ary->zkbfr_, idx );
			if( elem ){
				obj_ary->elem_deleter_( elem );
			}
		}
		if( idx == num-1 ){
			ZnkBfr_pop_bk_ptr( obj_ary->zkbfr_ );
		} else {
			ZnkBfr_erase_ptr( obj_ary->zkbfr_, idx*sizeof(void*), 1 );
		}
		return true;
	}
	return false;
}
size_t
ZnkObjAry_erase_withQuery( ZnkObjAry obj_ary, ZnkObjQueryFunc query_func, void* query_arg )
{
	const size_t size = ZnkBfr_size( obj_ary->zkbfr_ );
	const size_t num  = size / sizeof(ZnkObj);

	if( num && query_func ){
		size_t count = 0;
		size_t idx;
		ZnkObj  obj_ptr;
		/***
		 * Ç‡Ç¡Ç∆çÇë¨Ç»éËñ@Ç™Ç†ÇÈÇ©Ç‡ÇµÇÍÇ»Ç¢Ç™ÅAÇ±Ç±Ç≈ÇÕãtï˚å¸Ç©ÇÁ
		 * ÉVÉìÉvÉãÇ…erase_byIdxÇåƒÇ—èoÇ∑é¿ëïÇ…Ç∆ÇËÇ†Ç¶Ç∏óØÇﬂÇƒÇ®Ç≠.
		 */
		for( idx=num-1; idx>0; --idx ){
			obj_ptr = ZnkObjAry_at( obj_ary, idx );
			if( query_func( obj_ptr, query_arg ) ){
				if( ZnkObjAry_erase_byIdx( obj_ary, idx ) ){
					++count;
				}
			}
		}
		return count;
	}
	return 0;
}
bool
ZnkObjAry_pop_bk( ZnkObjAry obj_ary )
{
	const size_t size = ZnkBfr_size( obj_ary->zkbfr_ );
	const size_t num  = size / sizeof(ZnkObj);
	if( num ){
		const size_t idx = num-1;
		if( obj_ary->elem_deleter_ ){
			ZnkObj elem = (ZnkObj)ZnkBfr_at_ptr( obj_ary->zkbfr_, idx );
			if( elem ){
				obj_ary->elem_deleter_( elem );
			}
		}
		ZnkBfr_pop_bk_ptr( obj_ary->zkbfr_ );
		return true;
	}
	return false;
}
void
ZnkObjAry_clear( ZnkObjAry obj_ary )
{
	/***
	 * elem_deleter_Ç™ê›íËÇ≥ÇÍÇƒÇ¢ÇÈèÍçáÇÕóvëféıñΩÇ…ê”îCÇéùÇ¬ÉPÅ[ÉXÇ∆Ç›Ç»Çµ
	 * óvëfÇÇ∑Ç◊Çƒîjä¸.
	 */
	if( obj_ary->elem_deleter_ ){
		const size_t size = ZnkBfr_size( obj_ary->zkbfr_ );
		const size_t num  = size / sizeof(ZnkObj);
		ZnkObj elem;
		size_t idx;
		for( idx=0; idx<num; ++idx ){
			elem = (ZnkObj)ZnkBfr_at_ptr( obj_ary->zkbfr_, idx );
			if( elem ){
				obj_ary->elem_deleter_( elem );
			}
		}
	}
	ZnkBfr_resize( obj_ary->zkbfr_, 0 );
}
void
ZnkObjAry_resize( ZnkObjAry obj_ary, size_t size )
{
	if( obj_ary->elem_deleter_ ){
		const size_t old_size = ZnkVPtr_size( obj_ary->zkbfr_ );
		if( size < old_size ){
			ZnkObj elem;
			size_t idx;
			for( idx=size; idx<old_size; ++idx ){
				elem = (ZnkObj)ZnkBfr_at_ptr( obj_ary->zkbfr_, idx );
				if( elem ){
					obj_ary->elem_deleter_( elem );
				}
			}
		}
	}
	ZnkVPtr_resize_fill( obj_ary->zkbfr_, size, NULL );
}

void
ZnkObjAry_push_bk( ZnkObjAry obj_ary, ZnkObj obj )
{
	ZnkBfr_push_bk_ptr( obj_ary->zkbfr_, obj );
}
void
ZnkObjAry_set( ZnkObjAry obj_ary, size_t idx, ZnkObj obj )
{
	ZnkObj elem = ZnkVPtr_at( obj_ary->zkbfr_, idx );
	if( elem ){
		/* ãåelemÇÇ‹Ç∏deleteÇ∑ÇÈïKóvÇ™Ç†ÇÈ. */
		if( obj_ary->elem_deleter_ ){
			obj_ary->elem_deleter_( elem );
		}
	}
	ZnkVPtr_set( obj_ary->zkbfr_, idx, obj );
}

void
ZnkObjAry_swap( ZnkObjAry obj_ary1, ZnkObjAry obj_ary2 )
{
	Znk_SWAP( struct ZnkObjAryImpl, *obj_ary1, *obj_ary2 );
}

ZnkObj
ZnkObjAry_intern( ZnkObjAry obj_ary, size_t* ans_idx,
		ZnkObjQueryFunc query_func, void* query_arg,
		ZnkObjAllocFunc alloc_func, void* alloc_arg,
		bool* is_newly )
{
	const size_t size = ZnkObjAry_size( obj_ary );
	ZnkObj ptr = NULL;
	size_t idx;
	size_t null_idx = Znk_NPOS; /* ç≈èâÇ…NULLÇ™åªÇÍÇÈà íuäiî[óp */
	if( query_func ){
		for( idx=0; idx<size; ++idx ){
			ptr = ZnkObjAry_at( obj_ary, idx );
			if( ptr ){
				if( query_func( ptr, query_arg ) ){
					/***
					 * query_func Çñûë´Ç∑ÇÈ ptr Ç™ä˘Ç…ë∂ç›Ç∑ÇÈ.
					 */
					if( is_newly ){ *is_newly = false; }
					goto FUNC_END;
				}
			} else if( null_idx == Znk_NPOS ){
				null_idx = idx;
			}
		}
	} else {
		/* Ç±ÇÃÉPÅ[ÉXÇ≈ÇÕ NULL Ç™äiî[Ç≥ÇÍÇƒÇ¢ÇÈà íuÇÃÇ›ÇåüçıÇ∑ÇÈ */
		for( idx=0; idx<size; ++idx ){
			ptr = ZnkObjAry_at( obj_ary, idx );
			if( ptr == NULL ){
				null_idx = idx;
				break;
			}
		}
	}

	if( null_idx != Znk_NPOS ){
		/* ìríÜÇ…ë∂ç›ÇµÇΩNULLÇ™Ç¢ÇÌÇ‰ÇÈÅuãÛÇ´ÅvÇ≈Ç†ÇËÅAÇªÇÃà íuÇidxÇ∆Ç∑ÇÈ. */
		idx = null_idx;
		if( alloc_func ){
			ptr = alloc_func( alloc_arg );
			ZnkObjAry_set( obj_ary, idx, ptr );
		}
	} else {
		/* êVãKìoò^.
		 * ç≈å„îˆÇ…ptrÇpush_bkÇ∑ÇÈ. Ç±ÇÃÇ∆Ç´ÇÃidxÇÕsizeÇÃílÇ∆Ç»Ç¡ÇƒÇ¢ÇÈÇÕÇ∏. */
		if( alloc_func ){
			ptr = alloc_func( alloc_arg );
		}
		ZnkObjAry_push_bk( obj_ary, ptr );
	}
	ptr = ZnkObjAry_at( obj_ary, idx );
	if( is_newly ){ *is_newly = true; }

FUNC_END:
	if( ans_idx ){ *ans_idx = idx; }
	return ptr;
}

