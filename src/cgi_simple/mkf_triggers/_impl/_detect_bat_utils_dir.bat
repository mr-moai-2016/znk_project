@echo off

if "%BAT_UTILS_DIR%" == "" goto AutoDetecting
REM if BAT_UTILS_DIR is set as fullpath
if exist %BAT_UTILS_DIR% goto OK
echo BAT_UTILS_DIR=[%BAT_UTILS_DIR%] is set but it doesn't exist.
echo So we try to detect it automatically.
set BAT_UTILS_DIR=
:AutoDetecting

REM try to seek relative path
if exist bat_utils set BAT_UTILS_DIR=.\bat_utils
if not "%BAT_UTILS_DIR%" == "" goto OK
if exist ..\bat_utils set BAT_UTILS_DIR=..\bat_utils
if not "%BAT_UTILS_DIR%" == "" goto OK
if exist ..\..\bat_utils set BAT_UTILS_DIR=..\..\bat_utils
if not "%BAT_UTILS_DIR%" == "" goto OK
if exist ..\..\..\bat_utils set BAT_UTILS_DIR=..\..\..\bat_utils
if not "%BAT_UTILS_DIR%" == "" goto OK
if exist ..\..\..\..\bat_utils set BAT_UTILS_DIR=..\..\..\..\bat_utils
if not "%BAT_UTILS_DIR%" == "" goto OK
if exist ..\..\..\..\..\bat_utils set BAT_UTILS_DIR=..\..\..\..\..\bat_utils
if not "%BAT_UTILS_DIR%" == "" goto OK
if exist ..\..\..\..\..\..\bat_utils set BAT_UTILS_DIR=..\..\..\..\..\..\bat_utils
if "%BAT_UTILS_DIR%" == "" echo BAT_UTILS_DIR can not be detected automatically.
goto End
:OK
echo BAT_UTILS_DIR=[%BAT_UTILS_DIR%] is detected.
:End
