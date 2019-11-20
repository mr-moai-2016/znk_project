@echo off

REM
REM binaryを除くすべてのassetsファイルをassetsフォルダ(ASSETS_DIR)へコピーする.
REM
set ASSETS_DIR=app\src\main\assets
set MY_LIBS_ROOT=..\..

if exist %ASSETS_DIR% rmdir /s /q %ASSETS_DIR%
mkdir %ASSETS_DIR%
mkdir %ASSETS_DIR%\bin
mkdir %ASSETS_DIR%\plugins

REM
REM moai
REM
call xcopy_begin.bat %MY_LIBS_ROOT%\moai %ASSETS_DIR%
set LIST=%LIST% analysis.myf
set LIST=%LIST% cert.pem
set LIST=%LIST% hosts.myf
set LIST=%LIST% moai_for_android.sh
set LIST=%LIST% target.myf
set LIST=%LIST% vtag
call xcopy_end.bat

call xcopy_begin.bat %MY_LIBS_ROOT%\moai\platform\android %ASSETS_DIR%
set LIST=%LIST% config.myf
set LIST=%LIST% config_cgi.myf
set LIST=%LIST% rano_app.myf
call xcopy_end.bat

set SUBDIRS=default
for %%a in ( %SUBDIRS% ) do call xcopy_dir.bat %MY_LIBS_ROOT%\moai\%%a %ASSETS_DIR%\%%a


REM
REM cgi_developers
REM
call xcopy_begin.bat %MY_LIBS_ROOT%\cgi_developers %ASSETS_DIR%\cgis\cgi_developers\publicbox
set LIST=%LIST% hello_template.html
set LIST=%LIST% post_go.go
set LIST=%LIST% progress.html
set LIST=%LIST% progress.js
set WILD=%WILD% %SRC%\*.c
call xcopy_end.bat

call xcopy_begin.bat %MY_LIBS_ROOT%\cgi_developers %ASSETS_DIR%\cgis\cgi_developers\protected
set LIST=%LIST% post_go.go
call xcopy_end.bat


REM
REM custom_boy
REM
call xcopy_begin.bat %MY_LIBS_ROOT%\custom_boy %ASSETS_DIR%\cgis\custom_boy
set LIST=%LIST% custom_boy.myf
call xcopy_end.bat

call xcopy_begin.bat %MY_LIBS_ROOT%\custom_boy\platform\android %ASSETS_DIR%\cgis\custom_boy
set LIST=%LIST% rano_app.myf
call xcopy_end.bat

set SUBDIRS=publicbox RE templates UA
for %%a in ( %SUBDIRS% ) do call xcopy_dir.bat %MY_LIBS_ROOT%\custom_boy\%%a %ASSETS_DIR%\cgis\custom_boy\%%a


REM
REM easter
REM
call xcopy_begin.bat %MY_LIBS_ROOT%\easter %ASSETS_DIR%\cgis\easter
set LIST=%LIST% core_behavior.myf
call xcopy_end.bat

call xcopy_begin.bat %MY_LIBS_ROOT%\easter\platform\android %ASSETS_DIR%\cgis\easter
set LIST=%LIST% rano_app.myf
call xcopy_end.bat

set SUBDIRS=default publicbox templates
for %%a in ( %SUBDIRS% ) do call xcopy_dir.bat %MY_LIBS_ROOT%\easter\%%a %ASSETS_DIR%\cgis\easter\%%a


REM
REM moai/doc_root
REM
call xcopy_begin.bat %MY_LIBS_ROOT%\moai\doc_root %ASSETS_DIR%\doc_root
set LIST=%LIST% announcement.html
set LIST=%LIST% bulma.css
set LIST=%LIST% FAQ.html
set LIST=%LIST% index.html
set LIST=%LIST% moai.png
set LIST=%LIST% msty.css
set LIST=%LIST% progress.js
call xcopy_end.bat

set SUBDIRS=common imgs moai2.0
for %%a in ( %SUBDIRS% ) do call xcopy_dir.bat %MY_LIBS_ROOT%\moai\doc_root\%%a %ASSETS_DIR%\doc_root\%%a

pause

