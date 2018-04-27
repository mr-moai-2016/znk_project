#ifndef INCLUDE_GUARD__Est_assort_h__
#define INCLUDE_GUARD__Est_assort_h__

#include <Znk_varp_ary.h>
#include <Znk_bird.h>
#include <Est_box_base.h>

Znk_EXTERN_C_BEGIN

bool
EstAssort_renameFile_toMD5ofFile( const char* src_file_path, const char* dst_topdir, ZnkStr msg, size_t* processed_count,
		const char* file_tags, const char* comment, bool is_marge_tags,
		ZnkStr renamed_filename, bool* ans_is_moved_file );

bool
EstAssort_getEssential( ZnkStr comment,
		EstBoxDirType ast_dir_type, const char* fsys_path, const char* assort_path,
		const char* box_fsys_dir, ZnkStr msg, ZnkStr renamed_filename );
bool
EstAssort_isExist( EstBoxDirType ast_dir_type, const char* fsys_path, const char* assort_path,
		const char* favorite_dir, ZnkStr msg, ZnkStr renamed_filename );

size_t
EstAssort_doOneNewly( const char* file_tags, const char* comment, bool is_marge_tags,
		const char* src_fpath, const char* src_vpath,
		const char* box_fsys_dir, const char* box_vname, ZnkStr msg, ZnkStr renamed_filename );

size_t
EstAssort_addTags( const char* file_tags, const char* comment, const char* src_fpath, const char* src_vpath,
		const char* box_fsys_dir, const char* box_vname, ZnkStr msg, ZnkStr renamed_filename );
void
EstAssort_convertCachePath_toFilePath( ZnkStr cache_path, const char* cache_top_dir, const char* file_top_dir );

/**
 * @brief
 * トップディレクトリ src_top_dir の配下(直属とは限らない)に存在するファイル src_file_path を
 * そのサブディレクトリは維持したまま dst_top_dir へ移動する.
 * 換言すればsrc_top_dirの部分だけをdst_top_dirに置き換えたものを移動先パスとして移動するとも言える.
 *
 * @param src_top_dir:
 *  移動元のファイルを含むトップディレクトリのパスである.
 *  必ずしも移動元のファイルを直属に持つディレクトリのパスであるとは限らないことに注意.
 *
 * @param src_file_path:
 *  移動元のファイルのパスである.
 *  src_file_path は必ず src_top_dir から始まる文字列でなければならない.
 *  またこの関数では src_top_dir の終端から src_file_path の終端までにサブディレクトリに相当する文字列が存在する場合
 *  そのサブディレクトリを維持したままで dst_top_dir 配下へ移す.
 *
 * @param src_top_dir:
 *  移動先のファイルを格納するトップディレクトリのパスである.
 *  必ずしも移動先のファイルを直属に持つディレクトリのパスであるとは限らないことに注意.
 *
 * @param dst_additional_under_dir:
 *  これが明示的に指定されていた場合、dst_top_dir直下のサブディレクトリへこのディレクトリを特別に追加挿入する.
 *  例えば移動先のファイルのパスが dst_top_dir/jpg/image.jpg となっていた場合、
 *  これを dst_top_dir/dst_additional_under_dir/jpg/image.jpg のように変換する.
 *  ファイルシステム上にそのようなディレクトリが存在しない場合は自動的に作成される.
 *  これが空またはNULLで指定されている場合はサブディレクトリにそのような追加的挿入を行わない.
 *
 * @param title:
 *  エラー/リポートメッセージの際に行頭にこれを表示する.
 *
 * @param ermsg:
 *  エラー/リポートメッセージを格納するためのZnkStr.
 *  この取得が不要な場合はNULLを指定してもよい.
 */
size_t
EstAssort_moveSubdirFile( const char* src_top_dir, const char* src_file_path, const char* dst_top_dir, const char* dst_additional_under_dir,
		const char* title, ZnkStr ermsg );
bool
EstAssort_remove( EstBoxDirType ast_dir_type, const char* fsys_path, ZnkStr msg );
bool
EstAssort_remove_fromFInfList( EstBoxDirType box_dir_type, const char* md5filename, ZnkStr msg );

void
EstAssort_addImgURLList( ZnkStr EstCM_img_url_list, size_t file_count, size_t begin_idx, size_t end_idx, const char* path );

//EstBoxDirType
//EstAssort_convertAssortDir_toFSysDir( ZnkStr fsys_path, const char* src_assort_dir, size_t* ans_box_path_offset, ZnkStr msg );

bool
EstAssort_registNewTag( ZnkVarpAry post_vars, ZnkStr tag_editor_msg, ZnkStr tagid );
void
EstAssort_addPostVars_ifNewTagRegisted( ZnkVarpAry post_vars, ZnkStr tag_editor_msg );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
