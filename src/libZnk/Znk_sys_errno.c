#include "Znk_sys_errno.h"
#include <errno.h>

/***
 * POSIX 標準の errno の値としては、次のようなものがある.
 *
 * E2BIG:
 *   関数に渡される引数リストが長すぎました。
 * EACCESS:
 *   アクセスが拒否されました。このプログラムを実行しているユーザーは、ファイルやディレクトリーなどへのアクセス権を持っていません。
 * EAGAIN:
 *   要求されたリソースが一時的に利用不能です。同じ操作を後で再度行えば、成功するかもしれません。
 * EBADF:
 *   関数が、不適切なファイル記述子を使おうとしました.
 *   例えば、開いているファイルを参照していない、あるいは、
 *   読み取り専用として開かれているファイルに書き込むために使われた、など.
 * EBUSY:
 *   要求されたリソースは利用できません。
 *   例えば、別のアプリケーションが読んでいる最中にディレクトリーを削除しようとした、など。
 *   EBUSY と EAGAIN との微妙な違いに注意してください。当然ですが、読み取りを行っているプログラムが終了した後なら、
 *   そのディレクトリーを削除することができます。
 * ECHILD:
 *   wait() 関数、または waitpid() 関数が、子プロセスが終了するのを待とうとしましたが、
 *   すべての子プロセスは既に終了していました。
 * EDEADLK:
 *   リクエストが継続されると、リソース・デッドロックが発生します。
 *   これは、マルチスレッドのコードで起こるようなデッドロックではないことに注意してください。
 *   errno とその仲間では、とてもそうしたデッドロックは追跡できません。
 * EDOM:
 *   入力引数が、その数学関数の対象外です。
 * EEXIST:
 *   ファイルが既に存在し、それが問題です。
 *   例えば、パスを付けて mkdir() を呼ぶと、既存のファイルやディレクトリーに
 *   名前を付けることになってしまうような場合です。
 * EFAULT:
 *   関数の引数の 1 つが、無効なアドレスを参照しています。大部分の実装では、
 *   これは検出できません (プログラムは SIGSEGFAULT シグナルを受け取り、終了します)。
 * EFBIG:
 *   リクエストが、実装で定義された最大のファイルサイズを超えてファイルを拡張しようとしました。
 *   これは通常は約 2 GB ですが、最近のほとんどのファイルシステムはずっと大きなファイルをサポートしており、
 *   場合によると read()/write() や lseek() などの関数の 64 ビット版が必要になります。
 * EINTR:
 *   関数がシグナルに割り込まれ、割り込みはプログラムのシグナル・ハンドラーによってキャッチされ、
 *   そしてシグナル・ハンドラーの戻りは正常でした。
 * EINVAL:
 *   関数に対して無効な引数を渡しました。
 * EIO:
 *   I/O エラーが発生しました。これは通常、ハードウェアの問題への反応として生成されます。
 * EISDIR:
 *   ファイル引数を要求する関数を、ディレクトリー引数を付けて呼びました。
 * ENFILE:
 *   このプロセスで既に開かれているファイルが多すぎます。
 *   各プロセスは OPEN_MAX ファイル記述子を持っており、今ユーザーは (OPEN_MAX + 1) ファイルを開こうとしています。
 *   ファイル記述子には、ソケットのようなものもの含まれることに注意してください。
 * ENLINK:
 *   この関数を呼ぶと、そのファイルは LINK_MAX 以上のリンクを持つことになります。
 * ENAMETOOLONG:
 *   PATH_MAX よりも長いパス名を作ってしまいました。
 *   あるいは、NAME_MAX よりも長いファイル名やディレクトリー名を作ってしまいました。
 * ENFILE:
 *   このシステムで同時に開いているファイルが多すぎます。
 *   これは一時的な状態のはずであり、最近のシステムでは、通常は起こらないはずです。
 * ENODEV:
 *   そのようなデバイスはありません。
 *   あるいは、指定されたデバイスに対して不適切なことを試みようとしています
 *   (例えば、大昔のライン・プリンターから読み取ろうとしてはいけません)。
 * ENOENT:
 *   そのようなファイルは見つかりません。あるいは、指定されたパス名が存在しません。
 * ENOEXEC:
 *   実行可能ではないファイルを実行しようとしました。
 * ENOLCK:
 *   ロックが利用できません。システム全体でのファイル制限、あるいはレコード・ロックに達しました。
 * ENOMEM:
 *   システムのメモリーが足りなくなりました。
 *   従来から、アプリケーションは (そして OS そのものも)、これをうまく扱えません。
 *   そのために、特にディスク上のスワップ空間のサイズを動的に増加できないシステムでは、
 *   使いそうな RAM 量よりも多くの RAM が必要なのです。
 * ENOSPC:
 *   デバイス上に空間が残っていません。ユーザーが、既に一杯のデバイスに対して書き込もうとした、
 *   あるいはファイルを作成しようとしました。これも同じく、アプリケーションでも OS でもうまく扱えないエラーです。
 * ENOSYS:
 *   システムは、この関数をサポートしません。例えば、ジョブ・コントロールを持たないシステムに対して
 *   setpgid() を呼ぶと、ENOSYS エラーが発生します。
 * ENOTDIR:
 *   指定されたパス名はディレクトリーである必要がありますが、そうなっていません。
 *   これは EISDIR エラーの逆です。
 * ENOTEMPTY:
 *   指定されたディレクトリーが空ではありません. 空である必要があります。
 *   空のディレクトリーでも、「.」 や「..」 などが含まれていることに注意してください。
 * ENOTTY:
 *   その操作をサポートしていないファイル、あるいは特別なファイルに対して
 *   I/O コントロール操作をしようとしました。
 *   例えば、ディレクトリーに対してボーレートを設定しようとしてはいけません。
 * ENXIO:
 *   存在しないデバイスの特別なファイルに対して I/O リクエストを試みました。
 * EPERM:
 *   この操作は許可されていません。
 *   ユーザーは、指定されたリソースへのアクセス権を持っていません。
 * EPIPE:
 *   もはや存在しないパイプから読み取ろうとしました、あるいはパイプに書き込もうとしました。
 *   パイプ・チェーンの中のプログラムの 1 つが、そのストリーム部分を閉じてしまいました (例えば終了によって)。
 * ERANGE:
 *   関数が呼ばれましたが、戻り値は戻り型で表現するには大きすぎます。
 *   例えば、ある関数が unsigned char の値を返したものの、結果を 256 以上 (あるいは -1 以下)と
 *   計算したとすると、errno は ERANGE に設定され、その関数は無関係な値を返します。
 *   こうした場合には、入力データが適切かどうかを調べるか、あるいは関数を呼んだら必ず errno を調べることが重要です。
 * EROFS:
 *   読み取り専用のファイルシステム (あるいは読み取り専用モードでマウントされたファイルシステム) に
 *   保存されたファイル、あるいはディレクトリーを変更しようとしました。
 * ESPIPE:
 *   パイプ、あるいは FIFO (First In, First Out) に対してシークしようとしました。
 * ESRCH:
 *   無効なプロセス ID、あるいはプロセス・グループを指定しました。
 * EXDEV:
 *   デバイスにまたがってリンクを移動する操作を行おうとしました。
 *   例えば、UNIX ファイルシステムでは、ファイルシステム間でファイルを移動することを許しません
 *   (ファイルをコピーし、その後でオリジナルを削除する必要があります)。
 */

static ZnkSysErrnoInfo st_sys_errno_info_table[] = {

	/* POSIX */
	{ 0,        "0",       "Unknown error", }, /* 0 ( not POSIX ) */
	{ EPERM,    "EPERM",   "Operation not permitted" }, /* EPERM=1 */
	{ ENOENT,   "ENOENT",  "No such file or directory" },
	{ ESRCH,    "ESRCH",   "No such process" },
	{ EINTR,    "EINTR",   "Interrupted system call" },
	{ EIO,      "EIO",     "Input/output error" },
	{ ENXIO,    "ENXIO",   "No such device or address" },
	{ E2BIG,    "E2BIG",   "Argument list too long" },
	{ ENOEXEC,  "ENOEXEC", "Exec format error" },
	{ EBADF,    "EBADF",   "Bad file descriptor" },
	{ ECHILD,   "ECHILD",  "No child processes" },
	{ EAGAIN,   "EAGAIN",  "Resource temporarily unavailable" },
	{ ENOMEM,   "ENOMEM",  "Cannot allocate memory" },
	{ EACCES,   "EACCES",  "Permission denied" },
	{ EFAULT,   "EFAULT",  "Bad address" },
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	/* ENOTBLK is not defined on VC and MinGW, but number 15 is reserved as Unknown. */
	{ 15,       "ENOTBLK", "Unknown error" },
#elif defined(ENOTBLK)
	/* not POSIX but Cygwin, Linux, dmc, bcc55 can use. */
	{ ENOTBLK,  "ENOTBLK", "Block device required" },
#endif
	{ EBUSY,    "EBUSY",   "Device or resource busy" },
	{ EEXIST,   "EEXIST",  "File exists" },
	{ EXDEV,    "EXDEV",   "Invalid cross-device link" },
	{ ENODEV,   "ENODEV",  "No such device" },
	{ ENOTDIR,  "ENOTDIR", "Not a directory" },
	{ EISDIR,   "EISDIR",  "Is a directory" },
	{ EINVAL,   "EINVAL",  "Invalid argument" },
	{ ENFILE,   "ENFILE",  "Too many open files in system" },
	{ EMFILE,   "EMFILE",  "Too many open files" },
	{ ENOTTY,   "ENOTTY",  "Inappropriate ioctl for device" },
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	/* ETXTBSY is not defined on VC and MinGW, but number 26 is reserved as Unknown. */
	{ 26,       "ETXTBSY", "Unknown error" },
#elif defined(ETXTBSY)
	/* not POSIX but Cygwin, Linux, dmc, bcc55 can use. */
	{ ETXTBSY,  "ETXTBSY", "Text file busy" },
#endif
	{ EFBIG,    "EFBIG",   "File too large" },
	{ ENOSPC,   "ENOSPC",  "No space left on device" },
	{ ESPIPE,   "ESPIPE",  "Illegal seek" },
	{ EROFS,    "EROFS",   "Read-only file system" },
	{ EMLINK,   "EMLINK",  "Too many links" },
	{ EPIPE,    "EPIPE",   "Broken pipe" },
	{ EDOM,     "EDOM",    "Numerical argument out of domain" },
	{ ERANGE,   "ERANGE",  "Numerical result out of range" }, /* 34 */

	/*
	 * ここまではほぼすべての環境で定義されているとみてよい.
	 ***/

	/***
	 * 以下は EUCLEAN, UNKNOWN を除いては Windows以外にも値は存在するが、その実値が異なる.
	 * (これらはLinuxにはすべてあるが、下記でリストしているのでここでは含めない)
	 * 以下が有効なのは VCとMinGWのみである.
	 */
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) || defined(__DMC__)
	/* EUCLEAN is not defined on VC and MinGW, but number 35 and 37 is reserved as Unknown. */
	{ 35,           "EUCLEAN",      "Unknown error" },  /* not POSIX. */
	{ EDEADLK,      "EDEADLK",      "Resource deadlock avoided" }, /* not bcc55 */
	{ 37,           "UNKNOWN",      "Unknown error" },  /* MS only */
	{ ENAMETOOLONG, "ENAMETOOLONG", "Filename too long" },
	{ ENOLCK,       "ENOLCK",       "No locks available" }, /* not bcc55 */
	{ ENOSYS,       "ENOSYS",       "Function not implemented" }, /* not bcc55 */
	{ ENOTEMPTY,    "ENOTEMPTY",    "Directory not empty" },
	{ EILSEQ,       "EILSEQ",       "Illegal byte sequence" }, /* not bcc55 */
#endif
#if defined(__BORLANDC__)
	{ EDEADLOCK,    "EDEADLOCK",    "Locking violation" },
	{ EUCLEAN,      "EUCLEAN",      "?" },  /* not POSIX. */
	{ ENAMETOOLONG, "ENAMETOOLONG", "Filename too long" },
	{ ENOTEMPTY,    "ENOTEMPTY",    "Directory not empty" },
	{ EINVFNC,      "EINVFNC",      "Invalid function number" },
	{ ENOPATH,      "ENOPATH",      "Path not found" },
	{ ECONTR ,      "ECONTR",       "Memory blocks destroyed" },
	{ EINVMEM,      "EINVMEM",      "Invalid memory block address" },
	{ EINVENV,      "EINVENV",      "Invalid environment" },
	{ EINVFMT,      "EINVFMT",      "Invalid format" },
	{ EINVACC,      "EINVACC",      "Invalid access code" },
	{ EINVDAT,      "EINVDAT",      "Invalid data" },
	{ ECURDIR,      "ECURDIR",      "Attempt to remove CurDir" },
	{ ENOTSAM,      "ENOTSAM",      "Not same device" },
	{ ENMFILE,      "ENMFILE",      "No more files" },
#endif

	/***
	 * UNIX(cygwin or linux).
	 * linux の場合、errno.h に極一部、
	 * そして大部分は、34以下(POSIX?)は asm-generic/errno-base.h,
	 * 35以降は asm-generic/errno.h に定義してある.
	 * なお、並び順は Cygwinとlinuxで一致していない.
	 * 連続した一塊として一致している部分もあるが、これから先もそうであるとは限らない.
	 * バラバラであるとみなした方がよい.
	 */
#if defined( Znk_TARGET_UNIX)
	/* Linux/Cygwin共通(ただし実値としての整数は異なる) */
	{ ENOMSG,       "ENOMSG",       "No message of desired type" },
	{ EIDRM,        "EIDRM",        "Identifier removed" },
	{ ECHRNG,       "ECHRNG",       "Channel number out of range" },
	{ EL2NSYNC,     "EL2NSYNC",     "Level 2 not synchronized" },
	{ EL3HLT,       "EL3HLT",       "Level 3 halted" },
	{ EL3RST,       "EL3RST",       "Level 3 reset" },
	{ ELNRNG,       "ELNRNG",       "Link number out of range" },
	{ EUNATCH,      "EUNATCH",      "Protocol driver not attached" },
	{ ENOCSI,       "ENOCSI",       "No CSI structure available" },
	{ EL2HLT,       "EL2HLT",       "Level 2 halted" },
	{ EDEADLK,      "EDEADLK",      "Resource deadlock avoided" },
	{ ENOLCK,       "ENOLCK",       "No locks available" },
	{ EBADE,        "EBADE",        "Invalid exchange" },
	{ EBADR,        "EBADR",        "Invalid request descriptor" },
	{ EXFULL,       "EXFULL",       "Exchange full" },
	{ ENOANO,       "ENOANO",       "No anode" },
	{ EBADRQC,      "EBADRQC",      "Invalid request code" },
	{ EBADSLT,      "EBADSLT",      "Invalid slot" },
	{ EDEADLOCK,    "EDEADLOCK",    "Resource deadlock avoided" },
	{ EBFONT,       "EBFONT",       "Bad font file format" },
	{ ENOSTR,       "ENOSTR",       "Device not a stream" },
	{ ENODATA,      "ENODATA",      "No data available" },
	{ ETIME,        "ETIME",        "Timer expired" },
	{ ENOSR,        "ENOSR",        "Out of streams resources" },
	{ ENONET,       "ENONET",       "Machine is not on the network" },
	{ ENOPKG,       "ENOPKG",       "Package not installed" },
	{ EREMOTE,      "EREMOTE",      "Object is remote" },
	{ ENOLINK,      "ENOLINK",      "Link has been severed" },
	{ EADV,         "EADV",         "Advertise error" },
	{ ESRMNT,       "ESRMNT",       "Srmount error" },
	{ ECOMM,        "ECOMM",        "Communication error on send" },
	{ EPROTO,       "EPROTO",       "Protocol error" },
	{ EMULTIHOP,    "EMULTIHOP",    "Multihop attempted" },
	{ EDOTDOT,      "EDOTDOT",      "RFS specific error" },
	{ EBADMSG,      "EBADMSG",      "Bad message" },
	{ ENOTUNIQ,     "ENOTUNIQ",     "Name not unique on network" },
	{ EBADFD,       "EBADFD",       "File descriptor in bad state" },
	{ EREMCHG,      "EREMCHG",      "Remote address changed" },
	{ ELIBACC,      "ELIBACC",      "Can not access a needed shared library" },
	{ ELIBBAD,      "ELIBBAD",      "Accessing a corrupted shared library" },
	{ ELIBSCN,      "ELIBSCN",      ".lib section in a.out corrupted" },
	{ ELIBMAX,      "ELIBMAX",      "Attempting to link in too many shared libraries" },
	{ ELIBEXEC,     "ELIBEXEC",     "Cannot exec a shared library directly" },
	{ ENOSYS,       "ENOSYS",       "Function not implemented" },
	{ ENOTEMPTY,    "ENOTEMPTY",    "Directory not empty" },
	{ ENAMETOOLONG, "ENAMETOOLONG", "File name too long" },
	{ ELOOP,        "ELOOP",        "Too many levels of symbolic links" },
	{ EOPNOTSUPP,   "EOPNOTSUPP",   "Operation not supported" },
	{ EPFNOSUPPORT, "EPFNOSUPPORT", "Protocol family not supported" },
	{ ECONNRESET,   "ECONNRESET",   "Connection reset by peer" },
	{ ENOBUFS,      "ENOBUFS",      "No buffer space available" },
	{ EAFNOSUPPORT, "EAFNOSUPPORT", "Address family not supported by protocol" },
	{ EPROTOTYPE,   "EPROTOTYPE",   "Protocol wrong type for socket" },
	{ ENOTSOCK,     "ENOTSOCK",     "Socket operation on non-socket" },
	{ ENOPROTOOPT,  "ENOPROTOOPT",  "Protocol not available" },
	{ ESHUTDOWN,    "ESHUTDOWN",    "Cannot send after transport endpoint shutdown" },
	{ ECONNREFUSED, "ECONNREFUSED", "Connection refused" },
	{ EADDRINUSE,   "EADDRINUSE",   "Address already in use" },
	{ ECONNABORTED, "ECONNABORTED", "Software caused connection abort" },
	{ ENETUNREACH,  "ENETUNREACH",  "Network is unreachable" },
	{ ENETDOWN,     "ENETDOWN",     "Network is down" },
	{ ETIMEDOUT,    "ETIMEDOUT",    "Connection timed out" },
	{ EHOSTDOWN,    "EHOSTDOWN",    "Host is down" },
	{ EHOSTUNREACH, "EHOSTUNREACH", "No route to host" },
	{ EINPROGRESS,  "EINPROGRESS",  "Operation now in progress" },
	{ EALREADY,     "EALREADY",     "Operation already in progress" },
	{ EDESTADDRREQ, "EDESTADDRREQ", "Destination address required" },
	{ EMSGSIZE,     "EMSGSIZE",     "Message too long" },
	{ EPROTONOSUPPORT, "EPROTONOSUPPORT", "Protocol not supported" },
	{ ESOCKTNOSUPPORT, "ESOCKTNOSUPPORT", "Socket type not supported" },
	{ EADDRNOTAVAIL, "EADDRNOTAVAIL", "Cannot assign requested address" },
	{ ENETRESET,    "ENETRESET",    "Network dropped connection on reset" },
	{ EISCONN,      "EISCONN",      "Transport endpoint is already connected" },
	{ ENOTCONN,     "ENOTCONN",     "Transport endpoint is not connected" },
	{ ETOOMANYREFS, "ETOOMANYREFS", "Too many references: cannot splice" },
	{ EUSERS,       "EUSERS",       "Too many users" },
	{ EDQUOT,       "EDQUOT",       "Disk quota exceeded" },
	{ ESTALE,       "ESTALE",       "Stale NFS file handle" },
	{ ENOTSUP,      "ENOTSUP",      "Operation not supported" },
	{ ENOMEDIUM,    "ENOMEDIUM",    "No medium found" },
	{ EILSEQ,       "EILSEQ",       "Invalid or incomplete multibyte or wide character" },
	{ EOVERFLOW,    "EOVERFLOW",    "Value too large for defined data type" },
#endif

#if defined(__CYGWIN__)
	/* Cygwin固有 */
	{ ELBIN,        "ELBIN",        "Inode is remote (not really error)" },
	{ EFTYPE,       "EFTYPE",       "Inappropriate file type or format" },
	{ ENMFILE,      "ENMFILE",      "No more files" },
	{ EPROCLIM,     "EPROCLIM",     "EPROCLIM" },
	{ ENOSHARE,     "ENOSHARE",     "No such host or network path" },
	{ ECASECLASH,   "ECASECLASH",   "Filename exists with different case" },
#endif

#if defined(__linux__)
	/* Linux固有 */
	{ ECANCELED,    "ECANCELED",    "Operation canceled" },
	{ ERESTART,     "ERESTART",     "Interrupted system call should be restarted" },
	{ ESTRPIPE,     "ESTRPIPE",     "Streams pipe error" },
	{ ENOTNAM,      "ENOTNAM",      "Not a XENIX named type file" },
	{ ENAVAIL,      "ENAVAIL",      "No XENIX semaphores available" },
	{ EISNAM,       "EISNAM",       "Is a named type file" },
	{ EREMOTEIO,    "EREMOTEIO",    "Remote I/O error" },
	{ EMEDIUMTYPE,  "EMEDIUMTYPE",  "Wrong medium type" },
	{ ENOKEY,       "ENOKEY",       "Required key not available" },
	{ EKEYEXPIRED,  "EKEYEXPIRED",  "Key has expired" },
	{ EKEYREVOKED,  "EKEYREVOKED",  "Key has been revoked" },
	{ EKEYREJECTED, "EKEYREJECTED", "Key was rejected by service" },
	{ EOWNERDEAD,   "EOWNERDEAD",   "Owner died" },
	{ ENOTRECOVERABLE, "ENOTRECOVERABLE", "State not recoverable" },
	/* 以下はUbuntu(というかgcc4.2.4)では存在しないようなのでコメントアウト */
	//{ ERFKILL,      "ERFKILL",      "Operation not possible due to RF-kill" },
#endif

};
/***
 * linuxでの最大番号は現在132である.
 */

/* From cygwin32. linuxにもある模様.  */
//#define EWOULDBLOCK EAGAIN	/* Operation would block */

/* cygwin only */
//#define __ELASTERROR 2000	/* Users can add values starting here */


uint32_t
ZnkSysErrno_errno( void )
{
	return errno; /* std global variable : errno */
}

ZnkSysErrnoInfo*
ZnkSysErrno_getInfo( uint32_t sys_errno )
{
	/***
	 * POSIXの部分 1から34( EPERMからERANGE )までの間は
	 * (bcc55を除き)どの環境でもたいていこの順番になってはいるようだが、
	 * それ以外の部分は環境によってバラバラであると考えてよい.
	 * そこでここではループにより合致する値をスキャンしなければならない.
	 */
	ZnkSysErrnoInfo* info;
	size_t idx;
	for( idx=0; idx<Znk_NARY( st_sys_errno_info_table ); ++idx ){
		info = st_sys_errno_info_table + idx;
		if( sys_errno == info->sys_errno_ ){
			return info;
		}
	}
	return st_sys_errno_info_table + 0 ; /* not found */
}


