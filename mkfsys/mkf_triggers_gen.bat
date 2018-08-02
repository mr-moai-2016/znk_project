@echo off
if "%1" == ""   goto Error_arg1
if not exist %1 goto Error_arg1
if exist %1\__this_is_mkfsys_dir__ goto Error_arg1

echo Now we make mkf_triggers in here...
echo ( Here is [%1] )
pause

REM /F オプションをつけない場合、何も報告されない.
set _TR_CP_CMD=xcopy /H /C /Y

REM Getting current directory path
REM set _BUTM_VAR0_=set MKFSYS_DIR
REM type template_bat__var0>  __temp_bb_cmd.bat
REM type template_bat__equal>>__temp_bb_cmd.bat
REM cd>> __temp_bb_cmd.bat
REM call __temp_bb_cmd.bat
REM del  __temp_bb_cmd.bat
set MKFSYS_DIR=%CD%
if not exist %MKFSYS_DIR% goto Error_InvalidMkfSysDir

cd /d %1
if not exist mkf_triggers mkdir mkf_triggers

%_TR_CP_CMD% %MKFSYS_DIR%\mkf_triggers\*.bat mkf_triggers\
if not exist mkf_triggers\_impl mkdir mkf_triggers\_impl
%_TR_CP_CMD% %MKFSYS_DIR%\mkf_triggers\_impl\*.bat mkf_triggers\_impl\
%_TR_CP_CMD% %MKFSYS_DIR%\auto_trigger_run.bat .\

echo:
echo mkf_triggers is automatically generated in here.
pause
goto End

:Error_arg1
echo Error : arg1=[%1] is not invalid directory.
pause
goto End

:Error_InvalidMkfSysDir
echo Error : MKFSYS_DIR=[%MKFSYS_DIR%] is not found.
pause
goto End

:End
