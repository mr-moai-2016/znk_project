@echo off
echo ===

if "%MACHINE%" == "x64" goto For_x64

REM ------
REM Automatic Recognize
REM
REM VC141 : VC Ver14.1( Visual Studio 2017 )からはVS*COMNTOOLS 環境変数は廃止されたようである.
REM VC140 : Visual Studio 2015
REM VC130 : VC Ver13は(多分不吉だとかそういう理由で)存在しない. 従ってVS130COMNTOOLS は存在しない.
REM VC120 : Visual Studio 2013
REM VC110 : Visual Studio 2012
REM VC100 : Visual Studio 2010
REM VC90  : Visual Studio 2008
REM VC80  : Visual Studio 2005
REM VC71  : Visual Studio 2003
REM VC70  : Visual Studio 2002
REM
if not "%ZNK_VC_SETENV32_CMD%" == "" goto x86_ExecCmd
if not "%ZNK_VSCOMNTOOLS%" == "" goto x86_Unknown
if not "%VS140COMNTOOLS%"  == "" goto x86_VC140
if not "%VS120COMNTOOLS%"  == "" goto x86_VC120
if not "%VS110COMNTOOLS%"  == "" goto x86_VC110
if not "%VS100COMNTOOLS%"  == "" goto x86_VC100
if not "%VS90COMNTOOLS%"   == "" goto x86_VC90
if not "%VS80COMNTOOLS%"   == "" goto x86_VC80
if not "%VS71COMNTOOLS%"   == "" goto x86_VC71
if not "%VS70COMNTOOLS%"   == "" goto x86_VC70
if not "%ZNK_VC60_DIR%"    == "" goto x86_VC60

echo Error on setenv_vc.bat:
echo Can't find environment variable ZNK_VSCOMNTOOLS.
echo This script recognizes VC by this environment variables set.
echo You must set environment variable ZNK_VSCOMNTOOLS to VC install directory.
pause
exit

:x86_VC140
set ZNK_VSCOMNTOOLS=%VS140COMNTOOLS%
goto x86_Unknown

:x86_VC120
set ZNK_VSCOMNTOOLS=%VS120COMNTOOLS%
goto x86_Unknown

:x86_VC110
set ZNK_VSCOMNTOOLS=%VS110COMNTOOLS%
goto x86_Unknown

:x86_VC100
set ZNK_VSCOMNTOOLS=%VS100COMNTOOLS%
goto x86_Unknown

:x86_VC90
set ZNK_VSCOMNTOOLS=%VS90COMNTOOLS%
goto x86_Unknown

:x86_VC80
set ZNK_VSCOMNTOOLS=%VS80COMNTOOLS%
goto x86_Unknown

REM ------
REM Unknown VC Version.
REM 最終的手段としてはZNK_VSCOMNTOOLSで、以下のbatファイルのあるパスを直接与える.
REM
:x86_Unknown
if exist "%ZNK_VSCOMNTOOLS%\vsvars32.bat" goto x86_vsvars32
if exist "%ZNK_VSCOMNTOOLS%\VsDevCmd.bat" goto x86_VsDevCmd
echo Error : Missing vsvars32.bat, VsDevCmd.bat.
goto End

:x86_vsvars32
set ZNK_VC_SETENV32_CMD="%ZNK_VSCOMNTOOLS%\vsvars32.bat"
goto x86_ExecCmd

:x86_VsDevCmd
set VSCMD_START_DIR=%CD%
set ZNK_VC_SETENV32_CMD="%ZNK_VSCOMNTOOLS%\VsDevCmd.bat" -no_ext
goto x86_ExecCmd

:x86_VC60
set ZNK_VC_SETENV32_CMD="%ZNK_VC60_DIR%\VC98\Bin\VCVARS32.BAT"
goto x86_ExecCmd

:x86_ExecCmd
echo Now call %ZNK_VC_SETENV32_CMD%
call %ZNK_VC_SETENV32_CMD%
goto End


:For_x64

REM ------
REM Automatic Recognize
REM
if not "%ZNK_VC_SETENV64_CMD%" == "" goto x64_ExecCmd
if not "%ZNK_VSCOMNTOOLS%" == "" goto x64_Unknown
if not "%VS140COMNTOOLS%"  == "" goto x64_VC140
if not "%VS120COMNTOOLS%"  == "" goto x64_VC120
if not "%VS110COMNTOOLS%"  == "" goto x64_VC110
if not "%VS100COMNTOOLS%"  == "" goto x64_VC100
if not "%VS90COMNTOOLS%"   == "" goto x64_VC90
if not "%VS80COMNTOOLS%"   == "" goto x64_VC80

echo Error : This version of VC is not supported to x64 in this script.
goto End

:x64_VC140
set ZNK_VSCOMNTOOLS=%VS140COMNTOOLS%
goto x64_Unknown

:x64_VC120
set ZNK_VSCOMNTOOLS=%VS120COMNTOOLS%
goto x64_Unknown

:x64_VC110
set ZNK_VSCOMNTOOLS=%VS110COMNTOOLS%
goto x64_Unknown

:x64_VC100
set ZNK_VSCOMNTOOLS=%VS100COMNTOOLS%
goto x64_Unknown

:x64_VC90
set ZNK_VSCOMNTOOLS=%VS90COMNTOOLS%
goto x64_Unknown

:x64_VC80
set ZNK_VSCOMNTOOLS=%VS80COMNTOOLS%
goto x64_Unknown

REM ------
REM Unknown VC Version.
REM 最終的手段としてはZNK_VSCOMNTOOLSで、以下のbatファイルのあるパスを直接与える.
REM VsDevCmd.batはVS2017以上
REM
:x64_Unknown
if exist "%ZNK_VSCOMNTOOLS%\..\..\VC\bin\vcvars64.bat"          goto x64_vcbin_vcvars64
if exist "%ZNK_VSCOMNTOOLS%\..\..\VC\bin\amd64\vcvarsamd64.bat" goto x64_vcbin_amd64_vcvarsamd64
if exist "%ZNK_VSCOMNTOOLS%\vcvars64.bat"                       goto x64_vcvars64
if exist "%ZNK_VSCOMNTOOLS%\vcvarsamd64.bat"                    goto x64_vcvarsamd64
if exist "%ZNK_VSCOMNTOOLS%\VsDevCmd.bat"                       goto x64_VsDevCmd
echo Error : Missing vcvars64.bat, vcvarsamd64.bat, VsDevCmd.bat
goto End

:x64_vcbin_vcvars64
set ZNK_VC_SETENV64_CMD="%ZNK_VSCOMNTOOLS%\..\..\VC\bin\vcvars64.bat"
goto x64_ExecCmd

:x64_vcbin_amd64_vcvarsamd64
set ZNK_VC_SETENV64_CMD="%ZNK_VSCOMNTOOLS%\..\..\VC\bin\amd64\vcvarsamd64.bat"
goto x64_ExecCmd

:x64_vcvars64
set ZNK_VC_SETENV64_CMD="%ZNK_VSCOMNTOOLS%\vcvars64.bat"
goto x64_ExecCmd

:x64_vcvarsamd64
set ZNK_VC_SETENV64_CMD="%ZNK_VSCOMNTOOLS%\vcvarsamd64.bat"
goto x64_ExecCmd

:x64_VsDevCmd
set VSCMD_START_DIR=%CD%
set ZNK_VC_SETENV64_CMD="%ZNK_VSCOMNTOOLS%\VsDevCmd.bat" -no_ext -arch=amd64
goto x64_ExecCmd

:x64_ExecCmd
echo Now call %ZNK_VC_SETENV64_CMD%
call %ZNK_VC_SETENV64_CMD%
goto End

:End
if exist setenv_vc_additional.bat call setenv_vc_additional.bat
echo ===
echo INCLUDE=[%INCLUDE%]
echo LIB=[%LIB%]
echo PATH=[%PATH%]
echo ===
