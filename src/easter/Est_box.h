#ifndef INCLUDE_GUARD__Est_box_h__
#define INCLUDE_GUARD__Est_box_h__

#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

size_t
EstBox_remove( ZnkVarpAry post_vars, ZnkStr msg );

size_t
EstBox_dustize( ZnkVarpAry post_vars, ZnkStr msg );

size_t
EstBox_favoritize( ZnkVarpAry post_vars, ZnkStr msg, const char* box_fsys_dir, const char* box_vname, ZnkStrAry dst_vpath_list );

size_t
EstBox_stock( ZnkVarpAry post_vars, ZnkStr msg, const char* box_fsys_dir, const char* box_vname, ZnkStrAry dst_vpath_list );

size_t
EstBox_recentize( ZnkVarpAry post_vars, ZnkStr msg );

void
EstBox_makeFInfList( const char* box_fsys_dir, const char* box_vname );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
