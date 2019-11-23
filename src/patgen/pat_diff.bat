@echo off
set old_parent_ver=2.1
set old_ver=2.1
set new_parent_ver=2.2
set new_ver=2.2

set CP=xcopy /H /C /Y
if exist set_ver.bat call set_ver.bat

win64\patgen diff ../releases_dir/v%old_ver%/moai-v%old_parent_ver%-win32               ../moai-v%new_parent_ver%-win32               > diff_win32.pmk
win64\patgen diff ../releases_dir/v%old_ver%/moai-v%old_parent_ver%-win64               ../moai-v%new_parent_ver%-win64               > diff_win64.pmk
win64\patgen diff ../releases_dir/v%old_ver%/moai-v%old_parent_ver%-android-armeabi     ../moai-v%new_parent_ver%-android-armeabi     > diff_android-armeabi.pmk
REM win64\patgen diff ../releases_dir/v%old_ver%/moai-v%old_parent_ver%-android-armeabi-v7a ../moai-v%new_parent_ver%-android-armeabi-v7a > diff_android-armeabi-v7a.pmk
win64\patgen diff ../releases_dir/v%old_ver%/moai-v%old_parent_ver%-android-x86         ../moai-v%new_parent_ver%-android-x86         > diff_android-x86.pmk

if exist ..\moai-v%new_parent_ver%-win32\birdman\birdman.myf %CP% ..\moai-v%new_parent_ver%-win32\birdman\birdman.myf apply_this_scripts\
if exist ..\moai-v%new_parent_ver%-win64\birdman\birdman.myf %CP% ..\moai-v%new_parent_ver%-win64\birdman\birdman.myf apply_this_scripts\

pause
