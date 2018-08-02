@echo off

if exist ..\mkf_triggers cd ..
if not exist mkf_triggers goto Error_InvalidTrigger

set MKF_ID=vc
set MAKE_CMD=nmake
set MAKE_ARG=install
call mkf_triggers\_impl\_make_general.bat 
goto End

:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.

:End
pause
