#include <Znk_dir.h>
#include <Znk_s_base.h>
#include <Znk_stdc.h>

#if   defined(Znk_TARGET_WINDOWS)
#  include <windows.h>
#else
#  include <dirent.h>
#  include <unistd.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <errno.h>
#endif

typedef struct{
#if   defined(Znk_TARGET_WINDOWS)
/* Win32 */
	WIN32_FIND_DATA wfd;
#else
/* UNIX */
	struct dirent *dentp;
#endif
}ZnkDirEntry;

struct ZnkDirInfoImpl {
#if   defined(Znk_TARGET_WINDOWS)
	/* Win32 */
	HANDLE hnd;
	bool first_skip_flag;
#else
	/* UNIX */
	DIR *dp;
#endif
	ZnkDirEntry dir_entry_;
	bool is_skip_dot_and_dot_dot_;
	char dir_name_[ 512+8 ];
};


ZnkDirId
ZnkDir_openDirEx( const char *dir_name, size_t dir_name_leng, bool is_skip_dot_and_dot_dot )
{
	struct ZnkDirInfoImpl draft;
	ZnkDirId id = NULL;

	if( dir_name_leng == Znk_NPOS ){
		dir_name_leng = Znk_strlen( dir_name );
	}

#if   defined(Znk_TARGET_WINDOWS)
	/* Win32 */
	{
		char last_ch = dir_name[ dir_name_leng-1 ];
		ZnkS_copy( draft.dir_name_, sizeof(draft.dir_name_), dir_name, dir_name_leng );
		ZnkS_concat( draft.dir_name_, sizeof(draft.dir_name_),
				( last_ch == '/' || last_ch == '\\' ) ? "*" : "/*" );
		draft.hnd = FindFirstFile( draft.dir_name_ , &(draft.dir_entry_.wfd) );
	}

	if( draft.hnd == INVALID_HANDLE_VALUE ){ return NULL; }
	draft.first_skip_flag = true;

#else
	/* UNIX */
	draft.dp = opendir(dir_name);
	if( draft.dp == NULL ){ return NULL; }

#endif

	draft.is_skip_dot_and_dot_dot_ = is_skip_dot_and_dot_dot;
	id = (ZnkDirId)Znk_malloc( sizeof(struct ZnkDirInfoImpl) );
	if(id == NULL){ return NULL; }
	*id = draft;
	return id;
}

/***
 * ZnkDirEntryからファイル名を取得する.
 */
Znk_INLINE const char*
getDName( const ZnkDirEntry* dir_entry )
{
#if   defined(Znk_TARGET_WINDOWS)
	/* Win32 */
	return dir_entry->wfd.cFileName;
#else
	/* UNIX */
	return dir_entry->dentp->d_name;
#endif
}

Znk_INLINE const ZnkDirEntry*
getNextDirEntry( ZnkDirId id )
{
	if( id == NULL ){ return NULL; }

#if   defined(Znk_TARGET_WINDOWS)
	/* Win32 (注：最初の一回目はFindNextFileを呼ばない) */
	if( id->first_skip_flag ){
		id->first_skip_flag = false;
		return &id->dir_entry_;
	}
	if( !FindNextFile(id->hnd, &(id->dir_entry_.wfd)) ){
		return NULL;
	}
#else
	/* UNIX */
	id->dir_entry_.dentp = readdir( id->dp );
	if( !id->dir_entry_.dentp ){
		return NULL;
	}
#endif

	/* Success */
	return &id->dir_entry_;
}

Znk_DECLARE_HANDLE( ZnkDirEntrySysDataPtr );

Znk_INLINE const char*
getNextEx( ZnkDirId id, ZnkDirEntrySysDataPtr* dirent_sysptr_ans )
{
	const ZnkDirEntry* dir_entry = getNextDirEntry( id );
	if( dir_entry ){
		const char* name = getDName( dir_entry );
		if( id->is_skip_dot_and_dot_dot_ ){
			while( true ){
				/***
				 * ここは素直に strcmp を使って . or .. と等しいか否かを調べてももよいが、
				 * 少しでも高速化するため、文字単位で比較している.
				 */
				if( name[0] == '.' ){
					if(  name[1] == '\0' || ( name[1] == '.' && name[2] == '\0' ) ){
						/* 次の名前を取得 */
						dir_entry = getNextDirEntry( id );
						if( dir_entry == NULL ){ return NULL; }
						name = getDName( dir_entry );
						continue;
					} else {
						/***
						 * なんらかの . で始まる通常ファイルと思われる.
						 * 当然この場合はskipしない.
						 */
					}
				}
				break;
			}
		} 

		/***
		 * この時点で最終的な dir_entry と name が確定.
		 */
#if   defined(Znk_TARGET_WINDOWS)
		if( dirent_sysptr_ans ){ *dirent_sysptr_ans = (ZnkDirEntrySysDataPtr) &dir_entry->wfd; }
#else
		if( dirent_sysptr_ans ){ *dirent_sysptr_ans = (ZnkDirEntrySysDataPtr) dir_entry->dentp; }
#endif

		return name;
	}
	return NULL;
}

const char*
ZnkDir_readDir( ZnkDirId id )
{
	return getNextEx( id, NULL );
}

void
ZnkDir_closeDir( ZnkDirId id )
{
	if( id == NULL ){ return; }

#if   defined(Znk_TARGET_WINDOWS)
	/* Win32 */
	if( id->hnd != INVALID_HANDLE_VALUE ){
		FindClose( id->hnd );
	}
#else
	/* UNIX */
	if( id->dp != NULL ){
		closedir( id->dp );
	}
#endif

	/* メモリ解放 */
	Znk_free( id );
}

void
ZnkDir_rewindDir( ZnkDirId id )
{
	if( id == NULL ){ return; }

#if   defined(Znk_TARGET_WINDOWS)
	/* Win32 */
	if( id->hnd != INVALID_HANDLE_VALUE ){
		FindClose( id->hnd );
		id->hnd = FindFirstFile( id->dir_name_, &(id->dir_entry_.wfd) );
	}
#else
	/* UNIX */
	rewinddir( id->dp );
#endif
}

