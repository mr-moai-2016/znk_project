#!/bin/sh
MKFSYS_DIR=../../mkfsys
export LD_LIBRARY_PATH=$MKFSYS_DIR
$MKFSYS_DIR/mkfgen

cd zlib
MKFSYS_DIR=../../../mkfsys
export LD_LIBRARY_PATH=$MKFSYS_DIR
$MKFSYS_DIR/mkfgen
