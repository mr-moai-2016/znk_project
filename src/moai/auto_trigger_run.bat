@echo off
call mkf_triggers\_impl\_detect_mkfsys_dir.bat
if "%MKFSYS_DIR%" == ""   goto Error_InvalidMkfSysDir
if not exist %MKFSYS_DIR% goto Error_InvalidMkfSysDir

%MKFSYS_DIR%\auto_trigger.exe
goto End

:Error_InvalidMkfSysDir
echo Error : MKFSYS_DIR=[%MKFSYS_DIR%] is not found.
pause
goto End

:End
