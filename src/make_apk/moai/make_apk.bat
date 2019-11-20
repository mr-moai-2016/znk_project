@echo off

REM
REM abiÇí«â¡çÌèúÇ∑ÇÈç€Ç…èëÇ´ä∑Ç¶ÇÈÉtÉ@ÉCÉã
REM
REM make_apk.bat
REM app/build.gradle
REM app/jni/Application.mk
REM

set ASSEMBLE_TYPE=Debug
set APK_TYPE=debug
REM set ASSEMBLE_TYPE=Release
REM set APK_TYPE=release-unsigned

set APK_BASENAME=moai-v2.2-android
set CP=xcopy /H /C /Y
set LIST=armeabi-v7a x86
for %%a in ( %LIST% ) do rmdir /S /Q app\src\main\jniLibs\%%a\
for %%a in ( %LIST% ) do mkdir app\src\main\jniLibs\%%a
REM execute app\ndk_build_by_x86.bat, if libMoai, libRano and libZnk are updated.
for %%a in ( %LIST% ) do %CP% app\libs\%%a app\src\main\jniLibs\%%a

for %%a in ( %LIST% ) do mkdir app\src\main\assets\bin\%%a
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v2.2-android-%%a\cgis\easter\*.cgi     app\src\main\assets\bin\%%a
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v2.2-android-%%a\cgis\custom_boy\*.cgi app\src\main\assets\bin\%%a
for %%a in ( %LIST% ) do mkdir app\src\main\assets\bin\%%a\cgi_developers\protected
for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v2.2-android-%%a\cgis\cgi_developers\protected\*.cgi app\src\main\assets\bin\%%a\cgi_developers\protected

for %%a in ( %LIST% ) do %CP% ..\..\..\moai-v2.2-android-%%a\plugins\*.so          app\src\main\jniLibs\%%a

call setenv.bat
call C:\share_7\AndroidSDK\gradle-2.14.1\bin\gradle.bat assemble%ASSEMBLE_TYPE% --stacktrace
%CP% app\build\outputs\apk\app-%APK_TYPE%.apk .\
if exist %APK_BASENAME%.apk del %APK_BASENAME%.apk
rename app-%APK_TYPE%.apk %APK_BASENAME%.apk
pause
