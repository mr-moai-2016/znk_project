#include <Rano_hash.h>

uint32_t
RanoHash_murmurhash3_32_gc( const char* key, size_t key_length, uint32_t seed )
{
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	const size_t remainder = key_length & 3; /* key_length % 4 */
	const size_t bytes = key_length - remainder;

	uint32_t h1 = seed;
	uint32_t i = 0;
	uint32_t h1b;
	uint32_t k1;
	
	while( i < bytes ){
		/***
		 * ISO/IEC 9899/1990 6.3 によれば、Cにおいて副作用完了点(Sequence point)とは
		 * すべての副作用が完了したことが保証されている点である. これは式の本当の終わり、
		 * 実引数の評価完了後の関数の呼出し時、&&、||、三項演算子?:、カンマ演算子において
		 * 存在する. これら以外での演算子で区切られた範囲では、その範囲内で変数xを
		 * 複数回更新した場合の、その範囲内で変数xを複数回更新した場合のxの評価順序は
		 * 未定義である. またたとえそのような範囲においてxの更新が一回のみであっても、
		 * x更新前の値が、x更新後の値を決定するためだけにアクセスされている場合(x = x+1など)を除き、
		 * xの変更と参照が同時に行われている場合(a[x]=x++など)の動作は未定義となる.
		 * 
		 * 従ってオリジナルのJavascriptにあるような ++i を一つの式内で複数使う記述はCではNGであり
		 * ( | 演算子で区分されてはいるが、これは上記の条件を満たす演算子ではないことに注意 )、
		 * 例えば以下のように後でまとめてインクリメントする必要がある.
		 */
		k1 =
			( (key[ i   ] & 0xff)      ) |
			( (key[ i+1 ] & 0xff) << 8 ) |
			( (key[ i+2 ] & 0xff) << 16) |
			( (key[ i+3 ] & 0xff) << 24);
		i += 4;
		k1  = ((((k1 & 0xffff) * c1) + ((((k1 >> 16) * c1) & 0xffff) << 16))) & 0xffffffff;
		k1  = (k1 << 15) | (k1 >> 17);
		k1  = ((((k1 & 0xffff) * c2) + ((((k1 >> 16) * c2) & 0xffff) << 16))) & 0xffffffff;
		h1 ^= k1;
		h1  = (h1 << 13) | (h1 >> 19);
		h1b = ((((h1 & 0xffff) * 5) + ((((h1 >> 16) * 5) & 0xffff) << 16))) & 0xffffffff;
		h1  = (((h1b & 0xffff) + 0x6b64) + ((((h1b >> 16) + 0xe654) & 0xffff) << 16));
	}

	k1 = 0;
	
	switch( remainder ){
	case 3: k1 ^= (key[ i + 2 ] & 0xff) << 16; /* fall-through */
	case 2: k1 ^= (key[ i + 1 ] & 0xff) << 8;  /* fall-through */
	case 1: k1 ^= (key[ i     ] & 0xff);
		k1  = (((k1 & 0xffff) * c1) + ((((k1 >> 16) * c1) & 0xffff) << 16)) & 0xffffffff;
		k1  = (k1 << 15) | (k1 >> 17);
		k1  = (((k1 & 0xffff) * c2) + ((((k1 >> 16) * c2) & 0xffff) << 16)) & 0xffffffff;
		h1 ^= k1;
		break;
	default:
		break;
	}
	
	h1 ^= key_length;
	
	h1 ^= h1 >> 16;
	h1  = (((h1 & 0xffff) * 0x85ebca6b) + ((((h1 >> 16) * 0x85ebca6b) & 0xffff) << 16)) & 0xffffffff;
	h1 ^= h1 >> 13;
	h1  = ((((h1 & 0xffff) * 0xc2b2ae35) + ((((h1 >> 16) * 0xc2b2ae35) & 0xffff) << 16))) & 0xffffffff;
	h1 ^= h1 >> 16;
	
	return h1 >> 0;
}

