#ifndef INCLUDE_GUARD__Znk_str_path_h__
#define INCLUDE_GUARD__Znk_str_path_h__

#include <Znk_str.h>
#include <Znk_dir.h>

Znk_EXTERN_C_BEGIN

void
ZnkStrPath_replaceDSP( ZnkStr path, char dsp );

/**
 * @brief:
 *  与えられたpathにおけるTailの開始位置を返す.
 *
 * @note:
 *  以下のいずれかを満たすとき、これをpathのTailと呼ぶ.
 *  1. pathの最後に現れるdspより後の文字列.
 *  2. path内にdspが出現しない場合、それがUNCパスであればその開始文字列(\\)より後の文字列.
 *  3. path内にdspが出現しない場合、それがUNCパスでなければpath全体.
 *
 * @exam:
 *  path = /abc/def のとき d の位置、すなわち 5 を返す.
 *  path = abc のとき a の位置、すなわち 0 を返す.
 */
size_t
ZnkStrPath_getTailPos( const char* path );

/**
 * @brief:
 * PATH型環境変数path_envvarに新しいパスnew_pathを追加する.
 * ここで言うPATH型環境変数とは、WindowsにおけるPATH環境変数のように ; で区切られた要素を連結した塊である.
 * あるいはUNIXにおけるPATHのように : で区切られることもあるが、この区切り文字をpspとして引数に与える.
 * 尚、path_envvar内に既にnew_pathが存在する場合は重複して追加されない.
 */
void
ZnkStrPath_addPathEnvVar( ZnkStr path_envvar, char psp, const char* new_path );

/**
 * @brief:
 * pathの先頭に Cygwin型のドライブプレフィックス(/cygdrive/x/) が存在する場合は
 * その部分をDOS型のドライブプレフィックス(x:)に変換しtrueを返す.
 * 存在しない場合は何もせずfalseを返す.
 */
bool
ZnkStrPath_convertCygDrive_toDosDrive( ZnkStr path );

/**
 * @brief:
 * この関数は まずans_dir に設定されたパスから始め、その配下にlandmarkで指定されたファイルまたはディレクトリが
 * ファイルシステム上に存在するかを調べる.
 * 存在したならばそこで探索を打ち切ってtrueを返す.
 * しないならば、ans_dirの最後尾に ../ を追加してもう一度それを行う.
 * これをdepthで指定した回数試しても存在しなかった場合は処理を打ち切ってfalseを返す.
 *
 */
bool
ZnkStrPath_searchParentDir( ZnkStr ans_dir, size_t depth, const char* landmark, ZnkDirType landmark_type, char dsp );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
