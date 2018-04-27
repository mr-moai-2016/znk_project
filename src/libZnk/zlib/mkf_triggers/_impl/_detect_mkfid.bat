@echo off

if not "%ZNK_VSCOMNTOOLS%" == "" goto Detect_vc
if not "%VS120COMNTOOLS%"  == "" goto Detect_vc
if not "%VS110COMNTOOLS%"  == "" goto Detect_vc
if not "%VS100COMNTOOLS%"  == "" goto Detect_vc
if not "%VS90COMNTOOLS%"   == "" goto Detect_vc
if not "%VS80COMNTOOLS%"   == "" goto Detect_vc
if not "%VS71COMNTOOLS%"   == "" goto Detect_vc
if not "%VS70COMNTOOLS%"   == "" goto Detect_vc
if not "%ZNK_VC60_DIR%"    == "" goto Detect_vc
if not "%ZNK_MINGW_DIR%"   == "" goto Detect_mingw
if not "%ZNK_NDK_DIR%"     == "" goto Detect_android

REM Default mingw

:Detect_mingw
set MKF_ID=mingw
goto OK

:Detect_vc
set MKF_ID=vc
goto OK

:Detect_android
set MKF_ID=android
goto OK

:OK
echo MKF_ID=[%MKF_ID%] is detected.
:End
