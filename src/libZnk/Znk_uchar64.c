#include <Znk_uchar64.h>
#include <Znk_s_base.h>

static const uint8_t*
incrementP_a( const uint8_t* p )
{
	return p+1;
}
static const uint8_t*
getUChar64_a( ZnkUChar64* uc, const uint8_t* p )
{
	uc->u64_ = *p++;
	return p;
}

static const uint8_t*
getUChar64_sjis( ZnkUChar64* uc, const uint8_t* p )
{
	if( *p & 0x80 ){
		uc->u64_ =  *p++;
		uc->u64_ <<= 8;
		uc->u64_ |= *p++;
	} else {
		uc->u64_ =  *p++;
	}
	return p;
}
static const uint8_t*
incrementP_sjis( const uint8_t* p )
{
	if( *p & 0x80 ){
		p += 2;
	} else {
		++p;
	}
	return p;
}

Znk_INLINE bool UTF8_isMidByte   ( uint8_t b ){ return (bool)((b & 0xc0) == 0x80); } /* 2バイト目以降は必ず 10xxxxxx */
Znk_INLINE bool UTF8_isFirstByte2( uint8_t b ){ return (bool)((b & 0xe0) == 0xc0); } /* 2byte文字の開始 110xxxxx 10xxxxxx (実質11bit) */
Znk_INLINE bool UTF8_isFirstByte3( uint8_t b ){ return (bool)((b & 0xf0) == 0xe0); } /* 3byte文字の開始 1110xxxx 10xxxxxx, ... (実質16bit) */
Znk_INLINE bool UTF8_isFirstByte4( uint8_t b ){ return (bool)((b & 0xf8) == 0xf0); } /* 4byte文字の開始 11110xxx 10xxxxxx, ... (実質21bit) */
Znk_INLINE int  UTF8_getFirstByteType( uint8_t b )
{
	if( b < 0x80 ){ return 1; } /* ASCII. */
	if( UTF8_isMidByte(    b ) ){ return 0; }
	if( UTF8_isFirstByte2( b ) ){ return 2; }
	if( UTF8_isFirstByte3( b ) ){ return 3; }
	if( UTF8_isFirstByte4( b ) ){ return 4; }
	return -1; /* Invalid UTF8 */
}

static const uint8_t*
getUChar64_utf8( ZnkUChar64* uc, const uint8_t* p )
{
	if( *p == '\0' ){
		uc->u64_ = 0;
	} else {
		size_t i;
		const int delta = UTF8_getFirstByteType( *p );

		switch( delta ){
		case 1:
			/* ASCII */
			uc->u64_ =  *p++;
			break;
		case 2:
			uc->u64_ =  *p++;

			if( *p == '\0' ){ break; }
			uc->u64_ <<= 8;
			uc->u64_ |= *p++;
			break;
		case 3:
			uc->u64_ =  *p++;

			/* 3の頻度は高いと思われるのでここだけは展開しておく */
			if( *p == '\0' ){ break; }
			uc->u64_ <<= 8;
			uc->u64_ |= *p++;
			if( *p == '\0' ){ break; }
			uc->u64_ <<= 8;
			uc->u64_ |= *p++;
			break;
		case 4: case 5: case 6:
			uc->u64_ =  *p++;

			/* これ以降は頻度は少ないと思われるのでfor文で */
			for( i=0; i<(size_t)delta-2; ++i ){
				if( *p == '\0' ){ break; }
				uc->u64_ <<= 8;
				uc->u64_ |= *p++;
			}
			break;
		default:
			/* maybe broken utf8 */
			uc->u64_ = '?';
			++p;
			break;
		}
	}
	return p;
}
static const uint8_t*
incrementP_utf8( const uint8_t* p )
{
	const int delta = UTF8_getFirstByteType( *p );
	if( delta > 0 ){
		p += delta;
	} else {
		/* broken utf8 : anyway increment +1 */
		++p;
	}
	return p;
}


static const uint8_t*
getUChar64_utf16( ZnkUChar64* uc, const uint8_t* p )
{
	if( *p ){
		uc->s_[0] =  *p++;
		uc->s_[1] =  *p++;
	}
	return p;
}
static const uint8_t*
incrementP_utf16( const uint8_t* p )
{
	if( *p ){
		p += 2;
	}
	return p;
}


struct ZnkUChar64EncImpl {
	const char* id_;
	ZnkUChar64FuncT_get  func_get_;
	ZnkUChar64FuncT_increment func_increment_;
};

static struct ZnkUChar64EncImpl st_encode_info_table[] = {
	{ "sjis",  getUChar64_sjis,  incrementP_sjis },
	{ "utf8",  getUChar64_utf8,  incrementP_utf8 },
	{ "utf16", getUChar64_utf16, incrementP_utf16 },
	{ "a",     getUChar64_a,     incrementP_a },
};

ZnkUChar64Enc
ZnkUChar64Enc_find( const char* id )
{
	ZnkUChar64Enc enc = NULL;
	size_t i;
	for( i=0; i<Znk_NARY(st_encode_info_table); ++i ){
		enc = st_encode_info_table + i;
		if( ZnkS_eq( enc->id_, id ) ){
			break;
		}
	}
	return enc;
}

const char*
ZnkUChar64Enc_id( const ZnkUChar64Enc enc )
{
	return enc->id_;
}
ZnkUChar64FuncT_get
ZnkUChar64Enc_func_get( const ZnkUChar64Enc enc )
{
	return enc->func_get_;
}
ZnkUChar64FuncT_increment
ZnkUChar64Enc_func_increment( const ZnkUChar64Enc enc )
{
	return enc->func_increment_;
}
