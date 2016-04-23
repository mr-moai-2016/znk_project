@echo off

set ROOT_DIR_=%1
if "%ROOT_DIR_%" == "" set ROOT_DIR_=..

set CP_=xcopy /H /C /Y
set INST_DIR_=%ROOT_DIR_%\bin_for_win32

if not exist %INST_DIR_% mkdir %INST_DIR_%

REM
REM copy libZnk dll
REM
if exist libZnk set SRC_DIR=libZnk
call bat_utils\get_dl_ver_sfx.bat bat_utils %SRC_DIR%\Makefile_version.mak
%CP_% %SRC_DIR%\out_dir\Znk%DL_VER_SFX%.dll %INST_DIR_%\

REM
REM setup moai
REM
%CP_% moai\out_dir\moai.exe %INST_DIR_%\
if not exist %INST_DIR_%\doc_root mkdir %INST_DIR_%\doc_root
%CP_% moai\doc_root\*.html %INST_DIR_%\doc_root\
%CP_% moai\doc_root\*.htm  %INST_DIR_%\doc_root\
%CP_% moai\doc_root\*.png  %INST_DIR_%\doc_root\
%CP_% moai\doc_root\*.jpg  %INST_DIR_%\doc_root\
%CP_% moai\doc_root\*.gif  %INST_DIR_%\doc_root\
%CP_% moai\doc_root\*.js   %INST_DIR_%\doc_root\
%CP_% moai\doc_root\*.css  %INST_DIR_%\doc_root\

REM
REM setup http_decorator
REM
%CP_% http_decorator\out_dir\http_decorator.exe %INST_DIR_%\

REM
REM setup virtual_users
REM
%CP_% virtual_users\out_dir\virtual_users.exe %INST_DIR_%\
call bat_utils\install_if_not_exist.bat user_agent.txt  virtual_users %INST_DIR_%
call bat_utils\install_if_not_exist.bat screen_size.txt virtual_users %INST_DIR_%
if not exist %INST_DIR_%\filters mkdir %INST_DIR_%\filters
call bat_utils\install_if_not_exist.bat futaba_recv.myf virtual_users\filters %INST_DIR_%\filters
call bat_utils\install_if_not_exist.bat futaba_send.myf virtual_users\filters %INST_DIR_%\filters
call bat_utils\install_if_not_exist.bat 2ch_recv.myf    virtual_users\filters %INST_DIR_%\filters
call bat_utils\install_if_not_exist.bat config.myf      virtual_users %INST_DIR_%
call bat_utils\install_if_not_exist.bat analysis.myf    virtual_users %INST_DIR_%
call bat_utils\install_if_not_exist.bat target.myf      virtual_users %INST_DIR_%
%CP_% virtual_users\README.txt      %INST_DIR_%\
%CP_% virtual_users\README_more.txt %INST_DIR_%\

REM
REM setup plugin_futaba
REM
if not exist %INST_DIR_%\plugins mkdir %INST_DIR_%\plugins
%CP_% plugin_futaba\out_dir\futaba.dll %INST_DIR_%\plugins\

echo All files installed to %INST_DIR_% successfully.
pause

