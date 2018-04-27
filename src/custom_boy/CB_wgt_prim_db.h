#ifndef INCLUDE_GUARD__CB_wgt_prim_db_h__
#define INCLUDE_GUARD__CB_wgt_prim_db_h__

#include <CB_wgt_prim.h>
#include <Znk_duple_ary.h>
#include <Znk_bird.h>

/**
 * Simple databese of Weighted Primitive for CustomBoy
 * Author : Zenkaku
 * The Original Proposer : K.Yakisoba.H
 */

Znk_EXTERN_C_BEGIN

Znk_DECLARE_HANDLE( CBWgtPrimDB );

CBWgtPrimDB
CBWgtPrimDB_create( void );

void
CBWgtPrimDB_destroy( CBWgtPrimDB wpdb );

CBWgtPrimAry
CBWgtPrimDB_regist( CBWgtPrimDB wpdb, const char* key );

size_t
CBWgtPrimDB_size( const CBWgtPrimDB wpdb );

CBWgtPrimAry
CBWgtPrimDB_atAry( CBWgtPrimDB wpdb, size_t idx );

ZnkDuple
CBWgtPrimDB_atDuple( CBWgtPrimDB wpdb, size_t idx );

CBWgtPrimAry
CBWgtPrimDB_findAry( CBWgtPrimDB wpdb, const char* key );

bool
CBWgtPrimDB_load( CBWgtPrimDB wpdb, const char* conffile );

bool
CBWgtPrimDB_exec( CBWgtPrimDB wpdb, ZnkBird bird, ZnkStr ans, const char* entry_key );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
