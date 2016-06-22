#!/bin/sh

if test "x$UNAME_OS" = "x"; then
	UNAME_OS=`uname | cut -d _ -f 1`
fi

if test "x$ZNK_NDK_HOME" = "x"; then
	if test "x$UNAME_OS" = "xCYGWIN"; then
		export ZNK_NDK_HOME=/cygdrive/c/android-ndk-r12
	elif test "x$UNAME_OS" = "xMINGW32"; then
		export ZNK_NDK_HOME=/c/android-ndk-r12
		#export GNUMAKE=/d/mingw/bin/mingw32-make.exe
	else
		export ZNK_NDK_HOME=$HOME/android-ndk-r12
	fi
fi

export PATH=$PATH:$ZNK_NDK_HOME
