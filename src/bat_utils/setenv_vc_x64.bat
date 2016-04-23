@echo off

echo ===

REM ------
REM Automatic Recognize
REM
if not "%VS90COMNTOOLS%"  == "" goto VC90
if not "%VS80COMNTOOLS%"  == "" goto VC80
if not "%VS71COMNTOOLS%"  == "" goto VC71
if not "%VS100COMNTOOLS%" == "" goto VC100

REM ------
REM とりあえず現時点ではVC8.0のみのサポートとする.
REM

REM ------
REM Use Visual Studio 2005 Professional
REM
:VC80
call "%VS80COMNTOOLS%\..\..\VC\bin\amd64\vcvarsamd64.bat"
goto End

:VC71
:VC90
:VC100
echo Error : This version of VC is not supported in this script.
goto End


:End
if exist setenv_vc_additional.bat call setenv_vc_additional.bat
echo ===
echo INCLUDE=[%INCLUDE%]
echo LIB=[%LIB%]
echo PATH=[%PATH%]
echo ===
