#!/bin/sh

CP_="install -p"
CP_COMFIRM_="cp -i"

mkdir -p ../bin_for_android

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

install_abi()
{
	TARGET_ARCH_ABI=$1
	INST_DIR_=../bin_for_android/$TARGET_ARCH_ABI
	mkdir -p $INST_DIR_

	#
	# copy libZnk so
	#
	$CP_ libZnk/android/libs/$TARGET_ARCH_ABI/libZnk.so              $INST_DIR_/

	#
	# setup moai
	# 
	$CP_ moai/android/libs/$TARGET_ARCH_ABI/moai                     $INST_DIR_/
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
	$CP_ http_decorator/android/libs/$TARGET_ARCH_ABI/http_decorator $INST_DIR_/

	#
	# setup virtual_users
	#
	install_one -c user_agent.txt   virtual_users $INST_DIR_
	install_one -c screen_size.txt  virtual_users $INST_DIR_
	install_one -c parent_proxy.txt virtual_users $INST_DIR_
	$CP_ virtual_users/*.md $INST_DIR_/

	#
	# setup plugins
	#
	mkdir -p "$INST_DIR_/plugins"
	$CP_ plugin_futaba/android/libs/$TARGET_ARCH_ABI/libfutaba.so $INST_DIR_/plugins/futaba.so
	$CP_ plugin_2ch/android/libs/$TARGET_ARCH_ABI/lib2ch.so       $INST_DIR_/plugins/2ch.so
	
	echo "$TARGET_ARCH_ABI files installed to $INST_DIR_ successfully."
}

install_abi x86
install_abi armeabi
install_abi armeabi-v7a
