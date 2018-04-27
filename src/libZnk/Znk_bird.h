#ifndef INCLUDE_GUARD__Znk_bird_h__
#define INCLUDE_GUARD__Znk_bird_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkBirdImpl* ZnkBird;

typedef bool (*ZnkBirdProcessFunc)( const ZnkBird info,
		ZnkStr dst,
		const char* src,
		const char* key,
		size_t begin_idx, /* src上におけるkeyの開始位置をhintとして与える */
		void* func_arg );

ZnkBird
ZnkBird_create( const char* key_beginer, const char* key_ender );

void
ZnkBird_destroy( ZnkBird info );

void
ZnkBird_regist( ZnkBird info, const char* key, const char* val );

void
ZnkBird_setFunc( ZnkBird info, ZnkBirdProcessFunc func, void* func_arg );

const char*
ZnkBird_at( ZnkBird info, const char* key );

const bool
ZnkBird_exist( ZnkBird info, const char* key );

/**
 * @brief
 * registにおいて登録したkeyとvalを全クリア.
 */
void
ZnkBird_clear( ZnkBird info );

/***
 * #[key]# Directiveが埋め込まれた文字列を展開する.
 *
 * この処理はsrcが頻繁に変更されるような処理に向いている.
 * 一方で、srcは初期化後固定であり、info内のhashのvalの値(keyは不変)が頻繁に変わるような処理の場合、
 * この関数でも一応対応できるが、前処理によりもっと高速な実装を考えることができる.
 * これはMsgObjというモジュールで将来実装予定.
 *
 * 尚、dstとsrcは同一のZnkStrであってはならない.
 */
bool ZnkBird_extend( const ZnkBird info,
		ZnkStr dst, const char* src, size_t expected_size );

bool ZnkBird_extend_self( const ZnkBird info, ZnkStr src, size_t expected_size );


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
