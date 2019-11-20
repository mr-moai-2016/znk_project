@echo off

if exist mkf_triggers goto End_MoveToMkfTriggers
if exist ..\_impl cd ..
if exist ..\mkf_triggers cd ..
if not exist mkf_triggers goto Error_InvalidTrigger
:End_MoveToMkfTriggers

call mkf_triggers\_impl\_detect_bat_utils_dir.bat
if "%BAT_UTILS_DIR%" == ""   goto Error_InvalidBatUtilDir
if not exist %BAT_UTILS_DIR% goto Error_InvalidBatUtilDir

call mkf_triggers\_impl\_detect_mkfsys_dir.bat
if "%MKFSYS_DIR%" == ""   goto Error_InvalidMkfSysDir
if not exist %MKFSYS_DIR% goto Error_InvalidMkfSysDir

REM /F オプションをつけない場合、何も報告されない.
set _TR_CP_CMD=xcopy /H /C /F /Y
if "%1" == "-silent" set _TR_CP_CMD=xcopy /H /C /Q /Y

%_TR_CP_CMD% %MKFSYS_DIR%\mkf_triggers\*.bat mkf_triggers\
if not exist mkf_triggers\_impl mkdir mkf_triggers\_impl
%_TR_CP_CMD% %MKFSYS_DIR%\mkf_triggers\_impl\*.bat mkf_triggers\_impl\
%_TR_CP_CMD% %MKFSYS_DIR%\auto_trigger_run.bat .\

echo mkf_triggers syncronized.
goto End

:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.
pause
goto End

:Error_InvalidBatUtilDir
echo Error : BAT_UTILS_DIR=[%BAT_UTILS_DIR%] is not found.
pause
goto End

:Error_InvalidMkfSysDir
echo Error : MKFSYS_DIR=[%MKFSYS_DIR%] is not found.
pause
goto End

:End
pause
