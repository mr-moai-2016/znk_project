@echo off

set CP=xcopy /H /C /Y
set SRC_DIR=..\src\moai
set DST_DIR=..\moai-v2.1-win64

mkdir %DST_DIR%\doc_root\source_doc

if exist %SRC_DIR%\doc_root\*.html            %CP% /F %SRC_DIR%\doc_root\*.html            %DST_DIR%\doc_root\
if exist %SRC_DIR%\doc_root\*.css             %CP% /F %SRC_DIR%\doc_root\*.css             %DST_DIR%\doc_root\
if exist %SRC_DIR%\doc_root\*.png             %CP% /F %SRC_DIR%\doc_root\*.png             %DST_DIR%\doc_root\
if exist %SRC_DIR%\doc_root\*.jpg             %CP% /F %SRC_DIR%\doc_root\*.jpg             %DST_DIR%\doc_root\
if exist %SRC_DIR%\doc_root\*.gif             %CP% /F %SRC_DIR%\doc_root\*.gif             %DST_DIR%\doc_root\
if exist %SRC_DIR%\doc_root\*.js              %CP% /F %SRC_DIR%\doc_root\*.js              %DST_DIR%\doc_root\
if exist %SRC_DIR%\doc_root\common\*.html     %CP% /F %SRC_DIR%\doc_root\common\*.html     %DST_DIR%\doc_root\common\
if exist %SRC_DIR%\doc_root\moai2.0\*.html    %CP% /F %SRC_DIR%\doc_root\moai2.0\*.html    %DST_DIR%\doc_root\moai2.0\
if exist %SRC_DIR%\doc_root\imgs\*.png        %CP% /F %SRC_DIR%\doc_root\imgs\*.png        %DST_DIR%\doc_root\imgs\
if exist %SRC_DIR%\doc_root\imgs\*.jpg        %CP% /F %SRC_DIR%\doc_root\imgs\*.jpg        %DST_DIR%\doc_root\imgs\
if exist %SRC_DIR%\doc_root\imgs\*.gif        %CP% /F %SRC_DIR%\doc_root\imgs\*.gif        %DST_DIR%\doc_root\imgs\
if exist %SRC_DIR%\doc_root\public\moai.png   %CP% /F %SRC_DIR%\doc_root\public\moai.png   %DST_DIR%\doc_root\public\
if exist %SRC_DIR%\doc_root\source_doc\*.html %CP% /F %SRC_DIR%\doc_root\source_doc\*.html %DST_DIR%\doc_root\source_doc\

