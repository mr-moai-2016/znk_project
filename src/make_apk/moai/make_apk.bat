@echo off

REM
REM abi��ǉ��폜����ۂɏ���������t�@�C��
REM
REM make_apk.bat
REM app/build.gradle
REM app/jni/Application.mk
REM

call set_ver.bat
set ASSEMBLE_TYPE=Debug
set APK_TYPE=debug
REM set ASSEMBLE_TYPE=Release
REM set APK_TYPE=release-unsigned

set APK_BASENAME=moai-v%apk_ver%-android
set CP=xcopy /H /C /Y
set LIST=armeabi-v7a arm64-v8a x86
for %%a in ( %LIST% ) do rmdir /S /Q app\src\main\jniLibs\%%a\
for %%a in ( %LIST% ) do mkdir app\src\main\jniLibs\%%a
REM execute app\ndk_build_by_x86.bat, if libMoai, libRano and libZnk are updated.
for %%a in ( %LIST% ) do %CP% app\libs\%%a app\src\main\jniLibs\%%a

for %%a in ( %LIST% ) do mkdir app\src\main\assets\bin\%%a
REM easter
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v%parent_ver%-android-%%a\cgis\easter\*.cgi     app\src\main\assets\bin\%%a
REM custom_boy
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v%parent_ver%-android-%%a\cgis\custom_boy\*.cgi app\src\main\assets\bin\%%a
REM proxy_finder
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v%parent_ver%-android-%%a\cgis\proxy_finder\*.cgi app\src\main\assets\bin\%%a
REM cgi_developers
for %%a in ( %LIST% ) do mkdir app\src\main\assets\bin\%%a\cgi_developers\protected
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v%parent_ver%-android-%%a\cgis\cgi_developers\protected\*.cgi app\src\main\assets\bin\%%a\cgi_developers\protected

REM
REM plugin�ɂ��Ă� so �t�@�C���ł͂��邪 lib �v���t�B�N�X���Ȃ����߂�
REM apk ���� lib �ɓ���Ă����Ă� Android �� installd �������F�����Ȃ��͗l. 
REM ( /data/data/znkproject.moai/lib ���փR�s�[����Ȃ� )
REM ����Ă����� jniLibs �փR�s�[���Ă��_���ł���.
REM assets/bin �փR�s�[���A����ւ̃V���{���b�N�����N�𒣂邱�ƂőΏ�����.
REM
for %%a in ( %LIST% ) do mkdir app\src\main\assets\bin\%%a\plugins
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v%parent_ver%-android-%%a\plugins\*.so          app\src\main\assets\bin\%%a\plugins

call setenv.bat
call C:\share_7\AndroidSDK\gradle-2.14.1\bin\gradle.bat assemble%ASSEMBLE_TYPE% --stacktrace
%CP% app\build\outputs\apk\app-%APK_TYPE%.apk .\
if exist %APK_BASENAME%.apk del %APK_BASENAME%.apk
rename app-%APK_TYPE%.apk %APK_BASENAME%.apk
pause
