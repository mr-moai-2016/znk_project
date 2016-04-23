#!/bin/sh

echo "===="
make -f Makefile_linux.mak -C libZnk clean
echo "===="
echo ""

echo "===="
make -f Makefile_linux.mak -C libZnk/zlib clean
echo "===="
echo ""

echo "===="
make -f Makefile_linux.mak -C moai clean
echo "===="
echo ""

echo "===="
make -f Makefile_linux.mak -C http_decorator clean
echo "===="
echo ""

echo "===="
make -f Makefile_linux.mak -C virtual_users clean
echo "===="
echo ""

echo "===="
make -f Makefile_linux.mak -C plugin_futaba clean
echo "===="
echo ""

echo "All cleanings are done successfully."
