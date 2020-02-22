/***
 * ���̎����ł́AZnkVSNPrintfCtx��p���邱�Ƃɂ��A�W�J������̃o�b�t�@�ւ̒ǉ���ėp�����Ă���.
 * ����͑����A�C�ӂ̃f�o�C�X��t�@�C���A�\�P�b�g�Ȃǂւ̓]����vsnprintf�ň�U������Ƃ���
 * �擾�����_�C���N�g�ɍs�����Ƃ��ł���Ƃ������Ƃł���A����ɂ�荂�������]�߂�P�[�X������.
 *
 * �������Ȃ���A���̏ꍇ�A���̂悤�ȃf�o�C�X��t�@�C���A�\�P�b�g�ւ̓]���͒ᑬ�ł���A
 * �]���āA�W�J������̕����v�f(���������%����)����������x�ɂ��̂悤�ȓ]���֐����Ăяo�����Ǝ���
 * �������Ēᑬ���������Ă��܂��\��������. ���̂悤�ȏꍇ�A��������̃o�b�t�@�Ɋ��������o�b�t�@
 * ����U����Ă����Ĉ�C�Ƀu���b�N�]�����������ނ��덂���ł���(�������ւ̓]�����ł������ł��邩��).
 *
 * Windows�̃R�}���h�v�����v�g��Linux�Ȃǂ�xterm�ւ̕W���o�͂́A���z�f�o�C�X�ł�����ۂɂ�
 * �������ւ̓]���ł��낤����A���̂悤�ȃ_�C���N�g�ȓ]���ɂ�荂�����͊��҂ł���\��������.
 * ���̏ꍇ�Astdout���e�L�X�g���[�h�ł���ꍇ�͊e�W�J�����v�f���Ƃ�NULL�I�[������fputs�֐���
 * �Ăяo���Ȃǂ̑[�u�����Ȃ���΂Ȃ�Ȃ�. �Ƃ��낪�A���ۂɎ������Ă݂�Ƃ킩�邪�A����͈ӊO��
 * ���܂������Ȃ�. �e�v�f���Ƃ�NULL�I�[������ƂȂ�ƁAfputs�œn���ꍇ�ɂ����āA�ǂ����Ă���
 * �o�b�t�@��p�ӂ��A�����ֈ�U�R�s�[���Ȃ���΂Ȃ�Ȃ�����ł���. ����ł͌��ǖ{���]�|�ł���.
 *
 * �܂�Apache�̂悤��Web�T�[�o�ŕW���o�͂�����Ń\�P�b�g�]���Ƀ��_�C���N�g���Ă���P�[�X������A
 * ���̂悤�ȏꍇ�Astdout�ł��������Ēᑬ���������\��������.
 *
 * �Ƃ����킯�ŁAZnk_vsnprintf_general�Ƃ����֐��͈ꉞ�p�ӂ���Ă���킯����
 * �����ZnkVSNPrintfCtx��������Znk_vsnprintf_C99�̎g�����ȊO�ŗL�p�ƂȂ鎖���
 * �����_�ł͂قڎv�����Ȃ�.
 */
#include "Znk_vsnprintf.h"

#include <Znk_stdc.h>
#include <Znk_def_util.h>

#include <Znk_tostr_int.h>
#include <Znk_tostr_double.h>
//#include <Znk_f_std.h>

#include <ctype.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>


#if defined(__BORLANDC__)
/**
 * �x�� W8066 hoge.cpp XX: ���s����Ȃ��R�[�h(�֐�)
 */
#  pragma warn -8066
#endif


#ifdef __cplusplus
#  define M_STATIC_CAST(type,val)       static_cast<type>(val)
#  define M_REINTERPRET_CAST(type,val)  reinterpret_cast<type>(val)
#else
#  define M_STATIC_CAST(type,val)      ((type)(val))
#  define M_REINTERPRET_CAST(type,val) ((type)(val))
#endif

#define M_P_VA_ARG_AS_TYPE(as_type, ap, type)  M_STATIC_CAST( as_type, va_arg(ap, type) )

typedef long long          LongLong;
typedef unsigned long long ULongLong;


typedef struct {
	va_list ap_;
} VAWrap;

/***
 * va_list�̃R�s�[�͕K�����̃}�N����ʂ��čs��Ȃ���΂Ȃ�Ȃ�.
 * (va_list�̎��̂�=�ŒP���ɑ���ł���I�u�W�F�N�g�Ƃ͌���Ȃ�.
 * 64bit AMD��gcc�ł͎��̂����W�X�^�ł���A�f���t�@�����X���Z�q������Ғʂ�̋����ƂȂ�Ȃ��ꍇ���炠��)
 */
#if defined(__GNUC__)
#  define M_P_VA_COPY( dst, src ) va_copy( dst, src )
#else
#  define M_P_VA_COPY( dst, src ) (dst)=(src)
#endif


/*****************************************************************************/
/**
 * POD Utility
 */

Znk_INLINE int diffSize( size_t op1, size_t op2 )
{
	return M_STATIC_CAST(int, op1) - M_STATIC_CAST(int, op2);
}

#define M_PTR_DIFF_AS_TYPE(type, ptr1, ptr2)  (M_STATIC_CAST(type, ptr1) - M_STATIC_CAST(type, ptr2))


Znk_INLINE size_t
Buf_memset( void* buf, size_t buf_size, size_t pos, uint8_t ch, size_t req_size )
{
	if( buf_size > pos && req_size ){
		const size_t avail = buf_size - pos;
		const size_t copy_size = Znk_MIN( req_size, avail );
		memset( M_STATIC_CAST(uint8_t*, buf) + pos, ch, copy_size );
		return copy_size;
	}
	return 0;
}

/**
 * buf�̈ʒupos�� bytes[0,req_size)���R�s�[����.
 * ������ pos+req_size �� buf_size ���I�[�o�[����ꍇ�́A
 * �o�b�t�@�I�[�o�[���N���Ȃ��悤��req_size���������ꂽ�T�C�Y���݂̂��R�s�[�����.
 */
Znk_INLINE size_t
Buf_memcpy( void* buf, size_t buf_size, size_t pos, const void* bytes, size_t req_size )
{
	if( buf_size > pos && req_size ){
		const size_t avail = buf_size - pos;
		const size_t copy_size = Znk_MIN( req_size, avail );
		memcpy( M_STATIC_CAST(uint8_t*, buf) + pos, bytes, copy_size );
		return copy_size;
	}
	return 0;
}
Znk_INLINE size_t
Buf_memmove( void* buf, size_t buf_size, size_t pos, const void* bytes, size_t req_size )
{
	if( buf_size > pos && req_size ){
		const size_t avail = buf_size - pos;
		const size_t copy_size = Znk_MIN( req_size, avail );
		memmove( M_STATIC_CAST(uint8_t*, buf) + pos, bytes, copy_size );
		return copy_size;
	}
	return 0;
}

/**
 * buf[0, buf_size) �܂łōŏ��� ch �������ʒu�����߂�.
 * �ʒu�̓A�h���X�ł͂Ȃ��Abuf[0] �� 0 �Ƃ����z��̃C���f�b�N�X�Ƃ��Ď擾�����.
 * ch ��������Ȃ������ꍇ�� buf_size ���߂�l�ƂȂ�.
 */
Znk_INLINE size_t
Buf_findByte( const void* buf, size_t buf_size, uint8_t ch )
{
	const void* end = memchr( buf, ch, buf_size );
	return ( end == NULL ) ?
		buf_size :
		M_PTR_DIFF_AS_TYPE( const uint8_t*, end, buf );
}


//M_PRIVATE_FUNC uint32_t castPtrToU32( void* ptr );
//M_PRIVATE_FUNC uint64_t castPtrToU64( void* ptr );


/**
 * BitExtend �Ŏ�����镔���� bit�g�����ɕK���C�����K�v�ȏꏊ������.
 * �Ⴆ�΁A����A�ő� 64 bit �f�[�^�܂ň����邪�A����� 128 bit �f�[�^��������p��
 * �g������ꍇ�ABitExtend���e�L�X�g�G�f�B�^�̌����L�[���[�h�ɓ����
 * ���ꂪ�����������C������΂悢.
 */


Znk_INLINE uintmax_t
cast_to_uintmax_t_from_ptrdiff_t( ptrdiff_t src )
{
#if defined(Znk_RECENT_VC) || defined(__GNUC__)
	return (uintmax_t)(size_t)( src );
#else
	switch( sizeof(ptrdiff_t) ){
	case 1: return (uintmax_t)( (uint8_t )( src ) );
	case 2: return (uintmax_t)( (uint16_t)( src ) );
	case 4: return (uintmax_t)( (uint32_t)( src ) );
	case 8: return (uintmax_t)( (uint64_t)( src ) );
	default: break;
	}
	return (uintmax_t)(size_t)( src );
#endif
}
Znk_INLINE intmax_t
cast_to_intmax_t_from_size_t( size_t src )
{
#if defined(Znk_RECENT_VC) || defined(__GNUC__)
	return (intmax_t)(ptrdiff_t)( src );
#else
	switch( sizeof(size_t) ){
	case 1: return (intmax_t)( (int8_t )( src ) );
	case 2: return (intmax_t)( (int16_t)( src ) );
	case 4: return (intmax_t)( (int32_t)( src ) );
	case 8: return (intmax_t)( (int64_t)( src ) );
	default: break;
	}
	return (intmax_t)(ptrdiff_t)( src );
#endif
}


/*
 * endof POD Utility
 ***/
/*****************************************************************************/


/*****************************************************************************/
/**
 * Data
 */

/**
 * ZeroPad
 */
typedef struct {
	/**
	 * index into body where zero padding is to be inserted
	 */
	size_t pos;
	/**
	 * number of zeros to be inserted for numeric conversions
	 * as required by the precision or minimal field width
	 */
	size_t size;
}ZeroPad;

Znk_INLINE void
ZeroPad_clear( ZeroPad* zeropad )
{
	zeropad->pos  = 0;
	zeropad->size = 0;
}


/**
 * PercentFlags
 */
typedef struct {
	bool zero_padding;   //�]���'0'�Ŗ��߂邩�ۂ�(false�̏ꍇ' '�Ŗ��߂�)
	bool justify_left;   //���񂹂��s��.
	bool alternate_form; //��֌`��(#�t���O)�w�肪����Ă��邩�ۂ�.
	bool force_sign;     //���̐��̏ꍇ�ɕ��������邩�ۂ�.
	bool space_for_positive; //+�̑����' '���g�p���邩�ۂ�.
}PercentFlags;

Znk_INLINE void
PercentFlags_clear( PercentFlags* pfg )
{
	pfg->zero_padding   = false;
	pfg->justify_left   = false;
	pfg->alternate_form = false;
	pfg->force_sign     = false;
	pfg->space_for_positive = true;
}
Znk_INLINE const char*
PercentFlags_get( PercentFlags* pfg, const char* p )
{
	/**
	 * If both the ' ' and '+' flags appear,
	 * the ' ' flag should be ignored.
	 *
	 * If the '0' and '-' flags both appear,
	 * the '0' flag should be ignored.
	 */

	while (*p == '0' || *p == '-' || *p == '+' ||
	       *p == ' ' || *p == '#' || *p == '\'')
	{
		switch (*p) {
		case '0': pfg->zero_padding = true; break;
		case '-': pfg->justify_left = true; break;
		case '+':
			pfg->force_sign         = true;
			pfg->space_for_positive = false;
			break;
		case ' ':
			pfg->force_sign = true;
			/**
			 * If both the ' ' and '+' flags appear, the ' ' flag should be ignored.
			 * But in Perl the last of ' ' and '+' applies.
			 */
#ifdef PERL_COMPATIBLE
			pfg->space_for_positive = true;
#endif
			break;
		case '#': pfg->alternate_form = true; break;
		case '\'': break;
		default:
			break;
		}
		p++;
	}
	return p;
}


/**
 * MinFieldWidth
 */
Znk_INLINE size_t
MinFieldWidth_get( const char** pp, VAWrap* vawp, bool* justify_left )
{
	size_t min_field_width = 0;
	const char* p = *pp;

	if( *p == '*' ){
		int aster_arg;
		++p;
		aster_arg = va_arg(vawp->ap_, int);

		if( aster_arg >= 0 ){
			min_field_width = aster_arg;
		} else {
			min_field_width = -aster_arg;
			*justify_left = true;
		}

	} else if( isdigit((int)(*p)) ){
		/**
		 * size_t could be wider than unsigned int;
		 * make sure we treat argument like common implementations do
		 */
		unsigned int dec_sum = *p - '0';
		++p;
		while( isdigit((int)(*p)) ){
			dec_sum = 10 * dec_sum + (*p - '0');
			++p;
		}

		min_field_width = dec_sum;
	}

	*pp = p;
	return min_field_width;
}


/**
 * PrecisionInfo
 */
typedef struct {
	size_t size;
	bool   specified;
} PrecisionInfo;

Znk_INLINE void
PrecisionInfo_clear( PrecisionInfo* prec_info )
{
	prec_info->size = 0;
	prec_info->specified = false;
}
Znk_INLINE const char*
PrecisionInfo_get( PrecisionInfo* prec_info, const char* p, VAWrap* vawp )
{
	/**
	 * parse PrecisionInfo
	 * %XXX.YYY �� ".YYY" �ɑ������镔��.
	 */
	if (*p != '.') {
		return p;
	}
	p++; // skip '.'

	prec_info->specified = true;

	if (*p == '*') {
		int j = va_arg(vawp->ap_, int);
		p++;

		if (j >= 0){
			prec_info->size = j;
		} else {
			/**
			 * NOTE:
			 *   Solaris 2.6 man page claims
			 *   that in this case the precision should be set to 0.
			 *
			 *   Digital Unix 4.0, HPUX 10 and BSD man page claim
			 *   that this case should be treated as unspecified precision,
			 *   which is what we do here.
			 */
			prec_info->specified = false;
			prec_info->size = 0;
		}
	} else if ( isdigit( (int)(*p) ) ){
		/* size_t could be wider than unsigned int;
		 * make sure we treat argument like common implementations do
		 */
		unsigned int uj = *p++ - '0';
		while( isdigit( (int)(*p) ) ){
			uj = 10*uj + (unsigned int)(*p++ - '0');
		}

		prec_info->size = uj;
	}

	return p;
}


/***
 * LengthModifier
 */
Znk_INLINE int
LengthModifier_get( const char** pp )
{
	/**
	 * 0 ��length_modifier���w�肳��Ă��Ȃ����Ƃ�����(int�^�Ƃ݂Ȃ�)
	 * int16_t/uint16_t�����^�͓����\���Ƃ���1�ɕϊ������.
	 * int32_t/uint32_t�����^�͓����\���Ƃ���2�ɕϊ������.
	 * int64_t/uint64_t�����^�͓����\���Ƃ���3�ɕϊ������.
	 */
	int length_modifier = 0;
	const char* p = *pp;

	switch( *p ){
	case 'h':
	case 'z':
	case 'j':
	case 't':
	case 'L':
		length_modifier = *p;
		++p;
		break;
	case 'l':
		length_modifier = *p;
		++p;
		/**
		 * ll = long long
		 */
		if (length_modifier == 'l' && *p == 'l') {
			length_modifier = '2'; /* encoded as '2' */
			++p;
		}
		break;
	case 'I':
		/**
		 * BitExtend
		 * VC, bcc �ɂ��� I64 modifiers ���T�|�[�g.
		 * C99�W���ł͂Ȃ�.
		 * �܂����ł�I16,I32���T�|�[�g����.
		 */
		switch( *(p+1) ){
		case '1':
			if( *(p+2) == '6' ){
				/* I16 */
				length_modifier = 1; /* encoded as 1 */
				p += 3;
			}
			break;
		case '3':
			if( *(p+2) == '2' ){
				/* I32 */
				length_modifier = 2; /* encoded as 2 */
				p += 3;
			}
			break;
		case '6':
			if( *(p+2) == '4' ){
				/* I64 */
				length_modifier = 3; /* encoded as 3 */
				p += 3;
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	*pp = p;
	return length_modifier;
}

/*
 * endof Data
 **/
/*****************************************************************************/



/*****************************************************************************/
/**
 * GetBodyOfInteger
 */

/**
 * fmt_spec, arg_sign, pfg �Ŏw�肳����񂩂�o�b�t�@��prefix��t������.
 * prefix �Ƃ́A'+'(' ') or 0x(0X) ������.
 * �o�b�t�@�̊J�n�ʒu��p�Ŏ������.
 * prefix�̃o�b�t�@���ł̏I�[�ʒu(�ŏI�����̎��̈ʒu)���߂�l�ƂȂ�.
 */
Znk_INLINE char*
setPrefix_bySignAndFlags( char* p, char fmt_spec, int arg_sign, const PercentFlags* pfg )
{
	if (fmt_spec == 'd') {
		if (pfg->force_sign && arg_sign >= 0)
			*p++ = pfg->space_for_positive ? ' ' : '+';

		/* leave negative numbers for sprintf to handle,
		 * to avoid handling tricky cases like (short int)(-32768) */

#ifdef LINUX_COMPATIBLE
	} else if (fmt_spec == 'p' && pfg->force_sign && arg_sign > 0) {
		*p++ = pfg->space_for_positive ? ' ' : '+';
#endif
	} else if (pfg->alternate_form) {

		/**
		 * alternate form should have no effect for p conversion,
		 * but HPUX 10: for an alternate form of p conversion,
		 * a nonzero result is prefixed by 0x.
		 */
		if (arg_sign != 0 && (fmt_spec == 'x' || fmt_spec == 'X') ) {
			*p++ = '0';
			*p++ = fmt_spec;
		} else {

#ifdef HPUX_COMPATIBLE
			if (fmt_spec == 'p'
#ifndef HPUX_BUG_COMPATIBLE
					/* Actually it uses 0x prefix even for a zero value. */
					&& arg_sign != 0
#endif
			){
				*p++ = '0';
				*p++ = 'x';
			}
#endif

		}

	} else {
	}
	return p;
}

/**
 * buf[ idx ] ����n�܂镶���� "-" or "0x" or "0X" ��
 * �����ꂩ�ł���ꍇ�́A�������X�L�b�v���邽�߁Aidx�����Z����.
 */
Znk_INLINE void
updateZeroPaddingPos( size_t* idx, const char* buf, size_t buf_size )
{
	if(*idx < buf_size && buf[*idx] == '-') { ++*idx; }

	if(*idx <= buf_size - 2 &&
		buf[*idx] == '0' && (buf[*idx+1] == 'x' || buf[*idx+1] == 'X')
	){
		*idx += 2;
	}
}


/***
 * �P�� va_arg( ap, uint16_t ) �ȂǂƂ���� 64bit���gcc(Ver4.8.1)�ł͎��̂悤�Ȍx������������.
 *
 *   Znk_vsnprintf.c:892:42: �x��: �eint16_t�f is promoted to �eint�f when passed through �e...�f [�f�t�H���g�ŗL��]
 *
 * �w�E���Ă�����e�͓K�؂ł��邪�A�������Ȃ���ȉ��̂悤�ɖ��炩�ɖ��Ȃ��悤�ȃR�[�h�ł����̌x���͔������Ă��܂�.
 *   val = ( sizeof(uint16_t) <= sizeof(int) ) ?
 *       (uint16_t)va_arg(ap, int) : va_arg(ap, uint16_t);
 *
 * �܂� va_arg( ap, uint16_t ) �Ƃ����L�q���܂܂�鎞�_��(���ꂪ���炩�Ɏ��s����Ȃ��ꍇ�ł�)���̌x���͏o�Ă��܂�.
 *
 * gcc�͒��炭�t�@�C���̈ꕔ�̃R�[�h�ɑ΂��Ă̂݌x���I�v�V������ύX����@�\���Ȃ�������,
 * gcc 4.6.0����#pragma�ŕύX�\�ƂȂ���. ���Ȃ킿�ȉ��̂悤�ɂ��Čx���𕔕��I���ꎞ�I�ɖ����ɂ��邱�Ƃ��ł���.
 *   #pragma GCC diagnostic push
 *   #pragma GCC diagnostic ignored "-Wformat"
 *       ...
 *   #pragma GCC diagnostic pop
 *
 * �������Ȃ��炱���[�f�t�H���g�ŗL��]�Ƃ���ʂ�A�����Ȃ�x���I�v�V�����ɂ������Ȃ����߁A
 * ���������}�����邱�Ƃ��ł��Ȃ�.
 *
 * ����Ă����Ŏc���ꂽ��i�Ƃ��ẮAint��bit���������ɒ��ׂ�define���A
 * ���̒l�ɂ���Čx���̔�������R�[�h���̂��܂߂Ȃ��悤�ɐ؂�ւ��邵���Ȃ�.
 */

Znk_INLINE int16_t va_arg_int16_t( VAWrap* vawp ){
#if Znk_BIT_int >= 16
	return (int16_t)va_arg( vawp->ap_, int );
#else
	return va_arg( vawp->ap_, int16_t );
#endif
}
Znk_INLINE int32_t va_arg_int32_t( VAWrap* vawp ){
#if Znk_BIT_int >= 32
	return (int32_t)va_arg( vawp->ap_, int );
#else
	return va_arg( vawp->ap_, int32_t );
#endif
}
Znk_INLINE int64_t va_arg_int64_t( VAWrap* vawp ){
#if Znk_BIT_int >= 64
	return (int64_t)va_arg( vawp->ap_, int );
#else
	return va_arg( vawp->ap_, int64_t );
#endif
}
Znk_INLINE uint16_t va_arg_uint16_t( VAWrap* vawp ){
#if Znk_BIT_int >= 16
	return (uint16_t)va_arg( vawp->ap_, unsigned int );
#else
	return va_arg( vawp->ap_, uint16_t );
#endif
}
Znk_INLINE uint32_t va_arg_uint32_t( VAWrap* vawp ){
#if Znk_BIT_int >= 32
	return (uint32_t)va_arg( vawp->ap_, unsigned int );
#else
	return va_arg( vawp->ap_, uint32_t );
#endif
}
Znk_INLINE uint64_t va_arg_uint64_t( VAWrap* vawp ){
#if Znk_BIT_int >= 64
	return (uint64_t)va_arg( vawp->ap_, unsigned int );
#else
	return va_arg( vawp->ap_, uint64_t );
#endif
}


#define M_GET_IVAL_SIGN( ival, sign ) {\
			if      ((ival) > 0) (sign) =  1; \
			else if ((ival) < 0) (sign) = -1; \
}
#define M_GET_UVAL_SIGN( uval, sign ) {\
			(sign) = (uval != 0); \
}

#define M_VA_GET_IVAL( ap, type, val, sign, data_size ) {\
			(val) = va_arg((ap), type); \
			M_GET_IVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}
#define M_VA_GET_UVAL( ap, type, val, sign, data_size ) {\
			(val) = va_arg((ap), type); \
			M_GET_UVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}

#define M_VA_GET_IVAL_EX( vawp, type, val, sign, data_size ) {\
			val = va_arg_##type( vawp ); \
			M_GET_IVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}
#define M_VA_GET_UVAL_EX( vawp, type, val, sign, data_size ) {\
			val = va_arg_##type( vawp ); \
			M_GET_UVAL_SIGN( val, sign ) \
			(data_size) = sizeof(type); \
}


Znk_INLINE bool
getBody_forInteger( ZnkVSNPrintfCtx* vsnp_ctx,
		int length_modifier, char fmt_spec, size_t min_field_width,
		VAWrap* vawp, PercentFlags* pfg )
{

	/**
	 * NOTE:
	 * the u, o, x, X and p conversion specifiers imply the value is unsigned;
	 * d implies a signed value
	 */

	/**
	 * 0 if numeric argument is zero (or if pointer is NULL for 'p'),
	 * +1 if greater than zero (or nonzero for unsigned arguments),
	 * -1 if negative (unsigned argument is never negative)
	 */
	int arg_sign = 0;

	size_t data_size = 0;

	union {
		/**
		 * only defined for length modifier h, or for no length modifiers
		 */
		int          ival;
		unsigned int uval;
		/**
		 * only defined for length modifier l
		 */
		long          lval;
		unsigned long ulval;
		/**
		 * pointer value only defined for p conversion
		 */
		void*         ptr;
		/**
		 * only defined for length modifier ll
		 */
		LongLong   llval;
		ULongLong  ullval;

		intmax_t  intmax_val;
		uintmax_t uintmax_val;
		size_t    size_val;
		ptrdiff_t ptrdiff_val;

		/**
		 * BitExtend
		 */
		int32_t  i32;
		uint32_t u32;
		int64_t  i64;
		uint64_t u64;
	} u;
	u.uintmax_val = 0;


	if (fmt_spec == 'p') {
		/**
		 * HPUX 10:
		 *   An l, h, ll or L before any other conversion character
		 *   (other than d, i, u, o, x, or X) is ignored.
		 *
		 * Digital Unix:
		 *   not specified, but seems to behave as HPUX does.
		 *
		 * Solaris:
		 *   If an h, l, or L appears before any other conversion specifier
		 *   (other than d, i, u, o, x, or X), the behavior is undefined.
		 *   (Actually %hp converts only 16-bits of address and %llp treats address
		 *   as 64-bit data which is incompatible with (void *) argument on a 32-bit system).
		 *
		 * We adopt HPUX behavior.
		 */

		u.ptr = va_arg( vawp->ap_, void* );
		if (u.ptr != NULL) arg_sign = 1;
		data_size = sizeof( void* );

	} else if (fmt_spec == 'd') {  /* signed */

		/**
		 * It is non-portable to specify a second argument of char or short to va_arg,
		 * because arguments seen by the called function are not char or short.
		 * C converts char and short arguments to int before passing them to a function.
		 *
		 * �ϐ��̃T�C�Y���w�肷�� h �Al �AL �ɂ��Ă͒��ӂ��K�v�ł���.
		 * %hd �� short int �A%d �� int �A%ld �� long int �̈����ɑΉ����A
		 * %f �� float �A%lf �� double �A%Lf �� long double �ɑΉ����Ă��邱�ƂɂȂ��Ă���.
		 * �������Ȃ���A�ϒ����������֐��錾�̏ꍇ(�܂��͈����Ɍ^���L�q���Ȃ��Â��^�C�v
		 * �̊֐��錾�̏ꍇ)�ɂ́A�e�����ɂ͕W���ϊ����Ȃ���Aint ���Z������������ int �ɁA
		 * double ���Z�����������_�������� double �ɈÖق̂����ɕϊ������(��).
		 * ���̂��Ƃ��A%hd �� %d �A%f �� %lf �͌��Ǔ����Ӗ��ɂȂ��Ă��܂�.
		 * ����� va_arg(ap, short) �� va_arg(ap, float) �Ƃ������L�q�͏�ɐ����������Ƃ�
		 * ����Ȃ����A���������g���K�v���Ȃ��i�x�����o���Ă���鏈���n������炵��).
		 *
		 * (��)
		 * ���̂悤�ȉϒ��֐������ɂ�����uint��菬�����^��int�ւ̈����グ�v��
		 * �ufloat�^��double�^�ւ̈����グ�v�� �v���O���~���O����C ��2��(K&R)�� A7.3.2 
		 * �֐��Ăяo���̍�(p247,p248)�ɖ����I�ɋL�ڂ���Ă���. �܂��ȉ��̃T�C�g�ɂ�
		 * �킩��₷������������. �u�W���ϊ��v�Ƃ����p��ɂ��Ă͂��̃T�C�g����ؗp����.
		 *
		 * http://www.k-cube.co.jp/wakaba/server/func/va_arg.html
		 */

		/**
		 * %jd, %td ��F�߂�.
		 * %zd (�����t��size_t)�́Asize_t�^���󂯎���Ă���𕄍��t10�i���ŕ\������Ƃ���
		 * �w�������A������ǂ����߂��邩? �������� size_t�^�Ƃ͕����Ȃ��̌^�ł���.
		 * ����𕁒ʂɕ����Ȃ�10�i���ŕ\������Ȃ�΁A���[�U�� %zu �Ǝw�肷�ׂ������A
		 * �����ł͂����Ă������� %zd �Ǝw�肳��Ă���. ����� size_t �^�� ssize_t ��
		 * �L���X�g�������̂�\������Ƃ����ӎu�Ƃ݂Ȃ����Ƃɂ���. �Ⴆ�� size_t �̎��̂�
		 * uint16_t �̊��̏ꍇ�ŁA�n���ꂽ�l�� 65534 �ł������ꍇ�A������ -2 �ƕ\�������.
		 * ����� %d �Ƃ���Ƃ���� (����Ȓl��)unsigned int �̒l���w�肵���ꍇ�̋����ɏ�����.
		 * Unix���ɂ���ssize_t �͕W���ł͂Ȃ�. �Ⴆ��VC,bcc�ł̓G���[�ɂȂ�. size_t �̃T�C�Y
		 * �͈�ʂɂ͂킩��Ȃ����߁A�ȉ��ł� cast_to_intmax_t_from_size_t ��p���ē��^���ٕ����ւ�
		 * ���ۉ����ꂽ�L���X�g���������Ă���.
		 */
		switch (length_modifier) {
		case 1  : M_VA_GET_IVAL_EX( vawp, int16_t,     u.i32,         arg_sign, data_size ) break;
		case 2  : M_VA_GET_IVAL_EX( vawp, int32_t,     u.i32,         arg_sign, data_size ) break;
		case 3  : M_VA_GET_IVAL_EX( vawp, int64_t,     u.i64,         arg_sign, data_size ) break;
		case 0  : /* fall */
		case 'h': M_VA_GET_IVAL(    vawp->ap_, int,         u.ival,        arg_sign, data_size ) break;
		case 'l': M_VA_GET_IVAL(    vawp->ap_, long,        u.lval,        arg_sign, data_size ) break;
		case '2': M_VA_GET_IVAL(    vawp->ap_, LongLong, u.llval,       arg_sign, data_size ) break;
		case 'j': M_VA_GET_IVAL(    vawp->ap_, intmax_t,    u.intmax_val,  arg_sign, data_size ) break;
		case 't': M_VA_GET_IVAL(    vawp->ap_, ptrdiff_t,   u.ptrdiff_val, arg_sign, data_size ) break;
		case 'z':
		{
			size_t size_val = va_arg( vawp->ap_, size_t );
			u.intmax_val = cast_to_intmax_t_from_size_t( size_val );
			M_GET_IVAL_SIGN( u.intmax_val, arg_sign );
			data_size = sizeof(size_t);
			break;
		}
		default:
			assert(0);
			break;
		}

	} else {  /* unsigned */

		/**
		 * �ȉ��� * �Ƃ́Au, o, x, X�Ȃǂ̕�������������\�����邽�߂̎w��q�̂����ꂩ��
		 * ����Ƃ���.
		 *
		 * %j*, %z* ��F�߂�.
		 * %t* (�����Ȃ�ptrdiff_t)�́Aptrdiff_t�^���󂯎���Ă���𕄍����������ŕ\������Ƃ���
		 * �w�������A������ǂ����߂��邩? �������� ptrdiff_t�^�Ƃ͕����t���̌^�ł���.
		 * ����𕁒ʂɕ����t��10�i���Ȃǂŕ\������Ȃ�΁A���[�U�� %td �ȂǂƎw�肷�ׂ������A
		 * �����ł͂����Ă������� %t* (*��d�ȊO)�Ǝw�肳��Ă���. ����� ptrdiff_t �^�𓯌^��
		 * unsigned ptrdiff_t �^�Ƃł������ׂ��^ �փL���X�g�������̂�\������Ƃ����ӎu��
		 * �݂Ȃ����Ƃɂ���. �Ⴆ�� ptrdiff_t �̎��̂� int16_t �̊��̏ꍇ�ŁA�n���ꂽ�l��
		 * -2 �ł���A%tu �Ǝw�肳��Ă���ꍇ�A������ 65534 �ƕ\�������.
		 * ����� %u �Ƃ���Ƃ���� signed int �̕��̒l���w�肵���ꍇ�̋����ɏ�����.
		 * ptrdiff_t �̃T�C�Y�͈�ʂɂ͂킩��Ȃ����߁A�ȉ��ł� cast_to_uintmax_t_from_ptrdiff_t ��p����
		 * ���^���ٕ����ւ̒��ۉ����ꂽ�L���X�g���������Ă���.
		 */
		switch (length_modifier) {
		case 1  : M_VA_GET_UVAL_EX( vawp, uint16_t,     u.u32,         arg_sign, data_size ) break;
		case 2  : M_VA_GET_UVAL_EX( vawp, uint32_t,     u.u32,         arg_sign, data_size ) break;
		case 3  : M_VA_GET_UVAL_EX( vawp, uint64_t,     u.u64,         arg_sign, data_size ) break;
		case 0  : /* fall */
		case 'h': M_VA_GET_UVAL(    vawp->ap_, unsigned int,  u.uval,        arg_sign, data_size ) break;
		case 'l': M_VA_GET_UVAL(    vawp->ap_, unsigned long, u.ulval,       arg_sign, data_size ) break;
		case '2': M_VA_GET_UVAL(    vawp->ap_, ULongLong,  u.ullval,      arg_sign, data_size ) break;
		case 'j': M_VA_GET_UVAL(    vawp->ap_, uintmax_t,     u.uintmax_val, arg_sign, data_size ) break;
		case 'z': M_VA_GET_UVAL(    vawp->ap_, size_t,        u.size_val,    arg_sign, data_size ) break;
		case 't':
		{
			ptrdiff_t ptrdiff_val = va_arg( vawp->ap_, ptrdiff_t );
			u.uintmax_val = cast_to_uintmax_t_from_ptrdiff_t( ptrdiff_val );
			M_GET_UVAL_SIGN( u.uintmax_val, arg_sign );
			data_size = sizeof(ptrdiff_t);
			break;
		}
		default:
			assert(0);
			break;
		}
	}

	{
		/**
		 * justify_left �ւ̑Ή����K�v��...
		 */
	
		char   body[sizeof(uintmax_t)*16];
		size_t body_leng = setPrefix_bySignAndFlags( body, fmt_spec, arg_sign, pfg ) - body;
		size_t body_size = sizeof(body);

		ZnkToStrFlags flags = ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_COMPACT;
		char space_ch = pfg->zero_padding ? '0' : ' ';

		char*  begin    = body + body_leng;
		size_t capacity = body_size - body_leng;
		size_t width = min_field_width;
		size_t radix = 10;
		bool   is_unsigned = true;

		if( space_ch == '0' ){
			flags |= ZnkToStr_e_IS_SIGN_TOP;
		}
	
		if (fmt_spec == 'p'){
			width = data_size * 2;
			radix = 16;
			is_unsigned = true;
	
		} else if (fmt_spec == 'd') {  /* signed */
			/**
			 * %d �̏ꍇ�Amin_field_width == 0
			 */
			width = min_field_width;
			radix = 10;
			is_unsigned = false;
	
		} else { /* unsigned */
			/**
			 * %u ���̏ꍇ�Amin_field_width == 0
			 */
			width = min_field_width;
	
			/**
			 * radix����.
			 */
			switch( fmt_spec ){
			case 'x':
				flags |= ZnkToStr_e_IS_LOWER;
				radix = 16;
				break;
			case 'X':
				flags &= ~ZnkToStr_e_IS_LOWER;
				radix = 16;
				break;
			case 'o':
				radix = 8;
				break;
			default:
				radix = 10;
				break;
			}
			is_unsigned = true;
		}

		/**
		 * BitExtend
		 */
		if( is_unsigned ){
			switch( data_size ){
			case 1: case 2: case 4:
				body_leng += ZnkToStr_U32_getStr( begin, capacity, u.u32, width, space_ch, radix, flags ); break;
			case 8:
				body_leng += ZnkToStr_U64_getStr( begin, capacity, u.u64, width, space_ch, radix, flags ); break;
			default:
				assert( 0 );
				break;
			}
		} else {
			switch( data_size ){
			case 1: case 2: case 4:
				body_leng += ZnkToStr_I32_getStr( begin, capacity, u.i32, width, space_ch, radix, flags ); break;
			case 8:
				body_leng += ZnkToStr_I64_getStr( begin, capacity, u.i64, width, space_ch, radix, flags ); break;
			default:
				assert( 0 );
				break;
			}
		}

		vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
		vsnp_ctx->pos_ += body_leng;
	}

	return true;
}


Znk_INLINE void
updatePrecision_byAlternateForm( PrecisionInfo* prec_info,
		bool alternate_form, char fmt_spec, size_t zeropad_pos,
		const char* body, size_t body_leng )
{
	/**
	 * 0���߂������������{�̂����錅��.
	 */
	const size_t digit_size = body_leng - zeropad_pos;

	if( alternate_form && fmt_spec == 'o'
#ifdef HPUX_COMPATIBLE
			/* ("%#.o",0) -> ""  */
			&& (body_leng > 0)
#endif

#ifdef DIGITAL_UNIX_BUG_COMPATIBLE
			/* ("%#o",0) -> "00" */
#else
			/**
			 * unless zero is already the first character
			 */
			&& !( zeropad_pos < body_leng && body[ zeropad_pos ] == '0' )
#endif
	){
		/**
		 * assure leading zero for alternate-form octal numbers
		 */
		if( !prec_info->specified || prec_info->size <= digit_size ){
			/**
			 * prec_info.size is increased to force the first character to be zero,
			 * except if a zero value is formatted with an explicit prec_info.size of zero
			 */
			prec_info->size      = digit_size + 1;
			prec_info->specified = true;
		}
	}
}

/*
 * endof GetBodyOfInteger
 **/
/*****************************************************************************/




/*****************************************************************************/
/**
 * GetBodyOfDouble
 */
Znk_INLINE void
supplement_forDecE( char* body, size_t body_size,
		size_t min_field_width, size_t* body_leng, char space_ch )
{
	if( min_field_width > 0 && min_field_width > *body_leng ){
		char* mid_ch;
		/**
		 * %e �� min_field_width ��^���邱�Ƃ͂Ȃ��Ǝv����
		 * �^�����ꍇ�́A����͎w�����܂ł����ׂĊ܂߂��S�̂̒���������.
		 * �]���āAspace_ch �𖄂߂� supplement_size �� min_field_width - body_leng �ƂȂ�.
		 * �܂��ABuf_memmove �� body ���� memmove ���鏈�������邽�߁A�����x���Ȃ�.
		 */
		const size_t supplement_size = min_field_width - *body_leng;

		if( body_size ){ /* body_size��0�̏ꍇ�͏ȗ����邱�Ƃō����� */
			Buf_memmove( body, body_size, supplement_size, body, *body_leng+1 );
			Buf_memset( body, body_size, 0, space_ch, supplement_size );

			/**
			 * ���������͂�������Ȃ��body�̒��Ԃɂ���͂��ł���.
			 * (flags�ɂ���Ă͕��ł��t���Ȃ��ꍇ�����邵�A���ł��t���ꍇ������)
			 * ����������Aspace_ch == '0' �̂Ƃ��͂��ꂾ�Ƃ��������̂ŁA
			 * �����Ɛ擪�̕���('0')�Ɠ���ւ���
			 */
			mid_ch = body + supplement_size;
			if( space_ch == '0' &&
			  ( *mid_ch == '+' || *mid_ch == '-' || *mid_ch == ' ' ) ){
				Znk_SWAP( char, *mid_ch, body[0] );
			}
		}
		*body_leng += supplement_size;
	}
}

/**
 * buf �����w���|�C���^ del_begin ���w����������
 * del_end ���w�������̎�O�܂ł��폜����.
 * �폜��̕�����̒�����buf_leng�ɍX�V����.
 */
Znk_INLINE void
CStr_deletePart( char* buf, size_t buf_size, size_t* buf_leng, char* del_begin, char* del_end )
{
	char* end = buf + *buf_leng;
	size_t back_size = end - del_end; 
	size_t del_pos = del_begin - buf;
	Buf_memmove( buf, buf_size, del_pos, del_end, back_size );
	*buf_leng = del_pos + back_size;
}

Znk_INLINE bool
getBody_forDouble( ZnkVSNPrintfCtx* vsnp_ctx,
		double dval, size_t min_field_width, char fmt_spec,
		const PrecisionInfo* prec_info, char space_ch )
{
	/**
	 * �����Ǝ����I�u�W�F�N�g�������ꍇ�Ɏg�p����ꎞ�o�b�t�@.
	 * temporary buffer for simple integer or double->cstring conversion
	 */
	char   body[ 512 ];
	size_t body_leng     = 0;
	size_t decimal_width = 0;
	char   sign_policy   = 'E';
	/**
	 * VC�Ȃ�΂�����3
	 * GCC�Ȃ�΂�����2
	 * BCC�Ȃ�΂�����2
	 */
	size_t exp_width = 2;

	/**
	 * printf�ł͒Z��integer������0�����߂��邱�Ƃ͂����Ă�
	 * ����integer���؂�̂Ă��邱�Ƃ͂Ȃ�.
	 */
	ZnkToStrFlags flags = ZnkToStr_e_IS_SUPPLEMENT | ZnkToStr_e_IS_COMPACT |
		ZnkToStr_e_IS_EXP_PLUS | ZnkToStr_e_IS_EXP_ZERO_SIGN_PLUS |
		ZnkToStr_e_IS_EXP_SUPPLEMENT;
	if( space_ch == '0' ){ flags |= ZnkToStr_e_IS_SIGN_TOP; }

	switch( fmt_spec ){
	case 'f': case 'F':
	{
		size_t int_width;
		decimal_width = prec_info->specified ? prec_info->size : 6;
		int_width = (min_field_width == 0) ?
			0 :
			(min_field_width >= decimal_width + 1) ?
				min_field_width - (decimal_width + 1) :
				0;

		body_leng = ZnkToStr_Double_getStrF( body, sizeof(body), dval,
				int_width, decimal_width,
				space_ch, flags, sign_policy );

		//fprintf(stderr, "dval=[%e] f=[%s] body_leng=[%u] int_width=[%u] decimal_width=[%u]\n", dval, body, body_leng, int_width, decimal_width);
		break;
	}
	case 'E': case 'e':
	{
		if( fmt_spec == 'e' ){ flags |= ZnkToStr_e_IS_EXP_LOWER; }

		/**
		 * printf�ł͒Z��integer������0�����߂��邱�Ƃ͂����Ă�
		 * ����integer���؂�̂Ă��邱�Ƃ͂Ȃ�.
		 */
		decimal_width = prec_info->specified ? prec_info->size : 6;

		body_leng = ZnkToStr_Double_getStrE( body, sizeof(body), dval,
				decimal_width, exp_width, flags );

		supplement_forDecE( body, sizeof(body), min_field_width, &body_leng, space_ch );

		//fprintf(stderr, "dval=[%e] e=[%s] body_leng=[%u]\n", dval, body, body_leng);
		break;
	}
	case 'G': case 'g':
	{
		size_t int_width = 0;
		char selected_method;
		char* p;

		if( fmt_spec == 'g' ){ flags |= ZnkToStr_e_IS_EXP_LOWER; }

		/**
		 * %g �ł� %f �Ɛ��x�̈Ӗ����قȂ�A�������Ə������̘a������.
		 * ���̂��߁A����%e�Ɖ��߂���ĉ�����(1)�ƂȂ����ꍇ�A
		 * �������̃f�t�H���g���� 6-1=5 �Ƃ������ƂɂȂ�.
		 */
		decimal_width = prec_info->specified ? prec_info->size : 6;

		body_leng = ZnkToStr_Double_getStrG_Ex( body, sizeof(body), dval,
				int_width, decimal_width, exp_width,
				space_ch, flags, sign_policy,
				&selected_method );

		//fprintf(stderr, "body=[%s], decimal_width=[%u] selected_method=[%c]\n", body, decimal_width, selected_method);
		/**
		 * ���̏������K�v�ȗ��R�͈ȉ��ɂ��.
		 * �܂� %X.Yg �Ƃ������ꍇ�� ���ꂪ %f �Ɖ��߂��ꂽ�ꍇ��
		 * Y�͏����_�ȉ��̌����ł͂Ȃ��A��������+���������̌��ƂȂ�.
		 * ���������͎��ۂ̐������������߂Ă���o�Ȃ��ƌ���ł��Ȃ�.
		 * ���ۂ̐��������͏k������Ȃ����A���������͐��������ɉ����ďk������邩��ł���.
		 * ���̂��߁A�����ł͈�UY(���Ȃ킿�����ł� decimal_width) �����擪����X�L����������,
		 * body_leng ���X�V���Ă���.
		 */
		p = body;
		{
			size_t num = 0;
			/**
			 * �������X�L�b�v
			 */
			if( *p == '+' || *p == '-' || *p == ' ' ){ ++p; }
			/**
			 * space_ch���߂��X�L�b�v
			 * (�����Ƃ��Aint_width=0�Ƃ��Ă��邽�߁A����͂Ȃ��͂�)
			 */
			while( *p == space_ch ){ ++p; }
			while( *p != '\0' && num < decimal_width ){
				if( *p == '.' ){ ++p; continue; }
				++p;
				++num;
			}
		}

		if( selected_method == 'f' || *p == '\0' ){
			body_leng = p - body;
		} else {
			/**
			 * selected_method == 'e' �̏ꍇ�A���̈ʒup����'e' �܂���'E'���n�܂��O�܂ł�
			 * �������Ȃ���΂Ȃ�Ȃ�.
			 */
			char* del_begin = p;
			while( *p != '\0' && *p != 'e' && *p != 'E' ){ ++p; }
			if( *p == '\0' ){
				/**
				 * 'e' or 'E' ���Ȃ�.
				 */
				body_leng = del_begin - body;
			} else {
				/**
				 * del_begin ���� p �܂ł���������. 
				 * ������̕����񒷂�body_leng�ɍX�V����.
				 */
				CStr_deletePart( body, sizeof(body), &body_leng, del_begin, p );
			}

			//0123456789
			//...x..e...
		}
		supplement_forDecE( body, sizeof(body), min_field_width, &body_leng, space_ch );

		//fprintf(stderr, "dval=[%e] e=[%s] body_leng=[%u]\n", dval, body, body_leng);
		break;
	}
	case 'A': case 'a':
	{
		if( fmt_spec == 'a' ){ flags |= ZnkToStr_e_IS_LOWER; flags |= ZnkToStr_e_IS_EXP_LOWER; }
		if( !prec_info->specified ){ flags |= ZnkToStr_e_IS_FLEX_DECIMAL; }

		/**
		 * %a �̏ꍇ�����Ȃ����w������GCC4,VC8�Ƃ���supplement�Ȃ�.
		 * (C99�ɂ�����߂��Ă���?)
		 * ���̂��߁A�����ł�exp_width=0���w�肵�Ă���.
		 */
		decimal_width = prec_info->specified ? prec_info->size : 0;

		body_leng = ZnkToStr_Double_getStrHexE( body, sizeof(body), dval,
				decimal_width, exp_width, flags );
		break;
	}
	default:
		assert(0);
		break;
	}//endof switch

	{
		vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
		vsnp_ctx->pos_ += body_leng;
	}
	return true;
}

/*
 * endof GetBodyOfDouble
 **/
/*****************************************************************************/




/*****************************************************************************/
/**
 * Znk_vsnprintf_general
 */

Znk_INLINE const char*
parsePercentFormat( const char* p, ZnkVSNPrintfCtx* vsnp_ctx, VAWrap* vawp )
{
#if defined(PERL_COMPATIBLE) || defined(LINUX_COMPATIBLE)
	const char *begin_p = p;
#endif
	PercentFlags pfg;
	size_t min_field_width;
	PrecisionInfo prec_info;
	int  length_modifier;
	char fmt_spec;

	/**
	 * �I�u�W�F�N�g�������̏ꍇ�̂݁A0 �ȊO�̒l�ɒ��������ꍇ������.
	 * ���̑��̃I�u�W�F�N�g�̏ꍇ�A��{�I�� zeropad.pos == 0, zeropad.size == 0 �̂܂܂ɂȂ�.
	 */
	ZeroPad zeropad;


	//fprintf(stderr, "p=[%s] pos=[%u]\n", p, pos);
	
	++p;  /* skip '%' */


	/**
	 * PercentFlags
	 */
	PercentFlags_clear( &pfg );
	p = PercentFlags_get( &pfg, p );


	/**
	 * MinFieldWidth
	 * %9d or %9f �� 9 �̕���
	 * �f�t�H���g�l(%d�ƋL�q�����ꍇ��)�� 0 
	 */
	min_field_width = MinFieldWidth_get( &p, vawp, &pfg.justify_left );


	/**
	 * PrecisionInfo
	 * %6.3f �� 3 �̕���
	 */
	PrecisionInfo_clear( &prec_info );
	p = PrecisionInfo_get( &prec_info, p, vawp );


	/**
	 * LengthModifier
	 * %ld or %hd or %lld �� l, h, ll �̕���. 
	 * ll �� �����̕֋X��A�����I�� 2 �ɒu�������.
	 */
	length_modifier = LengthModifier_get( &p );


	/**
	 * fmt_spec
	 * �Ⴆ�� %10d �� d �̕���.
	 * (current conversion specifier character)
	 */
	fmt_spec = *p;

	/**
	 * common synonyms:
	 * modify fmt_spec and length_modifier by fmt_spec.
	 */
	{
		switch (fmt_spec) {
		case 'i': fmt_spec = 'd'; break;
		case 'D': fmt_spec = 'd'; length_modifier = 'l'; break;
		case 'U': fmt_spec = 'u'; length_modifier = 'l'; break;
		case 'O': fmt_spec = 'o'; length_modifier = 'l'; break;
		default: break;
		}
	}


	ZeroPad_clear( &zeropad );


	/**
	 * get parameter value, do initial processing
	 */
	switch (fmt_spec) {
	case '%': /* % behaves similar to 's' regarding flags and field widths */
	case 'c': /* c behaves similar to 's' regarding flags and field widths */
	case 's':
	{
		const char *body = NULL;
		size_t body_leng = 0;

		/**
		 * %c �̂Ƃ��Ɏg�p����ꎞ�o�b�t�@.
		 *
		 * unsigned char argument value - only defined for c conversion.
		 * N.B. standard explicitly states the char argument for the c conversion is unsigned
		 */
		unsigned char uchar_arg;

		length_modifier = 0;          /* wint_t and wchar_t not supported */

		/**
		 * the result of zero padding flag with non-numeric conversion specifier is undefined.
		 * Solaris and HPUX 10 does zero padding in this case, Digital Unix and Linux does not.
		 */
#if !defined(SOLARIS_COMPATIBLE) && !defined(HPUX_COMPATIBLE)
		pfg.zero_padding = false;    /* turn zero padding off for cstring conversions */
#endif

		body_leng = 1;
		switch (fmt_spec) {
		case '%':
			body = p;
			break;
		case 'c':
		{
			int ival = va_arg(vawp->ap_, int);
			/**
			 * standard demands unsigned char
			 * ���̃X�R�[�v���o�Ă�body�͎g�p����邽�߁A
			 * �K�� uchar_arg �Ɉ�U�擾���Ȃ���΂Ȃ�Ȃ�.
			 */
			uchar_arg = M_STATIC_CAST(unsigned char, ival);
			body = M_REINTERPRET_CAST( const char*, &uchar_arg );
			break;
		}
		case 's':
			body = va_arg(vawp->ap_, const char *);
			if (!body){
				/**
				 * make sure not to address cstring beyond the specified precision !!!
				 */
				body_leng = 0;
			} else if (!prec_info.specified) {
				/**
				 * truncate cstring if necessary as requested by precision
				 */
				body_leng = strlen(body);
			} else if (prec_info.size == 0) {
				body_leng = 0;
			} else {
				body_leng = Buf_findByte( body, prec_info.size, '\0' );
			}
			break;
		default: break;
		}
		{
			vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
			vsnp_ctx->pos_ += body_leng;
		}

		break;
	}
	/*
	 * va_arg ���� non-POD �^���擾���Ă͂����Ȃ�(����`�̓���ɂȂ�)
	 * �K�i����5.2.2 Function call�̑�7�i���ɂ���
	 * non-POD �^���ψ����ɓn�������̐U�镑���͖���`�Ƃ���Ă���.
	 * ( va_arg�ł̓r�b�g�P�ʂ̃R�s�[�����s���Ȃ����߁Anon-POD�ł̓R�s�[��
	 * ���s����\��������. )
	 *
	 * VC�̏ꍇ���ꂪ�f�ʂ肵�A���Ғʂ蓮������Ă��܂������S�ɏ����n�ˑ��ł���.
	 * g++ �̏ꍇ�A���̂悤�ȋL�q������ƁA�R���p�C�����Ɉȉ��̂悤�Ȍx�����o��.
	 * warning: cannot receive objects of non-POD type
	 * `const struct std::string' through `...'; call will abort at runtime
	 * ����Ɏ��ۏ�̌x���ʂ�A���s���� Abort����.
	 *
	 * �]���� std::basic_string �� %S ��Ή�������悤�Ȏd�l�͕s�\�ł���.
	 **/

	case 'd': case 'u':
	case 'o':
	case 'x': case 'X':
	case 'p':
	{
		getBody_forInteger( vsnp_ctx,
				length_modifier, fmt_spec, min_field_width,
				vawp, &pfg );
		break;
	}

	case 'f': case 'F':
	case 'e': case 'E':
	case 'g': case 'G':
	case 'a': case 'A':
	{
		double dval;
		if(length_modifier == 'L') {
			/**
			 * long double�͍��̓T�|�[�g���Ȃ�.
			 */
			dval = M_P_VA_ARG_AS_TYPE( double, vawp->ap_, long double );
		} else {
			/***
			 * C����d�l�u�ψ����ł� float=>double�W���ϊ��v�ɂ��Afloat�̏ꍇ�͍l���s�v.
			 */
			dval = va_arg(vawp->ap_, double);
		}

		getBody_forDouble( vsnp_ctx,
				dval, min_field_width, fmt_spec,
				&prec_info, (char)(pfg.zero_padding ? '0' : ' ') );
		break;
	}

	default:
	{
		/*
		 * default of fmt_spec switch:
		 * unrecognized conversion specifier, keep format cstring as-is
		 **/
		const char *body = NULL;
		size_t body_leng = 0;

		/**
		 * turn zero padding off for non-numeric convers.
		 */
		pfg.zero_padding = false;

#ifndef DIGITAL_UNIX_COMPATIBLE
		/**
		 * reset flags
		 */
		pfg.justify_left = true;
		min_field_width  = 0;
#endif

#if defined(PERL_COMPATIBLE) || defined(LINUX_COMPATIBLE)
		/**
		 * keep the entire format cstring unchanged.
		 * well, not exactly so for Linux, which does something inbetween,
		 * and I don't feel an urge to imitate it: "%+++++hy" -> "%+y"
		 */
		body      = begin_p;
		body_leng = p - begin_p;
#else
		/**
		 * discard the unrecognized conversion,
		 * just keep the unrecognized conversion character.
		 */
		body      = p;
		body_leng = 0;
#endif

		/**
		 * include invalid conversion specifier unchanged
		 * if not at end-of-cstring
		 */
		if( *p ){ ++body_leng; }

		{
			vsnp_ctx->add_func_( vsnp_ctx, body, body_leng );
			vsnp_ctx->pos_ += body_leng;
		}

		break;
	}
	} // endof switch

	/**
	 * �܂����̕����񂪑����悤�Ȃ�
	 * fmt_spec �̎��̕������w���悤�� p ��i�߂Ă���.
	 * step over the just processed conversion specifier
	 * if not at end-of-cstring
	 */
	if( *p ){ ++p; }

	return p;
}


bool
Znk_vsnprintf_general( ZnkVSNPrintfCtx* vsnp_ctx, const char* fmt, va_list ap )
{
//#if defined(__GNUC__)
//#  pragma GCC diagnostic push
//#  pragma GCC diagnostic ignored "-Wunused-but-set-variable"
//#endif
	/***
	 * ����pos�̒l�͍ŏI�I�ɂ�fmt��W�J���č���镶���񂪉�byte�ɂȂ邩���������̂ƂȂ�.
	 * �����buf_size�̐����Ƃ͊�{�I�Ɋ֌W���Ȃ�.
	 */
	//size_t pos = 0;

	const char* p = fmt;
	const char* q = NULL;
	size_t copy_size   = 0;
	size_t copied_size = 0;
	VAWrap vawp;
	M_P_VA_COPY( vawp.ap_, ap );

	/**
	 * In contrast with POSIX, the ISO C99 now says
	 * that buf can be NULL and buf_size can be 0.
	 * This is more useful than the old:  if (buf_size < 1) return -1;
	 */
	if (!p) p = "";

	while( *p ){
		if( *p != '%' ){
			/**
			 * if (pos < buf_size) buf[pos++] = *p++;    -- this would be sufficient
			 * but the following code achieves better performance for cases
			 * where format cstring is long and contains few conversions
			 */
			q = strchr(p+1,'%');

			if( q == NULL ){
				/***
				 * p�ȍ~�A���͂�%�͏o�����Ȃ�.
				 * ���Ȃ킿�c������̂܂܃R�s�[���ďI���ł悢.
				 */
				copy_size = strlen(p);
				/***
				 * �����ŕK�v�Ƃ����pos�͌����݂̃T�C�Y�ł���Abuf�Ɏ��ۉ�byte�R�s�[���ꂽ���͊֌W���Ȃ�.
				 * ����Ă������� pos += Buf_memcpy( ... ) �ȂǂƂ��Ă͂����Ȃ�.
				 */
				vsnp_ctx->add_func_( vsnp_ctx, p, copy_size );
				vsnp_ctx->pos_ += copy_size;
				break;
			}

			/***
			 * %���o��������O�܂ł����̂܂܃R�s�[����.
			 */
			copy_size = q - p;
			copied_size = vsnp_ctx->add_func_( vsnp_ctx, p, copy_size );

			/***
			 * ZnkVSNPrintfCtx�̎����ɕs������݂���ꍇ�A
			 * ���̋t�]���ۂ��������邩������Ȃ�.
			 */
			assert( copied_size <= copy_size );

			/***
			 * �����ŕK�v�Ƃ����pos�͌����݂̃T�C�Y�ł���Abuf�Ɏ��ۉ�byte�R�s�[���ꂽ���͊֌W���Ȃ�.
			 * ����Ă������� pos += copied_size �ȂǂƂ��Ă͂����Ȃ�.
			 */
			vsnp_ctx->pos_ += copy_size;
			p   += copy_size;

			/***
			 * ���̎��_�� copied_size < copy_size �̏ꍇ:
			 *   buf_size�����ɂЂ��������Ă���.
			 *   �������Ȃ��炱����break�Ȃǂ��Ă͂Ȃ�Ȃ�.
			 *   ���f�����pos�̒l���������̂܂܏I����Ă��܂�.
			 *
			 * ���̎��_�� copied_size == copy_size �̏ꍇ:
			 *   %�̈��O�܂ł̃R�s�[���������Ă���.
			 *   ���̎��_�� p �͕K������ % ���w���Ă���͂��ł���.
			 *   (q����NULL�ł��������߁Ap��NULL�������w���Ă���\���͂Ȃ�)
			 *
			 * ���ǁA������̏ꍇ�ł����������A���̂܂�parsePercentFormat�ֈڍs����΂悢.
			 */
		}

		p = parsePercentFormat( p, vsnp_ctx, &vawp );
	}
	

	if( vsnp_ctx->terminate_null_func_ ){
		vsnp_ctx->terminate_null_func_( vsnp_ctx );
	}

	/**
	 * Return the number of characters formatted (excluding trailing null
	 * character), that is, the number of characters that would have been
	 * written to the buffer if it were large enough.
	 *
	 * The value of pos should be returned,
	 * but pos is of unsigned type size_t, and snprintf is int,
	 * possibly leading to an undetected integer overflow,
	 * resulting in a negative return value, which is illegal.
	 * Both XSH5 and ISO C99 (at least the draft) are silent on this issue.
	 * Should errno be set to EOVERFLOW and EOF returned in this case???
	 */
	//return (int) pos;
	return true;
//#if defined(__GNUC__)
//#  pragma GCC diagnostic pop
//#endif
}
/*
 * endof Znk_vsnprintf_general
 **/
/*****************************************************************************/


struct BufAndSizeInfo {
	char*   buf_ref_;
	size_t  buf_size_;
};
static size_t
My_memcpy( ZnkVSNPrintfCtx* vsnp_ctx, const void* src, size_t req_size )
{
	struct BufAndSizeInfo* my_info = (struct BufAndSizeInfo*)vsnp_ctx->user_info_;
	return Buf_memcpy( my_info->buf_ref_, my_info->buf_size_, vsnp_ctx->pos_, src, req_size );
}
static void
My_terminate_null( ZnkVSNPrintfCtx* vsnp_ctx )
{
	struct BufAndSizeInfo* my_info = (struct BufAndSizeInfo*)vsnp_ctx->user_info_;
	/**
	 * make sure the cstring is null-terminated
	 * even at the expense of overwriting the last character
	 * (shouldn't happen, but just in case)
	 */
	my_info->buf_ref_[ Znk_MIN(vsnp_ctx->pos_, my_info->buf_size_-1) ] = '\0';
}

int
Znk_vsnprintf_C99( char* buf, size_t buf_size, const char* fmt, va_list ap )
{
	struct BufAndSizeInfo my_info;
	ZnkVSNPrintfCtx vsnp_ctx;

	my_info.buf_ref_  = buf;
	my_info.buf_size_ = buf_size;

	vsnp_ctx.user_info_ = &my_info;
	vsnp_ctx.add_func_  = My_memcpy;
	vsnp_ctx.terminate_null_func_ = My_terminate_null;
	vsnp_ctx.pos_ = 0;

	Znk_vsnprintf_general( &vsnp_ctx, fmt, ap );
	return (int)vsnp_ctx.pos_;
}

