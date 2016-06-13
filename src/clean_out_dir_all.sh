#!/bin/sh

if test "x$UNAME_OS" = "x"; then
	UNAME_OS=`uname | cut -d _ -f 1`
fi

MAKEFILE_NAME=Makefile_linux.mak
if test "x$UNAME_OS" = "xCYGWIN"; then
	MAKEFILE_NAME=Makefile_cygwin.mak
fi

echo "===="
make -f ${MAKEFILE_NAME} -C libZnk clean
echo "===="
echo ""

echo "===="
make -f ${MAKEFILE_NAME} -C libZnk/zlib clean
echo "===="
echo ""

echo "===="
make -f ${MAKEFILE_NAME} -C moai clean
echo "===="
echo ""

echo "===="
make -f ${MAKEFILE_NAME} -C http_decorator clean
echo "===="
echo ""

echo "===="
make -f ${MAKEFILE_NAME} -C plugin_futaba clean
echo "===="
echo ""

echo "===="
make -f ${MAKEFILE_NAME} -C plugin_2ch clean
echo "===="
echo ""

echo "All cleanings are done successfully."
