@echo off

if "%MKFSYS_DIR%" == "" goto AutoDetecting
REM if MKFSYS_DIR is set as fullpath
if exist %MKFSYS_DIR% goto OK
echo MKFSYS_DIR=[%MKFSYS_DIR%] is set but it doesn't exist.
echo So we try to detect it automatically.
set MKFSYS_DIR=
:AutoDetecting

REM try to seek relative path
if exist mkfsys set MKFSYS_DIR=.\mkfsys
if not "%MKFSYS_DIR%" == "" goto OK
if exist ..\mkfsys set MKFSYS_DIR=..\mkfsys
if not "%MKFSYS_DIR%" == "" goto OK
if exist ..\..\mkfsys set MKFSYS_DIR=..\..\mkfsys
if not "%MKFSYS_DIR%" == "" goto OK
if exist ..\..\..\mkfsys set MKFSYS_DIR=..\..\..\mkfsys
if not "%MKFSYS_DIR%" == "" goto OK
if exist ..\..\..\..\mkfsys set MKFSYS_DIR=..\..\..\..\mkfsys
if not "%MKFSYS_DIR%" == "" goto OK
if exist ..\..\..\..\..\mkfsys set MKFSYS_DIR=..\..\..\..\..\mkfsys
if not "%MKFSYS_DIR%" == "" goto OK
if exist ..\..\..\..\..\..\mkfsys set MKFSYS_DIR=..\..\..\..\..\..\mkfsys
if "%MKFSYS_DIR%" == "" echo MKFSYS_DIR can not be detected automatically.
goto End
:OK
echo MKFSYS_DIR=[%MKFSYS_DIR%] is detected.
:End
