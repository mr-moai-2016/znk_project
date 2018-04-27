@echo off

if exist ..\mkf_triggers cd ..
if not exist mkf_triggers goto Error_InvalidTrigger

call mkf_triggers\_impl\_detect_bat_utils_dir.bat
if "%BAT_UTILS_DIR%" == ""   goto Error_InvalidBatUtilDir
if not exist %BAT_UTILS_DIR% goto Error_InvalidBatUtilDir

if not "%MKF_ID%" == "" goto EndOf_AutoDetectMkfID
	if exist mkf_triggers\__MKF_ID_vc__      set MKF_ID=vc
	if exist mkf_triggers\__MKF_ID_mingw__   set MKF_ID=mingw
	if exist mkf_triggers\__MKF_ID_android__ set MKF_ID=android
	if "%MKF_ID%" == "" call mkf_triggers\_impl\_detect_mkfid.bat
	if "%MKF_ID%" == "" goto Error_TargetNotSelected
:EndOf_AutoDetectMkfID

call mkf_triggers\_impl\_setenv_machine.bat

if not exist %BAT_UTILS_DIR%\setenv_%MKF_ID%.bat goto Error_InvalidMkfID
call %BAT_UTILS_DIR%\setenv_%MKF_ID%.bat

set MAKE_FILE=Makefile_%MKF_ID%.mak
if "%MKF_ID%" == "vc"      set MAKE_CMD=nmake
if "%MKF_ID%" == "mingw"   set MAKE_CMD=mingw32-make
if "%MKF_ID%" == "android" set MAKE_CMD=%ZNK_NDK_DIR%\prebuilt\windows\bin\make

if exist mkf_triggers\__DEBUG__ set DEBUG=d
if exist mkf_triggers\__BOTH__  set BOTH=d

set MAKE_ARG=%1

:Begin
if "%BOTH%"=="d" set DEBUG=d

echo ====
%MAKE_CMD% -f Makefile_%MKF_ID%.mak %MAKE_ARG%
if errorlevel 1 goto Error_Make
echo ====
echo;

if not "%BOTH%"=="d" goto OK
set DEBUG=
set BOTH=o
echo Next, we compile this as optimaize mode.
goto Begin

:OK
echo All make %MAKE_ARG% is done successfully.
goto End

:Error_InvalidMkfID
echo Error : MKF_ID=[%MKF_ID%] is not supported.
pause
goto End

:Error_TargetNotSelected
echo Error : MKF_ID target is not selected. At first, please select target.
pause
goto End

:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.
pause
goto End

:Error_InvalidBatUtilDir
echo Error : BAT_UTILS_DIR=[%BAT_UTILS_DIR%] is not found.
pause
goto End

:Error_Make
echo Error : %MAKE_CMD% exit failure.
pause
goto End

:End
