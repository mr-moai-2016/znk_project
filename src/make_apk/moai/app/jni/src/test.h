#include <jni.h>

#ifndef _Included_MrMoai2016_helloworld_MainActivity
#define _Included_MrMoai2016_helloworld_MainActivity

#define Znk_JNI_INTERFACE( function )  Java_znkproject_moai_MainActivity_##function

JNIEXPORT jboolean JNICALL
Znk_JNI_INTERFACE( exitIfOldProcessExist )( JNIEnv* env, jobject obj );

JNIEXPORT jboolean JNICALL
Znk_JNI_INTERFACE( init )( JNIEnv* env, jobject obj, jstring private_path, jstring abi_jstr, jstring pkg_jpath, jstring native_lib_jpath );

JNIEXPORT jboolean JNICALL
Znk_JNI_INTERFACE( copyFromAssets )( JNIEnv* env, jclass clazz, jobject assetManager,
		jstring src_filename, jstring dst_dir, jstring dst_filename );

JNIEXPORT jstring JNICALL
Znk_JNI_INTERFACE( mainLoop )( JNIEnv* env, jobject obj );

#endif
