#!/bin/sh

old_parent_ver=2.1
old_ver=2.1
new_parent_ver=2.1
new_ver=2.1.1

if test -e set_ver.sh ; then source ./set_ver.sh ; fi

if test "x$MK_OS" = "x"; then
	MK_OS=`uname | cut -d _ -f 1`
fi

if test "$MK_OS" = "CYGWIN"; then
	patch_platform=cygwin
	patch_basename=moai-v$old_parent_ver-patch-v$new_ver-$patch_platform
	mkdir  $patch_basename
	rm -rf $patch_basename/moai-common
	rm -rf $patch_basename/moai-cygwin32
	mkdir $patch_basename/moai-common
	mkdir $patch_basename/moai-cygwin32
	cygwin32/patgen make diff_cygwin32.pmk $patch_basename/moai-common $patch_basename/moai-cygwin32 cygwin32
	if test -e apply_this_scripts/cygwin/apply_this_patch.sh ; then cp apply_this_scripts/cygwin/apply_this_patch.sh $patch_basename/ ; fi
	if test -e set_ver.sh ; then cp set_ver.sh $patch_basename/ ; fi
else
	patch_platform=linux
	patch_basename=moai-v$old_parent_ver-patch-v$new_ver-$patch_platform
	mkdir  $patch_basename
	rm -rf $patch_basename/moai-common
	rm -rf $patch_basename/moai-linux32
	rm -rf $patch_basename/moai-linux64
	mkdri $patch_basename/moai-common
	mkdri $patch_basename/moai-linux32
	mkdri $patch_basename/moai-linux64
	linux32/patgen make diff_linux32.pmk $patch_basename/moai-common $patch_basename/moai-linux32 linux32
	linux64/patgen make diff_linux64.pmk $patch_basename/moai-common $patch_basename/moai-linux64 linux64
	if test -e apply_this_scripts/linux/apply_this_patch.sh ; then cp apply_this_scripts/linux/apply_this_patch.sh $patch_basename/ ; fi
	if test -e set_ver.sh ; then cp set_ver.sh $patch_basename/ ; fi
fi
