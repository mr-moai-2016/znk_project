#include <Znk_zip.h>
#include <Znk_stdc.h>
#include <Znk_missing_libc.h>
#include <Znk_dir.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#if defined(Znk_TARGET_UNIX)
#  include <unistd.h>
#  include <utime.h>
#else
#  include <direct.h>
#  include <io.h>
#endif

#include "minizip/unzip.h"

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (8192)
#define MAXFILENAME (256)

#ifdef WIN32
#  define USEWIN32IOAPI
#  include "minizip/iowin32.h"
#endif

/*
  mini unzip, demo of unzip package

  usage :
  Usage : miniunz [-exvlo] file.zip [file_to_extract] [-d extractdir]

  list the file in the zipfile, and print the content of FILE_ID.ZIP or README.TXT
    if it exists
*/


/* change_file_date : change the date/time of a file
    filename : the filename of the file where date/time must be modified
    dosdate : the new date at the MSDos format (4 bytes)
    tmu_date : the SAME new date at the tm_unz format */
static void
change_file_date( const char *filename, uLong dosdate, tm_unz tmu_date )
{
#ifdef WIN32
	HANDLE hFile;
	FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;
	
	hFile = CreateFile(filename,GENERIC_READ | GENERIC_WRITE,
	                    0,NULL,OPEN_EXISTING,0,NULL);
	GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
	DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
	LocalFileTimeToFileTime(&ftLocal,&ftm);
	SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
	CloseHandle(hFile);
#elif defined(unix)
	struct utimbuf ut;
	struct tm newdate;

	newdate.tm_sec = tmu_date.tm_sec;
	newdate.tm_min=tmu_date.tm_min;
	newdate.tm_hour=tmu_date.tm_hour;
	newdate.tm_mday=tmu_date.tm_mday;
	newdate.tm_mon=tmu_date.tm_mon;
	if (tmu_date.tm_year > 1900)
	    newdate.tm_year=tmu_date.tm_year - 1900;
	else
	    newdate.tm_year=tmu_date.tm_year ;
	newdate.tm_isdst=-1;
	
	ut.actime=ut.modtime=mktime(&newdate);
	utime(filename,&ut);
#endif
}


/* mymkdir and change_file_date are not 100 % portable
   As I don't know well Unix, I wait feedback for the unix portion */

static int
mymkdir( const char* dirname )
{
	int ret = 0;
#ifdef WIN32
	ret = mkdir( dirname );
#elif defined(Znk_TARGET_UNIX)
	ret = mkdir( dirname, 0775 );
#endif
	return ret;
}

static int
makedir( const char* newdir )
{
	char *buffer ;
	char *p;
	int  len = (int)strlen(newdir);
	
	if (len <= 0) return 0;
	
	buffer = (char*)malloc(len+1);
	strcpy( buffer, newdir );
	
	if (buffer[len-1] == '/') {
		buffer[len-1] = '\0';
	}
	if (mymkdir(buffer) == 0) {
		free(buffer);
		return 1;
	}
	
	p = buffer+1;
	while (1) {
		char hold;
		
		while(*p && *p != '\\' && *p != '/') p++;
		hold = *p;
		*p = 0;
		if ((mymkdir(buffer) == -1) && (errno == ENOENT)) {
			Znk_printf("couldn't create directory %s\n",buffer);
			free(buffer);
			return 0;
		}
		if (hold == 0) break;
		*p++ = hold;
	}
	free(buffer);
	return 1;
}

static int
do_extract_currentfile( unzFile uf,
		const int* popt_extract_without_path, int* popt_overwrite, const char* password,
		ZnkStr ermsg, ZnkZipProgressFuncT progress_func, void* param, size_t num_of_entry, size_t idx_of_entry )
{
	char  filename_inzip[256];
	char* filename_withoutpath; /* tail */
	char* p;
	int   err=UNZ_OK;
	FILE* fout=NULL;
	void* buf;
	uInt  size_buf;
	char  rep_buf[ 512 ] = "";
	
	unz_file_info file_info;

	err = unzGetCurrentFileInfo( uf,
			&file_info,
			filename_inzip, sizeof(filename_inzip),
			NULL, 0, /* extraField */
			NULL, 0  /* szComment */
	);
	
	if( err != UNZ_OK ){
		ZnkStr_addf( ermsg, "ZnkZip : Error : %d with zipfile in unzGetCurrentFileInfo\n", err );
		return err;
	}
	
	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if( buf == NULL ){
		Znk_printf_e( "Error : allocating memory\n" );
		return UNZ_INTERNALERROR;
	}
	
	p = filename_withoutpath = filename_inzip;
	while( (*p) != '\0' ){
		if (((*p)=='/') || ((*p)=='\\'))
			filename_withoutpath = p+1;
		p++;
	}
	
	if( (*filename_withoutpath)=='\0' ){
		/* tail is empty : thus, this is directory */
		if( (*popt_extract_without_path)==0 ){
			if( progress_func ){
				Znk_snprintf( rep_buf, sizeof(rep_buf), "creating directory: %s", filename_inzip );
				(*progress_func)( rep_buf, num_of_entry, idx_of_entry, param );
			}
			mymkdir(filename_inzip);
		}
	} else {
		const char* write_filename;
		int skip=0;
		
		if( (*popt_extract_without_path)==0 ){
			write_filename = filename_inzip;
		} else {
			write_filename = filename_withoutpath;
		}
		
		err = unzOpenCurrentFilePassword( uf, password );
		if( err != UNZ_OK ){
			ZnkStr_addf( ermsg, "ZnkZip : Error : %d with zipfile in unzOpenCurrentFilePassword.\n", err );
		}
		
#if 0
		if( ((*popt_overwrite)==0) && (err==UNZ_OK) ){
			char rep=0;
			FILE* ftestexist;
			ftestexist = fopen(write_filename,"rb");
			if( ftestexist != NULL ){
				fclose(ftestexist);
				do {
					char answer[128];
					int ret;
					
					Znk_printf("The file %s exists. Overwrite ? [y]es, [n]o, [A]ll: ",write_filename);
					ret = scanf("%1s",answer);
					if (ret != 1) {
						exit(EXIT_FAILURE);
					}
					rep = answer[0] ;
					if ((rep>='a') && (rep<='z'))
						rep -= 0x20;
				} while ((rep!='Y') && (rep!='N') && (rep!='A'));
			}
			
			if (rep == 'N') skip = 1;
			
			if (rep == 'A') *popt_overwrite=1;
		}
#else
		*popt_overwrite = 1; /* all-overwrite */
#endif
		
		if( (skip==0) && (err==UNZ_OK) ){
			fout = fopen( write_filename, "wb" );
			
			/* some zipfile don't contain directory alone before file */
			if(  ( fout==NULL )
			  && ( (*popt_extract_without_path)==0 )
			  && ( filename_withoutpath != (char*)filename_inzip )
			){
				char c = *(filename_withoutpath-1);
				*(filename_withoutpath-1)='\0';
				makedir( write_filename );
				*(filename_withoutpath-1)=c;
				fout = fopen( write_filename, "wb" );
			}
			
			if( fout == NULL ){
				ZnkStr_addf( ermsg, "ZnkZip : Error : opening [%s].\n", write_filename );
			}
		}
		
		if( fout != NULL ){
			//Znk_printf(" extracting: %s\n",write_filename);
			if( progress_func ){
				Znk_snprintf( rep_buf, sizeof(rep_buf), "extracting: %s", write_filename );
				(*progress_func)( rep_buf, num_of_entry, idx_of_entry, param );
			}
			do{
				err = unzReadCurrentFile(uf,buf,size_buf);
				if( err < 0 ){
					ZnkStr_addf( ermsg, "ZnkZip : Error : %d with zipfile in unzReadCurrentFile\n", err );
				    break;
				} else if( err > 0 ){
				 	if( fwrite(buf,err,1,fout) != 1 ){
						ZnkStr_addf( ermsg, "ZnkZip : Error : in writing extracted file\n" );
						err=UNZ_ERRNO;
						break;
				 	}
				}
			} while( err>0 );

			if (fout)
				fclose(fout);
			
			if (err==0){
				change_file_date( write_filename,
						file_info.dosDate,
						file_info.tmu_date );
			}
		}
		
		if( err==UNZ_OK ){
			err = unzCloseCurrentFile (uf);
			if (err!=UNZ_OK){
				ZnkStr_addf( ermsg, "ZnkZip : Error : %d with zipfile in unzCloseCurrentFile\n", err );
			}
		} else {
			unzCloseCurrentFile(uf); /* don't lose the error */
		}
	}
	
	free(buf);
	return err;
}

static bool
do_extract( unzFile uf,
		int opt_extract_without_path, int opt_overwrite, const char* password,
		ZnkStr ermsg, ZnkZipProgressFuncT progress_func, void* param )
{
	uLong i;
	unz_global_info gi;
	int err;
	
	err = unzGetGlobalInfo( uf, &gi );
	if( err != UNZ_OK ){
		ZnkStr_addf( ermsg, "ZnkZip : Error : %d with zipfile in unzGetGlobalInfo \n", err );
		return false;
	}
	
	for( i=0; i<gi.number_entry; ++i ){
		err = do_extract_currentfile( uf,
				&opt_extract_without_path,
				&opt_overwrite, password,
				ermsg, progress_func, param, gi.number_entry, i );
		if( err != UNZ_OK ){
			ZnkStr_addf( ermsg, "ZnkZip : Error : %d do_extract_currentfile.\n", err );
			break;
		}

		if( (i+1)<gi.number_entry ){
			err = unzGoToNextFile( uf );
			if( err != UNZ_OK ){
				ZnkStr_addf( ermsg, "ZnkZip : Error : %d with zipfile in unzGoToNextFile\n", err );
				break;
			}
		}
	}
	
	return (bool)( err == UNZ_OK );
}

Znk_INLINE int
do_extract_onefile( unzFile uf,
		const char* filename, int opt_extract_without_path, int opt_overwrite, const char* password,
		ZnkStr ermsg )
{
	int err = UNZ_OK;

	err = unzLocateFile( uf,
			filename,
			CASESENSITIVITY );
	if( err != UNZ_OK ){
		Znk_printf_e("file %s not found in the zipfile\n",filename);
		return 2;
	}

	err = do_extract_currentfile( uf,
			&opt_extract_without_path,
			&opt_overwrite, password,
			ermsg, NULL, NULL, 1, 0 );

	return err == UNZ_OK ? 0 : 1;
}

static unzFile
openUnzFile( const char* zipfilename )
{
	unzFile uf = NULL;
	if( zipfilename ){
#ifdef USEWIN32IOAPI
		zlib_filefunc_def ffunc;
		fill_win32_filefunc( &ffunc );
		uf = unzOpen2( zipfilename, &ffunc );
#else
		uf = unzOpen( zipfilename );
#endif
	}
	return uf;
}

bool
ZnkZip_extract( const char* zipfilename, ZnkStr ermsg, ZnkZipProgressFuncT progress_func, void* param, const char* dst_dir )
{
	const char* password =NULL;
	const int   opt_do_extract_withoutpath = 0;
	int         opt_overwrite = 0;
	unzFile     uf = openUnzFile( zipfilename );
	bool        result = false;
	ZnkStr      curdir = NULL;

	if( uf == NULL ){
		ZnkStr_addf( ermsg, "ZnkZip : Cannot open [%s]\n", zipfilename );
		goto FUNC_END;
	}

	if( dst_dir ){
		curdir = ZnkStr_new( "" );
		ZnkDir_getCurrentDir( curdir );
		ZnkDir_changeCurrentDir( dst_dir );
	}
	result = do_extract( uf,
			opt_do_extract_withoutpath, opt_overwrite, password,
			ermsg, progress_func, param );
	unzClose( uf );

	if( curdir ){
		ZnkDir_changeCurrentDir( ZnkStr_cstr(curdir) );
		ZnkStr_delete( curdir );
	}

FUNC_END:
	return result;
}

