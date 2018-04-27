@echo off
if not exist %ZNK_NDK_DIR%\prebuilt (
	echo ZNK_NDK_DIR=[%ZNK_NDK_DIR%] is not set or it is not set as android-ndk directory. 
)
