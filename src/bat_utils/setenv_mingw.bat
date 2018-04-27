@echo off

if "%MACHINE%" == "x64" goto For_x64


if not "%ZNK_MINGW32_DIR%" == "" goto EndOf_SetMinGW32Dir

set ZNK_MINGW32_DIR=C:\MinGW32

:EndOf_SetMinGW32Dir

set PATH=%ZNK_MINGW32_DIR%\bin;%PATH%

goto End


:For_x64

if not "%ZNK_MINGW64_DIR%" == "" goto EndOf_SetMinGW64Dir

set ZNK_MINGW64_DIR=C:\MinGW64

:EndOf_SetMinGW64Dir

set PATH=%ZNK_MINGW64_DIR%\bin;%PATH%


:End
