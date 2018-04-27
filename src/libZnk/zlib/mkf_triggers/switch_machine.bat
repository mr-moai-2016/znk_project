@echo off

if exist mkf_triggers goto Begin
if exist ..\mkf_triggers cd ..
if not exist mkf_triggers goto Error_InvalidTrigger
:Begin


REM
REM Phase 0 : If first argument is given
REM
if "%1" == "x86"         goto x86
if "%1" == "x64"         goto x64
if "%1" == "armeabi"     goto armeabi
if "%1" == "armeabi-v7a" goto armeabi_v7a


REM
REM Phase 1 : Automatically decide machine type by __MKF_ID_*__ file.
REM
set MKF_ID=mingw
if exist mkf_triggers\__MKF_ID_mingw__   set MKF_ID=mingw
if exist mkf_triggers\__MKF_ID_vc__      set MKF_ID=vc
if exist mkf_triggers\__MKF_ID_android__ set MKF_ID=android

if "%MKF_ID%" == "vc"      goto For_PC
if "%MKF_ID%" == "mingw"   goto For_PC
if "%MKF_ID%" == "android" goto For_Android

:For_PC
	if exist mkf_triggers\__MACHINE_x86__    goto x64
	if exist mkf_triggers\__MACHINE_x64__    goto x86
	REM default
	if "%PROCESSOR_ARCHITECTURE%" == "AMD64" goto x64
	goto x86
:For_Android
	if exist mkf_triggers\__MACHINE_x86__         goto armeabi
	if exist mkf_triggers\__MACHINE_x64__         goto x86
	if exist mkf_triggers\__MACHINE_armeabi__     goto armeabi_v7a
	if exist mkf_triggers\__MACHINE_armeabi-v7a__ goto x86
	REM default
	goto armeabi_v7a


REM
REM Phase 2 : Delete old __MACHINE_*__ file and output new.
REM
:x86
	if exist mkf_triggers\__MACHINE_*__ del mkf_triggers\__MACHINE_*__
	echo x86>mkf_triggers\__MACHINE_x86__
goto End

:x64
	if exist mkf_triggers\__MACHINE_*__ del mkf_triggers\__MACHINE_*__
	echo x64>mkf_triggers\__MACHINE_x64__
goto End

:armeabi
	if exist mkf_triggers\__MACHINE_*__ del mkf_triggers\__MACHINE_*__
	echo armeabi>mkf_triggers\__MACHINE_armeabi__
goto End

:armeabi_v7a
	if exist mkf_triggers\__MACHINE_*__ del mkf_triggers\__MACHINE_*__
	echo armeabi-v7a>mkf_triggers\__MACHINE_armeabi-v7a__
goto End


:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.
pause

:End
