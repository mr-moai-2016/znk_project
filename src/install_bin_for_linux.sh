#!/bin/sh

ROOT_DIR_="$1"
if test "$ROOT_DIR_" = "" ; then
	ROOT_DIR_=".."
fi

CP_="install -p"
INST_DIR_=$ROOT_DIR_/bin_for_linux

mkdir -p "$INST_DIR_"

#
# copy libZnk so
#
if test -e libZnk ; then
	SRC_DIR=libZnk
fi

DL_VER_SFX=
$CP_ $SRC_DIR/out_dir/libZnk$DL_VER_SFX.so $INST_DIR_/

#
# setup moai
#
$CP_ moai/out_dir/moai $INST_DIR_/
mkdir -p "$INST_DIR_/doc_root"
LIST=`ls moai/doc_root`
for i in $LIST
do
	$CP_ moai/doc_root/$i $INST_DIR_/doc_root/
done

#
# setup http_decorator
#
$CP_ http_decorator/out_dir/http_decorator $INST_DIR_/

#
# setup virtual_users
#
install_if_not_exist()
{
	file=$1
	src_dir=$2
	dst_dir=$3
	if ! test -e "$dst_dir/$file" ; then
		$CP_ $src_dir/$file $dst_dir/
	fi
}
$CP_ virtual_users/out_dir/virtual_users $INST_DIR_/
install_if_not_exist user_agent.txt  virtual_users $INST_DIR_
install_if_not_exist screen_size.txt virtual_users $INST_DIR_
mkdir -p $INST_DIR_/filters
LIST=`ls virtual_users/filters`
for i in $LIST
do
	install_if_not_exist $i virtual_users/filters $INST_DIR_/filters
done
install_if_not_exist config.myf      virtual_users $INST_DIR_
install_if_not_exist analysis.myf    virtual_users $INST_DIR_
install_if_not_exist target.myf      virtual_users $INST_DIR_
install_if_not_exist README.txt      virtual_users $INST_DIR_
install_if_not_exist README_more.txt virtual_users $INST_DIR_

#
# setup plugin_futaba
#
mkdir -p "$INST_DIR_/plugins"
$CP_ plugin_futaba/out_dir/futaba.so $INST_DIR_/plugins/

echo "All files installed to $INST_DIR_ successfully."
