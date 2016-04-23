#include <Znk_math.h>

static size_t
factorial( size_t ival ){
	size_t ret = 1;
	size_t i;
	/* 当然ながらリカーシブコールは使わない */
	for( i=2; i<=ival; ++i ){
		ret *= i;
	}
	return ret;
}

/* Lanczos approximation */

static double
getA( double dval, const double* coefs, size_t coefs_size )
{
	size_t i;
	double ret = coefs[0];
	dval -= 1.0;
	for( i=1; i<coefs_size; ++i ){
		ret += coefs[i] / (dval + i);
	}
	return ret;
}

static double
get_gamma_ln( double dval )
{
	static const double coefs[] = {
		 2.50662827329,
		 2901.41861668,
		-5929.16815028,
		 4148.27391368,
		-1164.7605648,
		 117.413508415,
		-2.78664112091,
		 0.00379359722137,
	};
	static const double g = 7.0;
	const double A = getA( dval, coefs, Znk_NARY(coefs) );
	return log( A ) + (dval - 0.5) * log(dval + g) - (dval + g);
}
Znk_INLINE double
get_gamma_1( double dval )
{
	return exp( get_gamma_ln(dval) );
}

static double
get_gamma_2( double dval )
{
	static const double coefs[] = {
		 1.000000000178,
		 76.180091729406,
		-86.505320327112,
		 24.014098222230,
		-1.231739516140,
		 0.001208580030,
		-0.000005363820,
	};
	static const double g = 4.5;
	const double A = getA( dval, coefs, Znk_NARY(coefs) );
    return sqrt( 2.0 * M_PI ) * pow( dval + g, dval - 0.5 ) * exp( -(dval + g) ) * A;
}

double
ZnkMath_gamma( double dval )
{
	double rdvf = 1.0;
	double integer;
	double fraction = modf( dval, &integer );

	/* 正の整数であった場合は高速化のため通常の階乗計算を用いる */
	if( dval > 0 && fraction == 0 ){
		return (double)factorial( (size_t)(dval)-1 );
	}

	/* 1.0未満の入力であった場合は、それ以上になるまでrdvfを以下のように算出. */
	while( dval < 1.0 ){
		rdvf *= 1/dval;
		dval += 1.0;
	}
	return rdvf * get_gamma_2( dval );
}

