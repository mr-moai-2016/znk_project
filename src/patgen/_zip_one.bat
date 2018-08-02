@echo off
set SRC_DIR=%1
set DST_DIR=.
set RESULT_BASENAME=%2


if not exist "%DST_DIR%" goto Error0
if exist "%DST_DIR%\%RESULT_BASENAME%__tmp__.zip" goto Error1

"C:\Program Files\7-Zip\7z.exe" a %DST_DIR%\%RESULT_BASENAME%__tmp__.zip %SRC_DIR%
if errorlevel 1 goto Error2

move /y %DST_DIR%\%RESULT_BASENAME%__tmp__.zip %DST_DIR%\%RESULT_BASENAME%.zip
pause
goto End

:Error0
echo Error : Destination dir=[%DST_DIR%] does not exist.
pause
goto End

:Error1
echo Error : %DST_DIR%\%RESULT_BASENAME%__tmp__.zip is already exist.
echo       : At first, please remove this file.
pause
goto End

:Error2
echo Error : 7z.exe command is terminated abnormally.
echo       : %DST_DIR%\%RESULT_BASENAME%.zip file is not created/updated.
pause
goto End

:End
