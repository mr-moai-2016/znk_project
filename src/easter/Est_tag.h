#ifndef INCLUDE_GUARD__Est_tag_h__
#define INCLUDE_GUARD__Est_tag_h__

#include <Znk_myf.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

bool
EstTag_registNew( ZnkMyf tags_myf, const char* tag, ZnkStr tagid, ZnkStr msg, const char* select_group );
bool
EstTag_remove( ZnkMyf tags_myf, const char* tag, ZnkStr msg );

bool
EstTag_registNewGroup( ZnkMyf tags_myf, const char* group, ZnkStr msg, ZnkStrAry category_id_list );
bool
EstTag_removeGroup( ZnkMyf tags_myf, const char* query_category_key, ZnkStr msg );

bool
EstTag_registNewCategory( ZnkMyf tags_myf, const char* category_name, ZnkStr msg );
bool
EstTag_removeCategory( ZnkMyf tags_myf, const char* query_category_key, ZnkStr msg );

void
EstTag_joinTagIDs( ZnkStr ans, ZnkVarpAry post_vars, char connector_ch );

const char*
EstTag_getCategoryKey( ZnkMyf tags_myf, size_t idx );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
