@echo off
set CP=xcopy /H /C /Y

if "%MOAI_PROFILE_DIR%" == "" set MOAI_PROFILE_DIR=..\moai_profile

mkdir %MOAI_PROFILE_DIR%\mydoc\imgs
%CP% /F ..\..\zdoc\mydoc_in\imgs\*.jpg %MOAI_PROFILE_DIR%\mydoc\imgs\
%CP% /F ..\..\zdoc\mydoc_in\imgs\*.png %MOAI_PROFILE_DIR%\mydoc\imgs\
win64\docgen.exe html_doc ../../zdoc/mydoc_in %MOAI_PROFILE_DIR%/mydoc
pause
