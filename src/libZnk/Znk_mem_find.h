#ifndef INCLUDE_GUARD__Znk_mem_find_h__
#define INCLUDE_GUARD__Znk_mem_find_h__

#include <Znk_base.h>

Znk_EXTERN_C_BEGIN


/*****************************************************************************/
/* lfind/rfind */

/**
 * @brief
 *   bufを 16/32/64bitデータnum個からなる配列とする.
 *   valに等しい最初の要素数のインデックスを返す.
 *   見つからなかった場合はZnk_NPOSを返す.
 *
 * @note
 *   uint8_t* のポインタをキャストしてこれらの関数に渡す場合、そのアドレス値は
 *   バイト境界になければならない. 例えば uint8_t* ptr を (const uint16_t*)ptr など
 *   として、ZnkAry_lfind_16へ渡す場合、ptr のアドレス値は２の倍数でなければならない.
 *   さもなければ、処理系によっては期待通りの動作をするかもしれない（しかしその場合
 *   でも期待通りの速度を持つ機械語に翻訳されるとは限らない)し、クラッシュするかも
 *   しれない(要するに未定義の動作となる). 
 */
size_t ZnkAry_lfind_16( const uint16_t* buf, uint16_t val, size_t num, size_t delta );
size_t ZnkAry_rfind_16( const uint16_t* buf, uint16_t val, size_t num, size_t delta );
size_t ZnkAry_lfind_32( const uint32_t* buf, uint32_t val, size_t num, size_t delta );
size_t ZnkAry_rfind_32( const uint32_t* buf, uint32_t val, size_t num, size_t delta );
size_t ZnkAry_lfind_64( const uint64_t* buf, uint64_t val, size_t num, size_t delta );
size_t ZnkAry_rfind_64( const uint64_t* buf, uint64_t val, size_t num, size_t delta );
size_t ZnkAry_lfind_sz( const size_t*   buf, size_t   val, size_t num, size_t delta );
size_t ZnkAry_rfind_sz( const size_t*   buf, size_t   val, size_t num, size_t delta );


/**
 * @brief
 *   8/16/24/32/64bit画素配列がsizeバイトの範囲内でpaddingなしにシリアライズされた
 *   イメージ(buf)から、valに等しい最初の画素のインデックスを返す.
 *   (ただし24bitにおいては、valの下位24bitに等しいものを探す. valの上位8bitにある
 *   bit値は単に無視される. 厳密な仕様についてはparam is_swapの説明を参照).
 *   ここで言うインデックスとは、これらの画素の連続を純粋な配列とみなした場合における
 *   インデックスであり16bit以上の場合バイト位置とは等しくならないことに注意する.
 *
 *   見つからなかった場合はZnk_NPOSを返す.
 *
 * @param buf :
 *   bufの開始アドレス.
 *   この関数では、bufのアドレス値はバイト境界を気にしなくてもよい.
 *   (任意の倍数アドレスを開始位置とすることができる).
 *   例えば奇数アドレスから16bit要素の連続が始まっていてもよい.
 *
 * @param size :
 *   sizeはbufのバイトサイズを指定することに注意する.
 *   また size が、U=sizeof(val)の倍数(24bitの場合はU=3)でない場合は、
 *   最終の端数バイトを差し引いたものが指定されたとみなす. 
 *
 * @param delta :
 *   ここで指定した個数をシークにおける増分とする.
 *   例えば普通に１要素ずつ調べていきたい場合は 1 を指定する.
 *   2 を指定した場合、0, 2, 4, 6, ... 番目、3 の場合 0, 3, 6, 9,... というように
 *   deltaの倍数番目の要素のみが調べられ、それ以外の要素には検索値はないものとして
 *   スキップされる(たとえ存在してもそれは無視される).
 *   delta に 0 を指定した場合、1 が指定されたのと同じとみなされる.
 *
 * @param is_LE :
 *   8/24bit版以外でのみ意味を持つ.
 *   この場合、valはパターンというよりも算術的な整数値としての意味合いを持つキーであり、
 *   bufも整数値としてシリアライズされているものであると考える.
 *   buf内の要素値が LE(Little Endian)でシリアライズされているものとみなす場合はtrue,
 *   BE(Big Endian)でシリアライズされているものとみなす場合はfalseを指定する.
 *
 *   指定するvalはその処理系ネイティブの値をそのまま与えればよい.
 *   例えば uint32_t 型の 1 という値を探す場合、処理系のEndianがどうなっているかを
 *   この関数が内部で自動的に判断し、またその結果とis_LEの指定に応じて必要に応じて
 *   自動的に byte swap 等が行われる. これによりbufが LE, BEのいずれでシリアライズ
 *   されていようとも uint32_t 型 1 を期待通り find することができる.
 *
 * @param is_swap :
 *   24bit版でのみ意味を持つ.
 *   この場合、valはパターンとしての意味合いを持つキーであり、算術的な整数値としての意味合いは
 *   ないものと考える.
 *   4byte 整数 val 値はC言語のbit演算的に上位バイトから A B C D で構成されているように見えるものとする。
 *   is_swapがfalseのとき、配列で言えばKey[3] = { B, C, D } で構成されるバイト列をbufから探索する.
 *   is_swapがtrueのとき、これとは逆に Key[3] = { D, C, B } で構成されるバイト列をbufから探索する.
 *   いずれにせよ、最上位バイトのAは無視される.
 *
 *   繰り返しになるが、指定するvalの上位,下位の概念はCでのbit演算の扱いで考える.
 *   例えば uint32_t 型の 1 という値を指定し、is_swapがfalseの場合、
 *   BE環境ならばval実際メモリ上には { 0, 0, 0, 1 } とシリアライズされているが
 *   Cのbit演算上での上位,下位を考えた場合、A=0, B=0, C=0, D=1 となり、
 *   つまり 0 0 1 というシーケンスが探索されることになる.
 *   LE環境ならばval実際メモリ上には { 1, 0, 0, 0 } とシリアライズされているが
 *   Cのbit演算上での上位,下位を考えた場合、BEと同じく A=0, B=0, C=0, D=1 となり、
 *   やはり 0 0 1 というシーケンスが探索されることになる.
 *   要するに、同じ 1 という値をvalを与えた場合、BE/LEによって挙動の差はない.
 *
 * @note
 *   これらの関数では、アドレス値がバイト境界に整えられているかどうかを内部で
 *   自動的に判定する. バイト境界にある場合はより高速なZnkAry_*関数が自動的に
 *   呼び出され、そうではない場合はshift演算などを駆使した確実な処理が行われる.
 *
 * 例
 * buf = { 9, 9, 0, 1, 2, 3, 4, 5, 6, 7, 9, 9, 8 } とする.
 * 以下では val のbyte-imageを {1,0} などと表記する.
 * 例えば、LEでuint16_t val=1 のとき、val={1,0}などと書く.
 * BEの場合であれば、これが{0,1}となる. シリアライズされているバイト列の各塊が
 * LEとBEのどちらの形式で格納されているかをis_LEフラグで指定する.
 *
 * ZnkMem_lfind_16 では {9,9}, {0,1}, {2,3}, {4,5}, {6,7}, {9,9} という塊をuint16_tとみて、
 * 順に調べる. 最後の 8 は余りになるため、無視される.
 * これらと指定した val が等しいか否かを調べ、等しいものが見つかった最初のインデックスを返す.
 * 例えば、以下のようになる.
 *
 *   val={2,3}の場合、2 が返される(配列の要素としては2番目にあるため).
 *   val={1,2}の場合、Znk_NPOSが返される. 
 *                    バイト列としては[3,5)の位置に存在するものの、
 *                    上記の塊の中に含まれないため、検知されない.
 *   val={9,9}の場合、0 が返される.
 *
 * さらに delta=2 を指定した場合、上記の塊が {9,9}, {2,3}, {6,7} のみになる.
 * (deltaの倍数番目(ただし0から数える)以外の要素はスキップされる)
 * このとき、val={2,3}の場合は、2 が返される(見つかった場合であれば
 * 結果は delta=1のときと同じである). val={1,0}の場合は、スキップされるので検知されない.
 *
 * ZnkMem_rfind_16 では {9,9}, {0,1}, {2,3}, {4,5}, {6,7}, {9,9} という塊をuint16_tとみて、
 * 逆に調べる. 最後の 8 は余りになるため、無視される（この点は lfind と同じである).
 * これらと指定した val が等しいか否かを調べ、等しいものが見つかった最後のインデックスを返す.
 * 例えば、以下のようになる.
 *
 *   val={2,3}の場合、2 が返される(lfindと同じ)
 *   val={1,2}の場合、Znk_NPOSが返される(lfindと同じ).
 *                    ( ここを {9,8},{7,9},{5,6},{3,4},{1,2}...のように調べられると
 *                    勘違いしてはいけない )
 *   val={9,9}の場合、5 が返される(lfindと異なる).
 *
 * さらに delta=2 を指定した場合、上記の塊が {9,9}, {2,3}, {6,7} のみになる.
 * (deltaの倍数番目(ただし0から数える)以外の要素はスキップされる).
 * ここまでは lfindと同じで、その上でこれらを逆に探索する( {6,7}, {2,3}, {9,9} の順 ).
 * このとき、val={2,3}の場合は、2 が返される( rfind の場合でも見つかった場合であれば
 * 結果はやはり delta=1 のときと同じである). val={1,0}の場合は、スキップされるので検知
 * されない(これもlfindと同様である). また val={9,9}の場合、0 が返される(一番初めにある
 * {9,9}の部分のみがヒットする).
 *
 * ZnkMem_lfind_32, ZnkMem_rfind_32, ZnkMem_rfind_64, ZnkMem_rfind_64 の場合も同様である.
 * ZnkMem_lfind_24, ZnkMem_rfind_24 の場合は、指定するvalの下位24bitが検索値として使われる
 * という点以外は同様である(is_LEの意味に若干注意する必要がある. 24bit版では 3バイト整数
 * というものが仮想的に存在するとして、それのバイトオーダーで考える.)
 */
size_t ZnkMem_lfind_8(  const uint8_t* buf, size_t size, uint8_t  val, size_t delta );
size_t ZnkMem_rfind_8(  const uint8_t* buf, size_t size, uint8_t  val, size_t delta );
size_t ZnkMem_lfind_16( const uint8_t* buf, size_t size, uint16_t val, size_t delta, bool is_LE );
size_t ZnkMem_rfind_16( const uint8_t* buf, size_t size, uint16_t val, size_t delta, bool is_LE );
size_t ZnkMem_lfind_32( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_LE );
size_t ZnkMem_rfind_32( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_LE );
size_t ZnkMem_lfind_64( const uint8_t* buf, size_t size, uint64_t val, size_t delta, bool is_LE );
size_t ZnkMem_rfind_64( const uint8_t* buf, size_t size, uint64_t val, size_t delta, bool is_LE );
size_t ZnkMem_lfind_24( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_swap );
size_t ZnkMem_rfind_24( const uint8_t* buf, size_t size, uint32_t val, size_t delta, bool is_swap );

/**
 * @brief
 *   指定されたvalを bufにおける [ begin, end ) の範囲においてbeginの位置から順方向に調べ、
 *   それが存在する位置を返す. 範囲が指定できる点以外は ZnkMem_lfind_8 と同じである.
 *   戻り値となる位置は、beginからの位置ではなく、bufの先頭位置を基準とした位置である.
 *
 *   bufのサイズは、end バイト以上が確保されていなければならない.
 *   特にbufの最後の位置まで調べたい場合は end に bufのバイトサイズを指定すればよい.
 */
Znk_INLINE size_t ZnkMem_lfind_pos_8( const uint8_t* buf, size_t begin, size_t end, uint8_t val, size_t delta ){
	if( begin < end ){
		size_t offset = ZnkMem_lfind_8( buf + begin, end - begin, val, delta );
		return ( offset == Znk_NPOS ) ? Znk_NPOS : ( begin + offset );
	}
	return Znk_NPOS;
}
/**
 * @brief
 *   指定されたvalを bufにおける [ begin, end ) の範囲において最後(endの一つ前)から逆方向に調べ、
 *   それが存在する位置を返す. 範囲が指定できる点以外は ZnkMem_rfind_8 と同じである.
 *   戻り値となる位置は、beginからの位置ではなく、bufの先頭位置を基準とした位置である.
 *
 *   bufのサイズは、end バイト以上が確保されていなければならない.
 *   特にbufの最後の位置から調べたい場合は end に bufのバイトサイズを指定すればよい.
 */
Znk_INLINE size_t ZnkMem_rfind_pos_8( const uint8_t* buf, size_t begin, size_t end, uint8_t val, size_t delta ){
	if( begin < end ){
		size_t offset = ZnkMem_rfind_8( buf + begin, end - begin, val, delta );
		return ( offset == Znk_NPOS ) ? Znk_NPOS : ( begin + offset );
	}
	return Znk_NPOS;
}

/***
 * 一旦occ_tblが初期化された場合、その値は0にはなり得ないため、
 * 値が 0 か否かを調べることで初期化されているか否かを判定できる.
 * 使用例.
 *
 *   static ZnkOccTable_D( st_occ_tbl );
 *   if( !ZnkOccTable_isInitialized(st_occ_tbl) ){
 *     ZnkMem_getLOccTable_forBMS( st_occ_tbl, (uint8_t*)"\r\n", 2 );
 *   }
 */
#define ZnkOccTable_D( name ) size_t name[ 256 ] = { 0 }
#define ZnkOccTable_isInitialized( occ_tbl ) ( (occ_tbl)[ 0 ] )

/***
 * @brief
 *   buf内にあると予想されるdata列の位置を探索し、そのバイト位置を返す.
 *   この関数では、delta を指定することにより、検索したい文字列をbufからdeltaの倍数の
 *   位置にあるもののみに限定することができる.（換言すれば、仮にdeltaの倍数ではない位置に
 *   パターンが見つかった場合はそれは無視する). ちなみに buf のアドレス値については delta
 *   の倍数である必要はない. delta == 1 のときは、内部でそれ相応の高速なバージョンが呼び出される.
 *
 * @param buf:
 *   検索が行われる側のバッファ.
 *   (コンピュータサイエンスではHaystack(ワラ)と表記される場合もある)
 *   バイト境界は気にする必要はない.
 *
 * @param data:
 *   検索を行うキーとなるデータ列.
 *   (コンピュータサイエンスではNeedle(針)と表記される場合もある)
 *
 * @param occ_tbl:
 *   BMS( Boyer-Moore Sunday ) 法による検索を行いたい場合のみに指定する.
 *   ここに NULL を指定することもでき、その場合は関数内部で OccTableを自動的に作成する.
 *   occ_tbl は要素数が必ず256となるsize_t型配列の先頭を示すポインタである.
 *   詳細については、ZnkMem_getLOccTable_forBMSを参照.
 *   BMS法の場合、ZnkMem_getLOccTable_forBMSでこのテーブルをdata毎に一度初期化した後に、
 *   この関数を呼び出すと効率的である. 以下にコード例を示す.
 *
 *   size_t occ_tbl[256];
 *   ZnkMem_getLOccTable_forBMS( occ_tbl, data1, data1_size );
 *   ZnkMem_lfind_data_BMS( buf1, buf1_size, data1, data1_size, delta, occ_tbl );
 *   ZnkMem_lfind_data_BMS( buf2, buf2_size, data1, data1_size, delta, occ_tbl );
 *   ZnkMem_lfind_data_BMS( buf3, buf3_size, data1, data1_size, delta, occ_tbl );
 *   ZnkMem_getLOccTable_forBMS( occ_tbl, data2, data2_size );
 *   ZnkMem_lfind_data_BMS( buf1, buf1_size, data2, data2_size, delta, occ_tbl );
 *   ZnkMem_lfind_data_BMS( buf2, buf2_size, data2, data2_size, delta, occ_tbl );
 *
 *   // Make occ_tbl internal
 *   ZnkMem_lfind_data_BMS( buf, buf_size, data, data_size, delta, NULL );
 *
 *   // Use Brute Force
 *   ZnkMem_lfind_data_BF( buf, buf_size, data, data_size, delta );
 *
 * @param delta:
 *   何バイト単位の倍数で検索するかを指定する.
 *   0 を指定した場合は 1 と同じとみなされる.
 *
 * @note
 *   例1.
 *   以下の24bitイメージから rgb というパターンを検索したい.
 *   ただし、二つの画素間で ..r gbx ... というパターンが来た場合は
 *   それは無視してほしい.
 *
 *     rxy abg xyr gbb xyz rgb yyr
 *               ^         ^
 *
 *   上記では、15 が期待する答えである. また、8 は予期しない答えとなる.
 *   この関数では delta = 3 の場合期待通り、15 を返し、delta = 1 の場合、
 *   通常のbyte単位検索として 8 を返す.
 *
 *   例2.
 *   以下のマルチバイト(2byte)文字列から 1234 というパターンを検索したい.
 *   ただし、異なるマルチバイト文字間で xx12 34yy というパターンが来た場合は
 *   それは無視してほしい.
 *
 *     xxxx xxxx xx12 34yy yyyy 1234 xxxx
 *
 *   上記では、delta = 2のとき期待通り 20 を返し、delta = 1 のとき 10 を返す.
 *
 *   このような2byteのケースでは、ZnkAry_lfind_16が使える状況ならばその方が高速である.
 *   ただし、ZnkAry_lfind_16 では bufが奇数アドレスの場合等で使用ができない.
 *   (処理系によってはうまく動作する可能性はあるがやや危険度が高い).
 *   この関数ではその制限がない.
 *
 *   ちなみにUTF8は全角文字は3byteで構成されることが多いが、たとえすべて全角文字
 *   から構成される文字列でも各文字が常に3byteとは限らないので、delta = 3 という
 *   指定は使えない. ただ、UTF8の場合は1byte目が2byte目以降とかぶらないコードに
 *   なっているはずなので、delta = 1 という指定でもこの種の文字を跨ぐ問題は発生
 *   しないはずである.
 *
 *   例3.
 *   以下のpaddingのない仮想的な構造体をシリアライズしたシーケンスがあるものとして、
 *   その中から特定のデータを検索したい.
 *   例えば、type = 10 となっているデータの位置を知りたい.
 *   struct{ uint16_t type; int data; } // size は全長6byteとする.
 *
 *     11 0234  2a 1076  10 abcd  00 xxxx
 *      ^          ^     ^
 *
 *   上記の場合、delta = 6、data = 10 とすれば 期待通りの位置12を返す.
 *   delta = 1, data = 10 の場合は位置1を返す.
 *   さらに delta = 2, data = 10 とした場合は 位置8 を返す.
 */
void ZnkMem_getLOccTable_forBMS( size_t* occ_tbl, const uint8_t* data, size_t data_size );
void ZnkMem_getROccTable_forBMS( size_t* occ_tbl, const uint8_t* data, size_t data_size );

size_t
ZnkMem_lfind_data_BF(  const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta );
size_t
ZnkMem_rfind_data_BF(  const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta );
size_t
ZnkMem_lfind_data_BMS( const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta, const size_t* occ_tbl );
size_t
ZnkMem_rfind_data_BMS( const uint8_t* buf, size_t buf_size,
		const uint8_t* data, size_t data_size, size_t delta, const size_t* occ_tbl );

/* endof lfind/rfind */
/*****************************************************************************/


Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
