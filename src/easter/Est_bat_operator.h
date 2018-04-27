#ifndef INCLUDE_GUARD__Est_bat_operator_h__
#define INCLUDE_GUARD__Est_bat_operator_h__

#include <Est_box_base.h>
#include <Znk_varp_ary.h>

Znk_EXTERN_C_BEGIN

typedef bool (*EstBatOperator_ProcessFunc)( EstBoxDirType ast_dir_type,
		const char* fsys_path, const char* unesc_vpath, size_t box_path_offset, ZnkStr msg, void* param );

/**
 * @brief
 *   given_vars より操作対象のBox被操作オブジェクトを抽出し、
 *   それらがDirectoryかFileかに応じて EstBatOperator_ProcessFunc を一括適用する処理の抽象化である.
 */
size_t
EstBatOperator_processBox( const ZnkVarpAry given_vars, ZnkStr msg,
		EstBatOperator_ProcessFunc proc_dir_func, 
		EstBatOperator_ProcessFunc proc_file_func, 
		void* param );

Znk_EXTERN_C_END

#endif /* INCLUDE_GUARD */
