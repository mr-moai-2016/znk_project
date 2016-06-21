#!/bin/sh

source ./android_setting_ndk.sh

cd libZnk/android
ndk-build
cd ../..

cd moai/android
ndk-build
cd ../..

cd http_decorator/android
ndk-build
cd ../..

cd plugin_futaba/android
ndk-build
cd ../..

cd plugin_2ch/android
ndk-build
cd ../..

