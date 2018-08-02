#!/bin/sh
current_dir_save=`pwd`

check_error()
{
	status=$1
	msg=$2
	if test $status -ne 0; then
		echo "[Error] : $msg"; exit $status;
	fi
}

make_one()
{
	cd $1
	echo ""
	echo ""
	echo "==== $1 $2 ===="
	echo ""
	make -f Makefile_$ZNK_MKID.mak $2
	check_error $? "$1 $2"
	cd $current_dir_save
}

LIST=""
if test -e proj_list.sh ; then LIST="$LIST `cat proj_list.sh`" ; fi

if test "$1" = "--help" ; then
	echo "Usage : ./make_world.sh MAKE_ARGS"
	exit
fi

#
# ZNK_MKID が明示的に設定されていない場合はunameより自動判定する.
# cut -d _ -f N で 入力文字列を _ でsplitし、
# それによって得られた文字列リストの N番目を返す.
# (Nは1からはじまる)
#
# これはたとえばCygwinの場合 unameの出力が CYGWIN_NT-5.0,
# MSYS の場合 MINGW32_NT-5.0 のようになるためで、_より前の
# 部分のみを抽出する狙いがある.
# 
if test "x$ZNK_MKID" = "x"; then
	ZNK_MKID=`uname | cut -d _ -f 1`
fi
if test "$ZNK_MKID" = "CYGWIN"; then
	ZNK_MKID="cygwin"
elif test "$ZNK_MKID" = "MINGW32"; then
	ZNK_MKID="msys"
elif test "$ZNK_MKID" = "MINGW64"; then
	ZNK_MKID="msys"
else
	# Anyway, we make as linux.
	ZNK_MKID="linux"
fi
echo "ZNK_MKID=[$ZNK_MKID]"


for i in $LIST
do
	make_one $i $1
done


echo ""
echo ""
echo "==== make_world.sh done. ===="
echo ""
