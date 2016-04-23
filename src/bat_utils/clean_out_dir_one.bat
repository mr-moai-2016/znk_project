@echo off

set RM_=del
set TARGET_PROJ_=%1

if "%TARGET_PROJ_%" == "" goto End

if not exist %TARGET_PROJ_%\out_dir goto End
	echo ====
	echo Cleaning %TARGET_PROJ_%...
	@echo on
	%RM_% %TARGET_PROJ_%\out_dir\*.exe
	%RM_% %TARGET_PROJ_%\out_dir\*.dll
	%RM_% %TARGET_PROJ_%\out_dir\*.res
	%RM_% %TARGET_PROJ_%\out_dir\*.o
	%RM_% %TARGET_PROJ_%\out_dir\*.obj
	@echo off
	echo ====
	echo;
:End

