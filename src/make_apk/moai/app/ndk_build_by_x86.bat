@echo off
set NDK_ROOT=E:\share_e\trunk_id1\Android\android-ndk-r14b
"%NDK_ROOT%\prebuilt\windows\bin\make.exe" -f "%NDK_ROOT%\build\core\build-local.mk" SHELL=cmd HOST_TAG64=windows %*
pause
