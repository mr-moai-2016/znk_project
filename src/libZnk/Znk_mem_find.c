#include "Znk_mem_find.h"
#include "Znk_stdc.h"

#define GET_PAD_N(  size, pdsz )  ( ((size) + ((pdsz)-1)) /(pdsz)*(pdsz) )
#define GET_PAD_E2_MINUS1( size, pdsz_minus1 )  ( ((size) + (pdsz_minus1)) & ~(pdsz_minus1) )
#define CUT_MOD_N(  size, mod )  ( (size)/(mod) * (mod) )
#define CUT_MOD_E2( size, mod )  ( (size) & ~((mod)-1) )

/***
 * 与えられた size_t numが2のべき乗であるかどうかをできるだけ高速に判定する.
 */
static bool
isExp2( size_t num )
{
	/***
	 * メモリの局所化(キャッシュヒット率)のことを考慮すると、
	 * あまり大きなサイズのテーブルを作ればよいというものでもない気がする.
	 * (とはいえ、もう少し大きなサイズにしてもよい気もする)
	 */
	static const uint8_t table[128+1] = {
		1, 1, 1, 0, 1, 0, 0, 0,  1, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, /* 32... */
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, /* 64... */
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
		1, /* 128 */
	};
	register size_t n = num;
	if( n < sizeof( table ) ){ return (bool)( table[ n ] != 0 ); }
	/***
	 * 256の倍数かどうかを調べるため、下8bitがすべて0かどうかを確かめる.
	 * そのために、マスク値 ffを利用する.
	 */
	while( (n & 0xff) == 0 ){
		n >>= 8; /* 2の8乗、すなわち256で割る */
		if( n < sizeof( table ) ){ return (bool)( table[ n ] != 0 ); }
	}
	/* 256の倍数ではないbitパターンがあった */
	return false;
}

/*****************************************************************************/
/* Ary lfind/rfind */

/***
 * 以下の実装では、敢えてループの外でもう一度判定を行っている.
 * whileループ中にreturnを入れるとコンパイル結果が若干複雑になるため、
 * このようにする方が効率がよくなるらしい.
 */
size_t
ZnkAry_lfind_16( const uint16_t* buf, uint16_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint16_t* r1  = buf;
		register const uint16_t* end = r1 + num;
		while( r1 != end && *r1 != val ) ++r1;
		if( r1 != end ){ return (size_t)(r1 - buf); }
		break;
	}
	default:
	{
		register size_t i = 0;
		register const size_t end = GET_PAD_N( num, delta );
		register const uint16_t* r = buf;
		register const size_t dlt = delta;
		while( i != end && r[i] != val ) i += dlt;
		if( i != end ){ return i; }
		break;
	}
	}
	return Znk_NPOS;
}
size_t
ZnkAry_rfind_16( const uint16_t* buf, uint16_t val, size_t num, size_t delta )
{
	if( num == 0 ){ return Znk_NPOS; }
	switch( delta ){
	case 0: case 1:
	{
		register const uint16_t* r1   = buf + num-1;
		register const uint16_t* last = buf;
		while( r1 != last && *r1 != val ) --r1;
		if( r1 == last ){
			if( *r1 == val ) return 0;
		} else {
			return (size_t)(r1 - buf);
		}
		break;
	}
	default:
	{
		register size_t i = CUT_MOD_N( num-1, delta );
		register const uint16_t* r = buf;
		register const size_t dlt = delta;
		while( i && r[i] != val ) i -= dlt;
		if( i ){ return i; } else { if( r[0] == val ) return 0; }
		break;
	}
	}
	return Znk_NPOS;
}
size_t
ZnkAry_lfind_32( const uint32_t* buf, uint32_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint32_t* r1  = buf;
		register const uint32_t* end = r1 + num;
		while( r1 != end && *r1 != val ) ++r1;
		if( r1 != end ){ return (size_t)(r1 - buf); }
		break;
	}
	default:
	{
		register size_t i = 0;
		register const size_t end = GET_PAD_N( num, delta );
		register const uint32_t* r = buf;
		register const size_t dlt = delta;
		while( i != end && r[i] != val ) i += dlt;
		if( i != end ){ return i; }
		break;
	}
	}
	return Znk_NPOS;
}
size_t
ZnkAry_rfind_32( const uint32_t* buf, uint32_t val, size_t num, size_t delta )
{
	if( num == 0 ){ return Znk_NPOS; }
	switch( delta ){
	case 0: case 1:
	{
		register const uint32_t* r1   = buf + num-1;
		register const uint32_t* last = buf;
		while( r1 != last && *r1 != val ) --r1;
		if( r1 == last ){
			if( *r1 == val ) return 0;
		} else {
			return (size_t)(r1 - buf);
		}
		break;
	}
	default:
	{
		register size_t i = CUT_MOD_N( num-1, delta );
		register const uint32_t* r = buf;
		register const size_t dlt = delta;
		while( i && r[i] != val ) i -= dlt;
		if( i ){ return i; } else { if( r[0] == val ) return 0; }
		break;
	}
	}
	return Znk_NPOS;
}
size_t
ZnkAry_lfind_64( const uint64_t* buf, uint64_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint64_t* r1  = buf;
		register const uint64_t* end = r1 + num;
		while( r1 != end && *r1 != val ) ++r1;
		if( r1 != end ){ return (size_t)(r1 - buf); }
		break;
	}
	default:
	{
		register size_t i = 0;
		register const size_t end = GET_PAD_N( num, delta );
		register const uint64_t* r = buf;
		register const size_t dlt = delta;
		while( i != end && r[i] != val ) i += dlt;
		if( i != end ){ return i; }
		break;
	}
	}
	return Znk_NPOS;
}
size_t
ZnkAry_rfind_64( const uint64_t* buf, uint64_t val, size_t num, size_t delta )
{
	if( num == 0 ){ return Znk_NPOS; }
	switch( delta ){
	case 0: case 1:
	{
		register const uint64_t* r1   = buf + num-1;
		register const uint64_t* last = buf;
		while( r1 != last && *r1 != val ) --r1;
		if( r1 == last ){
			if( *r1 == val ) return 0;
		} else {
			return (size_t)(r1 - buf);
		}
		break;
	}
	default:
	{
		register size_t i = CUT_MOD_N( num-1, delta );
		register const uint64_t* r = buf;
		register const size_t dlt = delta;
		while( i && r[i] != val ) i -= dlt;
		if( i ){ return i; } else { if( r[0] == val ) return 0; }
		break;
	}
	}
	return Znk_NPOS;
}

size_t
ZnkAry_lfind_sz( const size_t* buf, size_t val, size_t num, size_t delta )
{
	switch( sizeof(size_t) ){
	case 4: return ZnkAry_lfind_32( (const uint32_t*)buf, (uint32_t)val, num, delta ); 
	case 8: return ZnkAry_lfind_64( (const uint64_t*)buf, (uint64_t)val, num, delta );
	case 2: return ZnkAry_lfind_16( (const uint16_t*)buf, (uint16_t)val, num, delta ); 
	}
	return Znk_NPOS;
}
size_t
ZnkAry_rfind_sz( const size_t* buf, size_t val, size_t num, size_t delta )
{
	switch( sizeof(size_t) ){
	case 4: return ZnkAry_rfind_32( (const uint32_t*)buf, (uint32_t)val, num, delta ); 
	case 8: return ZnkAry_rfind_64( (const uint64_t*)buf, (uint64_t)val, num, delta );
	case 2: return ZnkAry_rfind_16( (const uint16_t*)buf, (uint16_t)val, num, delta ); 
	}
	return Znk_NPOS;
}

/* endof Ary lfind/rfind */
/*****************************************************************************/


/*****************************************************************************/
/* find */

#define M_LSFT64( v, delta ) ((uint64_t)(v)<<(delta))

#define M_MAKE_UI16_LE( r ) (uint16_t)( ((r)[1]<<8)  | ((r)[0]) )
#define M_MAKE_UI24_LE( r ) (uint32_t)( ((r)[2]<<16) | ((r)[1]<<8)  | ((r)[0]) )
#define M_MAKE_UI32_LE( r ) (uint32_t)( ((r)[3]<<24) | ((r)[2]<<16) | ((r)[1]<<8)  | ((r)[0]) )
#define M_MAKE_UI64_LE( r ) (uint64_t)( \
		M_LSFT64((r)[7],56) | M_LSFT64((r)[6],48) | M_LSFT64((r)[5],40) | M_LSFT64((r)[4],32) | \
		M_LSFT64((r)[3],24) | M_LSFT64((r)[2],16) | M_LSFT64((r)[1],8)  | (r)[0] )

#define M_MAKE_UI16_BE( r ) (uint16_t)( ((r)[0]<<8)  | ((r)[1]) )
#define M_MAKE_UI24_BE( r ) (uint32_t)( ((r)[0]<<16) | ((r)[1]<<8)  | ((r)[2]) )
#define M_MAKE_UI32_BE( r ) (uint32_t)( ((r)[0]<<24) | ((r)[1]<<16) | ((r)[2]<<8)  | ((r)[3]) )
#define M_MAKE_UI64_BE( r ) (uint64_t)( \
		M_LSFT64((r)[0],56) | M_LSFT64((r)[1],48) | M_LSFT64((r)[2],40) | M_LSFT64((r)[3],32) | \
		M_LSFT64((r)[4],24) | M_LSFT64((r)[5],16) | M_LSFT64((r)[6],8)  | (r)[7] )


#define M_LFIND_END( num, delta, byte_size ) GET_PAD_N( (num)*(byte_size), (delta)*(byte_size) )
#define M_RFIND_I( num, delta, byte_size ) CUT_MOD_N( ((num)-1)*(byte_size), (delta)*(byte_size) )

static size_t
safe_lfind_16( const uint8_t* buf, uint16_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1  = buf;
		register const uint8_t* end = r1 + num*2;
		while( r1 != end && M_MAKE_UI16_BE( r1 ) != val ){ r1 += 2; }
		if( r1 != end ){ return (size_t)(r1 - buf)/2; }
		break;
	}
	default:
	{
		register size_t i = 0;
		//register const size_t end = GET_PAD_N( num*2, delta*2 );
		register const size_t end = M_LFIND_END( num, delta, 2 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta << 1;
		while( i != end && M_MAKE_UI16_BE( r+i ) != val ){ i += dlt; }
		if( i != end ){ return i/2; }
		break;
	}
	}
	return Znk_NPOS;
}
static size_t
safe_rfind_16( const uint8_t* buf, uint16_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1   = buf + (num-1)*2;
		register const uint8_t* last = buf;
		while( r1 != last && M_MAKE_UI16_BE( r1 ) != val ){ r1 -= 2; }
		if( r1 == last ){
			if( M_MAKE_UI16_BE( r1 ) == val ){ return 0; }
		} else {
			return (size_t)(r1 - buf)/2;
		}
		break;
	}
	default:
	{
		register size_t i = M_RFIND_I( num, delta, 2 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta << 1;
		while( i && M_MAKE_UI16_BE( r+i ) != val ){ i -= dlt; }
		if( i == 0 ){
			if( M_MAKE_UI16_BE( r+i ) == val ){ return 0; }
		} else {
			return i/2;
		}
		break;
	}
	}
	return Znk_NPOS;
}


static size_t
safe_lfind_32( const uint8_t* buf, uint32_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1  = buf;
		register const uint8_t* end = r1 + num*4;
		while( r1 != end && M_MAKE_UI32_BE( r1 ) != val ){ r1 += 4; }
		if( r1 != end ){ return (size_t)(r1 - buf)/4; }
		break;
	}
	default:
	{
		register size_t i = 0;
		//register const size_t end = GET_PAD_N( num*4, delta*4 );
		register const size_t end = M_LFIND_END( num, delta, 4 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta << 2;
		while( i != end && M_MAKE_UI32_BE( r+i ) != val ){ i += dlt; }
		if( i != end ){ return i/4; }
		break;
	}
	}
	return Znk_NPOS;
}
static size_t
safe_rfind_32( const uint8_t* buf, uint32_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1   = buf + (num-1)*4;
		register const uint8_t* last = buf;
		while( r1 != last && M_MAKE_UI32_BE( r1 ) != val ){ r1 -= 4; }
		if( r1 == last ){
			if( M_MAKE_UI32_BE( r1 ) == val ){ return 0; }
		} else {
			return (size_t)(r1 - buf)/4;
		}
		break;
	}
	default:
	{
		register size_t i = M_RFIND_I( num, delta, 4 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta << 2;
		while( i && M_MAKE_UI32_BE( r+i ) != val ){ i -= dlt; }
		if( i == 0 ){
			if( M_MAKE_UI32_BE( r+i ) == val ){ return 0; }
		} else {
			return i/4;
		}
		break;
	}
	}
	return Znk_NPOS;
}


static size_t
safe_lfind_64( const uint8_t* buf, uint64_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1  = buf;
		register const uint8_t* end = r1 + num*8;
		while( r1 != end && M_MAKE_UI64_BE( r1 ) != val ){ r1 += 8; }
		if( r1 != end ){ return (size_t)(r1 - buf)/8; }
		break;
	}
	default:
	{
		register size_t i = 0;
		register const size_t end = M_LFIND_END( num, delta, 8 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta << 3;
		while( i != end && M_MAKE_UI64_BE( r+i ) != val ){ i += dlt; }
		if( i != end ){ return i/8; }
		break;
	}
	}
	return Znk_NPOS;
}
static size_t
safe_rfind_64( const uint8_t* buf, uint64_t val, size_t num, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1   = buf + (num-1)*8;
		register const uint8_t* last = buf;
		while( r1 != last && M_MAKE_UI64_BE( r1 ) != val ){ r1 -= 8; }
		if( r1 == last ){
			if( M_MAKE_UI64_BE( r1 ) == val ){ return 0; }
		} else {
			return (size_t)(r1 - buf)/8;
		}
		break;
	}
	default:
	{
		register size_t i = M_RFIND_I( num, delta, 8 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta << 3;
		while( i && M_MAKE_UI64_BE( r+i ) != val ){ i -= dlt; }
		if( i == 0 ){
			if( M_MAKE_UI64_BE( r ) == val ){ return 0; }
		} else {
			return i/8;
		}
		break;
	}
	}
	return Znk_NPOS;
}

static size_t
safe_lfind_24( const uint8_t* buf, uint32_t val, size_t num, size_t delta, bool is_swap )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1  = buf;
		register const uint8_t* end = r1 + num * 3;
		/* 画素でいえば下位24bitを 0RGB とみなす */
		val = val & 0x00ffffff;
		if( is_swap ){
			/* 画素でいえばBGRという順番でシリアライズされているケース */
			while( r1 != end && M_MAKE_UI24_LE( r1 ) != val ){ r1 += 3; }
		} else {
			/* 画素でいえばRGBという順番でシリアライズされているケース */
			while( r1 != end && M_MAKE_UI24_BE( r1 ) != val ){ r1 += 3; }
		}
		return r1 != end ? (size_t)(r1 - buf)/3 : Znk_NPOS;
	}
	default:
	{
		register size_t i = 0;
		register const size_t end = M_LFIND_END( num, delta, 3 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta*3;
		val = val & 0x00ffffff;
		if( is_swap ){
			while( i != end && M_MAKE_UI24_LE( r+i ) != val ){ i += dlt; }
		} else {
			while( i != end && M_MAKE_UI24_BE( r+i ) != val ){ i += dlt; }
		}
		if( i != end ){ return i/3; }
		break;
	}
	}
	return Znk_NPOS;
}
static size_t
safe_rfind_24( const uint8_t* buf, uint32_t val, size_t num, size_t delta, bool is_swap )
{
	switch( delta ){
	case 0: case 1:
	{
		register const uint8_t* r1   = buf + (num-1)*3;
		register const uint8_t* last = buf;
		val = val & 0x00ffffff;
		if( is_swap ){
			while( r1 != last && M_MAKE_UI24_LE( r1 ) != val ){ r1 -= 3; }
			if( r1 == last ){
				if( M_MAKE_UI24_LE( r1 ) == val ) return 0;
			} else {
				return (size_t)(r1 - buf)/3;
			}
		} else {
			while( r1 != last && M_MAKE_UI24_BE( r1 ) != val ){ r1 -= 3; }
			if( r1 == last ){
				if( M_MAKE_UI24_BE( r1 ) == val ) return 0;
			} else {
				return (size_t)(r1 - buf)/3;
			}
		}
	}
	default:
	{
		register size_t i = M_RFIND_I( num, delta, 3 );
		register const uint8_t* const r = buf;
		register const size_t dlt = delta*3;
		val = val & 0x00ffffff;
		if( is_swap ){
			while( i && M_MAKE_UI24_LE( r+i ) != val ){ i -= dlt; }
			if( i ){
				return i/3;
			} else {
				if( M_MAKE_UI24_LE( r ) == val ) return 0;
			}
		} else {
			while( i && M_MAKE_UI24_BE( r+i ) != val ){ i -= dlt; }
			if( i ){
				return i/3;
			} else {
				if( M_MAKE_UI24_BE( r ) == val ) return 0;
			}
		}
		break;
	}
	}
	return Znk_NPOS;
}

size_t
ZnkMem_lfind_8( const uint8_t* buf, size_t size, uint8_t val, size_t delta )
{
	register const uint8_t* r1 = buf;
	register const uint8_t* end;
	if( size == 0 ){ return Znk_NPOS; }
	switch( delta ){
	case 0: case 1:
		/* memchr と等価である */
		end = r1 + size;
		while( r1 != end && *r1 != val ) ++r1;
		break;
	default:
	{
		/***
		 * isExp2( delta ) ? CUT_MOD_E2( size, delta ) : CUT_MOD_N( size, delta );
		 * などとやる必要はない(CUT_MOD_NよりisExp2の方がかえって時間がかかる)
		 */
		register const size_t dlt = delta;
		end = r1 + GET_PAD_N( size, dlt );
		while( r1 != end && *r1 != val ) r1 += dlt;
		break;
	}
	}
	return r1 != end ? (size_t)(r1 - buf) : Znk_NPOS;
}
size_t
ZnkMem_rfind_8( const uint8_t* buf, size_t size, uint8_t val, size_t delta )
{
	register const uint8_t* r1;
	register const uint8_t* last = buf;
	if( size == 0 ){ return Znk_NPOS; }
	switch( delta ){
	case 0: case 1:
		/* memrchr と等価である(memrchrは標準関数にはない) */
		r1 = buf + size - 1;
		while( r1 != last && *r1 != val ) --r1;
		break;
	default:
	{
		/***
		 * isExp2( delta ) ? CUT_MOD_E2( size, delta ) : CUT_MOD_N( size, delta );
		 * などとやる必要はない(CUT_MOD_NよりisExp2の方がかえって時間がかかる)
		 */
		register const size_t dlt = delta;
		r1 = buf + CUT_MOD_N( size, dlt );
		while( r1 != last && *r1 != val ) r1 -= dlt;
		break;
	}
	}
	if( r1 == last ){ return ( *r1 == val ) ? 0 : Znk_NPOS; }
	return (size_t)(r1 - buf);
}
size_t
ZnkMem_lfind_16( const uint8_t* buf, size_t size, uint16_t val, size_t delta, bool is_LE )
{
	const size_t num = size/2;
	/* 開始位置がbyte境界にあるか否かで分ける */
	if( (uintptr_t)(buf) % 2 == 0 ){
		if( Znk_isLE() != is_LE ){ Znk_swapU16( &val ); }
		return ZnkAry_lfind_16( (uint16_t*)(buf), val, num, delta );
	}
	if( is_LE ){ Znk_swapU16( &val ); }
	return safe_lfind_16( buf, val, num, delta );
}
size_t
ZnkMem_rfind_16( const uint8_t* buf, size_t size, uint16_t val, size_t delta, bool is_LE )
{
	const size_t num = size/2;
	/* 開始位置がbyte境界にあるか否かで分ける */
	if( (uintptr_t)(buf) % 2 == 0 ){
		if( Znk_isLE() != is_LE ){ Znk_swapU16( &val ); }
		return ZnkAry_rfind_16( (uint16_t*)(buf), val, num, delta );
	}
	if( is_LE ){ Znk_swapU16( &val ); }
	return safe_rfind_16( buf, val, num, delta );
}
size_t
ZnkMem_lfind_32( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_LE )
{
	const size_t num = size/4;
	/* 開始位置がbyte境界にあるか否かで分ける */
	if( (uintptr_t)(buf) % 4 == 0 ){
		if( Znk_isLE() != is_LE ){ Znk_swapU32( &val ); }
		return ZnkAry_lfind_32( (uint32_t*)(buf), val, num, delta );
	}
	if( is_LE ){ Znk_swapU32( &val ); }
	return safe_lfind_32( buf, val, num, delta );
}
size_t
ZnkMem_rfind_32( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_LE )
{
	const size_t num = size/4;
	/* 開始位置がbyte境界にあるか否かで分ける */
	if( (uintptr_t)(buf) % 4 == 0 ){
		if( Znk_isLE() != is_LE ){ Znk_swapU32( &val ); }
		return ZnkAry_rfind_32( (uint32_t*)(buf), val, num, delta );
	}
	if( is_LE ){ Znk_swapU32( &val ); }
	return safe_rfind_32( buf, val, num, delta );
}
size_t
ZnkMem_lfind_64( const uint8_t* buf, size_t size, uint64_t val, size_t delta, bool is_LE )
{
	const size_t num = size/8;
	/* 開始位置がbyte境界にあるか否かで分ける */
	if( (uintptr_t)(buf) % 8 == 0 ){
		if( Znk_isLE() != is_LE ){ Znk_swapU64( &val ); }
		return ZnkAry_lfind_64( (uint64_t*)(buf), val, num, delta );
	}
	if( is_LE ){ Znk_swapU64( &val ); }
	return safe_lfind_64( buf, val, num, delta );
}
size_t
ZnkMem_rfind_64( const uint8_t* buf, size_t size, uint64_t val, size_t delta, bool is_LE )
{
	const size_t num = size/8;
	/* 開始位置がbyte境界にあるか否かで分ける */
	if( (uintptr_t)(buf) % 8 == 0 ){
		if( Znk_isLE() != is_LE ){ Znk_swapU64( &val ); }
		return ZnkAry_rfind_64( (uint64_t*)(buf), val, num, delta );
	}
	if( is_LE ){ Znk_swapU64( &val ); }
	return safe_rfind_64( buf, val, num, delta );
}

size_t
ZnkMem_lfind_24( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_swap )
{
	return safe_lfind_24( buf, val, size / 3, delta, is_swap );
}
size_t
ZnkMem_rfind_24( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_swap )
{
	return safe_rfind_24( buf, val, size / 3, delta, is_swap );
}


Znk_INLINE size_t
lfind_data_BF( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size, size_t delta )
{
	switch( delta ){
	case 0: case 1:
	{
		const uint8_t* buf_begin = buf;
		register const uint8_t* r1  = buf;
		register const uint8_t* end = buf + buf_size - data_size + 1;
		do {
			if( Znk_memcmp( r1, data, data_size ) == 0 ){ return (size_t)(r1 - buf_begin); }
			++r1;
		} while( r1 != end );
		break;
	}
	default:
	{
		register size_t i = 0;
		register const size_t end = CUT_MOD_N( buf_size - data_size, delta ) + delta;
		register const size_t dlt = delta;
		register const uint8_t* const r = buf;
		do {
			if( Znk_memcmp( r+i, data, data_size ) == 0 ){ return i; }
			i += dlt;
		} while( i != end );
		break;
	}
	}
	return Znk_NPOS;
}
Znk_INLINE size_t
rfind_data_BF( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size, size_t delta )
{
	const uint8_t* buf_begin = buf;
	register const uint8_t* r1;
	register const uint8_t* last;
	switch( delta ){
	case 0: case 1:
		r1   = buf + buf_size - data_size;
		last = buf;
		do {
			if( Znk_memcmp( r1, data, data_size ) == 0 ){ return (size_t)(r1 - buf_begin); }
		} while( r1-- != last );
	default:
	{
		register const size_t dlt = delta;
		r1   = buf + CUT_MOD_N( buf_size - data_size, delta );
		last = buf;
		while( r1 != last ){
			if( Znk_memcmp( r1, data, data_size ) == 0 ){ return (size_t)(r1 - buf_begin); }
			r1 -= dlt;
		}
		if( r1 == last ){ if( Znk_memcmp( r1, data, data_size ) == 0 ) return 0; }
		break;
	}
	}
	return Znk_NPOS;
}

/*****************************************************************************/
/* fill */

#define IS_EQ_ALLBYTE_16( val ) \
	( (((val) >> 8)  & 0xff) == ((val) & 0xff) )

#define IS_EQ_ALLBYTE_32( val ) \
	( (((val) >> 8)  & 0xff)   == ((val) & 0xff) && \
	  (((val) >> 16) & 0xffff) == ((val) & 0xffff) )

#define IS_EQ_ALLBYTE_64( val ) \
	( (((val) >> 8)  & 0xff)   == ((val) & 0xff) && \
	  (((val) >> 16) & 0xffff) == ((val) & 0xffff) && \
	  (((uint64_t)(val) >> 32) & 0xffffffff) == ((val) & 0xffffffff) )

#define M_ARY_FILL( type, bit ) \
	if( val == 0 || IS_EQ_ALLBYTE_##bit( val ) ){ \
		Znk_memset( buf, (uint8_t)(val&0xff), num*sizeof(type) ); \
	} else { \
		register type*       p   = buf; \
		register const type* end = p + num; \
		while( p != end ){ *p++ = val; } \
	}

static void
Ary_fill_sz( size_t*   buf, size_t  num, size_t   val ){
	switch( sizeof(size_t) ){
	case 4: M_ARY_FILL( size_t, 32 ) break;
	case 8: M_ARY_FILL( size_t, 64 ) break;
	case 2: M_ARY_FILL( size_t, 16 ) break;
	case 1: Znk_memset( buf, (uint8_t)val, num ); break;
	default: break;
	}
}

/* endof fill */
/*****************************************************************************/

/***
 * これを関数で分ける理由は、同じdataで複数の異なるbufを検索する際、
 * 無駄を省けるからである.
 *
 * このテーブルデータは bad-character shift (または occurrence shift) と呼ばれる.
 * ここではこれの呼び方をOccTableに統一する.
 */
void
ZnkMem_getLOccTable_forBMS( size_t* occ_tbl, const uint8_t* data, size_t data_size ){
	size_t i;
	/* まずocc_tbl全体をdata_size+1という値で初期化する */
	Ary_fill_sz( occ_tbl, 256, data_size+1 );
	for( i=0; i<data_size; ++i) occ_tbl[ data[i] ] = data_size - i;
}
void
ZnkMem_getROccTable_forBMS( size_t* occ_tbl, const uint8_t* data, size_t data_size ){
	size_t i;
	/* まずocc_tbl全体をdata_size+1という値で初期化する */
	Ary_fill_sz( occ_tbl, 256, data_size+1 );
	for( i=0; i<data_size; ++i) occ_tbl[ data[data_size-1-i] ] = data_size - i;
}
/***
 * @brief
 *   BMS( Boyer-Moore Sunday ) 法による検索.
 *
 * @note
 *   正式名称はおそらく Boyer-Moore Sunday-Quick Search Algorithm.
 *   これは Boyer-Moore Horspool法の亜種であり、現時点で最速の探索法であると思われる.
 *   尚、Horspool, Sunday 法にはオリジナルのBoyer-Mooreに見られたシフトの
 *   逆転現象は発生しない.
 *
 *   参考:
 *   http://www-igm.univ-mlv.fr/~lecroq/string/node19.html#SECTION00190
 */
Znk_INLINE size_t
lfind_data_BMS( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size,
		size_t delta, const size_t* occ_tbl )
{
	register size_t j = 0;
	register const size_t j_end = buf_size - data_size + 1;
	switch( delta ){
	case 0: case 1:
		do {
			if( Znk_memcmp(data, buf + j, data_size) == 0 ){ return j; }
			j += occ_tbl[ buf[j + data_size] ]; /* buf[j+data_size]が次の文字. SundayQuickではこれを使う. */
		} while( j < j_end ); /* ここは不等式の比較にせざるを得ない */
		break;
	default:
		/***
		 * その他のdelta.
		 * case 値があまりに大きくなると switch-case 文のtable展開が行われない可能性が高くなる
		 * (その場合おそらく内部的にif-else 文になる).
		 * またコードサイズをあまり大きくしたくないので一旦ここで区切る.
		 */
		if( isExp2( delta ) ){
			/* 2のべき乗 */
			--delta;
			do {
				if( Znk_memcmp(data, buf + j, data_size) == 0 ){ return j; }
				j += occ_tbl[ buf[j + data_size] ];
				j = GET_PAD_E2_MINUS1( j, delta );
			} while( j < j_end );
		} else {
			/* 任意の自然数 */
			do {
				if( Znk_memcmp(data, buf + j, data_size) == 0 ){ return j; }
				j += occ_tbl[ buf[j + data_size] ];
				j = GET_PAD_N( j, delta );
			} while( j < j_end );
		}
		break;
	}
	return Znk_NPOS;
}
Znk_INLINE size_t
rfind_data_BMS( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size,
		size_t delta, const size_t* occ_tbl )
{
	register size_t j = 0;
	switch( delta ){
	case 0: case 1:
	{
		register const size_t j_end = buf_size - data_size;
		do {
			if( Znk_memcmp( buf + j_end-j, data, data_size) == 0 ){ return j_end-j; }
			j += occ_tbl[ buf[j_end-j - 1] ]; /* buf[j_end-j-1]が次の文字. SundayQuickではこれを使う. */
		} while( j < j_end ); /* ここは不等式の比較にせざるを得ない */
		if( j == j_end ){ if( Znk_memcmp( buf, data, data_size) == 0 ) return 0; }
		break;
	}
	default:
		if( isExp2( delta ) ){
			/* 2のべき乗 */
			register const size_t j_end = CUT_MOD_E2( buf_size - data_size, delta );
			--delta;
			do {
				if( Znk_memcmp( buf + j_end-j, data, data_size) == 0 ){ return j_end-j; }
				j += occ_tbl[ buf[j_end-j - 1] ];
				j = GET_PAD_E2_MINUS1( j, delta );
			} while( j < j_end );
			if( j == j_end ){ if( Znk_memcmp( buf, data, data_size) == 0 ) return 0; }
		} else {
			/* 任意の自然数 */
			register const size_t j_end = CUT_MOD_N( buf_size - data_size, delta );
			do {
				if( Znk_memcmp( buf + j_end-j, data, data_size) == 0 ){ return j_end-j; }
				j += occ_tbl[ buf[j_end-j - 1] ];
				j = GET_PAD_N( j, delta );
			} while( j < j_end );
			if( j == j_end ){ if( Znk_memcmp( buf, data, data_size) == 0 ) return 0; }
		}
		break;
	}
	return Znk_NPOS;
}

size_t ZnkMem_lfind_data_BF( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size,
		size_t delta )
{
	if( data_size > buf_size ){ return Znk_NPOS; }
	switch( data_size ){
	case 0: return 0; /* strstrと同様の仕様 */
	case 1: return ZnkMem_lfind_8( buf, buf_size, *data, delta );
	default: break;
	}
	return lfind_data_BF( buf, buf_size, data, data_size, delta );
}
size_t ZnkMem_rfind_data_BF( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size,
		size_t delta )
{
	if( data_size > buf_size ){ return Znk_NPOS; }
	switch( data_size ){
	case 0: return 0; /* strstrと同様の仕様 */
	case 1: return ZnkMem_rfind_8( buf, buf_size, *data, delta );
	default: break;
	}
	return rfind_data_BF( buf, buf_size, data, data_size, delta );
}
size_t ZnkMem_lfind_data_BMS( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size,
		size_t delta, const size_t* occ_tbl )
{
	if( data_size > buf_size ){ return Znk_NPOS; }
	switch( data_size ){
	case 0: return 0; /* strstrと同様の仕様 */
	case 1: return ZnkMem_lfind_8( buf, buf_size, *data, delta );
	default: break;
	}
	if( occ_tbl == NULL ){
		size_t occ_tbl_tmp[256];
		ZnkMem_getLOccTable_forBMS( occ_tbl_tmp, data, data_size ); 
		return lfind_data_BMS( buf, buf_size, data, data_size, delta, occ_tbl_tmp );
	}
	return lfind_data_BMS( buf, buf_size, data, data_size, delta, occ_tbl );
}
size_t ZnkMem_rfind_data_BMS( const uint8_t* buf, size_t buf_size, const uint8_t* data, size_t data_size,
		size_t delta, const size_t* occ_tbl )
{
	if( data_size > buf_size ){ return Znk_NPOS; }
	switch( data_size ){
	case 0: return 0; /* strstrと同様の仕様 */
	case 1: return ZnkMem_rfind_8( buf, buf_size, *data, delta );
	default: break;
	}
	if( occ_tbl == NULL ){
		size_t occ_tbl_tmp[256];
		ZnkMem_getROccTable_forBMS( occ_tbl_tmp, data, data_size ); 
		return rfind_data_BMS( buf, buf_size, data, data_size, delta, occ_tbl_tmp );
	}
	return rfind_data_BMS( buf, buf_size, data, data_size, delta, occ_tbl );
}

/* endof find */
/*****************************************************************************/


