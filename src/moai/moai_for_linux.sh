#!/bin/sh

chmod 755 ./moai
find ./cgis/ -name \*.cgi -exec chmod 755 {} \;
env LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./moai

