#include "Znk_dir.h"
#include "Znk_sys_errno.h"
#include "Znk_stdc.h"


/***
 * 処理系依存システムコール(API)
 */
#if   defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#elif defined(Znk_TARGET_UNIX)
/* UNIX(Linux) */
#  include <dirent.h>
#  include <unistd.h>
/* for stat. */
#  include <sys/types.h>
#  include <sys/stat.h>
#else
#  error "not supported"
#endif

/* for errno of system call */
#include <errno.h>
#include <string.h>
#include <stdio.h> /* for perror */

/***
 * Windows の _errno() は mterrno.c と dosmaperr.cの２つに定義されている.
 * 前者では、_MT が定義されていない場合の実体を定義し、後者では _MT が定義
 * されている場合の実体を定義している. (なぜこんな別々のファイルでこのような
 * 定義の仕方をしているのかは不明). _MT が定義されている、つまりマルチスレッド
 * 版のCライブラリである場合、以下のようになっており、実体は_getptd()関数より
 * 取得することになる. この _getptd 関数は tidtable.cに定義されている.
 *
 * int * __cdecl _errno( void) { return ( &(_getptd()->_terrno) ); }
 *
 * この_getptd は TLSからの領域から構造体データを取得しているようだが、
 * ここではこれ以上の深入りはしない.
 * なにはともあれ、Windowsの errno も(マルチスレッド版Cライブラリを使っている
 * 限り) thread-safeになっているようである.
 */

#if defined(Znk_TARGET_WINDOWS)

struct errentry {
	uint32_t oscode_;    /* OS return value */
	uint32_t errnocode_; /* System V error code */
};

static struct errentry errtable[] = {
	{ ERROR_INVALID_FUNCTION,       EINVAL    },  /* 1 */
	{ ERROR_FILE_NOT_FOUND,         ENOENT    },  /* 2 */
	{ ERROR_PATH_NOT_FOUND,         ENOENT    },  /* 3 */
	{ ERROR_TOO_MANY_OPEN_FILES,    EMFILE    },  /* 4 */
	{ ERROR_ACCESS_DENIED,          EACCES    },  /* 5 */
	{ ERROR_INVALID_HANDLE,         EBADF     },  /* 6 */
	{ ERROR_ARENA_TRASHED,          ENOMEM    },  /* 7 */
	{ ERROR_NOT_ENOUGH_MEMORY,      ENOMEM    },  /* 8 */
	{ ERROR_INVALID_BLOCK,          ENOMEM    },  /* 9 */
	{ ERROR_BAD_ENVIRONMENT,        E2BIG     },  /* 10 */
	{ ERROR_BAD_FORMAT,             ENOEXEC   },  /* 11 */
	{ ERROR_INVALID_ACCESS,         EINVAL    },  /* 12 */
	{ ERROR_INVALID_DATA,           EINVAL    },  /* 13 */
	{ ERROR_INVALID_DRIVE,          ENOENT    },  /* 15 */
	{ ERROR_CURRENT_DIRECTORY,      EACCES    },  /* 16 */
	{ ERROR_NOT_SAME_DEVICE,        EXDEV     },  /* 17 */
	{ ERROR_NO_MORE_FILES,          ENOENT    },  /* 18 */
	{ ERROR_LOCK_VIOLATION,         EACCES    },  /* 33 */
	{ ERROR_BAD_NETPATH,            ENOENT    },  /* 53 */
	{ ERROR_NETWORK_ACCESS_DENIED,  EACCES    },  /* 65 */
	{ ERROR_BAD_NET_NAME,           ENOENT    },  /* 67 */
	{ ERROR_FILE_EXISTS,            EEXIST    },  /* 80 */
	{ ERROR_CANNOT_MAKE,            EACCES    },  /* 82 */
	{ ERROR_FAIL_I24,               EACCES    },  /* 83 */
	{ ERROR_INVALID_PARAMETER,      EINVAL    },  /* 87 */
	{ ERROR_NO_PROC_SLOTS,          EAGAIN    },  /* 89 */
	{ ERROR_DRIVE_LOCKED,           EACCES    },  /* 108 */
	{ ERROR_BROKEN_PIPE,            EPIPE     },  /* 109 */
	{ ERROR_DISK_FULL,              ENOSPC    },  /* 112 */
	{ ERROR_INVALID_TARGET_HANDLE,  EBADF     },  /* 114 */
	{ ERROR_INVALID_HANDLE,         EINVAL    },  /* 124 */
	{ ERROR_WAIT_NO_CHILDREN,       ECHILD    },  /* 128 */
	{ ERROR_CHILD_NOT_COMPLETE,     ECHILD    },  /* 129 */
	{ ERROR_DIRECT_ACCESS_HANDLE,   EBADF     },  /* 130 */
	{ ERROR_NEGATIVE_SEEK,          EINVAL    },  /* 131 */
	{ ERROR_SEEK_ON_DEVICE,         EACCES    },  /* 132 */
	{ ERROR_DIR_NOT_EMPTY,          ENOTEMPTY },  /* 145 */
	{ ERROR_NOT_LOCKED,             EACCES    },  /* 158 */
	{ ERROR_BAD_PATHNAME,           ENOENT    },  /* 161 */
	{ ERROR_MAX_THRDS_REACHED,      EAGAIN    },  /* 164 */
	{ ERROR_LOCK_FAILED,            EACCES    },  /* 167 */
	{ ERROR_ALREADY_EXISTS,         EEXIST    },  /* 183 */
	{ ERROR_FILENAME_EXCED_RANGE,   ENOENT    },  /* 206 */
	{ ERROR_NESTING_NOT_ALLOWED,    EAGAIN    },  /* 215 */
	{ ERROR_NOT_ENOUGH_QUOTA,       ENOMEM    }   /* 1816 */
};

/* size of the table */
#define ERRTABLESIZE (sizeof(errtable)/sizeof(errtable[0]))

/* The following two constants must be the minimum and maximum
   values in the (contiguous) range of Exec Failure errors. */
/* WinError 188 */
#define MIN_EXEC_ERROR ERROR_INVALID_STARTING_CODESEG
/* WinError 202 */
#define MAX_EXEC_ERROR ERROR_INFLOOP_IN_RELOC_CHAIN

/* These are the low and high value in the range of errors that are
   access violations */
/* WinError 19 */
#define MIN_EACCES_RANGE ERROR_WRITE_PROTECT
/* WinError 36 */
#define MAX_EACCES_RANGE ERROR_SHARING_BUFFER_EXCEEDED


/***
 * void _dosmaperr(oserrno) - Map function number
 * 
 * Purpose:
 *        This function takes an OS error number, and maps it to the
 *        corresponding errno value (based on UNIX System V values). The
 *        OS error number is stored in _doserrno (and the mapped value is
 *        stored in errno)
 * 
 * Entry:
 *        ULONG oserrno = OS error value
 * 
 * Exit:
 *        sets _doserrno and errno.
 * 
 * Exceptions:
 */
static uint32_t
getUNIXSystemVErrno_fromWinErrCode( uint32_t oserrno )
{
	uint32_t ans_errno;
	int i;
	/* check the table for the OS error code */
	for( i=0; i < ERRTABLESIZE; ++i ){
		if (oserrno == errtable[i].oscode_) {
			ans_errno = errtable[i].errnocode_;
			return ans_errno;
		}
	}
	
	/* The error code wasn't in the table.  We check for a range of */
	/* EACCES errors or exec failure errors (ENOEXEC).  Otherwise   */
	/* EINVAL is returned.                                          */
	if (oserrno >= MIN_EACCES_RANGE && oserrno <= MAX_EACCES_RANGE){
		/***
		 * ERROR_WRITE_PROTECT(=19) から ERROR_SHARING_BUFFER_EXCEEDED(=36) の間であれば、
		 * 全部EACCESとみなす.
		 */
		ans_errno = EACCES;
	} else if (oserrno >= MIN_EXEC_ERROR && oserrno <= MAX_EXEC_ERROR) {
		/***
		 * ERROR_INVALID_STARTING_CODESEG(=188) から ERROR_INFLOOP_IN_RELOC_CHAIN(=202) の間であれば、
		 * 全部ENOEXECとみなす.
		 */
		ans_errno = ENOEXEC;
	} else {
		/***
		 * 上記以外はすべて EINVAL とみなす.
		 */
		ans_errno = EINVAL;
	}
	return ans_errno;
}

#if 0
static void
getLastErrMsg( ZnkStr msg, uint32_t sys_err_code )
{
	LPVOID lpMsgBuf;
	/* エラー表示文字列作成 */
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, (DWORD)sys_err_code,
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );
	ZnkStr_append( msg, (const char*)lpMsgBuf, Znk_NPOS );
	LocalFree(lpMsgBuf);
}
#endif
static void
printLastErrMsg( const char* prefix, uint32_t sys_err_code )
{
	LPVOID lpMsgBuf;
	/* エラー表示文字列作成 */
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, (DWORD)sys_err_code,
			MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL );
	Znk_printf_e( "%s : %s\n", prefix, (const char*)lpMsgBuf );
	LocalFree(lpMsgBuf);
}

static ZnkDirType
getType_byFindFirstFile( const char* path )
{
	WIN32_FIND_DATA FindData;
	ZnkDirType ans = ZnkDirType_e_CannotKnow;
	HANDLE hFind;
	ZnkStr path_str = ZnkStr_new( path );
	while( ZnkStr_last( path_str ) == '/' || ZnkStr_last( path_str ) == '\\' ){
		ZnkStr_cut_back( path_str, 1 );
	}
	//Znk_printf_e( "ZnkDir_getType(use FindFirstFile) : modified path=[%s]\n", ZnkStr_cstr(path_str) );
	if( ZnkStr_empty( path_str ) ){
		/* 無効なパス. つまり存在しないとみなせる. */
		ZnkStr_delete( path_str );
		return ZnkDirType_e_NotFound;
	}
	hFind = FindFirstFile( ZnkStr_cstr(path_str), &FindData);
	ZnkStr_delete( path_str );

	if( hFind == INVALID_HANDLE_VALUE ){
		/***
		 * 想定外の状況...
		 * あるいは、GetFileAttributes と FindFirstFileが呼ばれるちょうどその間の瞬間に
		 * (非常に奇跡的なタイミングだが)該当ファイルが削除されたなどの可能性はある.
		 */
		uint32_t win_errcode = (uint32_t)GetLastError();
		uint32_t sys_errno = getUNIXSystemVErrno_fromWinErrCode( win_errcode );
		Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : hFind is INVALID_HANDLE_VALUE. Very rare case.\n" );
		switch( sys_errno ){
		case ENOENT:
			/* file/directory does not exist. */
			Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : Recognize NotFound. win_errcode=[%I32u]\n", win_errcode );
			printLastErrMsg( "  OSErrMsg=", win_errcode );
			return ZnkDirType_e_NotFound;
		default:
		{
			/***
			 * GetFileAttributesとFindFirstFileの両方でも取得不可で
			 * かつ存在しているというなんらかの状態にある.
			 */
			ZnkSysErrnoInfo* info = ZnkSysErrno_getInfo( sys_errno );
			Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : sys_errno=[%s](%s)\n",
					info->sys_errno_key_, info->sys_errno_msg_ );
			break;
		}
		}
		Znk_printf_e( "ZnkDir_getType : getType_byFindFirstFile : Recognize CannotKnow. win_errcode=[%I32u]\n", win_errcode );
		printLastErrMsg( "  OSErrMsg=", win_errcode );
		return ZnkDirType_e_CannotKnow;
	}
	FindClose(hFind);

	if( FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
		ans = ZnkDirType_e_Directory;
	} else {
		ans = ZnkDirType_e_File;
	}
	return ans;
}
#endif


#if defined(Znk_TARGET_UNIX)
Znk_INLINE ZnkDirType
getStatErrorType( uint32_t syscall_err )
{
	ZnkDirType dir_type = ZnkDirType_e_CannotKnow; 
	bool is_irregular = true;
	/***
	 * statがエラーとなった場合、-1 を返し、
	 * errnoには以下の値が設定される.
	 *
	 * EACCES:
	 *   いわゆる Permition denied エラーの正体はこれである.
	 *   ただしこれはパスの最後に到達する前のオブジェクトにおけるサーチパーミッション
	 *   に関するものである.
	 *
	 *   例えば、 A/B/C と言う形でパスが指定されたとき、最後のCより前のもの
	 *   つまりこの場合、A,Bのうちいずれかのサーチパーミッション(Directoryに
	 *   付加された x パーミッションのこと)のフラグが立っていない場合、
	 *   それより深部の探索は不可能になり、最後のCまでたどり着けない.
	 *   この場合、EACCESが発生する. このエラーが発生した場合、Cの存在性を
	 *   確認するのは不可能であるということになる(存在するかもしれないししない
	 *   かもしれないが、現在のユーザの権限ではこれ以上確認するすべがない).
	 *
	 * EBADF:
	 *   Bad fd である、と記載されている.
	 *   おそらくは、A/B/Cとあった場合にこれらのうちのいずれかのfdが壊れていると
	 *   いうことだろう. これはファイルシステムが一部破壊されているということかも
	 *   しれないが、パス自体は存在している可能性がある. この場合は CannotKnow としてよいだろう.
	 *
	 * EFAULT:
	 *   途中でSegmentation Faltが発生したということらしい.
	 *   パス文字列がNULL終端していないかあるいは不正なポインタ値であった場合など
	 *   に発生するものと思われる. いずれにせよ不正なパスということなので
	 *   NotFoundとしてよいと思われる.
	 *
	 * ELOOP:
	 *   パスを辿っているときに途中でシンボリックリンクに出くわしたとする.
	 *   それが最後尾なら、そのシンボリックリンク自体をファイルとみなして情報を取得
	 *   という考え方もできるが、そうではないとき、そこからさらに深部へ行くなどするには
	 *   まずそのシンボリックリンクがさす先を解決せねばならない. しかし、シンボリックリンクの
	 *   先がさらにシンボリックリンクである場合もあり、そしてそれがあまりに多いか、
	 *   あるいはループしている場合は、エラーとみなして中断する.
	 *   この場合、与えられたパスはある種の存在していない実体を示しているともとれるので
	 *   NotFoundとしてよいと思われる.
	 *
	 * ENAMETOOLONG:
	 *   パス文字列が長すぎるということだろう.
	 *   この場合はNotFoundとしてよいだろう.
	 *
	 * ENOENT:
	 *   いわゆる No such file or directory エラーの正体はこれである.
	 *   もちろんNotFoundそのものである.
	 *
	 * ENOMEM:
	 *   カーネルにおけるメモリ確保エラー.
	 *   このような状況が発生する場合はむしろプロセスを終了すべきかと思われるが、
	 *   ここでは CannotKnowを返す.
	 *
	 * ENOTDIR:
	 *   No Directory エラーと呼ばれる.
	 *   例えば、 A/B/C と言う形でパスが指定されたとき、途中のBがディレクトリでは
	 *   なくファイルであった場合、これは論理矛盾であり、このエラーが発行される.
	 *   要するに、指定されたパスの C は存在しないということなので、NotFoundである.
	 *
	 * EOVERFLOW:
	 *   ファイルサイズ取得に関して、あまりに大きなファイルサイズであったため、
	 *   その取得に失敗した(サイズ格納用変数が許容量を超え、オーバーフローを起こした)
	 *   ことを示す. ファイル自体は見つけているので、これは存在はしている.
	 */
	switch( syscall_err ){
	case EACCES:
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	case EBADF:
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	case EFAULT:
		dir_type = ZnkDirType_e_NotFound;
		break;
	case ELOOP:
		dir_type = ZnkDirType_e_NotFound;
		break;
	case ENAMETOOLONG:
		dir_type = ZnkDirType_e_NotFound;
		break;
	case ENOENT:
		dir_type = ZnkDirType_e_NotFound;
		is_irregular = false;
		break;
	case ENOMEM:
		/* exit すべきか ? */
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	case ENOTDIR:
		dir_type = ZnkDirType_e_NotFound;
		is_irregular = false;
		break;
	case EOVERFLOW:
		/***
		 * ファイルとして存在することは確か.
		 */
		dir_type = ZnkDirType_e_File;
		is_irregular = false;
		break;
	default:
		dir_type = ZnkDirType_e_CannotKnow;
		break;
	}
	if( is_irregular ){
		perror( "ZnkDir_getType" );
	}
	return dir_type;
}
#endif


ZnkDirType
ZnkDir_getType( const char* path )
{

#if   defined(Znk_TARGET_WINDOWS)

/***
 * INVALID_FILE_ATTRIBUTES:
 * これは 0xFFFFFFFF として代用されることもある.
 * また処理系によっては定義されていないこともあるため、ここで確実に定義しておく.
 */
#ifndef   INVALID_FILE_ATTRIBUTES
#  define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

	/* Windows */
	uint32_t attr = GetFileAttributes( path );
	//Znk_printf_e( "ZnkDir_getType(use GetFileAttributes) : path=[%s]\n", path );
	if( attr == INVALID_FILE_ATTRIBUTES ){
		/***
		 * GetLastError:
		 * Retrieves the calling thread's last-error code value.
		 * The last-error code is maintained on a per-thread basis.
		 * Multiple threads do not overwrite each other's last-error code.;
		 */
		uint32_t win_errcode = (uint32_t)GetLastError();
		uint32_t sys_errno = getUNIXSystemVErrno_fromWinErrCode( win_errcode );
		switch( sys_errno ){
		case ENOENT:
			/* file/directory does not exist. */
			//Znk_printf_e( "ZnkDir_getType : Recognize NotFound. win_errcode=[%I32u]\n", win_errcode );
			//printLastErrMsg( "  OSErrMsg=", win_errcode );
			return ZnkDirType_e_NotFound;
		default:
		{
			/***
			 * さらに win_errcode が ERROR_SHARING_VIOLATION の場合、
			 * FindFirstFileによるダブルチェックを行う.
			 * 存在性だけならば ERROR_SHARING_VIOLATION の時点で確定はしているが、
			 * これがFileなのかDirectoryなのかがわからない.
			 *
			 * pathがファイルならば、最後がDSPである可能性は除外できる.
			 * このような場合、GetFileAttributes は優先的にERROR_INVALID_NAME エラーとする.
			 * それがたとえ、ERROR_SHARING_VIOLATION を発生させるようなファイルでも
			 * まずは ERROR_INVALID_NAME が優先される.
			 * pathがディレクトリならば、最後がDSPである可能性は排除できない.
			 * 要するにここでは、最後がDSPである場合はまずそれをカットしなければならないだろう.
			 */
			if( win_errcode == ERROR_SHARING_VIOLATION ){
				return getType_byFindFirstFile( path );
			} 
			break;
		}
		}
		//C1_log( "ZnkDir_getType : Recognize CannotKnow. win_errcode=[%u]\n", win_errcode );
		//printLastErrMsg( "  OSErrMsg=", win_errcode );
		return ZnkDirType_e_CannotKnow;
	}
	if( attr & FILE_ATTRIBUTE_DIRECTORY ){
		/***
		 * 隠しファイル属性を持つディレクトリ等、必ずしも attr == FILE_ATTRIBUTE_DIRECTORY と
		 * ならない場合がある. そのため、ここでは必ずFILE_ATTRIBUTE_DIRECTORY ビットが立っている
		 * か否かで判定しなければならない.
		 */
		return ZnkDirType_e_Directory;
	}
	/* 上記以外はファイルとみなす */
	return ZnkDirType_e_File;

#elif defined(Znk_TARGET_UNIX)
	/* Unix */
	ZnkDirType dir_type = ZnkDirType_e_CannotKnow;
	struct stat statbuf;
	if( stat(path, &statbuf) == -1 ){
		/***
		 * errnoはthread localであるため、あるスレッドのerrno書き込みが
		 * 他スレッドのerrnoの値に影響することはない.
		 */
		uint32_t syscall_err = errno; /* look global errno */
		/***
		 * このとき、statbuf.st_modeの値は正しくないため、参照する意味はない.
		 * (実際、Permition Denied 系のエラーの場合など、正しく設定されてはいない.)
		 */
		dir_type = getStatErrorType( syscall_err );
		return dir_type;
	}

	if( statbuf.st_mode & S_IFMT & S_IFDIR ){
		/* this is directory */
		return ZnkDirType_e_Directory;
	}
	return ZnkDirType_e_File;

#else
#  error "not supported"
#endif
}


ZnkDirIdentity
ZnkDir_getIdentity( const char* path1, const char* path2 )
{
#if   defined(Znk_TARGET_WINDOWS)
	ZnkDirIdentity identity = ZnkDirIdentity_e_CannotKnow;
	ZnkDirType dir_type1 = ZnkDir_getType( path1 );
	ZnkDirType dir_type2 = ZnkDir_getType( path2 );

	/***
	 * まず、如何なる場合もどちらか一方または両方が NotFoundの場合は
	 * NotFoundを返す.
	 */
	if( dir_type1 == ZnkDirType_e_NotFound || dir_type2 == ZnkDirType_e_NotFound ){
		return ZnkDirIdentity_e_NotFound;
	}
	/***
	 * 次に、如何なる場合もどちらか一方または両方が CannotKnowの場合は
	 * CannotKnowを返す.
	 */
	if( dir_type1 == ZnkDirType_e_CannotKnow || dir_type2 == ZnkDirType_e_CannotKnow ){
		return ZnkDirIdentity_e_CannotKnow;
	}
	/***
	 * 以降はいずれの場合もDirectoryまたはFile以外ありえない.
	 */
	if( path1 == path2 || strcmp( path1, path2 ) == 0 ){
		/***
		 * 文字列のレベルで等しい. Sameと判断する.
		 */
		return ZnkDirIdentity_e_Same;
	}

	{
		bool is_directory1 = (bool)( dir_type1 == ZnkDirType_e_Directory );
		bool is_directory2 = (bool)( dir_type2 == ZnkDirType_e_Directory );
		DWORD dwFlagsAndAttributes1 = FILE_ATTRIBUTE_NORMAL;
		DWORD dwFlagsAndAttributes2 = FILE_ATTRIBUTE_NORMAL;
		HANDLE hFile1 = NULL;
		HANDLE hFile2 = NULL;
		BY_HANDLE_FILE_INFORMATION FileInfo1;
		BY_HANDLE_FILE_INFORMATION FileInfo2;
	
		/***
		 * Directory Handler をCreateFileにて取得するには
		 * FILE_FLAG_BACKUP_SEMANTICS をつけておく必要がある.
		 * これによって得られた HANDLE は、GetFileInformationByHandle の
		 * 引数として渡すこともできる.
		 */
		if( is_directory1 ){ dwFlagsAndAttributes1 |= FILE_FLAG_BACKUP_SEMANTICS; }
		if( is_directory2 ){ dwFlagsAndAttributes2 |= FILE_FLAG_BACKUP_SEMANTICS; }
	
		hFile1 = CreateFile( path1, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, dwFlagsAndAttributes1, NULL);
	
		hFile2 = CreateFile( path2, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, dwFlagsAndAttributes2, NULL);
	
		if( GetFileInformationByHandle( hFile1, &FileInfo1 ) == 0 ){
			/* 失敗 */
			identity = ( dir_type1 != dir_type2 ) ?
				ZnkDirIdentity_e_DifferAndCannotKnowDevice : ZnkDirIdentity_e_CannotKnow;
			goto FUNC_END;
		}
		if( GetFileInformationByHandle( hFile2, &FileInfo2 ) == 0 ){
			/* 失敗 */
			identity = ( dir_type1 != dir_type2 ) ?
				ZnkDirIdentity_e_DifferAndCannotKnowDevice : ZnkDirIdentity_e_CannotKnow;
			goto FUNC_END;
		}

		if( FileInfo1.dwVolumeSerialNumber == FileInfo2.dwVolumeSerialNumber ){
			if(  FileInfo1.nFileIndexHigh == FileInfo2.nFileIndexHigh
			  && FileInfo1.nFileIndexLow  == FileInfo2.nFileIndexLow  )
			{
				identity = ZnkDirIdentity_e_Same;
			} else {
				identity = ZnkDirIdentity_e_DifferAndSameDevice;
			}
		} else {
			identity = ZnkDirIdentity_e_DifferAndDifferDevice;
		}

FUNC_END:
		CloseHandle(hFile1);
		CloseHandle(hFile2);
	}

	return identity;

#elif defined(Znk_TARGET_UNIX)

	/* Unix */
	ZnkDirType dir_type1 = ZnkDirType_e_CannotKnow;
	ZnkDirType dir_type2 = ZnkDirType_e_CannotKnow;
	struct stat statbuf1;
	struct stat statbuf2;
	bool statbuf_success1 = true;
	bool statbuf_success2 = true;

	if( stat(path1, &statbuf1) == -1 ){
		uint32_t syscall_err = errno; /* look global errno */
		dir_type1 = getStatErrorType( syscall_err );
		statbuf_success1 = false;
	}
	if( stat(path2, &statbuf2) == -1 ){
		uint32_t syscall_err = errno; /* look global errno */
		dir_type2 = getStatErrorType( syscall_err );
		statbuf_success2 = false;
	}

	if( statbuf_success1 && statbuf_success2 ){
		if( statbuf1.st_dev == statbuf2.st_dev ){
			if( statbuf1.st_ino == statbuf2.st_ino ){
				return ZnkDirIdentity_e_Same;
			} else {
				return ZnkDirIdentity_e_DifferAndSameDevice;
			}
		} else {
			return ZnkDirIdentity_e_DifferAndDifferDevice;
		}
	} else {
		/***
		 * まず、如何なる場合もどちらか一方または両方が NotFoundの場合は
		 * NotFoundを返す.
		 */
		if( dir_type1 == ZnkDirType_e_NotFound || dir_type2 == ZnkDirType_e_NotFound ){
			return ZnkDirIdentity_e_NotFound;
		}
		/***
		 * 次に、如何なる場合もどちらか一方または両方が CannotKnowの場合は
		 * CannotKnowを返す.
		 */
		if( dir_type1 == ZnkDirType_e_CannotKnow || dir_type2 == ZnkDirType_e_CannotKnow ){
			return ZnkDirIdentity_e_CannotKnow;
		}
		/***
		 * 以降はいずれの場合もDirectoryまたはFile以外ありえない.
		 */
		if( path1 == path2 || strcmp( path1, path2 ) == 0 ){
			/***
			 * 文字列のレベルで等しい. Sameと判断する.
			 */
			return ZnkDirIdentity_e_Same;
		}
	}
	return ( dir_type1 != dir_type2 ) ?
		ZnkDirIdentity_e_DifferAndCannotKnowDevice : ZnkDirIdentity_e_CannotKnow;

#else
#  error "not supported"
#endif
}

