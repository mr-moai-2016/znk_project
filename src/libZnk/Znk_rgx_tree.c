#include "Znk_rgx_tree.h"
#include <Znk_base.h>
#include <Znk_stdc.h>
#include <Znk_str.h>
#include <stdlib.h>

#define DEBUG   1


/**
 * 正規表現をパースして構文木を生成する.
 * 次の構文で定義される正規表現を受理する.
 *   <regexp>        ::= <term> | <term> "|" <regexp>
 *   <term>          ::= EMPTY  | <factor> <term>
 *   <factor>        ::= <primary> | <primary> "*" | <primary> "+"
 *   <primary>       ::=  CHARACTER | "(" <regexp> ")"
 */

/* token_tはトークンを表す型 */
typedef enum {
	tk_char,  /* 文字（実際の文字はst_token_charにセットされる） */
	tk_union, /* '|' */
	tk_lpar,  /* '(' */
	tk_rpar,  /* ')' */
	tk_star,  /* '*' */
	tk_plus,  /* '+' */
	tk_dot,   /* '.' */
	tk_end    /* 文字列の終わり */
} token_t;


typedef struct Context_tag {
	/* 現在処理中のトークン */
	token_t current_token_;
	
	/* current_token_がtk_charであるときの文字値 */
	ZnkUChar64 token_char_;
	
	/* 解析する文字列へのポインタ */
	const char* strbuff_;

	ZnkUChar64FuncT_get func_get_;

	/* エラー/リポート格納用文字列 */
	ZnkStr rep_msg_;
} Context;

static void
addMsgf( Context* ctx, const char* fmt, ... )
{
	if( ctx->rep_msg_ ){
		va_list ap;
		va_start(ap, fmt);
		ZnkStr_vsnprintf2( ctx->rep_msg_, Znk_NPOS, Znk_NPOS, fmt, ap );
		va_end(ap);
	}
}

#define SYNTAX_ERROR( ctx, msg ) addMsgf( ctx, "ZnkRgxTree : Syntax Error : " msg "\n" )

static size_t st_allocated_node_count = 0;
static ZnkRgxTree* allocTree( void )
{
	++st_allocated_node_count;
	return (ZnkRgxTree*) Znk_alloc0( sizeof(ZnkRgxTree) );
}
static void freeTree( ZnkRgxTree* tree )
{
	--st_allocated_node_count;
	Znk_free( tree );
}
size_t
ZnkRgxTree_getAllocatedNodeCount( void )
{
	return st_allocated_node_count;
}


/* トークンを1つ読み込んで返す */
static token_t get_token( Context* ctx )
{
	ZnkUChar64 uc = { 0 };
	
	/* 文字列の終わりに到達したらtk_endを返す */
	if( *(ctx->strbuff_) == '\0' ){
		ctx->current_token_ = tk_end;
	} else {
		/* 文字をトークンに変換する */
		ctx->strbuff_ = (const char*)( (*ctx->func_get_)( &uc, (const uint8_t*)ctx->strbuff_ ) );
		switch( uc.u64_ ){
		case '|': ctx->current_token_ = tk_union; break;
		case '(': ctx->current_token_ = tk_lpar;  break;
		case ')': ctx->current_token_ = tk_rpar;  break;
		case '*': ctx->current_token_ = tk_star;  break;
		case '+': ctx->current_token_ = tk_plus;  break;
		case '.':
			ctx->current_token_ = tk_dot;
			ctx->token_char_.u64_ = ZnkRgx_DOT;
			break;
		default:
			ctx->current_token_ = tk_char;
			ctx->token_char_ = uc;
			break;
		}
	}
	return ctx->current_token_;
}

/* 正規表現を解析するパーサを初期化する */
static void
initialize_regexp_parser( Context* ctx, const char *str, ZnkUChar64FuncT_get func_get )
{
	ctx->strbuff_  = str;
	ctx->func_get_ = func_get;
	get_token( ctx );
}

/* 構文木のノードを作成する。
   opはノードが表す演算、leftは左の子、rightは右の子 */
static ZnkRgxTree*
make_tree_node( ZnkRgxOP_t op, ZnkRgxTree* left, ZnkRgxTree* right )
{
	ZnkRgxTree* p;

	/* ノードを割り当てる */
	p = allocTree();
	/* ノードに情報を設定する */
	p->op = op;
	p->u.x.left_  = left;
	p->u.x.right_ = right;
	return p;
}

/* 構文木の葉を作る。
   cはこの葉が表す文字 */
static ZnkRgxTree*
make_atom( ZnkUChar64 c )
{
	ZnkRgxTree* p;

	/* 葉を割り当てる */
	p = allocTree();
	/* 葉に情報を設定する */
	p->op = ZnkRgxOP_char;
	p->u.c_ = c;
	return p;
}
static ZnkRgxTree*
make_dot( void )
{
	ZnkRgxTree* p;

	/* 葉を割り当てる */
	p = allocTree();
	/* 葉に情報を設定する */
	p->op = ZnkRgxOP_dot;
	p->u.c_.u64_ = ZnkRgx_DOT;
	return p;
}

static ZnkRgxTree* term( Context* ctx );
static ZnkRgxTree* factor( Context* ctx );
static ZnkRgxTree* primary( Context* ctx );

/* <regexp>をパースして、得られた構文木を返す。
   選択X|Yを解析する */
static ZnkRgxTree* regexp( Context* ctx )
{
	ZnkRgxTree* x = term( ctx );
	while( ctx->current_token_ == tk_union ){
		get_token( ctx );
		x = make_tree_node(ZnkRgxOP_union, x, term( ctx ));
	}
	return x;
}

/* <term>をパースして、得られた構文木を返す。
   連結XYを解析する */
static ZnkRgxTree*
term( Context* ctx )
{
	ZnkRgxTree* x;
	
	switch( ctx->current_token_ ){
	case tk_union:
	case tk_rpar:
	case tk_end:
		x = make_tree_node(ZnkRgxOP_empty, NULL, NULL);
		break;
	default:
		x = factor( ctx );
		while( ctx->current_token_ != tk_union &&
		       ctx->current_token_ != tk_rpar &&
		       ctx->current_token_ != tk_end )
		{
			x = make_tree_node(ZnkRgxOP_concat, x, factor( ctx ));
		}
		break;
	}
	return x;
}

/* <factor>をパースして、得られた構文木を返す。
   繰り返しX*、X+を解析する */
static ZnkRgxTree* factor( Context* ctx )
{
	ZnkRgxTree* p;
	ZnkRgxTree* x = NULL;
	
	p = primary( ctx );
	if( ctx->current_token_ == tk_star ){
		x = make_tree_node(ZnkRgxOP_closure, p, NULL);
		get_token( ctx );
	} else if ( ctx->current_token_ == tk_plus ) {
		/* ここのみ p を共有参照している.
		 * したがって解放時に工夫が必要. */
		x = make_tree_node(ZnkRgxOP_concat, p, make_tree_node(ZnkRgxOP_closure, p, NULL));
		get_token( ctx );
	} else {
		x = p;
	}
	return x;
}

/* <primary>をパースして、得られた構文木を返す。
   文字そのもの、(X)を解析する */
static ZnkRgxTree*
primary( Context* ctx )
{
	ZnkRgxTree* x = NULL;
	
	if (ctx->current_token_ == tk_char) {
		x = make_atom(ctx->token_char_);
		get_token( ctx );
	} else if (ctx->current_token_ == tk_dot) {
		x = make_dot();
		get_token( ctx );
	} else if (ctx->current_token_ == tk_lpar) {
		get_token( ctx );
		x = regexp( ctx );
		if (ctx->current_token_ != tk_rpar){
			SYNTAX_ERROR( ctx, "primary : Close paren is expected." );
			return NULL;
		}
		get_token( ctx );
	} else {
		if( ctx->rep_msg_ ){
			SYNTAX_ERROR( ctx, "primary : Normal character or open paren is expected." );
		}
	}
	return x;
}

#if     DEBUG
/* 構文木を表示する（デバッグ用） */
static bool
dumpTree( Context* ctx, ZnkRgxTree* p )
{
	switch (p->op) {
	case ZnkRgxOP_char:
		addMsgf( ctx, "\"%c\"", p->u.c_);
		break;
	case ZnkRgxOP_dot:
		addMsgf( ctx, "\"[%x]\"", p->u.c_);
		break;
	case ZnkRgxOP_concat:
		addMsgf( ctx, "(concat ");
		if( !dumpTree(ctx, p->u.x.left_) ){ return false; }
		addMsgf( ctx, " ");
		if( !dumpTree(ctx, p->u.x.right_) ){ return false; }
		addMsgf( ctx, ")");
		break;
	case ZnkRgxOP_union:
		addMsgf( ctx, "(or ");
		if( !dumpTree(ctx, p->u.x.left_) ){ return false; }
		addMsgf( ctx, " ");
		if( !dumpTree(ctx, p->u.x.right_) ){ return false; }
		addMsgf( ctx, ")");
		break;
	case ZnkRgxOP_closure:
		addMsgf( ctx, "(closure ");
		if( !dumpTree(ctx, p->u.x.left_) ){ return false; }
		addMsgf( ctx, ")");
		break;
	case ZnkRgxOP_empty:
		addMsgf( ctx, "ZnkRgx_EMPTY");
		break;
	default:
		addMsgf( ctx, "ZnkRgxTree : Internal Error : dumpTree : This cannot happen.\n");
		return false;
	}
	return true;
}
#endif  /* DEBUG */


/***
 * 正規表現をパースして、正規表現に対応する構文木を返す。
 * strは正規表現が入っている文字列.
 */
ZnkRgxTree*
ZnkRgxTree_create( const char* str, ZnkUChar64FuncT_get func_get, ZnkStr rep_msg )
{
	ZnkRgxTree* t = NULL;
	Context ctx = { 0 };
	ctx.rep_msg_ = rep_msg;
	
	/* パーサを初期化する */
	initialize_regexp_parser( &ctx, str, func_get );
	
	/* 正規表現をパースする */
	t = regexp( &ctx );
	
	/* 次のトークンがtk_endでなければエラー */
	if( ctx.current_token_ != tk_end ){
		SYNTAX_ERROR( &ctx, "create : Extra character at end of pattern." );
		goto FUNC_ERROR;
	}

#if     DEBUG
	/* 得られた構文木の内容を表示する */
	if( !dumpTree( &ctx, t ) ){
		goto FUNC_ERROR;
	}
	addMsgf( &ctx, "\n");
#endif  /* DEBUG */

	/* 生成した構文木を返す */
	return t;

FUNC_ERROR:
	ZnkRgxTree_destroy( t, ctx.rep_msg_ );
	return NULL;
}

void
ZnkRgxTree_destroy( ZnkRgxTree* tree, ZnkStr rep_msg )
{
	if( tree ){

    	if( tree->op == ZnkRgxOP_concat ){
			ZnkRgxTree* left  = tree->u.x.left_;
			ZnkRgxTree* right = tree->u.x.right_;
			if( right ){
				if( right->op == ZnkRgxOP_closure ){
					/* plus closureの場合、left == right->leftが共有参照となっていることがあり得る. */
					if( left == right->u.x.left_ ){
						if( rep_msg ){
							ZnkStr_add( rep_msg, "ZnkRgxTree_destroy : Report : sharing reference of plus_closure detect.\n" );
						}
						ZnkRgxTree_destroy( left, rep_msg );
						/* right->leftは既に解放されており、right->rightはこの場合必ずNULLである.
						 * したがって単にrightを単独freeしなければならない. */
						freeTree( right );

						freeTree( tree );
						return;
					}
				}
			}
		}

		if( tree->op != ZnkRgxOP_char && tree->op != ZnkRgxOP_dot ){
			if( tree->u.x.left_ ){
				ZnkRgxTree_destroy( tree->u.x.left_, rep_msg );
			}
			if( tree->u.x.right_ ){
				ZnkRgxTree_destroy( tree->u.x.right_, rep_msg );
			}
		}
		freeTree( tree );
	}
}
