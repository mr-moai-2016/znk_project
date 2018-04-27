/***
 * Mersenne twister(MT19937) method.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 *
 * Before using, initialize the state by using init_genrand(seed)  
 * or init_by_array(init_key, key_length).
 *
 * Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 * All rights reserved.                          
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. The names of its contributors may not be used to endorse or promote 
 *      products derived from this software without specific prior written 
 *      permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Znk_rand.h"
#include <time.h>

#if defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#elif defined(Znk_TARGET_UNIX)
#  include <sys/time.h>
#endif

/* Period parameters */  
#define M_NSV 624
#define M_M   397
#define M_MATRIX_A   0x9908b0dfUL /* constant vector a */
#define M_UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define M_LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long st_sv[ M_NSV ];     /* the array for the state vector  */
static int           st_sv_idx = M_NSV+1; /* st_sv_idx==M_NSV+1 means st_sv[M_NSV] is not initialized */

/* initializes st_sv[M_NSV] with a seed */
Znk_INLINE void
init_genrand( const unsigned long s )
{
	st_sv[0]= s & 0xffffffffUL;
	for( st_sv_idx=1; st_sv_idx<M_NSV; ++st_sv_idx ){
		st_sv[st_sv_idx] = (1812433253UL * (st_sv[st_sv_idx-1] ^ (st_sv[st_sv_idx-1] >> 30)) + st_sv_idx); 
		/***
		 * See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
		 * In the previous versions, MSBs of the seed affect
		 * only MSBs of the array st_sv[].
		 * modified by Makoto Matsumoto
		 */
		st_sv[st_sv_idx] &= 0xffffffffUL;
		/* for >32 bit machines */
	}
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
Znk_INLINE void
init_by_array( const unsigned long* init_key, int key_length )
{
	int i, j, k;
	init_genrand(19650218UL);
	i=1; j=0;
	k = (M_NSV>key_length ? M_NSV : key_length);
	for( ; k; --k ){
		/* non linear */
		st_sv[i] = (st_sv[i] ^ ((st_sv[i-1] ^ (st_sv[i-1] >> 30)) * 1664525UL)) + init_key[j] + j;

		/* for WORDSIZE > 32 machines */
		st_sv[i] &= 0xffffffffUL;

		++i; ++j;
		if( i>=M_NSV ) { st_sv[0] = st_sv[M_NSV-1]; i=1; }
		if( j>=key_length ) j=0;
	}
	for( k=M_NSV-1; k; --k ){
		/* non linear */
		st_sv[i] = (st_sv[i] ^ ((st_sv[i-1] ^ (st_sv[i-1] >> 30)) * 1566083941UL)) - i;

		/* for WORDSIZE > 32 machines */
		st_sv[i] &= 0xffffffffUL;

		++i;
		if( i>=M_NSV ){ st_sv[0] = st_sv[M_NSV-1]; i=1; }
	}

	/* MSB is 1; assuring non-zero initial array */ 
	st_sv[0] = 0x80000000UL;
}

/* generates a random number on [0,0xffffffff]-interval */
Znk_INLINE unsigned long
genrand_int32( void )
{
	unsigned long y;
	static const unsigned long mag01[2] = { 0x0UL, M_MATRIX_A };
	/* mag01[x] = x * M_MATRIX_A  for x=0,1 */
	
	if( st_sv_idx >= M_NSV ){ /* generate M_NSV words at one time */
		int kk;
		/* if init_genrand() has not been called, */
		if( st_sv_idx == M_NSV+1 ) init_genrand( 5489UL ); /* a default initial seed is used */
		
		for( kk=0; kk<M_NSV-M_M; ++kk ){
			y = ( st_sv[ kk      ]&M_UPPER_MASK ) | ( st_sv[ kk+1 ]&M_LOWER_MASK );
			st_sv[ kk      ] = st_sv[ kk + M_M         ] ^ (y>>1) ^ mag01[ y&0x1UL ];
		}
		for( ;kk<M_NSV-1; ++kk ){
			y = ( st_sv[ kk      ]&M_UPPER_MASK ) | ( st_sv[ kk+1 ]&M_LOWER_MASK );
			st_sv[ kk      ] = st_sv[ kk + (M_M-M_NSV) ] ^ (y>>1) ^ mag01[ y&0x1UL ];
		}
		/* last one */ {
			y = ( st_sv[ M_NSV-1 ]&M_UPPER_MASK ) | ( st_sv[ 0    ]&M_LOWER_MASK );
			st_sv[ M_NSV-1 ] = st_sv[ M_M-1            ] ^ (y>>1) ^ mag01[ y&0x1UL ];
		}
		
		st_sv_idx = 0;
	}
	
	y = st_sv[ st_sv_idx++ ];
	
	/* Tempering */
	y ^= (y >> 11);
	y ^= (y <<  7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	
	return y;
}

/* generates a random number on [0,1)-real-interval */
Znk_INLINE double genrand_real2(void)
{
	return genrand_int32()*(1.0/4294967296.0); /* divided by 2^32 */
}

static unsigned long st_last_time = 0;
static unsigned long st_counter   = 0;
static bool          st_is_first_shuffled = false;

static unsigned long
getMillsecTSeed( unsigned long seed_delta )
{
	unsigned long t = (unsigned long)time( NULL );

	if( t == st_last_time ){
#if defined(Znk_TARGET_WINDOWS)
		SYSTEMTIME st;
		GetSystemTime( &st );
		t += st.wMilliseconds;
#elif defined(Znk_TARGET_UNIX)
		struct timeval tv;
		gettimeofday( &tv, NULL );
		t = tv.tv_sec + tv.tv_usec;
#else
		t += st_counter;
		/***
		 * 毎回新しいプロセスでプログラムを実行する場合などではst_counterが固定化されてしまうため、
		 * プロセス毎に値が異なる可能性のある情報を使う.
		 */
		t += Znk_force_ptr_cast( unsigned long, getMillsecTSeed ) / sizeof(void*);
		t += clock();
#endif
	} else {
		st_last_time = t;
	}
	++st_counter;

	/***
	 * 必要ならseed_deltaでさらなる外部調整を行うこともできる.
	 */
	return t + seed_delta;
}

void
ZnkRand_shuffle( unsigned long seed_delta )
{
    unsigned long seed_ary[] = {0x123, 0x234, 0x345, 0x456};
	static unsigned long seed;
	size_t idx;
	seed = getMillsecTSeed( seed_delta );

	for(idx=0; idx<Znk_NARY(seed_ary); ++idx){
		seed_ary[ idx ] = seed % seed_ary[ idx ];
	}

    init_by_array( seed_ary, Znk_NARY(seed_ary) );

	st_is_first_shuffled = true;
}

unsigned long
ZnkRand_getRandUL( unsigned long num )
{
	double rand_val;
	unsigned long rand_i;

	if( !st_is_first_shuffled ){ ZnkRand_shuffle( 0 ); }
	if( num <= 1UL ){ return 0UL; }
	rand_val = genrand_real2();

	/***
	 * rand_valは[0,1)であるため、rand_iは必ずnum未満である.
	 */
	rand_i = ( unsigned long )( rand_val * num );
	return rand_i;
}

double
ZnkRand_getRandR( double min_real, double max_real )
{
	const double delta = max_real - min_real;
	if( !st_is_first_shuffled ){ ZnkRand_shuffle( 0 ); }
	return genrand_real2() * delta + min_real ;
}

