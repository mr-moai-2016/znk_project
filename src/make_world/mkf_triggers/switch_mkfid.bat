@echo off

if exist mkf_triggers goto Begin
if exist ..\mkf_triggers cd ..
if not exist mkf_triggers goto Error_InvalidTrigger
:Begin

REM
REM Phase 0 :
REM __MKF_ID_*__ file �܂��͊��ϐ�MKF_ID����̎����؂�ւ�.
REM vc => mingw => android ���z����.
REM
set CK_vc= 
set CK_mingw= 
set CK_android= 
if exist mkf_triggers\__MKF_ID_vc__      goto Check_mingw
if exist mkf_triggers\__MKF_ID_mingw__   goto Check_android
if exist mkf_triggers\__MKF_ID_android__ goto Check_vc
if "%MKF_ID%" == "vc"      goto Check_mingw
if "%MKF_ID%" == "mingw"   goto Check_android
if "%MKF_ID%" == "android" goto Check_vc
goto Check_mingw


REM
REM Phase 1 :
REM Makefile_*.mak �̑��݂��`�F�b�N���A���ꂪ���݂��Ȃ��ꍇ��
REM �z�̃`�F�[������X�L�b�v���Ď��̊��Ń`�F�b�N����.
REM �����z��h�����߁A��x�`�F�b�N�������̂ɂ��Ă�CK_* �ɋL�^��
REM ���ꂪyes�ɐݒ肳��Ă����ꍇ�̓`�F�b�N���ꏄ�����Ƃ݂Ȃ���
REM ����������I�ɍ̗p����.
REM
:Check_vc
if "%CK_vc%" == "yes" goto Setto_vc
set CK_vc=yes
if not exist Makefile_vc.mak goto Check_mingw
goto Setto_vc

:Check_mingw
if "%CK_mingw%" == "yes" goto Setto_mingw
set CK_mingw=yes
if not exist Makefile_mingw.mak goto Check_android
goto Setto_mingw

:Check_android
if "%CK_android%" == "yes" goto Setto_android
set CK_android=yes
if not exist Makefile_android.mak goto Check_vc
goto Setto_android


REM
REM Phase 2 :
REM ���ۂ�__MKF_ID_*__ �t�@�C�����X�V.
REM �܂�machine�^�C�v�̓f�t�H���g�Ƀ��Z�b�g���Ă���.
REM
:Setto_vc
	set MKF_ID=vc
	if exist mkf_triggers\__MKF_ID_*__ del mkf_triggers\__MKF_ID_*__
	echo vc>mkf_triggers\__MKF_ID_vc__
	call mkf_triggers\switch_machine.bat x86
	goto End

:Setto_mingw
	set MKF_ID=mingw
	if exist mkf_triggers\__MKF_ID_*__ del mkf_triggers\__MKF_ID_*__
	echo mingw>mkf_triggers\__MKF_ID_mingw__
	call mkf_triggers\switch_machine.bat x86
	goto End

:Setto_android
	set MKF_ID=android
	if exist mkf_triggers\__MKF_ID_*__ del mkf_triggers\__MKF_ID_*__
	echo android>mkf_triggers\__MKF_ID_android__
	call mkf_triggers\switch_machine.bat armeabi-v7a
	goto End


:Error_InvalidTrigger
echo Error : mkf_triggers dir is not found from here.
pause

:End
