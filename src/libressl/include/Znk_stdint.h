#ifndef INCLUDE_GUARD__Znk_stdint_h__
#define INCLUDE_GUARD__Znk_stdint_h__

/***
 * When using winsock2,h on DMC, you have to: #define _WINSOCKAPI_ 
 * Before you include anything to prevent winsock.h from inclusion as that creates conflicts.
 * ( http://www.digitalmars.com/d/archives/c++/idde/326.html )
 */
#if defined(__DMC__)
/* Prevent winsock.h #include's. */
#  define _WINSOCKAPI_
#endif

/* size_t�Ȃǂ�C���ʌ^�̐錾 */
#include <stddef.h>


/**
 * C++�ɂ�����include���ꂽ�ꍇ�̂݃V���{���̃}���O�����O��}��.
 * ����ɂ��C�̃��W���[����C++����ł��g�p�\�Ƃ���.
 */
#ifdef __cplusplus
#  if !defined(Znk_EXTERN_C_BEGIN) || !defined(Znk_EXTERN_C_END)
#    define Znk_EXTERN_C_BEGIN extern "C" {
#    define Znk_EXTERN_C_END   } 
#  endif
#else
#  define Znk_EXTERN_C_BEGIN
#  define Znk_EXTERN_C_END
#endif



/***
 * Suppressing Warning
 */
#if defined(__GNUC__)
/* GCC */
#elif defined(_MSC_VER)
#  if _MSC_VER < 1300
/* VC6.0 */
#    pragma warning(disable: 4786)
#    pragma warning(disable: 4284)
#  endif

#  pragma warning(disable: 4127) /* for while(1) etc. */
#  pragma warning(disable: 4100)
#  pragma warning(disable: 4054)
#  pragma warning(disable: 4055)

#  if _MSC_VER == 1400
/* VC8.0(VS2005) */
#    pragma warning(disable: 4819)
#  endif

#  if _MSC_VER >= 1400
#    pragma warning(disable: 4996)
#  endif

/* for WP64 MISGUIDE */
#  pragma warning(disable: 4267)

#elif defined(__BORLANDC__)
#  pragma warn -8008
#  pragma warn -8057
#  pragma warn -8004

#  if !defined(__cplusplus)
#    pragma warn -8080
#  endif

#else
#endif



#if defined(_MSC_VER)
#  if _MSC_VER >= 1300
#    define Znk_RECENT_VC
#    if _MSC_VER < 1500 && defined(_WIN32)
       /* 32bit�R���p�C���ł� /Wp64 misguide �����̉\�����l������ */
#      define Znk_VC_WP64_MISGUIDE 1
#    endif
#  else
#    define Znk_OLD_VC /* VC6.0�̂� */
#  endif
#endif


/* Target OS */
#if defined(_WIN32) || defined(_WIN64) || defined(_MSC_VER) || \
	defined(__MINGW32__) || defined(__MINGW64__) || \
	defined(__BORLANDC__) || defined(__DMC__)
/* Windows API */
#  define Znk_TARGET_WINDOWS 1

#elif defined(__unix__) || defined(__linux__) || defined(__CYGWIN__) || \
	defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
/* POSIX UNIX + X11 ����т��̌݊��V�X�e�� */
#  define Znk_TARGET_UNIX 1

#else
#  error "Not supported OS"

#endif


/* Target CPU BIT */
#if   defined(_WIN64) || defined(__x86_64__) || defined(__MINGW64__) || defined(__aarch64__) || defined(__mips64)
#  define Znk_CPU_BIT 64
#elif defined(_WIN32) || defined(__i386__)   || defined(__MINGW32__) || defined(__arm__) || defined(__mips__)
#  define Znk_CPU_BIT 32
#else
#  error "Not supported CPU bit"
#endif


#if __STDC_VERSION__ >= 199901L || defined(__ANDROID__)
#  ifndef   Znk_C99_SUPPORTED
#    define Znk_C99_SUPPORTED 1
#  endif
#endif


/**
 * @brief
 * Znk_int64_t Znk_uint64_t
 * �����ł� long long �� __int64�^�Ȃǂ�p����.
 * �����Ȃ�int64_t,uint64_t���`�����A�܂��Ǝ����Ƃ���typedef����.
 * �����stdint.h��include�L���ɂ���Č^���Փ˂��邩�ۂ��̔����
 * �ł��邾����ӏ��ɏW�񂵂�������ł���.
 *
 * GCC
 *   long long �͎g�p�\�ƍl���Ă悢.
 *   (�g�������I�v�V�������ݒ肳��Ă��Ă��R���p�C�����ʂ�悤��
 *   __extension__��t����typedef���Ă���.)
 *
 * VC
 *   long long ��VC7.0�ȍ~����g�p�\
 *   (VC6.0�ł͎g�p�s��. �ւ���__int64���g��).
 *
 * BorlandC++5.5
 *   long long �͎g�p�s��. �ւ���__int64���g��.
 *
 * DMC
 *   long long �͎g�p�\.
 *
 * ���̑��̊�
 *   �Ƃ肠����long long�͉\�Ƃ݂Ȃ��A����ɂ����8byte(64bit)�����Ƃ݂Ȃ�.
 */
#if !defined(Znk_C99_SUPPORTED)
#  if defined(__GNUC__)
__extension__ typedef           long long  Znk__int64_t__;
__extension__ typedef  unsigned long long  Znk__uint64_t__;

#  elif defined(__BORLANDC__) || (defined(_MSC_VER) && _MSC_VER < 1300)
typedef           __int64  Znk__int64_t__;
typedef  unsigned __int64  Znk__uint64_t__;

#  else
typedef           long long  Znk__int64_t__;
typedef  unsigned long long  Znk__uint64_t__;

#  endif
#endif


/***
 * stdint.h�n�̐���typedef
 */
#if !defined(Znk_C99_SUPPORTED)
typedef signed char     int8_t;
typedef unsigned char   uint8_t;
typedef short           int16_t;
typedef unsigned short  uint16_t;
typedef int             int32_t;
typedef unsigned int    uint32_t;
typedef Znk__int64_t__  int64_t;
typedef Znk__uint64_t__ uint64_t;
typedef int64_t         intmax_t;
typedef uint64_t        uintmax_t;
#  if Znk_CPU_BIT == 64
typedef uint64_t uintptr_t;
typedef  int64_t  intptr_t;
#  elif Znk_CPU_BIT == 32
typedef uint32_t uintptr_t;
typedef  int32_t  intptr_t;
#  endif
#else
#  include <stdint.h>
#endif


#if  defined(__BORLANDC__) || defined(__WATCOMC__) \
  || defined(Znk_OLD_VC) \
  || defined (__alpha) || defined (__DECC)
#  ifndef   UINT64_C
#    define UINT64_C(num) num##UI64
#  endif
#  ifndef   INT64_C
#    define INT64_C(num)  num##I64
#  endif
#else
#  ifndef   UINT64_C
#    define UINT64_C(num) num##ULL
#  endif
#  ifndef   INT64_C
#    define INT64_C(num)  num##LL
#  endif
#endif

#ifndef   INTMAX_C
#  define INTMAX_C(num)  INT64_C(num)
#endif
#ifndef   UINTMAX_C
#  define UINTMAX_C(num) UINT64_C(num)
#endif

//#define INT8_MIN (-128) 
//#define INT16_MIN (-32768)
//#define INT32_MIN (-2147483647 - 1)
//#define INT64_MIN  (-9223372036854775807LL - 1)
//#define INT8_MAX 127
//#define INT16_MAX 32767

#ifndef   INT32_MAX
#  define INT32_MAX 2147483647
#endif
#ifndef   INT64_MAX
#  define INT64_MAX INT64_C(9223372036854775807)
#endif

#ifndef   UINT8_MAX
#  define UINT8_MAX 0xff
#endif
#ifndef   UINT16_MAX
#  define UINT16_MAX 0xffff
#endif
#ifndef   UINT32_MAX
#  define UINT32_MAX 0xffffffff  /* 4294967295U */
#endif
#ifndef   UINT64_MAX
#  define UINT64_MAX UINT64_C(0xffffffffffffffff) /* 18446744073709551615 */
#endif

#ifndef SIZE_MAX
#  ifdef _WIN64
#    define SIZE_MAX UINT64_MAX
#  else
#    define SIZE_MAX UINT32_MAX
#  endif
#endif


/***
 * bool�^����ђ萔true/false
 */
typedef int8_t Znk__bool__;

#if defined(Znk_C99_SUPPORTED)
#  include <stdbool.h>

#elif defined(__GNUC__) || defined(__DMC__)
#  include <stdbool.h>

#elif !defined(__bool_true_false_are_defined) && !defined(__cplusplus)
#  if !defined(bool)
#    define bool  Znk__bool__
#  endif
#  if !defined(true)
#    define true 1
#  endif
#  if !defined(false)
#    define false 0
#  endif
#  define __bool_true_false_are_defined	1
#endif


/**
 * @brief
 *  Znk_LongLong Znk_ULongLong
 *  (unsigned) long long (�W���ł�C99�ȍ~) ( Compiler Intrinsic )
 *  �����ł͂���𒊏ۂ��� Znk_LongLong, Znk_ULongLong ���`����.
 */
#if defined(Znk_C99_SUPPORTED)
typedef           long long  Znk_LongLong;
typedef  unsigned long long  Znk_ULongLong;

#else
#  if defined(__GNUC__)
__extension__ typedef           long long  Znk_LongLong;
__extension__ typedef  unsigned long long  Znk_ULongLong;

#  elif defined(__BORLANDC__) || defined(Znk_OLD_VC) || defined(__WATCOMC__) || defined(__alpha) || defined(__DECC)
/* �Â��R���p�C���ł� long long �����݂��Ȃ��̂őւ���__int64���g��. */
typedef           __int64  Znk_LongLong;
typedef  unsigned __int64  Znk_ULongLong;

#  else
typedef           long long  Znk_LongLong;
typedef  unsigned long long  Znk_ULongLong;

#  endif
#endif


#if ( !defined(__cplusplus) && !defined(inline) )
#  if defined(Znk_C99_SUPPORTED)
     /* C99�ł�Intrinsic�Ɏg�p�\. */
#  elif defined(_MSC_VER) || defined(__BORLANDC__)
#    define inline __inline
#  elif defined(__GNUC__)
#    define inline __inline__
#  elif defined(__DMC__)
     /* DMC�ł�inline���ŏ������`����Ă���(C����ł��f�t�H���g�Ŏg����) */
#  else
#    define inline /* none */
#  endif
#endif


#endif /* INCLUDE_GUARD */
