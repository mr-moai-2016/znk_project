@echo off
if not "%ZNK_MINGW_DIR%" == "" goto EndOf_SetMinGWDir

set ZNK_MINGW_DIR=C:\MinGW

:EndOf_SetMinGWDir

set PATH=%ZNK_MINGW_DIR%\bin;%PATH%
