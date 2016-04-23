#include "Znk_err.h"
#include "Znk_mutex.h"
#include "Znk_str.h"
#include <stdarg.h>

#define M_getZnkSAtomPr( err ) &((err).pr__)

Znk_THE_MUTEX( theMutexPtr, theMutex )

static ZnkSAtomDB* theErrAtomDBPtr( void )
{
	static ZnkSAtomDB st_atomdb = NULL;
	return &st_atomdb;
}
Znk_INLINE ZnkSAtomDB theErrAtomDB( void )
{
	/***
	 * この関数は必ず上位の関数のlock/unlockの中で呼ばれるものであると定める.
	 * 従って、逆に言えばこの関数の中でさらにlock/unlockを使用してはならない.
	 * 二重ロックとなってフリーズしてしまう.
	 * (lock/unlockを全くかけていない状態でこの関数を呼ぶとスレッドセーフでは
	 * なくなるが、一方でこれをネストして二重にかけると、今度は二重ロックにより
	 * デッドロック状態に陥る)
	 */
	ZnkSAtomDB* the_err_atomdb = theErrAtomDBPtr();
	if( *the_err_atomdb == NULL ){
		*the_err_atomdb = ZnkSAtomDB_create();
	}
	return *the_err_atomdb;
}


static ZnkStr st_zkbfr = NULL;

static ZnkStr theBuf( void )
{
	if( st_zkbfr == NULL ){
		st_zkbfr = ZnkStr_new( "" );
	}
	return st_zkbfr;
}


void
ZnkErr_intern( ZnkErr* ans_err, const char* err_msg )
{
	ZnkErr_D( err );
	ZnkMutex mtx = theMutex();
	ZnkMutex_lock( mtx );
	{
		ZnkSAtomDB the_err_atomdb = theErrAtomDB();
		ZnkSAtomPr_intern( M_getZnkSAtomPr(err), the_err_atomdb, err_msg );
	}
	ZnkMutex_unlock( mtx );
	if( ans_err ){ *ans_err = err; }
}
void
ZnkErr_internf( ZnkErr* ans_err, const char* fmt, ... )
{
	ZnkErr_D( err );
	ZnkMutex mtx = theMutex();
	ZnkMutex_lock( mtx );
	{
		ZnkStr the_buf = theBuf();
		ZnkSAtomDB the_err_atomdb = theErrAtomDB();
		va_list ap;
		va_start(ap, fmt);
		ZnkStr_vsnprintf( the_buf, 0, Znk_NPOS, fmt, ap );
		va_end(ap);
		ZnkSAtomPr_intern( M_getZnkSAtomPr(err), the_err_atomdb, ZnkStr_cstr(the_buf) );
	}
	ZnkMutex_unlock( mtx );
	if( ans_err ){ *ans_err = err; }
}
const char*
ZnkErr_cstr( const ZnkErr c1err )
{
	const char* ans = NULL;
	const ZnkSAtomPr* atompr;
	ZnkMutex mtx = theMutex();
	ZnkMutex_lock( mtx );
	atompr = M_getZnkSAtomPr( c1err );
	ans = atompr->db_ ? ZnkSAtomPr_cstr( atompr ) : "";
	ZnkMutex_unlock( mtx );
	return ans;
}

void
ZnkErr_initiate( void )
{
	theMutex();
	theErrAtomDB();
}
void
ZnkErr_finalize( void )
{
	/* thread unsafe */
	ZnkMutex*  mtx_ptr = theMutexPtr();
	ZnkSAtomDB* the_err_atomdb = theErrAtomDBPtr();
	if( *the_err_atomdb != NULL ){
		ZnkSAtomDB_destroy( *the_err_atomdb );
	}
	if( *mtx_ptr != NULL ){
		ZnkMutex_destroy( *mtx_ptr );
	}
	if( st_zkbfr ){
		ZnkStr_destroy( st_zkbfr );
		st_zkbfr = NULL;
	}
}

