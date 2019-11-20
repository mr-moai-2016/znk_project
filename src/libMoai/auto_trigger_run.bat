@echo off
call mkf_triggers\_impl\_detect_mkfsys_dir.bat
if "%MKFSYS_DIR%" == ""   goto Error_InvalidMkfSysDir
if not exist %MKFSYS_DIR% goto Error_InvalidMkfSysDir

if exist %MKFSYS_DIR%\win32\auto_trigger.exe set MKFSYS_PLATFORM=win32
if exist %MKFSYS_DIR%\win64\auto_trigger.exe set MKFSYS_PLATFORM=win64
%MKFSYS_DIR%\%MKFSYS_PLATFORM%\auto_trigger.exe
goto End

:Error_InvalidMkfSysDir
echo Error : MKFSYS_DIR=[%MKFSYS_DIR%] is not found.
pause
goto End

:End
