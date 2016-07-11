#!/bin/sh

ROOT_DIR_="$1"
if test "$ROOT_DIR_" = "" ; then
	ROOT_DIR_=".."
fi

CP_="install -p"
CP_COMFIRM_="cp -i"
INST_DIR_=$ROOT_DIR_/bin_for_cygwin

mkdir -p "$INST_DIR_"

#
# copy libZnk
#
if test -e libZnk ; then
	SRC_DIR=libZnk
fi

DL_VER_SFX=
$CP_ $SRC_DIR/out_dir/cygZnk$DL_VER_SFX.dll $INST_DIR_/

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
install_one -c user_agent.txt   virtual_users $INST_DIR_
install_one -c screen_size.txt  virtual_users $INST_DIR_
install_one -c parent_proxy.txt virtual_users $INST_DIR_
$CP_ virtual_users/*.md $INST_DIR_/

#
# setup plugin
#
mkdir -p "$INST_DIR_/plugins"
$CP_ plugin_futaba/out_dir/cygfutaba.dll $INST_DIR_/plugins/
$CP_ plugin_2ch/out_dir/cyg2ch.dll       $INST_DIR_/plugins/

echo "All files installed to $INST_DIR_ successfully."
