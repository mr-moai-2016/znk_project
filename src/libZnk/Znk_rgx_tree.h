#ifndef INCLUDE_GUARD__Znk_rgx_tree_h__
#define INCLUDE_GUARD__Znk_rgx_tree_h__

#include <Znk_rgx_base.h>
#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

/* ZnkRgxOP_tはノードの操作を表す型 */
typedef enum {
	ZnkRgxOP_char,            /* 文字そのもの  */
	ZnkRgxOP_concat,          /* XY */
	ZnkRgxOP_union,           /* X|Y */
	ZnkRgxOP_closure,         /* X* */
	ZnkRgxOP_dot,             /* dot */
	ZnkRgxOP_empty            /* 空 */
} ZnkRgxOP_t;

typedef const uint8_t* (*ZnkIncrementU8PFuncT)( const uint8_t* );
typedef const uint8_t* (*ZnkUChar64FuncT_get)( ZnkUChar64* uc, const uint8_t* p );

/* ZnkRgxTreeは構文木のノードを表す型 */
typedef struct ZnkRgxTree_tag {
	ZnkRgxOP_t op; /* このノードの操作 */
	union {
		ZnkUChar64 c_; /* op == ZnkRgxOP_char ZnkRgxOP_dot のとき: 文字 */
		struct { /* op == ZnkRgxOP_char ZnkRgxOP_dot 以外のとき: */
			struct ZnkRgxTree_tag* left_;  /* 左の子 */
			struct ZnkRgxTree_tag* right_; /* 右の子 */
		} x;
	} u;
} ZnkRgxTree;

size_t
ZnkRgxTree_getAllocatedNodeCount( void );

ZnkRgxTree*
ZnkRgxTree_create( const char* str, ZnkUChar64FuncT_get func_get, ZnkStr rep_msg );

void
ZnkRgxTree_destroy( ZnkRgxTree* tree, ZnkStr rep_msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */

