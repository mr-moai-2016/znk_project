@echo off

if "%MKF_ID%"   == "" goto Error_InvalidVar
if "%MAKE_CMD%" == "" goto Error_InvalidVar

call mkf_triggers\_impl\_detect_bat_utils_dir.bat
if "%BAT_UTILS_DIR%" == ""   goto Error_InvalidBatUtilDir
if not exist %BAT_UTILS_DIR% goto Error_InvalidBatUtilDir

call mkf_triggers\_impl\_setenv_machine.bat
if not exist %BAT_UTILS_DIR%\setenv_%MKF_ID%.bat goto Error_InvalidMkfID
call %BAT_UTILS_DIR%\setenv_%MKF_ID%.bat

if exist mkf_triggers\__DEBUG__ set DEBUG=d
if exist mkf_triggers\__BOTH__  set BOTH=d

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

:Error_InvalidVar
echo Error : MAKE_CMD=[%MAKE_CMD%] or MKF_ID=[%MKF_ID%] is not set.
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
