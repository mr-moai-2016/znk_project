@echo off

if "%MKF_ID%" == "vc"      goto For_PC
if "%MKF_ID%" == "mingw"   goto For_PC
if "%MKF_ID%" == "android" goto For_Android

:For_PC
	if exist mkf_triggers\__MACHINE_x86__    set MACHINE=x86
	if exist mkf_triggers\__MACHINE_x64__    set MACHINE=x64
	if not "%MACHINE%" == "" goto End
	REM Default.
	if "%PROCESSOR_ARCHITECTURE%" == "AMD64" set MACHINE=x64
	if not "%MACHINE%" == "" goto End
	set MACHINE=x86
	goto End

:For_Android
	if exist mkf_triggers\__MACHINE_x86__         set MACHINE=x86
	if exist mkf_triggers\__MACHINE_x64__         set MACHINE=x64
	if exist mkf_triggers\__MACHINE_armeabi__     set MACHINE=armeabi
	if exist mkf_triggers\__MACHINE_armeabi-v7a__ set MACHINE=armeabi-v7a
	if not "%MACHINE%" == "" goto End
	REM Default.
	set MACHINE=armeabi
	goto End

:End
