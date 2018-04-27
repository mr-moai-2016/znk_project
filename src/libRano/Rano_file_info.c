#include "Rano_file_info.h"

#if defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#elif defined(Znk_TARGET_UNIX)
#  include <time.h>
#  include <sys/stat.h>
#else
#  error "Not Supported"
#endif

/*****************************************************************************/
/* FileTime */

#if defined(Znk_TARGET_UNIX)
static bool
getLastUpdatedTime_forUNIX( const char* filepath, ZnkDate* date )
{
	struct stat fi;
	if( stat( filepath, &fi) == 0 ){
		struct tm* t;
#if 0
		/* ファイル情報を表示 */
		{
			printf( "デバイスID : %d\n"，                      fi.st_dev);
			printf( "inode番号 : %d\n"，                       fi.st_ino);
			printf( "アクセス保護 : %o\n"，                    fi.st_mode);
			printf( "ハードリンクの数 : %d\n"，                fi.st_nlink);
			printf( "所有者のユーザID : %d\n"，                fi.st_uid);
			printf( "所有者のグループID : %d\n"，              fi.st_gid);
			printf( "デバイスID（特殊ファイルの場合） : %d\n"，fi.st_rdev);
			printf( "容量（バイト単位） : %d\n"，              fi.st_size);
			printf( "ファイルシステムのブロックサイズ : %d\n"，fi.st_blksize);
			printf( "割り当てられたブロック数 : %d\n"，        fi.st_blocks);
			printf( "最終アクセス時刻 : %s"，ctime( &fi.st_atime ) );
			printf( "最終修正時刻 : %s"，    ctime( &fi.st_mtime ) );
			printf( "最終状態変更時刻 : %s"，ctime( &fi.st_ctime ) );
		}
#endif
		
		/***
		 * localtimeの代りにgmtimeでうまく動くならtimezone設定を見直せということになる.
		 * マルチスレッドなプログラムならlocaltime_rを使えって事になる.
		 */
		t = localtime( &fi.st_ctime );
		
		ZnkDate_set_year(  date, (uint16_t)(t->tm_year+1900) );
		ZnkDate_set_month( date, (uint8_t )(t->tm_mon+1) );
		ZnkDate_set_day(   date, (uint8_t )(t->tm_mday) );
		ZnkDate_set_hour(   date, (uint8_t)(t->tm_hour) );
		ZnkDate_set_minute( date, (uint8_t)(t->tm_min) );
		ZnkDate_set_second( date, (uint8_t)(t->tm_sec) );
		return true;
	}
	return false;
}
#endif

#if defined(Znk_TARGET_WINDOWS)
static bool
getLastUpdatedTime_forWindows( const char* filepath, ZnkDate* date )
{
	FILETIME ftC; /* 作成日時 */
	FILETIME ftA; /* 最終アクセス日時 */
	FILETIME ftW; /* 最終更新日時 */

	FILETIME ftl;
	SYSTEMTIME st;
	HANDLE hFile;

	hFile = CreateFile( filepath,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0 );

	if( hFile == INVALID_HANDLE_VALUE ){
		/* failed to open the file */
		return false;
	}else{
		/* get time */
		GetFileTime( hFile,
			&ftC,
			&ftA,
			&ftW );
	}
	CloseHandle( hFile );

	FileTimeToLocalFileTime( &ftW, &ftl );
	FileTimeToSystemTime( &ftl, &st );

	ZnkDate_set_year(  date, (uint16_t)(st.wYear)  );
	ZnkDate_set_month( date, (uint8_t )(st.wMonth) );
	ZnkDate_set_day(   date, (uint8_t )(st.wDay)   );
	ZnkDate_set_hour(   date, (uint8_t)(st.wHour)   );
	ZnkDate_set_minute( date, (uint8_t)(st.wMinute) );
	ZnkDate_set_second( date, (uint8_t)(st.wSecond) );

	return true;
};
#endif

bool
RanoFileInfo_getLastUpdatedTime( const char* filepath, ZnkDate* date )
{
	bool result = false;
#if defined(Znk_TARGET_WINDOWS)
	result = getLastUpdatedTime_forWindows( filepath, date );
#elif defined(Znk_TARGET_UNIX)
	result = getLastUpdatedTime_forUNIX( filepath, date );
#else
#  error "Not Supported"
#endif
	/* 曜日の更新 */
	ZnkDate_updateWDay( date );
	return result;
}

/*****************************************************************************/
/* FileSize */

#if defined(Znk_TARGET_UNIX)
static bool
getFileSize_forUNIX( const char* filepath, uint64_t* ans_size )
{
	struct stat fi;
	if( stat( filepath, &fi) == 0 ){
#if 0
		/* ファイル情報を表示 */
		{
			printf( "デバイスID : %d\n"，                      fi.st_dev);
			printf( "inode番号 : %d\n"，                       fi.st_ino);
			printf( "アクセス保護 : %o\n"，                    fi.st_mode);
			printf( "ハードリンクの数 : %d\n"，                fi.st_nlink);
			printf( "所有者のユーザID : %d\n"，                fi.st_uid);
			printf( "所有者のグループID : %d\n"，              fi.st_gid);
			printf( "デバイスID（特殊ファイルの場合） : %d\n"，fi.st_rdev);
			printf( "容量（バイト単位） : %d\n"，              fi.st_size);
			printf( "ファイルシステムのブロックサイズ : %d\n"，fi.st_blksize);
			printf( "割り当てられたブロック数 : %d\n"，        fi.st_blocks);
			printf( "最終アクセス時刻 : %s"，ctime( &fi.st_atime ) );
			printf( "最終修正時刻 : %s"，    ctime( &fi.st_mtime ) );
			printf( "最終状態変更時刻 : %s"，ctime( &fi.st_ctime ) );
		}
#endif
		*ans_size = fi.st_size;
		return true;
	}
	return false;
}
#endif

#if defined(Znk_TARGET_WINDOWS)
static bool
getFileSize_forWindows( const char* filepath, uint64_t* ans_size )
{
	bool result = false;
	HANDLE hFile = CreateFile( filepath,
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0 );

	if( hFile == INVALID_HANDLE_VALUE ){
		/* failed to open the file */
		return false;
	}else{
		/* get time */
		DWORD dw_filesize = GetFileSize( hFile, NULL );
		if( dw_filesize != 0xFFFFFFFF ){
			*ans_size = dw_filesize;
			result = true;
		}
	}
	CloseHandle( hFile );
	return result;
};
#endif

bool
RanoFileInfo_getFileSize( const char* filepath, uint64_t* ans_size )
{
	bool result = false;
#if defined(Znk_TARGET_WINDOWS)
	result = getFileSize_forWindows( filepath, ans_size );
#elif defined(Znk_TARGET_UNIX)
	result = getFileSize_forUNIX( filepath, ans_size );
#else
#  error "Not Supported"
#endif
	return result;
}
