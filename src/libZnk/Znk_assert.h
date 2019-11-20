#ifndef INCLUDE_GUARD__Znk_assert_h__
#define INCLUDE_GUARD__Znk_assert_h__

#include <Znk_stdc.h>
#include <stdlib.h> /* for abort */
#include <assert.h>

Znk_EXTERN_C_BEGIN

#define Znk_none /* none */ 

#ifdef NDEBUG
#  define Znk_assert( cond, stmt ) 
#else
#  define Znk_assert( cond, stmt ) do{ \
	if( !(cond) ){ \
		Znk_printf( "Assertion failed: %s, file %s, line %d\n", #cond, __FILE__, __LINE__ ); \
		stmt; \
		abort(); \
	} \
} while(0)
#endif

/***
 * @exam:
 *   // obj must have next_ field at first. 
 *
 *   #define M_CAST_NODE( Node, obj ) \
 *     (Node)( obj + Znk_assert_static_0( offsetof( obj, next_ ) == 0 ) )
 *
 *   #define M_IS_NODE( obj ) \
 *   { \
 *     Znk_assert_static( offsetof( obj, next_ ) == 0 ); \
 *     ... \
 *   }
 *
 * @note:
 *   Thanks to Rusty Russell( https://github.com/rustyrussell/ccan )
 */
#define Znk_assert_static_0( cond ) ( sizeof( char [1 - 2*!(cond)] ) - 1 )

#define Znk_assert_static( cond ) (void)Znk_assert_static_0( cond )

#define Znk_panic( stmt ) do{ \
	Znk_printf( "Panic: file %s, line %d\n", __FILE__, __LINE__ ); \
	stmt; \
	abort(); \
} while(0)

#define Znk_unreachable( stmt ) do{ \
	Znk_printf( "Unreachable : file %s, line %d\n", __FILE__, __LINE__ ); \
	stmt; \
	abort(); \
} while(0)

#define Znk_unreachable_kind( kind ) \
	Znk_unreachable( Znk_printf( #kind "=[%d]\n", kind ) )

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
