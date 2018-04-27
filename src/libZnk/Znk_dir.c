#include "Znk_dir.h"
#include "Znk_dir_recursive.h"
#include "Znk_stdc.h"
#include "Znk_s_base.h"
#include "Znk_missing_libc.h"
#include "Znk_sys_errno.h"

#if   defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#else
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <errno.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if   defined(Znk_TARGET_WINDOWS)
Znk_INLINE void printWinLastError( DWORD last_err, ZnkStr ermsg )
{
	LPVOID lpMsgBuf;
	const char* msg;
	/* エラー表示文字列作成 */
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, last_err,
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );
	msg = (const char*)lpMsgBuf;
	ZnkStr_add( ermsg, msg );
	LocalFree(lpMsgBuf);
}
#endif

/**
 * @code
 * Note:
 * 処理系ごとの使用不可文字
 * ext2 :
 *     '\0'、'/' のみ(他はすべて使用可能).
 *     例外:
 *     "."、".." というファイル名をつけることはできない.
 * NTFS,FAT32 :
 *     '\\'、'/'、':'、'.'、';'、'*'、'?'、'"'、'<'、'>'、'|' が使用不可.
 *     ただし、'.' に限り、'.'が２文字目以降に来る場合は何の問題もなく使用可能.
 *     '.'が一文字目にくる場合は、エクスプローラ上からは使用が拒否されるが、
 *     プログラム上(fopen上)は下記の例外を除き使用可能.
 *
 *     上記文字のうち、'\\' と '|' のみ、ShiftJISコード文字(の第2バイト目)に
 *     含まれる場合がある(※). (逆に上記文字のその他のものについては、ShiftJISに
 *     含まれることはない). '\\'と'|'に限り、ShiftJIS内で使用可能となる.
 *     例外:
 *     " "(半角スペース一文字だけ)というファイル名をつけることはできない.
 *     "."、".." というファイル名をつけることはできない.
 *
 * (※)ShiftJISの第２バイト目にくるASCII文字は、0x40 以降(@以降)の文字になる.
 *
 * Windowsではファイルの日本語パスはShiftJISになっている.
 * (おそらくwfopen等のワイド文字版ではUTF16、通常のfopenではShiftJISを受け付けるということだろう)
 * Linux, BSDではファイルの日本語パスはEUCやUTF-8があり得る(特に最近はUTF-8が多いらしい)
 * MacでもUTF-8を使用しているという情報がある.
 * glibでは、G_FILENAME_ENCODING という環境変数で、これを明示的に指定させるようにしてある.
 *
 * Win API関数(たとえば CreateDirectory, GetFileAttributes など) が受け付けるパスが
 * 日本語である場合、SJISの場合においてうまく動作する(たとえば「マイ表計算ソフト」という
 * 文字列に対して、懸念されるような問題は発生しない). 逆にそれ以外のたとえば UTF-8
 * のような文字コードでは文字化けしたものが出来上がり、うまくいかない.
 *
 * @endcode
 */

bool
ZnkDir_changeCurrentDir( const char* dir )
{
	int result;
#if   defined(Znk_TARGET_WINDOWS)
	result = SetCurrentDirectory( dir );	
	if( result == 0 ){
		/* Permition Deny */
		return false;
	}
#else
	result = chdir( dir );
	if( result == -1 ){
		/* Permition Deny */
		return false;
	}
#endif
	return true;
}

const char*
ZnkDir_getCurrentDir( ZnkStr ans )
{
	static const size_t buf_size = 512;
	ZnkBfr_resize( ans, buf_size );

#if   defined(Znk_TARGET_WINDOWS)
	GetCurrentDirectory( (DWORD)ZnkBfr_size(ans), (LPSTR)ZnkBfr_data(ans) );	
#else
	/* UNIX */
	getcwd( (char*)ZnkBfr_data(ans), ZnkBfr_size(ans) );
#endif
	ZnkStr_normalize( ans, true );
	return ZnkStr_cstr( ans );
}


bool
ZnkDir_getFileByteSize( const char* file, int64_t* file_size )
{
	/* かならずbinaryモードで開くこと */
	ZnkFile fp = Znk_fopen( file, "rb" );
	if( fp ){
		Znk_fseek_i64( fp, 0, SEEK_END );
		*file_size = Znk_ftell_i64( fp );
		Znk_fclose( fp );
		return true;
	}
	return false;
}


/***
 * 引数で与えたファイルをコピーする。
 * (ディレクトリは不可)
 * 与える引数はsrc_fileはもちろん、dst_fileもファイル名でなければならない.
 * (コピー先としてディレクトリ名は指定できない)
 */
bool
ZnkDir_copyFile_byForce( const char* src_file, const char* dst_file, ZnkStr ermsg )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * CopyFile
	 * ?
	 */
	static const BOOL bFailIfExists = false; /* falseの場合は強制コピー */
	bool result = (bool)CopyFile( src_file, dst_file, bFailIfExists );
	if( result ){
		/* success */
		return true;
	} else {
		if( ermsg ){
			DWORD last_err = GetLastError();
			printWinLastError( last_err, ermsg );
		}
	}
	return result;
#else
	/***
	 * 手抜きだがZnk_snprintfでbuf-overflowさえ防御すれば意外と手堅い実装である.
	 * fork, execl, sendfile, mmap など色々持ち出してゴチャゴチャするよりはいい.
	 */
	char cmd[ 4096 ] = "";
#  if defined(__ANDROID__)
	Znk_snprintf( cmd, sizeof(cmd), "/system/bin/cp -p '%s' '%s'", src_file, dst_file );
#  else
	Znk_snprintf( cmd, sizeof(cmd), "/bin/cp -p '%s' '%s'", src_file, dst_file );
#  endif
	system( cmd );
	return true;
#endif
}

bool
ZnkDir_deleteFile_byForce( const char* file )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * DeleteFile:
	 * ファイルのみを強制削除する(ディレクトリは不可)
	 */
	return (bool) DeleteFile(file);
#else
	/***
	 * unlink:
	 * ファイルのみを強制削除する(ディレクトリは不可)
	 */
	if( unlink(file) == 0 ){
		return true; /* if 0, then success. */
	}
	return false;
#endif
}

bool
ZnkDir_rmdir( const char* dir )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * RemoveDirectory:
	 * 空のディレクトリのみを削除する(ファイルは不可)
	 */
	return (bool) RemoveDirectory(dir);

#else
	/***
	 * rmdir:
	 * 空のディレクトリのみを削除する(ファイルは不可)
	 */
	if( rmdir(dir) == 0 ){
		/* if 0, then success. */
		return true;
	}
	/* failure. */
	return false;
#endif
}

bool
ZnkDir_rename( const char* src_path, const char* dst_path, ZnkStr ermsg )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * MoveFile:
	 *  ファイルまたはディレクトリの移動が可能.
	 *  ただし、ディレクトリに関しては、異なるドライブ(ボリューム)への移動(C:\内=>D:\内等)ができない.
	 *  (ファイルは可能). つまりディレクトリの場合は、src_path, dst_pathともに同じドライブ(ボリューム)で
	 *  なければならないとされる. 
	 *  dst_pathが指し示すもの(それがファイルであっても)が既に存在する場合、
	 *  この関数はFALSEを返し失敗することに注意.
	 */
	SetLastError(NO_ERROR); /* エラー情報をクリアする */
	if( MoveFile(src_path, dst_path) ){
		/* success */
		return true;
	} else {
		if( ermsg ){
			DWORD last_err = GetLastError();
			printWinLastError( last_err, ermsg );
		}
	}
	return false;

#else
	/***
	 * rename:
	 *  ファイルまたはディレクトリの移動が可能.
	 *  ただし、ディレクトリに関しては、指定した移動先名が存在しないディレクトリか
	 *  空のディレクトリである必要がある.
	 *  異なるマウントデバイスへの移動ができない.
	 */
	if( rename(src_path, dst_path) == 0 ){
		/* success */
		return true;
	} else {
		if( ermsg ){
			uint32_t sys_errno = ZnkSysErrno_errno();
			ZnkSysErrnoInfo* einfo = ZnkSysErrno_getInfo( sys_errno );
			ZnkStr_addf( ermsg, "Error : ZnkDir_rename src_path=[%s] dst_path=[%s] [%s]\n", src_path, dst_path, einfo->sys_errno_msg_ );
		}
	}
	return false;
#endif

}


/***
 * ここは処理系固有のAPIを用いて実装してもよい.
 *
 * dirを与え、そこに新規ディレクトリを一つ作成する.
 *
 * dirには、新しく作成したいディレクトリのパスを指定する.
 * これが示すものが、既に存在していてはならない.
 * (存在した場合の動作については、処理系依存となる).
 *
 * また、dirが指すディレクトリ直属の親は既に存在していなければならない.
 * (存在しない場合の動作については、処理系依存となる).
 *
 */
static bool
makeDirectory( const char* dir, ZnkStr ermsg )
{
#if   defined(Znk_TARGET_WINDOWS)
	/***
	 * CreateDirectory:
	 *  ただしdir直属の親は既に存在していなければならない.
	 *  さもなければ、0 を返す(つまり失敗とみなされる).
	 *  また、既に存在しているディレクトリを指定した場合、
	 *  この関数は 0 を返す(つまり失敗とみなされる)
	 */
	bool result = (bool)( CreateDirectory( dir, NULL ) != 0 );
	if( result ){
		/* success */
		if( ermsg ){
			ZnkStr_addf( ermsg, "makeDirectory[%s] result=[%d]\n", dir, result );
		}
		return true;
	} else {
		DWORD last_err = GetLastError();
		if( last_err == ERROR_ALREADY_EXISTS ){
			result = true;
		} else if( ermsg ){
			/***
			 * 「既に存在しています」を除いたエラーメッセージを出力.
			 */
			printWinLastError( last_err, ermsg );
			ZnkStr_addf( ermsg, "makeDirectory[%s]\n", dir );
		}
	}
	return result;

#else
	/***
	 * mkdir:
	 *  ディレクトリの作成.
	 *  以下でmode_t は作成されるディレクトリのパーミッションを示す.
	 *
	 *  S_IRWXU
	 *    Setting Read, Write, and Execute Permissions for the User(Owner) Only.
	 *
	 *  S_IRGRP
	 *    Setting Read Permissions for Group.
	 *
	 *  S_IXGRP
	 *    Setting Execute Permissions for Group.
	 *
	 *  S_IROTH
	 *    Setting Read Permissions for Others.
	 *
	 *  S_IXOTH
	 *    Setting Execute Permissions for Others.
	 *
	 *  この関数が成功した場合は 0 を返す.
	 *  ( 0 が成功の意である. この点は WinAPI CreateDirectoryと逆である )
	 *
	 *  dirの親は既に存在していなければならない(Arch Linuxにおいて確認済み).
	 *  この点は Win32 API のCreateDirectoryと同じである.
	 *
	 *  また、dirとして既に存在しているディレクトリを指定した場合、この関数は -1 を返す.
	 *  (つまり失敗とみなされるという点で、これもまた CreateDirectoryと同じである).
	 */
	mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
	bool result = ( mkdir( dir, mode ) == 0 );
	if( !result ){
		uint32_t sys_errno = ZnkSysErrno_errno();
		if( sys_errno == EEXIST ){
			result = true;
		} else if( ermsg ){
			/***
			 * 「既に存在しています」を除いたエラーメッセージを出力.
			 */
			ZnkSysErrnoInfo* einfo = ZnkSysErrno_getInfo( sys_errno );
			ZnkStr_addf( ermsg, "Error : makeDirectory[%s] [%s]\n", dir, einfo->sys_errno_msg_ );
		}
	}
	return result;
#endif
}

/***
 * ここで処理できる形式は、相対パス、'/'で始まる絶対パス、
 * X:\またはX:/(DOS型)で始まる絶対パス、\\hostnameまたは//hostname(UNC型)で始まる絶対パスである.
 * ディレクトリを区切るセパレータ文字(以下これをSEPと書く)として、/ と \ をサポートしており、
 * これらが混合したpathを扱うことも可能である.
 *
 * sep として / のみを認識するように指示することができ、この場合は日本語などのMultibyte文字を
 * 含むpathを扱うこともできる. sepにそれ以外の値を指定した場合は、/ と \ の両方を認識するが、
 * この場合、日本語などのMultibyte文字を含んでいてはならない. これはSJISにおいて「ソ」や「表」
 * などの文字が現れた場合、その部分をSEPの \ と誤判定する恐れがあるからである.
 *
 * この関数では、\ が現れた場合にその直前の文字がそのような文字の可能性がある時点でエラーとして
 * 処理を中断しfalseを返す. ここでは厳密な判定はしない. つまりprevがMultibyte文字における1byte目
 * なのか2byte目なのかの判定である. 1byte目である場合はこのendはSEPとみなしてはならず、
 * 2byte目の場合は逆にSEPとみなさなければならない. この判定を実現するにはZnkS_lfind_one_ofを
 * Multibyte文字対応としなければならずコードが複雑化するので現段階ではサポートしていない.
 */
bool
ZnkDir_mkdirPath( const char* path, size_t path_leng, char sep, ZnkStr ermsg )
{
	bool result = false;
	size_t begin = 0;
	size_t end   = 0;
	const char*  sep_set      = ( sep == '/' ) ? "/" : "\\/";
	const size_t sep_set_leng = ( sep == '/' ) ? 1 : 2;

	switch( path[0] ){
	case '\0' :
		return false;
	case '/':
		begin = 1;
		break;
	case '\\':
		/* DOSのUNC絶対パス */
		/* hostnameの部分をskip */
		begin = ZnkS_lfind_one_of( path, 2, path_leng, sep_set, sep_set_leng );
		if( begin == Znk_NPOS ){
			return false;
		}
		++begin;
		break;
	default:
		break;
	}

	if( path_leng >= 2 ){
		if( path[ 1 ] == ':' && isalpha( path[0] ) ){
			/* DOS絶対パス */
			begin = 3;
		}
	}

	{
		/***
		 * makeDirectory内のWindowsAPIおよびSystemCallではNULL終端するpath文字列を
		 * 渡す必要がある. そしてここではsepを通過するたびにディレクトリを親から子へと
		 * 段階的に作成していかなければならず、それぞれの呼び出しにおいてpath文字列内の
		 * 使用範囲を段階的に拡張していく形になる.
		 *
		 * これを実現するには、path内のsepの存在する位置に一時的にNULL終端文字を代入し、
		 * makeDirectoryを呼び出す必要がある. しかしながら、もしpathに文字列リテラルが
		 * 指定されていた場合、これは大抵の環境ではROM領域にあり、よってこの方法では
		 * セグメンテーションバイオレーションを引き起こす.
		 *
		 * よってpathを直接書き換えることはここでは出来ない. 
		 * 別の書込み可な文字列バッファに一旦全体をコピーしてそれを使う必要がある.
		 */
		char path_buf[ 1024 ] = "";
		ZnkS_copy( path_buf, sizeof(path_buf), path, path_leng );

		while( true ){
			end = ZnkS_lfind_one_of( path_buf, begin, path_leng, sep_set, sep_set_leng );
			if( end == Znk_NPOS ){
				result = makeDirectory( path_buf, ermsg ); /* 必ず文字列の開始位置(path_buf)から指定 */
				break;
			} else if( end == begin ){
				/***
				 * 絶対パスの先頭であるか、または SEPが二つ以上連続しているケースなどでは
				 * ここに来る可能性がある. これはスキップする.
				 */
				++begin;
			} else {
				if( path_buf[ end ] == '\\' ){
					/***
					 * \ 文字の直前が 下記の範囲であるような文字コードであった場合は
					 * ここで処理を中断し、エラーとする.
					 */
					size_t prev = end-1;
					int prev_ch = path_buf[ prev ];
					if( prev_ch & 0x80 ){
						if( prev_ch <= 0x9F || prev_ch >= 0xE0 ){ 
							return false;
						}
					}
				}
				/***
				 * 既に存在しているDirectoryの場合、以下のresultはfalseとなるが
				 * その場合でも中断せず、最後まで続ける.
				 */
				path_buf[ end ] = '\0'; /* 一時的にここで終端する */
				result = makeDirectory( path_buf, ermsg ); /* 必ず文字列の開始位置(path_buf)から指定 */
				path_buf[ end ] = '/'; /* SEPに戻す */
				begin = end + 1;
			}
		}
	}
	return result;
}

static bool rmdirAll_force_onEnterDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	ZnkStr ermsg = Znk_force_ptr_cast( ZnkStr, arg );
	if( ermsg ){
		ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : onEnterDir : [%s]\n", top_dir );
	}
	return true;
}
static bool rmdirAll_force_processFile( ZnkDirRecursive recur, const char* file_path, void* arg, size_t local_err_num )
{
	bool result = false;
	ZnkStr ermsg = Znk_force_ptr_cast( ZnkStr, arg );
	if( ZnkDir_deleteFile_byForce( file_path ) ){
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : deleteFile : [%s] OK.\n", file_path );
		}
		result = true;
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : deleteFile : [%s] failure.\n", file_path );
		}
		result = false;
	}
	return result;
}
static bool rmdirAll_force_onExitDir( ZnkDirRecursive recur, const char* top_dir, void* arg, size_t local_err_num )
{
	bool result = false;
	ZnkStr ermsg = Znk_force_ptr_cast( ZnkStr, arg );
	if( local_err_num == 0 ){
		if( ZnkDir_rmdir( top_dir ) ){
			if( ermsg ){
				ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : rmdir : [%s] OK.\n", top_dir );
			}
			result = true;
		} else {
			if( ermsg ){
				ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : rmdir : [%s] failure.\n", top_dir );
			}
			result = false;
		}
	} else {
		if( ermsg ){
			ZnkStr_addf( ermsg, "ZnkDir_rmdirAll_force : onExitDir : local_err_num = [%zu].\n", local_err_num );
		}
		result = false;
	}
	return result;
}
bool
ZnkDir_rmdirAll_force( const char* dir, bool is_err_ignore, ZnkStr ermsg )
{
	bool result = false;
	ZnkDirRecursive recur = ZnkDirRecursive_create( is_err_ignore,
			rmdirAll_force_onEnterDir,  ermsg,
			rmdirAll_force_processFile, ermsg,
			rmdirAll_force_onExitDir,   ermsg );
	result = ZnkDirRecursive_traverse( recur, dir, ermsg );
	ZnkDirRecursive_destroy( recur );
	return result;
}
