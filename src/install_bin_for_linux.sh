#!/bin/sh

ROOT_DIR_="$1"
if test "$ROOT_DIR_" = "" ; then
	ROOT_DIR_=".."
fi

if test "x$UNAME_OS" = "x"; then
	UNAME_OS=`uname | cut -d _ -f 1`
fi
if test "x$UNAME_OS" = "xCYGWIN"; then
	INST_DIR_=$ROOT_DIR_/bin_for_cygwin
	DL_EXT_="dll"
	PLG_PFX_="cyg"
	LIB_PFX_="cyg"
elif test "x$UNAME_OS" = "xMINGW32"; then
	INST_DIR_=$ROOT_DIR_/bin_for_win32
	DL_EXT_="dll"
	PLG_PFX_=""
	LIB_PFX_=""
else
	# Anyway we think it as linux.
	INST_DIR_=$ROOT_DIR_/bin_for_linux
	DL_EXT_="so"
	PLG_PFX_=""
	LIB_PFX_="lib"
fi


CP_="install -p"
CP_COMFIRM_="cp -i"
#INST_DIR_=$ROOT_DIR_/bin_for_linux

mkdir -p "$INST_DIR_"

#
# copy libZnk
#
if test -e libZnk ; then
	SRC_DIR=libZnk
fi

DL_VER_SFX=
$CP_ $SRC_DIR/out_dir/${LIB_PFX_}Znk$DL_VER_SFX.${DL_EXT_} $INST_DIR_/

#
# setup moai
#
$CP_ moai/out_dir/moai $INST_DIR_/
mkdir -p "$INST_DIR_/doc_root"
LIST=`ls moai/doc_root *.html *.png *.jpg *.gif *.js *.css`
for i in $LIST
do
	$CP_ moai/doc_root/$i $INST_DIR_/doc_root/
	echo "$CP_ moai/doc_root/$i $INST_DIR_/doc_root/"
done
mkdir -p $INST_DIR_/filters
$CP_ moai/filters/*.myf $INST_DIR_/filters/
$CP_ moai/*.md  $INST_DIR_/
$CP_ moai/*.myf $INST_DIR_/

#
# setup http_decorator
#
$CP_ http_decorator/out_dir/http_decorator $INST_DIR_/

#
# setup virtual_users
#
install_one()
{
	if_dst_exist=$1
	file=$2
	src_dir=$3
	dst_dir=$4
	if   test "$if_dst_exist" = "-f" ; then
		$CP_ $src_dir/$file $dst_dir/
	elif test "$if_dst_exist" = "-n" ; then
		if ! test -e "$dst_dir/$file" ; then
			$CP_ $src_dir/$file $dst_dir/
		fi
	elif test "$if_dst_exist" = "-c" ; then
		$CP_COMFIRM_ $src_dir/$file $dst_dir/
	fi
}
#install_one -c user_agent.txt   virtual_users $INST_DIR_
$CP_COMFIRM_ virtual_users/user_agent_for_pc.txt  $INST_DIR_/user_agent.txt
install_one -c screen_size.txt  virtual_users $INST_DIR_
install_one -c parent_proxy.txt virtual_users $INST_DIR_
$CP_ virtual_users/*.md $INST_DIR_/

#
# setup plugin
#
mkdir -p "$INST_DIR_/plugins"
$CP_ plugin_futaba/out_dir/${PLG_PFX_}futaba.${DL_EXT_} $INST_DIR_/plugins/
$CP_ plugin_2ch/out_dir/${PLG_PFX_}2ch.${DL_EXT_}       $INST_DIR_/plugins/

echo "All files installed to $INST_DIR_ successfully."
