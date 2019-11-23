#!/system/bin/sh

old_parent_ver=2.1
new_parent_ver=2.2
if test -e set_ver.sh ; then source ./set_ver.sh ; fi

birdman_exe=./birdman
UNAME_M=`shell uname -m`
case "$UNAME_M" in
	i386 | i686 )
		if test -e moai-android-x86; then
			birdman_exe=moai-android-x86/birdman/birdman
		fi
		;;
	arm* )
		if test -e moai-android-armeabi; then
			birdman_exe=moai-android-armeabi/birdman/birdman
		fi
		;;
	* )
		echo "Error : Unsupported CPU : [$UNAME_M]"
esac
chmod 755 $birdman_exe
if test -e moai-common/birdman/birdman.myf ; then cp moai-common/birdman/birdman.myf ./ ; fi
$birdman_exe install android-x86         $old_parent_ver $new_parent_ver
$birdman_exe install android-armeabi     $old_parent_ver $new_parent_ver
$birdman_exe install android-armeabi-v7a $old_parent_ver $new_parent_ver

