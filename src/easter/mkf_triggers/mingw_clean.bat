@echo off

if exist ..\mkf_triggers cd ..
if not exist mkf_triggers goto Error_InvalidTrigger

set MKF_ID=mingw
set MAKE_CMD=mingw32-make
set MAKE_ARG=clean
call mkf_triggers\_impl\_make_general.bat 
goto End

:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.

:End
pause
