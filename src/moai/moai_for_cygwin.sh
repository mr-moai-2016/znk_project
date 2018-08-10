#!/bin/sh

chmod 755 ./moai.exe
#chmod 755 ./birdman/birdman.exe
chmod 755 ./*.dll

find ./cgis/ -name \*.cgi -exec chmod 755 {} \;
env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./moai

