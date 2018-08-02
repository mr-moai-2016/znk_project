#!/bin/sh

if test "x$MK_OS" = "x"; then
	MK_OS=`uname | cut -d _ -f 1`
fi
echo "MK_OS=[$MK_OS]"

if test "$MK_OS" = "CYGWIN"; then
	if test -e moai-cygwin32; then
		cp moai-cygwin32/birdman/birdman.exe ./
		cp moai-common/birdman/birdman.myf ./
		birdman.exe install cygwin32 2.0
	else
		echo "Error : This platform is cygwin, but does not exist moai-cygwin32"
	fi
elif test "$MK_OS" = "MINGW32"; then
	if test -e moai-win32; then
		cp moai-win32/birdman/birdman.exe ./
		cp moai-common/birdman/birdman.myf ./
		birdman.exe install win32 2.0
		birdman.exe install win64 2.0
	else
		echo "Error : This platform is MSYS(win32), but does not exist moai-win32"
	fi
else
	UNAME_M=`shell uname -m`
	case "$UNAME_M" in
		"i386" | "i686" )
			if test -e moai-linux32; then
				cp moai-linux32/birdman/birdman ./
			fi
			;;
		"x86_64" | "amd64" )
			if test -e moai-linux64; then
				cp moai-linux64/birdman/birdman ./
			fi
			;;
		* )
			echo "Error : Unsupported CPU : [$UNAME_M]"
	esac
	chmod 755 birdman
	cp moai-common/birdman/birdman.myf ./
	./birdman install linux32 2.0
	./birdman install linux64 2.0
fi

