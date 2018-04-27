#ifndef INCLUDE_GUARD__Znk_dir_h__
#define INCLUDE_GUARD__Znk_dir_h__

#include <Znk_str.h>

Znk_EXTERN_C_BEGIN

typedef struct ZnkDirInfoImpl* ZnkDirId;

/**
 * @brief
 * パスなどで指定したオブジェクトがどのようなタイプかを示す.
 * ここではそのタイプが不明な場合や、存在しない場合も一種のタイプとみなす.
 *
 * ZnkDirType_e_CannotKnow:
 *   途中のディレクトリのサーチパーミッションに阻まれ、
 *   パスにおけるtailの部分の存在性、属性などが現在の権限では調査不可能な場合を示す.
 *
 * ZnkDirType_e_NotFound:
 *   指定したパスが示すディレクトリ/ファイルなどのオブジェクトは存在しないことが
 *   確認されたことを示す.
 *
 * ZnkDirType_e_Directory:
 *   指定したパスが示すオブジェクトはディレクトリであることが確認されたことを示す.
 *
 * ZnkDirType_e_File:
 *   指定したパスが示すオブジェクトはファイルであることが確認されたことを示す.
 *   (現状ではシンボリックリンクやパイプなどの特殊ファイルもこれに含める)
 */
typedef enum {
	 ZnkDirType_e_CannotKnow=-1
	,ZnkDirType_e_NotFound=0
	,ZnkDirType_e_Directory
	,ZnkDirType_e_File
}ZnkDirType;


/***
 * ZnkDirIdentity_e_CannotKnow:
 *   少なくともいずれか一方にサーチパーミッション等がなく
 *   現在の権限では判定不能な場合.
 *
 * ZnkDirIdentity_e_Same:
 *   二つのパスは同じファイルorディレクトリを示していることが
 *   確認された場合.
 *
 * ZnkDirIdentity_e_DifferAndCannotKnowDevice:
 *   二つのパスは異なるファイルorディレクトリを示していることまでが
 *   確認されたが、デバイスが同じであるか否かまでは判定不能である場合.
 *
 * ZnkDirIdentity_e_DifferAndSameDevice:
 *   二つのパスは異なるファイルorディレクトリを示しているが、
 *   デバイスは同じであることが確認された場合.
 *
 * ZnkDirIdentity_e_DifferAndDifferDevice:
 *   二つのパスは異なるファイルorディレクトリを示しており、
 *   デバイスが異なることが確認された場合.
 *
 * ZnkDirIdentity_e_NotFound:
 *   二つのパスのいずれか一方、あるいは両方が存在しないファイルorディレクトリを
 *   示していることが確認された場合.
 */
typedef enum {
	 ZnkDirIdentity_e_CannotKnow = -1
	,ZnkDirIdentity_e_Same = 0
	,ZnkDirIdentity_e_DifferAndCannotKnowDevice
	,ZnkDirIdentity_e_DifferAndSameDevice
	,ZnkDirIdentity_e_DifferAndDifferDevice
	,ZnkDirIdentity_e_NotFound
}ZnkDirIdentity;

ZnkDirType
ZnkDir_getType( const char* path );

ZnkDirIdentity
ZnkDir_getIdentity( const char* path1, const char* path2 );

/**
 * @brief
 * カレントディレクトリの変更
 */
bool
ZnkDir_changeCurrentDir( const char* dir );

/**
 * @brief
 *   カレントディレクトリのフルパスを取得
 *
 * @return
 *   ans にはカレントディレクトリが設定されるが、
 *   その内部 C文字列へのポインタを返す.
 *   本来これは必要ないが、エラーメッセージの構築時の利便性のための仕様である.
 */
const char*
ZnkDir_getCurrentDir( ZnkStr ans );


/**
 * @brief
 * file が示すファイルのサイズを取得する.
 * 成功ならばtrue, さもなければfalseを返す.
 * (指定されたパスがディレクトリであった場合等に失敗する)
 */
bool
ZnkDir_getFileByteSize( const char* file, int64_t* file_size );


/**
 * @brief:
 *   引数で与えたファイルsrc_file を dst_file へコピーする.
 *   これらはディレクトリであってはならない(その場合何もせずエラー値を返す).
 *
 * @param ermsg
 *   エラーまたはリポートメッセージ格納用.
 *   NULLを指定することもでき、その場合は単に無視される.
 */
bool
ZnkDir_copyFile_byForce( const char* src_file, const char* dst_file, ZnkStr ermsg );

/**
 * @brief:
 *   引数で与えたファイル を削除する.
 *   これらはディレクトリであってはならない(その場合何もせずエラー値を返す).
 *
 * @return:
 *   削除に成功した場合trueを返す.
 *   さもなくばfalseを返す.
 */
bool
ZnkDir_deleteFile_byForce( const char* file );

/**
 * @brief:
 *   引数で与えたディレクトリの中身が空であった場合、
 *   そのディレクトリを削除する。
 *
 * @return:
 *   削除に成功した場合trueを返す.
 *   指定したディレクトリが空でなかったり、引数に指定したものがファイルで
 *   あった場合、その他なんらかの理由で削除に失敗した場合はなにもせずfalseを返す.
 */
bool
ZnkDir_rmdir( const char* dir );


/**
 * @brief
 *   ファイル/ディレクトリ名を変更(必要に応じて移動)する。
 *   ただし、ディレクトリに関しては同じドライブ内の移動に限られる.
 *   UNIXに関しては、ファイル/ディレクトリともに同じマウント内の移動に限られる.
 *
 * @param ermsg
 *   エラーまたはリポートメッセージ格納用.
 *   NULLを指定することもでき、その場合は単に無視される.
 *
 * @return:
 *   移動(リネーム)に成功した場合trueを返す.
 *   さもなくばfalseを返す.
 */
bool
ZnkDir_rename( const char* src_path, const char* dst_path, ZnkStr ermsg );



/**
 * @brief:
 *   引数で与えたディレクトリを作成する.
 *   pathには相対パスやA/B/Cというような複数の階層を指定してそれらを一挙に
 *   作成することができる.
 *
 * @param path:
 *   新規作成したいディレクトリを指定する.
 *   A/B/Cというような複数の階層を持つものも可能で、AやBが存在しない場合でも
 *   エラーとはならず自動的に作成される.
 *
 * @param path_leng:
 *   pathの文字列長を指定する.
 *   path が NULL終端している場合に限り、ここにZnk_NPOSを指定することができ、
 *   その場合は strlen(path)が指定されたのと同じとみなされる.
 *
 * @param sep
 *   ディレクトリを区切るセパレータ文字(以下これをSEPと書く)として、
 *   / のみを使いたい場合は '/' を指定する.
 *   その場合、pathに日本語を含めることが出来る.
 *
 *   それ以外の文字を指定した場合は / と \ の双方に対応する.
 *   その場合、これらが混合したpathを扱うことも可能であるが、
 *   pathに日本語を含めてはならない.
 *
 * @param ermsg
 *   エラーまたはリポートメッセージ格納用.
 *   NULLを指定することもでき、その場合は単に無視される.
 *
 * @return:
 *   作成に成功した場合trueを返す.
 *   あるいは指定したpathが既に存在する場合もtrueを返す.
 *   なんらかの理由で作成に失敗した場合はなにもせずfalseを返す.
 */
bool
ZnkDir_mkdirPath( const char* path, size_t path_leng, char sep, ZnkStr ermsg );



/**
 * @brief
 *   dir_name により指定した directory を Open する.
 *   失敗するとNULLを返す.
 *   これにより、dir_name が示す directory直下にある fileやdirectory
 *   (の主に名前)をイテレートによって順次取得することができる.
 *   通常は必ず、ZnkDir_openDir, ZnkDir_readDir, ZnkDir_closeDirをセットにして使う.
 *   詳しい使用方法は examを参照.
 *
 * @param dir_name:
 *   中身をイテレートしたいディレクトリパスを指定する.
 *   相対パス、絶対パスのいずれでもよい.
 *
 * @param dir_name_leng:
 *   ZnkDir_openDirEx のみ存在する.
 *   dir_name の文字列長を指定する.
 *   特にdir_nameがNULL終端している場合に限り、ここにZnk_NPOSを指定することができ、
 *   その場合は strlen(dir_name) が指定されたものとみなされる.
 *
 * @param is_skip_dot_and_dot_dot:
 *   ZnkDir_openDirEx のみ存在する.
 *   ZnkDir_readDirにおける file/directory名の取得において、. と .. が取得された場合、
 *   内部でそれを自動的にスキップし、次の名前を取得する. 特別な事情がない限り、通常は
 *   true にしておけばよいだろう.
 *
 * @exam
 *   たとえば、あるディレクトリdir内にあるファイルとディレクトリをすべて
 *   標準出力に表示するコードは以下のようになる.
 *
 *   test1()
 *   {
 *     const char* name;
 *     ZnkDirId id = ZnkDir_openDir( dir );
 *     if( id == NULL ){
 *       return false; // Error
 *     }
 *     while( true ){
 *       name = ZnkDir_readDir( id );
 *       if( name == NULL ){
 *         break; // すべてイテレートしつくした.
 *       }
 *       // name が . または .. に等しいか否かのチェックは不要である.
 *       // これらはデフォルトでは ZnkDir_readDir内で自動的にスキップされる.
 *       printf( "[%s]\n", name );
 *     }
 *     ZnkDir_closeDir( id );
 *   }
 *
 *   次に、ZnkDir_openDirEx を使った例を示す.
 *   今度の例では、. または .. をあえてスキップするようにはしていないため、
 *   ユーザはこの種の判定を必要とする(ただし通常はこのようなことは不要である).
 *
 *   test2()
 *   {
 *     const char* name;
 *     ZnkDirId id = ZnkDir_openDirEx( dir, Znk_NPOS, false );
 *     if( id == NULL ){
 *       return false; // Error
 *     }
 *     while( true ){
 *       name = ZnkDir_readDir( id );
 *       if( name == NULL ){
 *         break; // すべてイテレートしつくした.
 *       }
 *       if( strcmp( name, "." ) == 0 || strcmp( name, ".." ) == 0 ){
 *         continue;
 *       }
 *       printf( "[%s]\n", name );
 *     }
 *     ZnkDir_closeDir( id );
 *   }
 */
ZnkDirId
ZnkDir_openDirEx( const char *dir_name, size_t dir_name_leng, bool is_skip_dot_and_dot_dot );
Znk_INLINE ZnkDirId
ZnkDir_openDir( const char *dir_name ){
	return ZnkDir_openDirEx( dir_name, Znk_NPOS, true );
}

/**
 * @brief
 *   ZnkDirId を read し、ファイル名へのポインタを返す.
 *   このファイル名は idが指す内部データに内包する文字列バッファへのポインタであり、
 *   改変やfreeしてはならない. また一度この関数が呼ばれた後は、この内部バッファが
 *   次のファイル/ディレクトリを指すように内部で自動的に更新される. 従っていつかは
 *   ディレクトリの終端に達し、そのときNULLを返す.
 *   ただし内部でなんらかのerrorが発生した場合もNULLを返す.
 *   詳しい使用方法については、ZnkDir_openDirを参照.
 */
const char*
ZnkDir_readDir( ZnkDirId id );

/**
 * @brief
 * ZnkDirId をクローズする.
 * 詳しい使用方法については、ZnkDir_openDirを参照.
 */
void ZnkDir_closeDir( ZnkDirId id );


/**
 * @brief
 *   dir_path により指定したディレクトリ直下にあるfile/directoryの
 *   個数を取得する(この場合、. と .. は除いてカウントされる).
 *   あくまでdir_pathの直下にあるものだけであり、その下のsub-directory
 *   までを再帰的に数えた総数ではないことに注意する.
 *
 * @return
 *   ディレクトリ直下にあるfile/directoryの個数を返す.
 *   ただし、dir_pathで指定したディレクトリがオープンできないなど、
 *   なんらかのエラーが発生した場合、Znk_NPOS を返す.
 *   この値が 0 のとき、dir_pathが示すディレクトリは空であることを示す.
 */
Znk_INLINE size_t
ZnkDir_getNumOfDirList( const char* dir_path )
{
	size_t count = 0;
	ZnkDirId dir_id = ZnkDir_openDir(dir_path);
	if( dir_id == NULL ){
		return Znk_NPOS; /* Error */
	} else {
		while( ZnkDir_readDir( dir_id ) ){ ++count; }
		ZnkDir_closeDir( dir_id );
	}
	return count;
}

bool
ZnkDir_rmdirAll_force( const char* dir, bool is_err_ignore, ZnkStr ermsg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
