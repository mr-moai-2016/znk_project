#!/bin/sh

if test "$1" = "" ; then
	TARGET_ARCH_ABI=armeabi
else
	TARGET_ARCH_ABI=$1
fi

if test -e $TARGET_ARCH_ABI ; then
	cd $TARGET_ARCH_ABI
else
	echo "Here is not in bin_for_android directory."
	exit $_status
fi

chmod 755 *.sh moai http_decorator
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
./moai

