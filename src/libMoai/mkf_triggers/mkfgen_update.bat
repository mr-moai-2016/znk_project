@echo off

if exist mkf_triggers goto MkfTriggersFound
if exist ..\mkf_triggers cd ..
if exist mkf_triggers goto MkfTriggersFound
goto Error_InvalidTrigger
:MkfTriggersFound

call mkf_triggers\_impl\_detect_mkfsys_dir.bat
if "%MKFSYS_DIR%" == ""   goto Error_InvalidMkfSysDir
if not exist %MKFSYS_DIR% goto Error_InvalidMkfSysDir

if exist %MKFSYS_DIR%\win32\mkfgen.exe set MKFSYS_PLATFORM=win32
if exist %MKFSYS_DIR%\win64\mkfgen.exe set MKFSYS_PLATFORM=win64
%MKFSYS_DIR%\%MKFSYS_PLATFORM%\mkfgen.exe
goto End

:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.
goto End

:Error_InvalidMkfSysDir
echo Error : MKFSYS_DIR=[%MKFSYS_DIR%] is not found.
goto End

:End
pause
