#ifndef INCLUDE_GUARD__Znk_tostr_double_h__
#define INCLUDE_GUARD__Znk_tostr_double_h__

#include <Znk_tostr_int.h>

Znk_EXTERN_C_BEGIN

double ZnkDouble_getPositiveInf( void );
double ZnkDouble_getNegativeInf( void );

bool   ZnkDouble_isPositiveInf( double dval );
bool   ZnkDouble_isNegativeInf( double dval );
bool   ZnkDouble_isInf   ( double dval );

double ZnkDouble_getNaN  ( double fraction );
bool   ZnkDouble_isNaN   ( double dval );
bool   ZnkDouble_isFinite( double dval );

size_t
ZnkToStr_Double_getStrF( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy );
size_t
ZnkToStr_Double_getStrE( char* buf, size_t buf_size, double dval,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags );
size_t
ZnkToStr_Double_getStrG( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy );
size_t
ZnkToStr_Double_getStrG_Ex( char* buf, size_t buf_size, double dval,
		size_t int_width, size_t decimal_width, size_t exp_width,
		char space_ch, ZnkToStrFlags flags, char sign_policy,
		char* selected_method );
size_t
ZnkToStr_Double_getStrHexE( char* buf, size_t buf_size, double dval,
		size_t decimal_width, size_t exp_width, ZnkToStrFlags flags );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
