@echo off

if exist set_ver.bat call set_ver.bat

set CP=xcopy /H /C /Y
if exist moai-common\birdman\birdman.myf %CP% moai-common\birdman\birdman.myf .\

moai-win32\birdman\birdman.exe install win32 %old_parent_ver% %new_parent_ver%
moai-win32\birdman\birdman.exe install win64 %old_parent_ver% %new_parent_ver%

pause
