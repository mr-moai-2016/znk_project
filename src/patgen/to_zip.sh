#!/bin/sh

old_parent_ver=2.1
old_ver=2.1
new_parent_ver=2.1
new_ver=2.1

if test -e set_ver.sh ; then source ./set_ver.sh ; fi

zip moai-v$old_parent_ver-patch-v$new_ver-linux.zip -r zip moai-v$old_parent_ver-patch-v$new_ver-linux
