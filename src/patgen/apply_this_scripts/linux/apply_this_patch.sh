#!/bin/sh

old_parent_ver=2.0
new_parent_ver=2.1
if test -e set_ver.sh ; then source ./set_ver.sh ; fi

birdman_exe=./birdman
UNAME_M=`shell uname -m`
case "$UNAME_M" in
	"i386" | "i686" )
		if test -e moai-linux32; then
			birdman_exe=moai-linux32/birdman/birdman
		fi
		;;
	"x86_64" | "amd64" )
		if test -e moai-linux64; then
			birdman_exe=moai-linux64/birdman/birdman
		fi
		;;
	* )
		echo "Error : Unsupported CPU : [$UNAME_M]"
		exit
esac

chmod 755 $birdman_exe
cp moai-common/birdman/birdman.myf ./
$birdman_exe install linux32 $old_parent_ver $new_parent_ver
$birdman_exe install linux64 $old_parent_ver $new_parent_ver

