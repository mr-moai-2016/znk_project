@echo off
set CP=xcopy /H /C /Y

mkdir ..\moai_profile\mydoc\imgs
%CP% /F ..\..\zdoc\mydoc_in\imgs\*.jpg ..\moai_profile\mydoc\imgs\
%CP% /F ..\..\zdoc\mydoc_in\imgs\*.png ..\moai_profile\mydoc\imgs\
win64\docgen.exe html_doc ../../zdoc/mydoc_in ../moai_profile/mydoc
pause
