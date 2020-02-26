@echo off
call set_ver.bat
C:\share_7\AndroidSDK\adb_ok\platform-tools\adb.exe install -r moai-v%apk_ver%-android.apk
REM C:\share_7\AndroidSDK\adb_ok\platform-tools\adb.exe install -r moai-v2.2.3-prototype4-android.apk
pause
