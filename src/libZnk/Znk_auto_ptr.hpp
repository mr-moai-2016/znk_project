#ifndef INCLUDE_GUARD__Znk_auto_ptr_hpp__
#define INCLUDE_GUARD__Znk_auto_ptr_hpp__

/* This header is for C++ only. */

#include <Znk_base.h>

/**
 * @brief
 *   deleter関数/ファンクタを自由に設定可能なauto_ptr.
 *   以下はよくあるポインタ型オブジェクトとdeleter関数/ファンクタをユーザ指定することにより
 *   これを自動的に破棄するようにしたものである.
 *
 * @exam
 *   ...
 *   test(){
 *     // MyObj は Data* のように * が付加されているものでもよいし、
 *     // typedef Data* MyData のように typedefされたものでもよい.
 *     Znk_AutoPtr<MyObj> obj_ap( MyObj_create("MyName",...), MyObj_destroy );
 *     MyObj obj = obj_ap.get();
 *
 *     ... obj(MyObj型)を使用する処理 ...
 *
 *     // 例外発生時または関数の終了で obj は MyObj_destroy( obj ) の自動呼び出しにより
 *     // 解放される.
 *   }
 *
 * @note
 *   既に数多くのスマートポインタが世に存在する中、敢えてこれを導入した背景を一応述べておく.
 *
 *   最初に言っておくが、我々は特に大規模開発においてこの種のスマートポインタの使用には
 *   (このZnk_AutoPtrも含め)否定的である.
 *
 *   さらに最も良い選択はC++を使わないことだが、実際そうもいかない. そしてC++を使う以上、
 *   いつどこで例外が発生するかは殆ど予測不能であり、その場合動的確保したポインタ
 *   オブジェクトをこの種のクラスでラップしておかないと例外発生時にリークしてしまう.
 *   仕方がないのでこのlibZnkでも渋々このようなクラスを提供した. 
 *
 *   この種のクラスを使う主たる動機はぶっちゃげそれだけであって、よく言われる最後の
 *   解放関数呼び出しを行わなくても済むので楽だとか忘れても大丈夫なので安心などという
 *   宣伝文句は、実際のところ巷で騒ぐほどの労力の節約でもないし別に安心でもなんでもない.
 *   そんなことは、既存のスマートポインタに存在するもっと邪悪な問題と比べれば微々たる
 *   事項である. では面倒だがこれについても一応ざっくりと説明しておく.
 *
 *   std::auto_ptr ではdelete用関数/ファンクタなどが独自に設定できない( operator deleteを
 *   オーバーライドして強引に設定することはできるが、グローバルなoperator deleteを上書き
 *   すべきではない. 仮にそうするとしても色々と憂慮する事項を増やすだけである. コンパイラ
 *   毎に存在する癖の問題もある).
 *
 *   boost::shared_ptrでは当然boostを導入しなければならないが、それを嫌ってboostから
 *   その実装部分のみを抽出することも実は可能なのだが、Windows/Linuxだけに環境を絞っても
 *   最低5,6ファイルほど必要になり、これはこの機能だけを実現するファイル数としては少々多い.
 *   しかもそれらはすべてヘッダである. 特にマルチスレッドにおける代入処理に対応するための
 *   lock/unlockに関する実装( boost/smart_ptr/detail/spinlock.hpp を参照 )やatomic operation
 *   に関する処理が隠蔽されず、一応namespaceで囲ってあるとは言え、グローバル領域にばら撒かれ
 *   shared_ptrを利用するすべてのコードからこれらの実装の詳細が見えている状況である.
 *
 *   実装は極力隠蔽すべきである. それが複雑で上記のような処理系依存なものならば尚更である.
 *   複雑過ぎる実装が表に見えているということは作者達ですら予期しない未知の不具合の
 *   混入するリスクをそれだけ高めるということでもある. 特にこの種のインフラ的なクラスで
 *   このリスクが高いのは無視できない不安要素であり、その導入にはそれだけに慎重でなければ
 *   ならない. 例えば boost1.38以前では bcc 5.5.1 において以下の典型的なコードが実際に
 *   コンパイルエラーとなる問題があった.
 *
 *   if( sp && flag ){ ...spを使う処理... } // ここで sp は shared_ptr, flag は bool 型
 *
 *   spの替わりにsp.get() と書けばよいのだが、その他のコンパイラでは上記が何のエラーもなく
 *   コンパイルできてしまうのが実のところさらに厄介である. boostの作者達のような優秀な
 *   プログラマ達でさえ、長い間この問題に気づくことができなかったのもそのためである.
 *
 *   少し詳しく書けばこの問題はboost::shared_ptr内において unspecified_bool_type という
 *   ややトリッキーな仕掛けを導入していることが元凶であった. これを削除すればよいと思わ
 *   れるかもしれないが、そうすると今度は別の環境において問題を誘発する.
 *   尚 1.39以降では以下の型変換演算子オーバライドが追加され、これに対する対策がなされている.
 *
 *   operator bool () const { return px != 0; }
 *
 *   仮に楽観的な見方をし、予期しない未知の不具合はもう存在しないと仮定しても、クラスの
 *   持つある種の癖とその使い方の相性が合わず、ローカルな問題が偶発するというのはよくある
 *   話である. このような時、その原因を追跡するのは他ならぬそれを使う我々である. ましてや
 *   一見してコード上に現れない多くの自動処理が裏で行われるスマートポインタのようなクラス
 *   の場合、その作業は幾何級数的に困難になり、尚更に邪悪な事態へと容易に陥る.
 *
 *   さらにその他のreference counter型GC全般に言えることであるがdeleterが起動するタイミング
 *   の予測が難しいといった問題もある. ある種のリアルタイム系プログラムの開発においては
 *   これが問題になることがある. というわけで、ここではこの種のGC型スマートポインタも
 *   基本的に用いない. ポインタオブジェクトの寿命が関数単位での管理スキームを逸脱し、
 *   これを準グローバルに延長する必要があるケースは実際にはそうはない. 仮にそのようなことが
 *   一見して必要に思える場合でも大抵はその設計が甘いことが殆どであり、まずそれを見直す
 *   べきである.
 *
 *   過度な機能をたった一つのもので提供することは、それが持つ複雑性がために結局はその
 *   メリット以上の皺寄せが後で押し寄せるというのは当たり前の話である. どんなライブラリ
 *   であれ、クラスであれこの種の問題は存在するが、その実装が複雑であるほどその作業は
 *   困難になる. その中にあって、これをできるだけ緩和するにはその入り口においてその複雑性
 *   を極力排除すべく最初に制限やエラーチェックを設けて防衛するか、さもなくばその複雑性を
 *   一箇所に集中させず分散させた上で隠蔽することが重要となる.
 *
 *   Znk_AutoPtr は、そこらのプロジェクトで良く見られる説明の必要もないくらいシンプルな
 *   実装である. std::auto_ptr のようなオーナーという概念はなく、余計な処理は悉くサポート
 *   しない. Znk_AutoPtrどうしの代入処理すら完全禁止である. 関数の最初に宣言して初期化する
 *   だけという用途と割り切る. 
 *
 *   これによる対価は当然その実装がこれ以上ないほどにシンプルとなることである. これにより
 *   バグの可能性を心配する必要はまずないし、仮に問題発生しても追跡は容易である. とはいえ
 *   ここまでシンプルでも尚、デストラクタの自動呼出しにより、デバッガで追跡し難い状況などが
 *   起こり得る. 伝統的な C のように最後に明示的に解放関数を呼ぶようにするスタイルがコード
 *   記述時には面倒なようでも、いざ問題が発生して追跡する段になった場合にはやはり最も確実
 *   であり問題解決を迅速に導く. だからこそ大規模な開発には逆にこの種のラッパーを使う選択に
 *   我々は否定的である.
 */
template< typename PtrT >
class Znk_AutoPtr{
public:
	typedef void (*DelT)( PtrT ptr );
	Znk_AutoPtr( PtrT ptr, DelT del )
		: m_ptr( ptr )
		, m_del( del )
	{}
	~Znk_AutoPtr(){
		m_del( m_ptr );
	}
	PtrT get() const // never throws
	{
		return m_ptr;
	}
private:
	PtrT m_ptr;
	DelT m_del;
	/* 代入は全面禁止 */
	Znk_AutoPtr( const Znk_AutoPtr& ); Znk_AutoPtr& operator=( const Znk_AutoPtr& );
};

/**
 * @brief
 *   引数を二つとり、第1引数で解放すべきポインタ、第2引数に任意の型(ただし生成時に値が確定
 *   されるもの)をとるタイプのdeleter関数/ファンクタを自由に設定可能なZnk_AutoPtr.
 *
 *   メインとなるポインタ型を第1引数にとり、第2引数には補助となる値を指定するタイプの
 *   解放関数が使用されるケースが多々ある.
 *   これは、そのようなタイプのオブジェクト解放関数に対応したZnk_AutoPtrである.
 *
 * @exam
 *   登録するdeleterは、戻り値がなく、第1引数には解放すべきポインタ型、第2引数には
 *   任意の型の引数を受け取って解放するタイプであればなんでもよい. 関数終了時に自動で
 *   これを呼び出し、ポインタを解放する.
 *
 *   test1(){
 *     Znk_AutoPtrArg<Obj> obj_ap( Obj_create_null(), Obj_destroy, true );
 *     Obj obj = obj_ap.get();
 *
 *     ... obj(Obj型)を使用する処理 ...
 *
 *     // 関数の終了で obj はObj_destroy( obj, true ) という呼び出しにより自動的に解放される.
 *     // AutoPtrArgのコンストラクタの第3引数で指定した値が、そのまま del で指定した deleterの
 *     // 第2引数として使われる.
 *   }
 *
 * @note
 *   基本的には Znk_AutoPtr と同じ注意点がある.
 */
template< typename PtrT, typename ArgT >
class Znk_AutoPtrArg{
public:
	typedef void (*DelT)( PtrT ptr, ArgT arg );
	Znk_AutoPtrArg( PtrT ptr, DelT del, ArgT arg )
		: m_ptr( ptr )
		, m_del( del )
		, m_arg( arg )
	{}
	~Znk_AutoPtrArg(){
		m_del( m_ptr, m_arg );
	}
	PtrT get() const // never throws
	{
		return m_ptr;
	}
	ArgT arg() const // never throws
	{
		return m_arg;
	}
private:
	PtrT m_ptr;
	DelT m_del;
	ArgT m_arg;
	/* 代入は全面禁止 */
	Znk_AutoPtrArg( const Znk_AutoPtrArg& ); Znk_AutoPtrArg& operator=( const Znk_AutoPtrArg& );
};


/**
 * @brief
 *   PtrT型を保持するAutoPtr または AutoPtrArgと同時にnameという名前のPtrT型ポインタ変数を
 *   宣言し、さらに変数nameを、このAutoPtrのgetより取得した値で初期化する.
 *   これは、AutoPtr/AutoPtrArgでよく使用されるパターンをマクロ化したものである.
 */
#define Znk_AUTO_PTR( PtrT, name, ptr, del ) \
	Znk_AutoPtr< PtrT > name##__ap__( ptr, del ); \
	PtrT name = name##__ap__.get()

#define Znk_AUTO_PTR_ARG( PtrT, name, ptr, del, ArgT, arg ) \
	Znk_AutoPtrArg< PtrT, ArgT > name##__ap__( ptr, del, arg ); \
	PtrT name = name##__ap__.get()

#endif /* INCLUDE_GUARD */
