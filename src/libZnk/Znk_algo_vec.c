#include "Znk_algo_vec.h"

void
ZnkAlgoVec_quicksort( void* list, size_t pos, const size_t num,
		ZnkAlgoVecFuncT_compare func_compare,
		ZnkAlgoVecFuncT_swap    func_swap )
{
	if( list && num ){
		size_t pivot = 0;
		size_t begin = 0;
		size_t end   = num-1;
	
		while( pivot+1 != num && !func_compare( list, pos+pivot, list, pos+(pivot+1) ) ){
			++pivot;
		}
		if( pivot+1 == num ){
			return;
		}
		if( func_compare( list, pos+pivot, list, pos+(pivot+1) ) < 0 ){
			++pivot;
		}
		while( begin < end ){
			while( 0 < func_compare( list, pos+pivot, list, pos+begin ) && begin != num-1 ){
				++begin;
			}
			while(0 >= func_compare( list, pos+pivot, list, pos+end ) && end ){
				--end;
			}
			if( begin < end ){
				if(pivot == begin || pivot == end){
					pivot = pivot^end^begin;
				}
				func_swap( list, pos + begin, list, pos + end );
			}
		}
		ZnkAlgoVec_quicksort( list, pos,       begin,     func_compare, func_swap );
		ZnkAlgoVec_quicksort( list, pos+begin, num-begin, func_compare, func_swap );
	}
}

