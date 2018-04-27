#include "Znk_str_fio.h"
#include "Znk_stdc.h"

bool
ZnkStrFIO_fgets( ZnkStr line, size_t pos, size_t block_size, ZnkFile fp )
{
	const size_t leng = ZnkStr_leng( line );
	uint8_t* data;
	char* ptr;
	size_t readed_len;
	if( pos == Znk_NPOS || pos > leng ){
		pos = leng;
	}
	ZnkStr_releng( line, pos );
	if( block_size < 4 ){ block_size = 4; }

	ZnkBfr_reserve( line, pos + block_size );
	data = ZnkBfr_data( line ); 
	ptr = (char*)( data + pos );

	while( true ){
		if( Znk_fgets( ptr, block_size, fp ) == NULL ){
			return false;
		}
		readed_len = Znk_strlen( ptr );
		pos += readed_len;
		ZnkStr_releng( line, pos );

		if( readed_len + 1 < block_size ){
			/* bufをフルに使っていない状態.
			 * OK. 明らかに一行をすべて読みきっている */
			break;
		}

		/* bufをフルに使っている状態. */

		/* 最終文字が'\n'であるか否かを確認する. */
		if( ZnkStr_last( line ) == '\n' ){
			break; /* OK. 一行をすべて読みきっている */
		}

		/* 最終文字が'\n'ではない. 一行をすべて読みきっていない.
		 * 続行する. */
		ZnkBfr_reserve( line, pos + block_size );
		data = ZnkBfr_data( line ); 
		ptr = (char*) data + pos;
	}
	return true;
}

