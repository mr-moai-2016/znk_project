#!/bin/sh

old_parent_ver=2.0
new_parent_ver=2.1
if test -e set_ver.sh ; then source ./set_ver.sh ; fi

if test "x$MK_OS" = "x"; then
	MK_OS=`uname | cut -d _ -f 1`
fi
echo "MK_OS=[$MK_OS]"

birdman_exe=./birdman.exe
if test "$MK_OS" = "CYGWIN"; then
	if test -e moai-cygwin32; then
		cp moai-common/birdman/birdman.myf ./
		birdman_exe=moai-cygwin32/birdman/birdman.exe
		$birdman_exe install cygwin32 $old_parent_ver $new_parent_ver
	else
		echo "Error : This platform is cygwin, but does not exist moai-cygwin32"
	fi
else
	echo "Error : This platform is not cygwin."
fi

